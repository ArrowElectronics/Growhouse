/*
 * RGBIntensity-measurement-server.c
 *
 *  Created on: 17-Sep-2018
 *      Author: einfochips
 */

#include "app/framework/include/af.h"

#ifdef EMBER_AF_PLUGIN_LIGHT_INTENSITY_STUB
#include "platform/base/hal/plugin/light_intensity_stub/light-intensity-stub.h"
#endif

#ifdef EMBER_AF_PLUGIN_REPORTING
#include "app/framework/plugin/reporting/reporting.h"
#endif

//------------------------------------------------------------------------------
// Plugin private macros

// Shorter macros for plugin options
#define MAX_RGB_INTENSITY_MEASUREMENT_INTERVAL_MS                               \
  (EMBER_AF_PLUGIN_RGB_INTENSITY_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S \
   * MILLISECOND_TICKS_PER_SECOND)

//------------------------------------------------------------------------------
// Forward Declaration of private functions
static void writeRGBIntensityAttributes(uint32_t RIntensity, uint32_t GIntensity, uint32_t BIntensity);
static void checkForReportingConfig(void);
void emberAfPluginRGBIntensityMeasurementServerSetMeasurementInterval(uint32_t measurementIntervalS);

//------------------------------------------------------------------------------
// Global variables
EmberEventControl emberAfPluginRgbIntensityMeasurementServerReadEventControl;
static uint32_t RGBIntensityMeasurementIntervalMs =
  MAX_RGB_INTENSITY_MEASUREMENT_INTERVAL_MS;

// The default configuration to be used if no reporting has been set up
#ifdef EMBER_AF_PLUGIN_REPORTING
static EmberAfPluginReportingEntry defaultConfiguration[3] = {
		{
				EMBER_ZCL_REPORTING_DIRECTION_REPORTED, //direction
				0, //endpoint, which will be set on a per-use basis
				ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID, //clusterId
				ZCL_R_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID, //attributeId
				CLUSTER_MASK_SERVER, //mask
				EMBER_AF_NULL_MANUFACTURER_CODE, //manufacturerCode
				.data.reported = {
						0, //minInterval
						EMBER_AF_PLUGIN_RGB_INTENSITY_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S, //maxInterval
						0 //reportableChange
				}
		},
		{
				EMBER_ZCL_REPORTING_DIRECTION_REPORTED, //direction
				0, //endpoint, which will be set on a per-use basis
				ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID, //clusterId
				ZCL_G_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID, //attributeId
				CLUSTER_MASK_SERVER, //mask
				EMBER_AF_NULL_MANUFACTURER_CODE, //manufacturerCodere
				.data.reported = {
						1, //minInterval
						50, //maxInterval
						0 //reportableChange
				}
		},
		{
				EMBER_ZCL_REPORTING_DIRECTION_REPORTED, //direction
				0, //endpoint, which will be set on a per-use basis
				ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID, //clusterId
				ZCL_B_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID, //attributeId
				CLUSTER_MASK_SERVER, //mask
				EMBER_AF_NULL_MANUFACTURER_CODE, //manufacturerCode
				.data.reported = {
						1, //minInterval
						50, //maxInterval
						0 //reportableChange
				}
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
  bool existingREntry = false;
  bool existingGEntry = false;
  bool existingBEntry = false;


  // Cycle through the reporting table to determine if an entry has been created
  // for the device before.  If so, update the sensor's hardware polling rate to
  // match the attribute defined maxInterval.  Otherwise, the plugin will use
  // the plugin's option defined default hardware polling interval.
  for (i = 0; i < REPORT_TABLE_SIZE; i++) {
    emAfPluginReportingGetEntry(i, &entry);
    if ((entry.clusterId == ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID)
        && (entry.direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED)
        && (entry.endpoint != EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID)
        && (entry.manufacturerCode == EMBER_AF_NULL_MANUFACTURER_CODE)) {
      // Max interval is set in seconds, which is the same unit of time the
      // emberAfPluginRGBIntensityMeasurementServerSetMeasurementInterval
      // expects in this API.
      switch (entry.attributeId) {
      	  case ZCL_R_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID:
      		existingREntry = true;
    	  break;

      	  case ZCL_G_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID:
      		existingGEntry = true;
    	  break;

      	  case ZCL_B_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID:
      		existingBEntry = true;
    	  break;
      }

    }
  }

  if (existingREntry && existingGEntry && existingBEntry) {
	  emberAfPluginRGBIntensityMeasurementServerSetMeasurementInterval(
	          entry.data.reported.maxInterval);
  }

  // If no entry is found for the rgb intensity  measurement server, a default
  // reporting configuration should be created using the plugin defined options.
  // This needs to be done for all endpoints that support a rgb intensity
  // measurement server.
  if (!(existingREntry && existingGEntry && existingBEntry)) {
    for (i = 0; i < emberAfEndpointCount(); i++) {
      endpoint = emberAfEndpointFromIndex(i);
      defaultConfiguration[0].endpoint = endpoint;
      defaultConfiguration[1].endpoint = endpoint;
      defaultConfiguration[2].endpoint = endpoint;
      if (emberAfContainsServer(endpoint, ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID)) {
        emAfPluginReportingAddEntry(&defaultConfiguration[0]);
        emAfPluginReportingAddEntry(&defaultConfiguration[1]);
        emAfPluginReportingAddEntry(&defaultConfiguration[2]);
      }
    }
  }
#endif
}

//******************************************************************************
// Plugin init function
//******************************************************************************
void emberAfPluginRgbIntensityMeasurementServerInitCallback(void)
{
  checkForReportingConfig();

  // Start the ReadEvent, which will re-activate itself perpetually
  emberEventControlSetActive(
		  emberAfPluginRgbIntensityMeasurementServerReadEventControl);
}

void emberAfPluginRgbIntensityMeasurementServerStackStatusCallback(
  EmberStatus status)
{
  // On network connect, chances are very good that someone (coordinator,
  // gateway, etc) will poll the rgb intensity  for an initial status.  As such,
  // it is useful to have fresh data to be polled.
  if (status == EMBER_NETWORK_UP) {
    checkForReportingConfig();
    emberEventControlSetActive(
      emberAfPluginRgbIntensityMeasurementServerReadEventControl);
  }
}

//------------------------------------------------------------------------------
// Plugin event handlers

//******************************************************************************
// Event used to generate a read of a new rgb intensity  value
//******************************************************************************
void emberAfPluginRgbIntensityMeasurementServerReadEventHandler(void)
{
  halRGBIntensityStartRead();
}

void halRGBIntensityReadingCompleteCallback(int set_RIntensity,
											int set_GIntensity,
											int set_BIntensity,
                                           bool readSuccess)
{
  // If the read was successful, post the results to the cluster
  if (readSuccess) {
	  emberAfRgbIntensityMeasurementClusterPrintln("Rgb: %d, %d, %d", set_RIntensity, set_GIntensity, set_BIntensity);
    writeRGBIntensityAttributes(set_RIntensity, set_GIntensity, set_BIntensity);
  } else {
	  emberAfRgbIntensityMeasurementClusterPrintln("Error reading rgb intensity  from HW");
  }

  emberEventControlSetDelayMS(
    emberAfPluginRgbIntensityMeasurementServerReadEventControl,
    RGBIntensityMeasurementIntervalMs);
}

//------------------------------------------------------------------------------
// Plugin public functions

void emberAfPluginRGBIntensityMeasurementServerSetMeasurementInterval(
  uint32_t measurementIntervalS)
{
  if ((measurementIntervalS == 0)
      || (measurementIntervalS
          > MAX_RGB_INTENSITY_MEASUREMENT_INTERVAL_MS)) {
    RGBIntensityMeasurementIntervalMs = MAX_RGB_INTENSITY_MEASUREMENT_INTERVAL_MS;
  } else {
    RGBIntensityMeasurementIntervalMs
      = measurementIntervalS * MILLISECOND_TICKS_PER_SECOND;
  }
  emberEventControlSetDelayMS(
    emberAfPluginRgbIntensityMeasurementServerReadEventControl,
    RGBIntensityMeasurementIntervalMs);
}

//------------------------------------------------------------------------------
// Plugin private functions

//******************************************************************************
// Update the rgb intensity  attribute of the rgb intensity  measurement cluster to
// be the rgb intensity  value given by the function's parameter.  This function
// will also query the current max and min read values, and update them if the
// given values is higher (or lower) than the previous records.
//******************************************************************************
static void writeRGBIntensityAttributes(uint32_t RIntensity, uint32_t GIntensity, uint32_t BIntensity)
{
  uint16_t rIntensity = RIntensity;
  uint16_t gIntensity = GIntensity;
  uint16_t bIntensity = BIntensity;

  uint8_t i;

  uint8_t endpoint;

  // Cycle through all endpoints, check to see if the endpoint has a rgb intensity
  // server, and if so update the rgb intensity  attributes of that endpoint
  for (i = 0; i < emberAfEndpointCount(); i++) {
    endpoint = emberAfEndpointFromIndex(i);
    if (emberAfContainsServer(endpoint, ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID)) {
      // Write the current rgb intensity  attribute
      emberAfWriteServerAttribute(endpoint,
                                  ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID,
                                  ZCL_R_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID,
                                  (uint8_t *) &rIntensity,
                                  ZCL_INT16U_ATTRIBUTE_TYPE);
      emberAfWriteServerAttribute(endpoint,
                                        ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID,
										ZCL_G_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID,
                                        (uint8_t *) &gIntensity,
                                        ZCL_INT16U_ATTRIBUTE_TYPE);
      emberAfWriteServerAttribute(endpoint,
                                        ZCL_RGB_INTENSITY_MEASUREMENT_CLUSTER_ID,
										ZCL_B_INTENSITY_MEASURED_VALUE_ATTRIBUTE_ID,
                                        (uint8_t *) &bIntensity,
                                        ZCL_INT16U_ATTRIBUTE_TYPE);
    }
  }
}
