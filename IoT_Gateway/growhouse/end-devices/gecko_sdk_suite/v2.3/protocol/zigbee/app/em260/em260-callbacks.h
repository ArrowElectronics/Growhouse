// -----------------------------------------------------------------------------
// @file em260-callbacks.h
// @brief EZSP stack callbacks header
//
// @section License
// <b>(C) Copyright 2016 Silicon Laboratories, www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#ifndef EM260_CALLBACKS_H
#define EM260_CALLBACKS_H
bool setCallbackQueueBufferLength(uint16_t requiredLength, bool extendOnly);
uint8_t getCallbackQueueByte(uint16_t index);
void setCallbackQueueByte(uint16_t index, uint8_t byte);
bool addToCallbackQueue(uint8_t *callback,
                        uint8_t callbackLength);
bool addToCallbackQueueExtended(uint8_t *callback,
                                uint8_t callbackLength,
                                EmberMessageBuffer buffer,
                                uint8_t *additionalData,
                                uint8_t additionalDataLength);
uint16_t findInCallbackQueue(uint8_t frameId, uint16_t index);
void writeLqiAndRssi(uint8_t *loc);
extern EmberMessageBuffer callbackQueue;
extern uint16_t callbackQueueLength;
extern uint16_t callbackQueueBufferLength;
extern bool callbackOverflow;
extern bool uartSynchCallbacks;

#endif //EM260_CALLBACKS_H
