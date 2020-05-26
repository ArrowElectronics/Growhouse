/***************************************************************************//**
 * @file
 * @brief Dot Matrix Display interface
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __DMD_H__
#define __DMD_H__

/***************************************************************************//**
 * @addtogroup glib
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup DMD Dot Matrix Display
 * @{
 *
 * The DMD interface is the hardware abstraction layer for a physical display.
 * The DMD interface provides functions for treating a connected display as
 * a matrix of pixels of a specific size. The DMD provides functions for
 * initializing the display hardware interface by calling DMD_init() and
 * writing pixel data to the display by calling the function DMD_writeData().
 *
 * @section dmd_drivers DMD Drivers
 *
 * GLIB provides a dot matrix display driver for the LCD controller SSD2119
 * (@ref dmd_ssd2119.c) which is used on the Silicon Labs Development Kit.
 *
 * GLIB also provides a DMD driver for the Sharp Memory LCD screens used
 * on the Silicon Labs Starter Kit (@ref dmd_display.c). This driver
 * must be used together with the kit display drivers (@ref Display).
 *
 ******************************************************************************/

#include <stdint.h>
#include "em_types.h"
/* TODO: remove this and replace with include types and ecodes */
#define ECODE_DMD_BASE    0x00000000

/* Error codes */
/** Successful call */
#define DMD_OK                                  0x00000000
/** Driver not initialized correctly */
#define DMD_ERROR_DRIVER_NOT_INITIALIZED        (ECODE_DMD_BASE | 0x0001)
/** Driver is already initialized */
#define DMD_ERROR_DRIVER_ALREADY_INITIALIZED    (ECODE_DMD_BASE | 0x0002)
/** Length of data is larger than size of clip */
#define DMD_ERROR_TOO_MUCH_DATA                 (ECODE_DMD_BASE | 0x0003)
/** Pixel is outside current clipping area */
#define DMD_ERROR_PIXEL_OUT_OF_BOUNDS           (ECODE_DMD_BASE | 0x0004)
/** Clipping area is empty */
#define DMD_ERROR_EMPTY_CLIPPING_AREA           (ECODE_DMD_BASE | 0x0005)
/** Wrong device code */
#define DMD_ERROR_WRONG_DEVICE_CODE             (ECODE_DMD_BASE | 0x0006)
/** Memory error */
#define DMD_ERROR_MEMORY_ERROR                  (ECODE_DMD_BASE | 0x0007)
/** Error code expected, but didn't happen */
#define DMD_ERROR_NO_ERROR_CODE                 (ECODE_DMD_BASE | 0x0008)
/** Test run failed */
#define DMD_ERROR_TEST_FAILED                   (ECODE_DMD_BASE | 0x0009)
/** Function or option not supported - yet. */
#define DMD_ERROR_NOT_SUPPORTED                 (ECODE_DMD_BASE | 0x000a)
/** Not enough memory.  */
#define DMD_ERROR_NOT_ENOUGH_MEMORY             (ECODE_DMD_BASE | 0x000b)

/* Tests */
/** Device code test */
#define DMD_TEST_DEVICE_CODE         0x00000001
/** Memory test */
#define DMD_TEST_MEMORY              0x00000002
/** Parameter checks test */
#define DMD_TEST_PARAMETER_CHECKS    0x00000004
/** Color test */
#define DMD_TEST_COLORS              0x00000008
/** Clipping test */
#define DMD_TEST_CLIPPING            0x00000010

#define DMD_MEMORY_TEST_WIDTH        4
#define DMD_MEMORY_TEST_HEIGHT       3

/** Configuration parameter for DMD_init. This typedef is defined 'void' and
    may be defined differently in the future. */
typedef void DMD_InitConfig;

/** @struct __DMD_DisplayGeometry
 *  @brief Dimensions of the display
 */
typedef struct __DMD_DisplayGeometry{
  /** Horizontal size of the display, in pixels */
  uint16_t xSize;
  /** Vertical size of the display, in pixels */
  uint16_t ySize;
  /** X coordinate of the top left corner of the clipping area */
  uint16_t xClipStart;
  /** Y coordinate of the top left corner of the clipping area */
  uint16_t yClipStart;
  /** Width of the clipping area */
  uint16_t clipWidth;
  /** Height of the clipping area */
  uint16_t clipHeight;
} DMD_DisplayGeometry; /**< Typedef for display dimensions */

/** @struct __DMD_MemoryError
 *  @brief Information about a memory error
 */
typedef struct __DMD_MemoryError{
  /** X coordinate of the address where the error happened */
  uint16_t x;
  /** Y coordinate of the address where the error happened */
  uint16_t y;
  /** The color that was written to the memory address */
  uint8_t  writtenColor[3];
  /** The color that was read from the memory address */
  uint8_t  readColor[3];
} DMD_MemoryError; /**< Typedef for memory error information */

/* Module prototypes */
EMSTATUS DMD_init(DMD_InitConfig *initConfig);
EMSTATUS DMD_getDisplayGeometry(DMD_DisplayGeometry **geometry);
EMSTATUS DMD_setClippingArea(uint16_t xStart, uint16_t yStart,
                             uint16_t width, uint16_t height);
EMSTATUS DMD_writeData(uint16_t x, uint16_t y,
                       const uint8_t data[], uint32_t numPixels);
EMSTATUS DMD_writeDataRLE(uint16_t x, uint16_t y, uint16_t xlen, uint16_t ylen,
                          const uint8_t *data);
EMSTATUS DMD_writeDataRLEFade(uint16_t x, uint16_t y, uint16_t xlen, uint16_t ylen,
                              const uint8_t *data,
                              int red, int green, int blue, int weight);
EMSTATUS DMD_readData(uint16_t x, uint16_t y,
                      uint8_t data[], uint32_t numPixels);
EMSTATUS DMD_writeColor(uint16_t x, uint16_t y, uint8_t red,
                        uint8_t green, uint8_t blue, uint32_t numPixels);
EMSTATUS DMD_sleep(void);
EMSTATUS DMD_wakeUp(void);
EMSTATUS DMD_flipDisplay(int horizontal, int vertical);

EMSTATUS DMD_selectFramebuffer (void *framebuffer);
EMSTATUS DMD_getFrameBuffer (void **framebuffer);
EMSTATUS DMD_updateDisplay (void);

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/* Test functions */
EMSTATUS DMD_testParameterChecks(void);
EMSTATUS DMD_testMemory(uint16_t x, uint16_t y,
                        uint32_t useClipWrite, uint32_t useClipRead,
                        DMD_MemoryError *memoryError);
EMSTATUS DMD_testMemory2(uint16_t x, uint16_t y,
                         uint32_t useClipWrite);
EMSTATUS DMD_testDeviceCode(void);
EMSTATUS DMD_testColors(uint32_t delay);
EMSTATUS DMD_testClipping(void);
EMSTATUS DMD_runTests(uint32_t tests, uint32_t *result);
/** @endcond */

/**
 * @} (end addtogroup DMD)
 * @} (end addtogroup glib) */

#endif /* __DISPLAY_DMD_H__ */
