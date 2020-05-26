/***************************************************************************//**
 * @file
 * @brief Platform Abstraction Layer (PAL) interface for DISPLAY driver.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef _DISPLAY_PAL_H_
#define _DISPLAY_PAL_H_

#include "emstatus.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 ********************************  DEFINES  ************************************
 ******************************************************************************/

/** EMSTATUS codes of the PAL interface. */
#define PAL_EMSTATUS_OK                                  (0) /**< Operation successful. */
#define PAL_EMSTATUS_INVALID_PARAM (PAL_EMSTATUS_BASE   | 1) /**< Invalid parameter. */
#define PAL_EMSTATUS_REPEAT_FAILED (PAL_EMSTATUS_BASE   | 2) /**< Repeat failed. */

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** PAL GPIO Pin modes. */
typedef enum {
  /** Push-pull output */
  palGpioModePushPull
} PAL_GpioMode_t;

/*******************************************************************************
 **************************    FUNCTION PROTOTYPES    **************************
 ******************************************************************************/

/**************************************************************************//**
 * @brief   Initialize the PAL GPIO interface
 *
 * @detail  This function initializes all resources required to support the
 *          PAL GPIO interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_GpioInit (void);

/**************************************************************************//**
 * @brief   Shutdown the PAL GPIO interface
 *
 * @detail  This function releases/stops all resources used by the
 *          PAL GPIO interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_GpioShutdown (void);

/***************************************************************************//**
 * @brief
 *   Set the mode for a GPIO pin.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin number in the port.
 *
 * @param[in] mode
 *   The desired pin mode.
 *
 * @param[in] platformSpecific
 *   Platform specific value which may need to be set.
 *   For EFM32:
 *   Value to set for pin in DOUT register. The DOUT setting is important for
 *   even some input mode configurations, determining pull-up/down direction.
 ******************************************************************************/
EMSTATUS PAL_GpioPinModeSet(unsigned int   port,
                            unsigned int   pin,
                            PAL_GpioMode_t mode,
                            unsigned int   platformSpecific);

/***************************************************************************//**
 * @brief
 *   Set a single pin in GPIO data out register to 1.
 *
 * @note
 *   In order for the setting to take effect on the output pad, the pin must
 *   have been configured properly. If not, it will take effect whenever the
 *   pin has been properly configured.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin to set.
 ******************************************************************************/
EMSTATUS PAL_GpioPinOutSet(unsigned int port, unsigned int pin);

/***************************************************************************//**
 * @brief
 *   Set a single pin in GPIO data out port register to 0.
 *
 * @note
 *   In order for the setting to take effect on the output pad, the pin must
 *   have been configured properly. If not, it will take effect whenever the
 *   pin has been properly configured.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin to set.
 ******************************************************************************/
EMSTATUS PAL_GpioPinOutClear(unsigned int port, unsigned int pin);

/***************************************************************************//**
 * @brief
 *   Toggle a single pin in GPIO port data out register.
 *
 * @note
 *   In order for the setting to take effect on the output pad, the pin must
 *   have been configured properly. If not, it will take effect whenever the
 *   pin has been properly configured.
 *
 * @param[in] port
 *   The GPIO port to access.
 *
 * @param[in] pin
 *   The pin to toggle.
 ******************************************************************************/
EMSTATUS PAL_GpioPinOutToggle(unsigned int port, unsigned int pin);

/**************************************************************************//**
 * @brief   Toggle a GPIO pin automatically at the given frequency.
 *
 * @param[in] gpioPort  GPIO port number of GPIO ping to toggle.
 * @param[in] gpioPin   GPIO pin number.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_GpioPinAutoToggle (unsigned int gpioPort,
                                unsigned int gpioPin,
                                unsigned int frequency);

/**************************************************************************//**
 * @brief   Initialize the PAL SPI interface
 *
 * @detail  This function initializes all resources required to support the
 *          PAL SPI interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_SpiInit (void);

/**************************************************************************//**
 * @brief   Shutdown the PAL SPI interface
 *
 * @detail  This function releases/stops all resources used by the
 *          PAL SPI interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_SpiShutdown (void);

/**************************************************************************//**
 * @brief      Transmit data on the SPI interface.
 *
 * @param[in]  data    Pointer to the data to be transmitted.
 * @param[in]  len     Length of data to transmit.
 *
 * @return     EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_SpiTransmit (uint8_t* data, unsigned int len);

/**************************************************************************//**
 * @brief   Initialize the PAL Timer interface
 *
 * @detail  This function initializes all resources required to support the
 *          PAL Timer interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerInit (void);

/**************************************************************************//**
 * @brief   Shutdown the PAL Timer interface
 *
 * @detail  This function releases/stops all resources used by the
 *          PAL Timer interface functions.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerShutdown (void);

/**************************************************************************//**
 * @brief   Delay for the specified number of micro seconds.
 *
 * @param[in] usecs   Number of micro seconds to delay.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerMicroSecondsDelay(unsigned int usecs);

#ifdef PAL_TIMER_REPEAT_FUNCTION
/**************************************************************************//**
 * @brief   Call a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  EMSTATUS code of the operation.
 *****************************************************************************/
EMSTATUS PAL_TimerRepeat (void(*pFunction)(void*),
                          void* argument,
                          unsigned int frequency);
#endif

#ifdef __cplusplus
}
#endif

/** @endcond */

#endif /* _DISPLAY_PAL_H_ */
