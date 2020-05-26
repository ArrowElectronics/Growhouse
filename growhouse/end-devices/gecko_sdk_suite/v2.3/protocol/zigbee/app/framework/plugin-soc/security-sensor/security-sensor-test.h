// Copyright 2017 Silicon Laboratories, Inc.

#ifndef SILABS_SECURITY_SENSOR_TEST_H
#define SILABS_SECURITY_SENSOR_TEST_H

#define EMBER_AF_API_LED_BLINK "util/plugin/plugin-common/led-blink/led-blink.h"
#define EMBER_AF_API_GPIO_SENSOR "util/plugin/plugin-common/gpio-sensor/gpio-sensor.h"

// Macros to allow plugin options to be defined in unit tests
#define EMBER_AF_PLUGIN_SECURITY_SENSOR_TIME_BETWEEN_PRESSES_MS 500
#define EMBER_AF_PLUGIN_SECURITY_SENSOR_BUTTON_DEBOUNCE_TIME_MS 100
#define EMBER_AF_PLUGIN_SECURITY_SENSOR_NETWORK_LEAVE_TIME_MS   1000
#define EMBER_AF_PLUGIN_SECURITY_SENSOR_IAS_ZONE_ENDPOINT       1

#endif
