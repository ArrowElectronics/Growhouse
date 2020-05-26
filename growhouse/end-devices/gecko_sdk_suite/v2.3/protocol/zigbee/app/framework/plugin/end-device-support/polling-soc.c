// Copyright 2014 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/plugin/end-device-support/end-device-support.h"

// *****************************************************************************
// Globals

extern EmberEventControl emberAfPluginEndDeviceSupportPollingNetworkEventControls[];

// *****************************************************************************
// Functions

// This is called to scheduling polling events for the network(s).  We only
// care about end device networks.  For each of those, a polling event will be
// scheduled for joined networks or canceled otherwise.
void emberAfPluginEndDeviceSupportTickCallback(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    emberAfPushNetworkIndex(i);
    if (emAfProIsCurrentNetwork()
        && EMBER_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
      if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
        EmAfPollingState *state = &emAfPollingStates[i];
        uint32_t lastPollIntervalMs = state->pollIntervalMs;
        state->pollIntervalMs = emberAfGetCurrentPollIntervalMsCallback();
        if (state->pollIntervalMs != lastPollIntervalMs
            || !emberAfNetworkEventControlGetActive(emberAfPluginEndDeviceSupportPollingNetworkEventControls)) {
          emberAfDebugPrintln("Next poll nwk %d in %l ms",
                              i,
                              state->pollIntervalMs);
          emberAfNetworkEventControlSetDelayMS(emberAfPluginEndDeviceSupportPollingNetworkEventControls,
                                               state->pollIntervalMs);
        }
      } else {
        emberAfNetworkEventControlSetInactive(emberAfPluginEndDeviceSupportPollingNetworkEventControls);
      }
    }
    emberAfPopNetworkIndex();
  }
}

// Whenever the polling event fires for a network, a MAC data poll is sent.
void emberAfPluginEndDeviceSupportPollingNetworkEventHandler(void)
{
  EmberNetworkStatus state = emberAfNetworkState();
  if (state == EMBER_JOINED_NETWORK) {
    EmberStatus status = emberPollForData();
    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("poll nwk %d: 0x%x", emberGetCurrentNetwork(), status);
    }
  }
}

// This function is called when a poll completes and explains what happend with
// the poll.  If the number of sequential data polls not ACKed by the parent
// exceeds the threshold, we will try to find a new parent.
void emberPollCompleteHandler(EmberStatus status)
{
  emAfPollCompleteHandler(status,
                          EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS);
}
