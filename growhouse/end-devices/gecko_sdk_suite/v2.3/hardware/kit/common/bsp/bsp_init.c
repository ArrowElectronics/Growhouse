/***************************************************************************//**
 * @file bsp_init.c
 * @brief Board support package device initialization
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#include "bsp_init.h"
#include "hal-config.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#if BSP_LED_PRESENT
#include "bsp.h"
#endif
#if HAL_IOEXP_ENABLE
#include "bsp.h"
#include "bsp_stk_ioexp.h"
#endif
#if HAL_I2CSENSOR_ENABLE
#include "i2cspm.h"
#endif
#if defined(BSP_SERIAL_APP_PORT)
#include "retargetserial.h"
#endif

void BSP_initDevice(void)
{
  // Device errata
  CHIP_Init();

#if defined (_EMU_DCDCCTRL_MASK)
  // Set up DC-DC converter
  BSP_initDcdc();
#endif // _EMU_DCDCCTRL_MASK

  // Set up clocks
  BSP_initClocks();

  // Set up energy mode configuration
  BSP_initEmu();
}

#if defined (_EMU_DCDCCTRL_MASK)
void BSP_initDcdc(void)
{
#if BSP_DCDC_PRESENT
  EMU_DCDCInit_TypeDef dcdcInit = BSP_DCDC_INIT;
  #if HAL_DCDC_BYPASS
  dcdcInit.dcdcMode = emuDcdcMode_Bypass;
  #endif
  EMU_DCDCInit(&dcdcInit);
#else
  EMU_DCDCPowerOff();
#endif
}
#endif // _EMU_DCDCCTRL_MASK

void BSP_initEmu(void)
{
#if HAL_EMU_ENABLE
#if defined(_EMU_CMD_EM01VSCALE0_MASK)
  EMU_EM01Init_TypeDef em01Init = EMU_EM01INIT_DEFAULT;
  em01Init.vScaleEM01LowPowerVoltageEnable = HAL_EMU_EM01_VSCALE;
  EMU_EM01Init(&em01Init);
#endif // _EMU_CMD_EM01VSCALE0_MASK

  EMU_EM23Init_TypeDef em23init = EMU_EM23INIT_DEFAULT;
#if HAL_EMU_EM23_VREG
  em23init.em23VregFullEn = true;
#endif // HAL_EMU_EM23_VREG
#if defined(_EMU_CTRL_EM23VSCALE_MASK)
  #if HAL_EMU_EM23_VSCALE == HAL_EMU_EM23_VSCALE_FASTWAKEUP
  em23init.vScaleEM23Voltage = emuVScaleEM23_FastWakeup;
  #elif HAL_EMU_EM23_VSCALE == HAL_EMU_EM23_VSCALE_LOWPOWER
  em23init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
  #endif // HAL_EMU_EM23_VSCALE
#endif // _EMU_CTRL_EM23VSCALE_MASK
  EMU_EM23Init(&em23init);
#endif //HAL_EMU_ENABLE
}

void BSP_initClocks(void)
{
  // --------------------------------
  // Initialize HFXO if present

#if BSP_CLK_HFXO_PRESENT
  // HFXO
  CMU_HFXOInit_TypeDef hfxoInit = BSP_CLK_HFXO_INIT;

#if defined(_CMU_HFXOCTRL_MASK)
#if defined(BSP_CLK_HFXO_CTUNE) && BSP_CLK_HFXO_CTUNE > 0
  hfxoInit.ctuneStartup = BSP_CLK_HFXO_CTUNE;
  hfxoInit.ctuneSteadyState = BSP_CLK_HFXO_CTUNE;
#elif BSP_CLK_HFXO_CTUNE_TOKEN
  #error "Getting CTUNE from manufacturing token not supported by this driver"
#endif // BSP_CLK_HFXO_CTUNE
#endif //_CMU_HFXOCTRL_MASK
  CMU_HFXOInit(&hfxoInit);

  // Set system HFXO frequency
  SystemHFXOClockSet(BSP_CLK_HFXO_FREQ);
#endif // BSP_CLK_HFXO_PRESENT

  // --------------------------------
  // Enable HFXO if selected as HFCLK

#if (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFXO)
  // Enable HFXO oscillator, and wait for it to be stable
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

#if defined(HAL_CLK_HFXO_AUTOSTART) && HAL_CLK_HFXO_AUTOSTART == HAL_CLK_HFXO_AUTOSTART_SELECT
  // Automatically start and select HFXO
  CMU_HFXOAutostartEnable(0, true, true);
#else
#if defined(HAL_CLK_HFXO_AUTOSTART) && HAL_CLK_HFXO_AUTOSTART == HAL_CLK_HFXO_AUTOSTART_START
  // Automatically start HFXO
  CMU_HFXOAutostartEnable(0, true, false);
#endif // HAL_CLK_HFXO_AUTOSTART
  // Use HFXO as high frequency clock
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
#endif // HAL_CLK_HFXO_AUTOSTART

  // HFRCO not needed when using HFXO
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
#elif (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFRCO)
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
#else
  #error "Must define HAL_CLK_HFCLK_SOURCE"
#endif // HAL_CLK_HFCLK_SOURCE

  // --------------------------------
  // Initialize LFXO if present

#if BSP_CLK_LFXO_PRESENT
  // LFXO
  CMU_LFXOInit_TypeDef lfxoInit = BSP_CLK_LFXO_INIT;

#if defined(_CMU_HFXOCTRL_MASK) && defined(BSP_CLK_LFXO_CTUNE) && BSP_CLK_LFXO_CTUNE > 0
  lfxoInit.ctune = BSP_CLK_LFXO_CTUNE;
#endif
  CMU_LFXOInit(&lfxoInit);

  // Set system LFXO frequency
  SystemLFXOClockSet(BSP_CLK_LFXO_FREQ);
#endif

  // --------------------------------
  // Enable LFXO if selected as LFCLK

#if (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)  \
  || (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO) \
  || (HAL_CLK_LFCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO) \
  || (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  #if !BSP_CLK_LFXO_PRESENT
    #error "Cannot select LFXO when LFXO is not present"
  #endif
#endif

  // LFA
#if (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
#elif (HAL_CLK_LFACLK_SOURCE == HAL_CLK_LFCLK_SOURCE_HFLE)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_HFCLKLE);
#endif

  // LFB
#if (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_ULFRCO);
#elif (HAL_CLK_LFBCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_HFLE)
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_HFCLKLE);
#endif

  // LFC
#if (HAL_CLK_LFCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFC, cmuSelect_LFXO);
#elif (HAL_CLK_LFCCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFC, cmuSelect_LFRCO);
#endif

  // LFE
#if (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFXO);
#elif (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#elif (HAL_CLK_LFECLK_SOURCE == HAL_CLK_LFCLK_SOURCE_ULFRCO)
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_ULFRCO);
#endif
}

void BSP_initBoard(void)
{
  // Board functionality necessarily needs GPIO
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize IO expander
#if HAL_IOEXP_ENABLE
  BSP_Init(BSP_INIT_IOEXP);
#endif

  // Initialize LEDs
#if BSP_LED_PRESENT
  #if HAL_IOEXP_ENABLE
  BSP_PeripheralAccess(BSP_IOEXP_LEDS, true);
  #else
  BSP_LedsInit();
  #endif
#endif

  // Initialize sensors
#if HAL_I2CSENSOR_ENABLE
  #if HAL_IOEXP_ENABLE
  BSP_PeripheralAccess(BSP_IOEXP_SENSORS, true);
  #endif
  #if defined(BSP_I2CSENSOR_ENABLE_PORT)
  GPIO_PinModeSet(BSP_I2CSENSOR_ENABLE_PORT, BSP_I2CSENSOR_ENABLE_PIN, gpioModePushPull, 1);
  #endif

  I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_SENSOR;
  I2CSPM_Init(&i2cInit);
#endif

  // Configure VCOM enable
#if HAL_VCOM_ENABLE || HAL_IOEXP_VCOM_ENABLE
  #if HAL_IOEXP_VCOM_ENABLE
  BSP_PeripheralAccess(BSP_IOEXP_VCOM, true);
  #endif
  #if defined(BSP_VCOM_ENABLE_PORT)
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, 1);
  #endif
#endif

  // Initialize serial port
#if defined(BSP_SERIAL_APP_PORT)
  RETARGET_SerialInit();
#endif
}
