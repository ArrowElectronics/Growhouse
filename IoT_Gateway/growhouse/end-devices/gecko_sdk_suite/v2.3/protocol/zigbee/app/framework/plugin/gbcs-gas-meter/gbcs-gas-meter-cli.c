/// *******************************************************************
// * gbcs-gas-meter-cli.c
// *
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "gbcs-gas-meter.h"

// plugin gbcs-gas-meter report-attributes
void emAfPluginGbcsGasMeterCliReportAttributes(void)
{
  emberAfPluginGbcsGasMeterReportAttributes();
}
