// File: serial-interface.h
//
// Description: The interface down to the serial protocol being used to
// transport EZSP frames (either SPI or UART).
//
// Copyright 2007-2010 by Ember Corporation. All rights reserved.           *80*

#ifndef SILABS_APP_EM260_SERIAL_INTERFACE_H
#define SILABS_APP_EM260_SERIAL_INTERFACE_H

void serialInit(void);
bool serialReleaseBuffers(void);
void serialAllocateBuffers(void);
bool serialOkToSleep(void);
bool serialOkToBootload(void);
bool serialCommandReceived(void);
void serialTransmit(void);
void serialResponseReady(bool highPriority);
void serialPendingCallbacks(bool pending);
void serialDebugTrace(void);
EmberMessageBuffer serialFillBufferFromCommand(uint8_t startIndex,
                                               uint8_t length);
uint8_t serialGetCommandLength(void);
void serialSetResponseLength(uint8_t data);
#define serialGetCommandByte(index)        (ezspFrameContents[(index)])
#define serialGetResponseByte(index)       (ezspFrameContents[(index)])
#define serialSetResponseByte(index, data) (ezspFrameContents[(index)] = (data))
bool serialCallbackResponse(void);

void serialMfglibFillPacket(uint8_t *packet);
void serialDelayTest(uint16_t delay);

#ifdef SLEEPY_EZSP_ASH
  #include "hal/micro/generic/ash-ncp.h"
#else
  #define serialHostIsActive() !serialOkToSleep()
  #define serialWaitingForHostToWake() (false)
  #define serialSendAwakeSignal()
  #define serialInhibitCallbackSignal()
  #if defined(EZSP_SPI) && !defined (BOARD_SIMULATION)
    #define serialPowerDown() halHostSerialPowerdown()
    #define serialPowerUp() halHostSerialPowerup()
  #else
    #define serialPowerDown()
    #define serialPowerUp()
  #endif
#endif

#endif // SILABS_APP_EM260_SERIAL_INTERFACE_H
