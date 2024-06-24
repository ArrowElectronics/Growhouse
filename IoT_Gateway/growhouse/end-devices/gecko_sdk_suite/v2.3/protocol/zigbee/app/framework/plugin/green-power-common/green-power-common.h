// *******************************************************************
// * green-power-common.c
// *
// * Place for common functions / definitions shared by
// * Green Power Client/Server
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// * Jing Teng ( jing.teng@silabs.com )
// *******************************************************************

#define GP_ENDPOINT (242)
#define GP_HIDDEN_GP_ZCL_ENDPOINT (EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_ZCL_MESSAGE_PROXY_ENDPOINT)

#define GP_COMMISSIONING_SECURITY_LEVEL_TO_OPTIONS_SHIFT (4)
#define GP_COMMISSIONING_SECURITY_KEY_TYPE_TO_OPTIONS_SHIFT (6)
#define EMBER_GP_NOTIFICATION_COMMISSIONED_GROUPCAST_SEQUENCE_NUMBER_OFFSET (9)
#define EMBER_GP_COMMISSIONING_NOTIFICATION_SEQUENCE_NUMBER_OFFSET (12)
#define EMBER_GP_NOTIFICATION_BROADCAST_SEQUENCE_NUMBER_OFFSET (14)
#define GP_NOTIFICATION_OPTIONS_SECURITY_LEVEL_TO_OPTIONS_SHIFT (6)

#define EMBER_AF_GREEN_POWER_SERVER_GPDF_SCENE_DERIVED_GROUP_ID (0xFFFF)
#define EMBER_AF_GREEN_POWER_SERVER_GPDF_TRANSLATION_DEFAULT_OPTION (0x00)

#define emberAfGreenPowerPairingOptionsGetCommMode(options) (((options) & EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE) >> 5)
#define emberAfGreenPowerPairingOptionsGetAddSink(options) (((options) & EMBER_AF_GP_PAIRING_OPTION_ADD_SINK) >> 3)
#define emberAfGreenPowerGetApplicationId(options) ((EmberGpApplicationId) ((options) & 0x07))
#define emberAfGreenPowerGpdSeqNumCap(entry)      ((entry->options >> 8) & 0x01)
#define emberAfGreenPowerSecurityLevel(entry)     ((entry->options >> 9) & 0x03)
#define emberAfGreenPowerPortableCap(options)     ((((options) & GP_PAIRING_OPTIONS_GPD_FIXED) >> 7) & 0x01)
#define emberAfGreenPowerSecurityKeyType(options) ((((options) & GP_PAIRING_OPTIONS_SECURITY_KEY_TYPE) >> 11) & 0x07)

uint16_t emberAfFillCommandGreenPowerClusterGpNotificationSmart(uint16_t options,
                                                                uint32_t gpdSrcId,
                                                                uint8_t* gpdIeee,
                                                                uint8_t  gpdEndpoint,
                                                                uint32_t gpdSecurityFrameCounter,
                                                                uint8_t  gpdCommandId,
                                                                uint8_t gpdCommandPayloadLength,
                                                                const uint8_t* gpdCommandPayload,
                                                                uint16_t gppShortAddress,
                                                                uint8_t  gppDistance);

uint16_t emberAfFillCommandGreenPowerClusterGpPairingSearchSmart(uint16_t options,
                                                                 uint32_t gpdSrcId,
                                                                 uint8_t* gpdIeee,
                                                                 uint8_t endpoint);

uint16_t emberAfFillCommandGreenPowerClusterGpTunnelingStopSmart(uint8_t options,
                                                                 uint32_t gpdSrcId,
                                                                 uint8_t* gpdIeee,
                                                                 uint8_t endpoint,
                                                                 uint32_t gpdSecurityFrameCounter,
                                                                 uint16_t gppShortAddress,
                                                                 int8_t gppDistance);

uint16_t emberAfFillCommandGreenPowerClusterGpCommissioningNotificationSmart(uint16_t options,
                                                                             uint32_t gpdSrcId,
                                                                             uint8_t* gpdIeee,
                                                                             uint8_t endpoint,
                                                                             uint8_t sequenceNumber,
                                                                             EmberGpSecurityLevel gpdfSecurityLevel,
                                                                             EmberGpSecurityFrameCounter gpdSecurityFrameCounter,
                                                                             uint8_t gpdCommandId,
                                                                             uint8_t gpdCommandPayloadLength,
                                                                             const uint8_t *gpdCommandPayload,
                                                                             EmberNodeId gppShortAddress,
                                                                             uint8_t gppLink,
                                                                             EmberGpMic mic);

uint16_t emberAfFillCommandGreenPowerClusterGpTranslationTableUpdateSmart(uint16_t options,
                                                                          uint32_t gpdSrcId,
                                                                          uint8_t* gpdIeee,
                                                                          uint8_t endpoint,
                                                                          uint8_t translationsLen,
                                                                          GpTranslationTableUpdateTranslation* translations);

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
                                                                        uint16_t * clusterListClient);

uint16_t emberAfFillCommandGreenPowerClusterGpSinkTableRequestSmart(uint8_t options,
                                                                    uint32_t gpdSrcId,
                                                                    uint8_t* gpdIeee,
                                                                    uint8_t endpoint,
                                                                    uint8_t index);

uint16_t emberAfFillCommandGreenPowerClusterGpProxyTableResponseSmart(uint8_t status,
                                                                      uint8_t totalNumberOfNonEmptyProxyTableEntries,
                                                                      uint8_t startIndex,
                                                                      uint8_t entriesCount,
                                                                      uint8_t* proxyTableEntries);

uint32_t emberAfFillCommandGreenPowerClusterGpNotificationResponseSmart(uint8_t options,
                                                                        uint32_t gpdSrcId,
                                                                        uint8_t* gpdIeee,
                                                                        uint8_t endpoint,
                                                                        uint32_t gpdSecurityFrameCounter);

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
                                                           uint8_t forwardingRadius);

uint16_t emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(uint8_t options,
                                                                          uint16_t commissioningWindow,
                                                                          uint8_t channel);

uint16_t emberAfFillCommandGreenPowerClusterGpResponseSmart(uint8_t  options,
                                                            uint16_t tempMasterShortAddress,
                                                            uint8_t  tempMasterTxChannel,
                                                            uint32_t gpdSrcId,
                                                            uint8_t* gpdIeee,
                                                            uint8_t  endpoint,
                                                            uint8_t  gpdCommandId,
                                                            uint8_t gpdCommandPayloadLength,
                                                            uint8_t* gpdCommandPayload);

uint16_t emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(uint8_t status,
                                                                     uint8_t totalNumberofNonEmptySinkTableEntries,
                                                                     uint8_t startIndex,
                                                                     uint8_t sinkTableEntriesCount,
                                                                     uint8_t *sinkTableEntries);

uint16_t emberAfFillCommandGreenPowerClusterGpProxyTableRequestSmart(uint8_t options,
                                                                     uint32_t gpdSrcId,
                                                                     uint8_t* gpdIeee,
                                                                     uint8_t endpoint,
                                                                     uint8_t index);

uint16_t emberAfFillCommandGreenPowerClusterGpSinkCommissioningModeSmart(uint8_t options,
                                                                         uint16_t gpmAddrForSecurity,
                                                                         uint16_t gpmAddrForPairing,
                                                                         uint8_t sinkEndpoint);

bool emberAfGreenPowerCommonGpAddrCompare(const EmberGpAddress * a,
                                          const EmberGpAddress * b);

EmberNodeId emGpdAlias(EmberGpAddress *addr);
