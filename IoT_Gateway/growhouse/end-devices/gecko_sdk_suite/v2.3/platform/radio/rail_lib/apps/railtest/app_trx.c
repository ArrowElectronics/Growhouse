/***************************************************************************//**
 * @file app_trx.c
 * @brief RAILTEST transmit and receive events
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rail.h"
#include "rail_types.h"

#include "buffer_pool_allocator.h"
#include "circular_queue.h"

#include "app_ci.h"
#include "app_common.h"
#include "app_trx.h"
#include "rail_ble.h"
/******************************************************************************
 * Variables
 *****************************************************************************/
RAIL_TxPacketDetails_t previousTxAppendedInfo = { .isAck = false, };
RAIL_TxPacketDetails_t previousTxAckAppendedInfo = { .isAck = true, };

static uint16_t dataLeft = 0;
static uint8_t *dataLeftPtr = NULL;

// Variables to keep track of the receive packet
static uint16_t rxLengthTarget;
static uint16_t rxLengthCount;
static void *rxFifoPacketHandle = 0;
static RxPacketData_t *rxFifoPacketData;
static uint8_t *currentRxFifoPacketPtr;

uint32_t abortRxDelay = 0;

// fifo mode test variables
/*
 * This test bit is used to prevent RAILTEST from automatically loading packet
 * data into transmit fifo
 */
bool txFifoManual = false;
/*
 * This test bit is used to prevent RAILTEST from automatically reading out the
 * packet from the fifo
 */
bool rxFifoManual = false;

/******************************************************************************
 * Static
 *****************************************************************************/
static void packetMode_RxPacketReceived(RAIL_Handle_t railHandle)
{
  RAIL_RxPacketInfo_t packetInfo;
  RAIL_RxPacketHandle_t packetHandle
    = RAIL_GetRxPacketInfo(railHandle, RAIL_RX_PACKET_HANDLE_NEWEST,
                           &packetInfo);
  // assert(packetHandle != NULL);
  uint16_t length = packetInfo.packetBytes;
  RxPacketData_t *rxPacket;
  void *rxPacketMemoryHandle = NULL;

  // Get a memory buffer for the received packet data
  rxPacketMemoryHandle = memoryAllocate(length + sizeof(RxPacketData_t));
  rxPacket = (RxPacketData_t *)memoryPtrFromHandle(rxPacketMemoryHandle);
  if (rxPacket != NULL) {
    // Read packet data into our packet structure
    RAIL_CopyRxPacket(rxPacket->dataPtr, &packetInfo);

    // Read the appended info into our packet structure
    rxPacket->dataLength = length;
    rxPacket->appendedInfo.timeReceived.timePosition = RAIL_PACKET_TIME_DEFAULT;
    rxPacket->appendedInfo.timeReceived.totalPacketBytes = 0;
    if (RAIL_GetRxPacketDetailsAlt(railHandle, packetHandle,
                                   &rxPacket->appendedInfo)
        != RAIL_STATUS_NO_ERROR) {
      // assert(false);
    }
    RAIL_Time_t *time = &rxPacket->appendedInfo.timeReceived.packetTime;
    if (RAIL_GetRxTimeSyncWordEnd(railHandle, 0U, time)
        != RAIL_STATUS_NO_ERROR) {
      // assert(false);
    }
  }

  if ((rxSuccessTransition == RAIL_RF_STATE_TX)
      || (RAIL_IsAutoAckEnabled(railHandle) && afterRxUseTxBufferForAck)) {
    // Load packet for either the non-AutoACK RXSUCCESS => TX transition,
    // or for the ACK transition when we intend to use the TX buffer
    // We don't do this in other circumstances in case of an RX2TX
    // transition to send a packet that's already been loaded,
    // which could cause TX underflow if we were to disturb it.
    RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
  }

  // Count packets that we received but had no memory to store
  if (rxPacket == NULL) {
    counters.noRxBuffer++;
  } else {
    // If we have just received an ACK, don't respond with an ACK
    if (rxPacket->dataPtr[2] == 0xF1) {
      RAIL_CancelAutoAck(railHandle);
    }

    // Cancel ack if user requested
    if (afterRxCancelAck) {
      afterRxCancelAck = false;
      RAIL_CancelAutoAck(railHandle);
    }

    // Use Tx Buffer for Ack if user requested
    if (afterRxUseTxBufferForAck) {
      afterRxUseTxBufferForAck = false;
      RAIL_UseTxFifoForAutoAck(railHandle);
    }

    if (currentAppMode() == SCHTX_AFTER_RX) {
      // Schedule the next transmit after this receive
      RAIL_ScheduleTxConfig_t scheduledTxOptions = {
        .when = rxPacket->appendedInfo.timeReceived.packetTime
                + txAfterRxDelay,
        .mode = RAIL_TIME_ABSOLUTE,
        .txDuringRx = RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX
      };

      setNextPacketTime(&scheduledTxOptions);

      txCount = 1;
      pendPacketTx();
    }

    if (logLevel & ASYNC_RESPONSE) {
      redrawDisplay = true;

      // Take an extra reference to this rx packet pointer so it's not released
      memoryTakeReference(rxPacketMemoryHandle);

      // Copy this received packet into our circular queue
      queueAdd(&rxPacketQueue, rxPacketMemoryHandle);
    }

    updateStats(rxPacket->appendedInfo.rssi, &counters.rssi);
  }

  // Track the state of scheduled Rx to figure out when it ends
  if (inAppMode(RX_SCHEDULED, NULL) && schRxStopOnRxEvent) {
    enableAppMode(RX_SCHEDULED, false, NULL);
  }

  // In Rx overflow test mode hang in this ISR to prevent processing new
  // packets to force an overflow
  if ((currentAppMode() == RX_OVERFLOW)) {
    enableAppMode(RX_OVERFLOW, false, NULL); // Switch back after the overflow
    changeAppModeIfPending();
    // Trigger an overflow by waiting in the interrupt handler
    usDelay(rxOverflowDelay);
  }

  // Free the allocated memory now that we're done with it
  memoryFree(rxPacketMemoryHandle);
}

// Only support fixed length
static void fifoMode_RxPacketReceived(void)
{
  uint16_t bytesRead;
  // Discard incoming data stream in BER mode.
  if (currentAppMode() == BER) {
    RAIL_ResetFifo(railHandle, true, true);
  } else {
    if (rxLengthCount > 0) {
      // Read the rest of the bytes out of the fifo
      bytesRead = RAIL_ReadRxFifo(railHandle, currentRxFifoPacketPtr, rxLengthCount);
      rxLengthCount -= bytesRead;
      currentRxFifoPacketPtr += bytesRead;
    }
    // Configure how many bytes were received
    rxFifoPacketData->dataLength = rxLengthTarget;

    // Get the appended info details
    rxFifoPacketData->appendedInfo.timeReceived.timePosition
      = RAIL_PACKET_TIME_DEFAULT;
    rxFifoPacketData->appendedInfo.timeReceived.totalPacketBytes = 0;
    RAIL_GetRxPacketDetailsAlt(railHandle, RAIL_RX_PACKET_HANDLE_OLDEST,
                               &(rxFifoPacketData->appendedInfo));
    RAIL_Time_t *time = &rxFifoPacketData->appendedInfo.timeReceived.packetTime;
    RAIL_GetRxTimeSyncWordEnd(railHandle, 0U, time);
    queueAdd(&rxPacketQueue, rxFifoPacketHandle);
  }
}

/**
 * In Fifo mode, prepare for the upcoming rx
 *
 * Grab a buffer to store rx data
 * Keep track of writing data to this buffer
 */
void rxFifoPrep(void)
{
  // Don't allocate memory to save incoming data in BER mode.
  if ((railDataConfig.rxMethod == FIFO_MODE)
      && (currentAppMode() != BER)
      && !rxFifoManual) {
    rxLengthCount = rxLengthTarget;
    rxFifoPacketHandle = memoryAllocate(rxLengthTarget);
    rxFifoPacketData = (RxPacketData_t *)memoryPtrFromHandle(rxFifoPacketHandle);
    currentRxFifoPacketPtr = rxFifoPacketData->dataPtr;
  }
}

/******************************************************************************
 * Public
 *****************************************************************************/
void loadTxData(uint8_t *data, uint16_t dataLen)
{
  uint16_t dataWritten;

  if (railDataConfig.txMethod == PACKET_MODE) {
    RAIL_WriteTxFifo(railHandle, data, dataLen, true);
  } else {
    dataWritten = RAIL_WriteTxFifo(railHandle, data, dataLen, false);
    dataLeft = dataLen - dataWritten;
    dataLeftPtr = data + dataWritten;
  }
}

void configRxLengthSetting(uint16_t rxLength)
{
  if (railDataConfig.rxMethod == FIFO_MODE) {
    rxLengthTarget = rxLength;
  }
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
void RAILCb_TxPacketSent(RAIL_Handle_t railHandle, bool isAck)
{
  LedToggle(1);
  redrawDisplay = true;

  // Store the previous tx time for printing later
  if (isAck) {
    sentAckPackets++;
    // previousTxAckAppendedInfo.isAck already initialized true
    RAIL_Time_t *time = &previousTxAckAppendedInfo.timeSent.packetTime;
    (void) RAIL_GetTxPacketDetailsAlt(railHandle, true, time);
    (void) RAIL_GetTxTimeFrameEnd(railHandle, 0U, time);
    pendFinishTxAckSequence();
  } else {
    internalTransmitCounter++;
    // previousTxAppendedInfo.isAck already initialized false
    RAIL_Time_t *time = &previousTxAppendedInfo.timeSent.packetTime;
    (void) RAIL_GetTxPacketDetailsAlt(railHandle, false, time);
    (void) RAIL_GetTxTimeFrameEnd(railHandle, 0U, time);
    scheduleNextTx();
  }
}

void RAILCb_RxPacketReceived(RAIL_Handle_t railHandle)
{
  counters.receive++;
  LedToggle(0);

  if (railDataConfig.rxMethod == PACKET_MODE) {
    packetMode_RxPacketReceived(railHandle);
  } else if (!rxFifoManual) {
    fifoMode_RxPacketReceived();
  }
}

void RAILCb_TxFifoAlmostEmpty(RAIL_Handle_t railHandle)
{
  uint16_t dataWritten;
  counters.txFifoAlmostEmpty++;

  if (dataLeft) {
    dataWritten = RAIL_WriteTxFifo(railHandle, dataLeftPtr, dataLeft, false);
    dataLeft -= dataWritten;
    dataLeftPtr += dataWritten;
  }
}

// count number of 1s in a byte without a loop
static uint8_t countBits(uint8_t num)
{
  uint8_t count = 0;
  static const uint8_t nibblebits[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
  count += nibblebits[num & 0x0F];
  count += nibblebits[num >> 4];
  return count;
}

static void berSource_RxFifoAlmostFull(uint16_t bytesAvailable)
{
  // All incoming bytes are received and validated here.
  uint16_t numBytes;
  bool stopBerRx = false;

  // If rxOfEvent is > 0, then we're overflowing the incoming RX buffer
  // probably because the BER test isn't processing incoming bits fast
  // enough. The test will automatically try to re-synchronize and read in bits
  // from the stream, but the bits under test will not be continuous. Abort
  // testing and notify the user if this is the case.
  if (counters.rxOfEvent) {
    stopBerRx = true;
  }

  while ((RAIL_GetRxFifoBytesAvailable(railHandle) > RAIL_GetRxFifoThreshold(railHandle))
         && !stopBerRx) {
    // Read multiple bytes in if they're available.
    // Reuse the txData[APP_MAX_PACKET_LENGTH] array since we won't be
    // transmitting in BER Test mode anyway.
    numBytes = RAIL_ReadRxFifo(railHandle, txData, APP_MAX_PACKET_LENGTH);

    for (uint16_t x = 0; x < numBytes && !stopBerRx; x++) {
      // Update BER statistics
      if (berStats.bytesTested < berStats.bytesTotal) {
        // Counters will not overflow since bytesTotal max value is capped.
        berStats.bitErrors += countBits(txData[x]);
        berStats.bytesTested++;
      } else {
        stopBerRx = true; // statistics are all gathered - stop now
      }
    }
  }
  berStats.rssi = (int8_t)(RAIL_GetRssi(railHandle, true) / 4); // disregard decimal point

  // stop RXing when enough bits are acquired or an error (i.e. RX overflow)
  if (stopBerRx) {
    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN, true);
    RAIL_ResetFifo(railHandle, true, true);
    berTestModeEnabled = false;
  }
}

// @TODO It would be better if we could 'register' callback contents
static void packetSource_RxFifoAlmostFull(uint16_t bytesAvailable)
{
  uint16_t bytesRead;
  if (rxLengthCount > 0) {
    // Amount to read is either bytes avilable or number of bytes remaining in packet
    bytesRead = (rxLengthCount > bytesAvailable) ? bytesAvailable : rxLengthCount;
    bytesRead = RAIL_ReadRxFifo(railHandle, currentRxFifoPacketPtr, bytesRead);
    rxLengthCount -= bytesRead;
    currentRxFifoPacketPtr += bytesRead;
  }
}

void RAILCb_RxFifoAlmostFull(RAIL_Handle_t railHandle)
{
  uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
  counters.rxFifoAlmostFull++;

  if (berTestModeEnabled) {
    berSource_RxFifoAlmostFull(bytesAvailable);
  } else if (RAIL_BLE_IsEnabled(railHandle)) {
    RAIL_ConfigEvents(railHandle, RAIL_EVENT_RX_FIFO_ALMOST_FULL,
                      RAIL_EVENTS_NONE); // Disable this event
  } else {
    packetSource_RxFifoAlmostFull(bytesAvailable);
  }
}

void setNextPacketTime(RAIL_ScheduleTxConfig_t *scheduledTxOptions)
{
  memcpy(&nextPacketTxTime, scheduledTxOptions, sizeof(RAIL_ScheduleTxConfig_t));
}
