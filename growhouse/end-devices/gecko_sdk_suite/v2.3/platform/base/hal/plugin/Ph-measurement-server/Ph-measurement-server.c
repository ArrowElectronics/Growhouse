/*
 * ph-measurement-server.c
 *
 *  Created on: 17-Sep-2018
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
  (EMBER_AF_PLUGIN_PH_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S \
   * MILLISECOND_TICKS_PER_SECOND)

//------------------------------------------------------------------------------
// Forward Declaration of private functions
static void writePhAttributes(int32_t phDevidedByHundred);
static void checkForReportingConfig(void);
void emberAfPluginPhMeasurementServerSetMeasurementInterval(uint16_t measurementIntervalS);
//------------------------------------------------------------------------------
// Global variables
EmberEventControl emberAfPluginPhMeasurementServerReadEventControl;
static uint32_t pHMeasurementIntervalMs =
  MAX_PH_MEASUREMENT_INTERVAL_MS;

// The default configuration to be used if no reporting has been set up
#ifdef EMBER_AF_PLUGIN_REPORTING
static EmberAfPluginReportingEntry defaultConfiguration = {
  EMBER_ZCL_REPORTING_DIRECTION_REPORTED, //direction
  0, //endpoint, which will be set on a per-use basis
  ZCL_PH_MEASUREMENT_CLUSTER_ID, //clusterId
  ZCL_PH_MEASURED_VALUE_ATTRIBUTE_ID, //attributeId
  CLUSTER_MASK_SERVER, //mask
  EMBER_AF_NULL_MANUFACTURER_CODE, //manufacturerCode
  .data.reported = {
	0, //minInterval
	EMBER_AF_PLUGIN_PH_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S, //maxInterval
	EMBER_AF_PLUGIN_PH_MEASUREMENT_SERVER_DEFAULT_REPORTABLE_PH_CHANGE //reportableChange
  }
};
#endif

//------------------------------------------------------------------------------
// Plugin consumed callback implementations

static void checkForReportingConfig(void)
{
#ifdef EMBER_AF_PLUGIN_REPORTING
  uint8_t i;
  EmberAfPluginReportingEntry entry;
  uint8_t endpoint;
  bool existingEntry = false;

  // Cycle through the reporting table to determine if an entry has been created
  // for the device before.  If so, update the sensor's hardware polling rate to
  // match the attribute defined maxInterval.  Otherwise, the plugin will use
  // the plugin's option defined default hardware polling interval.
  for (i = 0; i < REPORT_TABLE_SIZE; i++) {
    emAfPluginReportingGetEntry(i, &entry);
    if ((entry.clusterId == ZCL_PH_MEASUREMENT_CLUSTER_ID)
        && (entry.attributeId == ZCL_PH_MEASURED_VALUE_ATTRIBUTE_ID)
        && (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED)
        && (entry.endpoint != EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID)
        && (entry.manufacturerCode == EMBER_AF_NULL_MANUFACTURER_CODE)) {
      // Max interval is set in seconds, which is the same unit of time the
      // emberAfPluginPhMeasurementServerSetMeasurementInterval
      // expects in this API.
      emberAfPluginPhMeasurementServerSetMeasurementInterval(
        entry.data.reported.maxInterval);
      existingEntry = true;
    }
  }

  // If no entry is found for the pH measurement server, a default
  // reporting configuration should be created using the plugin defined options.
  // This needs to be done for all endpoints that support a pH
  // measurement server.
  if (!existingEntry) {
    for (i = 0; i < emberAfEndpointCount(); i++) {
      endpoint = emberAfEndpointFromIndex(i);
      defaultConfiguration.endpoint = endpoint;
      if (emberAfContainsServer(endpoint, ZCL_PH_MEASUREMENT_CLUSTER_ID)) {
        emAfPluginReportingAddEntry(&defaultConfiguration);
      }
    }
  }
#endif
}

//******************************************************************************
// Plugin init function
//******************************************************************************
void emberAfPluginPhMeasurementServerInitCallback(void)
{
  checkForReportingConfig();

  // Start the ReadEvent, which will re-activate itself perpetually
  emberEventControlSetActive(
		  emberAfPluginPhMeasurementServerReadEventControl);
}

void emberAfPluginPhMeasurementServerStackStatusCallback(
  EmberStatus status)
{
  // On network connect, chances are very good that someone (coordinator,
  // gateway, etc) will poll the pH for an initial status.  As such,
  // it is useful to have fresh data to be polled.
  if (status == EMBER_NETWORK_UP) {
    checkForReportingConfig();
    emberEventControlSetActive(
      emberAfPluginPhMeasurementServerReadEventControl);
  }
}

//------------------------------------------------------------------------------
// Plugin event handlers

//******************************************************************************
// Event used to generate a read of a new pH value
//******************************************************************************
void emberAfPluginPhMeasurementServerReadEventHandler(void)
{
  halPhStartRead();
}

void halPhReadingCompleteCallback(int pH, bool readSuccess)
{
  // If the read was successful, post the results to the cluster
  if (readSuccess) {
	emberAfPhMeasurementClusterPrintln("Ph: %d", pH);
    writePhAttributes(pH);
  } else {
	  emberAfPhMeasurementClusterPrintln("Error reading pH from HW");
  }

  emberEventControlSetDelayMS(
    emberAfPluginPhMeasurementServerReadEventControl,
    pHMeasurementIntervalMs);
}

//------------------------------------------------------------------------------
// Plugin public functions

void emberAfPluginPhMeasurementServerSetMeasurementInterval(
  uint16_t measurementIntervalS)
{
  if ((measurementIntervalS == 0)
      || (measurementIntervalS
          > MAX_PH_MEASUREMENT_INTERVAL_MS)) {
    pHMeasurementIntervalMs = MAX_PH_MEASUREMENT_INTERVAL_MS;
  } else {
    pHMeasurementIntervalMs
      = measurementIntervalS * MILLISECOND_TICKS_PER_SECOND;
  }
  emberEventControlSetDelayMS(
    emberAfPluginPhMeasurementServerReadEventControl,
    pHMeasurementIntervalMs);
}

//------------------------------------------------------------------------------
// Plugin private functions

//******************************************************************************
// Update the pH attribute of the pH measurement cluster to
// be the pH value given by the function's parameter.  This function
// will also query the current max and min read values, and update them if the
// given values is higher (or lower) than the previous records.
//******************************************************************************
static void writePhAttributes(int32_t pH)
{
  uint8_t i;
  uint8_t endpoint;

  // Cycle through all endpoints, check to see if the endpoint has a pH
  // server, and if so update the pH attributes of that endpoint
  for (i = 0; i < emberAfEndpointCount(); i++) {
    endpoint = emberAfEndpointFromIndex(i);
    if (emberAfContainsServer(endpoint, ZCL_PH_MEASUREMENT_CLUSTER_ID)) {
      // Write the current pH attribute
      emberAfCorePrintln("");
      emberAfWriteServerAttribute(endpoint,
                                  ZCL_PH_MEASUREMENT_CLUSTER_ID,
                                  ZCL_PH_MEASURED_VALUE_ATTRIBUTE_ID,
                                  (uint8_t *) &pH,
                                  ZCL_INT16U_ATTRIBUTE_TYPE);
    }
  }
}
