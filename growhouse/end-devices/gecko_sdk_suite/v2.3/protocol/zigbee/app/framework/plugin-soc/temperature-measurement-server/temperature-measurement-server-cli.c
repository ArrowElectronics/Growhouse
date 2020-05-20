// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#ifdef EMBER_SCRIPTED_TEST
  #include "app/framework/plugin-soc/temperature-measurement-server/temperature-measurement-server-test.h"
#endif

#include "app/framework/include/af.h"
#include EMBER_AF_API_TEMPERATURE

//******************************************************************************
// CLI function to manually generate a temperature read
//******************************************************************************
void emAfTemperatureMeasurementServerReadCommand(void)
{
  halTemperatureStartRead();

  // No result will be printed here, as the callback that generates the value
  // is already implemented in temperature-measurement-server.c.  As such, the
  // callback will generate the diagnostic print.
}
