/* File: debug-jtag-stub.c
 * Simulator Stub implementation of Debug Transport Mechanism (JTAG)
 *
 * Copyright 2017 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER
#include "hal/hal.h"
#include "hal/plugin/debug-jtag/debug-channel.h"

EmberStatus emDebugInit(void)
{
  return EMBER_SUCCESS;
}

void emDebugPowerUp(void)
{
}

void emDebugPowerDown(void)
{
}

uint8_t emDebugAddInitialFraming(uint8_t *buff, uint16_t debugType)
{
  UNUSED_VAR(buff);
  UNUSED_VAR(debugType);

  return 0;
}

void emDebugReceiveData(void)
{
}

bool halStackDebugActive(void)
{
  return true;
}

#if defined(EMBER_SCRIPTED_TEST)
void emDebugReceiveTick(void)
{
}
#endif

//Mirror the compile logic seen in base/hal/micro/serial.h
#if !(defined(CORTEXM3_EFM32_MICRO) && !defined(EMBER_STACK_CONNECT)) && !defined(CORTEXM3_EMBER_MICRO)
void halStackReceiveVuartMessage(uint8_t *data, uint8_t length)
{
  UNUSED_VAR(data);
  UNUSED_VAR(length);
}
#endif

//The commented out #if-#else are left here as a reminder of which
//stack the functions pair with.

//#if defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT)

EmberStatus emDebugSend(uint8_t *data, uint8_t *length)
{
  UNUSED_VAR(data);
  UNUSED_VAR(length);

  return EMBER_SUCCESS;
}

//else //defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT))

EmberStatus halStackDebugPutBuffer(EmberMessageBuffer packet)
{
  UNUSED_VAR(packet);

  return EMBER_SUCCESS;
}

//endif //defined(EMBER_STACK_IP) || defined(EMBER_STACK_CONNECT))
