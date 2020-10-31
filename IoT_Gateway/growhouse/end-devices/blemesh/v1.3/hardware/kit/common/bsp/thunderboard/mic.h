/***************************************************************************//**
 * @file mic.h
 * @brief Driver for the SPV1840LR5H-B MEMS Microphone
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

#ifndef MIC_H
#define MIC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "mic_config.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @addtogroup Mic
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
#define MIC_ADC_ACQ_TIME      MIC_CONFIG_ADC_ACQ_TIME    /**< ADC acquisition time  */
#define MIC_ADC_CLOCK_FREQ    MIC_CONFIG_ADC_CLOCK_FREQ  /**< ADC clock frequency   */
/** @endcond */

/**************************************************************************//**
* @name Error Codes
* @{
******************************************************************************/
#define MIC_OK                0                          /**< No errors */
/**@}*/

uint32_t  MIC_init           (uint32_t fs, uint16_t *sampleBuffer, size_t len);
void      MIC_deInit         (void);
void      MIC_start          (uint32_t nsamples);
bool      MIC_isBusy         (void);

float     MIC_getSoundLevel  (float *var);
float     MIC_getMean        (void);
uint16_t *MIC_getSampleBuffer(void);

/** @} */
/** @} */

#endif // MIC_H
