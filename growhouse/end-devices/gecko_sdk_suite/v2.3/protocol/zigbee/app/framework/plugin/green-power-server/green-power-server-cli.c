#include "app/framework/include/af.h"
#include "app/framework/plugin/green-power-common/green-power-common.h"
#include "app/framework/plugin/green-power-server/green-power-server.h"

#ifndef EMBER_AF_GENERATE_CLI
#error The Green Power Client plugin is not compatible with the legacy CLI.
#endif

extern uint8_t newKey[EMBER_ENCRYPTION_KEY_SIZE];
#if 0
//XXXEZSP
void emberHmacAesHash(const uint8_t *key,
                      const uint8_t *data,
                      uint8_t dataLength,
                      uint8_t *result);

void hmacTest()
{
  uint8_t i;
  EmberKeyData linkKey = { {
                             0x01, 0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0d
                           } };

  EmberKeyData key2 = { {
                          0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
                        } };

  uint8_t id[] = { 0x21, 0x43, 0x65, 0x87 };

  uint8_t result[16];
  emberHmacAesHash(linkKey.contents, (uint8_t *)"ZGP", 3, result);
  emberHmacAesHash(key2.contents, id, 4, result);
  for (i = 0; i < 16; i++) {
    emberAfCorePrint("%x", result[i]);
  }
}
#endif

void emberAfPluginGreenPowerServerCryptoTest(void)
{
  uint8_t payload[] = { 0, // length
                        0, //deviceID,
                        EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_EXTENDED_OPTIONS_FIELD,
                        (EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT
                         | EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION)
                        >> 8,
                        0x7D,
                        0x17,
                        0x7B,
                        0xD2,
                        0x9E,
                        0xA0,
                        0xFD,
                        0xA6, //key
                        0xB0,
                        0x17,
                        0x03,
                        0x65,
                        0x87,
                        0xDC,
                        0x26,
                        0x00, //key
  };

  emberAfGreenPowerClusterGpCommissioningNotificationCallback(0,
                                                              0x12345678,
                                                              NULL,
                                                              0, //endpoint
                                                              0,
                                                              0xF2, //GPD_COMMAND_COMMISSIONING,
                                                              payload,
                                                              0,
                                                              0,
                                                              0);

  emberAfCorePrint("key is:");
  //TODO fix this to make a real test
  //for(i=0;i<EMBER_ENCRYPTION_KEY_SIZE;i++)
  //  emberAfCorePrint("%x",newKey[i]);

  //hmacTest();
}

void emberAfPluginGreenPowerServerCommissioningMode(void)
{
  if (emberUnsignedCommandArgument(0)) {
    emberAfGreenPowerClusterGpSinkCommissioningModeCallback(0x09,    //options)
                                                            0xFFFF,    //addr
                                                            0xFFFF,    //addr
                                                            0xFF); //all endpoints
  } else {
    emberAfGreenPowerClusterGpSinkCommissioningModeCallback(0x00,    //options)
                                                            0xFFFF,    //addr
                                                            0xFFFF,    //addr
                                                            0xFF); //all endpoints
  }
}

void emberAfPluginGreenPowerServerCliClearCustomizedTranslationTable(void)
{
  emberAfPluginGreenPowerServerClearCustomizedTranslationTable();
}

void emberAfPluginGreenPowerServerCliSetCustomizedTranslationTable(void)
{
  EmberAfGreenPowerServerGpdCommandTranslation table = { 0 };
  uint8_t index = emberUnsignedCommandArgument(0);
  bool validEntry = (bool)emberUnsignedCommandArgument(1);

  if (index < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE) {
    table.options = emberUnsignedCommandArgument(2);
    table.gpAddr.endpoint = emberUnsignedCommandArgument(3);

    if ((table.options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID)
        == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
      emberCopyBigEndianEui64Argument(4, table.gpAddr.id.gpdIeeeAddress);
    } else if ((table.options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID)
               == EMBER_GP_APPLICATION_SOURCE_ID) {
      table.gpAddr.id.sourceId = emberUnsignedCommandArgument(5);
    }
    table.validEntry = validEntry;
    table.endpoint = emberUnsignedCommandArgument(6);
    table.gpdCommand = emberUnsignedCommandArgument(7);
    table.zigbeeProfile = emberUnsignedCommandArgument(8);
    table.zigbeeCluster = emberUnsignedCommandArgument(9);
    table.zigbeeCommandId = emberUnsignedCommandArgument(10);
    table.payloadSrc = emberUnsignedCommandArgument(11);
    emberCopyStringArgument(12,
                            table.zclPayloadDefault,
                            EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN,
                            false);
    emberAfPluginGreenPowerServerSetCustomizedTranslationTable(index, &table);
  } else {
    emberAfGreenPowerClusterPrintln("invalid Translation Table index!");
  }
}

void emberAfPluginGreenPowerServerCliTablePrint(void)
{
  emberAfPluginGreenPowerServerTablePrint();
}
