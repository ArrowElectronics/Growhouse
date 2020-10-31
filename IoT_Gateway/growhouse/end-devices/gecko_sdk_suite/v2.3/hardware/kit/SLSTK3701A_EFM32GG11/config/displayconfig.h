/***************************************************************************//**
 * @file displayconfig.h
 * @brief Configuration file for DISPLAY device driver interface.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef __SILICON_LABS_DISPLAYCONFIG_H__
#define __SILICON_LABS_DISPLAYCONFIG_H__

/* Include the application specific configuration file. */
#include "displayconfigapp.h"

/* Include support for the SHARP Memory LCD model LS013B7DH06 */
#define INCLUDE_DISPLAY_SHARP_LS013B7DH06

#include "displayls013b7dh06config.h"
#include "displayls013b7dh06.h"

/** Display color mode */
#define DISPLAY_COLOUR_MODE_IS_RGB_3BIT

/**
 * Maximum number of display devices the display module is configured
 * to support. This number may be increased if the system includes more than
 * one display device. However, the number should be kept low in order to
 * save memory.
 */
#define DISPLAY_DEVICES_MAX   (1)

/**
 * Geometry of display device #0 in the system. Display device #0 on this kit
 * is the SHARP Memory LCD LS013B7DH06 which has 128x128 pixels.
 * These defines can be used to declare static framebuffers in order to save
 * extra memory consumed by malloc.
 */
#define DISPLAY0_WIDTH          (LS013B7DH06_WIDTH)
#define DISPLAY0_HEIGHT         (LS013B7DH06_HEIGHT)
#define DISPLAY0_BITS_PER_PIXEL (LS013B7DH06_BITS_PER_PIXEL)

/**
 * Define all display device driver initialization functions here.
 */
#define DISPLAY_DEVICE_DRIVER_INIT_FUNCTIONS \
  {                                          \
    DISPLAY_Ls013b7dh06Init,                 \
    NULL                                     \
  }

#endif /* __SILICON_LABS_DISPLAYCONFIG_H__ */
