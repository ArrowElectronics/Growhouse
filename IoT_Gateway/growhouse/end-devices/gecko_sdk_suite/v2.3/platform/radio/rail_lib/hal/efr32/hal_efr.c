/***************************************************************************//**
 * @file hal_efr.c
 * @brief This file contains EFR32 specific HAL code to handle chip startup.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_init.h"
#include "em_chip.h"

#include "rail.h"
#include "rail_chip_specific.h"
#include "rail_config.h"
#include "hal_common.h"

#if HAL_FEM_ENABLE
#include "util/plugin/plugin-common/fem-control/fem-control.h"
#endif

static void boardDisableSpiFlash(void);
static void boardLowPowerInit(void);
static void usecDelay(uint32_t usecs);

#ifdef RAIL_PA_CONVERSIONS
#include RAIL_PA_CONVERSIONS

#ifndef BSP_PA_VOLTAGE // Work with older Gecko SDK
#define BSP_PA_VOLTAGE HAL_PA_VOLTAGE
#endif//BSP_PA_VOLTAGE
#if BSP_PA_VOLTAGE == 3300
RAIL_DECLARE_TX_POWER_VBAT_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
#else
RAIL_DECLARE_TX_POWER_DCDC_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
#endif
#endif

void halInitChipSpecific(void)
{
  BSP_initDevice();

  BSP_initBoard();

#if HAL_PTI_ENABLE
  RAIL_PtiConfig_t railPtiConfig = {
#if HAL_PTI_MODE == HAL_PTI_MODE_SPI
    .mode = RAIL_PTI_MODE_SPI,
#elif HAL_PTI_MODE == HAL_PTI_MODE_UART
    .mode = RAIL_PTI_MODE_UART,
#elif HAL_PTI_MODE == HAL_PTI_MODE_UART_ONEWIRE
    .mode = RAIL_PTI_MODE_UART_ONEWIRE,
#else
    .mode = RAIL_PTI_MODE_DISABLED,
#endif
    .baud = HAL_PTI_BAUD_RATE,
    .doutLoc = BSP_PTI_DOUT_LOC,
    .doutPort = BSP_PTI_DOUT_PORT,
    .doutPin = BSP_PTI_DOUT_PIN,
#ifdef BSP_PTI_DCLK_LOC
    .dclkLoc = BSP_PTI_DCLK_LOC,
    .dclkPort = BSP_PTI_DCLK_PORT,
    .dclkPin = BSP_PTI_DCLK_PIN,
#endif
    .dframeLoc = BSP_PTI_DFRAME_LOC,
    .dframePort = BSP_PTI_DFRAME_PORT,
    .dframePin = BSP_PTI_DFRAME_PIN
  };

  RAIL_ConfigPti(RAIL_EFR32_HANDLE, &railPtiConfig);
#endif // HAL_PTI_ENABLE

  // Only create and save the curves if the customer wants them
  #ifdef RAIL_PA_CONVERSIONS
  RAIL_TxPowerCurvesConfig_t txPowerCurvesConfig = { curves24Hp, curvesSg, curves24Lp, piecewiseSegments };
  RAIL_InitTxPowerCurves(&txPowerCurvesConfig);
  #endif

  /* Initialize other chip clocks, choose the best available clock source in this order:
        LFXO    - Crystal
        LFRCO   - Regular RC Oscillator
   */

  // Default to LFRCO, it's the only option always present,
  // the code below will pick a better option if available
  CMU_Osc_TypeDef lowFrequencyClockSource = cmuOsc_LFRCO;
  CMU_Select_TypeDef lowFrequencyClockSelect = cmuSelect_LFRCO;

  if (CMU->STATUS & CMU_STATUS_LFXOENS) {
    // Is the LFXO already being used? if so, use it
    lowFrequencyClockSource = cmuOsc_LFXO;
    lowFrequencyClockSelect = cmuSelect_LFXO;
  }

  // Enable necessary clocks and set clock selections
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_OscillatorEnable(lowFrequencyClockSource, true, true);
  CMU_ClockSelectSet(cmuClock_LFA, lowFrequencyClockSelect);
  CMU_ClockSelectSet(cmuClock_LFB, lowFrequencyClockSelect);
  CMU_ClockSelectSet(cmuClock_LFE, lowFrequencyClockSelect);

#if (HAL_FEM_ENABLE)
  initFem();
#endif

  // Disable any unused peripherals to ensure we enter a low power mode
  boardLowPowerInit();
}

static void boardLowPowerInit(void)
{
  boardDisableSpiFlash();
}

#define CMD_POWER_DOWN                      (0xB9)
#define CMD_POWER_UP                        (0xAB)

/** Synchronous mode init structure. */
static const USART_InitSync_TypeDef usartInit =
{
  /** Specifies whether TX and/or RX shall be enabled when init completed. */
  //USART_Enable_TypeDef    enable;
  usartEnable,

  /**
   * USART/UART reference clock assumed when configuring baudrate setup. Set
   * it to 0 if currently configurated reference clock shall be used.
   */
  //uint32_t                refFreq;
  38400000U,

  /** Desired baudrate. */
  //uint32_t                baudrate;
  12000000U,

  /** Number of databits in frame. */
  //USART_Databits_TypeDef  databits;
  usartDatabits8,

  /** Select if to operate in master or slave mode. */
  //bool                    master;
  true,

  /** Select if to send most or least significant bit first. */
  //bool                    msbf;
  true,

  /** Clock polarity/phase mode. */
  //USART_ClockMode_TypeDef clockMode;
  usartClockMode0,

  /** Enable USART Rx via PRS. */
  //bool                    prsRxEnable;
  false,

  /** Select PRS channel for USART Rx. (Only valid if prsRxEnable is true). */
  //USART_PrsRxCh_TypeDef   prsRxCh;
  usartPrsRxCh0,

  /** Enable AUTOTX mode. Transmits as long as RX is not full.
   *  If TX is empty, underflows are generated. */
  //bool                    autoTx;
  false,

  /** Auto CS enabling */
  //bool autoCsEnable;
  false,

  /** Auto CS hold time in baud cycles */
  //uint8_t autoCsHold;
  0,

  /** Auto CS setup time in baud cycles */
  //uint8_t autoCsSetup;
  0,
};

static void boardDisableSpiFlash(void)
{
  // Enable clocks needed for using the USART
  // cmuClock_GPIO is enabled by BSP_initBoard above
  CMU_ClockEnable(cmuClock_HFPER, true);
  CMU_ClockEnable(cmuClock_USART1, true);

  USART_InitSync(USART1, &usartInit);

  USART1->ROUTEPEN  =   USART_ROUTEPEN_TXPEN
                      | USART_ROUTEPEN_RXPEN
                      | USART_ROUTEPEN_CLKPEN;

  USART1->ROUTELOC0 =   USART_ROUTELOC0_TXLOC_LOC11
                      | USART_ROUTELOC0_RXLOC_LOC11
                      | USART_ROUTELOC0_CLKLOC_LOC11;

  // MOSI
  GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 0);
  // MISO
  GPIO_PinModeSet(gpioPortC, 7, gpioModeInputPull, 0);
  // CLK
  GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 0);
  // CS#
  GPIO_PinModeSet(gpioPortA, 4, gpioModePushPull, 1);

  // Delay for 10ms to wait for the part to be ready to talk to
  usecDelay(10000);

  // Send the power on command so we can talk to it
  GPIO_PinOutClear(gpioPortA, 4);
  USART_Tx(USART1, CMD_POWER_UP);
  USART_Rx(USART1);
  GPIO_PinOutSet(gpioPortA, 4);

  // Delay for 30 usecs to make sure it's awake
  usecDelay(30);

  // Finally, send the power down command to put it in low power mode
  GPIO_PinOutClear(gpioPortA, 4);
  USART_Tx(USART1, CMD_POWER_DOWN);
  USART_Rx(USART1);
  GPIO_PinOutSet(gpioPortA, 4);
}

// Simple microsecond delay routine that has been calibrated for an EFR32
// running at 38.4MHz. This code should be moved to a more clean location
// along with all board initialization in the future.
static const uint32_t iterations_per_microsecond = 3;
static void usecDelay(uint32_t usecs)
{
  volatile uint64_t iterations = iterations_per_microsecond * usecs;

  while (iterations--) ;
}

// Create defines for the different PRS signal sources as they vary per chip
#if _SILICON_LABS_32B_SERIES_1_CONFIG == 1
// Defines for EFR32xG1 chips
#define _PRS_CH_CTRL_SOURCESEL_FRC     0x00000025UL
#else
// Defines for EFR32xG12 and newer chips
#define _PRS_CH_CTRL_SOURCESEL_FRC     0x00000055UL
#endif

/**
 * Define the signals that are supported for debug in railtest. These are chip
 * specific because on some chips these are supported by the PRS while on others
 * the debugging must come from the library directly.
 */
static const debugSignal_t debugSignals[] =
{
  {
    .name = "RXACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_RX & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_RX & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "TXACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_TX & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_TX & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "LNAEN",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_LNAEN & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_LNAEN & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "PAEN",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_PAEN & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_PAEN & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "RACACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_ACTIVE & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_ACTIVE & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "PTIDATA",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = 0x00,
        .source = _PRS_CH_CTRL_SOURCESEL_FRC
      }
    }
  },
  {
    .name = "FRAMEDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_FRAMEDET & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_FRAMEDET & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "PREAMBLEDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_PREDET & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_PREDET & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "TIMINGDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_TIMDET & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_TIMDET & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "FRAMESENT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_FRAMESENT & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_FRAMESENT & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "SYNCSENT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_SYNCSENT & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_SYNCSENT & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
};

const debugSignal_t* halGetDebugSignals(uint32_t *size)
{
  if (size != NULL) {
    *size = sizeof(debugSignals) / sizeof(debugSignal_t);
  }
  return debugSignals;
}

/**
 * Define the pins that are supported for debugging on the EFR32. This includes
 * PF2, PF3, PF4, PF5, PC10, and PC11. Along with these pins there are specific
 * PRS channels that will be used to output debug information on these pins.
 * This is all for debug and very specific to the EFR32.
 */
static const debugPin_t debugPins[] = {
  {
    .name = "PC10", // EFR32xG1/EFR32xG12/EFR32xG13 - EXP_HEADER15, WSTK_P12
    .prsChannel = 9, // PRS 0/12 9/15 10/4 11/3
    .prsLocation = 15,
    .gpioPort = gpioPortC,
    .gpioPin = 10
  },
  {
    .name = "PC11", // EFR32xG1/EFR32xG12/EFR32xG13 - EXP_HEADER16, WSTK_P13
    .prsChannel = 10, // PRS 0/13 9/16 10/5 11/4
    .prsLocation = 5,
    .gpioPort = gpioPortC,
    .gpioPin = 11
  },
  {
    .name = "PF2", // EFR32xG1/EFR32xG13 - WSTK_P28 (SWO)
    .prsChannel = 0, // PRS 0/2 1/1 2/0 3/7
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 2
  },
  {
    .name = "PF3", // EFR32xG1/EFR32xG13 - EXP_HEADER13, WSTK_10 (TDI) [EFR32xG12 PC9]
    .prsChannel = 1, // PRS 0/3 1/2 2/1 3/0
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 3
  },
  {
    .name = "PF4", // EFR32xG1/EFR32xG13 - EXP_HEADER11, WSTK_P8 (LED0) [EFR32xG12 PD12]
    .prsChannel = 2, // PRS 0/4 1/3 2/2 3/1
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 4
  },
  {
    .name = "PF5", // EFR32xG1/EFR32xG13 - WSTK_P32 (LED1)
    .prsChannel = 3, // PRS 0/5 1/4 2/3 3/2
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 5
  },
  {
    .name = "PC9", // EFR32xG12 - EXP_HEADER13, WSTK_P10 [EFR32xG1/EFR32xG13 PF3]
    .prsChannel = 11, // PRS 0/11 9/14 10/3 11/2
    .prsLocation = 2,
    .gpioPort = gpioPortC,
    .gpioPin = 9
  },
  {
    .name = "PD9", // EFR32xG12 - EXP_HEADER5, WSTK_P2 [EFR32xG1/EFR32xG13 PA3 (VCOM_CTS)]
    .prsChannel = 3, // PRS 3/8 4/0 5/6 6/11
    .prsLocation = 8,
    .gpioPort = gpioPortD,
    .gpioPin = 9
  },
  {
    .name = "PD10", // EFR32xG12 - EXP_HEADER7, WSTK_P4 [EFR32xG1/EFR32xG13 PF6 (BUTTON0)]
    .prsChannel = 4, // PRS 3/9 4/1 5/0 6/12
    .prsLocation = 1,
    .gpioPort = gpioPortD,
    .gpioPin = 10
  },
  {
    .name = "PD11", // EFR32xG12 - EXP_HEADER9, WSTK_P6 [EFR32xG1/EFR32xG13 PF7 (BUTTON1)]
    .prsChannel = 5, // PRS 3/10 4/2 5/1 6/13
    .prsLocation = 1,
    .gpioPort = gpioPortD,
    .gpioPin = 11
  },
  {
    .name = "PD12", // EFR32xG12 - EXP_HEADER11, WSTK_P8 [EFR32xG1/EFR32xG13 PF4 (LED0)]
    .prsChannel = 6, // PRS 3/11 4/3 5/2 6/14
    .prsLocation = 14,
    .gpioPort = gpioPortD,
    .gpioPin = 12
  },
};

const debugPin_t* halGetDebugPins(uint32_t *size)
{
  if (size != NULL) {
    *size = sizeof(debugPins) / sizeof(debugPin_t);
  }
  return debugPins;
}

void halDisablePrs(uint8_t channel)
{
  // Turn the specified PRS channel off
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  0);
}

void halEnablePrs(uint8_t channel, uint8_t loc, uint8_t source, uint8_t signal)
{
  volatile uint32_t *routeLocPtr;

  // Make sure the PRS is on and clocked
  CMU_ClockEnable(cmuClock_PRS, true);

  // Make sure this PRS channel is off while reconfiguring
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  0);

  PRS->CH[channel].CTRL = signal << _PRS_CH_CTRL_SIGSEL_SHIFT
                          | source << _PRS_CH_CTRL_SOURCESEL_SHIFT
                          | PRS_CH_CTRL_EDSEL_OFF;

  // Configure the output location for this PRS channel
  routeLocPtr   = &PRS->ROUTELOC0 + (channel / 4);
  *routeLocPtr &= ~(0xFFUL << (_PRS_ROUTELOC0_CH1LOC_SHIFT
                               * (channel % 4)));
  *routeLocPtr |= loc << (_PRS_ROUTELOC0_CH1LOC_SHIFT
                          * (channel % 4));

  // Set the enable bit for this PRS channel
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  1);
}
