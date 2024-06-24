// Copyright 2015 Silicon Laboratories, Inc.

#include "app/framework/plugin/network-steering/network-steering.h"

extern PGM_P emAfPluginNetworkSteeringStateNames[];
extern uint8_t emAfPluginNetworkSteeringPanIdIndex;
extern uint8_t emAfPluginNetworkSteeringCurrentChannel;
extern uint8_t emAfPluginNetworkSteeringTotalBeacons;
extern uint8_t emAfPluginNetworkSteeringJoinAttempts;

uint8_t emAfPluginNetworkSteeringGetMaxPossiblePanIds(void);
void emAfPluginNetworkSteeringClearStoredPanIds(void);
uint16_t* emAfPluginNetworkSteeringGetStoredPanIdPointer(uint8_t index);

void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState);

extern EmberAfPluginNetworkSteeringJoiningState emAfPluginNetworkSteeringState;

#define emAfPluginNetworkSteeringStateUsesInstallCodes()                 \
  (((emAfPluginNetworkSteeringState)                                     \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_INSTALL_CODE) \
   || ((emAfPluginNetworkSteeringState)                                  \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_INSTALL_CODE))

#define emAfPluginNetworkSteeringStateUsesCentralizedKey()              \
  (((emAfPluginNetworkSteeringState)                                    \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_CENTRALIZED) \
   || ((emAfPluginNetworkSteeringState)                                 \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_CENTRALIZED))

#define emAfPluginNetworkSteeringStateUsesDistributedKey()              \
  (((emAfPluginNetworkSteeringState)                                    \
    == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_PRIMARY_DISTRIBUTED) \
   || ((emAfPluginNetworkSteeringState)                                 \
       == EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_SCAN_SECONDARY_DISTRIBUTED))

#define emAfPluginNetworkSteeringStateSetUpdateTclk() \
  ((emAfPluginNetworkSteeringState)                   \
     |= EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK)
#define emAfPluginNetworkSteeringStateUpdateTclk() \
  ((emAfPluginNetworkSteeringState)                \
   & EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK)
#define emAfPluginNetworkSteeringStateClearUpdateTclk() \
  ((emAfPluginNetworkSteeringState)                     \
     &= ~EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_UPDATE_TCLK)

#define emAfPluginNetworkSteeringStateSetVerifyTclk() \
  ((emAfPluginNetworkSteeringState)                   \
     |= EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK)
#define emAfPluginNetworkSteeringStateVerifyTclk() \
  ((emAfPluginNetworkSteeringState)                \
   & EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK)
#define emAfPluginNetworkSteeringStateClearVerifyTclk() \
  ((emAfPluginNetworkSteeringState)                     \
     &= ~EMBER_AF_PLUGIN_NETWORK_STEERING_STATE_VERIFY_TCLK)
