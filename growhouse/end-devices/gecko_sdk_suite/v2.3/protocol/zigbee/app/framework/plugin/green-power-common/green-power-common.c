// *******************************************************************
// * green-power-common.c
// *
// * Place for common functions / definitions shared by
// * Green Power Client/Server
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// * Jing Teng ( jing.teng@silabs.com )
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"
#include "green-power-common.h"
#include "stack/include/gp-types.h"

uint16_t emberAfFillCommandGreenPowerClusterGpNotificationSmart(uint16_t options,
                                                                uint32_t gpdSrcId,
                                                                uint8_t* gpdIeee,
                                                                uint8_t  gpdEndpoint,
                                                                uint32_t gpdSecurityFrameCounter,
                                                                uint8_t  gpdCommandId,
                                                                uint8_t gpdCommandPayloadLength,
                                                                const uint8_t* gpdCommandPayload,
                                                                uint16_t gppShortAddress,
                                                                uint8_t  gppDistance)
{
  uint16_t charCount = 0;
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                          | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_NOTIFICATION_COMMAND_ID,
                                         "v",
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(gpdEndpoint);
    charCount += EUI64_SIZE + 1;
  }

  emberAfPutInt32uInResp(gpdSecurityFrameCounter);
  emberAfPutInt8uInResp(gpdCommandId);
  emberAfPutInt8uInResp(gpdCommandPayloadLength);
  emberAfPutBlockInResp(gpdCommandPayload, gpdCommandPayloadLength);

  charCount += 2 + 1 + gpdCommandPayloadLength + 1;
  if (options & EMBER_AF_GP_NOTIFICATION_OPTION_PROXY_INFO_PRESENT) {
    emberAfPutInt16uInResp(gppShortAddress);
    emberAfPutInt8uInResp(gppDistance);
    charCount += 3;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpPairingSearchSmart(uint16_t options,
                                                                 uint32_t gpdSrcId,
                                                                 uint8_t* gpdIeee,
                                                                 uint8_t endpoint)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                                         ZCL_GREEN_POWER_CLUSTER_ID,             \
                                         ZCL_GP_PAIRING_SEARCH_COMMAND_ID,       \
                                         "v",                                    \
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpTunnelingStopSmart(uint8_t options,
                                                                 uint32_t gpdSrcId,
                                                                 uint8_t* gpdIeee,
                                                                 uint8_t endpoint,
                                                                 uint32_t gpdSecurityFrameCounter,
                                                                 uint16_t gppShortAddress,
                                                                 int8_t gppDistance)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                                         ZCL_GREEN_POWER_CLUSTER_ID,             \
                                         ZCL_GP_TUNNELING_STOP_COMMAND_ID,       \
                                         "u",                                    \
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  emberAfPutInt32uInResp(gpdSecurityFrameCounter);
  emberAfPutInt16uInResp(gppShortAddress);
  emberAfPutInt8uInResp(gppDistance);

  charCount += 7;

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpCommissioningNotificationSmart(uint16_t options,
                                                                             uint32_t gpdSrcId,
                                                                             uint8_t* gpdIeee,
                                                                             uint8_t endpoint,
                                                                             uint8_t macSequenceNumber,
                                                                             EmberGpSecurityLevel gpdfSecurityLevel,
                                                                             EmberGpSecurityFrameCounter gpdSecurityFrameCounter,
                                                                             uint8_t gpdCommandId,
                                                                             uint8_t gpdCommandPayloadLength,
                                                                             const uint8_t *gpdCommandPayload,
                                                                             EmberNodeId gppShortAddress,
                                                                             uint8_t gppLink,
                                                                             EmberGpMic mic)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;
  uint32_t securityFrameCounter = 0;

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                                          | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_COMMISSIONING_NOTIFICATION_COMMAND_ID,
                                         "v",
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  if (gpdfSecurityLevel == 0 ) {
    securityFrameCounter = (uint32_t)macSequenceNumber;
  } else {
    securityFrameCounter = gpdSecurityFrameCounter;
  }
  emberAfPutInt32uInResp(securityFrameCounter);
  emberAfPutInt8uInResp(gpdCommandId);
  emberAfPutInt8uInResp(gpdCommandPayloadLength);
  emberAfPutBlockInResp(gpdCommandPayload, gpdCommandPayloadLength);
  charCount += 3 + gpdCommandPayloadLength + 1;
  if (options & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_PROXY_INFO_PRESENT) {
    emberAfPutInt16uInResp(gppShortAddress);
    emberAfPutInt8uInResp(gppLink);
    charCount += 3;
  }

  if (options & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_PROCESSING_FAILED) {
    emberAfPutInt32uInResp(mic);
    charCount += 4;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpTranslationTableUpdateSmart(uint16_t options,
                                                                          uint32_t gpdSrcId,
                                                                          uint8_t* gpdIeee,
                                                                          uint8_t endpoint,
                                                                          uint8_t translationsLen,
                                                                          GpTranslationTableUpdateTranslation* translations)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;
  uint8_t i;
  if (((appId != EMBER_GP_APPLICATION_SOURCE_ID)
       && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS))
      || (translations == NULL)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND               \
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),     \
                                         ZCL_GREEN_POWER_CLUSTER_ID,                 \
                                         ZCL_GP_TRANSLATION_TABLE_UPDATE_COMMAND_ID, \
                                         "v",                                        \
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  for (i = 0; i < translationsLen; i++) {
    emberAfPutInt8uInResp(translations[i].index);
    emberAfPutInt8uInResp(translations[i].gpdCommandId);
    emberAfPutInt8uInResp(translations[i].endpoint);
    emberAfPutInt16uInResp(translations[i].profile);
    emberAfPutInt16uInResp(translations[i].cluster);
    emberAfPutInt8uInResp(translations[i].zigbeeCommandId);
    emberAfPutStringInResp(translations[i].zigbeeCommandPayload);
    charCount += 8 + emberAfStringLength(translations[i].zigbeeCommandPayload) + 1;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpPairingConfigurationSmart(uint8_t actions,
                                                                        uint16_t options,
                                                                        uint32_t gpdSrcId,
                                                                        uint8_t* gpdIeee,
                                                                        uint8_t endpoint,
                                                                        uint8_t deviceId,
                                                                        uint8_t groupListCount,
                                                                        uint8_t* groupList,
                                                                        uint16_t gpdAssignedAlias,
                                                                        uint8_t forwardingRadius,
                                                                        uint8_t securityOptions,
                                                                        uint32_t gpdSecurityFrameCounter,
                                                                        uint8_t* gpdSecurityKey,
                                                                        uint8_t numberOfPairedEndpoints,
                                                                        uint8_t* pairedEndpoints,
                                                                        uint8_t applicationInformation,
                                                                        uint16_t manufacturerId,
                                                                        uint16_t modeId,
                                                                        uint8_t numberOfGpdCommands,
                                                                        uint8_t * gpdCommandIdList,
                                                                        uint8_t clusterIdListCount,
                                                                        uint16_t * clusterListServer,
                                                                        uint16_t * clusterListClient)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND            \
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),  \
                                         ZCL_GREEN_POWER_CLUSTER_ID,              \
                                         ZCL_GP_PAIRING_CONFIGURATION_COMMAND_ID, \
                                         "uv",                                    \
                                         actions,
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  emberAfPutInt8uInResp(deviceId);
  emberAfPutBlockInResp(groupList, groupListCount);
  charCount += 1 + groupListCount;

  if (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_ASSIGNED_ALIAS) {
    emberAfPutInt16uInResp(gpdAssignedAlias);
    charCount += 2;
  }

  emberAfPutInt8uInResp(forwardingRadius);
  charCount += 1;

  if (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE) {
    emberAfPutInt8uInResp(securityOptions);
    emberAfPutInt32uInResp(gpdSecurityFrameCounter);
    emberAfPutBlockInResp(gpdSecurityKey, EUI64_SIZE);
    charCount += 3 + EUI64_SIZE;
  }

  emberAfPutInt8uInResp(numberOfPairedEndpoints);
  emberAfPutBlockInResp(pairedEndpoints, numberOfPairedEndpoints);
  charCount += 1 + numberOfPairedEndpoints;

  if (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_APPLICATION_INFORMATION_PRESENT) {
    emberAfPutInt8uInResp(applicationInformation);
    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_MANUFACTURE_ID_PRESENT) {
      emberAfPutInt16uInResp(manufacturerId);
      charCount += 2;
    }

    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_MODEL_ID_PRESENT) {
      emberAfPutInt16uInResp(modeId);
      charCount += 2;
    }

    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_COMMANDS_PRESENT) {
      emberAfPutInt8uInResp(numberOfGpdCommands);
      emberAfPutBlockInResp(gpdCommandIdList, numberOfGpdCommands);
      charCount += 1 + numberOfGpdCommands;
    }

    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_CLUSTER_LIST_PRESENT) {
      emberAfPutInt8uInResp(clusterIdListCount);
      emberAfPutBlockInResp((uint8_t*)clusterListServer, 2 * clusterIdListCount);
      emberAfPutBlockInResp((uint8_t*)clusterListClient, 2 * clusterIdListCount);
      charCount += 1 + 2 * clusterIdListCount * 2;
    }
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpSinkTableRequestSmart(uint8_t options,
                                                                    uint32_t gpdSrcId,
                                                                    uint8_t* gpdIeee,
                                                                    uint8_t endpoint,
                                                                    uint8_t index)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER), \
                                         ZCL_GREEN_POWER_CLUSTER_ID,             \
                                         ZCL_GP_SINK_TABLE_REQUEST_COMMAND_ID,   \
                                         "uw",                                   \
                                         options,                                \
                                         gpdSrcId);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(index);
    charCount += EUI64_SIZE + 1;
  }

  if ((options & EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TYPE)
      == EMBER_ZCL_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TABLE_ENTRIES_BY_INDEX) {
    emberAfPutInt8uInResp(index);
    charCount++;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpProxyTableResponseSmart(uint8_t status,
                                                                      uint8_t totalNumberOfNonEmptyProxyTableEntries,
                                                                      uint8_t startIndex,
                                                                      uint8_t entriesCount,
                                                                      uint8_t* proxyTableEntries)
{
  uint16_t charCount = 0;
  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                                          | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_PROXY_TABLE_RESPONSE_COMMAND_ID,
                                         "uuuu",
                                         status,
                                         totalNumberOfNonEmptyProxyTableEntries,
                                         startIndex,
                                         entriesCount);

  // FIXME: need to add proper fields to EmberGpProxyTableEntry / append.
  emberAfPutBlockInResp(proxyTableEntries, entriesCount);

  return charCount;
}

uint32_t emberAfFillCommandGreenPowerClusterGpNotificationResponseSmart(uint8_t options,
                                                                        uint32_t gpdSrcId,
                                                                        uint8_t* gpdIeee,
                                                                        uint8_t endpoint,
                                                                        uint32_t gpdSecurityFrameCounter)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_NOTIFICATION_RESPONSE_COMMAND_ID,
                                         "u",
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }
  emberAfPutInt32uInResp(gpdSecurityFrameCounter);
  charCount += 4;

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpPairingSmart(uint32_t options,
                                                           uint32_t gpdSrcId,
                                                           uint8_t* gpdIeee,
                                                           uint8_t endpoint,
                                                           uint8_t* sinkIeeeAddress,
                                                           uint16_t sinkNwkAddress,
                                                           uint16_t sinkGroupId,
                                                           uint8_t deviceId,
                                                           uint32_t gpdSecurityFrameCounter,
                                                           uint8_t* gpdKey,
                                                           uint16_t assignedAlias,
                                                           uint8_t forwardingRadius)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;
  uint8_t commMode = emberAfGreenPowerPairingOptionsGetCommMode(options);

  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_PAIRING_COMMAND_ID,
                                         "x",
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  if ((options & EMBER_AF_GP_PAIRING_OPTION_REMOVE_GPD) == 0x00) {
    if ((commMode == 0x00) || (commMode == 0x03)) {
      emberAfPutBlockInResp(sinkIeeeAddress, EUI64_SIZE);
      emberAfPutInt16uInResp(sinkNwkAddress);
      charCount += EUI64_SIZE + 2;
    } else if (commMode == 0x01 || commMode == 0x02) {
      emberAfPutInt16uInResp(sinkGroupId);
      charCount += 2;
    }
  }

  if (emberAfGreenPowerPairingOptionsGetAddSink(options)) {
    emberAfPutInt8uInResp(deviceId);
    charCount++;
  }
  if (options & EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT) {
    emberAfPutInt32uInResp(gpdSecurityFrameCounter);
    charCount += 4;
  }
  if (options & EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT) {
    emberAfPutBlockInResp(gpdKey, EMBER_ENCRYPTION_KEY_SIZE);
    charCount += EMBER_ENCRYPTION_KEY_SIZE;
  }
  if (options & EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT) {
    emberAfPutInt16uInResp(assignedAlias);
    charCount += 2;
  }
  if (options & EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT) {
    emberAfPutInt8uInResp(forwardingRadius);
    charCount++;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(uint8_t options,
                                                                          uint16_t commissioningWindow,
                                                                          uint8_t channel)
{
  // docs-14-0563-08: "In the current version of the GP specification,
  // the Channel present sub-field SHALL always be set to 0b0 and
  // the Channel field SHALL NOT be present."
  uint16_t charCount = 0;

  options &= ~EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_CHANNEL_PRESENT;
  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                                          | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_PROXY_COMMISSIONING_MODE_COMMAND_ID,
                                         "u",
                                         options);

  if (options & EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION) {
    emberAfPutInt16uInResp(commissioningWindow);
    charCount += 2;
  }

  if (options & EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_CHANNEL_PRESENT) {
    emberAfPutInt8uInResp(channel);
    charCount++;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpResponseSmart(uint8_t  options,
                                                            uint16_t tempMasterShortAddress,
                                                            uint8_t  tempMasterTxChannel,
                                                            uint32_t gpdSrcId,
                                                            uint8_t* gpdIeee,
                                                            uint8_t  endpoint,
                                                            uint8_t  gpdCommandId,
                                                            uint8_t gpdCommandPayloadLength,
                                                            uint8_t* gpdCommandPayload)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint16_t charCount = 0;
  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_RESPONSE_COMMAND_ID,
                                         "uvu",
                                         options,
                                         tempMasterShortAddress,
                                         tempMasterTxChannel);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    charCount += EUI64_SIZE;
    emberAfPutInt8uInResp(endpoint);
    charCount++;
  }

  emberAfPutInt8uInResp(gpdCommandId);
  emberAfPutInt8uInResp(gpdCommandPayloadLength);
  emberAfPutBlockInResp(gpdCommandPayload, gpdCommandPayloadLength);
  charCount += gpdCommandPayloadLength + 1;

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(uint8_t status,
                                                                     uint8_t totalNumberofNonEmptySinkTableEntries,
                                                                     uint8_t startIndex,
                                                                     uint8_t sinkTableEntriesCount,
                                                                     uint8_t *sinkTableEntries)
{
  uint16_t charCount = 0;

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND           \
                                          | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT), \
                                         ZCL_GREEN_POWER_CLUSTER_ID,             \
                                         ZCL_GP_SINK_TABLE_RESPONSE_COMMAND_ID,  \
                                         "uuuu",                                 \
                                         status,                                 \
                                         totalNumberofNonEmptySinkTableEntries,  \
                                         startIndex,                             \
                                         sinkTableEntriesCount);

  // FIXME: we need to define a type for tokTypeStackGpSinkTableEntry
  // emberAfPutBlockInResp(sinkTableEntry, entriesCount);
  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpProxyTableRequestSmart(uint8_t options,
                                                                     uint32_t gpdSrcId,
                                                                     uint8_t* gpdIeee,
                                                                     uint8_t endpoint,
                                                                     uint8_t index)
{
  EmberGpApplicationId appId = emberAfGreenPowerGetApplicationId(options);
  uint8_t requestType = (options & EMBER_AF_GP_PROXY_TABLE_REQUEST_OPTIONS_REQUEST_TYPE) >> 3;
  uint16_t charCount = 0;
  if ((appId != EMBER_GP_APPLICATION_SOURCE_ID)
      && (appId != EMBER_GP_APPLICATION_IEEE_ADDRESS)) {
    return 0;
  }

  charCount += emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                                          | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT),
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_PROXY_TABLE_REQUEST_COMMAND_ID,
                                         "u",
                                         options);

  if (appId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfPutInt32uInResp(gpdSrcId);
    charCount += 4;
  } else if (appId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfPutBlockInResp(gpdIeee, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    charCount += EUI64_SIZE + 1;
  }

  if (requestType == 0x01) {
    emberAfPutInt8uInResp(endpoint);
    charCount++;
  }

  return charCount;
}

uint16_t emberAfFillCommandGreenPowerClusterGpSinkCommissioningModeSmart(uint8_t options,
                                                                         uint16_t gpmAddrForSecurity,
                                                                         uint16_t gpmAddrForPairing,
                                                                         uint8_t sinkEndpoint)
{
  return emberAfFillCommandGreenPowerClusterGpSinkCommissioningMode(options,
                                                                    gpmAddrForSecurity,
                                                                    gpmAddrForPairing,
                                                                    sinkEndpoint);
}

/*
 * Comparing two GP address.
 *
 * return - true if addresses are the same. otherwise, false.
 *
 * NOTE: Endpoint field is ignored.
 *
 */
bool emberAfGreenPowerCommonGpAddrCompare(const EmberGpAddress * a,
                                          const EmberGpAddress * b)
{
  if (a->applicationId != b->applicationId) {
    return false;
  }

  if (a->applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
    return a->id.sourceId == b->id.sourceId;
  } else if (a->applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
    return MEMCOMPARE(a->id.gpdIeeeAddress, b->id.gpdIeeeAddress, EUI64_SIZE);
  }

  return false;
}
