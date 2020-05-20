/***************************************************************************//**
 * @file
 * @brief BRD4104A specific configuration for the display driver for
 *        the Sharp Memory LCD model LS013B7DH03.
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

#ifndef DISPLAYLS013B7DH03CONFIG_H
#define DISPLAYLS013B7DH03CONFIG_H

#include "displayconfigapp.h"

/* Display device name. */
#define SHARP_MEMLCD_DEVICE_NAME   "Sharp LS013B7DH03 #1"

/* LCD and SPI GPIO pin connections on the kit. */
#define LCD_PORT_SCLK             (gpioPortC)  /* EFM_DISP_SCLK on PC8 */
#define LCD_PIN_SCLK              (8)
#define LCD_PORT_SI               (gpioPortC)  /* EFM_DISP_MOSI on PC6 */
#define LCD_PIN_SI                (6)
#define LCD_PORT_SCS              (gpioPortD)  /* EFM_DISP_CS on PD14 */
#define LCD_PIN_SCS               (14)
#define LCD_PORT_EXTCOMIN         (gpioPortD)  /* EFM_DISP_COM on PD13 */
#define LCD_PIN_EXTCOMIN          (13)
#define LCD_PORT_DISP_SEL         (gpioPortD)  /* EFM_DISP_ENABLE on PD15 */
#define LCD_PIN_DISP_SEL          (15)

/* PRS settings for polarity inversion extcomin auto toggle.  */
#define LCD_AUTO_TOGGLE_PRS_CH    (4)  /* PRS channel 4.      */
#define LCD_AUTO_TOGGLE_PRS_ROUTELOC()  PRS->ROUTELOC1 = \
  ((PRS->ROUTELOC1 & ~_PRS_ROUTELOC1_CH4LOC_MASK) | PRS_ROUTELOC1_CH4LOC_LOC4)
#define LCD_AUTO_TOGGLE_PRS_ROUTEPEN    PRS_ROUTEPEN_CH4PEN

/*
 * Select how LCD polarity inversion should be handled:
 *
 * If POLARITY_INVERSION_EXTCOMIN is defined,
 * and the polarity inversion is armed for every rising edge of the EXTCOMIN
 * pin. The actual polarity inversion is triggered at the next transision of
 * SCS. This mode is recommended because it causes less CPU and SPI load than
 * the alternative mode, see below.
 * If POLARITY_INVERSION_EXTCOMIN is undefined,
 * the polarity inversion is toggled by sending an SPI command. This mode
 * causes more CPU and SPI load than using the EXTCOMIN pin mode.
 */
#define POLARITY_INVERSION_EXTCOMIN

/* Define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE if you want the PAL
 * (Platform Abstraction Layer interface) to automatically toggle the EXTCOMIN
 *  pin.
 * If the PAL_TIMER_REPEAT function is defined the EXTCOMIN toggling is handled
 * by a timer repeat system, therefore we must undefine
 * POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE;
 */
#ifndef PAL_TIMER_REPEAT_FUNCTION
  #define POLARITY_INVERSION_EXTCOMIN_PAL_AUTO_TOGGLE
#endif

#endif /* DISPLAYLS013B7DH03CONFIG_H */
