// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#ifndef SILABS_ILLUMINANCE_MEASUREMENT_SERVER_H
#define SILABS_ILLUMINANCE_MEASUREMENT_SERVER_H

//------------------------------------------------------------------------------
// Plugin public function declarations

/** @brief Sets the hardware read interval.
 *
 * This function sets the amount of time to wait (in seconds) between polls
 * of the illuminance sensor. This function will never set the measurement
 * interval to be greater than the plugin-specified maximum measurement
 * interval. If a value of 0 is given, the plugin-specified maximum measurement
 * interval will be used for the polling interval.
 */
void emberAfPluginIlluminanceMeasurementServerSetMeasurementInterval(
  uint32_t measurementIntervalS);

#endif //__ILLUMINANCE_MEASUREMENT_SERVER_H__
