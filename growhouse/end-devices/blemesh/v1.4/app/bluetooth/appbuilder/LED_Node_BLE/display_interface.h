/***************************************************************************//**
 * @file
 * @brief Display interface header file
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

#ifndef DISPLAY_INTERFACE_H
#define DISPLAY_INTERFACE_H

#include "board_features.h"

/***********************************************************************************************//**
 * \defgroup disp_interface Display Interface
 * \brief Interface for displaying data.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup disp_interface
 * @{
 **************************************************************************************************/

/// Pointer to display initialization function type
typedef void (*init_func_t)(void);
/// Pointer to display print function type
typedef void (*print_func_t)(char *str, uint8 row);

/// Struct contains display interface functions
typedef struct {
  init_func_t  init;   ///< Pointer to initialization function
  print_func_t print;  ///< Pointer to print function
}display_interface_t;

#if defined(FEATURE_LCD_SUPPORT)

#include "lcd_driver.h"

#define DI_ROW_NAME         LCD_ROW_NAME
#define DI_ROW_STATUS       LCD_ROW_STATUS
#define DI_ROW_CONNECTION   LCD_ROW_CONNECTION
#define DI_ROW_FRIEND       LCD_ROW_FRIEND
#define DI_ROW_LPN          LCD_ROW_LPN
#define DI_ROW_LIGHTNESS    LCD_ROW_LIGHTNESS
#define DI_ROW_TEMPERATURE  LCD_ROW_TEMPERATURE
#define DI_ROW_DELTAUV      LCD_ROW_DELTAUV
#define DI_ROW_HUE          LCD_ROW_HUE
#define DI_ROW_SATURATION   LCD_ROW_SATURATION

#define DEFAULT_DISPLAY_INTERFACE \
  {                               \
    LCD_init,                     \
    LCD_write                     \
  }

#else

#define DI_ROW_NAME         0
#define DI_ROW_STATUS       0
#define DI_ROW_CONNECTION   0
#define DI_ROW_FRIEND       0
#define DI_ROW_LPN          0
#define DI_ROW_LIGHTNESS    0
#define DI_ROW_TEMPERATURE  0
#define DI_ROW_DELTAUV      0
#define DI_ROW_HUE          0
#define DI_ROW_SATURATION   0

#define DEFAULT_DISPLAY_INTERFACE \
  {                               \
    NULL,                         \
    NULL                          \
  }

#endif

/***************************************************************************//**
 * Configure Display Interface.
 *
 * @param[in] init   Pointer to display initialization function.
 * @param[in] print  Pointer to display print function.
 ******************************************************************************/
void DI_Config(init_func_t init, print_func_t print);

/***************************************************************************//**
 * Initialize Display Interface.
 ******************************************************************************/
void DI_Init(void);

/***************************************************************************//**
 * Print to Display Interface.
 *
 * @param[in] str  Pointer to string which is displayed in the specified row.
 * @param[in] row  Selects which line of display is written,
 *                 possible values are defined as DI_ROW_xxx.
 ******************************************************************************/
void DI_Print(char *str, uint8 row);

/** @} (end addtogroup disp_interface) */

#endif /* DISPLAY_INTERFACE_H */
