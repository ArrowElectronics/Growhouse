/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
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

#ifndef BSPCONFIG_H
#define BSPCONFIG_H

#define BSP_STK
#define BSP_WSTK
#define BSP_WSTK_BRD4170A

#define BSP_BCC_USART             USART0
#define BSP_BCC_CLK               cmuClock_USART0
#define BSP_BCC_TX_LOCATION       USART_ROUTELOC0_TXLOC_LOC0
#define BSP_BCC_RX_LOCATION       USART_ROUTELOC0_RXLOC_LOC0
#define BSP_BCC_TXPORT            gpioPortA
#define BSP_BCC_TXPIN             0
#define BSP_BCC_RXPORT            gpioPortA
#define BSP_BCC_RXPIN             1

#define BSP_BCC_ENABLE_PORT       gpioPortA
#define BSP_BCC_ENABLE_PIN        5                 /* VCOM_ENABLE */

#define BSP_DISP_ENABLE_PORT      gpioPortD
#define BSP_DISP_ENABLE_PIN       15                /* MemLCD display enable */

#define BSP_SENSOR_ENABLE_PORT    gpioPortB
#define BSP_SENSOR_ENABLE_PIN     10

#define BSP_GPIO_LEDS
#define BSP_NO_OF_LEDS            2
#define BSP_GPIO_LED0_PORT        gpioPortF
#define BSP_GPIO_LED0_PIN         4
#define BSP_GPIO_LED1_PORT        gpioPortF
#define BSP_GPIO_LED1_PIN         5
#define BSP_GPIO_LEDARRAY_INIT    { { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, { BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN } }

#define BSP_GPIO_BUTTONS
#define BSP_NO_OF_BUTTONS         2
#define BSP_GPIO_PB0_PORT         gpioPortF
#define BSP_GPIO_PB0_PIN          6
#define BSP_GPIO_PB1_PORT         gpioPortF
#define BSP_GPIO_PB1_PIN          7

#define BSP_GPIO_BUTTONARRAY_INIT { { BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN }, { BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN } }

#define BSP_INIT_DEFAULT          0

#define BSP_LFXO_CTUNE            32U
#define BSP_HFXO_CTUNE            340U

#if !defined(EMU_DCDCINIT_WSTK_DEFAULT)
/* Use emlib defaults */
#define EMU_DCDCINIT_WSTK_DEFAULT EMU_DCDCINIT_DEFAULT
#endif

#if !defined(CMU_HFXOINIT_WSTK_DEFAULT)
#define CMU_HFXOINIT_WSTK_DEFAULT                  \
  {                                                \
    false,      /* Low-noise mode for EFR32 */     \
    false,      /* deprecated/unused */            \
    false,      /* deprecated/unused */            \
    false,      /* deprecated/unused */            \
    _CMU_HFXOSTARTUPCTRL_CTUNE_DEFAULT,            \
    BSP_HFXO_CTUNE, /* Steady-state CTUNE value */ \
    _CMU_HFXOSTEADYSTATECTRL_REGISH_DEFAULT,       \
    _CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT,     \
    _CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT, \
    0x0,        /* unused */                       \
    _CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_DEFAULT,  \
    _CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_DEFAULT,   \
    0x4,        /* Recommended steady timeout */   \
    _CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_DEFAULT,   \
    cmuOscMode_Crystal,                            \
  }
#endif

#if !defined(RADIO_PTI_INIT)
#define RADIO_PTI_INIT                                                  \
  {                                                                     \
    RADIO_PTI_MODE_UART,    /* Simplest output mode is UART mode */     \
    1600000,                /* Choose 1.6 MHz for best compatibility */ \
    6,                      /* DOUT location 6 = PB12 */                \
    gpioPortB,                                                          \
    12,                                                                 \
    6,                      /* DCLK location 6 = PB11 */                \
    gpioPortB,                                                          \
    11,                                                                 \
    6,                      /* DFRAME location 6 = PB13 */              \
    gpioPortB,                                                          \
    13,                                                                 \
  }
#endif

#if !defined(RAIL_PTI_CONFIG)
#define RAIL_PTI_CONFIG                                                 \
  {                                                                     \
    RAIL_PTI_MODE_UART,     /* Simplest output mode is UART mode */     \
    1600000,                /* Choose 1.6 MHz for best compatibility */ \
    6,                      /* DOUT location 6 = PB12 */                \
    gpioPortB,                                                          \
    12,                                                                 \
    6,                      /* DCLK location 6 = PB11 */                \
    gpioPortB,                                                          \
    11,                                                                 \
    6,                      /* DFRAME location 6 = PB13 */              \
    gpioPortB,                                                          \
    13,                                                                 \
  }
#endif

#if !defined(RADIO_PA_2P4_INIT)
#define RADIO_PA_2P4_INIT                                    \
  {                                                          \
    PA_SEL_2P4_HP,    /* Power Amplifier mode */             \
    PA_VOLTMODE_VBAT, /* Power Amplifier vPA Voltage mode */ \
    190,              /* Desired output power in dBm * 10 */ \
    0,                /* Output power offset in dBm * 10 */  \
    10,               /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_2P4_CONFIG)
#define RAIL_PA_2P4_CONFIG                                            \
  {                                                                   \
    RAIL_TX_POWER_MODE_2P4_HP, /* Power Amplifier mode */             \
    1800,                      /* Power Amplifier vPA Voltage mode */ \
    10,                        /* Desired ramp time in us */          \
  }
#endif

#if !defined(RADIO_PA_SUBGIG_INIT)
#define RADIO_PA_SUBGIG_INIT                                 \
  {                                                          \
    PA_SEL_SUBGIG,    /* Power Amplifier mode */             \
    PA_VOLTMODE_VBAT, /* Power Amplifier vPA Voltage mode */ \
    190,              /* Desired output power in dBm * 10 */ \
    0,                /* Output power offset in dBm * 10 */  \
    10,               /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_SUBGIG_CONFIG)
#define RAIL_PA_SUBGIG_CONFIG                                         \
  {                                                                   \
    RAIL_TX_POWER_MODE_SUBGIG, /* Power Amplifier mode */             \
    1800,                      /* Power Amplifier vPA Voltage mode */ \
    10,                        /* Desired ramp time in us */          \
  }
#endif

#if !defined(RAIL_PA_DEFAULT_POWER)
#define RAIL_PA_DEFAULT_POWER 190
#endif

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif /* BSPCONFIG_H */
