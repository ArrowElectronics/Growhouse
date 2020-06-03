// *******************************************************************
// * green-power-server.h
// *
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

#define EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN (7)

#ifndef EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE (1)
#endif

typedef struct {
  bool inCommissioningMode;
  bool proxiesInvolved;
  //EmberAfGreenPowerClientCommissioningExitMode exitMode;
  //uint16_t gppCommissioningWindow;
  //uint8_t channel;
  //bool unicastCommunication;
  //EmberNodeId commissioningSink;
} EmberAfGreenPowerServerCommissioningState;

typedef enum {
  EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED = 1,
  EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA = 2,
  EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD = 4
} EmberAfGreenPowerServerGpdToZclCmdMappingPayloadSrc;

typedef struct {
  bool            validEntry;
  uint8_t         options;
  EmberGpAddress  gpAddr;

  // A.3.6.2.2 GPD application functionality translation.
  // If the EndPoint field is set to 0xfd, there are no paired endpoints. If
  // the EndPoint field is set to 0xff, all matching endpoints are paired.
  uint8_t         endpoint;

  // If the GPD Command field is set to 0xAF, all of the GPD sensor report
  // commands 0xA0 – 0xA3 are supported. Thus, 0xAF is not used as a true
  // GPD CommandID, but as a way to make the Translation Tables more compact.
  uint8_t         gpdCommand;
  uint16_t        zigbeeProfile;
  uint16_t        zigbeeCluster;
  uint8_t         zigbeeCommandId;
  EmberAfGreenPowerServerGpdToZclCmdMappingPayloadSrc payloadSrc;

  // This is a Zigbee string./ If the Length sub-field of the ZigBee Command payload field is set to 0x00,
  // the Payload sub-field is not present, and the ZigBee command is sent without payload.
  //
  // If the Length sub-field of the ZigBee Command payload field is set to 0xff,
  // the Payload sub-field is not present, and the payload from the triggering
  // GPD command is to be copied verbatim into the ZigBee command.  If the Length
  // sub-field of the ZigBee Command payload field is set to 0xfe, the Payload sub-field is not present, and the pay-load from the triggering GPD command needs to be parsed. For all other values of the Length sub-field, the Payload sub-field is present, has a length as defined in the Length sub-field and specifies the pay-load to be used.
  uint8_t zclPayloadDefault[EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN];
} EmberAfGreenPowerServerGpdCommandTranslation;

void emberAfPluginGreenPowerServerClearCustomizedTranslationTable();

void emberAfPluginGreenPowerServerSetCustomizedTranslationTable(uint8_t index,
                                                                EmberAfGreenPowerServerGpdCommandTranslation * trans);
void emberAfPluginGreenPowerServerTablePrint(void);
