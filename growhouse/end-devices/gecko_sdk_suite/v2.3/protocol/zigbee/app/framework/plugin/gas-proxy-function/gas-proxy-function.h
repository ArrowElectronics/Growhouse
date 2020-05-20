// *******************************************************************
// * gas-proxy-function.h
// *
// *
// * Copyright 2014 by Silicon Laboratories, Inc. All rights reserved.      *80*
// *******************************************************************

#ifndef SILABS_GAS_PROXY_FUNCTION_H
#define SILABS_GAS_PROXY_FUNCTION_H

#define emberAfPluginGasProxyFunctionPrint(...)                          emberAfAppPrint(__VA_ARGS__)
#define emberAfPluginGasProxyFunctionPrintln(...)                        emberAfAppPrintln(__VA_ARGS__)
#define emberAfPluginGasProxyFunctionDebugExec(x)                        emberAfAppDebugExec(x)
#define emberAfPluginGasProxyFunctionPrintBuffer(buffer, len, withSpace) emberAfAppPrintBuffer(buffer, len, withSpace)
#define emberAfPluginGasProxyFunctionPrintString(buffer)                 emberAfPrintString(EMBER_AF_PRINT_APP, (buffer))

EmberStatus emberAfPluginGasProxyFunctionNonTapOffMessageHandler(uint8_t * gbzCommands,
                                                                 uint16_t  gbzCommandsLength,
                                                                 uint16_t  messageCode);

EmberStatus emberAfPluginGasProxyFunctionTapOffMessageHandler(uint8_t * gbzCommand,
                                                              uint16_t  gbzCommandLength,
                                                              uint8_t * gbzCommandResponse,
                                                              uint16_t  gbzCommandResponseLength,
                                                              uint16_t  messageCode);
typedef enum {
  EMBER_AF_GPF_ZCL_COMMAND_PERMISSION_ALLOWED,
  EMBER_AF_GPF_ZCL_COMMAND_PERMISSION_NOT_ALLOWED,
  EMBER_AF_GPF_ZCL_COMMAND_PERMISSION_IGNORED,
} EmberAfGpfZclCommandPermission;

typedef enum {
  EMBER_AF_GPF_MESSAGE_TYPE_TOM,
  EMBER_AF_GPF_MESSAGE_TYPE_NON_TOM,
} EmberAfGPFMessageType;

uint32_t emAfGasProxyFunctionGetGbzStartTime(void);
void emAfGasProxyFunctionAlert(uint16_t alertCode,
                               EmberAfClusterCommand *cmd,
                               uint16_t messageCode);
/*
 * @brief Ignores future commands.
 * Per GBCS v0.8.1, Section 10.3.4.2 Step 12/13. It is inferred that per
 * elemental ZCL command, future commands ( startTime greater than now ) will
 * be discarded and the application will be notified.
 *
 * @return true - if we shall ignore future dated current command, false - otherwise.
 */
bool emberAfPluginGasProxyFunctionIgnoreFutureCommand(uint32_t startTime);

#endif // #ifndef _GAS_PROXY_FUNCTION_H_
