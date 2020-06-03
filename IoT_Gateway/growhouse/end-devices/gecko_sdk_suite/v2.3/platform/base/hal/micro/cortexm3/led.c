/** @file hal/micro/cortexm3/led.c
 *  @brief LED manipulation routines; stack and example APIs
 *
 * Copyright 2016 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER
#if !defined(MINIMAL_HAL) && defined(BOARD_HEADER)
// full hal needs the board header to get pulled in
  #include "hal/micro/micro.h"
  #include BOARD_HEADER
#endif
#include "hal/micro/led.h"

#ifdef LED_ACTIVE_HIGH
#define GPIO_SET_LED   SET
#define GPIO_CLEAR_LED CLR
#else
#define GPIO_SET_LED   CLR
#define GPIO_CLEAR_LED SET
#endif

void halSetLed(HalBoardLed led)
{
  uint32_t port = (uint8_t) led / 8U;
  uint32_t pin = (uint8_t) led & 0x7U;
#if (!defined(MINIMAL_HAL) && !defined(BOOTLOADER) && defined(RHO_GPIO))
  if (led == RHO_GPIO && halGetRadioHoldOff()) { // Avoid Radio HoldOff conflict
    return;
  }
#endif

  GPIO->P[port].GPIO_SET_LED = BIT(pin);
}

void halClearLed(HalBoardLed led)
{
  uint32_t port = (uint8_t) led / 8U;
  uint32_t pin = (uint8_t) led & 0x7U;
#if (!defined(MINIMAL_HAL) && !defined(BOOTLOADER) && defined(RHO_GPIO))
  if (led == RHO_GPIO && halGetRadioHoldOff()) { // Avoid Radio HoldOff conflict
    return;
  }
#endif

  GPIO->P[port].GPIO_CLEAR_LED = BIT(pin);
}

void halToggleLed(HalBoardLed led)
{
  uint32_t port = (uint8_t) led / 8U;
  uint32_t pin = (uint8_t) led & 0x7U;
#if (!defined(MINIMAL_HAL) && !defined(BOOTLOADER) && defined(RHO_GPIO))
  if (led == RHO_GPIO && halGetRadioHoldOff()) { // Avoid Radio HoldOff conflict
    return;
  }
#endif

  //to avoid contention with other code using the other pins for other
  //purposes, we disable interrupts since this is a read-modify-write
  {
    DECLARE_INTERRUPT_STATE;
    DISABLE_INTERRUPTS();
    GPIO->P[port].OUT ^= BIT(pin);
    RESTORE_INTERRUPTS();
  }
}

#ifndef MINIMAL_HAL
void halStackIndicateActivity(bool turnOn)
{
 #if     NO_LED
  // Don't touch LEDs when built with NO_LED
 #else//!NO_LED
  if (turnOn) {
    halSetLed(BOARD_ACTIVITY_LED);
  } else {
    halClearLed(BOARD_ACTIVITY_LED);
  }
 #endif//NO_LED
}

#endif //MINIMAL_HAL
