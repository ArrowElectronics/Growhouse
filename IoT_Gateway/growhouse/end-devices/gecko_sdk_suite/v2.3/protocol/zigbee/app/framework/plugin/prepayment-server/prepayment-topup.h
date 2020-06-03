#ifndef SILABS_PREPAYMENT_TOP_UP_H
#define SILABS_PREPAYMENT_TOP_UP_H

/**
 * @brief Formats top up payload data needed by the Publish Top Up Log command.
 * @param nodeId The address of the destination node to whom the Publish Top Up
 * Log command should be sent.
 * @param srcEndpoint The endpoint of the sender that supports the prepayment cluster.
 * @param dstEndpoint The endpoint on the destination node that supports the prepayment client.
 * @param latestEndTime The latest top up time of top up records to be returned in the
 * Publish Top Up Log command(s).
 * @param numberOfRecords The maximum number of records to return in the Publish
 * Top Up Log command(s).
 *
 **/
void emberAfPluginSendPublishTopUpLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                      uint32_t latestEndTime, uint8_t numberOfRecords);

/**
 * @brief Sends a Publish Top Up Log command.
 * @param nodeId The address of the destination node to whom the Publish Top Up
 * Log command should be sent.
 * @param srcEndpoint The endpoint of the sender that supports the prepayment cluster.
 * @param dstEndpoint The endpoint on the destination node that supports the prepayment client.
 * @param commandIndex Indicates which payload fragment is being sent.  Set to 0xFE
 * if this is the last expected command.
 * @param totalNumberOfCommands If the payload does not fit into 1 command,
 * this indicates the total number of sub-commands in the message.
 * @param topUpPayload An array of Top Up Payload structures, containing the payload
 * that should be included in the Publish Top Up Log command.
 *
 **/
void emberAfPluginPrepaymentServerPublishTopUpLog(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                  uint8_t commandIndex, uint8_t totalNumberOfCommands,
                                                  TopUpPayload *topUpPayload);

#endif  // #ifndef _PREPAYMENT_TOP_UP_H_
