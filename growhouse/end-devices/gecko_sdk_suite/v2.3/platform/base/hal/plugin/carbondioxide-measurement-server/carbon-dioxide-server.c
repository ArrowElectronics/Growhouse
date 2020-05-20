/*
 * carbondioxide-measurement-server.c
 *
 *  Created on: 17-Sep-2018
 *      Author: einfochips
 */

#include "app/framework/include/af.h"

#ifdef EMBER_AF_PLUGIN_REPORTING
#include "app/framework/plugin/reporting/reporting.h"
#endif

//------------------------------------------------------------------------------
// Plugin private macros

// Shorter macros for plugin options
#define MAX_CARBONDIOXIDE_MEASUREMENT_INTERVAL_MS                               \
  (EMBER_AF_PLUGIN_CARBONDIOXIDE_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S \
   * MILLISECOND_TICKS_PER_SECOND)

//------------------------------------------------------------------------------
// Forward Declaration of private functions
static void writeCarbonDioxideAttributes(int32_t carbondioxideDevidedByHundred);
static void checkForReportingConfig(void);

//------------------------------------------------------------------------------
// Global variables
EmberEventControl emberAfPluginCarbondioxideMeasurementServerReadEventControl;
static uint32_t carbondioxideMeasurementIntervalMs =
  MAX_CARBONDIOXIDE_MEASUREMENT_INTERVAL_MS;

// The default configuration to be used if no reporting has been set up
#ifdef EMBER_AF_PLUGIN_REPORTING
static EmberAfPluginReportingEntry defaultConfiguration = {
  EMBER_ZCL_REPORTING_DIRECTION_REPORTED, //direction
  0, //endpoint, which will be set on a per-use basis
  ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID, //clusterId
  ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_ID, //attributeId
  CLUSTER_MASK_SERVER, //mask
  EMBER_AF_NULL_MANUFACTURER_CODE, //manufacturerCode
  .data.reported = {
    0, //minInterval
	EMBER_AF_PLUGIN_CARBONDIOXIDE_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S, //maxInterval
	EMBER_AF_PLUGIN_CARBONDIOXIDE_MEASUREMENT_SERVER_DEFAULT_REPORTABLE_CARBONDIOXIDE_CHANGE //reportableChange
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
    if ((entry.clusterId == ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID)
        && (entry.attributeId == ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_ID)
        && (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED)
        && (entry.endpoint != EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID)
        && (entry.manufacturerCode == EMBER_AF_NULL_MANUFACTURER_CODE)) {
      // Max interval is set in seconds, which is the same unit of time the
      // emberAfPluginCarbonDioxideMeasurementServerSetMeasurementInterval
      // expects in this API.
      emberAfPluginCarbonDioxideMeasurementServerSetMeasurementInterval(
        entry.data.reported.maxInterval);
      existingEntry = true;
    }
  }

  // If no entry is found for the temperature measurement server, a default
  // reporting configuration should be created using the plugin defined options.
  // This needs to be done for all endpoints that support a temperature
  // measurement server.
  if (!existingEntry) {
    for (i = 0; i < emberAfEndpointCount(); i++) {
      endpoint = emberAfEndpointFromIndex(i);
      defaultConfiguration.endpoint = endpoint;
      if (emberAfContainsServer(endpoint, ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID)) {
        emAfPluginReportingAddEntry(&defaultConfiguration);
      }
    }
  }
#endif
}

//******************************************************************************
// Plugin init function
//******************************************************************************
void emberAfPluginCarbondioxideMeasurementServerInitCallback(void)
{
  checkForReportingConfig();

  // Start the ReadEvent, which will re-activate itself perpetually
  emberEventControlSetActive(
		  emberAfPluginCarbondioxideMeasurementServerReadEventControl);
}

void emberAfPluginCarbondioxideMeasurementServerStackStatusCallback(
  EmberStatus status)
{
  // On network connect, chances are very good that someone (coordinator,
  // gateway, etc) will poll the temperature for an initial status.  As such,
  // it is useful to have fresh data to be polled.
  if (status == EMBER_NETWORK_UP) {
    checkForReportingConfig();
    emberEventControlSetActive(
    		emberAfPluginCarbondioxideMeasurementServerReadEventControl);
  }
}

//------------------------------------------------------------------------------
// Plugin event handlers

//******************************************************************************
// Event used to generate a read of a new temperature value
//******************************************************************************
void emberAfPluginCarbondioxideMeasurementServerReadEventHandler(void)
{
  halCarbondioxideStartRead();
}

void halCarbondioxideReadingCompleteCallback(int carbondioxide, bool readSuccess)
{
  // If the read was successful, post the results to the cluster
  if (readSuccess) {
	emberAfCarbonDioxideConcentrationMeasurementClusterPrintln("carbondioxide: %d",
                                         carbondioxide);
    writeCarbonDioxideAttributes(carbondioxide);
  } else {
	  emberAfCarbonDioxideConcentrationMeasurementClusterPrintln("Error reading temperature from HW");
  }

  emberEventControlSetDelayMS(
		  emberAfPluginCarbondioxideMeasurementServerReadEventControl,
    carbondioxideMeasurementIntervalMs);
}

//------------------------------------------------------------------------------
// Plugin public functions

void emberAfPluginCarbonDioxideMeasurementServerSetMeasurementInterval(
  uint32_t measurementIntervalS)
{
  if ((measurementIntervalS == 0)
      || (measurementIntervalS
          > MAX_CARBONDIOXIDE_MEASUREMENT_INTERVAL_MS)) {
    carbondioxideMeasurementIntervalMs = MAX_CARBONDIOXIDE_MEASUREMENT_INTERVAL_MS;
  } else {
    carbondioxideMeasurementIntervalMs
      = measurementIntervalS * MILLISECOND_TICKS_PER_SECOND;
  }
  emberEventControlSetDelayMS(
		  emberAfPluginCarbondioxideMeasurementServerReadEventControl,
    carbondioxideMeasurementIntervalMs);
}

//------------------------------------------------------------------------------
// Plugin private functions

//******************************************************************************
// Update the temperature attribute of the temperature measurement cluster to
// be the temperature value given by the function's parameter.  This function
// will also query the current max and min read values, and update them if the
// given values is higher (or lower) than the previous records.
//******************************************************************************
static void writeCarbonDioxideAttributes(int32_t carbondixoideMilli)
{
  uint8_t i;
  uint8_t endpoint;

  // Cycle through all endpoints, check to see if the endpoint has a temperature
  // server, and if so update the temperature attributes of that endpoint
  for (i = 0; i < emberAfEndpointCount(); i++) {
    endpoint = emberAfEndpointFromIndex(i);
    if (emberAfContainsServer(endpoint, ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID)) {
      // Write the current temperature attribute
      emberAfWriteServerAttribute(endpoint,
                                  ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_CLUSTER_ID,
								  ZCL_CARBON_DIOXIDE_CONCENTRATION_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_ID,
                                  (uint8_t *) &carbondixoideMilli,
								  ZCL_FLOAT_SINGLE_ATTRIBUTE_TYPE);
    }
  }
}
