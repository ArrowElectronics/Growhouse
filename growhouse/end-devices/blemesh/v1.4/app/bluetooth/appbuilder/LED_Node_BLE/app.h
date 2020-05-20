/***************************************************************************//**
 * @file
 * @brief Application header file
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

#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************//**
 * \defgroup app Application Code
 * \brief Sample Application Implementation
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

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

/*** Button external signals ***/
#define EXT_SIGNAL_FACTORY_BUTTON_PRESS_SIG          (1 << 3)
#define EXT_SIGNAL_FACTORY_BUTTON_RELEASE_SIG        (1 << 4)
#define FACTORY_BUTTON_LONG_PRESS_DURATION (80000) /* ~2.5 seconds */

/***************************************************************************//**
 * Initialise used bgapi classes.
 ******************************************************************************/
void gecko_bgapi_classes_init (void);

/***************************************************************************//**
 * Handling of stack events. Both Bluetooth LE and Bluetooth mesh events
 * are handled here.
 * @param[in] evt_id  Incoming event ID.
 * @param[in] evt     Pointer to incoming event.
 ******************************************************************************/
void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
void lightness_request_usart(uint16_t led_intensity, uint16_t led_channel_num);
void default_lightness_request_usart();
int LEDS_SetLevel_Dpot(uint16_t led_intensity, uint16_t led_channel_num);
void gpio_interrupt_handler_cb(uint8_t gpioPinNo);
void set_led_config_gatt_attribute(void);
void set_led_intensity_gatt_attribute(void);
void set_firmware_version_gatt_attribute(void);
void reset_led_configuration_flash(void);
uint32_t extract_led_channel(uint32_t transition_ms);

/** @} (end addtogroup app) */
/** @} (end addtogroup Application) */

#ifdef __cplusplus
};
#endif

#endif /* APP_H */
