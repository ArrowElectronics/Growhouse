// Copyright 2016 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/util/common/form-and-join.h"
#include "zll-commissioning.h"

#define WAITING_BIT 0x80

enum {
  INITIAL            = 0x00,
  UNUSED_PRIMARY     = 0x01,
  JOINABLE_PRIMARY   = 0x02,
  JOINABLE_SECONDARY = 0x03,
  WAITING_PRIMARY    = WAITING_BIT | JOINABLE_PRIMARY,
  WAITING_SECONDARY  = WAITING_BIT | JOINABLE_SECONDARY,
};
static uint8_t state = INITIAL;
EmberEventControl emberAfPluginZllCommissioningNetworkEventControl;

EmberStatus emberAfZllScanForJoinableNetwork(void)
{
  EmberStatus status = EMBER_INVALID_CALL;
  if (state == INITIAL) {
    // TODO: The scan duration is hardcoded to 3 for the joinable part of the
    // form and join library, but ZLL specifies the duration should be 4.
    status = emberScanForJoinableNetwork(emberGetZllPrimaryChannelMask(),
                                         emAfZllExtendedPanId);
    if (status == EMBER_SUCCESS) {
      state = JOINABLE_PRIMARY;
    } else {
      emberAfAppPrintln("Error: %p: 0x%x",
                        "could not scan for joinable network",
                        status);
    }
  }
  return status;
}

EmberStatus emberAfZllScanForUnusedPanId(void)
{
  EmberStatus status = EMBER_INVALID_CALL;
  if (state == INITIAL) {
    status = emberScanForUnusedPanId(emberGetZllPrimaryChannelMask(),
                                     EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_DURATION);
    if (status == EMBER_SUCCESS) {
      state = UNUSED_PRIMARY;
    } else {
      emberAfAppPrintln("Error: %p: 0x%x",
                        "could not scan for unused network",
                        status);
    }
  }
  return status;
}

void emberAfJoinableNetworkFoundCallback(EmberZigbeeNetwork *networkFound,
                                         uint8_t lqi,
                                         int8_t rssi)
{
  EmberStatus status = EMBER_ERR_FATAL;
  if (emberAfPluginZllCommissioningJoinCallback(networkFound, lqi, rssi)) {
    EmberNetworkParameters parameters;
    MEMSET(&parameters, 0, sizeof(EmberNetworkParameters));
    MEMMOVE(parameters.extendedPanId,
            networkFound->extendedPanId,
            EXTENDED_PAN_ID_SIZE);
    parameters.panId = networkFound->panId;
    parameters.radioTxPower = EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RADIO_TX_POWER;
    parameters.radioChannel = networkFound->channel;

    emberAfZllSetInitialSecurityState();
    status = emberAfJoinNetwork(&parameters);
  }

  // Note: If the application wants to skip this network or if the join fails,
  // we cannot call emberScanForNextJoinableNetwork to continue the scan
  // because we would be recursing.  Instead, schedule an event to fire at the
  // next tick and restart from there.
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("Error: %p: 0x%x", "could not join network", status);
    emberEventControlSetActive(emberAfPluginZllCommissioningNetworkEventControl);
  }
}

void emberAfPluginFormAndJoinNetworkFoundCallback(EmberZigbeeNetwork *networkFound,
                                                  uint8_t lqi,
                                                  int8_t rssi)
{
  emberAfJoinableNetworkFoundCallback(networkFound, lqi, rssi);
}

void emberAfUnusedPanIdFoundCallback(EmberPanId panId, uint8_t channel)
{
  EmberStatus status = emAfZllFormNetwork(channel,
                                          EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RADIO_TX_POWER,
                                          panId);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("Error: %p: 0x%x", "could not form network", status);
    emberAfScanErrorCallback(status);
  }
}

void emberAfPluginFormAndJoinUnusedPanIdFoundCallback(EmberPanId panId, uint8_t channel)
{
  emberAfUnusedPanIdFoundCallback(panId, channel);
}

void emberAfScanErrorCallback(EmberStatus status)
{
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_SECONDARY_CHANNELS
  if (status == EMBER_NO_BEACONS
      && state == JOINABLE_PRIMARY
      && emberGetZllSecondaryChannelMask() != 0) {
    state = JOINABLE_SECONDARY;
    emberEventControlSetActive(emberAfPluginZllCommissioningNetworkEventControl);
    return;
  }
#endif
  emberAfAppPrintln("Error: %p: 0x%x",
                    (state == UNUSED_PRIMARY
                     ? "could not find unused network"
                     : "could not find joinable network"),
                    status);
  state = INITIAL;
}

void emberAfGetFormAndJoinExtendedPanIdCallback(uint8_t *resultLocation)
{
  MEMMOVE(resultLocation, emAfZllExtendedPanId, EXTENDED_PAN_ID_SIZE);
}

void emberAfSetFormAndJoinExtendedPanIdCallback(const uint8_t *extendedPanId)
{
  MEMMOVE(emAfZllExtendedPanId, extendedPanId, EXTENDED_PAN_ID_SIZE);
}

void emberAfPluginZllCommissioningNetworkEventHandler(void)
{
  EmberStatus status = EMBER_ERR_FATAL;
  emberEventControlSetInactive(emberAfPluginZllCommissioningNetworkEventControl);
  if ((state == JOINABLE_PRIMARY || state == JOINABLE_SECONDARY)
      && emberFormAndJoinCanContinueJoinableNetworkScan()) {
    status = emberScanForNextJoinableNetwork();
    if (status != EMBER_SUCCESS) {
      emberAfAppPrintln("Error: %p: 0x%x",
                        "could not continue scan for joinable network",
                        status);
    }
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_SECONDARY_CHANNELS
  } else if (state == JOINABLE_SECONDARY) {
    // TODO: The scan duration is hardcoded to 3 for the joinable part of the
    // form and join library, but ZLL specifies the duration should be 4.
    status = emberScanForJoinableNetwork(emberGetZllSecondaryChannelMask(),
                                         emAfZllExtendedPanId);
    if (status != EMBER_SUCCESS) {
      emberAfAppPrintln("Error: %p: 0x%x",
                        "could not scan for joinable network",
                        status);
    }
#endif
  }

  if (status != EMBER_SUCCESS) {
    state = INITIAL;
    emberFormAndJoinCleanup(EMBER_SUCCESS);
  }
}

void emAfZllStackStatus(EmberStatus status)
{
  uint8_t delayMinutes = MAX_INT8U_VALUE;

  // This function will pick up all non-touchlink stack status's, so as to keep
  // the ZLL API state up to date.
  if (status == EMBER_NETWORK_UP) {
    if (state == UNUSED_PRIMARY) {
      delayMinutes = 0;
    } else if (state == JOINABLE_PRIMARY || state == JOINABLE_SECONDARY || state == INITIAL) {
      // When either a router or an end device joins a non-ZLL network, it is
      // no longer factory new.  On a non-ZLL network, ZLL devices that are
      // normally address assignment capable do not have free network or group
      // addresses nor do they have a range of group addresses for themselves.
      // To ensure that ZLL devices will always operate as ZigBee 3.0 applications,
      // we need to set the ZLL profile interop bit even when the application
      // joins a classical ZigBee network. This way, if the device is stolen from
      // a classical ZigBee network to a ZLL network, it will operate as a
      // ZigBee 3.0 device. This is now set at plugin initialization time.
      EmberTokTypeStackZllData token;
      emberZllGetTokenStackZllData(&token);

      // emberAfCorePrintln("emAfZllStackStatus: OpInProg = %d, zllState = %X", emberZllOperationInProgress(), token.bitmask);

      // We need to filter out a NETWORK UP at startup, as well as a NETWORK UP
      // from a device during a touchlink operation, whether target or initiator,
      // or from a rejoin.
      if (!emberZllOperationInProgress() && (token.bitmask & EMBER_ZLL_STATE_FACTORY_NEW)) {
        // Set ZLL stack tokens for a non-zll network.
        token.bitmask &= ~EMBER_ZLL_STATE_FACTORY_NEW;
        token.freeNodeIdMin = token.freeNodeIdMax = EMBER_ZLL_NULL_NODE_ID;
        token.myGroupIdMin = EMBER_ZLL_NULL_GROUP_ID;
        token.freeGroupIdMin = token.freeGroupIdMax = EMBER_ZLL_NULL_GROUP_ID;
        emberZllSetTokenStackZllData(&token);
        emberZllSetNonZllNetwork();
      }

      if (state > INITIAL) {
        // If we're here via find and join, finish off.
        state |= WAITING_BIT; // JOINABLE_XXX to WAITING_XXX
        delayMinutes = EMBER_AF_PLUGIN_ZLL_COMMISSIONING_JOINABLE_SCAN_TIMEOUT_MINUTES;
      }
    }
  } else if (JOINABLE_PRIMARY <= state) {
    state &= ~WAITING_BIT; // JOINABLE_XXX or WAITING_XXX to JOINABLE_XXX
    delayMinutes = 0;
  }

  if (delayMinutes == 0) {
    emberAfPluginZllCommissioningNetworkEventHandler();
  } else if (delayMinutes != MAX_INT8U_VALUE) {
    emberEventControlSetDelayMinutes(emberAfPluginZllCommissioningNetworkEventControl,
                                     delayMinutes);
  }
}
