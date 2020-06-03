/**************************************************************************//**
 * Copyright 2016 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "cslib_hwconfig.h"
#include "cslib_config.h"
#include "low_power_config.h"
#include "cslib.h"
#include "hardware_routines.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_csen.h"
#include "em_ldma.h"

/// Configures LDMA channels to set BASELINE values and read CSEN data
static LDMA_Descriptor_t LDMA_descCh0;
static LDMA_Descriptor_t LDMA_descCh1;

/// Set LDMA transfer ISR, checked/cleared by CSLIB timing callback checkTimer()
uint8_t CSENtimerTick = 0;

/// Maps aport settings to bits of CSEN active mode bit array
static const CSEN_SingleSel_TypeDef CSLIB_pinMapping[64] = {
  csenSingleSelAPORT1XCH0,
  csenSingleSelAPORT1YCH1,
  csenSingleSelAPORT1XCH2,
  csenSingleSelAPORT1YCH3,
  csenSingleSelAPORT1XCH4,
  csenSingleSelAPORT1YCH5,
  csenSingleSelAPORT1XCH6,
  csenSingleSelAPORT1YCH7,
  csenSingleSelAPORT1XCH8,
  csenSingleSelAPORT1YCH9,
  csenSingleSelAPORT1XCH10,
  csenSingleSelAPORT1YCH11,
  csenSingleSelAPORT1XCH12,
  csenSingleSelAPORT1YCH13,
  csenSingleSelAPORT1XCH14,
  csenSingleSelAPORT1YCH15,
  csenSingleSelAPORT1XCH16,
  csenSingleSelAPORT1YCH17,
  csenSingleSelAPORT1XCH18,
  csenSingleSelAPORT1YCH19,
  csenSingleSelAPORT1XCH20,
  csenSingleSelAPORT1YCH21,
  csenSingleSelAPORT1XCH22,
  csenSingleSelAPORT1YCH23,
  csenSingleSelAPORT1XCH24,
  csenSingleSelAPORT1YCH25,
  csenSingleSelAPORT1XCH26,
  csenSingleSelAPORT1YCH27,
  csenSingleSelAPORT1XCH28,
  csenSingleSelAPORT1YCH29,
  csenSingleSelAPORT1XCH30,
  csenSingleSelAPORT1YCH31,
  csenSingleSelAPORT3XCH0,
  csenSingleSelAPORT3YCH1,
  csenSingleSelAPORT3XCH2,
  csenSingleSelAPORT3YCH3,
  csenSingleSelAPORT3XCH4,
  csenSingleSelAPORT3YCH5,
  csenSingleSelAPORT3XCH6,
  csenSingleSelAPORT3YCH7,
  csenSingleSelAPORT3XCH8,
  csenSingleSelAPORT3YCH9,
  csenSingleSelAPORT3XCH10,
  csenSingleSelAPORT3YCH11,
  csenSingleSelAPORT3XCH12,
  csenSingleSelAPORT3YCH13,
  csenSingleSelAPORT3XCH14,
  csenSingleSelAPORT3YCH15,
  csenSingleSelAPORT3XCH16,
  csenSingleSelAPORT3YCH17,
  csenSingleSelAPORT3XCH18,
  csenSingleSelAPORT3YCH19,
  csenSingleSelAPORT3XCH20,
  csenSingleSelAPORT3YCH21,
  csenSingleSelAPORT3XCH22,
  csenSingleSelAPORT3YCH23,
  csenSingleSelAPORT3XCH24,
  csenSingleSelAPORT3YCH25,
  csenSingleSelAPORT3XCH26,
  csenSingleSelAPORT3YCH27,
  csenSingleSelAPORT3XCH28,
  csenSingleSelAPORT3YCH29,
  csenSingleSelAPORT3XCH30,
  csenSingleSelAPORT3YCH31
};

// Configures whether sleep mode scan uses DMA transfers or single conversions
// for capacitive sensing output
uint16_t CSLIB_autoScanComplete = 0;
uint16_t CSLIB_autoScan;

// Temporarily saves sensor data before being pushed into CSLIB_node struct
volatile uint32_t autoScanBuffer[DEF_NUM_SENSORS];
// Buffer passed back to CSLIB to copy into CSLIB_node struct
volatile uint32_t CSLIB_autoScanBuffer[DEF_NUM_SENSORS];

static void configureCSENActiveMode(void);

/**************************************************************************//**
 * CSEN pin mapping function
 *
 * This function maps CSLIB_node[] indexes to Aport CSEN_SingleSel_TypeDef
 * values by stepping through the CSEN input masks, counting enabled inputs,
 * and finding the input that corresponds to the CSLIB_node index.  The
 * function then returns the corresponding CSEN_SingleSel_TypeDef value from
 * CSLIB_pinMapping.
 * Note that CSLIB_pinMapping is only valid for default
 * em_csen.h CSEN_InputSel_TypeDef values.
 *
 *****************************************************************************/
static CSEN_SingleSel_TypeDef CSLIB_findAPortForIndex(uint8_t mux_index, uint32_t Aport0, uint32_t Aport1)
{
  uint32_t cslib_index, index, count;
  count = 0;
  cslib_index = CSLIB_muxValues[mux_index];
  for (index = 0; index < 32; index++) {
    if (Aport0 & (0x0001 << index)) {
      if (count == cslib_index) {
        return CSLIB_pinMapping[index];
      } else {
        count = count + 1;
      }
    }
  }

  for (index = 0; index < 32; index++) {
    if (Aport1 & (0x0001 << index)) {
      if (count == cslib_index) {
        return CSLIB_pinMapping[32 + index];
      } else {
        count = count + 1;
      }
    }
  }
  return csenSingleSelDefault;
}

/**************************************************************************//**
 * scanSensor callback
 *
 * Configures CSEN for a single conversion with no DMA.  In this implementation,
 * scanSensor is not used during operation because the system uses scan+DMA
 * transfers instead.  In this implementation, scanSensor is only called
 * in baseline initialization.
 *
 *****************************************************************************/
uint32_t CSLIB_scanSensorCB(uint8_t index)
{
  uint32_t return_value;
  CSEN_Init_TypeDef csen_init = CSEN_INIT_DEFAULT;
  CSEN_InitMode_TypeDef active_mode_single = CSEN_ACTIVEMODE_DEFAULT;
  active_mode_single.singleSel = CSLIB_findAPortForIndex(index,
                                                         active_mode_single.inputMask0,
                                                         active_mode_single.inputMask1);
  // Disable DMA transfer for single conversion
  active_mode_single.enableDma = false;

  active_mode_single.convSel = csenConvSelSAR;
  active_mode_single.sampleMode = csenSampleModeSingle;

  CMU_ClockEnable(cmuClock_CSEN_HF, true);

  // Clear interrupts
  CSEN_Disable(CSEN);
  CSEN->IFC = CSEN->IF;

  // Initialize the block
  CSEN_Init(CSEN, &csen_init);
  CSEN_InitMode(CSEN, &active_mode_single);

  // Convert one sample
  CSEN_Enable(CSEN);
  return_value = executeConversion();
  CSEN_Disable(CSEN);
  return return_value;
}

/**************************************************************************//**
 * Execute CSEN conversion callback
 *
 * Called after configuration of CSEN has completed in order to make
 * one conversion and return that value.
 *
 *****************************************************************************/
uint32_t executeConversion(void)
{
  CSEN_Start(CSEN);
  while (!(CSEN->IF & _CSEN_IF_CONV_MASK))
    ;
  return CSEN->DATA;
}

/**************************************************************************//**
 * Pre baseline initialization callback
 *
 * Called before a baseline for a sensor has been initialized.  In the case
 * of the CSEN implementation, no configuration is necessary.
 *
 *****************************************************************************/
void CSLIB_baselineInitEnableCB(void)
{
}

/**************************************************************************//**
 * Post baseline initialization callback
 *
 * Called after a baseline for a sensor has been initialized.
 *
 *****************************************************************************/
void CSLIB_baselineInitDisableCB(void)
{
  configureCSENActiveMode();
}

/*********************************************************
 *****************//**
 * Configure CSEN timer
 *
 * Configures self timer using LFRCO if not already configured.  Derives
 * correct self timer reload and prescale values based on user configuration.
 *
 *****************************************************************************/
void setupCSLIBClock(uint32_t clock_period, CSEN_Init_TypeDef* csen_init)
{
  uint32_t clock_freq;
  uint32_t exponent, prescaler, reload;

  if (CMU_ClockSelectGet(cmuClock_LFB) == cmuSelect_Disabled) {
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
  }

  CMU_ClockEnable(cmuClock_CSEN_LF, true);

  clock_freq = CMU_ClockFreqGet(cmuClock_CSEN_LF);

  exponent = 1;
  prescaler = 0;

  // Find the first prescaler with a reload value that is one byte in size,
  // and configure the clock with those values
  do {
    reload = clock_period * clock_freq / 1000 / exponent;
    if (reload < 255) {
      csen_init->pcReload = 255 - reload;
      csen_init->pcPrescale = (CSEN_PCPrescale_TypeDef)prescaler;
      return;
    } else {
      exponent = exponent * 2;
      prescaler = prescaler + 1;
    }
  } while ((exponent < 128));

  // If we reach here, no prescale value can yield a slow enough
  // clock frequency, and so set the reload+prescale to max period
  csen_init->pcReload = 0;
  csen_init->pcPrescale = csenPCPrescaleDiv128;
}

/**************************************************************************//**
 * Set up DMA channel for CSEN data transfer
 *
 * Configures DMA channel to read CSEN words during scan.  Channel 0
 * is used to transfer DATA from a conversion result into a buffer,
 * and channel 1 is used to load DM BASELINE with a value before
 * a scan starts.
 *
 *****************************************************************************/
static void setupCSENdataDMA(void)
{
  static const LDMA_Init_t ldma_init = LDMA_INIT_DEFAULT;

  LDMA_TransferCfg_t csenTransfer =
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMA_CH_REQSEL_SIGSEL_CSENDATA | LDMA_CH_REQSEL_SOURCESEL_CSEN);

  LDMA_Descriptor_t csenTransferDesc = LDMA_DESCRIPTOR_SINGLE_P2M_BYTE(&CSEN->DATA,
                                                                       autoScanBuffer,
                                                                       DEF_NUM_SENSORS);

  LDMA_TransferCfg_t baselineTransfer =
    LDMA_TRANSFER_CFG_PERIPHERAL(LDMA_CH_REQSEL_SIGSEL_CSENBSLN | LDMA_CH_REQSEL_SOURCESEL_CSEN);

  LDMA_Descriptor_t baselineTransferDesc = LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(autoScanBuffer, &CSEN->DMBASELINE,
                                                                           DEF_NUM_SENSORS);

  /* Initialize descriptor for CSEN DATA LDMA transfer */
  LDMA_descCh0 = csenTransferDesc;
  LDMA_descCh0.xfer.doneIfs = 0;
  LDMA_descCh0.xfer.blockSize = ldmaCtrlBlockSizeUnit1;
  LDMA_descCh0.xfer.ignoreSrec = 0;
  LDMA_descCh0.xfer.reqMode = ldmaCtrlReqModeBlock;
  LDMA_descCh0.xfer.size = ldmaCtrlSizeWord;

  /* Initialize descriptor for CSEN BASELINE LDMA transfer */
  LDMA_descCh1 = baselineTransferDesc;
  LDMA_descCh1.xfer.doneIfs = 0;
  LDMA_descCh1.xfer.blockSize = ldmaCtrlBlockSizeUnit1;
  LDMA_descCh1.xfer.ignoreSrec = 0;
  LDMA_descCh1.xfer.reqMode = ldmaCtrlReqModeBlock;
  LDMA_descCh1.xfer.size = ldmaCtrlSizeWord;

  /* Initialize the LDMA with default values. */
  LDMA_Init(&ldma_init);
  LDMA_StartTransfer(1, &baselineTransfer, &LDMA_descCh1);
  LDMA_StartTransfer(0, &csenTransfer, &LDMA_descCh0);
}

/**************************************************************************//**
 * Configure CSEN and LDMA for active mode
 *
 * Function uses contents of cslib_hwconfig.h to set up active mode scanning.
 *
 *****************************************************************************/
static void configureCSENActiveMode(void)
{
  CSEN_Init_TypeDef csen_init = CSEN_INIT_DEFAULT;

  CSEN_InitMode_TypeDef active_mode = CSEN_ACTIVEMODE_DEFAULT;

  setupCSLIBClock(DEF_ACTIVE_MODE_PERIOD, &csen_init);

  // Set converter and scan configuration
  CMU_ClockEnable(cmuClock_CSEN_HF, true);
  CSEN_Init(CSEN, &csen_init);

  CSEN_InitMode(CSEN, &active_mode);

  CMU_ClockEnable(cmuClock_LDMA, true);

  setupCSENdataDMA();

  // Start a conversion
  CSEN_Enable(CSEN);
}

/**************************************************************************//**
 * LDMA interrupt handler
 *
 * The LDMA is used to signal that a CSEN scan has completed and
 * and that the LDMA has copied all scan results.  The ISR sets up the
 * LDMA for the next transfer and sets flags for timing and sample processing.
 *
 *****************************************************************************/
void LDMA_IRQHandler(void)
{
  uint32_t pending;
  uint32_t index;
  /* Read and clear interrupt source */
  pending = LDMA_IntGetEnabled();

  if ((pending & 3) == 3) {
    /* Setup LDMA for next transfer, common for single and scan mode */
    LDMA->IFC = 0x03;
    for ( index = 0; index < DEF_NUM_SENSORS; index++ ) {
      CSLIB_autoScanBuffer[index] = autoScanBuffer[CSLIB_muxValues[index]];
    }
    setupCSENdataDMA();
    CSLIB_autoScanComplete = 1;
    CSENtimerTick = 1;
  }
}

/**************************************************************************//**
 * CSEN interrupt service routine
 *
 * This ISR is only entered when exiting EM2.  ISR should clear
 * and disable CSEN interrupts before exiting.
 * Note that this function could be used to signal application
 * layer firmware that a candidate touch event has happened.
 *
 *****************************************************************************/
void CSEN_IRQHandler(void)
{
  CSEN->IFC = CSEN->IF;                // Clear interrupt
  NVIC_DisableIRQ(CSEN_IRQn);
  NVIC_ClearPendingIRQ(CSEN_IRQn);
}

/**************************************************************************//**
 * Callback for active mode scan configuration
 *
 * This is a top-level call to configure the sensor to its operational state
 * during active mode.
 *
 *****************************************************************************/
void CSLIB_configureSensorForActiveModeCB(void)
{
  configureCSENActiveMode();           // Initialize CSEN and LDMA
  CSLIB_autoScan = 1;                  // Signal to CSLIB to use auto scan state machine
}
