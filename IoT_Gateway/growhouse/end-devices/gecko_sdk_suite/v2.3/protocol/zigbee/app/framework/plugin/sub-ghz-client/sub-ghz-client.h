// *******************************************************************
// * sub-ghz-client.h
// *
// *
// * Copyright 2017 by Silicon Labs. All rights reserved.
// *******************************************************************

#ifndef SUB_GHZ_CLIENT_H_INCLUDED
#define SUB_GHZ_CLIENT_H_INCLUDED

/** @brief Sends the 'Get Suspend ZCL Messages Status' command.
 *
 * This is a client command. The client sends it to determine the current status
 * of its ZCL communications from the server.
 *
 * @param nodeId The server's node ID.
 * @param endpoint The server's endpoint.
 */
EmberStatus emberAfPluginSubGhzClientSendGetSuspendZclMessagesStatusCommand(EmberNodeId nodeId,
                                                                            uint8_t endpoint);

/** @brief Sends a request to change the channel to the server by
 * Mgmt_NWK_Unsolicited_Enhanced_Update_notify command.
 *
 * @param destination ........ the Sub-GHz server's node ID
 * @param channelPage ........ current channel page
 * @param channel ............ current channel
 *                             - the application needs to keep track of these values
 * @param macTxUcastTotal .... total number of Mac Tx transaction attempts
 * @param macTxUcastFailures . total number of Mac Tx transaction failures
 * @param macTxUcastRetries .. total number of Mac Tx transaction retries
 * @param period ............. time in minutes over which macTxUcastXxx were measured
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS
 * - ::EMBER_BAD_ARGUMENT if the channel page or channel are not in the valid range
 * - ::EMBER_NO_BUFFERS
 * - ::EMBER_NETWORK_DOWN
 * - ::EMBER_NETWORK_BUSY
 */
EmberStatus emberAfPluginSubGhzClientSendUnsolicitedEnhancedUpdateNotify(EmberNodeId destination,
                                                                         uint8_t channelPage,
                                                                         uint8_t channel,
                                                                         uint16_t macTxUcastTotal,
                                                                         uint16_t macTxUcastFailures,
                                                                         uint16_t macTxUcastRetries,
                                                                         uint8_t period);

/** @brief Returns the current suspend status.
 *
 * @return  True if called within a suspend period, false otherwise.
 */
boolean emberAfPluginSubGhzClientIsSendingZclMessagesSuspended();

/** @brief Ignores the SuspendZclMessages command.
 *
 * When the client receives the SuspendZclMessages command, it suspends sending
 * any ZCL messages for a given time. This function allows the client to ignore
 * the command and continue sending messages even when suspended.
 *
 * NOTE: For testing only.
 *       Ignoring the SuspendZclMessages command in production is non-compliant.
 *
 * @param ignore  True to ignore, false to switch back to normal behavior.
 */
void emberAfPluginSubGhzClientIgnoreSuspendZclMessagesCommand(boolean ignore);

#endif // SUB_GHZ_CLIENT_H_INCLUDED
