/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.34 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to Silicon Labs Norway, a subsidiary
of Silicon Labs Inc. whose registered office is 400 West Cesar Chavez,
Austin,  TX 78701, USA solely for  the purposes of creating  libraries 
for its  ARM Cortex-M3, M4F  processor-based devices,  sublicensed and 
distributed  under the  terms and conditions  of the  End User License  
Agreement supplied by Silicon Labs.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              Silicon Laboratories Norway
Licensed SEGGER software: emWin
License number:           GUI-00140
License model:            See Agreement, dated 20th April 2012
Licensed product:         - 
Licensed platform:        Cortex M3, Cortex M4F
Licensed number of seats: -
----------------------------------------------------------------------
File        : LCD_Private.h
Purpose     : To be used only by the display drivers
----------------------------------------------------------------------
*/

#ifndef LCD_Private_H
#define LCD_Private_H

#include "LCDConf.h"
#include "LCD_Protected.h"
#include "GUI.h"

/*********************************************************************
*
*       API functions
*/
extern const struct tLCDDEV_APIList_struct * /* const */ LCD_aAPI[GUI_NUM_LAYERS];

/*********************************************************************
*
*       Support for Segment/COMLUTs
*/
#define LCD_TYPE_SEGTRANS U16
#define LCD_TYPE_COMTRANS U16

#ifdef LCD_LUT_COM
  extern LCD_TYPE_COMTRANS LCD__aLine2Com0[LCD_YSIZE];
#endif

#ifdef LCD_LUT_SEG
  extern LCD_TYPE_COMTRANS LCD__aCol2Seg0[LCD_XSIZE];
#endif

/*********************************************************************
*
*       Support for multiple display controllers
*/
#define DECLARE_PROTOTYPES(DISTX) \
  void     LCD_##DISTX##_SetPixelIndex(int x, int y, int PixelIndex); \
  unsigned LCD_##DISTX##_GetPixelIndex(int x, int y); \
  void     LCD_##DISTX##_XorPixel     (int x, int y); \
  void     LCD_##DISTX##_DrawHLine    (int x0, int y,  int x1); \
  void     LCD_##DISTX##_DrawVLine    (int x, int y0,  int y1); \
  void     LCD_##DISTX##_FillRect     (int x0, int y0, int x1, int y1); \
  void     LCD_##DISTX##_DrawBitmap   (int x0, int y0, int xsize, int ysize, int BitsPerPixel, int BytesPerLine,  const U8 * pData, int Diff, const LCD_PIXELINDEX * pTrans); \
  void     LCD_##DISTX##_SetOrg       (int x, int y); \
  void     LCD_##DISTX##_On           (void); \
  void     LCD_##DISTX##_Off          (void); \
  int      LCD_##DISTX##_Init         (void); \
  void     LCD_##DISTX##_SetLUTEntry  (U8 Pos, LCD_COLOR Color); \
  void   * LCD_##DISTX##_GetDevFunc   (int Index); \
  void     LCD_##DISTX##_ReInit       (void)
    
DECLARE_PROTOTYPES(DIST0);
DECLARE_PROTOTYPES(DIST1);
DECLARE_PROTOTYPES(DIST2);
DECLARE_PROTOTYPES(DIST3);

#endif /* Avoid multiple inclusion */

/*************************** End of file ****************************/
