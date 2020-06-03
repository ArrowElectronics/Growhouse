// File: concentrator.c
//
// The concentrator library is deprecated and will be removed in a future
// release.  Similar functionality is available in the Concentrator Support
// plugin in Application Framework.
//
// Copyright 2008 by Ember Corporation.  All rights reserved.               *80*

#include PLATFORM_HEADER
#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
  #include "stack/include/error.h"
  #include "stack/include/ember-types.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"
  #include "app/util/ezsp/serial-interface.h"
#else
// Includes needed for ember related functions for the EM250
  #include "stack/include/ember.h"
#endif // EZSP_HOST
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"              // for halCommonGetInt16uQuarterSecondTick()
#include "concentrator.h"

static uint8_t routeErrorCount;
static uint8_t deliveryFailureCount;

#define MIN_QS (EMBER_CONCENTRATOR_MIN_SECONDS_BETWEEN_DISCOVERIES << 2)
#define MAX_QS (EMBER_CONCENTRATOR_MAX_SECONDS_BETWEEN_DISCOVERIES << 2)

// Event control stuff.
static bool eventActive = true;
static uint16_t timeToExecuteQS = 0;
static uint16_t lastExecutionQS;

#define elapsedTimeInt16u(oldTime, newTime) \
  ((uint16_t) ((uint16_t)(newTime) - (uint16_t)(oldTime)))

static uint16_t elapsedQS(void)
{
  uint16_t now = halCommonGetInt16uQuarterSecondTick();
  return elapsedTimeInt16u(lastExecutionQS, now);
}

//---------------------------------------------------------------------------
// Public functions

void emberConcentratorStartDiscovery(void)
{
  eventActive = true;
}

void emberConcentratorStopDiscovery(void)
{
  eventActive = false;
}

uint16_t emberConcentratorQueueRouteDiscovery(void)
{
  timeToExecuteQS = MIN_QS;
  return timeToExecuteQS;
}

void emberConcentratorNoteRouteError(EmberStatus status)
{
  if (status == EMBER_SOURCE_ROUTE_FAILURE
      || status == EMBER_MANY_TO_ONE_ROUTE_FAILURE) {
    routeErrorCount += 1;
    if (routeErrorCount >= EMBER_CONCENTRATOR_ROUTE_ERROR_THRESHOLD) {
      emberConcentratorQueueRouteDiscovery();
    }
  }
}

void emberConcentratorNoteDeliveryFailure(EmberOutgoingMessageType type,
                                          EmberStatus status)
{
  if ((type == EMBER_OUTGOING_DIRECT
       || type == EMBER_OUTGOING_VIA_ADDRESS_TABLE
       || type == EMBER_OUTGOING_VIA_BINDING)
      && status == EMBER_DELIVERY_FAILED) {
    deliveryFailureCount += 1;
    if (deliveryFailureCount >= EMBER_CONCENTRATOR_DELIVERY_FAILURE_THRESHOLD) {
      emberConcentratorQueueRouteDiscovery();
    }
  }
}

void emberConcentratorTick(void)
{
  if (eventActive) {
    uint16_t elapsed = elapsedQS();
    if (elapsed >= timeToExecuteQS) {
      routeErrorCount = 0;
      deliveryFailureCount = 0;
      lastExecutionQS = halCommonGetInt16uQuarterSecondTick();
      if (emberSendManyToOneRouteRequest(
            EMBER_CONCENTRATOR_TYPE, EMBER_CONCENTRATOR_RADIUS) == EMBER_SUCCESS) {
        emberConcentratorRouteDiscoverySentHandler();
      }
      timeToExecuteQS = MAX_QS;
    }
  }
}
