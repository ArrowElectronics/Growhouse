// Copyright 2015 Silicon Laboratories, Inc.

#include "af.h"
#include "network-steering.h"
#include "network-steering-internal.h"

#if defined(EMBER_AF_GENERATE_CLI) || defined(EMBER_AF_API_COMMAND_INTERPRETER2)

// -----------------------------------------------------------------------------
// Helper macros and functions

static void addOrSubtractChannel(uint8_t maskToAddTo,
                                 uint8_t channelToAdd,
                                 bool operationIsAdd)
{
  if (channelToAdd < EMBER_MIN_802_15_4_CHANNEL_NUMBER
      || channelToAdd > EMBER_MAX_802_15_4_CHANNEL_NUMBER) {
    emberAfCorePrintln("Channel not valid: %d", channelToAdd);
  } else if (maskToAddTo == 1) {
    if (operationIsAdd) {
      SETBIT(emAfPluginNetworkSteeringPrimaryChannelMask, channelToAdd);
    } else {
      CLEARBIT(emAfPluginNetworkSteeringPrimaryChannelMask, channelToAdd);
    }

    emberAfCorePrintln("%p mask now 0x%4X",
                       "Primary",
                       emAfPluginNetworkSteeringPrimaryChannelMask);
  } else if (maskToAddTo == 2) {
    if (operationIsAdd) {
      SETBIT(emAfPluginNetworkSteeringSecondaryChannelMask, channelToAdd);
    } else {
      CLEARBIT(emAfPluginNetworkSteeringSecondaryChannelMask, channelToAdd);
    }

    emberAfCorePrintln("%p mask now 0x%4X",
                       "Secondary",
                       emAfPluginNetworkSteeringSecondaryChannelMask);
  } else {
    emberAfCorePrintln("Mask not valid: %d", maskToAddTo);
  }
}

// -----------------------------------------------------------------------------
// Command definitions

// plugin network-steering mask add <[1=primary|2=secondary]:1> <channel:1>
// plugin network-steering mask subtract <[1=primary|2=secondary]:1> <channel:1>
void emberAfPluginNetworkSteeringChannelAddOrSubtractCommand(void)
{
  bool operationIsAdd = (emberStringCommandArgument(-1, NULL)[0] == 'a');
  uint8_t maskToAddTo  = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t channelToAdd = (uint8_t)emberUnsignedCommandArgument(1);

  addOrSubtractChannel(maskToAddTo,
                       channelToAdd,
                       operationIsAdd);
}

void emberAfPluginNetworkSteeringChannelSetCommand(void)
{
  bool maskIsPrimary = (((uint8_t)emberUnsignedCommandArgument(0)) == 1);
  uint32_t value = (uint32_t)emberUnsignedCommandArgument(1);
  uint32_t *mask = (maskIsPrimary
                    ? &emAfPluginNetworkSteeringPrimaryChannelMask
                    : &emAfPluginNetworkSteeringSecondaryChannelMask);

  emberAfCorePrintln("%p: Set %p mask to 0x%4X",
                     emAfNetworkSteeringPluginName,
                     (maskIsPrimary ? "primary" : "secondary"),
                     (*mask = value));
}

// plugin network-steering status
void emberAfPluginNetworkSteeringStatusCommand(void)
{
  emberAfCorePrintln("%p: %p:",
                     emAfNetworkSteeringPluginName,
                     "Status");

  emberAfCorePrintln("Channel mask:");
  emberAfCorePrint("    (1) 0x%4X [",
                   emAfPluginNetworkSteeringPrimaryChannelMask);
  emberAfPrintChannelListFromMask(emAfPluginNetworkSteeringPrimaryChannelMask);
  emberAfCorePrintln("]");
  emberAfCorePrint("    (2) 0x%4X [",
                   emAfPluginNetworkSteeringSecondaryChannelMask);
  emberAfPrintChannelListFromMask(emAfPluginNetworkSteeringSecondaryChannelMask);
  emberAfCorePrintln("]");

  emberAfCorePrintln("State: 0x%X (%p)",
                     emAfPluginNetworkSteeringState,
                     emAfPluginNetworkSteeringStateNames[emAfPluginNetworkSteeringState]);
  emberAfCorePrintln("Pan ID index: %d",
                     emAfPluginNetworkSteeringPanIdIndex);
  emberAfCorePrintln("Current channel: %d",
                     emAfPluginNetworkSteeringCurrentChannel);
  emberAfCorePrintln("Total beacons: %d",
                     emAfPluginNetworkSteeringTotalBeacons);
  emberAfCorePrintln("Join attempts: %d",
                     emAfPluginNetworkSteeringJoinAttempts);
  emberAfCorePrintln("Network state: 0x%X",
                     emberAfNetworkState());
}

void emberAfPluginNetworkSteeringStartCommand(void)
{
  emAfPluginNetworkSteeringOptionsMask = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfPluginNetworkSteeringStart();
}

void emberAfPluginNetworkSteeringStopCommand(void)
{
  emberAfCorePrintln("%p: %p: 0x%X",
                     emAfNetworkSteeringPluginName,
                     "Stop",
                     emberAfPluginNetworkSteeringStop());
}

#endif /*
          defined(EMBER_AF_GENERATE_CLI)
        || defined(EMBER_AF_API_COMMAND_INTERPRETER2)
        */
