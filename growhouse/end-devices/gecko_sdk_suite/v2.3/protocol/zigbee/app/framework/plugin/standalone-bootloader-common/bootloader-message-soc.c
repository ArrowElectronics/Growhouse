// *****************************************************************************
// * bootloader-message-host.c
// *
// * This file defines the interface to the host to send Ember proprietary
// * bootloader messages.
// *
// * Copyright 2012 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "bootloader-protocol.h"

//------------------------------------------------------------------------------
// External Declarations

void emAesEncrypt(uint8_t *block, const uint8_t *key);

//------------------------------------------------------------------------------
// Globals

//------------------------------------------------------------------------------
// Functions

EmberStatus emAfSendBootloadMessage(bool isBroadcast,
                                    EmberEUI64 destEui64,
                                    uint8_t length,
                                    uint8_t* message)
{
  EmberStatus status;
  EmberMessageBuffer buffer = emberFillLinkedBuffers(message,
                                                     length);
  if (buffer == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  }

  status = emberSendBootloadMessage(isBroadcast, destEui64, buffer);
  emberReleaseMessageBuffer(buffer);
  return status;
}

void emberIncomingBootloadMessageHandler(EmberEUI64 longId,
                                         EmberMessageBuffer message)
{
  uint8_t incomingBlock[MAX_BOOTLOAD_MESSAGE_SIZE];
  uint8_t length = emberMessageBufferLength(message);
  if (length > MAX_BOOTLOAD_MESSAGE_SIZE) {
    bootloadPrintln("Bootload message too long (%d > %d), dropping!",
                    length,
                    MAX_BOOTLOAD_MESSAGE_SIZE);
    return;
  }
  emberCopyFromLinkedBuffers(message,
                             0,       // start index
                             incomingBlock,
                             length);

  emberAfPluginStandaloneBootloaderCommonIncomingMessageCallback(longId,
                                                                 length,
                                                                 incomingBlock);
}

void emberBootloadTransmitCompleteHandler(EmberMessageBuffer message,
                                          EmberStatus status)
{
  if (status != EMBER_SUCCESS) {
    uint8_t commandId = 0xFF;
    if (emberMessageBufferLength(message) >= 2) {
      commandId = emberGetLinkedBuffersByte(message, 1);
    }
    bootloadPrintln("Bootload message (0x%X) send failed: 0x%X",
                    commandId,
                    status);
  }
}

void emAfStandaloneBootloaderClientEncrypt(uint8_t* block, uint8_t* key)
{
  emAesEncrypt(block, key);
}
