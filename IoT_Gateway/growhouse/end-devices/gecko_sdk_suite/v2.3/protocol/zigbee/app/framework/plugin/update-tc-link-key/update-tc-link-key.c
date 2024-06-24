// Copyright 2015 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "update-tc-link-key.h"

// TODO: add plugin option for joining node only functionality?

#define SERVER_MASK_STACK_COMPLIANCE_REVISION_MASK 0xFE00
//Bits 9 to 15 are set to 21. Everything else is 0.
#define SERVER_MASK_STACK_COMPLIANCE_REVISION_21 0x2A00

#define trustCenterAddressIsAllZeros(securityState, allZeros) \
  (!MEMCOMPARE((securityState).trustCenterLongAddress, (allZeros), EUI64_SIZE))

#ifdef EZSP_HOST
  #define lookupEui64ByNodeId ezspLookupEui64ByNodeId
  #define requestLinkKey ezspRequestLinkKey
#else
  #define lookupEui64ByNodeId emberLookupEui64ByNodeId
  #define requestLinkKey emberRequestLinkKey
#endif

#define STATE_NONE     (0x00)
#define STATE_REQUEST  (0x01)
#define STATE_RESPONSE (0x02)
static uint8_t state = STATE_NONE;

EmberEventControl emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl;
#define beginTcLinkKeyUpdateEvent (emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl)

//Setting the default timer to a day.
static uint32_t LinkKeyUpdateTimerMilliseconds = MILLISECOND_TICKS_PER_DAY;
bool emNodeDescriptorCheckCallbackStatus = false;
// -----------------------------------------------------------------------------
// Public API

EmberStatus emberAfPluginUpdateTcLinkKeyStart(void)
{
  EmberStatus status = EMBER_INVALID_CALL;
  EmberCurrentSecurityState currentSecurityState;
  const EmberEUI64 allZeroEui64 = { 0, 0, 0, 0, 0, 0, 0, 0, };
  if (state == STATE_NONE || state == STATE_REQUEST) {
    status = EMBER_NOT_JOINED;
    if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
      status = EMBER_SECURITY_CONFIGURATION_INVALID;
      emberGetCurrentSecurityState(&currentSecurityState);
      if (!trustCenterAddressIsAllZeros(currentSecurityState, allZeroEui64)) {
        status = EMBER_INVALID_CALL;
        if (emberAfGetNodeId() != EMBER_TRUST_CENTER_NODE_ID) {
          status = emberNodeDescriptorRequest(EMBER_TRUST_CENTER_NODE_ID,
                                              EMBER_AF_DEFAULT_APS_OPTIONS);
          if (status == EMBER_SUCCESS) {
            state = STATE_REQUEST;
            emNodeDescriptorCheckCallbackStatus = false;
          }
        }
      }
    }
  }

  return status;
}

bool emberAfPluginUpdateTcLinkKeyStop(void)
{
  uint8_t oldState = state;

  state = STATE_NONE;

  return (oldState != STATE_NONE);
}

void emberAfPluginUpdateTcLinkKeySetDelay(uint32_t delayMs)
{
  emberEventControlSetDelayMS(beginTcLinkKeyUpdateEvent, delayMs);
}

void emberAfPluginUpdateTcLinkKeySetInactive(void)
{
  emberEventControlSetInactive(beginTcLinkKeyUpdateEvent);
}

bool emberAfPluginUpdateTcLinkKeyZdoMessageReceivedCallback(EmberNodeId sender,
                                                            EmberApsFrame* apsFrame,
                                                            uint8_t* message,
                                                            uint16_t length)
{
  EmberStatus status;
  EmberEUI64 trustCenterEui64;
  emNodeDescriptorCheckCallbackStatus = true;
  if (state == STATE_REQUEST) {
    if (apsFrame->clusterId == NODE_DESCRIPTOR_RESPONSE) {
      uint16_t serverMask = message[12] | (message[13] << 8);
      if (((serverMask & SERVER_MASK_STACK_COMPLIANCE_REVISION_MASK)
           >= SERVER_MASK_STACK_COMPLIANCE_REVISION_21)) {
        if (lookupEui64ByNodeId(EMBER_TRUST_CENTER_NODE_ID, trustCenterEui64)
            == EMBER_SUCCESS) {
          status = requestLinkKey(trustCenterEui64); // trust center link key
          state = (status == EMBER_SUCCESS ? STATE_RESPONSE : STATE_NONE);
          emberAfCorePrintln("%p: %p: 0x%X",
                             EMBER_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME,
                             "Requesting link key from R21 trust center",
                             status);
          emberEventControlSetInactive(beginTcLinkKeyUpdateEvent);
        }
      } else {
        // The reason for setting the delay is because we may have a trust center
        // which is pre-R21 and a joining device which is R21+. Then the joining
        // device sends a node descriptor request and realizes that the trust center
        // is not R21+ so no trust center link key update is attempted. Now say in
        // the future at some point of time the Trust center is updated from pre-R21
        // to R21+ then we want to have the joining device to continue attempting a
        // link key update periodically to build a secure connection and not just
        // attempt it once during the joining of the network never to try again if it
        // fails. Hence the periodic event handler which attempts this update.
        emberAfPluginUpdateTcLinkKeySetDelay(LinkKeyUpdateTimerMilliseconds);
        emberAfPluginUpdateTcLinkKeyStatusCallback(EMBER_TRUST_CENTER_IS_PRE_R21);
        state = STATE_NONE;
      }
    }
  }

  return false;
}

void emberAfPluginUpdateTcLinkKeyZigbeeKeyEstablishmentCallback(EmberEUI64 partner,
                                                                EmberKeyStatus status)
{
  if (state == STATE_RESPONSE) {
    emberAfCorePrintln("%p: %p: 0x%X",
                       EMBER_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME,
                       "New key established",
                       status);
    emberAfCorePrint("Partner: ");
    emberAfCorePrintBuffer(partner, EUI64_SIZE, true); // withSpace?
    emberAfCorePrintln("");

    if (emberAfPluginUpdateTcLinkKeyStatusCallback(status)) {
      state = STATE_NONE;
    }
  }
}

// =============================================================================
// Begin Update of TC Link Key

// At the end of the network steering process, we need to update the
// trust center link key (if we are in a centralized network) and broadcast
// a permit join to extend the network. This process needs to happen after
// we send our device announce and possibly our network timeout request if we
// are an end device. If the trust center link key update does not happen after
// the network steering process then it needs to be attempted at a regular time
// interval such that once the conditions are met(devices are > R21), the trust
// center link key is updated.

void emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler(void)
{
  if (state == STATE_NONE || state == STATE_REQUEST) {
    emberEventControlSetInactive(beginTcLinkKeyUpdateEvent);
    EmberStatus status = emberAfPluginUpdateTcLinkKeyStart();
    emberAfCorePrintln("%p: %p: 0x%X",
                       EMBER_AF_PLUGIN_UPDATE_TC_LINK_KEY_PLUGIN_NAME,
                       "Starting update trust center link key process",
                       status);
    if (status != EMBER_SUCCESS) {
      emberLeaveNetwork();
      emAfPluginNetworkSteeringCleanup(status);
    }
  }
  // Resetting the link key event timer since this is needed until the
  // node descriptor request callback is processed. This is useful in
  // the case when the end device sends node descriptor requests and the
  // hub for some reason is not reachable due to upgrades happening, turned
  // off, etc. If the node descriptor response is received then this is
  // set to inactive.
  if (!emNodeDescriptorCheckCallbackStatus) {
    emberAfPluginUpdateTcLinkKeySetDelay(LinkKeyUpdateTimerMilliseconds);
  }
}

void emberAfPluginSetTCLinkKeyUpdateTimerMilliSeconds(uint32_t timeInMilliseconds)
{
  LinkKeyUpdateTimerMilliseconds = timeInMilliseconds;
}
