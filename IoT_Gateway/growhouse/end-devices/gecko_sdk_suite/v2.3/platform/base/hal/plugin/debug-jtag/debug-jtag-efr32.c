/* File: debug-jtag-efr32.c
 * EFM32/EFR32 Debug Transport Mechanism
 *
 * Copyright 2017 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER
#include <string.h>
#include "hal/hal.h"
#include "hal/plugin/debug-jtag/debug-channel.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_device.h"
#include "em_gpio.h"

#include "plugin/debug/rtt/SEGGER_RTT.h"

#if DEBUG_LEVEL >= BASIC_DEBUG

#define OUT_VERSION_INDEX   0
#define OUT_TYPE_INDEX      1
#define OUT_SEQUENCE_INDEX  3
#define OUT_DATA_INDEX      4
#define OUT_VERSION 0xD1

#define TIMEOUT_MS 500

// This is the division factor used to generate the correct baud rate:
// NOTE:  If the prescaler is changed, then the delay in emDebugPowerDown
//        must be adjusted to be 50 bit times at the new baud rate.
#define SPEED_HZ 500000
// This is the amount of time needed to allow the SWO hardware to finish sending
// data.
// 20 byte fifo (four 4 byte transmissions plus their header byte)
// 10 bits per byte
// 500000 baud rate
// (1/500000) * (20*10) = 400 us
#define TX_COMPLETE_DELAY 400

static uint8_t sequenceNumber = 0;
// Size of RTT down buffer
#define DEBUG_VUART_BUFFER_DOWN_SIZE 64
// RTT down channel number
static int rtt_buffer_index = -1;
// RTT down channel ring buffer
static char rtt_buffer[DEBUG_VUART_BUFFER_DOWN_SIZE];

#define DEBUG_OFF           0
#define DEBUG_INITIALIZED   1
#define DEBUG_ENABLED       2
#define DEBUG_POWERING_DOWN 3

//debugChannelState is a variable only used by Zip and Connect.
#if defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT)
static uint8_t debugChannelState = DEBUG_OFF;
#endif //defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT))

// Fallback BSP_TRACE definitions match EFR32 WSTK routing
#ifndef BSP_TRACE_SWO_LOC
#define BSP_TRACE_SWO_LOC  0
#define BSP_TRACE_SWO_PIN  2
#define BSP_TRACE_SWO_PORT gpioPortF
#endif

EmberStatus emDebugInit(void)
{
  // Initialize SWO for target -> debugger output (up-channel)
  // enable gpio clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Enable Serial wire output pin
  GPIO_DbgSWOEnable(true);

  // set location to 0
  GPIO_DbgLocationSet(BSP_TRACE_SWO_LOC);

  // TODO: Use AF_DBG_SWV_PORT and AF_DBG_SWV_PIN macros instead?
  // enable output on pin
  GPIO_PinModeSet(BSP_TRACE_SWO_PORT, BSP_TRACE_SWO_PIN, gpioModePushPull, 1);

#if !(defined(__CORTEX_M) && (__CORTEX_M == 0x00))
//Fancy debugging is not supported on M0(+)...
  emberDebugReportRestore(true);
  emDebugPowerUp();
  emDebugClearStats();
#else
  // enable debug clock, which is done by emDebugPowerUp for non-M0 cores
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
#endif

//debugChannelState is a variable only used by Zip and Connect.
#if defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT)
  debugChannelState = DEBUG_INITIALIZED;
#endif //defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT))

  // Initialize RTT for debugger -> target input (down-channel)
  SEGGER_RTT_Init();

  // The "silabsvuartdown" RTT channel name is used by the debugger to find the
  // location of the RTT buffer in RAM, and can therefore not be changed without
  // updating the board controller firmware.
  rtt_buffer_index = SEGGER_RTT_AllocDownBuffer("silabsvuartdown",
                                                rtt_buffer,
                                                DEBUG_VUART_BUFFER_DOWN_SIZE,
                                                SEGGER_RTT_MODE_NO_BLOCK_TRIM);

  if (rtt_buffer_index >= 0) {
    return EMBER_SUCCESS;
  } else {
    return EMBER_ERR_FATAL;
  }
}

void emDebugPowerUp(void)
{
  // enable debug clock
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);

  /* Enable trace in core debug */
#if defined(ITM)
  CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; //trace enable
  ITM->LAR  = 0xC5ACCE55UL; // unlock itm
  ITM->TER  = 0x0UL;        // trace enable
  ITM->TCR  = 0x0UL;        // trace control
  TPI->SPPR = 2UL;          // pin protocol (2 = NRZ)
  TPI->ACPR = 21UL;         // clock prescaler
  ITM->TPR  = 0x0UL;        // trace priveledge
  DWT->CTRL = ((0x4UL << DWT_CTRL_NUMCOMP_Pos)
               | (1UL << DWT_CTRL_CYCTAP_Pos)
               | (0xFUL << DWT_CTRL_POSTINIT_Pos)
               | (0xFUL << DWT_CTRL_POSTPRESET_Pos)
               | (1UL << DWT_CTRL_CYCCNTENA_Pos));
  ITM->TCR  = ((1UL << ITM_TCR_TraceBusID_Pos)
               | (1UL << ITM_TCR_DWTENA_Pos)
               | (1UL << ITM_TCR_SYNCENA_Pos)
               | (1UL << ITM_TCR_ITMENA_Pos));
  TPI->FFCR = ((1UL << TPI_FFCR_TrigIn_Pos));     // disable formatter
  ITM->TER  |= (1UL << 8);   // enable stimulus channel 8 // BinaryDebugProtocol
  //Left the following code around in case it might be used in the future.
  //ITM->TER  |= (1UL<<0);   // enable stimulus channel 0 // Raw VUART/port 0
#endif
}

#if defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT)

//NOTE:  If emDebugPowerDown is ever called, emDebugPowerUp must be called
//       in order to restore configuration.

void emDebugPowerDown(void)
{
  uint8_t done = false;

  // Only begin the power down process if we're in the enabled state
  {
    DECLARE_INTERRUPT_STATE;
    DISABLE_INTERRUPTS();
    if (debugChannelState != DEBUG_ENABLED) {
      done = true;
    } else {
      // Set the debug state back down to initialized
      debugChannelState = DEBUG_POWERING_DOWN;
    }
    RESTORE_INTERRUPTS();
  }
  // Now that we're out of the disabled interrupts block return if we are done
  if (done) {
    return;
  }

  if (ITM->TCR & ITM_TCR_ITMENA_Msk) {
    DWT->CTRL = (DWT->CTRL & (~DWT_CTRL_SYNCTAP_Msk));

    ITM->TCR &= ~ITM_TCR_ITMENA_Msk;

    while (ITM->TCR & ITM_TCR_BUSY_Msk) {
      // do nothing
    }

    halCommonDelayMicroseconds(100);
  }
  // disable debug oscillator
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, false);

  TPI->SPPR = 0;
  TPI->FFCR = 0;
  CoreDebug->DHCSR = 0;

  debugChannelState = DEBUG_INITIALIZED;
}

bool halStackDebugActive(void)
{
  return ((CoreDebug->DHCSR && CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0);
}

uint8_t emDebugAddInitialFraming(uint8_t *buff, uint16_t debugType)
{
  buff[OUT_VERSION_INDEX]  = OUT_VERSION;
  buff[OUT_TYPE_INDEX]     = LOW_BYTE(debugType);
  buff[OUT_TYPE_INDEX + 1] = HIGH_BYTE(debugType);
  buff[OUT_SEQUENCE_INDEX] = sequenceNumber++;
  return OUT_DATA_INDEX;
}

///Left the following code around in case it might be used in the future.
//__STATIC_INLINE uint8_t itmSendByteChannel0( uint8_t byte )
//{
//   if( (ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << 0) ) )
//   {
//      while (ITM->PORT[0].u32 == 0);
//      ITM->PORT[0].u8 = byte;
//   }
//   return byte;
//}

__STATIC_INLINE uint8_t itmSendByteChannel8(uint8_t byte)
{
  if ((ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << 8))) {
    while (ITM->PORT[8].u32 == 0) ;
    ITM->PORT[8].u8 = byte;
  }
  return byte;
}

// Byte format used by swoOutput() over ITM Stimulus Port 8:
//
// byte 0 = '['
// byte 1 = length
// byte 2 = buffer[0] (the payload)
// byte x = buffer[...] (the payload)
// byte y = crc of payload, high byte
// byte y+1 = crc of payload, low byte
// byte z = ']'

static EmberStatus swoOutput(uint8_t *data, uint8_t length)
{
  length += 5;
  uint8_t index = 0;
  uint16_t startTime = halCommonGetInt16uMillisecondTick();
  uint16_t crc = 0xFFFF;
  uint8_t outputByte = 0;

  while ((halStackDebugActive()) && (index < length)) {
    uint16_t now = halCommonGetInt16uMillisecondTick();
    if (elapsedTimeInt16u(startTime, now) > TIMEOUT_MS) {
      break;
    }

    if (index == 0) {
      outputByte = '[';
      index++;
    } else if (index == 1) {
      outputByte = length - 5;
      index++;
    } else if (index == length - 3) {
      outputByte = LOW_BYTE(crc);
      index++;
    } else if (index == length - 2) {
      outputByte = HIGH_BYTE(crc);
      index++;
    } else if (index == length - 1) {
      outputByte = ']';
      index++;
    } else if (index == length) {
      outputByte = 0;
    } else {
      outputByte = data[index - 2];// emberGetLinkedBuffersByte(buffer, index - 2);
      crc = halCommonCrc16(outputByte, crc);
      index++;
    }

    itmSendByteChannel8(outputByte);
//Left the following code around in case it might be used in the future.
//      itmSendByteChannel0(outputByte);
  }

  if (index == length) {
    return EMBER_SUCCESS;
  } else {
    return EMBER_ERR_FATAL;
  }
}

//x?EmberStatus halStackDebugPutBuffer(EmberMessageBuffer buffer)
//x?{
//x?  if (emberMessageBufferLength(buffer) < OUT_DATA_INDEX) {
//x?    return EMBER_ERR_FATAL;
//x?  }
//x?  return swoOutput(buffer);
//x?}

//zip and znet are fine until this function....
EmberStatus emDebugSend(uint8_t *data, uint8_t *length)
{
  EmberStatus status = EMBER_SUCCESS;
  if (*length > OUT_DATA_INDEX) {
    status = swoOutput(data, *length);
  }
  *length = OUT_DATA_INDEX;
  return status;
}

void emDebugReceiveData(void)
{
  uint8_t buf[DEBUG_VUART_BUFFER_DOWN_SIZE];
  int len = 0;

  len = SEGGER_RTT_Read(rtt_buffer_index, buf, DEBUG_VUART_BUFFER_DOWN_SIZE);

  if (len) {
    emDebugProcessIncoming(EM_DEBUG_VIRTUAL_UART_RX, buf, len);
  }
}

#else //defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT))
#include "stack/include/packet-buffer.h"

//NOTE:  If emDebugPowerDown is ever called, emDebugPowerUp must be called
//       in order to restore configuration.

void emDebugPowerDown(void)
{
#if defined(ITM)
  if (ITM->TCR & ITM_TCR_ITMENA_Msk) {
    DWT->CTRL = (DWT->CTRL & (~DWT_CTRL_SYNCTAP_Msk));

    ITM->TCR &= ~ITM_TCR_ITMENA_Msk;

    while ((ITM->TCR & ITM_TCR_BUSY_Msk) != 0U) {
      // do nothing
    }

    halCommonDelayMicroseconds(100);
  }
  // disable debug oscillator
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, false, false);

  TPI->SPPR = 0;
  TPI->FFCR = 0;
  CoreDebug->DHCSR = 0;
#endif
}

bool halStackDebugActive(void)
{
#if defined(CoreDebug)
  return ((CoreDebug->DHCSR && CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0);
#else
  return false;
#endif
}

uint8_t emDebugAddInitialFraming(uint8_t *buff, uint16_t debugType)
{
#if defined(ITM)
  buff[OUT_VERSION_INDEX]  = OUT_VERSION;
  buff[OUT_TYPE_INDEX]     = LOW_BYTE(debugType);
  buff[OUT_TYPE_INDEX + 1] = HIGH_BYTE(debugType);
  buff[OUT_SEQUENCE_INDEX] = sequenceNumber++;
  return OUT_DATA_INDEX;
#else
  return 0;
#endif
}

///Left the following code around in case it might be used in the future.
//__STATIC_INLINE uint8_t itmSendByteChannel0( uint8_t byte )
//{
//   if( (ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << 0) ) )
//   {
//      while (ITM->PORT[0].u32 == 0);
//      ITM->PORT[0].u8 = byte;
//   }
//   return byte;
//}

__STATIC_INLINE uint8_t itmSendByteChannel8(uint8_t byte)
{
#if defined(ITM)
  if ((ITM->TCR & ITM_TCR_ITMENA_Msk) && (ITM->TER & (1UL << 8))) {
    while (ITM->PORT[8].u32 == 0) {
    }
    ITM->PORT[8].u8 = byte;
  }
#endif
  return byte;
}

// Byte format used by swoOutput() over ITM Stimulus Port 8:
//
// byte 0 = '['
// byte 1 = length
// byte 2 = buffer[0] (the payload)
// byte x = buffer[...] (the payload)
// byte y = crc of payload, high byte
// byte y+1 = crc of payload, low byte
// byte z = ']'

static EmberStatus swoOutput(EmberMessageBuffer buffer)
{
  uint8_t length = emberMessageBufferLength(buffer) + 5;
  uint8_t index = 0;
  uint16_t startTime = halCommonGetInt16uMillisecondTick();
  uint16_t crc = 0xFFFF;
  uint8_t outputByte = 0;

  while ((halStackDebugActive()) && (index < length)) {
    uint16_t now = halCommonGetInt16uMillisecondTick();
    if (elapsedTimeInt16u(startTime, now) > TIMEOUT_MS) {
      break;
    }

    if (index == 0) {
      outputByte = '[';
      index++;
    } else if (index == 1) {
      outputByte = emberMessageBufferLength(buffer);
      index++;
    } else if (index == length - 3) {
      outputByte = LOW_BYTE(crc);
      index++;
    } else if (index == length - 2) {
      outputByte = HIGH_BYTE(crc);
      index++;
    } else if (index == length - 1) {
      outputByte = ']';
      index++;
    } else if (index == length) {
      outputByte = 0;
    } else {
      outputByte = emberGetLinkedBuffersByte(buffer, index - 2);
      crc = halCommonCrc16(outputByte, crc);
      index++;
    }

    itmSendByteChannel8(outputByte);
//Left the following code around in case it might be used in the future.
//      itmSendByteChannel0(outputByte);
  }

  if (index == length) {
    return EMBER_SUCCESS;
  } else {
    return EMBER_ERR_FATAL;
  }
}

EmberStatus halStackDebugPutBuffer(EmberMessageBuffer buffer)
{
  if (emberMessageBufferLength(buffer) < OUT_DATA_INDEX) {
    return EMBER_ERR_FATAL;
  }
  return swoOutput(buffer);
}

void emDebugReceiveData(void)
{
  uint8_t buf[DEBUG_VUART_BUFFER_DOWN_SIZE];
  int len = 0;

  len = SEGGER_RTT_Read(rtt_buffer_index, buf, DEBUG_VUART_BUFFER_DOWN_SIZE);

  if (len) {
    EmberMessageBuffer message = emberFillLinkedBuffers(buf, len);
    emDebugProcessIncoming(EM_DEBUG_VIRTUAL_UART_RX, message);
    emberReleaseMessageBuffer(message);
  }
}

#endif //defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT))

#endif // DEBUG_LEVEL >= BASIC_DEBUG
