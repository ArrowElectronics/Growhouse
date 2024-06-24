// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#include "app/framework/include/af.h"
#include EMBER_AF_API_HUMIDITY

//******************************************************************************
// CLI function to manually generate a humidity read
//******************************************************************************
void emAfRelativeHumidityMeasurementServerReadCommand(void)
{
  // No result will be printed here, as the callback that generates the value
  // is already implemented in relative-humidity-measurement-server.c.
  // As such, the callback will generate the diagnostic print.
  halHumidityStartRead();
}
