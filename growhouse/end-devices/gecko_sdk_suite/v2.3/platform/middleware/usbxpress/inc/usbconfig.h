/***************************************************************************//**
 * @file    usbconfig.h
 * @brief   USB protocol stack library, application supplied configuration
 *          options.
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef SILICON_LABS_USBCONFIG_H
#define SILICON_LABS_USBCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define USB_DEVICE        // Compile stack for device mode.

#define USB_CORECLK_HFRCO // Use HFRCO as core clock, default is HFXO.

#define USB_PWRSAVE_MODE (USB_PWRSAVE_MODE_ONVBUSOFF   \
                          | USB_PWRSAVE_MODE_ONSUSPEND \
                          | USB_PWRSAVE_MODE_ENTEREM2)

/****************************************************************************
**                                                                         **
** Specify number of endpoints used (in addition to EP0).                  **
**                                                                         **
*****************************************************************************/
#define NUM_EP_USED 2

/****************************************************************************
**                                                                         **
** Specify number of application timers you need.                          **
**                                                                         **
*****************************************************************************/
#define NUM_APP_TIMERS 1

#ifdef __cplusplus
}
#endif

#endif // SILICON_LABS_USBCONFIG_H
