/***************************************************************************//**
 * @file bspconfig.h
 * @brief Provide BSP (board support package) configuration parameters.
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

#ifndef BSPCONFIG_H
#define BSPCONFIG_H

#include "em_gpio.h"

#define BSP_STK
#define BSP_TBSENSE2

#define BSP_BCC_USART         USART0
#define BSP_BCC_CLK           cmuClock_USART0
#define BSP_BCC_TX_LOCATION   USART_ROUTELOC0_TXLOC_LOC0
#define BSP_BCC_RX_LOCATION   USART_ROUTELOC0_RXLOC_LOC0
#define BSP_BCC_TXPORT        gpioPortA
#define BSP_BCC_TXPIN         0
#define BSP_BCC_RXPORT        gpioPortA
#define BSP_BCC_RXPIN         1
#define BSP_BCC_ENABLE_PORT   gpioPortA
#define BSP_BCC_ENABLE_PIN    5                 /* VCOM_ENABLE */

#define BSP_DISP_ENABLE_PORT  gpioPortD
#define BSP_DISP_ENABLE_PIN   15                /* MemLCD display enable */

#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS          2
#define BSP_GPIO_LED0_PORT      gpioPortD
#define BSP_GPIO_LED0_PIN       11
#define BSP_GPIO_LED1_PORT      gpioPortD
#define BSP_GPIO_LED1_PIN       12
#define BSP_GPIO_LEDARRAY_INIT  { { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, { BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN } }

#define BSP_GPIO_BUTTONS
#define BSP_NO_OF_BUTTONS       2
#define BSP_GPIO_PB0_PORT       gpioPortD
#define BSP_GPIO_PB0_PIN        14
#define BSP_GPIO_PB1_PORT       gpioPortD
#define BSP_GPIO_PB1_PIN        15

#define BSP_GPIO_BUTTONARRAY_INIT { { BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN }, { BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN } }

#define BSP_INIT_DEFAULT  0

#if !defined(EMU_DCDCINIT_WSTK_DEFAULT)
/* Use emlib defaults */
#define EMU_DCDCINIT_WSTK_DEFAULT       EMU_DCDCINIT_DEFAULT
#endif

#if !defined(CMU_HFXOINIT_WSTK_DEFAULT)
#define CMU_HFXOINIT_WSTK_DEFAULT                                             \
  {                                                                           \
    false,      /* Low-noise mode for EFR32                                */ \
    false,      /* Disable auto-start on EM0/1 entry                       */ \
    false,      /* Disable auto-select on EM0/1 entry                      */ \
    false,      /* Disable auto-start and select on RAC wakeup             */ \
    _CMU_HFXOSTARTUPCTRL_CTUNE_DEFAULT,                                       \
    0x142,      /* Steady-state CTUNE for TBSENSE boards without load caps */ \
    _CMU_HFXOSTEADYSTATECTRL_REGISH_DEFAULT,                                  \
    _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT,                                \
    0x7,        /* Recommended steady-state XO core bias current           */ \
    0x6,        /* Recommended peak detection threshold                    */ \
    _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_DEFAULT,                             \
    0xA,        /* Recommended peak detection timeout                      */ \
    _CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_DEFAULT,                               \
    _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_DEFAULT,                              \
    cmuOscMode_Crystal,                                                       \
  }
#endif

#if !defined(RADIO_PTI_INIT)
#define RADIO_PTI_INIT                                                     \
  {                                                                        \
    RADIO_PTI_MODE_UART,    /* Simplest output mode is UART mode        */ \
    1600000,                /* Choose 1.6 MHz for best compatibility    */ \
    6,                      /* TBSENSE uses location 6 for DOUT         */ \
    gpioPortB,              /* Get the port for this loc                */ \
    12,                     /* Get the pin, location should match above */ \
    6,                      /* TBSENSE uses location 6 for DCLK         */ \
    gpioPortB,              /* Get the port for this loc                */ \
    11,                     /* Get the pin, location should match above */ \
    6,                      /* TBSENSE uses location 6 for DFRAME       */ \
    gpioPortB,              /* Get the port for this loc                */ \
    13,                     /* Get the pin, location should match above */ \
  }
#endif

#if !defined(RAIL_PTI_CONFIG)
#define RAIL_PTI_CONFIG                                                    \
  {                                                                        \
    RAIL_PTI_MODE_UART,     /* Simplest output mode is UART mode        */ \
    1600000,                /* Choose 1.6 MHz for best compatibility    */ \
    6,                      /* TBSENSE uses location 6 for DOUT         */ \
    gpioPortB,              /* Get the port for this loc                */ \
    12,                     /* Get the pin, location should match above */ \
    6,                      /* TBSENSE uses location 6 for DCLK         */ \
    gpioPortB,              /* Get the port for this loc                */ \
    11,                     /* Get the pin, location should match above */ \
    6,                      /* TBSENSE uses location 6 for DFRAME       */ \
    gpioPortB,              /* Get the port for this loc                */ \
    13,                     /* Get the pin, location should match above */ \
  }
#endif

#if !defined(RADIO_PA_2P4_INIT)
#define RADIO_PA_2P4_INIT                                    \
  {                                                          \
    PA_SEL_2P4_HP,    /* Power Amplifier mode             */ \
    PA_VOLTMODE_DCDC, /* Power Amplifier vPA Voltage mode */ \
    100,              /* Desired output power in dBm * 10 */ \
    0,                /* Output power offset in dBm * 10  */ \
    10,               /* Desired ramp time in us          */ \
  }
#endif

#if !defined(RAIL_PA_2P4_CONFIG)
#define RAIL_PA_2P4_CONFIG                                            \
  {                                                                   \
    RAIL_TX_POWER_MODE_2P4_HP, /* Power Amplifier mode             */ \
    1800,                      /* Power Amplifier vPA Voltage mode */ \
    10,                        /* Desired ramp time in us          */ \
  }
#endif

#if !defined(RADIO_PA_SUBGIG_INIT)
#define RADIO_PA_SUBGIG_INIT                                 \
  {                                                          \
    PA_SEL_SUBGIG,    /* Power Amplifier mode             */ \
    PA_VOLTMODE_DCDC, /* Power Amplifier vPA Voltage mode */ \
    100,              /* Desired output power in dBm * 10 */ \
    0,                /* Output power offset in dBm * 10  */ \
    10,               /* Desired ramp time in us          */ \
  }
#endif

#if !defined(RAIL_PA_SUBGIG_CONFIG)
#define RAIL_PA_SUBGIG_CONFIG                                         \
  {                                                                   \
    RAIL_TX_POWER_MODE_SUBGIG, /* Power Amplifier mode             */ \
    1800,                      /* Power Amplifier vPA Voltage mode */ \
    10,                        /* Desired ramp time in us          */ \
  }
#endif

#if !defined(RAIL_PA_DEFAULT_POWER)
#define RAIL_PA_DEFAULT_POWER 100
#endif

/***************************************************************************//**
 * @defgroup BOARD_Config_Settings BOARD module configuration
 * @{
 * @brief BOARD module configuration macro definitions
 ******************************************************************************/

#define BOARD_RGBLED_TIMER        (TIMER0)        /**< RGB LED PWM control timer        */
#define BOARD_RGBLED_CMU_CLK      cmuClock_TIMER0 /**< RGB LED PWM control clock source */
#define BOARD_RGBLED_RED_PORT     gpioPortD       /**< RGB LED Red port                 */
#define BOARD_RGBLED_RED_PIN      11              /**< RGB LED Red pin                  */
#define BOARD_RGBLED_RED_CCLOC    19              /**< RGB LED Red CC location          */
#define BOARD_RGBLED_GREEN_PORT   gpioPortD       /**< RGB LED Green port               */
#define BOARD_RGBLED_GREEN_PIN    12              /**< RGB LED Green pin                */
#define BOARD_RGBLED_GREEN_CCLOC  19              /**< RGB LED Green CC location        */
#define BOARD_RGBLED_BLUE_PORT    gpioPortD       /**< RGB LED Blue port                */
#define BOARD_RGBLED_BLUE_PIN     13              /**< RGB LED Blue pin                 */
#define BOARD_RGBLED_BLUE_CCLOC   19              /**< RGB LED Blue CC location         */

#define BOARD_LED_PORT            gpioPortD       /**< LED port                         */
#define BOARD_LED_RED_PORT        gpioPortD       /**< Red LED port                     */
#define BOARD_LED_RED_PIN         9               /**< Red LED pin                      */
#define BOARD_LED_GREEN_PORT      gpioPortD       /**< Green LED port                   */
#define BOARD_LED_GREEN_PIN       8               /**< Green LED pin                    */
#define BOARD_RGBLED_PWR_EN_PORT  gpioPortJ       /**< RGB LED Power Enable port        */
#define BOARD_RGBLED_PWR_EN_PIN   14              /**< RGB LED Power Enable pin         */
#define BOARD_RGBLED_COM_PORT     gpioPortI       /**< RGB LED COM port                 */
#define BOARD_RGBLED_COM0_PORT    gpioPortI       /**< RGB LED COM0 port                */
#define BOARD_RGBLED_COM0_PIN     0               /**< RGB LED COM0 pin                 */
#define BOARD_RGBLED_COM1_PORT    gpioPortI       /**< RGB LED COM1 port                */
#define BOARD_RGBLED_COM1_PIN     1               /**< RGB LED COM1 pin                 */
#define BOARD_RGBLED_COM2_PORT    gpioPortI       /**< RGB LED COM2 port                */
#define BOARD_RGBLED_COM2_PIN     2               /**< RGB LED COM2 pin                 */
#define BOARD_RGBLED_COM3_PORT    gpioPortI       /**< RGB LED COM3 port                */
#define BOARD_RGBLED_COM3_PIN     3               /**< RGB LED COM3 pin                 */

#define BOARD_IMU_ENABLE_PORT       gpioPortF     /**< IMU enable port                  */
#define BOARD_IMU_ENABLE_PIN        8             /**< IMU enable pin                   */
#define BOARD_IMU_INT_PORT          gpioPortF     /**< IMU interrupt port               */
#define BOARD_IMU_INT_PIN           12            /**< IMU interrupt pin                */
#define BOARD_IMU_SPI_PORT          gpioPortC     /**< IMU SPI port                     */
#define BOARD_IMU_SPI_MOSI_PIN      0             /**< IMU SPI master out slave in pin  */
#define BOARD_IMU_SPI_MISO_PIN      1             /**< IMU SPI master in slave out pin  */
#define BOARD_IMU_SPI_SCLK_PIN      2             /**< IMU SPI serial clock pin         */
#define BOARD_IMU_SPI_CS_PIN        3             /**< IMU SPI chip select pin          */

#define BOARD_UV_ALS_INT_PORT     gpioPortF       /**< UV/ALS sensor Interrupt port     */
#define BOARD_UV_ALS_INT_PIN      11              /**< UV/ALS sensor Interrupt pin      */

#define BOARD_MIC_ENABLE_PORT       gpioPortF     /**< Microphone enable port           */
#define BOARD_MIC_ENABLE_PIN        10            /**< Microphone enable pin            */
#define BOARD_MIC_INPUT_PORT        gpioPortF     /**< Microphone analog input          */
#define BOARD_MIC_INPUT_PIN         7             /**< Microphone analog input          */

#define BOARD_ENV_ENABLE_PORT       gpioPortF     /**< Environmental sensor enable port    */
#define BOARD_ENV_ENABLE_PIN        9             /**< Environmental sensor enable pin     */
#define BOARD_UV_ALS_INT_PORT       gpioPortF     /**< Ambient light sensor interrupt port */
#define BOARD_UV_ALS_INT_PIN        11            /**< Ambient light sensor interrupt pin  */
#define BOARD_ENV_I2C_PORT          gpioPortC     /**< Environmental sensor I2C port       */
#define BOARD_ENV_I2C_SDA_PIN       4             /**< Environmental sensor I2C SDA pin    */
#define BOARD_ENV_I2C_SCL_PIN       5             /**< Environmental sensor I2C SCL pin    */
#define BOARD_ENV_I2C_ROUTELOC0     (I2C_ROUTELOC0_SDALOC_LOC17 | I2C_ROUTELOC0_SCLLOC_LOC17)

#define BOARD_CCS811_CTRL_PORT      gpioPortF     /**< Gas sensor enable port           */
#define BOARD_CCS811_INT_PIN        13            /**< Gas sensor interrupt pin         */
#define BOARD_CCS811_ENABLE_PIN     14            /**< Gas sensor enable pin            */
#define BOARD_CCS811_WAKE_PIN       15            /**< Gas sensor nWAKE pin             */
#define BOARD_CCS811_I2C_PORT       gpioPortB     /**< Gas sensor I2C port              */
#define BOARD_CCS811_I2C_SDA_PIN    6             /**< Gas sensor I2C SDA pin           */
#define BOARD_CCS811_I2C_SCL_PIN    7             /**< Gas sensor I2C SCL pin           */
#define BOARD_CCS811_I2C_ROUTELOC0  (I2C_ROUTELOC0_SDALOC_LOC6 | I2C_ROUTELOC0_SCLLOC_LOC6)

#define BOARD_HALL_PORT             gpioPortB     /**< GPIO port for Hall effect sensor */
#define BOARD_HALL_ENABLE_PIN       10            /**< Hall effect sensor enable pin    */
#define BOARD_HALL_OUTPUT_PIN       11            /**< Hall effect sensor output pin    */
#define BOARD_HALL_I2C_SDA_PIN      8             /**< Hall effect sensor I2C SDA pin   */
#define BOARD_HALL_I2C_SCL_PIN      9             /**< Hall effect sensor I2C SCL pin   */
#define BOARD_HALL_I2C_ROUTELOC0    (I2C_ROUTELOC0_SDALOC_LOC8 | I2C_ROUTELOC0_SCLLOC_LOC8)

#define BOARD_BMP_I2C_ROUTELOC0     (I2C_ROUTELOC0_SDALOC_LOC17 | I2C_ROUTELOC0_SCLLOC_LOC17)

#define BOARD_RHTEMP_I2C_ROUTELOC0  (I2C_ROUTELOC0_SDALOC_LOC17 | I2C_ROUTELOC0_SCLLOC_LOC17)

#define BOARD_ALS_I2C_ROUTELOC0     (I2C_ROUTELOC0_SDALOC_LOC17 | I2C_ROUTELOC0_SCLLOC_LOC17)

/* External interrupts */
#define EXTI_BUTTON1              15
#define EXTI_BUTTON0              14
#define EXTI_CCS811_INT           13
#define EXTI_IMU_INT              12
#define EXTI_UV_ALS_INT           11
#define EXTI_HALL_OUT1            10

#define BOARD_BUTTON_PORT         gpioPortD       /**< Pushbutton port                  */
#define BOARD_BUTTON_SHIFT        14              /**< Pushbutton shift value           */
#define BOARD_BUTTON_LEFT         0x01            /**< Left pushbutton value            */
#define BOARD_BUTTON_RIGHT        0x02            /**< Right pushbutton value           */
#define BOARD_BUTTON_MASK         0x03            /**< Pushbutton mask                  */
#define BOARD_BUTTON_LEFT_PORT    gpioPortD       /**< Left pushbutton port             */
#define BOARD_BUTTON_LEFT_PIN     14              /**< Left pushbutton pin              */
#define BOARD_BUTTON_RIGHT_PORT   gpioPortD       /**< Right pushbutton port            */
#define BOARD_BUTTON_RIGHT_PIN    15              /**< Right pushbutton pin             */
#define BOARD_BUTTON_INT_FLAG     0x04            /**< Pushbutton interrupt flag value  */
#define BOARD_BUTTON_INT_ENABLE   true            /**< Pushbutton interrupt enable      */

#define BOARD_HALL_PORT             gpioPortB     /**< GPIO port for Hall effect sensor */
#define BOARD_HALL_OUTPUT_PIN       11            /**< Hall effect sensor output pin    */

/** @} {end defgroup BOARD_Config_Setting} */

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif // BSPCONFIG_H
