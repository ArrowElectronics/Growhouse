// *******************************************************************
// * messaging-server.c
// *
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "../../include/af.h"
#include "messaging-server.h"

// The internal message is stored in the same structure type that is defined
// publicly.  The only exception is that the reserved bits of the message
// control are used to track some internal state.  These are cleaned up when
// the message is passed back to the application through the APIs.
static EmberAfPluginMessagingServerMessage msgTable[EMBER_AF_MESSAGING_CLUSTER_SERVER_ENDPOINT_COUNT];

// Bits 4, 5, and 6 are reserved in the message control field.  These are used
// internally to represent whether the message is valid, active, or is a "start
// now" message.
#define VALID  BIT(4)
#define ACTIVE BIT(5)
#define NOW    BIT(6)

#define messageIsValid(ep)   (msgTable[ep].messageControl & VALID)
#define messageIsActive(ep)  (msgTable[ep].messageControl & ACTIVE)
#define messageIsNow(ep)     (msgTable[ep].messageControl & NOW)
#define messageIsForever(ep) (msgTable[ep].durationInMinutes == ZCL_MESSAGING_CLUSTER_DURATION_UNTIL_CHANGED)
static bool messageIsCurrentOrScheduled(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return false;
  }

  return (messageIsValid(ep)
          && messageIsActive(ep)
          && (messageIsForever(ep)
              || (emberAfGetCurrentTime()
                  < msgTable[ep].startTime + (uint32_t)msgTable[ep].durationInMinutes * 60)));
}

void emberAfMessagingClusterServerInitCallback(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  msgTable[ep].messageControl &= ~VALID;
}

bool emberAfMessagingClusterGetLastMessageCallback(void)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  EmberAfPluginMessagingServerMessage message;
  emberAfMessagingClusterPrintln("RX: GetLastMessage");
  if (emberAfPluginMessagingServerGetMessage(endpoint, &message)) {
    emberAfFillCommandMessagingClusterDisplayMessage(message.messageId,
                                                     message.messageControl,
                                                     message.startTime,
                                                     message.durationInMinutes,
                                                     message.message,
                                                     message.extendedMessageControl);
    emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
    emberAfSendResponse();
  } else {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_NOT_FOUND);
  }
  return true;
}

#if defined(EMBER_AF_HAS_SPEC_VERSIONS_SE_1_0)   \
  || defined(EMBER_AF_HAS_SPEC_VERSIONS_SE_1_1B) \
  || defined(EMBER_AF_HAS_SPEC_VERSIONS_SE_1_0)  \
  || defined(EMBER_AF_HAS_SPEC_VERSIONS_SE_1_1)  \
  || defined(EMBER_AF_HAS_SPEC_VERSIONS_SE_1_1A)
bool emberAfMessagingClusterMessageConfirmationCallback(uint32_t messageId,
                                                        uint32_t confirmationTime)
#else
bool emberAfMessagingClusterMessageConfirmationCallback(uint32_t messageId,
                                                        uint32_t confirmationTime,
                                                        uint8_t messageConfirmationControl,
                                                        uint8_t *messageResponse)
#endif
{
  emberAfMessagingClusterPrintln("RX: MessageConfirmation 0x%4x, 0x%4x",
                                 messageId,
                                 confirmationTime);
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPluginMessagingServerGetMessage(uint8_t endpoint,
                                            EmberAfPluginMessagingServerMessage *message)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return false;
  }

  MEMMOVE(message, &msgTable[ep], sizeof(EmberAfPluginMessagingServerMessage));

  // Clear out our internal bits from the message control.
  message->messageControl &= ~ZCL_MESSAGING_CLUSTER_RESERVED_MASK;

  // If the message is expired or it has an absolute time, set the start time
  // and duration to the original start time and duration.  For "start now"
  // messages that are current or scheduled, set the start time to the special
  // value for "now" and set the duration to the remaining time, if it is not
  // already the special value for "until changed."
  if (messageIsCurrentOrScheduled(endpoint) && messageIsNow(ep)) {
    message->startTime = ZCL_MESSAGING_CLUSTER_START_TIME_NOW;
    if (!messageIsForever(ep)) {
      message->durationInMinutes -= ((emberAfGetCurrentTime() - msgTable[ep].startTime)
                                     / 60);
    }
  }
  return messageIsCurrentOrScheduled(endpoint);
}

void emberAfPluginMessagingServerSetMessage(uint8_t endpoint,
                                            const EmberAfPluginMessagingServerMessage *message)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  if (message == NULL) {
    msgTable[ep].messageControl &= ~ACTIVE;
    return;
  }

  MEMMOVE(&msgTable[ep], message, sizeof(EmberAfPluginMessagingServerMessage));

  // Rember if this is a "start now" message, but store the start time as the
  // current time so the duration can be adjusted.
  if (msgTable[ep].startTime == ZCL_MESSAGING_CLUSTER_START_TIME_NOW) {
    msgTable[ep].messageControl |= NOW;
    msgTable[ep].startTime = emberAfGetCurrentTime();
  } else {
    msgTable[ep].messageControl &= ~NOW;
  }

  msgTable[ep].messageControl |= (VALID | ACTIVE);
}

void emAfPluginMessagingServerPrintInfo(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterServerEndpointIndex(endpoint, ZCL_MESSAGING_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  emberAfMessagingClusterPrintln("= Server Message =");
  emberAfMessagingClusterFlush();

  emberAfMessagingClusterPrintln(" vld: %p", (messageIsValid(ep) ? "YES" : "NO"));
  emberAfMessagingClusterPrintln(" act: %p", (messageIsCurrentOrScheduled(endpoint)
                                              ? "YES"
                                              : "NO"));
  emberAfMessagingClusterPrintln("  id: 0x%4x", msgTable[ep].messageId);
  emberAfMessagingClusterPrintln("  mc: 0x%x",
                                 (msgTable[ep].messageControl
                                  & ~ZCL_MESSAGING_CLUSTER_RESERVED_MASK));
  emberAfMessagingClusterPrintln("  st: 0x%4x", msgTable[ep].startTime);
  emberAfMessagingClusterPrintln(" now: %p", (messageIsNow(ep) ? "YES" : "NO"));
  emberAfMessagingClusterPrintln("time: 0x%4x", emberAfGetCurrentTime());
  emberAfMessagingClusterPrintln(" dur: 0x%2x", msgTable[ep].durationInMinutes);
  emberAfMessagingClusterFlush();
  emberAfMessagingClusterPrint(" mes: \"");
  emberAfMessagingClusterPrintString(msgTable[ep].message);
  emberAfMessagingClusterPrintln("\"");
  emberAfMessagingClusterFlush();
}

void emberAfPluginMessagingServerDisplayMessage(EmberNodeId nodeId,
                                                uint8_t srcEndpoint,
                                                uint8_t dstEndpoint)
{
  EmberStatus status;
  EmberAfPluginMessagingServerMessage message;
  if (!emberAfPluginMessagingServerGetMessage(srcEndpoint, &message)) {
    emberAfMessagingClusterPrintln("invalid msg");
    return;
  }

  emberAfFillCommandMessagingClusterDisplayMessage(message.messageId,
                                                   message.messageControl,
                                                   message.startTime,
                                                   message.durationInMinutes,
                                                   message.message,
                                                   message.extendedMessageControl);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  if (status != EMBER_SUCCESS) {
    emberAfMessagingClusterPrintln("Error in display %x", status);
  }
}

void emberAfPluginMessagingServerCancelMessage(EmberNodeId nodeId,
                                               uint8_t srcEndpoint,
                                               uint8_t dstEndpoint)
{
  EmberStatus status;
  EmberAfPluginMessagingServerMessage message;

  // Nullify the current message before sending the cancellation.
  emberAfPluginMessagingServerSetMessage(srcEndpoint, NULL);

  // Then send the response
  emberAfPluginMessagingServerGetMessage(srcEndpoint, &message);

  emberAfFillCommandMessagingClusterCancelMessage(message.messageId,
                                                  message.messageControl);
  emberAfSetCommandEndpoints(srcEndpoint, dstEndpoint);
  emberAfGetCommandApsFrame()->options |= EMBER_APS_OPTION_SOURCE_EUI64;
  status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
  if (status != EMBER_SUCCESS) {
    emberAfMessagingClusterPrintln("Error in cancel %x", status);
  }
}
