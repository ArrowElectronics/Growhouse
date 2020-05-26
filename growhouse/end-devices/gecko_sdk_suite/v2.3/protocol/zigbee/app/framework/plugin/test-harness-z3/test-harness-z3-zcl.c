//
// test-harness-z3-zcl.c
//
// August 3, 2015
// Refactored November 23, 2015
//
// ZigBee 3.0 zcl test harness functionality
//

#include "app/framework/include/af.h"

#include "test-harness-z3-core.h"

// -----------------------------------------------------------------------------
// ZCL CLI commands

// plugin test-harness z3 read <cluster:2> <attribute:2> <dstShort:2>
// <srcEndpoint:1> <dstEndpoint:1> <profileId:2>
void emAfPluginTestHarnessZ3ZclReadCommand(void)
{
  EmberStatus status = EMBER_INVALID_CALL;

#ifndef EZSP_HOST

  uint16_t clusterId      = (uint16_t)emberUnsignedCommandArgument(0);
  uint16_t attributeId    = (uint16_t)emberUnsignedCommandArgument(1);
  EmberNodeId destination = (EmberNodeId)emberUnsignedCommandArgument(2);
  uint8_t srcEndpoint     = (uint8_t)emberUnsignedCommandArgument(3);
  uint8_t dstEndpoint     = (uint8_t)emberUnsignedCommandArgument(4);
  uint16_t profileId      = (uint16_t)emberUnsignedCommandArgument(5);
  uint8_t frame[5];
  EmberMessageBuffer message;
  EmberApsFrame apsFrame;

  // Frame control
  // global command, client to server
  frame[0] = (ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER);
  // sequence, whatever
  frame[1] = 0xAC;
  // command id: read attributes
  frame[2] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
  // payload: attribute id
  frame[3] = LOW_BYTE(attributeId);
  frame[4] = HIGH_BYTE(attributeId);

  apsFrame.profileId = profileId;
  apsFrame.clusterId = clusterId;
  apsFrame.sourceEndpoint = srcEndpoint;
  apsFrame.destinationEndpoint = dstEndpoint;
  apsFrame.options = (EMBER_APS_OPTION_RETRY);

  message = emberFillLinkedBuffers(frame, sizeof(frame));

  if (message == EMBER_NULL_MESSAGE_BUFFER) {
    status = EMBER_NO_BUFFERS;
  } else {
    status = emberSendUnicast(EMBER_OUTGOING_DIRECT,
                              destination,
                              &apsFrame,
                              message);
  }

  emberReleaseMessageBuffer(message);

#endif /* EZSP_HOST */

  emberAfCorePrintln("%p: %p: 0x%X",
                     TEST_HARNESS_Z3_PRINT_NAME,
                     "Read",
                     status);
}
