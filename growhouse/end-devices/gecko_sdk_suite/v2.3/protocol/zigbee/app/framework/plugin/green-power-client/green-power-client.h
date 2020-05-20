// *******************************************************************
// * green-power-client.h
// *
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

/* bookkeeping for Commissioning related info. */

#define GP_CLIENT_ON_TRANSMIT_CHANNEL_MASK BIT(0)
#define GP_CLIENT_TRANSMIT_SAME_AS_OPERATIONAL_CHANNEL_MASK BIT(1)

typedef enum  {
  EMBER_GP_GPD_MAC_SEQ_NUM_CAP_SEQUENTIAL  = 0x00,
  EMBER_GP_GPD_MAC_SEQ_NUM_CAP_RANDOM      = 0x01,
} EmberGpGpdMacSeqNumCap;

typedef enum {
  EMBER_AF_GPC_COMMISSIONING_EXIT_ON_COMMISSIONING_WINDOW_EXP = 0x1,
  EMBER_AF_GPC_COMMISSIONING_EXIT_ON_FIRST_PAIRING_SUCCESS = 0x2,
  EMBER_AF_GPC_COMMISSIONING_EXIT_ON_GP_PROXY_COMMISSIONING_MODE_EXIT = 0x4,
  EMBER_AF_GPC_COMMISSIONING_EXIT_MODE_MAX = 0xFF,
} EmberAfGreenPowerClientCommissioningExitMode;

typedef struct {
  bool inCommissioningMode;
  EmberAfGreenPowerClientCommissioningExitMode exitMode;
  uint16_t gppCommissioningWindow;
  uint8_t channel;
  bool unicastCommunication;
  EmberNodeId commissioningSink;
  uint8_t channelStatus;  //bit 0 shows if we are on transmit channel(1) or on operational channel(0). bit 1 shows if operational channel is same as transmit channel (1).
} EmberAfGreenPowerClientCommissioningState;

typedef struct {
  EmberGpAddress addrs[EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_ADDR_ENTRIES];
  uint8_t randomSeqNums[EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_ADDR_ENTRIES][EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_SEQ_NUM_ENTRIES_PER_ADDR];
  uint32_t expirationTimes[EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_ADDR_ENTRIES][EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_SEQ_NUM_ENTRIES_PER_ADDR];
} EmberAfGreenPowerDuplicateFilter;

bool emGpMessageChecking(EmberGpAddress *gpAddr, uint8_t sequenceNumber);
void emberAfPluginGreenPowerClientChannelEventHandler(void);
void emberAfPluginGreenPowerClientExitCommissioningEventHandler(void);
