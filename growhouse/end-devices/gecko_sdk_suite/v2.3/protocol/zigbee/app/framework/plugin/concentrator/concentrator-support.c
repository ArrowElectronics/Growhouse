// *****************************************************************************
// * concentrator-support.c
// *
// * Code common to SOC and host to handle periodically broadcasting
// * many-to-one route requests (MTORRs).
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "stack/include/zigbee-device-stack.h"
#include "source-route-common.h"
#include "concentrator-support.h"
// *****************************************************************************
// Globals

static uint8_t routeErrorCount = 0;
static uint8_t deliveryFailureCount = 0;

#define MIN_QS (EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS << 2)
#define MAX_QS (EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS << 2)

#if (MIN_QS > MAX_QS)
  #error "Minimum broadcast time must be less than max (EMBER_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS < EMBER_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS)"
#endif

// Handy values to make the code more readable.

typedef enum {
  USE_MIN_TIME = 0,
  USE_MAX_TIME = 1,
  USE_ADDRESS_DISCOVERY_TIME = 2,
} QueueDelay;

static EmberNodeId nodeIdWithBrokenRoute     = EMBER_UNKNOWN_NODE_ID;
static bool sendAddressDiscoveryNextEvent = false;

EmberEventControl emberAfPluginConcentratorUpdateEventControl;

// Use a shorter name to make the code more readable
#define myEvent emberAfPluginConcentratorUpdateEventControl

#ifndef EMBER_AF_HAS_ROUTER_NETWORK
  #error "Concentrator support only allowed on routers and coordinators."
#endif

//This is used to store the sourceRouteOverhead to our last sender
//It defaults to 0xFF if no valid sourceRoute is found. When available, it
//is used once to prevent the overhead of calling ezspGetSourceRouteOverhead()
//and cleared subsequently.
#if defined(EZSP_HOST) && defined(EMBER_AF_PLUGIN_CONCENTRATOR_NCP_SUPPORT)
static EmberNodeId targetId          = EMBER_UNKNOWN_NODE_ID;
static uint8_t sourceRouteOverhead     = EZSP_SOURCE_ROUTE_OVERHEAD_UNKNOWN;
#endif

// EMINSIGHT-2484 - allow applications to set whether or not they want
// routers to send mtorrs.
EmberAfPluginConcentratorRouterBehavior emAfPluginConcentratorRouterBehavior
  = EMBER_AF_PLUGIN_CONCENTRATOR_DEFAULT_ROUTER_BEHAVIOR;

// *****************************************************************************
// Functions

static uint32_t queueRouteDiscovery(QueueDelay time)
{
  uint32_t timeLeftQS = MIN_QS;  //(useMinMaxAddDiscoveryTime == USE_MIN_TIME) ? MIN_QS : MAX_QS;
  if (time == USE_MAX_TIME) {
    timeLeftQS = MAX_QS;
  } else if (time == USE_ADDRESS_DISCOVERY_TIME) {
    timeLeftQS = ADDRESS_DISCOVERY_DELAY_QS;
  }

  // NOTE:  Since timeToExecute is always in MS we must convert MIN_QS => MIN_MS
  if ( time != USE_MAX_TIME
       && (myEvent.timeToExecute > 0)
       && (myEvent.timeToExecute < (timeLeftQS * 250))) {
    // Do nothing.  Our queued event will fire shortly.
    // We don't want to reset its time and actually delay
    // when it will fire.
    timeLeftQS = myEvent.timeToExecute >> 2;
  } else {
    emberEventControlSetDelayQS(myEvent,
                                timeLeftQS);
  }

  // Tell the caller we have approximately 1 quarter second left
  // even though we actually have less than that.  This lets them plan their
  // for events that are waiting to fire based on the MTORR.
  return (timeLeftQS > 0
          ? timeLeftQS
          : 1);
}

uint32_t emberAfPluginConcentratorQueueDiscovery(void)
{
  return queueRouteDiscovery(USE_MIN_TIME);
}

void emberAfPluginConcentratorInitCallback(void)
{
  sourceRouteInit();
}

void emberAfPluginConcentratorNcpInitCallback(void)
{
}

void emberAfPluginConcentratorStopDiscovery(void)
{
  emberEventControlSetInactive(myEvent);
  myEvent.timeToExecute = 0;
  emberAfCorePrintln("Concentrator advertisements stopped.");
}

// TODO: we should not need this anymore, but needs to be confirmed in regression testing
/*
   static void routeErrorCallback(EmberStatus status)
   {
   if (status == EMBER_SOURCE_ROUTE_FAILURE
 || status == EMBER_MANY_TO_ONE_ROUTE_FAILURE) {
    routeErrorCount += 1;
    if (routeErrorCount >= EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD) {
      emberAfPluginConcentratorQueueDiscovery();
    }
   }
   }
 */
void emberAfPluginConcentratorMessageSentCallback(EmberOutgoingMessageType type,
                                                  uint16_t indexOrDestination,
                                                  EmberApsFrame *apsFrame,
                                                  EmberStatus status,
                                                  uint16_t messageLength,
                                                  uint8_t *messageContents)
{
  if ((type == EMBER_OUTGOING_DIRECT
       || type == EMBER_OUTGOING_VIA_ADDRESS_TABLE
       || type == EMBER_OUTGOING_VIA_BINDING)
      && status == EMBER_DELIVERY_FAILED) {
    deliveryFailureCount += 1;
    if (deliveryFailureCount >= EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD) {
      emberAfPluginConcentratorQueueDiscovery();
    }
  }
}

// We only store one valid overhead for one destination. We don't want to overwrite that with
// an invalid source route to another destination. We do however want to invalidate an
// overhead to our destination if it is now unknown.
void emberAfSetSourceRouteOverheadCallback(EmberNodeId destination, uint8_t overhead)
{
  #if defined(EZSP_HOST) && defined(EMBER_AF_PLUGIN_CONCENTRATOR_NCP_SUPPORT)
  if (!(destination != targetId && overhead == EZSP_SOURCE_ROUTE_OVERHEAD_UNKNOWN)) {
    targetId = destination;
    sourceRouteOverhead = overhead;
  }
  #endif
}

// In an effort to reduce the traffic between the host and NCP, for each incoming message,
// the sourceRouteOverhead to that particular destination is sent from the NCP to the host
// as a part of the incomingMessageHandler(). This information is cached and can be used
// once to calculate the MaximumPayload() to that same destination. It is invalidated after
// one use.
uint8_t emberAfGetSourceRouteOverheadCallback(EmberNodeId destination)
{
#if defined(EZSP_HOST)
  #if defined(EMBER_AF_PLUGIN_CONCENTRATOR_NCP_SUPPORT)
  if (targetId == destination && sourceRouteOverhead != EZSP_SOURCE_ROUTE_OVERHEAD_UNKNOWN) {
    emberAfDebugPrintln("ValidSourceRouteFound %u ", sourceRouteOverhead);
    return sourceRouteOverhead;
  } else {
    return ezspGetSourceRouteOverhead(destination);
  }
  #else
  return ezspGetSourceRouteOverhead(destination);
  #endif
#else
  return emberGetSourceRouteOverhead(destination);
#endif
}

void emberAfPluginConcentratorUpdateEventHandler(void)
{
  //if we are not a concentrator but somehow get here
  if (EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE != EMBER_LOW_RAM_CONCENTRATOR
      && EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE != EMBER_HIGH_RAM_CONCENTRATOR) {
    return;
  }

  EmberNodeType nodeType;

  routeErrorCount = 0;
  deliveryFailureCount = 0;

  if (sendAddressDiscoveryNextEvent) {
    emberIeeeAddressRequestToTarget(nodeIdWithBrokenRoute,
                                    false,   // report kids?
                                    0,       // child start index
                                    EMBER_APS_OPTION_NONE,
                                    EMBER_BROADCAST_ADDRESS);
    sendAddressDiscoveryNextEvent = false;
    nodeIdWithBrokenRoute = EMBER_UNKNOWN_NODE_ID;
  } else if ((emAfPluginConcentratorRouterBehavior == FULL
              || (emberAfGetNodeType(&nodeType) == EMBER_SUCCESS
                  && nodeType == EMBER_COORDINATOR))
             && (emberSendManyToOneRouteRequest(EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE,
                                                EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS)
                 == EMBER_SUCCESS)) {
    emberAfDebugPrintln("send MTORR");
    emberAfPluginConcentratorBroadcastSentCallback();
    if (nodeIdWithBrokenRoute != EMBER_UNKNOWN_NODE_ID) {
      sendAddressDiscoveryNextEvent = true;
      queueRouteDiscovery(USE_ADDRESS_DISCOVERY_TIME);
      return;
    }
  }
  queueRouteDiscovery(USE_MAX_TIME);
}

static void concentratorSupportRouteErrorHandler(EmberStatus status,
                                                 EmberNodeId nodeId)
{
  if (status == EMBER_SOURCE_ROUTE_FAILURE
      || status == EMBER_MANY_TO_ONE_ROUTE_FAILURE) {
    routeErrorCount += 1;
    if (routeErrorCount >= EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD) {
      if (nodeIdWithBrokenRoute == EMBER_UNKNOWN_NODE_ID) {
        nodeIdWithBrokenRoute = nodeId;
        sendAddressDiscoveryNextEvent = false;
      }
      queueRouteDiscovery(USE_MIN_TIME);
    }
  }
}

void emberIncomingRouteErrorHandler(EmberStatus status, EmberNodeId target)
{
  //routeErrorCallback(status);
  concentratorSupportRouteErrorHandler(status, target);
}

void ezspIncomingRouteErrorHandler(EmberStatus status, EmberNodeId target)
{
  // the same functionality is going to be also triggered from the em260/concentrator code
  // routeErrorCallback(status);
}

void emberAfPluginConcentratorStackStatusCallback(EmberStatus status)
{
  if (status == EMBER_NETWORK_DOWN
      && !emberStackIsPerformingRejoin()) {
    emberAfCorePrintln("Clearing source route table.");
    sourceRouteClearTable();
  } else if (status == EMBER_NETWORK_UP) {
#if (!defined(EZSP_HOST) || !defined(EMBER_AF_PLUGIN_CONCENTRATOR_NCP_SUPPORT))
    queueRouteDiscovery(USE_MAX_TIME);
#endif
#if defined(EZSP_HOST) && defined(EMBER_AF_PLUGIN_CONCENTRATOR_NCP_SUPPORT)
    ezspSetConcentrator(true,
                        EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE,
                        EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS,
                        EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS,
                        EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD,
                        EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD,
                        EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS);
#elif EZSP_HOST
    ezspSetConcentrator(false,
                        EMBER_AF_PLUGIN_CONCENTRATOR_CONCENTRATOR_TYPE,
                        EMBER_AF_PLUGIN_CONCENTRATOR_MIN_TIME_BETWEEN_BROADCASTS_SECONDS,
                        EMBER_AF_PLUGIN_CONCENTRATOR_MAX_TIME_BETWEEN_BROADCASTS_SECONDS,
                        EMBER_AF_PLUGIN_CONCENTRATOR_ROUTE_ERROR_THRESHOLD,
                        EMBER_AF_PLUGIN_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD,
                        EMBER_AF_PLUGIN_CONCENTRATOR_MAX_HOPS);
#endif
  }
}

//------------------------------------------------------------------------------
// These functions are defined for legacy support for the software using the old
// app/util/concentrator/ names.
// I would really like to #define the old function names to the new ones,
// but creating a real function with the same name will cause a duplicate symbol
// error to occur if both this plugin and the old code are included.  Users can then
// remove the app/util/concentrator code from their project to prevent confusion
// and redundancy.
uint32_t emberConcentratorQueueRouteDiscovery(void)
{
  return emberAfPluginConcentratorQueueDiscovery();
}

void emberConcentratorStartDiscovery(void)
{
  emberAfPluginConcentratorInitCallback();
}

void emberConcentratorStopDiscovery(void)
{
  emberAfPluginConcentratorStopDiscovery();
}

//------------------------------------------------------------------------------
