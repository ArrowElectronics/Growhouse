/***************************************************************************//**
 * @file
 * @brief Application code
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

/* C Standard Library headers */
#include <stdio.h>
#include <math.h>

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lib.h"

/* GPIO peripheral library */
#include <em_gpio.h>

/* Lightbulb with mesh models */
#include "lightbulb.h"

/* Display Interface header */
#include "display_interface.h"

/* LED driver with support for PWM dimming */
#include "led_driver.h"

/* Own header */
#include "app.h"

/* GPIO interrupt handler header */
#include "gpiointerrupt.h"

/* Zigbee UART comm header */
#include "ZIGBEE_Usart_Comm.h"

/* digital potentionmeter interface header */
#include "ad5206.h"

/* spi flash */
#include "mx25flash_spi.h"

/* watchdog support */
#include "em_wdog.h"

/* bootloader header */
#include "api/btl_interface.h"
/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

/*******************************************************************************
 * Global Variable and externs.
 ******************************************************************************/
extern LED_Configuration Current_LED_Configuration;
extern LED_Configuration Default_LED_Configuration;
LED_Configuration Temp_LED_Configuration;
extern uint8_t zigbeeAvailable;

/* Version specific Structure */
extern const ApplicationProperties_t bg_application_properties;

/*******************************************************************************
 * Timer handles defines.
 ******************************************************************************/
#define TIMER_ID_LED_FLASHING    81
#define TIMER_ID_PROVISION_COMPLETE    80
#define TIMER_ID_WATCHDOG    79
#define TIMER_ID_RESTART    78
#define TIMER_ID_FACTORY_RESET  77
#define TIMER_ID_PROVISIONING   66

/// Flag for indicating DFU Reset must be performed
static uint8_t boot_to_dfu = 0;
/// Address of the Primary Element of the Node
static uint16 _my_address = 0;
/// Number of active Bluetooth connections
static uint8 num_connections = 0;
/// Handle of the last opened LE connection
static uint8 conn_handle = 0xFF;
/// Flag for indicating that initialization was performed
static uint8 init_done = 0;

uint8_t provision_flag = 0;
#define PROVISION_SUCCESS_FLAG 0x55
#define LED_FLASHING_COUNT 3
#define BLINK_PERIOD_DELAY 1000 //delay in msec
#define FLASHING_ON 1
#define FLASHING_OFF 0
#define DEVICE_PROVISIONING_TIME_LIMIT 40 // time in seconds
/*******************************************************************************
 * Function Declaration.
 ******************************************************************************/
int writeLedStateFlash(LED_Configuration *LED_Configuration_state);
int readLedStateFlash(LED_Configuration *LED_Configuration_state);
int writeLedStateFlash(LED_Configuration *LED_Configuration_state);
void default_led_configuration_flash(void);
void gpio_irq_handler(uint32 signal);
extern bool WaitFlashReady(uint32_t ExpectTime);
int LED_Flashing(void);
/***************************************************************************//**
 * This function is called to initiate factory reset. Factory reset may be
 * initiated by keeping one of the WSTK pushbuttons pressed during reboot.
 * Factory reset is also performed if it is requested by the provisioner
 * (event gecko_evt_mesh_node_reset_id).
 ******************************************************************************/
static void initiate_factory_reset(void)
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

/***************************************************************************//**
 * Set device name in the GATT database. A unique name is generated using
 * the two last bytes from the Bluetooth address of this device. Name is also
 * displayed on the LCD.
 *
 * @param[in] pAddr  Pointer to Bluetooth address.
 ******************************************************************************/
static void set_device_name(bd_addr *pAddr)
{
  char name[20];
  char led_config[6] = {0x50, 0x51, 0x50, 0x51, 0x50, 0x51};
  uint16 res;

  // create unique device name using the last two bytes of the Bluetooth address
  sprintf(name, "light node %02x:%02x", pAddr->addr[1], pAddr->addr[0]);

  printf("Device name: '%s'\r\n", name);

  // write device name to the GATT database
  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_led_config, 0, 6, (uint8 *)led_config)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

#ifdef DISPLAY_EVAL_KIT
  // show device name on the LCD
  DI_Print(name, DI_ROW_NAME);
#endif //DISPLAY_EVAL_KIT
}

/***************************************************************************//**
 * This function prints debug information for mesh server state change event.
 *
 * @param[in] pEvt  Pointer to mesh_lib_generic_server_state_changed event.
 ******************************************************************************/
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

/*******************************************************************************
 * Initialise used bgapi classes.
 ******************************************************************************/
void gecko_bgapi_classes_init(void)
{
  gecko_bgapi_class_dfu_init();
  gecko_bgapi_class_system_init();
  gecko_bgapi_class_le_gap_init();
  gecko_bgapi_class_le_connection_init();
  //gecko_bgapi_class_gatt_init();
  gecko_bgapi_class_gatt_server_init();
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
}

/*******************************************************************************
 * Handling of stack events. Both Bluetooth LE and Bluetooth mesh events
 * are handled here.
 * @param[in] evt_id  Incoming event ID.
 * @param[in] evt     Pointer to incoming event.
 ******************************************************************************/
void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
  uint16_t result;
  char buf[30];
  int status = -1;
  uint8_t loop = 0;
  uint8_t reset_flag = 0x00;

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

        // Initialize Mesh stack in Node operation mode, it will generate initialized event
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

      /* set the LED Node firmware version string in characteristics attribute */
      set_firmware_version_gatt_attribute();

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
          // reset the device to finish factory reset
          gecko_cmd_system_reset(0);
          break;

        case TIMER_ID_RESTART:
          // restart timer expires, reset the device
          gecko_cmd_system_reset(0);
          break;

        case TIMER_ID_PROVISIONING:
          // toggle LED to indicate the provisioning state
          if (!init_done) {
            LEDS_SetState(LED_STATE_PROV);
          }
          break;

        case TIMER_ID_PROVISION_COMPLETE:
          // Device provision status check timer handler. If the device provisioning is initiated and not complete
          // within specific time then factory reset the node to avoid the situation of factory reset the node by
          // physical hardware reset.

          if (provision_flag != PROVISION_SUCCESS_FLAG) {
            printf("Device provisioning is not completed within defined interval.\r\n");
            printf("Factory reseting the node.\r\n");
            initiate_factory_reset();
          }
          break;

        case TIMER_ID_LED_FLASHING:
          // Flash all the LED channels for defined count to indicate the selected LED Node
          LED_Flashing();
          break;

        default:
          // lightbulb related timer events are handled by separate function
          handle_lightbulb_timer_evt(evt);
          break;
      }
      break;

    case gecko_evt_mesh_node_initialized_id:
      printf("node initialized\r\n");

      // Initialize generic server models
      result = gecko_cmd_mesh_generic_server_init()->result;
      if (result) {
        printf("mesh_generic_server_init failed, code 0x%x\r\n", result);
      }

      struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

      if (pData->provisioned) {
        printf("node is provisioned. address:%x, ivi:%ld\r\n", pData->address, pData->ivi);

        _my_address = pData->address;
        lightbulb_state_init();
        init_done = 1;
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

      // Reset the provision successful flag
      provision_flag = 0;

      // Start timer for uncompleted provision of LED Node
      gecko_cmd_hardware_set_soft_timer(32768 * DEVICE_PROVISIONING_TIME_LIMIT, TIMER_ID_PROVISION_COMPLETE, 1);
      break;

    case gecko_evt_mesh_node_provisioned_id:
      lightbulb_state_init();
      init_done = 1;
      printf("node provisioned, got address=%x\r\n", evt->data.evt_mesh_node_provisioned.address);
      // stop LED blinking when provisioning complete

      /* update the provision state of BLE in flash */
      Current_LED_Configuration.Is_BLE_Provisioned = 0x01;
      writeLedStateFlash(&Current_LED_Configuration);

      /* send BLE module provision state to zigbee module over UART */
      sendBLEProvisionState();

      gecko_cmd_hardware_set_soft_timer(0, TIMER_ID_PROVISIONING, 0);
#ifdef DISPLAY_EVAL_KIT
      DI_Print("provisioned", DI_ROW_STATUS);
#endif //DISPLAY_EVAL_KIT
      break;

    case gecko_evt_mesh_node_provisioning_failed_id:
      printf("provisioning failed, code %x\r\n", evt->data.evt_mesh_node_provisioning_failed.result);
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
      // pass the server client request event to mesh lib handler that will invoke
      // the callback functions registered by application
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
      int16_t current_delta;
      char tmp[21];

      if (init_done && (evt->data.evt_system_external_signal.extsignals & EXT_SIGNAL_LED_LEVEL_CHANGED)) {
        /* this signal from the LED PWM driver indicates that the level has changed,
         * we use it here to update the LCD status */
        current_level = LEDS_GetLevel();
        sprintf(tmp, "Lightness: %5u%%", (current_level * 100 + 99) / 65535);
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

        current_delta = LEDS_GetDeltaUV();
        sprintf(tmp, "Delta UV: %6d ", current_delta);
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

				// Set the successful provision status flag
				provision_flag = PROVISION_SUCCESS_FLAG;
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
        else if (evt->data.evt_gatt_server_attribute_value.attribute == gattdb_node_reset) {
			printf("Node Reset request received\r\n");
			printf("Data received : 0x%x\r\n", evt->data.evt_gatt_server_attribute_value.value.data[0]);
			if(evt->data.evt_gatt_server_attribute_value.value.data[0] == 0x01)
			{
				printf("Factory Reseting the LED Node\r\n");

				/* Send Notify to central device */
				gecko_cmd_gatt_server_send_characteristic_notification(0xFF,
						gattdb_node_reset, 1, (uint8 *) &reset_flag);

				initiate_factory_reset();
			}
        }
        else if (evt->data.evt_gatt_server_attribute_value.attribute == gattdb_led_flashing) {
			printf("LED Node Flashing request received\r\n");
			printf("Data received : 0x%x\r\n", evt->data.evt_gatt_server_attribute_value.value.data[0]);
			if(evt->data.evt_gatt_server_attribute_value.value.data[0] == 0x01)
			{
				printf("LED Node Flashing for %d seconds\r\n", LED_FLASHING_COUNT);

				/* Start LED Flashing timer */
				gecko_cmd_hardware_set_soft_timer(32768 * ((float) (BLINK_PERIOD_DELAY/1000)), TIMER_ID_LED_FLASHING, 1);
			}
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

/*******************************************************************************
 * Extract the requested LED channel number based on transition time parameter
 * are handled here.
 * @param[in] transition_ms  transition delay in ms .
 * @param[out] uint32_t      LED channel number.
 ******************************************************************************/
uint32_t extract_led_channel(uint32_t transition_ms)
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

/*******************************************************************************
 * GPIO interrupt handler callback function
 * @param[in] gpioPinNo      GPIO pin number.
 ******************************************************************************/
void gpio_interrupt_handler_cb(uint8_t gpioPinNo)
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

/*******************************************************************************
 * Set the led channel configuration read from the Flash and write
 * the configuration in LED configuration characteristic attributes.
 ******************************************************************************/
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

/*******************************************************************************
 * Set the led channel intensity read from the Flash and write
 * the configuration in LED intensity characteristic attributes.
 ******************************************************************************/
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

/*******************************************************************************
 * Set the led channel intensity read from the Flash and write
 * the configuration in LED intensity characteristic attributes.
 ******************************************************************************/
void set_firmware_version_gatt_attribute(void)
{
  uint32_t btl_version = 0;
  char version_string[36] = {0};
  uint16 res;

  btl_version = mainBootloaderTable->header.version & 0x000F;

  snprintf(version_string, 36, "btl_version: %d, App_version: %d", (int)btl_version, (int)bg_application_properties.app.version);

  printf("version string: %s, len:%u\r\n", version_string, strlen(version_string));

  res = gecko_cmd_gatt_server_write_attribute_value(gattdb_fw_version, 0, strlen(version_string), (uint8 *)version_string)->result;
  if (res) {
    printf("gecko_cmd_gatt_server_write_attribute_value() failed, code %x\r\n", res);
  }

  /* Send Notify to central device */
  gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_fw_version, 32, (uint8 *)version_string);
}

/*******************************************************************************
 * this function is for LED channel light intensity update request
 * to ZIGBEE module over USART.
 ******************************************************************************/
void lightness_request_usart(uint16_t led_intensity, uint16_t led_channel_num)
{
	ledPayload ledRequestData;

	memset(&ledRequestData, 0x00, sizeof(ledPayload));

	ledRequestData.requestType = CONTROL;
	ledRequestData.ledNumber = led_channel_num;

	/* Assign the requested LED channel intensity value */
	ledRequestData.ledLevel[led_channel_num-1] = (int) ceil((float)led_intensity * PERCENTAGE_FACTOR);

	/* send the light intensity update request to zigbee module over
	 * usart interface
	 */
	ZigbeeUsartSend(&ledRequestData);

}

/*******************************************************************************
 * This function is for LED channel light intensity update request
 * to ZIGBEE module over USART to set all channel to same intensity level.
 ******************************************************************************/
void lightness_request_all_channel_same_intensity_usart(uint16_t led_intensity)
{
	ledPayload ledRequestData;

	memset(&ledRequestData, 0x00, sizeof(ledPayload));

	ledRequestData.requestType = CONTROL;
	ledRequestData.ledNumber = LED_ALL;

	/* Assign the requested LED channel intensity value */
	ledRequestData.ledLevel[0] = (int) ceil((float)led_intensity * PERCENTAGE_FACTOR);
	ledRequestData.ledLevel[1] = ledRequestData.ledLevel[0];
	ledRequestData.ledLevel[2] = ledRequestData.ledLevel[0];
	ledRequestData.ledLevel[3] = ledRequestData.ledLevel[0];
	ledRequestData.ledLevel[4] = ledRequestData.ledLevel[0];
	ledRequestData.ledLevel[5] = ledRequestData.ledLevel[0];

	/* send the light intensity update request to zigbee module over
	 * usart interface
	 */
	ZigbeeUsartSend(&ledRequestData);

}

/*******************************************************************************
 * This function is for LED channel default light intensity request
 * to ZIGBEE module over USART.
 ******************************************************************************/
void default_lightness_request_usart()
{
	ledPayload ledRequestData;

	memset(&ledRequestData, 0x00, sizeof(ledPayload));

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

/*******************************************************************************
 * This function is for LED configuration and current LED channel state write
 * to external flash memory.
 ******************************************************************************/
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

/*******************************************************************************
 * This function is for LED configuration and current LED channel state write
 * to external flash memory.
 ******************************************************************************/
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

/*******************************************************************************
 * This function is for LED intensity change based on the zigbee module
 * availability. if the zigbee module is not present then set led
 * intensity through digital potentiometer directly.
 ******************************************************************************/
int LEDS_SetLevel_Dpot(uint16_t led_intensity, uint16_t led_channel_num)
{
	printf("LED Num: %d and intensity: %d\r\n", led_channel_num, led_intensity);

	/* Any intensity request below 10 is treated as 0% intensity request */
    /* This is the workaround for the issue when BLE mesh application is sending
       the same intensity level by incrementing the 1 level. This workaround
       will help the case when all the channel level is set to zero.
       TODO: Need to handle this from BLE mesh application if possible on leter.
    */

	if(led_intensity < 10)
	{
		led_intensity = 0;
		printf("LED Num: %d and intensity: %d\r\n", led_channel_num, led_intensity);
	}
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

/*******************************************************************************
 * This function is for LED Flashing when requested for all the channel.
 * If the zigbee module is not present then set led
 * intensity through digital potentiometer directly.
 ******************************************************************************/
int LED_Flashing(void)
{
	static uint8_t flashing_cnt = 0;
	static uint8_t flashing_last_state = 0;
	uint8_t loop = 0;

	if(flashing_last_state != FLASHING_ON)
	{
	    printf("Setting all channel led intensity to 100 percentage\r\n");
	    if(zigbeeAvailable)
	    {
	        lightness_request_all_channel_same_intensity_usart(MAX_LED_LEVEL);
	    }
	    else
	    {
	        for(loop=0; loop < LED_CHANNELS; loop++)
	        {
	            /* control the LED channel intensity directly using digital
	             * potentiometer as the zigbee module is not available. */
	            halAD5204DriverSetLedLevel(loop+1, MAX_LED_LEVEL);
	        }
	    }
	    flashing_last_state = FLASHING_ON;
	}
	else
	{
	    printf("Setting all channel led intensity to 0 percentage\r\n");
	    if(zigbeeAvailable)
	    {
	        lightness_request_all_channel_same_intensity_usart(0);
	    }
	    else
	    {
	        for(loop=0; loop < LED_CHANNELS; loop++)
	        {
	            /* control the LED channel intensity directly using digital
	             * potentiometer as the zigbee module is not available. */
	            halAD5204DriverSetLedLevel(loop+1, 0);
	        }
	    }
	    flashing_cnt++;
	    flashing_last_state = FLASHING_OFF;
	}
	if(flashing_cnt < LED_FLASHING_COUNT)
	{
		gecko_cmd_hardware_set_soft_timer(32768 * ((float) (BLINK_PERIOD_DELAY/1000)), TIMER_ID_LED_FLASHING, 1);
		printf("Timer Started\r\n");
	}
	else
	{
		flashing_cnt = 0;
		flashing_last_state = FLASHING_OFF;
		printf("LED Flashing completed\r\n");
	}
    return 0;
}

/*******************************************************************************
 * Initialize the Flash with default LED configuration.
 ******************************************************************************/
void default_led_configuration_flash(void)
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

/*******************************************************************************
 * reset LED configuration in external flash memory.
 ******************************************************************************/
void reset_led_configuration_flash(void)
{
	int status = -1;
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

/*******************************************************************************
 * Handling gpio event signal from BLE signal handler.
 ******************************************************************************/
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
			// short press
			printf("Factory Reset button short pressed\r\n");
		}
		break;

	default:
		break;
	}
}


/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */
