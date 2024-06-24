/**************************************************************************//**
 * Copyright 2018 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef OTA_SERVER_DYNAMIC_BLOCK_PERIOD_H
#define OTA_SERVER_DYNAMIC_BLOCK_PERIOD_H

#ifdef EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
void    emAfOtaServerDynamicBlockPeriodInit();
void    emAfOtaServerDynamicBlockPeriodTick();
uint8_t emAfOtaServerCheckDynamicBlockPeriodDownload(EmberAfImageBlockRequestCallbackStruct *data);
void    emAfOtaServerCompleteDynamicBlockPeriodDownload(EmberNodeId clientId);
bool    emAfOtaServerDynamicBlockPeriodClientUsesSeconds(EmberNodeId clientId);
void    emAfOtaServerReadAttributesResponseCallback(EmberNodeId nodeId,
                                                    uint8_t * buffer,
                                                    uint16_t bufLen);
#else // EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
// Stubs
  #define emAfOtaServerDynamicBlockPeriodInit()
  #define emAfOtaServerDynamicBlockPeriodTick()
  #define emAfOtaServerCheckDynamicBlockPeriodDownload(arg) \
  EMBER_ZCL_STATUS_SUCCESS
  #define emAfOtaServerCompleteDynamicBlockPeriodDownload(arg)
  #define emAfOtaServerDynamicBlockPeriodClientUsesSeconds(arg) false
  #define emAfOtaServerReadAttributesResponseCallback(args)
#endif // EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT

#endif // OTA_SERVER_DYNAMIC_BLOCK_PERIOD_H
