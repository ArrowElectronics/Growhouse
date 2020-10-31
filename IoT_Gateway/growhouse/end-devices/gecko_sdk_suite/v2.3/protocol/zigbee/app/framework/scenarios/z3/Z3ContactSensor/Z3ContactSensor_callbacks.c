// *******************************************************************
// * Z3ContactSensor_callbacks.c
// *
// * This file implements non-plugin functionality required for the Silicon Labs
// * contact sensor reference design to properly function.
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/framework/plugin/reporting/reporting.h"
#include "app/framework/plugin-soc/temperature-measurement-server/temperature-measurement-server.h"

// This callback will execute any time the reporting intervals are modified.
// In order to verify the contact sensor is polling the temperature sensor
// frequently enough, it is necessary to call the
// emberAfPluginTemperatureMeasurementServerSetMeasurementRate function any time
// the reporting intervals are changed.
EmberAfStatus emberAfPluginReportingConfiguredCallback(
  const EmberAfPluginReportingEntry *entry)
{
  if ((entry->clusterId == ZCL_TEMP_MEASUREMENT_CLUSTER_ID)
      && (entry->attributeId == ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID)
      && (entry->direction == EMBER_ZCL_REPORTING_DIRECTION_REPORTED)
      && (entry->endpoint != EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID)) {
    //Max interval is set in seconds, which is the same unit of time the
    //emberAfPluginTemperatureMeasurementServerSetMeasurementRate expects in
    //this API.
    emberAfPluginTemperatureMeasurementServerSetMeasurementInterval(
      entry->data.reported.maxInterval);
  }

  return EMBER_ZCL_STATUS_SUCCESS;
}
