/** @file hal/micro/cortexm3/efm32/board/wstk-common.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board-common
 *  @brief Common breakout board functions and definitions.
 *
 *@{
 */

#ifndef __BOARD_COMMON_H__
#define __BOARD_COMMON_H__

#include "bspconfig.h"

/**************             Main Board Definitions               **************/

/** @name LED Definitions
 *
 * The following are used to aid in the abstraction with the LED
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The ::HalBoardLedPins enum values should always be used when manipulating the
 * state of LEDs, as they directly refer to the GPIOs to which the LEDs are
 * connected.
 *
 * \b Note: LEDs 0 and 1 are on the mainboard. There are no LEDs on the RCM
 *
 * \b Note: LEDs 2 and 3 simply redirects to LEDs 0 and 1.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = BOARDLED0,
  BOARDLED3 = BOARDLED1,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};

/** @} END OF LED DEFINITIONS  */

/** @name Button Definitions
 *
 * The following are used to aid in the abstraction with the Button
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The BUTTONn macros should always be used with manipulating the buttons
 * as they directly refer to the GPIOs to which the buttons are connected.
 *
 */
//@{
//@} //END OF BUTTON DEFINITIONS

/** @name UART Definitions
 *
 * The following are used to aid in the abstraction with the UART.
 * The microcontroller-specific sources use these definitions so they are able
 * to work across a variety of boards which could have different connections.
 * The names and ports/pins used below are intended to match with a schematic
 * of the system to provide the abstraction.
 *
 */
//@{

/**
 * @brief Enable VCOM Pin, passthrough UART via the WSTK
 */
#ifndef halEnableVCOM
  #define halEnableVCOM()  do {                                                    \
    GPIO_PinModeSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN, gpioModePushPull, 1); \
    GPIO_PinOutSet(BSP_BCC_ENABLE_PORT, BSP_BCC_ENABLE_PIN);                       \
} while (0)
#endif

// Define USART0 flow control pins
#ifndef USART0_CTS_LOCATION
  #define USART0_CTS_LOCATION _USART_ROUTELOC1_CTSLOC_LOC30
#endif
#ifndef USART0_RTS_LOCATION
  #define USART0_RTS_LOCATION _USART_ROUTELOC1_RTSLOC_LOC30
#endif
#ifndef USART0_CTS_PORT
  #define USART0_CTS_PORT gpioPortA
#endif
#ifndef USART0_CTS_PIN
  #define USART0_CTS_PIN 2
#endif
#ifndef USART0_RTS_PORT
  #define USART0_RTS_PORT gpioPortA
#endif
#ifndef USART0_RTS_PIN
  #define USART0_RTS_PIN 3
#endif
//@} //END OF UART DEFINITIONS
//

/** @name External Flash Definitions
 *
 * The following are used to aid in the abstraction with the external flash.
 * The microcontroller-specific sources use these definitions so they are able
 * to work across a variety of boards which could have different connections.
 * The names and ports/pins used below are intended to match with a schematic
 * of the system to provide the abstraction.
 *
 * This default configuration sets the location to the WSTK expansion header
 */
//@{
// Define external flash pins

#ifndef EXTERNAL_FLASH_MOSI_PORT
  #define EXTERNAL_FLASH_MOSI_PORT    gpioPortC
#endif
#ifndef EXTERNAL_FLASH_MOSI_PIN
  #define EXTERNAL_FLASH_MOSI_PIN     6
#endif
#ifndef EXTERNAL_FLASH_MISO_PORT
  #define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#endif
#ifndef EXTERNAL_FLASH_MISO_PIN
  #define EXTERNAL_FLASH_MISO_PIN     7
#endif
#ifndef EXTERNAL_FLASH_CLK_PORT
  #define EXTERNAL_FLASH_CLK_PORT     gpioPortC
#endif
#ifndef EXTERNAL_FLASH_CLK_PIN
  #define EXTERNAL_FLASH_CLK_PIN      8
#endif
#ifndef EXTERNAL_FLASH_CS_PORT
  #define EXTERNAL_FLASH_CS_PORT      gpioPortC
#endif
#ifndef EXTERNAL_FLASH_CS_PIN
  #define EXTERNAL_FLASH_CS_PIN       9
#endif
//@} //END OF EXTERNAL FLASH DEFINITIONS

/**************            Common Configuration Options          **************/

#ifndef halInternalInitPrs
#define halInternalInitPrs()  do { \
} while (0)
#endif //halInternalInitPrs

/**
 * @name EMDRV_RTCDRV_USE_LFRCO
 *
 * If not defined, RTCDRV will default to use the LFXO as a clock source.
 * Must be defined if no LFXO is present
 *
 */
#define EMDRV_RTCDRV_USE_LFRCO

/**************            Essential Board Functions             **************/

/**
 * @name halInternalInitBoard
 *
 * Called on startup from halInit(). Should contain calls to all board
 * specific startup sequences.
 *
 */
#define halInternalInitBoard() do { \
    halInternalPowerUpBoard();      \
} while (0)

/**
 * @name halInternalPowerUpBoard
 *
 * Call after coming out of sleep to restore any features which need to
 * power off for sleep
 *
 */
#ifndef halInternalPowerUpBoard
  #define halInternalPowerUpBoard() do { \
} while (0)
#endif

/**
 * @name halInternalPowerDownBoard
 *
 * Call before going to sleep to power down any features which need to
 * off for sleep
 *
 */
#ifndef halInternalPowerDownBoard
  #define halInternalPowerDownBoard() do { \
} while (0)
#endif

/**
 *  Instantiate any variables that are used in the board header
 */
#ifndef DEFINE_BOARD_GPIO_CFG_VARIABLES
  #define DEFINE_BOARD_GPIO_CFG_VARIABLES()
#endif

/**
 * Define TEMP_SENSOR to satisfy Ember HAL. Should not be used
 */
#define TEMP_SENSOR_ADC_CHANNEL 0
#define TEMP_SENSOR_SCALE_FACTOR 0

#endif //__BOARD_COMMON_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
