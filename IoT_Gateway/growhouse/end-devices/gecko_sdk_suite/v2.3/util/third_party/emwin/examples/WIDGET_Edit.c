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
File        : WIDGET_Edit.c
Purpose     : Example demonstrating the use of a EDIT widget
Requirements: WindowManager - (x)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
----------------------------------------------------------------------
*/
#include "GUI.h"
#include "EDIT.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define WM_APP_SHOW_TEXT (WM_USER + 0)
#define TEXT_MAXLEN      40

//
// Recommended memory to run the sample with adequate performance
//
#define RECOMMENDED_MEMORY (1024L * 5)

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  static WM_HWIN hEdit;
  static U8      ShowText;
  char           aBuffer[TEXT_MAXLEN];

  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(GUI_DARKGRAY);
    GUI_Clear();
    GUI_SetFont(&GUI_Font24_ASCII);
    GUI_DispStringHCenterAt("WIDGET_Edit - Sample", 160, 5);
    GUI_SetFont(&GUI_Font8x16);
    if (ShowText) {
      GUI_DispStringHCenterAt("The string you have modified is:", 160, 90);
      EDIT_GetText(hEdit, aBuffer, TEXT_MAXLEN);
      GUI_DispStringHCenterAt(aBuffer, 160, 110);
    } else {
      GUI_DispStringHCenterAt("Use keyboard to modify string...", 160, 90);
    }
    break;
  case WM_APP_SHOW_TEXT:
    if (hEdit == 0) {
      hEdit = pMsg->hWinSrc;
    }
    WM_HideWindow(hEdit);
    ShowText = 1;
    WM_InvalidateWindow(WM_HBKWIN);
    WM_CreateTimer(WM_HBKWIN, 0, 3000, 0);
    break;
  case WM_TIMER:
    ShowText = 0;
    WM_InvalidateWindow(WM_HBKWIN);
    WM_ShowWindow(hEdit);
    break;
  default:
    WM_DefaultProc(pMsg);
  }
}

/*********************************************************************
*
*       _cbEdit
*/
static void _cbEdit(WM_MESSAGE * pMsg) {
  const WM_KEY_INFO * pInfo;
  WM_MESSAGE          Msg;

  switch (pMsg->MsgId) {
  case WM_KEY:
    pInfo = (WM_KEY_INFO *)pMsg->Data.p;
    if (pInfo->Key == GUI_KEY_ENTER) {
      if (pInfo->PressedCnt == 0) {
        Msg.MsgId   = WM_APP_SHOW_TEXT;
        Msg.hWinSrc = pMsg->hWin;
        WM_SendMessage(WM_HBKWIN, &Msg);
        return;
      }
    }
  }
  EDIT_Callback(pMsg);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void MainTask(void) {
  EDIT_Handle hEdit;

  GUI_Init();
  //
  // Check if recommended memory for the sample is available
  //
  if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY) {
    GUI_ErrorOut("Not enough memory available."); 
    return;
  }
  WM_SetCallback(WM_HBKWIN, _cbBk);
  hEdit = EDIT_CreateEx(50, 110, 220, 25, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_EDIT0, TEXT_MAXLEN);
  WM_SetCallback(hEdit, _cbEdit);
  EDIT_SetText(hEdit, "Press <ENTER> when done...");
  EDIT_SetFont(hEdit, &GUI_Font8x16);
  EDIT_SetTextColor(hEdit, 0, GUI_RED);
  EDIT_EnableBlink(hEdit, 300, 1);
  WM_SetFocus(hEdit);
  while (1) {
    GUI_Delay(10);
  }
}

/*************************** End of file ****************************/
