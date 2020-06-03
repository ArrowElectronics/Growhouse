/***************************************************************************//**
 * @file mic_config.h
 * @brief SPH0645LM4H-B MEMS Microphone configuration file
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

#ifndef MIC_CONFIG_H
#define MIC_CONFIG_H

#include "em_adc.h"

#define MIC_CONFIG_ADC_POSSEL           adcPosSelAPORT2XCH23
#define MIC_CONFIG_ADC_ACQ_TIME         adcAcqTime1
#define MIC_CONFIG_ADC_CLOCK_FREQ       1000000

#define MIC_CONFIG_USE_LETIMER          0

#if MIC_CONFIG_USE_LETIMER
   #define MIC_CONFIG_TIMER                LETIMER0
   #define MIC_CONFIG_TIMER_CMU_CLK        cmuClock_LETIMER0

   #define MIC_CONFIG_PRS_CH               0
   #define MIC_CONFIG_ADC_PRSSEL           adcPRSSELCh0
   #define MIC_CONFIG_PRS_SOURCE           PRS_CH_CTRL_SOURCESEL_LETIMER0
   #define MIC_CONFIG_PRS_SIGNAL           PRS_CH_CTRL_SIGSEL_LETIMER0CH0
#else
   #define MIC_CONFIG_PRS_CH               4
   #define MIC_CONFIG_ADC_PRSSEL           adcPRSSELCh4
   #define MIC_CONFIG_PRS_SOURCE           PRS_CH_CTRL_SOURCESEL_CMU
   #define MIC_CONFIG_PRS_SIGNAL           PRS_CH_CTRL_SIGSEL_CMUCLKOUT1
#endif

#define MIC_CONFIG_DMA_CH     0

#define MIC_PORT_DATA         gpioPortC
#define MIC_PIN_DATA          7
#define MIC_PORT_CLK          gpioPortC
#define MIC_PIN_CLK           8
#define MIC_PORT_WS           gpioPortC
#define MIC_PIN_WS            9

#define MIC_USART             USART1
#define MIC_USART_CLK         cmuClock_USART1

#define MIC_USART_LOC_DATA    USART_ROUTELOC0_RXLOC_LOC11
#define MIC_USART_LOC_CLK     USART_ROUTELOC0_CLKLOC_LOC11
#define MIC_USART_LOC_WS      USART_ROUTELOC0_CSLOC_LOC11

#define MIC_DMA_CH            0
#define MIC_DMA_LEFT_SIGNAL   ldmaPeripheralSignal_USART1_RXDATAV
#define MIC_DMA_RIGHT_SIGNAL  ldmaPeripheralSignal_USART1_RXDATAVRIGHT

#endif // MIC_CONFIG_H
