/*
 * ph-moisture-measurement-server.c
 *
 *  Created on: 15-Apr-2019
 *      Author: einfochips
 */

#include "app/framework/include/af.h"
#include "hal/plugin/soilnode-stub/CNO398.h"
#ifdef EMBER_AF_PLUGIN_REPORTING
#include "app/framework/plugin/reporting/reporting.h"
#endif

//------------------------------------------------------------------------------
// Plugin private macros

// Shorter macros for plugin options
#define MAX_PH_MEASUREMENT_INTERVAL_MS                               \
  (EMBER_AF_PLUGIN_PH_MOISTURE_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S \
   * MILLISECOND_TICKS_PER_SECOND)

//------------------------------------------------------------------------------
// Forward Declaration of private functions
static void writePhMoistureAttributes(int32_t);

//------------------------------------------------------------------------------
// Global variables
EmberEventControl emberAfPluginPhMoistureMeasurementServerReadEventControl;
static uint32_t pHMoistureMeasurementIntervalMs =
  MAX_PH_MEASUREMENT_INTERVAL_MS;

//******************************************************************************
// Plugin init function
//******************************************************************************
void emberAfPluginPhMoistureMeasurementServerInitCallback(void)
{
  // Start the ReadEvent, which will re-activate itself perpetually
  emberEventControlSetActive(
		  emberAfPluginPhMoistureMeasurementServerReadEventControl);
}

void emberAfPluginPhMoistureMeasurementServerStackStatusCallback(
  EmberStatus status)
{
  // On network connect, chances are very good that someone (coordinator,
  // gateway, etc) will poll the pH for an initial status.  As such,
  // it is useful to have fresh data to be polled.
  if (status == EMBER_NETWORK_UP) {
    emberEventControlSetActive(
      emberAfPluginPhMoistureMeasurementServerReadEventControl);
  }
}

//------------------------------------------------------------------------------
// Plugin event handlers

//******************************************************************************
// Event used to generate a read of a new pH and Moisture value
//******************************************************************************
void emberAfPluginPhMoistureMeasurementServerReadEventHandler(void)
{
	emberAfCorePrintln("Read Event triggered");
	halPhMoistureStartRead();
}

//******************************************************************************
// Update the pH attribute of the pH measurement cluster to
// be the pH value given by the function's parameter.
//******************************************************************************
void halPhMoistureReadingCompleteCallback(int16_t pH, int16_t moisture, bool readSuccess)
{
  int32_t pH_moisture = 0;
  int32_t tmp_pH = pH;
  int32_t tmp_moisture = moisture;

  // If the read was successful, post the results to the cluster
  if (readSuccess) {
    pH_moisture = (int32_t) (((tmp_pH << 16) & 0xFFFF0000) |
                              (tmp_moisture & 0x0000ffff));

	writePhMoistureAttributes(pH_moisture);
  } else {
	  emberAfPhMoistureMeasurementClusterPrintln("Error reading pH and Moisture value from HW");
  }
  emberAfCorePrintln("Setting next read event");
  emberEventControlSetDelayMS(
    emberAfPluginPhMoistureMeasurementServerReadEventControl,
    pHMoistureMeasurementIntervalMs);
}

static void writePhMoistureAttributes(int32_t pH_moisture)
{
  uint8_t i;
  uint8_t endpoint;

  // Cycle through all end points, check to see if the end point has a pH and moisture
  // server, and if so update the pH and moisture attributes of that end point
  for (i = 0; i < emberAfEndpointCount(); i++) {
    endpoint = emberAfEndpointFromIndex(i);
    if (emberAfContainsServer(endpoint, ZCL_PH_MOISTURE_MEASUREMENT_CLUSTER_ID)) {
      // Write the current pH and moisture attribute
      emberAfCorePrintln("");
      emberAfWriteServerAttribute(endpoint,
                                  ZCL_PH_MOISTURE_MEASUREMENT_CLUSTER_ID,
                                  ZCL_PH_MOISTURE_MEASURED_VALUE_ATTRIBUTE_ID,
                                  (uint8_t *) &pH_moisture,
                                  ZCL_INT32U_ATTRIBUTE_TYPE);
    }
  }
}
