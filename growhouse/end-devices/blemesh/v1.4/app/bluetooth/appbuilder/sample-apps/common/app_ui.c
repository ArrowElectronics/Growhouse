/***************************************************************************//**
 * @file
 * @brief User interface specific application code
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

/* standard headers */
#include <stdint.h>
#include <stdio.h>

/* Include feature header */
#include "ble-configuration.h"
#include "board_features.h"

/* BG stack headers*/
#include "bg_types.h"
#include "native_gecko.h"
#include "infrastructure.h"

/* HAL configuration */
#include "hal-config.h"

/* STK header files. */
#ifndef FEATURE_LED_BUTTON_ON_SAME_PIN
#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "bsp.h"
#endif /* FEATURE_LED_BUTTON_ON_SAME_PIN */

#ifdef FEATURE_LCD_SUPPORT
/* LCD */
#include "graphics.h"
#endif

#include "em_gpio.h"

/* application specific header files*/
#include "app_timer.h"

/* Own header */
#include "app_ui.h"

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app_ui
 * @{
 **************************************************************************************************/

/***************************************************************************************************
   Local Macros and Definitions
 **************************************************************************************************/
/* Text definitions*/
 #define APP_HEADER                   "SILICON LABORATORIES\nBluetooth Smart Demo\n\n" \
                                      "Blue Gecko #%05u \n\n"
 #define APP_HEADER_DEFAULT           "SILICON LABORATORIES\nBluetooth Smart Demo\n\n" \
                                      "Blue Gecko #00000 \n\n"
 #define APP_HEADER_SIZE              (sizeof(APP_HEADER_DEFAULT))

/** UI Timer periodical call frequency in ms. */
#define APP_UITIMER_PERIOD            100
#define APP_RC_DISCHARGE_PERIOD       2
/** Max. short Press Duration as a multiple of 100 ms. */
#define APP_SHORT_PRESS_DUR           20
/** Min. Long Press Duration as a multiple of 100 ms. */
#define APP_LONG_PRESS_DUR            50

// #ifdef FEATURE_LED_BUTTON_ON_SAME_PIN
// #define BSP_BUTTON0_PORT             BUTTON0_LED0_PORT
// #define BSP_BUTTON0_PIN              BUTTON0_LED0_PIN
// #define BSP_BUTTON1_PORT             BUTTON1_LED1_PORT
// #define BSP_BUTTON1_PIN              BUTTON1_LED1_PIN
// #define BSP_LED0_PORT            BUTTON0_LED0_PORT
// #define BSP_LED0_PIN             BUTTON0_LED0_PIN
// #define BSP_LED1_PORT            BUTTON1_LED1_PORT
// #define BSP_LED1_PIN             BUTTON1_LED1_PIN
// #endif /* FEATURE_LED_BUTTON_ON_SAME_PIN */

/***************************************************************************************************
   Local Type Definitions
 **************************************************************************************************/
/** Struct for states of the available LEDs on the board. */
struct appUiLedStates {
  uint8_t led1State;  /**< state can be 0 (off) and 1 (on). */
  uint8_t led2State;  /**< state can be 0 (off) and 1 (on). */
};

/** An LED sequence request consists of the sequence and its length. */
struct appUiLedSeqReq {
  struct appUiLedStates *ledSeq;  /**< pointer to the LED sequence. */
  uint8_t ledSeqLen;              /**< length of the sequence. */
};

/** Button press handler struct. */
static struct {
  appUiBtnCback_t cback;
} appUiBtn;

/***************************************************************************************************
   Local Variables
 **************************************************************************************************/
#ifdef FEATURE_LCD_SUPPORT
/** Character array to hold the string to be printed on the graphical display. */
static char appUiHeaderString[APP_HEADER_SIZE];
#endif /* FEATURE_LCD_SUPPORT */

/** Off LED sequence and request. */
static struct appUiLedStates appUiLedSeqOff[] = { { 0, 0 } };
static struct appUiLedSeqReq appUiLedSeqOffReq = { appUiLedSeqOff, COUNTOF(appUiLedSeqOff) };

/** Low alert LED sequence and request. */
static struct appUiLedStates appUiLedSeqLowAlert[] = { { 1, 0 }, { 1, 0 },
                                                       { 1, 0 }, { 1, 0 },
                                                       { 1, 0 }, { 1, 0 },
                                                       { 1, 0 }, { 1, 0 },
                                                       { 0, 1 }, { 0, 1 },
                                                       { 0, 1 }, { 0, 1 },
                                                       { 0, 1 }, { 0, 1 },
                                                       { 0, 1 }, { 0, 1 } };
static struct appUiLedSeqReq appUiLedSeqLowAlertReq =
{ appUiLedSeqLowAlert, COUNTOF(appUiLedSeqLowAlert) };

/** High alert LED sequence and request. */
static struct appUiLedStates appUiLedSeqHighAlert[] = { { 1, 0 }, { 1, 0 },
                                                        { 0, 1 }, { 0, 1 } };
static struct appUiLedSeqReq appUiLedSeqHighAlertReq =
{ appUiLedSeqHighAlert, COUNTOF(appUiLedSeqHighAlert) };

/** Request a sequence for driving the LEDs. */
static struct appUiLedSeqReq *appUiLedSeqReq = NULL;

/***************************************************************************************************
   Static Function Declarations
 **************************************************************************************************/
static void appUiLedTimerCback(void);
static uint8_t appUiPushButtonsGet(uint8_t button);
static void appUiButtonTimerCallback(void);
static void appUiBtnSendEvent(AppUiBtnEvt_t btn);

#ifdef FEATURE_LED_BUTTON_ON_SAME_PIN
static void appUiButtonInit(void);
static void appUiLedsInit(void);
static void BSP_LedSet(uint8_t AppUiLedId);
static void BSP_LedClear(uint8_t AppUiLedId);
#endif /* BRD4300A */

/***************************************************************************************************
   Public Function Definitions
 **************************************************************************************************/
void appUiBtnRegister(appUiBtnCback_t cback)
{
  appUiBtn.cback = cback;
}

void appUiLedOff(void)
{
  appUiLedSeqReq = &appUiLedSeqOffReq;
}

void appUiLedLowAlert(void)
{
  appUiLedSeqReq = &appUiLedSeqLowAlertReq;
}

void appUiLedHighAlert(void)
{
  appUiLedSeqReq = &appUiLedSeqHighAlertReq;
}

void appUiInit(uint16_t devId)
{
#ifdef FEATURE_LED_BUTTON_ON_SAME_PIN
  gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(APP_UITIMER_PERIOD - APP_RC_DISCHARGE_PERIOD), UI_TIMER, false);
#else /* !BRD4300A */
  /* Initialise LEDs */

  /* Initialize buttons */
  /* Start repeating (auto-load) timer */
  gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(APP_UITIMER_PERIOD), UI_TIMER, false);
#endif /* BRD4300A */

#ifdef FEATURE_LCD_SUPPORT
  /* Initialize graphics */
  /* Create the device name string based on the device ID */
  snprintf(appUiHeaderString, APP_HEADER_SIZE, APP_HEADER, devId);
  graphInit(appUiHeaderString);
#endif /* BRD4301A */
}

#ifdef FEATURE_LED_BUTTON_ON_SAME_PIN
void appUiTick(void)
{
  /* On BGM111A LEDs and buttons are connected to the same GPIOs, so before we poll buttons we have
   * to set GPIOs to input and wait for input capacitors to discharge
   */

  /* Flag indicating whether discharge has been done since last time GPIO was set to output */
  static bool appUiRcDischargeDone = false;

  if (false == appUiRcDischargeDone) {
    /* Initialize buttons */
    appUiButtonInit();
    /* Start a timer measuring a small time period, during which capacitors on GPIO ports can discharge */
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(APP_RC_DISCHARGE_PERIOD), UI_TIMER, false);
    appUiRcDischargeDone = true; /* Indicate discharge has been done */
  } else {
    /* Read button state*/
    appUiButtonTimerCallback();

    /* Initialise and drive LEDs */
    appUiLedsInit();
    appUiLedTimerCback();

    /* Restart timer */
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(APP_UITIMER_PERIOD - APP_RC_DISCHARGE_PERIOD), UI_TIMER, false);

    /* Discharge needs to be done before next button read */
    appUiRcDischargeDone = false;
  }
}
#else /* !BRD4300A */
void appUiTick(void)
{
  appUiLedTimerCback();
  appUiButtonTimerCallback();
}
#endif /* BRD4300A */

void appUiWriteString(char *string)
{
#ifdef FEATURE_LCD_SUPPORT
  graphWriteString(string);
#endif /* BRD4301A */
}

/***************************************************************************************************
   Static Function Definitions
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Timer callback for driving the LEDs on the DK based on the requested sequence.
 **************************************************************************************************/
static void appUiLedTimerCback(void)
{
  static uint8_t pos = 0;
  static struct appUiLedSeqReq *activeReq = &appUiLedSeqOffReq;

  /* if there is a request and it is not yet processed */
  if (appUiLedSeqReq && (activeReq != appUiLedSeqReq)) {
    activeReq = appUiLedSeqReq;
    pos = 0;
  }

  /* process the active request if it exists */
  if (activeReq) {
    activeReq->ledSeq[pos].led1State ?  BSP_LedSet(1) : BSP_LedClear(1);
    activeReq->ledSeq[pos].led2State ?  BSP_LedSet(0) : BSP_LedClear(0);
    pos++;
    if (pos >= activeReq->ledSeqLen) {
      pos = 0;
    }

    /* if the active request is the special off request, then after processing
     * clear the active request (and save some processing time while LEDs are off) */
    if (activeReq == &appUiLedSeqOffReq) {
      activeReq = NULL;
    }
  }
}

/***********************************************************************************************//**
 *  \brief  Timer Callback function for reading board buttons.
 *  \details  Function reads board buttons state and sets appropriate event or do nothing.
 **************************************************************************************************/
static void appUiButtonTimerCallback(void)
{
  static bool button0Released = true;
  static bool button1Released = true;
  static uint16_t button0Time = 0;
  static uint16_t button1Time = 0;

  bool button0State = appUiPushButtonsGet(0);
  bool button1State = appUiPushButtonsGet(1);

  if (button0State) {
    if (button0Released) {
      button0Released = false;
      button0Time = 0;
    } else {
      button0Time++;
      /* Long press after 5 seconds */
      if (button0Time >= APP_LONG_PRESS_DUR) {
        appUiBtnSendEvent(APP_UI_BTN_0_LONG);
      }
    }
  } else if (!button0Released) {
    button0Released = true;
    /* Short press under 2 seconds */
    if (button0Time < APP_SHORT_PRESS_DUR) {
      appUiBtnSendEvent(APP_UI_BTN_0_SHORT);
    }
    /* Medium press under 5 seconds */
    else if (button0Time < APP_LONG_PRESS_DUR) {
      appUiBtnSendEvent(APP_UI_BTN_0_MED);
    } else {
      /* already handled*/
    }
  }

  if (button1State) {
    if (button1Released) {
      button1Released = false;
      button1Time = 0;
    } else {
      button1Time++;
    }
  } else if (!button1Released) {
    button1Released = true;
    /* Short press under 2 seconds */
    if (button1Time < APP_SHORT_PRESS_DUR) {
      appUiBtnSendEvent(APP_UI_BTN_1_SHORT);
    }
    /* Medium press under 5 seconds */
    else if (button1Time < APP_LONG_PRESS_DUR) {
      appUiBtnSendEvent(APP_UI_BTN_1_MED);
    }
    /* Long press after 5 seconds */
    else {
      appUiBtnSendEvent(APP_UI_BTN_1_LONG);
    }
  }
}

/***********************************************************************************************//**
 *  \brief  Read push button states on WSTK.
 *  \param[in]  button  Which button's state is requested.
 *  \return  Requested button state.
 **************************************************************************************************/
static uint8_t appUiPushButtonsGet(uint8_t button)
{
  switch (button) {
    case 0:
      return (GPIO_PinInGet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN))
             ? false : true;
    case 1:
      return (GPIO_PinInGet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN))
             ? false : true;
    default:
      return false;
  }
}

/***********************************************************************************************//**
 *  \brief  Dispatch a button pressed event by calling the registered button event callback.
 *  \param[in]  btn  Button number and press duration enumeration.
 **************************************************************************************************/
static void appUiBtnSendEvent(AppUiBtnEvt_t btn)
{
  (*appUiBtn.cback)(btn);
}

#ifdef FEATURE_LED_BUTTON_ON_SAME_PIN
static void appUiButtonInit(void)
{
  /* Configure pin as input */
  GPIO_PinModeSet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN, gpioModeInput, 1);
  /* Configure pin as input */
  GPIO_PinModeSet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN, gpioModeInput, 1);
}

static void appUiLedsInit(void)
{
  /* Configure pin as output */
  GPIO_PinModeSet(BSP_LED0_PORT, BSP_LED0_PIN, gpioModePushPull, 1);
  /* Configure pin as output */
  GPIO_PinModeSet(BSP_LED1_PORT, BSP_LED1_PIN, gpioModePushPull, 1);
}

/* Output = 0 -> LED is on, Output = 1 -> LED is off */
static void BSP_LedSet(uint8_t AppUiLedId)
{
  switch (AppUiLedId) {
    case 0:
      GPIO_PinOutClear(BSP_LED0_PORT, BSP_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutClear(BSP_LED1_PORT, BSP_LED1_PIN);
      break;

    default:
      break;
  }
}

/* Output = 0 -> LED is on, Output = 1 -> LED is off */
static void BSP_LedClear(uint8_t AppUiLedId)
{
  switch (AppUiLedId) {
    case 0:
      GPIO_PinOutSet(BSP_LED0_PORT, BSP_LED0_PIN);
      break;

    case 1:
      GPIO_PinOutSet(BSP_LED1_PORT, BSP_LED1_PIN);
      break;

    default:
      break;
  }
}
#endif /* BRD4300A */

/** @} (end addtogroup app_ui) */
/** @} (end addtogroup Application) */
