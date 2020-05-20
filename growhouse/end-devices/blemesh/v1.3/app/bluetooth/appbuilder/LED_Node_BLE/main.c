/***********************************************************************************************//**
 * \file   main.c
 * \brief  Silicon Labs Bluetooth mesh light example
 *
 * This example implements a Bluetooth mesh light node.
 *
 ***************************************************************************************************
 * <b> (C) Copyright 2017 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

/* C Standard Library headers */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"
#include "retargetserial.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "mesh_lib.h"
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>

/* Device initialization header */
#include "hal-config.h"
#include "uartdrv.h"
/* Display Interface header */
#include "display_interface.h"

/* LED driver with support for PWM dimming */
#include "led_driver.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/* watchdog support */
#include "em_wdog.h"

/* USART for serial communication with Zigbee */
#include "ZIGBEE_Usart_Comm.h"

/* SPI driver header for AD5206 */
#include "ad5206.h"

/* spi flash */
#include "mx25flash_spi.h"

#include "em_gpio.h"
#include "gpiointerrupt.h"

#include "api/btl_interface.h"

/* SPI Flash memory address for storing the LED Node configuration */
#define PAGE_SIZE                   4096
#define EEPROM_SIZE					524288
#define LED_CONFIGURATION_ADDR      EEPROM_SIZE - PAGE_SIZE

/* LED channel - transition_ms time mapping */
#define LED_CHANNEL1_T	(100)
#define LED_CHANNEL2_T	(200)
#define LED_CHANNEL3_T	(300)
#define LED_CHANNEL4_T	(400)
#define LED_CHANNEL5_T	(500)
#define LED_CHANNEL6_T	(600)

/* dummy request for the device intensity */
#define LED_INTENSITY_GET_REQ 35

/* Mapping of LED level 0 to 100% to 0 to 65535 level. */
#define PERCENTAGE_VALUE_FACTOR (655.35f)

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/
volatile uint32_t msTicks; /* counts 1ms timeTicks */
extern uint8_t zigbeeAvailable;
uint32_t LED_Channel = 0;

extern LED_Configuration Current_LED_Configuration;
extern LED_Configuration Default_LED_Configuration;
LED_Configuration Temp_LED_Configuration;
/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/
/* Version specific Structure */
extern const ApplicationProperties_t bg_application_properties;

struct mesh_generic_state current, target;

bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

void lightness_request_usart(uint16_t led_intensity, uint16_t led_channel_num);
void default_lightness_request_usart();
int writeLedStateFlash(LED_Configuration *LED_Configuration_state);
int readLedStateFlash(LED_Configuration *LED_Configuration_state);
int LEDS_SetLevel_Dpot(uint16_t led_intensity, uint16_t led_channel_num);
static void gpio_interrupt_handler_cb(uint8_t gpioPinNo);
int writeLedStateFlash(LED_Configuration *LED_Configuration_state);
void set_led_config_gatt_attribute(void);
void set_led_intensity_gatt_attribute(void);
void reset_led_configuration_flash(void);

// Maximum number of simultaneous Bluetooth connections
#define MAX_CONNECTIONS 2

// heap for Bluetooth stack
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

// Bluetooth stack configuration
const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .max_timers = 16,
};

/** Timer Frequency used. */
#define TIMER_CLK_FREQ ((uint32)32768)
/** Convert msec to timer ticks. */
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)

#define TIMER_ID_WATCHDOG    79
#define TIMER_ID_RESTART    78
#define TIMER_ID_FACTORY_RESET  77
#define TIMER_ID_PROVISIONING   66
#define TIMER_ID_SAVE_STATE   60
#define TIMER_ID_ONOFF_TRANSITION         53
#define TIMER_ID_LIGHTNESS_TRANSITION     52
#define TIMER_ID_DELAYED_ONOFF            51
#define TIMER_ID_DELAYED_LIGHTNESS        50
#define TIMER_ID_DELAYED_CTL              49
#define TIMER_ID_CTL_TRANSITION           48
#define TIMER_ID_DELAYED_CTL_TEMPERATURE  47
#define TIMER_ID_CTL_TEMP_TRANSITION      46
#define TIMER_ID_DELAYED_PRI_LEVEL        45
#define TIMER_ID_PRI_LEVEL_TRANSITION     44
#define TIMER_ID_DELAYED_SEC_LEVEL        43
#define TIMER_ID_SEC_LEVEL_TRANSITION     42

/*** Button external signals ***/
#define EXT_SIGNAL_FACTORY_BUTTON_PRESS_SIG          (1 << 3)
#define EXT_SIGNAL_FACTORY_BUTTON_RELEASE_SIG        (1 << 4)
#define FACTORY_BUTTON_LONG_PRESS_DURATION (80000) /* ~2.5 seconds */

/** global variables */
static uint16 _primary_elem_index = 0xffff; /* For indexing elements of the node */
static uint16 _secondary_elem_index = 0xffff; /* For indexing elements of the node */
static uint16 _my_address = 0;    /* Address of the Primary Element of the Node */
static uint8 num_connections = 0;     /* number of active Bluetooth connections */
static uint8 conn_handle = 0xFF;      /* handle of the last opened LE connection */
static uint8 init_done = 0;

static PACKSTRUCT(struct lightbulb_state {
  // On/Off Server state
  uint8_t onoff_current;
  uint8_t onoff_target;

  // Transition Time Server state
  uint8_t transtime;

  // On Power Up Server state
  uint8_t onpowerup;

  // Lightness server
  uint16_t lightness_current;
  uint16_t lightness_target;
  uint16_t lightness_last;
  uint16_t lightness_default;

  // Primary Generic Level
  int16_t pri_level_current;
  int16_t pri_level_target;

  // Temperature server
  uint16_t temperature_current;
  uint16_t temperature_target;
  uint16_t temperature_default;
  uint16_t temperature_min;
  uint16_t temperature_max;

  // Delta UV
  int16_t deltauv_current;
  int16_t deltauv_target;
  int16_t deltauv_default;

  // Secondary Generic Level
  int16_t sec_level_current;
  int16_t sec_level_target;
}) lightbulb_state;

/* Watchdog init configuration */
WDOG_Init_TypeDef init =
{
 .enable = true,             /* Start watchdog when init done */
 .debugRun = false,          /* WDOG not counting during debug halt */
 .em2Run = true,             /* WDOG counting when in EM2 */
 .em3Run = true,             /* WDOG counting when in EM3 */
 .em4Block = false,          /* EM4 can be entered */
 .swoscBlock = false,        /* Do not block disabling LFRCO/LFXO in CMU */
 .lock = false,              /* Do not lock WDOG configuration (if locked, reset needed to unlock) */
 .clkSel = wdogClkSelULFRCO, /* Select 1kHZ WDOG oscillator */
 .perSel = wdogPeriod_8k,    /* Set the watchdog period to 8193 clock periods (ie ~8 seconds)*/
};

/* copy of transition delay parameter, needed for delayed on/off, lightness, temperature, ctl and
 * primary and secondary generic level requests */
uint32_t delayed_onoff_trans = 0;
uint32_t delayed_lightness_trans = 0;
uint32_t delayed_ctl_trans = 0;
uint32_t delayed_ctl_temperature_trans = 0;
uint32_t delayed_pri_level_trans = 0;
uint32_t delayed_sec_level_trans = 0;

static int lightbulb_state_load(void);
static int lightbulb_state_store(void);
static void lightbulb_state_changed(void);

static uint32_t default_transition_time(void)
{
  return mesh_lib_transition_time_to_ms(lightbulb_state.transtime);
}

static errorcode_t onoff_response(uint16_t element_index,
                                  uint16_t client_addr,
                                  uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_on_off;
  current.on_off.on = lightbulb_state.onoff_current;

  target.kind = mesh_generic_state_on_off;
  target.on_off.on = lightbulb_state.onoff_target;

  return mesh_lib_generic_server_response(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t onoff_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_on_off;
  current.on_off.on = lightbulb_state.onoff_current;

  target.kind = mesh_generic_state_on_off;
  target.on_off.on = lightbulb_state.onoff_target;

  return mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t onoff_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = onoff_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_generic_state_on_off);
  }

  return e;
}

static void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags)
{
  printf("ON/OFF request: requested state=<%s>, transition=%lu, delay=%u\r\n",
         request->on_off ? "ON" : "OFF", transition_ms, delay_ms);

  if (lightbulb_state.onoff_current == request->on_off) {
    printf("Request for current state received; no op\r\n");
  } else {
    printf("Turning lightbulb <%s>\r\n", request->on_off ? "ON" : "OFF");
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.onoff_current = request->on_off;
      lightbulb_state.onoff_target = request->on_off;
      if (lightbulb_state.onoff_current == MESH_GENERIC_ON_OFF_STATE_OFF) {
        LEDS_SetState(LED_STATE_OFF);
      } else {
        LEDS_SetState(LED_STATE_ON);
      }
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.onoff_target = request->on_off;
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delay_ms), TIMER_ID_DELAYED_ONOFF, 1);
      // store transition parameter for later use
      delayed_onoff_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.onoff_target = request->on_off;

      if (request->on_off == MESH_GENERIC_ON_OFF_STATE_OFF) {
        LEDS_SetLevel(0, transition_ms);
      } else {
        // restore last brightness
        lightbulb_state.lightness_target = lightbulb_state.lightness_last;
        LEDS_SetLevel(lightbulb_state.lightness_target, transition_ms);
      }
      // lightbulb current state will be updated when transition is complete
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_ONOFF_TRANSITION, 1);
    }
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    onoff_response(element_index, client_addr, appkey_index);
  } else {
    onoff_update(element_index);
  }
}

static void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
  if (current->on_off.on != lightbulb_state.onoff_current) {
    printf("on-off state changed %u to %u\r\n", lightbulb_state.onoff_current, current->on_off.on);

    lightbulb_state.onoff_current = current->on_off.on;
    lightbulb_state_changed();
  } else {
    printf("dummy onoff change - same state as before\r\n");
  }
}

static errorcode_t lightness_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_lightness_actual;
  current.lightness.level = lightbulb_state.lightness_current;

  target.kind = mesh_lighting_state_lightness_actual;
  target.lightness.level = lightbulb_state.lightness_target;

  return mesh_lib_generic_server_response(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t lightness_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_lightness_actual;
  current.lightness.level = lightbulb_state.lightness_current;

  target.kind = mesh_lighting_state_lightness_actual;
  target.lightness.level = lightbulb_state.lightness_target;

  return mesh_lib_generic_server_update(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t lightness_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = lightness_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_lighting_state_lightness_actual);
  }

  return e;
}

static uint32_t extract_led_channel(uint32_t transition_ms)
{
  uint32_t LED_Channel_Num = 0;
  /* extract the LED channel number from the transition delay parameter
   * The transition time is used here as channel identifier.
   *
   * transition time is multiple of 100 with channel number
   * i.e for channel 1 - transition time will be 100
   * channel -1 : transition_ms 100
   * channel -2 : transition_ms 200
   * channel -3 : transition_ms 300
   * channel -4 : transition_ms 400
   * channel -5 : transition_ms 500
   * channel -6 : transition_ms 600
   * */
  if (transition_ms != LED_CHANNEL1_T && transition_ms != LED_CHANNEL2_T
			&& transition_ms != LED_CHANNEL3_T
			&& transition_ms != LED_CHANNEL4_T
			&& transition_ms != LED_CHANNEL5_T
			&& transition_ms != LED_CHANNEL6_T)
  {
	  printf("Invalid LED channel request received. \r\n");
	  return 0;
  }

  switch(transition_ms) {

  case LED_CHANNEL1_T:
	  LED_Channel_Num = 1;
	  break;
  case LED_CHANNEL2_T:
	  LED_Channel_Num = 2;
	  break;
  case LED_CHANNEL3_T:
	  LED_Channel_Num = 3;
	  break;
  case LED_CHANNEL4_T:
	  LED_Channel_Num = 4;
	  break;
  case LED_CHANNEL5_T:
	  LED_Channel_Num = 5;
	  break;
  case LED_CHANNEL6_T:
	  LED_Channel_Num = 6;
	  break;
  default:
	  printf("Invalid transition time - No channel matched.\r\n");
  }

  return LED_Channel_Num;
}

static void lightness_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  // for simplicity, this demo assumes that all lightness requests use the actual scale.
  // other type of requests are ignored
  if (request->kind != mesh_lighting_request_lightness_actual) {
    return;
  }

  printf("lightness_request: level=%u, transition=%lu, delay=%u\r\n",
         request->lightness, transition_ms, delay_ms);

  /* Handle the get LED intensity dummy request from
   * BLE mesh application */
  if(delay_ms == LED_INTENSITY_GET_REQ)
  {
	  /* Extract the requested LED channel number */
	  LED_Channel = extract_led_channel(transition_ms);
	  if(LED_Channel < 1 || LED_Channel > LED_CHANNELS)
	  {
		  printf("ERROR: Invalid channel request received.\r\n");
		  return;
	  }
	  transition_ms = 0;
	  delay_ms = 0;

	  lightbulb_state.lightness_current =
			  (Current_LED_Configuration.LED_Channel_intensity[LED_Channel - 1]) * PERCENTAGE_VALUE_FACTOR;
	  printf("Requested Channel:%lu and Intensity: %d\r\n", LED_Channel,
			  lightbulb_state.lightness_current);

	  /* send the LED intensity update */
	  lightness_response(element_index, client_addr, appkey_index);
	  return;
  }

  /* extract the LED channel information */
  LED_Channel = extract_led_channel(transition_ms);
  if(LED_Channel < 1 || LED_Channel > LED_CHANNELS)
  {
	  printf("ERROR: Invalid channel request received.\r\n");
	  return;
  }
  transition_ms = 0;

  if (lightbulb_state.lightness_current == request->lightness) {
    printf("Request for current state received; no op\r\n");
  } else {
    printf("Setting lightness to <%u>\r\n", request->lightness);
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.lightness_current = request->lightness;
      lightbulb_state.lightness_target = request->lightness;
      if (request->lightness != 0) {
        lightbulb_state.lightness_last = request->lightness;
      }
      // update LED PWM duty cycle
      LEDS_SetLevel(lightbulb_state.lightness_current, 0);

	  /* control the LED channel intensity directly */
      LEDS_SetLevel_Dpot(lightbulb_state.lightness_current, LED_Channel);

    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.lightness_target = request->lightness;
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delay_ms), TIMER_ID_DELAYED_LIGHTNESS, 1);
      // store transition parameter for later use
      delayed_lightness_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.lightness_target = request->lightness;
      LEDS_SetLevel(lightbulb_state.lightness_target, transition_ms);

	  /* control the LED channel intensity directly */
      LEDS_SetLevel_Dpot(lightbulb_state.lightness_target, LED_Channel);

      // lightbulb current state will be updated when transition is complete
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_LIGHTNESS_TRANSITION, 1);
    }
    lightbulb_state_changed();
  }
  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    lightness_response(element_index, client_addr, appkey_index);
  } else {
    lightness_update(element_index);
  }
}

static void lightness_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  if (current->kind != mesh_lighting_state_lightness_actual) {
    // if kind is not 'actual' then just report the change here, no change to light state
    printf("lightness change, kind %u, value %u\r\n", current->kind, current->lightness.level);
    return;
  }

  if (lightbulb_state.lightness_current != current->lightness.level) {
    printf("lightness_change: from %u to %u\r\n", lightbulb_state.lightness_current, current->lightness.level);
    lightbulb_state.lightness_current = current->lightness.level;
    lightbulb_state_changed();
  } else {
    printf("lightness update -same value (%d)\r\n", lightbulb_state.lightness_current);
  }
}

static errorcode_t pri_level_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.pri_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.pri_level_target;

  return mesh_lib_generic_server_response(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t pri_level_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.pri_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.pri_level_target;

  return mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t pri_level_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = pri_level_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_generic_state_level);
  }

  return e;
}

static void pri_level_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  // for simplicity, this demo assumes that all level requests use set level.
  // other type of requests are ignored

  uint16_t lightness;

  if (request->kind != mesh_generic_request_level) {
    return;
  }

  printf("pri_level_request: level=%d, transition=%lu, delay=%u\r\n",
         request->level, transition_ms, delay_ms);

  /* extract the LED channel information */
  LED_Channel = extract_led_channel(transition_ms);
  transition_ms = 0;

  if (lightbulb_state.pri_level_current == request->level) {
    printf("Request for current state received; no op\n");
  } else {
    printf("Setting pri_level to <%d>\r\n", request->level);

    lightness = request->level + 32768;

    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.pri_level_current = request->level;
      lightbulb_state.pri_level_target = request->level;
      lightbulb_state.lightness_current = lightness;
      lightbulb_state.lightness_target = lightness;

      // update LED Temperature
      LEDS_SetLevel(lightness, 0);

      /* control the LED channel intensity directly */
      LEDS_SetLevel_Dpot(lightness, LED_Channel);

    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.pri_level_target = request->level;
      lightbulb_state.lightness_target = lightness;
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delay_ms), TIMER_ID_DELAYED_PRI_LEVEL, 1);
      // store transition parameter for later use
      delayed_pri_level_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.pri_level_target = request->level;
      lightbulb_state.lightness_target = lightness;
      LEDS_SetLevel(lightness, transition_ms);

      /* control the LED channel intensity directly */
      LEDS_SetLevel_Dpot(lightness, LED_Channel);

      // lightbulb current state will be updated when transition is complete
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_PRI_LEVEL_TRANSITION, 1);
    }
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    pri_level_response(element_index, client_addr, appkey_index);
  } else {
    pri_level_update(element_index);
  }
}

static void pri_level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  if (lightbulb_state.pri_level_current != current->level.level) {
    printf("pri_level_change: from %d to %d\r\n", lightbulb_state.pri_level_current, current->level.level);
    lightbulb_state.pri_level_current = current->level.level;
    lightbulb_state_changed();
  } else {
    printf("pri_level update -same value (%d)\r\n", lightbulb_state.pri_level_current);
  }
}

static errorcode_t ctl_response(uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl;
  current.ctl.lightness = lightbulb_state.lightness_current;
  current.ctl.temperature = lightbulb_state.temperature_current;
  current.ctl.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl;
  target.ctl.lightness = lightbulb_state.lightness_target;
  target.ctl.temperature = lightbulb_state.temperature_target;
  target.ctl.deltauv = lightbulb_state.deltauv_target;

  return mesh_lib_generic_server_response(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t ctl_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl;
  current.ctl.lightness = lightbulb_state.lightness_current;
  current.ctl.temperature = lightbulb_state.temperature_current;
  current.ctl.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl;
  target.ctl.lightness = lightbulb_state.lightness_target;
  target.ctl.temperature = lightbulb_state.temperature_target;
  target.ctl.deltauv = lightbulb_state.deltauv_target;

  return mesh_lib_generic_server_update(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t ctl_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = ctl_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_lighting_state_ctl);
  }

  return e;
}

static void ctl_request(uint16_t model_id,
                        uint16_t element_index,
                        uint16_t client_addr,
                        uint16_t server_addr,
                        uint16_t appkey_index,
                        const struct mesh_generic_request *request,
                        uint32_t transition_ms,
                        uint16_t delay_ms,
                        uint8_t request_flags)
{
  printf("ctl_request: lightness=%u, temperature=%u, delta_uv=%d, transition=%lu, delay=%u\r\n",
         request->ctl.lightness, request->ctl.temperature, request->ctl.deltauv, transition_ms, delay_ms);

  if ((lightbulb_state.lightness_current == request->ctl.lightness)
      && (lightbulb_state.temperature_current == request->ctl.temperature)
      && (lightbulb_state.deltauv_current == request->ctl.deltauv)) {
    printf("Request for current state received; no op\n");
  } else {
    if (lightbulb_state.lightness_current != request->ctl.lightness) {
      printf("Setting lightness to <%u>\r\n", request->lightness);
    }
    if (lightbulb_state.temperature_current != request->ctl.temperature) {
      printf("Setting temperature to <%u>\r\n", request->ctl.temperature);
    }
    if (lightbulb_state.deltauv_current != request->ctl.deltauv) {
      printf("Setting delta UV to <%d>\r\n", request->ctl.deltauv);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.lightness_current = request->ctl.lightness;
      lightbulb_state.lightness_target = request->ctl.lightness;
      if (request->lightness != 0) {
        lightbulb_state.lightness_last = request->ctl.lightness;
      }

      // update LED PWM duty cycle
      LEDS_SetLevel(lightbulb_state.lightness_current, 0);

      lightbulb_state.temperature_current = request->ctl.temperature;
      lightbulb_state.temperature_target = request->ctl.temperature;
      lightbulb_state.deltauv_current = request->ctl.deltauv;
      lightbulb_state.deltauv_target = request->ctl.deltauv;

      // update LED color temperature
      LEDS_SetTemperature(lightbulb_state.temperature_current, lightbulb_state.deltauv_current, 0);
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.lightness_target = request->ctl.lightness;
      lightbulb_state.temperature_target = request->ctl.temperature;
      lightbulb_state.deltauv_target = request->ctl.deltauv;
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delay_ms), TIMER_ID_DELAYED_CTL, 1);
      // store transition parameter for later use
      delayed_ctl_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.lightness_target = request->ctl.lightness;
      lightbulb_state.temperature_target = request->ctl.temperature;
      lightbulb_state.deltauv_target = request->ctl.deltauv;

      LEDS_SetLevel(lightbulb_state.lightness_target, transition_ms);
      LEDS_SetTemperature(lightbulb_state.temperature_target, lightbulb_state.deltauv_target, transition_ms);

      // lightbulb current state will be updated when transition is complete
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_CTL_TRANSITION, 1);
    }
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    ctl_response(element_index, client_addr, appkey_index);
  } else {
    ctl_update(element_index);
  }
}

static void ctl_change(uint16_t model_id,
                       uint16_t element_index,
                       const struct mesh_generic_state *current,
                       const struct mesh_generic_state *target,
                       uint32_t remaining_ms)
{
  if (current->kind != mesh_lighting_state_ctl) {
    // if kind is not 'ctl' then just report the change here
    printf("ctl change, kind %u\r\n", current->kind);
    return;
  }

  if (lightbulb_state.lightness_current != current->ctl.lightness) {
    printf("lightness_change: from %u to %u\r\n", lightbulb_state.lightness_current, current->ctl.lightness);
    lightbulb_state.lightness_current = current->ctl.lightness;
    lightbulb_state_changed();
  } else {
    printf("lightness update -same value (%u)\r\n", lightbulb_state.lightness_current);
  }

  if (lightbulb_state.temperature_current != current->ctl.temperature) {
    printf("temperature_change: from %u to %u\r\n", lightbulb_state.temperature_current, current->ctl.temperature);
    lightbulb_state.temperature_current = current->ctl.temperature;
    lightbulb_state_changed();
  } else {
    printf("temperature update -same value (%u)\r\n", lightbulb_state.temperature_current);
  }

  if (lightbulb_state.deltauv_current != current->ctl.deltauv) {
    printf("deltauv_change: from %d to %d\r\n", lightbulb_state.deltauv_current, current->ctl.deltauv);
    lightbulb_state.deltauv_current = current->ctl.deltauv;
    lightbulb_state_changed();
  } else {
    printf("deltauv update -same value (%d)\r\n", lightbulb_state.deltauv_current);
  }
}

static errorcode_t ctl_setup_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index,
                                      mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_ctl_default:
      current.ctl.lightness = lightbulb_state.lightness_default;
      current.ctl.temperature = lightbulb_state.temperature_default;
      current.ctl.deltauv = lightbulb_state.deltauv_default;
      break;
    case mesh_lighting_state_ctl_temperature_range:
      current.ctl_temperature_range.min = lightbulb_state.temperature_min;
      current.ctl_temperature_range.max = lightbulb_state.temperature_max;
      break;
    default:
      break;
  }

  return mesh_lib_generic_server_response(MESH_LIGHTING_CTL_SETUP_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          NULL,
                                          0,
                                          0x00);
}

static errorcode_t ctl_setup_update(uint16_t element_index, mesh_generic_state_t kind)
{
  struct mesh_generic_state current;

  current.kind = kind;

  switch (kind) {
    case mesh_lighting_state_ctl_default:
      current.ctl.lightness = lightbulb_state.lightness_default;
      current.ctl.temperature = lightbulb_state.temperature_default;
      current.ctl.deltauv = lightbulb_state.deltauv_default;
      break;
    case mesh_lighting_state_ctl_temperature_range:
      current.ctl_temperature_range.min = lightbulb_state.temperature_min;
      current.ctl_temperature_range.max = lightbulb_state.temperature_max;
      break;
    default:
      break;
  }

  return mesh_lib_generic_server_update(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        NULL,
                                        0);
}

static void ctl_setup_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  switch (request->kind) {
    case mesh_lighting_request_ctl_default:
      printf("ctl_setup_request: state=ctl_default, default_lightness=%u, default_temperature=%u, default_delta_uv=%d",
             request->ctl.lightness, request->ctl.temperature, request->ctl.deltauv);

      if ((lightbulb_state.lightness_default == request->ctl.lightness)
          && (lightbulb_state.temperature_default == request->ctl.temperature)
          && (lightbulb_state.deltauv_default == request->ctl.deltauv)) {
        printf("Request for current state received; no op\n");
      } else {
        if (lightbulb_state.lightness_default != request->ctl.lightness) {
          printf("Setting default lightness to <%u>\r\n", request->ctl.lightness);
          lightbulb_state.lightness_default = request->ctl.lightness;
        }
        if (lightbulb_state.temperature_default != request->ctl.temperature) {
          printf("Setting default temperature to <%u>\r\n", request->ctl.temperature);
          lightbulb_state.temperature_default = request->ctl.temperature;
        }
        if (lightbulb_state.deltauv_default != request->ctl.deltauv) {
          printf("Setting default delta UV to <%d>\r\n", request->ctl.deltauv);
          lightbulb_state.deltauv_default = request->ctl.deltauv;
        }
        lightbulb_state_changed();
      }
      break;

    case mesh_lighting_request_ctl_temperature_range:
      printf("ctl_setup_request: state=ctl_temperature_range, min_temperature=%u, max_temperature=%u",
             request->ctl_temperature_range.min, request->ctl_temperature_range.max);

      if ((lightbulb_state.temperature_min == request->ctl_temperature_range.min)
          && (lightbulb_state.temperature_max == request->ctl_temperature_range.max)) {
        printf("Request for current state received; no op\n");
      } else {
        if (lightbulb_state.temperature_min != request->ctl_temperature_range.min) {
          printf("Setting min temperature to <%u>\r\n", request->ctl_temperature_range.min);
          lightbulb_state.temperature_min = request->ctl_temperature_range.min;
        }
        if (lightbulb_state.temperature_max != request->ctl_temperature_range.max) {
          printf("Setting max temperature to <%u>\r\n", request->ctl_temperature_range.max);
          lightbulb_state.temperature_max = request->ctl_temperature_range.max;
        }
        lightbulb_state_changed();
      }
      break;

    default:
      break;
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    ctl_setup_response(element_index, client_addr, appkey_index, request->kind);
  } else {
    ctl_setup_update(element_index, request->kind);
  }
}

static void ctl_setup_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  switch (current->kind) {
    case mesh_lighting_state_ctl_default:
      if (lightbulb_state.lightness_default != current->ctl.lightness) {
        printf("default_lightness_change: from %u to %u\r\n", lightbulb_state.lightness_default, current->ctl.lightness);
        lightbulb_state.lightness_default = current->ctl.lightness;
        lightbulb_state_changed();
      } else {
        printf("default lightness update -same value (%u)\r\n", lightbulb_state.lightness_default);
      }

      if (lightbulb_state.temperature_default != current->ctl.temperature) {
        printf("default_temperature_change: from %u to %u\r\n", lightbulb_state.temperature_default, current->ctl.temperature);
        lightbulb_state.temperature_default = current->ctl.temperature;
        lightbulb_state_changed();
      } else {
        printf("default temperature update -same value (%u)\r\n", lightbulb_state.temperature_default);
      }

      if (lightbulb_state.deltauv_current != current->ctl.deltauv) {
        printf("default_deltauv_change: from %d to %d\r\n", lightbulb_state.deltauv_default, current->ctl.deltauv);
        lightbulb_state.deltauv_default = current->ctl.deltauv;
        lightbulb_state_changed();
      } else {
        printf("default deltauv update -same value (%d)\r\n", lightbulb_state.deltauv_default);
      }

      break;

    case mesh_lighting_state_ctl_temperature_range:
      if (lightbulb_state.temperature_min != current->ctl_temperature_range.min) {
        printf("min_temperature_change: from %u to %u\r\n", lightbulb_state.temperature_min, current->ctl_temperature_range.min);
        lightbulb_state.temperature_min = current->ctl_temperature_range.min;
        lightbulb_state_changed();
      } else {
        printf("min temperature update -same value (%u)\r\n", lightbulb_state.temperature_min);
      }

      if (lightbulb_state.temperature_max != current->ctl_temperature_range.max) {
        printf("max_temperature_change: from %u to %u\r\n", lightbulb_state.temperature_max, current->ctl_temperature_range.max);
        lightbulb_state.temperature_max = current->ctl_temperature_range.max;
        lightbulb_state_changed();
      } else {
        printf("max temperature update -same value (%u)\r\n", lightbulb_state.temperature_max);
      }

      break;

    default:
      break;
  }
}

static errorcode_t ctl_temperature_response(uint16_t element_index,
                                            uint16_t client_addr,
                                            uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl_temperature;
  current.ctl.temperature = lightbulb_state.temperature_current;
  current.ctl.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl_temperature;
  target.ctl.temperature = lightbulb_state.temperature_target;
  target.ctl.deltauv = lightbulb_state.deltauv_target;

  return mesh_lib_generic_server_response(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t ctl_temperature_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_lighting_state_ctl_temperature;
  current.ctl.temperature = lightbulb_state.temperature_current;
  current.ctl.deltauv = lightbulb_state.deltauv_current;

  target.kind = mesh_lighting_state_ctl_temperature;
  target.ctl.temperature = lightbulb_state.temperature_target;
  target.ctl.deltauv = lightbulb_state.deltauv_target;

  return mesh_lib_generic_server_update(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t ctl_temperature_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = ctl_temperature_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_lighting_state_ctl_temperature);
  }

  return e;
}

static void ctl_temperature_request(uint16_t model_id,
                                    uint16_t element_index,
                                    uint16_t client_addr,
                                    uint16_t server_addr,
                                    uint16_t appkey_index,
                                    const struct mesh_generic_request *request,
                                    uint32_t transition_ms,
                                    uint16_t delay_ms,
                                    uint8_t request_flags)
{
  printf("ctl_temperature_request: temperature=%u, delta_uv=%d, transition=%lu, delay=%u\r\n",
         request->ctl_temperature.temperature, request->ctl_temperature.deltauv, transition_ms, delay_ms);

  if ((lightbulb_state.temperature_current == request->ctl_temperature.temperature)
      && (lightbulb_state.deltauv_current == request->ctl_temperature.deltauv)) {
    printf("Request for current state received; no op\n");
  } else {
    if (lightbulb_state.temperature_current != request->ctl_temperature.temperature) {
      printf("Setting temperature to <%u>\r\n", request->ctl_temperature.temperature);
    }
    if (lightbulb_state.deltauv_current != request->ctl_temperature.deltauv) {
      printf("Setting delta UV to <%d>\r\n", request->ctl_temperature.deltauv);
    }
    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.temperature_current = request->ctl_temperature.temperature;
      lightbulb_state.temperature_target = request->ctl_temperature.temperature;
      lightbulb_state.deltauv_current = request->ctl_temperature.deltauv;
      lightbulb_state.deltauv_target = request->ctl_temperature.deltauv;

      // update LED color temperature
      LEDS_SetTemperature(lightbulb_state.temperature_current, lightbulb_state.deltauv_current, 0);
    } else if (delay_ms > 0) {
      // a delay has been specified for the temperature change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.temperature_target = request->ctl_temperature.temperature;
      lightbulb_state.deltauv_target = request->ctl_temperature.deltauv;
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delay_ms), TIMER_ID_DELAYED_CTL_TEMPERATURE, 1);
      // store transition parameter for later use
      delayed_ctl_temperature_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.temperature_target = request->ctl_temperature.temperature;
      lightbulb_state.deltauv_target = request->ctl_temperature.deltauv;

      LEDS_SetTemperature(lightbulb_state.temperature_target, lightbulb_state.deltauv_target, transition_ms);

      // lightbulb current state will be updated when transition is complete
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_CTL_TEMP_TRANSITION, 1);
    }
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    ctl_temperature_response(element_index, client_addr, appkey_index);
  } else {
    ctl_temperature_update(element_index);
  }
}

static void ctl_temperature_change(uint16_t model_id,
                                   uint16_t element_index,
                                   const struct mesh_generic_state *current,
                                   const struct mesh_generic_state *target,
                                   uint32_t remaining_ms)
{
  if (lightbulb_state.temperature_current != current->ctl.temperature) {
    printf("temperature_change: from %u to %u\r\n", lightbulb_state.temperature_current, current->ctl.temperature);
    lightbulb_state.temperature_current = current->ctl.temperature;
    lightbulb_state_changed();
  } else {
    printf("temperature update -same value (%u)\r\n", lightbulb_state.temperature_current);
  }

  if (lightbulb_state.deltauv_current != current->ctl.deltauv) {
    printf("deltauv_change: from %d to %d\r\n", lightbulb_state.deltauv_current, current->ctl.deltauv);
    lightbulb_state.deltauv_current = current->ctl.deltauv;
    lightbulb_state_changed();
  } else {
    printf("deltauv update -same value (%d)\r\n", lightbulb_state.deltauv_current);
  }
}

static errorcode_t sec_level_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.sec_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.sec_level_target;

  return mesh_lib_generic_server_response(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          &target,
                                          0,
                                          0x00);
}

static errorcode_t sec_level_update(uint16_t element_index)
{
  struct mesh_generic_state current, target;

  current.kind = mesh_generic_state_level;
  current.level.level = lightbulb_state.sec_level_current;

  target.kind = mesh_generic_state_level;
  target.level.level = lightbulb_state.sec_level_target;

  return mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        &target,
                                        0);
}

static errorcode_t sec_level_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = sec_level_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_generic_state_level);
  }

  return e;
}

static void sec_level_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  // for simplicity, this demo assumes that all level requests use set level.
  // other type of requests are ignored

  uint16_t temperature;

  if (request->kind != mesh_generic_request_level) {
    return;
  }

  printf("sec_level_request: level=%d, transition=%lu, delay=%u\r\n",
         request->level, transition_ms, delay_ms);

  if (lightbulb_state.sec_level_current == request->level) {
    printf("Request for current state received; no op\n");
  } else {
    printf("Setting sec_level to <%d>\r\n", request->level);

    temperature = lightbulb_state.temperature_min                                       \
                  + (uint32_t)(request->level + (int32_t)32768)                         \
                  * (lightbulb_state.temperature_max - lightbulb_state.temperature_min) \
                  / 65535;

    if (transition_ms == 0 && delay_ms == 0) { // Immediate change
      lightbulb_state.sec_level_current = request->level;
      lightbulb_state.sec_level_target = request->level;
      lightbulb_state.temperature_current = temperature;
      lightbulb_state.temperature_target = temperature;

      // update LED Temperature
      LEDS_SetTemperature(temperature, lightbulb_state.deltauv_current, 0);
    } else if (delay_ms > 0) {
      // a delay has been specified for the light change. Start a soft timer
      // that will trigger the change after the given delay
      // Current state remains as is for now
      lightbulb_state.sec_level_target = request->level;
      lightbulb_state.temperature_target = temperature;
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delay_ms), TIMER_ID_DELAYED_SEC_LEVEL, 1);
      // store transition parameter for later use
      delayed_sec_level_trans = transition_ms;
    } else {
      // no delay but transition time has been set.
      lightbulb_state.sec_level_target = request->level;
      lightbulb_state.temperature_target = temperature;
      LEDS_SetTemperature(temperature, lightbulb_state.deltauv_current, transition_ms);

      // lightbulb current state will be updated when transition is complete
      gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_SEC_LEVEL_TRANSITION, 1);
    }
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    sec_level_response(element_index, client_addr, appkey_index);
  } else {
    sec_level_update(element_index);
  }
}

static void sec_level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  if (lightbulb_state.sec_level_current != current->level.level) {
    printf("sec_level_change: from %d to %d\r\n", lightbulb_state.sec_level_current, current->level.level);
    lightbulb_state.sec_level_current = current->level.level;
    lightbulb_state_changed();
  } else {
    printf("sec_level update -same value (%d)\r\n", lightbulb_state.sec_level_current);
  }
}

static errorcode_t power_onoff_response(uint16_t element_index,
                                        uint16_t client_addr,
                                        uint16_t appkey_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_on_power_up;
  current.on_power_up.on_power_up = lightbulb_state.onpowerup;

  return mesh_lib_generic_server_response(MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          NULL,
                                          0,
                                          0x00);
}

static errorcode_t power_onoff_update(uint16_t element_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_on_power_up;
  current.on_power_up.on_power_up = lightbulb_state.onpowerup;

  return mesh_lib_generic_server_update(MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        NULL,
                                        0);
}

static errorcode_t power_onoff_update_and_publish(uint16_t element_index)
{
  errorcode_t e;

  e = power_onoff_update(element_index);
  if (e == bg_err_success) {
    e = mesh_lib_generic_server_publish(MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_ID,
                                        element_index,
                                        mesh_generic_state_on_power_up);
  }

  return e;
}

static void power_onoff_request(uint16_t model_id,
                                uint16_t element_index,
                                uint16_t client_addr,
                                uint16_t server_addr,
                                uint16_t appkey_index,
                                const struct mesh_generic_request *request,
                                uint32_t transition_ms,
                                uint16_t delay_ms,
                                uint8_t request_flags)
{
  printf("ON POWER UP request received; state=<%s>\n",
         lightbulb_state.onpowerup == 0 ? "OFF"
         : lightbulb_state.onpowerup == 1 ? "ON"
         : "RESTORE");

  if (lightbulb_state.onpowerup == request->on_power_up) {
    printf("Request for current state received; no op\n");
  } else {
    printf("Setting onpowerup to <%s>\n",
           request->on_power_up == 0 ? "OFF"
           : request->on_power_up == 1 ? "ON"
           : "RESTORE");
    lightbulb_state.onpowerup = request->on_power_up;
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    power_onoff_response(element_index, client_addr, appkey_index);
  } else {
    power_onoff_update(element_index);
  }
}

static void power_onoff_change(uint16_t model_id,
                               uint16_t element_index,
                               const struct mesh_generic_state *current,
                               const struct mesh_generic_state *target,
                               uint32_t remaining_ms)
{
  // TODO
}

static errorcode_t transtime_response(uint16_t element_index,
                                      uint16_t client_addr,
                                      uint16_t appkey_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_transition_time;
  current.transition_time.time = lightbulb_state.transtime;

  return mesh_lib_generic_server_response(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                                          element_index,
                                          client_addr,
                                          appkey_index,
                                          &current,
                                          NULL,
                                          0,
                                          0x00);
}

static errorcode_t transtime_update(uint16_t element_index)
{
  struct mesh_generic_state current;
  current.kind = mesh_generic_state_transition_time;
  current.transition_time.time = lightbulb_state.transtime;

  return mesh_lib_generic_server_update(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                                        element_index,
                                        &current,
                                        NULL,
                                        0);
}

static void transtime_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
  printf("TRANSTIME request received; state=<0x%x>\n",
         lightbulb_state.transtime);

  if (lightbulb_state.transtime == request->transition_time) {
    printf("Request for current state received; no op\n");
  } else {
    printf("Setting transtime to <0x%x>\n", request->transition_time);
    lightbulb_state.transtime = request->transition_time;
    lightbulb_state_changed();
  }

  if (request_flags & MESH_REQUEST_FLAG_RESPONSE_REQUIRED) {
    transtime_response(element_index, client_addr, appkey_index);
  } else {
    transtime_update(element_index);
  }
}

static void transtime_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
  // TODO
}

/**
 * This function loads the saved light state from Persistent Storage and
 * copies the data in the global variable lightbulb_state
 */
static int lightbulb_state_load(void)
{
  struct gecko_msg_flash_ps_load_rsp_t* pLoad;

  pLoad = gecko_cmd_flash_ps_load(0x4004);

  if (pLoad->result) {
    memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
    lightbulb_state.lightness_last = 0xFFFF;
    lightbulb_state.temperature_default = DEFAULT_TEMPERATURE;
    lightbulb_state.temperature_min = MIN_TEMPERATURE;
    lightbulb_state.temperature_max = MAX_TEMPERATURE;
    lightbulb_state.deltauv_default = DEFAULT_DELTAUV;
    return -1;
  }

  memcpy(&lightbulb_state, pLoad->value.data, pLoad->value.len);

  return 0;
}

/**
 * this function saves the current light state in Persistent Storage so that
 * the data is preserved over reboots and power cycles. The light state is hold
 * in a global variable lightbulb_state. a PS key with ID 0x4004 is used to store
 * the whole struct.
 */
static int lightbulb_state_store(void)
{
  struct gecko_msg_flash_ps_save_rsp_t* pSave;

  pSave = gecko_cmd_flash_ps_save(0x4004, sizeof(struct lightbulb_state), (const uint8*)&lightbulb_state);

  if (pSave->result) {
    printf("lightbulb_state_store(): PS save failed, code %x\r\n", pSave->result);
    return(-1);
  }

  return 0;
}

/**
 * this function is called each time the lightbulb state in RAM is changed. It sets up a soft timer
 * that will save the state in flash after small delay. The purpose is to reduce amount of unnecessary
 * flash writes.
 */
static void lightbulb_state_changed(void)
{
  gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(5000), TIMER_ID_SAVE_STATE, 1);
}

/**
 * Initialization of the models supported by this node. This function registers callbacks for
 * each of the three supported models.
 */
static void init_models(void)
{
  mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                           0,
                                           onoff_request,
                                           onoff_change);
  mesh_lib_generic_server_register_handler(MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_ID,
                                           0,
                                           power_onoff_request,
                                           power_onoff_change);
  mesh_lib_generic_server_register_handler(MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
                                           0,
                                           transtime_request,
                                           transtime_change);

  mesh_lib_generic_server_register_handler(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
                                           0,
                                           lightness_request,
                                           lightness_change);
  mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                           0,
                                           pri_level_request,
                                           pri_level_change);
  mesh_lib_generic_server_register_handler(MESH_LIGHTING_CTL_SERVER_MODEL_ID,
                                           0,
                                           ctl_request,
                                           ctl_change);
  mesh_lib_generic_server_register_handler(MESH_LIGHTING_CTL_SETUP_SERVER_MODEL_ID,
                                           0,
                                           ctl_setup_request,
                                           ctl_setup_change);
  mesh_lib_generic_server_register_handler(MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
                                           1,
                                           ctl_temperature_request,
                                           ctl_temperature_change);

  // uncomment following lines to enable generic level server on secondary element
  //mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
  //                                         1,
  //                                         sec_level_request,
  //                                         sec_level_change);
}

/**
 * Light node initialization. This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 */
void lightbulb_state_init(void)
{
  uint16 res;

  /* Initialize mesh lib */
  mesh_lib_init(malloc, free, 8);

  //Initialize Friend functionality
  printf("Friend mode initialization\r\n");
  res = gecko_cmd_mesh_friend_init()->result;
  if (res) {
    printf("Friend init failed 0x%x\r\n", res);
  }

  memset(&lightbulb_state, 0, sizeof(struct lightbulb_state));
  if (lightbulb_state_load() != 0) {
    printf("lightbulb_state_load() failed, using defaults\r\n");
    goto publish;
  }

  // Handle on power up behavior
  switch (lightbulb_state.onpowerup) {
    case MESH_GENERIC_ON_POWER_UP_STATE_OFF:
      printf("On power up state is OFF\r\n");
      lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_OFF;
      lightbulb_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_OFF;
      LEDS_SetState(LED_STATE_OFF);
      LEDS_SetTemperature(lightbulb_state.temperature_default, lightbulb_state.deltauv_default, 0);
      break;
    case MESH_GENERIC_ON_POWER_UP_STATE_ON:
      printf("On power up state is ON\r\n");
      lightbulb_state.onoff_current = MESH_GENERIC_ON_OFF_STATE_ON;
      lightbulb_state.onoff_target = MESH_GENERIC_ON_OFF_STATE_ON;
      LEDS_SetState(LED_STATE_ON);
      LEDS_SetTemperature(lightbulb_state.temperature_default, lightbulb_state.deltauv_default, 0);
      break;
    case MESH_GENERIC_ON_POWER_UP_STATE_RESTORE:
      printf("On power up state is RESTORE\r\n");
      if (lightbulb_state.onoff_current != lightbulb_state.onoff_target) {
        uint32_t transition_ms = default_transition_time();

        if (transition_ms > 0) {
          printf("Starting on power up transition\r\n");
          if (lightbulb_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_OFF) {
            LEDS_SetLevel(0, transition_ms);
          } else {
            LEDS_SetLevel(0xFFFF, transition_ms);
          }
          // state is updated when transition is complete
          gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(transition_ms), TIMER_ID_ONOFF_TRANSITION, 1);
        } else {
          // update current state without any transition time
          if (lightbulb_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_OFF) {
            LEDS_SetState(LED_STATE_OFF);
          } else {
            LEDS_SetState(LED_STATE_ON);
          }
          lightbulb_state.onoff_current = lightbulb_state.onoff_target;
        }
      } else {
        printf("Keeping loaded state\r\n");
        if (lightbulb_state.onoff_current == MESH_GENERIC_ON_OFF_STATE_OFF) {
          LEDS_SetState(LED_STATE_OFF);
        } else {
          LEDS_SetState(LED_STATE_ON);
        }
      }

      if (lightbulb_state.temperature_current != lightbulb_state.temperature_target) {
        LEDS_SetTemperature(lightbulb_state.temperature_target, lightbulb_state.deltauv_target, lightbulb_state.transtime);
      } else {
        LEDS_SetTemperature(lightbulb_state.temperature_current, lightbulb_state.deltauv_current, 0);
      }
      break;
  }

  publish:
  lightbulb_state_changed();
  init_models();
  onoff_update_and_publish(_primary_elem_index);
  power_onoff_update_and_publish(_primary_elem_index);

  init_done = 1;
}

static void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);

static void gpio_interrupt_handler_cb(uint8_t gpioPinNo)
{
	printf("gpio_interrupt_handler_cb for pin: %d\r\n", gpioPinNo);
	if (GPIO_PinInGet(BSP_BUTTON_FACTORY_RESET_PORT, BSP_BUTTON_FACTORY_RESET_PIN))
	{
		// BTN state is '1' -> rising edge
		gecko_external_signal(EXT_SIGNAL_FACTORY_BUTTON_RELEASE_SIG);
		printf("factory Reset RELEASE Event\r\n");
	}
	else
	{
		// BTN1 state is '0' -> falling edge
		gecko_external_signal(EXT_SIGNAL_FACTORY_BUTTON_PRESS_SIG);
		printf("factory Reset PRESS Event\r\n");
	}
}
/**
 * button initialization. Configure pushbuttons PB0,PB1
 * as inputs.
 */
static void button_init()
{
  // configure pushbutton PB0 and PB1 as inputs, with pull-up enabled
  GPIO_PinModeSet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN, gpioModeInputPull, 1);
  GPIO_PinModeSet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN, gpioModeInputPull, 1);
#ifdef FACTORY_RESET_BUTTON
  GPIO_PinModeSet(BSP_BUTTON_FACTORY_RESET_PORT, BSP_BUTTON_FACTORY_RESET_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(BSP_BUTTON_FACTORY_RESET_PORT, BSP_BUTTON_FACTORY_RESET_PIN, BSP_BUTTON_FACTORY_RESET_PIN, true, true, true);
  GPIOINT_Init();
  GPIOINT_CallbackRegister(BSP_BUTTON_FACTORY_RESET_PIN, gpio_interrupt_handler_cb);
#endif //FACTORY_RESET_BUTTON
}

/**
 * Set device name in the GATT database. A unique name is generated using
 * the two last bytes from the Bluetooth address of this device. Name is also
 * displayed on the LCD.
 */
void set_device_name(bd_addr *pAddr)
{
  char name[20];
  char led_config[6] = {0x50, 0x51, 0x50, 0x51, 0x50, 0x51};
  uint16 res;

  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "light node %x:%x", pAddr->addr[1], pAddr->addr[0]);

  printf("Device name: '%s'\r\n", name);

  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_led_config, 0, 6, (uint8 *)led_config)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

  // show device name on the LCD
#ifdef DISPLAY_EVAL_KIT
  DI_Print(name, DI_ROW_NAME);
#endif //DISPLAY_EVAL_KIT
}

/**
 * Set the led channel configuration read from the Flash and write
 * the configuration in LED configuration characteristic attributes.
 */
void set_led_config_gatt_attribute(void)
{
  char led_config[7] = {0};
  uint16 res;

  /* Write led configuration read from flash memory */
  memcpy(led_config, Current_LED_Configuration.LED_Configuration_Data, sizeof(Current_LED_Configuration.LED_Configuration_Data));

  if(Current_LED_Configuration.Is_LED_Configuration_Set == 0x55)
  {
	  led_config[6] = 0x00;
  }
  else if(Current_LED_Configuration.Is_LED_Configuration_Set == 0xAA)
  {
	  led_config[6] = 0x01;
  }

  printf("LED channel configuration: %c %c %c %c %c %c\r\n", led_config[0], led_config[1],
			led_config[2], led_config[3], led_config[4], led_config[5]);


  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_led_config, 0, 7, (uint8 *)led_config)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }
}

/**
 * Set the led channel intensity read from the Flash and write
 * the configuration in LED intensity characteristic attributes.
 */
void set_led_intensity_gatt_attribute(void)
{
  char led_intensity[6] = {0};
  uint16 res;

  led_intensity[0] = (Current_LED_Configuration.LED_Channel_intensity[0]);
  led_intensity[1] = (Current_LED_Configuration.LED_Channel_intensity[1]);
  led_intensity[2] = (Current_LED_Configuration.LED_Channel_intensity[2]);
  led_intensity[3] = (Current_LED_Configuration.LED_Channel_intensity[3]);
  led_intensity[4] = (Current_LED_Configuration.LED_Channel_intensity[4]);
  led_intensity[5] = (Current_LED_Configuration.LED_Channel_intensity[5]);


  printf("LED channel intensity percentage: %d %d %d %d %d %d\r\n", led_intensity[0], led_intensity[1],
		  led_intensity[2], led_intensity[3], led_intensity[4], led_intensity[5]);


  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_led_intensity, 0, 6, (uint8 *)led_intensity)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

  /* Send Notify to central device */
  gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_led_intensity, 6, (uint8 *)led_intensity);
}

/**
 * This function is called when a light on/off request with non-zero transition time has completed
 */
void onoff_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.onoff_current = lightbulb_state.onoff_target;

  printf("transition complete. New state is %s\r\n", lightbulb_state.onoff_current ? "ON" : "OFF");

  lightbulb_state_changed();
  onoff_update_and_publish(_primary_elem_index);
}

/**
 * This function is called when a lightness request with non-zero transition time has completed
 */
void lightness_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.lightness_current = lightbulb_state.lightness_target;
  if (lightbulb_state.lightness_target != 0) {
    lightbulb_state.lightness_last = lightbulb_state.lightness_target;
  }

  printf("transition complete. New level is %u\r\n", lightbulb_state.lightness_current);

  lightbulb_state_changed();
  lightness_update_and_publish(_primary_elem_index);
}

void pri_level_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.pri_level_current = lightbulb_state.pri_level_target;
  lightbulb_state.lightness_current = lightbulb_state.lightness_target;

  printf("transition complete. New pri_level is %d\r\n", lightbulb_state.pri_level_current);

  lightbulb_state_changed();
  pri_level_update_and_publish(_primary_elem_index);
}

void ctl_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.lightness_current = lightbulb_state.lightness_target;
  lightbulb_state.temperature_current = lightbulb_state.temperature_target;
  lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

  printf("transition complete. New lightness is %u, new temperature is %u and new deltauv is %d\r\n", lightbulb_state.lightness_current, lightbulb_state.temperature_current, lightbulb_state.deltauv_current);

  lightbulb_state_changed();
  ctl_update_and_publish(_primary_elem_index);
}

void ctl_temperature_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.temperature_current = lightbulb_state.temperature_target;
  lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

  printf("transition complete. New temperature is %u and new deltauv is %d\r\n", lightbulb_state.temperature_current, lightbulb_state.deltauv_current);

  lightbulb_state_changed();
  ctl_temperature_update_and_publish(_secondary_elem_index);
}

void sec_level_transition_complete()
{
  // transition done -> set state, update and publish
  lightbulb_state.sec_level_current = lightbulb_state.sec_level_target;
  lightbulb_state.temperature_current = lightbulb_state.temperature_target;

  printf("transition complete. New sec_level is %d\r\n", lightbulb_state.sec_level_current);

  lightbulb_state_changed();
  sec_level_update_and_publish(_secondary_elem_index);
}

void delayed_onoff_request()
{
  printf("starting delayed on/off request: %u -> %u, %lu ms\r\n",
         lightbulb_state.onoff_current,
         lightbulb_state.onoff_target,
         delayed_onoff_trans
         );

  if (delayed_onoff_trans == 0) {
    // no transition delay, update state immediately

    lightbulb_state.onoff_current = lightbulb_state.onoff_target;
    if (lightbulb_state.onoff_current == MESH_GENERIC_ON_OFF_STATE_OFF) {
      LEDS_SetState(LED_STATE_OFF);
    } else {
      // restore last brightness level
      LEDS_SetLevel(lightbulb_state.lightness_last, 0);
      lightbulb_state.lightness_current = lightbulb_state.lightness_last;
    }

    lightbulb_state_changed();
    onoff_update_and_publish(_primary_elem_index);
  } else {
    if (lightbulb_state.onoff_target == MESH_GENERIC_ON_OFF_STATE_OFF) {
      LEDS_SetLevel(0, delayed_onoff_trans);
    } else {
      // restore last brightness level, with transition delay
      LEDS_SetLevel(lightbulb_state.lightness_last, delayed_onoff_trans);
      lightbulb_state.lightness_target = lightbulb_state.lightness_last;
    }

    // state is updated when transition is complete
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delayed_onoff_trans), TIMER_ID_ONOFF_TRANSITION, 1);
  }
}

void delayed_lightness_request()
{
  printf("starting delayed lightness request: level %u -> %u, %lu ms\r\n",
         lightbulb_state.lightness_current,
         lightbulb_state.lightness_target,
         delayed_lightness_trans
         );

  LEDS_SetLevel(lightbulb_state.lightness_target, delayed_lightness_trans);

  if (delayed_lightness_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.lightness_current = lightbulb_state.lightness_target;
    if (lightbulb_state.lightness_target != 0) {
      lightbulb_state.lightness_last = lightbulb_state.lightness_target;
    }

    lightbulb_state_changed();
    lightness_update_and_publish(_primary_elem_index);
  } else {
    // state is updated when transition is complete
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delayed_lightness_trans), TIMER_ID_LIGHTNESS_TRANSITION, 1);
  }
}

void delayed_pri_level_request()
{
  printf("starting delayed primary level request: level %d -> %d, %lu ms\r\n",
         lightbulb_state.pri_level_current,
         lightbulb_state.pri_level_target,
         delayed_pri_level_trans
         );

  LEDS_SetLevel(lightbulb_state.lightness_target, delayed_pri_level_trans);

  /* extract the LED channel information */
  LED_Channel = extract_led_channel(delayed_pri_level_trans);
  delayed_pri_level_trans = 0;

  /* control the LED channel intensity directly */
  LEDS_SetLevel_Dpot(lightbulb_state.lightness_target, LED_Channel);


  if (delayed_pri_level_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.pri_level_current = lightbulb_state.pri_level_target;
    lightbulb_state.lightness_current = lightbulb_state.lightness_target;

    lightbulb_state_changed();
    pri_level_update_and_publish(_primary_elem_index);
  } else {
    // state is updated when transition is complete
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delayed_pri_level_trans), TIMER_ID_PRI_LEVEL_TRANSITION, 1);
  }
}

void delayed_ctl_request()
{
  printf("starting delayed ctl request: lightness %u -> %u, temperature %u -> %u, deltauv %d -> %d, %lu ms\r\n",
         lightbulb_state.lightness_current,
         lightbulb_state.lightness_target,
         lightbulb_state.temperature_current,
         lightbulb_state.temperature_target,
         lightbulb_state.deltauv_current,
         lightbulb_state.deltauv_target,
         delayed_ctl_trans
         );

  LEDS_SetLevel(lightbulb_state.lightness_target, delayed_ctl_trans);
  LEDS_SetTemperature(lightbulb_state.temperature_target, lightbulb_state.deltauv_target, delayed_ctl_trans);

  if (delayed_ctl_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.lightness_current = lightbulb_state.lightness_target;
    lightbulb_state.temperature_current = lightbulb_state.temperature_target;
    lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

    lightbulb_state_changed();
    ctl_update_and_publish(_primary_elem_index);
  } else {
    // state is updated when transition is complete
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delayed_ctl_trans), TIMER_ID_CTL_TRANSITION, 1);
  }
}

void delayed_ctl_temperature_request()
{
  printf("starting delayed ctl temperature request: temperature %u -> %u, deltauv %d -> %d, %lu ms\r\n",
         lightbulb_state.temperature_current,
         lightbulb_state.temperature_target,
         lightbulb_state.deltauv_current,
         lightbulb_state.deltauv_target,
         delayed_ctl_temperature_trans
         );

  LEDS_SetTemperature(lightbulb_state.temperature_target, lightbulb_state.deltauv_target, delayed_ctl_temperature_trans);

  if (delayed_ctl_temperature_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.temperature_current = lightbulb_state.temperature_target;
    lightbulb_state.deltauv_current = lightbulb_state.deltauv_target;

    lightbulb_state_changed();
    ctl_temperature_update_and_publish(_secondary_elem_index);
  } else {
    // state is updated when transition is complete
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delayed_ctl_temperature_trans), TIMER_ID_CTL_TEMP_TRANSITION, 1);
  }
}

void delayed_sec_level_request()
{
  printf("starting delayed secondary level request: level %d -> %d, %lu ms\r\n",
         lightbulb_state.sec_level_current,
         lightbulb_state.sec_level_target,
         delayed_sec_level_trans
         );

  LEDS_SetTemperature(lightbulb_state.temperature_target, lightbulb_state.deltauv_current, delayed_sec_level_trans);

  if (delayed_sec_level_trans == 0) {
    // no transition delay, update state immediately
    lightbulb_state.sec_level_current = lightbulb_state.sec_level_target;
    lightbulb_state.temperature_current = lightbulb_state.temperature_target;

    lightbulb_state_changed();
    sec_level_update_and_publish(_secondary_elem_index);
  } else {
    // state is updated when transition is complete
    gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(delayed_sec_level_trans), TIMER_ID_SEC_LEVEL_TRANSITION, 1);
  }
}

static void server_state_changed(struct gecko_msg_mesh_generic_server_state_changed_evt_t *pEvt)
{
  int i;

  printf("state changed: ");
  printf("model ID %4.4x, type %2.2x ", pEvt->model_id, pEvt->type);
  for (i = 0; i < pEvt->parameters.len; i++) {
    printf("%2.2x ", pEvt->parameters.data[i]);
  }
  printf("\r\n");
}

/**
 *  this function is called to initiate factory reset. Factory reset may be initiated
 *  by keeping one of the WSTK pushbuttons pressed during reboot. Factory reset is also
 *  performed if it is requested by the provisioner (event gecko_evt_mesh_node_reset_id)
 */
void initiate_factory_reset(void)
{
  printf("factory reset\r\n");
#ifdef DISPLAY_EVAL_KIT
  DI_Print("\n***\nFACTORY RESET\n***", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT

  /* set the LED configuration and intensity to default state */
  reset_led_configuration_flash();

  /* if connection is open then close it before rebooting */
  if (conn_handle != 0xFF) {
    gecko_cmd_le_connection_close(conn_handle);
  }

  /* perform a factory reset by erasing PS storage. This removes all the keys and other settings
     that have been configured for this node */
  gecko_cmd_flash_ps_erase_all();
  // reboot after a small delay
  gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FACTORY_RESET, 1);
}

/**
 *  this function is for Interrupt Service Routine for system tick counter
 */
void SysTick_Handler(void) {
  msTicks++; /* increment counter necessary in Delay()*/
}

/**
 *  this function is for Delays number of msTick Systicks (typically 1 ms)
 */
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/**
 *  this function is for LED channel light intensity update request
 *  to ZIGBEE module over USART.
 */
void lightness_request_usart(uint16_t led_intensity, uint16_t led_channel_num)
{
	ledPayload ledRequestData;

	ledRequestData.requestType = CONTROL;
	ledRequestData.ledNumber = led_channel_num;

	/* Assign the requested LED channel intensity value */
	ledRequestData.ledLevel[led_channel_num-1] = (int) ceil((float)led_intensity * PERCENTAGE_FACTOR);

	/* send the light intensity update request to zigbee module over
	 * usart interface
	 */
	ZigbeeUsartSend(&ledRequestData);

}

/**
 *  this function is for LED channel default light intensity request
 *  to ZIGBEE module over USART.
 */
void default_lightness_request_usart()
{
	ledPayload ledRequestData;

	ledRequestData.requestType = CONTROL;
	ledRequestData.ledNumber = LED_ALL;

	/* Assign the requested LED channel intensity value */
	memcpy(ledRequestData.ledLevel,
			Default_LED_Configuration.LED_Channel_intensity,
			sizeof(Default_LED_Configuration.LED_Channel_intensity));

	/* send the light intensity update request to zigbee module over
	 * usart interface
	 */
	ZigbeeUsartSend(&ledRequestData);
}

/**
 *  this function is for LED configuration and current LED channel state write
 *  to external flash memory.
 */
int writeLedStateFlash(LED_Configuration *LED_Configuration_state)
{
	int status = -1;
	if(LED_Configuration_state == NULL)
	{
		return status;
	}

	WaitFlashReady(100);

	/* erase the flash memory sector before write */
	status = MX25_SE(LED_CONFIGURATION_ADDR);
	WaitFlashReady(100);

	/* Write the data in the flash memory sector*/
	status = MX25_PP(LED_CONFIGURATION_ADDR, (uint8_t *) LED_Configuration_state,
			sizeof(LED_Configuration));
	printf("flash write : 0x%x\r\n", LED_Configuration_state->Is_LED_Configuration_Set);
	printf("flash write : %d %d %d %d %d %d\r\n", LED_Configuration_state->LED_Channel_intensity[0],
			LED_Configuration_state->LED_Channel_intensity[1], LED_Configuration_state->LED_Channel_intensity[2],
			LED_Configuration_state->LED_Channel_intensity[3], LED_Configuration_state->LED_Channel_intensity[4],
			LED_Configuration_state->LED_Channel_intensity[5]);

	WaitFlashReady(100);

	/* Extract the last state of LED channel from flash memory */
	readLedStateFlash(&Temp_LED_Configuration);

	if(memcmp(LED_Configuration_state, (uint8_t *)&Temp_LED_Configuration, sizeof(LED_Configuration)) == 0)
	{
		printf("LED Configuration updated in flash.\r\n");
		status = 0;
	}
	else
	{
		printf("Failed to update LED configuration into flash memory\r\n");
		status = -1;
	}

    return status;
}

/**
 *  this function is for LED configuration and current LED channel state write
 *  to external flash memory.
 */
int readLedStateFlash(LED_Configuration *LED_Configuration_state)
{
	int status = -1;
	if(LED_Configuration_state == NULL)
	{
		return status;
	}

	/* read the flash memory sector before write */
	status = MX25_READ(LED_CONFIGURATION_ADDR, (uint8_t *) LED_Configuration_state,
			sizeof(LED_Configuration));
	if(status != 0)
	{
		printf("Fail to read the LED configuration from FLash memory\r\n");
		return status;
	}
	printf("flash read : 0x%x\r\n", LED_Configuration_state->Is_LED_Configuration_Set);
	printf("flash read : %d %d %d %d %d %d\r\n", LED_Configuration_state->LED_Channel_intensity[0],
			LED_Configuration_state->LED_Channel_intensity[1], LED_Configuration_state->LED_Channel_intensity[2],
			LED_Configuration_state->LED_Channel_intensity[3], LED_Configuration_state->LED_Channel_intensity[4],
			LED_Configuration_state->LED_Channel_intensity[5]);

	printf("flash config : %c %c %c %c %c %c\r\n", LED_Configuration_state->LED_Configuration_Data[0],
			LED_Configuration_state->LED_Configuration_Data[1], LED_Configuration_state->LED_Configuration_Data[2],
			LED_Configuration_state->LED_Configuration_Data[3], LED_Configuration_state->LED_Configuration_Data[4],
			LED_Configuration_state->LED_Configuration_Data[5]);

	return status;

}

/**
 *  this function is for LED intensity change based on the zigbee module
 *  availability. if the zigbee module is not present then set led
 *  intensity through digital potentiometer directly.
 */
int LEDS_SetLevel_Dpot(uint16_t led_intensity, uint16_t led_channel_num)
{
	printf("LED Num: %lu and intensity: %lu\r\n", led_channel_num, led_intensity);
	int status = -1;
    if (zigbeeAvailable) {
		  /* send the LED intensity control request to zigbee module */
		  lightness_request_usart(led_intensity, led_channel_num);
    }
    else
    {
		  /* control the LED channel intensity directly using digital
		   * potentiometer as the zigbee module is not available. */
		  halAD5204DriverSetLedLevel(led_channel_num, led_intensity);
    }
    /* store the updated LED status on Flash memory */
    Current_LED_Configuration.LED_Channel_intensity[led_channel_num-1] = (int) ceil((float)led_intensity * PERCENTAGE_FACTOR);

    /* set the LED channel intensity characteristics to latest value info */
    set_led_intensity_gatt_attribute();

    status = writeLedStateFlash(&Current_LED_Configuration);
    if(status != 0)
    {
        printf("Fail to write LED configuration in flash memory\r\n");
        printf("Retry the Flash write...\r\n");
        status = writeLedStateFlash(&Current_LED_Configuration);
        if (status != 0)
        {
            printf("Flash memory write fail again.\r\n");
        }
    }
    return 0;
}

/**
 * Initialize the Flash with default LED configuration.
 */
static void default_led_configuration_flash(void)
{
	int status = -1;
    printf("Writing default value in flash for LED configuration.\r\n");
    /* No configuration written to flash memory, write the default configuration */
    memcpy(&Current_LED_Configuration, &Default_LED_Configuration, sizeof(LED_Configuration));
    status = writeLedStateFlash(&Current_LED_Configuration);
    if (status != 0)
    {
        printf("Fail to write LED configuration in flash memory\r\n");
        printf("Retry the Flash write\r\n");
        status = writeLedStateFlash(&Current_LED_Configuration);
        if (status != 0)
        {
            printf("Flash memory write fail again.\r\n");
        }
    }
}

/**
 * reset LED configuration.
 */
void reset_led_configuration_flash(void)
{
	int status = -1;
	uint8_t loop = 0;
    printf("Reset LED configuration channel mapping.\r\n");

    Current_LED_Configuration.Is_BLE_Provisioned = 0x00;

    if(Current_LED_Configuration.Is_ZIGBEE_Provisioned != 0x01)
    {
    	/* Reset the LED Configuration Flag */
    	Current_LED_Configuration.Is_LED_Configuration_Set = 0x55;

        /* Set the LED channel intensity to default state */
		memcpy(Current_LED_Configuration.LED_Channel_intensity,
				Default_LED_Configuration.LED_Channel_intensity,
				sizeof(Default_LED_Configuration.LED_Channel_intensity));

        printf("Setting default led intensity for all the LED channel\r\n");
        if(zigbeeAvailable)
        {
            default_lightness_request_usart();
        }
    }
    status = writeLedStateFlash(&Current_LED_Configuration);
    if (status != 0)
    {
        printf("Fail to write LED configuration in flash memory\r\n");
        printf("Retry the Flash write\r\n");
        status = writeLedStateFlash(&Current_LED_Configuration);
        if (status != 0)
        {
            printf("Flash memory write fail again.\r\n");
        }
    }
}

/**
 * Handling gpio event signal from BLE signal handler.
 */
void gpio_irq_handler(uint32 signal)
{
	struct gecko_msg_hardware_get_time_rsp_t *pTimeNow;
	uint32 rtcc_now;
	static uint32 pb1_press_time;
	uint32 duration;

	pTimeNow = gecko_cmd_hardware_get_time();

	// convert seconds + subsecond ticks into raw RTCC counter value:
	rtcc_now = pTimeNow->seconds * 32768 + pTimeNow->ticks;

	switch(signal)
	{

	case EXT_SIGNAL_FACTORY_BUTTON_PRESS_SIG:
		pb1_press_time = rtcc_now;
		break;

	case EXT_SIGNAL_FACTORY_BUTTON_RELEASE_SIG:
		duration = 	rtcc_now - pb1_press_time;
		if(duration > FACTORY_BUTTON_LONG_PRESS_DURATION)
		{
			printf("Factory Reset button long press, resetting all the configuration.\r\n");
//			reset_led_configuration_flash();
			initiate_factory_reset();
		}
		else
		{
			// sshort press
			printf("Factory Reset button short pressed\r\n");
		}
		break;

	default:
		break;
	}
}

int main()
{
  uint32_t btl_version = 0;
  uint32_t mfgId = 0;

  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  gecko_stack_init(&config);
  gecko_bgapi_class_dfu_init();
  gecko_bgapi_class_system_init();
  gecko_bgapi_class_le_gap_init();
  gecko_bgapi_class_le_connection_init();
  //gecko_bgapi_class_gatt_init();
  gecko_bgapi_class_gatt_server_init();
  gecko_bgapi_class_endpoint_init();
  gecko_bgapi_class_hardware_init();
  gecko_bgapi_class_flash_init();
  gecko_bgapi_class_test_init();
  //gecko_bgapi_class_sm_init();
  //mesh_native_bgapi_init();
  gecko_bgapi_class_mesh_node_init();
  //gecko_bgapi_class_mesh_prov_init();
  gecko_bgapi_class_mesh_proxy_init();
  gecko_bgapi_class_mesh_proxy_server_init();
  //gecko_bgapi_class_mesh_proxy_client_init();
  //gecko_bgapi_class_mesh_generic_client_init();
  gecko_bgapi_class_mesh_generic_server_init();
  //gecko_bgapi_class_mesh_vendor_model_init();
  //gecko_bgapi_class_mesh_health_client_init();
  //gecko_bgapi_class_mesh_health_server_init();
  //gecko_bgapi_class_mesh_test_init();
  //gecko_bgapi_class_mesh_lpn_init();
  gecko_bgapi_class_mesh_friend_init();

  gecko_initCoexHAL();

  RETARGET_SerialInit();

  /* Setup SysTick Timer for 1 msec interrupts */
  if (SysTick_Config(SystemCoreClock / 1000)) while (1) ;

  /* Initialize UART for communication with Zigbee module */
  ZIGBEE_SerialInit();

  /* initialize AD5206 digital potentiometer over SPI */
  dpotAd5206Init();

  /* initialize LEDs and buttons. Note: some radio boards share the same GPIO for button & LED.
   * Initialization is done in this order so that default configuration will be "button" for those
   * radio boards with shared pins. LEDS_init() is called later as needed to (re)initialize the LEDs
   * */
  LEDS_init();
  button_init();

#ifdef DISPLAY_EVAL_KIT
  DI_Init();
#endif //DISPLAY_EVAL_KIT

  /* Check for zigbee module presence in LED Node using the GPIO shared between
   * Zigbee and BLE modules. When the GPIO is LOW, Zigbee module is present otherwise the
   * zigbee module is not available */
  IsZigbeeAvailable();

  /* init watchdog */
  WDOG_Init(&init);

  btl_version = mainBootloaderTable->header.version & 0x000F;
  printf("Bootloader Version : %lu, Application Version : %lu\r\n",
                           btl_version,
                           bg_application_properties.app.version);

  MX25_RDID(&mfgId);
  printf("FLash memory manufactor ID: %x\r\n", mfgId);

  while (1) {
	/* Non-Blocking call for gecko events */
    struct gecko_cmd_packet *evt = gecko_peek_event();

    /* Blocking call for gecko events */
	//struct gecko_cmd_packet *evt = gecko_wait_event();

    if(evt != NULL) {
      bool pass = mesh_bgapi_listener(evt);
      if (pass) {
        handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
      }
    }
    if(zigbeeAvailable)
	{
        ZIGBEEUartProcessInput();
	}
  }
}

/**
 * Handling of stack events. Both BLuetooth LE and Bluetooth mesh events are handled here.
 */
static void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
  uint16_t result;
  char buf[30];
  int status = -1;
  uint8_t loop = 0;


  struct gecko_msg_mesh_node_provisioning_failed_evt_t  *prov_fail_evt;

  if (NULL == evt) {
    return;
  }

  switch (evt_id) {
    case gecko_evt_system_boot_id:
      // check pushbutton state at startup. If either PB0 or PB1 is held down then do factory reset
      if (GPIO_PinInGet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN) == 0 || GPIO_PinInGet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN) == 0) {
        initiate_factory_reset();
      } else {
        struct gecko_msg_system_get_bt_address_rsp_t *pAddr = gecko_cmd_system_get_bt_address();

        set_device_name(&pAddr->address);

        // Initialize Mesh stack in Node operation mode, wait for initialized event
        result = gecko_cmd_mesh_node_init()->result;
        if (result) {
          sprintf(buf, "init failed (0x%x)", result);
#ifdef DISPLAY_EVAL_KIT
          DI_Print(buf, DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT
        }

        // re-initialize LEDs (needed for those radio board that share same GPIO for button/LED)
        LEDS_init();

        /* Start soft timer to feed the watchdog every second.
         * If the stack freezes, the soft timer will stop, and the watchdog will not be fed. */
        gecko_cmd_hardware_set_soft_timer(32768,TIMER_ID_WATCHDOG,0);

      }

      /* Extract the last state of LED channel from flash memory */
      readLedStateFlash(&Current_LED_Configuration);

      if(Current_LED_Configuration.Is_LED_Configuration_Set != 0x55 && Current_LED_Configuration.Is_LED_Configuration_Set != 0xAA)
      {
        default_led_configuration_flash();
      }

      /* set the LED configuration in characteristics attribute */
      set_led_config_gatt_attribute();

      /* set the LED channel intensity in characteristics attribute */
      set_led_intensity_gatt_attribute();

      /* After reset, Initialize the Intensity as per previous value or Default value */
      if (!zigbeeAvailable)
      {
          /* set the LED intesity only if the zigbee module is not available. As when
           * the zigbee module is available, it will set the last LED intesity state
           * on boot up always.
           */
          printf("Setting initial led intensity for all the LED channel\r\n");
          for(loop=0; loop < LED_CHANNELS; loop++)
          {
              halAD5204DriverSetLedLevel(loop+1, (int)(Current_LED_Configuration.LED_Channel_intensity[loop] * PERCENTAGE_VALUE_FACTOR));
          }
      }

      if(zigbeeAvailable)
      {
          /* Request for the current LED channels intensity status from Zigbee only for first time */
          ZigbeeUsartRequest();

          /* send BLE privision state to Zigbee only for first time */
          sendBLEProvisionState();

          /* Request for the zigbee provision state for first time */
          IsZigbeeProvisioned();

      }
      break;

    case gecko_evt_hardware_soft_timer_id:
      switch (evt->data.evt_hardware_soft_timer.handle) {

		case TIMER_ID_WATCHDOG:
		  /* Feed the watchdog every second while the stack is running */
		  WDOG_Feed();
		  break;

        case TIMER_ID_FACTORY_RESET:
          gecko_cmd_system_reset(0);
          break;

        case TIMER_ID_RESTART:
          gecko_cmd_system_reset(0);
          break;

        case TIMER_ID_PROVISIONING:
          LEDS_SetState(LED_STATE_PROV);
          break;

        case TIMER_ID_SAVE_STATE:
          lightbulb_state_store();
          break;

        case TIMER_ID_DELAYED_ONOFF:
          /* delay for an on/off request has passed, now process the request */
          delayed_onoff_request();
          break;

        case TIMER_ID_DELAYED_LIGHTNESS:
          /* delay for a lightness request has passed, now process the request */
          delayed_lightness_request();
          break;

        case TIMER_ID_DELAYED_PRI_LEVEL:
          /* delay for a primary generic level request has passed, now process the request */
          delayed_pri_level_request();
          break;

        case TIMER_ID_DELAYED_CTL:
          /* delay for a ctl request has passed, now process the request */
          delayed_ctl_request();
          break;

        case TIMER_ID_DELAYED_CTL_TEMPERATURE:
          /* delay for a ctl temperature request has passed, now process the request */
          delayed_ctl_temperature_request();
          break;

        case TIMER_ID_DELAYED_SEC_LEVEL:
          /* delay for a secondary generic level request has passed, now process the request */
          delayed_sec_level_request();
          break;

        case TIMER_ID_ONOFF_TRANSITION:
          onoff_transition_complete();
          break;

        case TIMER_ID_LIGHTNESS_TRANSITION:
          lightness_transition_complete();
          break;

        case TIMER_ID_PRI_LEVEL_TRANSITION:
          pri_level_transition_complete();
          break;

        case TIMER_ID_CTL_TRANSITION:
          ctl_transition_complete();
          break;

        case TIMER_ID_CTL_TEMP_TRANSITION:
          ctl_temperature_transition_complete();
          break;

        case TIMER_ID_SEC_LEVEL_TRANSITION:
          sec_level_transition_complete();
          break;

        default:
          break;
      }

      break;

    case gecko_evt_mesh_node_initialized_id:
      printf("node initialized\r\n");

      gecko_cmd_mesh_generic_server_init();

      struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

      if (pData->provisioned) {
        printf("node is provisioned. address:%x, ivi:%ld\r\n", pData->address, pData->ivi);

        _my_address = pData->address;
        _primary_elem_index = 0;   // index of primary element is zero.
        _secondary_elem_index = 1; // index of secondary element is one.
        lightbulb_state_init();

        printf("Light initial state is <%s>\r\n", lightbulb_state.onoff_current ? "ON" : "OFF");
#ifdef DISPLAY_EVAL_KIT
        DI_Print("provisioned", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT
      } else {
        printf("node is unprovisioned\r\n");

#ifdef DISPLAY_EVAL_KIT
        DI_Print("unprovisioned", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT

        printf("starting unprovisioned beaconing...\r\n");
        gecko_cmd_mesh_node_start_unprov_beaconing(0x3);   // enable ADV and GATT provisioning bearer
      }
      break;

    case gecko_evt_mesh_node_provisioning_started_id:
      printf("Started provisioning\r\n");

#ifdef DISPLAY_EVAL_KIT
      DI_Print("provisioning...", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT

      // start timer for blinking LEDs to indicate which node is being provisioned
      gecko_cmd_hardware_set_soft_timer(32768 / 4, TIMER_ID_PROVISIONING, 0);
      break;

    case gecko_evt_mesh_node_provisioned_id:
      _primary_elem_index = 0;   // index of primary element is zero.
      _secondary_elem_index = 1; // index of secondary element is one.
      lightbulb_state_init();
      printf("node provisioned, got address=%x\r\n", evt->data.evt_mesh_node_provisioned.address);
      // stop LED blinking when provisioning complete

      /* update the provision state of BLE in flash */
      Current_LED_Configuration.Is_BLE_Provisioned = 0x01;
      writeLedStateFlash(&Current_LED_Configuration);

      /* send BLE module provision state to zigbee module over UART */
      sendBLEProvisionState();

      gecko_cmd_hardware_set_soft_timer(0, TIMER_ID_PROVISIONING, 0);
      LEDS_SetState(LED_STATE_OFF);
      LEDS_SetTemperature(DEFAULT_TEMPERATURE, DEFAULT_DELTAUV, 0);

#ifdef DISPLAY_EVAL_KIT
      DI_Print("provisioned", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT

      break;

    case gecko_evt_mesh_node_provisioning_failed_id:
      prov_fail_evt = (struct gecko_msg_mesh_node_provisioning_failed_evt_t  *)&(evt->data);
      printf("provisioning failed, code %x\r\n", prov_fail_evt->result);

#ifdef DISPLAY_EVAL_KIT
      DI_Print("prov failed", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT

      /* start a one-shot timer that will trigger soft reset after small delay */
      gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_RESTART, 1);
      break;

    case gecko_evt_mesh_node_key_added_id:
      printf("got new %s key with index %x\r\n", evt->data.evt_mesh_node_key_added.type == 0 ? "network" : "application",
             evt->data.evt_mesh_node_key_added.index);
      break;

    case gecko_evt_mesh_node_model_config_changed_id:
      printf("model config changed\r\n");
      break;

    case gecko_evt_mesh_generic_server_client_request_id:
      printf("evt gecko_evt_mesh_generic_server_client_request_id\r\n");
      mesh_lib_generic_server_event_handler(evt);
      break;

    case gecko_evt_mesh_generic_server_state_changed_id:
      // uncomment following line to get debug prints for each server state changed event
      //server_state_changed(&(evt->data.evt_mesh_generic_server_state_changed));

      // pass the server state changed event to mesh lib handler that will invoke
      // the callback functions registered by application
      mesh_lib_generic_server_event_handler(evt);
      break;

    case gecko_evt_mesh_node_reset_id:
      printf("evt gecko_evt_mesh_node_reset_id\r\n");
      initiate_factory_reset();
      break;

    case gecko_evt_mesh_friend_friendship_established_id:
      printf("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%x\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);

#ifdef DISPLAY_EVAL_KIT
      DI_Print("FRIEND", DI_ROW_FRIEND);
#endif //DISPLAY_EVAL_KIT

      break;

    case gecko_evt_mesh_friend_friendship_terminated_id:
      printf("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
#ifdef DISPLAY_EVAL_KIT
      DI_Print("NO LPN", DI_ROW_FRIEND);
#endif //DISPLAY_EVAL_KIT
      break;

    case gecko_evt_le_gap_adv_timeout_id:
      // adv timeout events silently discarded
      break;

    case gecko_evt_le_connection_opened_id:
      printf("evt:gecko_evt_le_connection_opened_id\r\n");
      num_connections++;
      conn_handle = evt->data.evt_le_connection_opened.connection;
#ifdef DISPLAY_EVAL_KIT
      DI_Print("connected", DI_ROW_CONNECTION);
#endif //DISPLAY_EVAL_KIT
      break;

    case gecko_evt_le_connection_parameters_id:
      printf("evt:gecko_evt_le_connection_parameters_id\r\n");
      break;

    case gecko_evt_le_connection_closed_id:
      /* Check if need to boot to dfu mode */
      if (boot_to_dfu) {
        /* Enter to DFU OTA mode */
        gecko_cmd_system_reset(2);
      }

      printf("evt:conn closed, reason 0x%x\r\n", evt->data.evt_le_connection_closed.reason);
      conn_handle = 0xFF;
      if (num_connections > 0) {
        if (--num_connections == 0) {
#ifdef DISPLAY_EVAL_KIT
          DI_Print("", DI_ROW_CONNECTION);
#endif //DISPLAY_EVAL_KIT
        }
      }
      break;
    case gecko_evt_gatt_server_user_write_request_id:
      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
        /* Set flag to enter to OTA mode */
        boot_to_dfu = 1;
        /* Send response to Write Request */
        gecko_cmd_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control,
          bg_err_success);

        /* Close connection to enter to DFU OTA mode */
        gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
      }
      break;

    case gecko_evt_system_external_signal_id:
    {
      uint16_t current_level;
      char tmp[21];

      if (init_done && (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_LED_LEVEL_CHANGED)) {
        /* this signal from the LED PWM driver indicates that the level has changed,
         * we use it here to update the LCD status */
        current_level = LEDS_GetLevel();
        sprintf(tmp, "Lightness: %5u%%", ++current_level * 100 / 65535);
#ifdef DISPLAY_EVAL_KIT
        DI_Print(tmp, DI_ROW_LIGHTNESS);
#endif //DISPLAY_EVAL_KIT
      }

      if (init_done && (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_LED_TEMPERATURE_CHANGED)) {
        /* this signal from the LED driver indicates that the temperature level has changed,
         * we use it here to update the LCD status */
        current_level = LEDS_GetTemperature();
        sprintf(tmp, "ColorTemp: %5uK", current_level);
#ifdef DISPLAY_EVAL_KIT
        DI_Print(tmp, DI_ROW_TEMPERATURE);
#endif //DISPLAY_EVAL_KIT

        current_level = LEDS_GetDeltaUV();
        sprintf(tmp, "Delta UV: %6d ", current_level);
#ifdef DISPLAY_EVAL_KIT
        DI_Print(tmp, DI_ROW_DELTAUV);
#endif //DISPLAY_EVAL_KIT
      }

		if ((evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_FACTORY_BUTTON_PRESS_SIG)
				|| (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_FACTORY_BUTTON_RELEASE_SIG)) {
          /* this signal from the Button event indicates that the button state has changed,
           * we use it here to get the long press event */
          gpio_irq_handler(evt->data.evt_system_external_signal.extsignals);
      }
    }
    break;
    case gecko_evt_gatt_server_attribute_value_id:
        printf("evt:gecko_evt_gatt_server_attribute_value_id\r\n");
		printf("Write request for att: %d and value: %s\r\n",
				evt->data.evt_gatt_server_attribute_value.attribute,
				evt->data.evt_gatt_server_attribute_value.value.data);
        if (evt->data.evt_gatt_server_attribute_value.attribute == gattdb_led_config) {
			printf("LED config change request received\r\n");

			/* Check for valid configuration */
			if(evt->data.evt_gatt_server_attribute_value.value.data[6] == 0x01)
			{
				Current_LED_Configuration.Is_LED_Configuration_Set = 0xAA;
				printf("Valid LED channel configuration received\r\n");
			}
			else
			{
				printf("Invalid LED channel configuration received\r\n");
				break;
			}

			/* store the LED configuration data in Flash memory */
			memcpy(Current_LED_Configuration.LED_Configuration_Data,
					evt->data.evt_gatt_server_attribute_value.value.data,
					sizeof(Current_LED_Configuration.LED_Configuration_Data));

			/* Check for the NC(Not Connected) channels and update
			 * the intensity to zero for all NC and save in Flash storage.
			 */
			for(loop=0; loop < LED_CHANNELS; loop++)
			{
				if(Current_LED_Configuration.LED_Configuration_Data[loop] == LED_CHANNEL_NC)
				{
					/* setting LED intensity value to 0 for NC channels in flash */
					Current_LED_Configuration.LED_Channel_intensity[loop] = 0;
				}
			}

			status = writeLedStateFlash(&Current_LED_Configuration);
			if (status != 0) {
				printf("Fail to write LED configuration in flash memory\r\n");
				printf("Retry the Flash write\r\n");
				status = writeLedStateFlash(&Current_LED_Configuration);
				if (status != 0)
				{
					printf("Flash memory write fail again.\r\n");
				}
			}

			/* send the LED configuration mapping to zigbee module over UART */
			sendLedConfigZigbee();
        }

    break;
    case gecko_cmd_gatt_server_read_attribute_value_id:
        printf("evt:gecko_cmd_gatt_server_read_attribute_value_id\r\n");
    break;
    default:
      //printf("unhandled evt: %8.8x class %2.2x method %2.2x\r\n", evt_id, (evt_id >> 16) & 0xFF, (evt_id >> 24) & 0xFF);
      break;
  }
}
