/***************************************************************************//**
 * @file hall.c
 * @brief Driver for Hall Effect Sensor
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stddef.h>
#include "i2cspm.h"
#include "gpiointerrupt.h"
#include "em_cmu.h"

#include "board.h"
#include "util.h"
#include "hall.h"
#include "hall_device.h"
#include "hall_config.h"

#include "stdio.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup HALL HALL - Hall Effect Sensor
 * @{
 * @brief Driver for Hall effect sensor
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

static HALL_IntCallback pinIntCallback;        /**<  Interrupt callback function   */
static void gpioIntCallback(void);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    Initializes the Hall sensor
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
uint32_t HALL_init(void)
{
  uint32_t status;

  /* Enable power to the sensor */
  BOARD_hallSensorEnable(true);

  /* Allow some time for the part to power up */
  UTIL_delay(10);

  /* Enable interrupt */
  BOARD_hallSensorSetIRQCallback(gpioIntCallback);
  BOARD_hallSensorEnableIRQ(true);

  status = HALL_initDevice();

  pinIntCallback = NULL;

  return status;
}

/***************************************************************************//**
 * @brief
 *    De-initializes the Hall sensor. Disables the sensor power domain.
 *
 * @return
 *    None
 ******************************************************************************/
void HALL_deInit(void)
{
  /* Disable interrupts */
  BOARD_hallSensorEnableIRQ(false);

  /* Disable sensor power */
  BOARD_hallSensorEnable(false);

  HALL_deInitDevice();
}

/***************************************************************************//**
 * @brief
 *    Register GPIO interrupt callback function. This function will be called
 *    when the ALERT pin of the Hall sensor changes state and the interrupt
 *    functions are enabled.
 *
 * @param[in] callback
 *    Pointer to the callback function
 *
 * @return
 *    None
 ******************************************************************************/
void HALL_registerCallback(HALL_IntCallback callback)
{
  pinIntCallback = callback;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *    GPIO interrupt callback function. It is called when the ALERT pin of the
 *    Hall sensor asserted and the interrupt functions are enabled.
 *
 * @param[in] pin
 *    The microcontroller pin connected to ALERT output (not used)
 *
 * @return
 *    Returns zero on OK, non-zero otherwise
 ******************************************************************************/
static void gpioIntCallback(void)
{
  bool pinIsHigh;

  if ( GPIO_PinInGet(HALL_GPIO_PORT_OUT, HALL_GPIO_PIN_OUT) ) {
    pinIsHigh = true;
  } else {
    pinIsHigh = false;
  }

  if ( pinIntCallback != NULL ) {
    pinIntCallback(pinIsHigh);
  }

  return;
}

/** @endcond */

/** @} {end addtogroup HALL} */
/** @} {end addtogroup TBSense_BSP} */
