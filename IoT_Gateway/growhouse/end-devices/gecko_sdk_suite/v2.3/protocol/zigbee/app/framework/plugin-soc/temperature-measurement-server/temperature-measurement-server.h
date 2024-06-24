// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#ifndef SILABS_TEMPERATURE_MEASUREMENT_SERVER_H
#define SILABS_TEMPERATURE_MEASUREMENT_SERVER_H

#define EMBER_TEMPERATURE_MEASUREMENT_SERVER_OVER_TEMPERATURE_NORMAL      0
#define EMBER_TEMPERATURE_MEASUREMENT_SERVER_OVER_TEMPERATURE_WARNING     1
#define EMBER_TEMPERATURE_MEASUREMENT_SERVER_OVER_TEMPERATURE_CRITICAL    2
//------------------------------------------------------------------------------
// Plugin public function declarations.

/** @brief Sets the hardware read interval.
 *
 * Sets the amount of time to wait (in seconds) between polls
 * of the temperature sensor. This function will never set the measurement
 * interval to be greater than the plugin specified maximum measurement
 * interval. If a value of 0 is given, the plugin specified maximum measurement
 * interval will be used for the polling interval.
 */
void emberAfPluginTemperatureMeasurementServerSetMeasurementInterval(
  uint32_t measurementRateS);

#endif //__TEMPERATURE_MEASUREMENT_SERVER_H__
