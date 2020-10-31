// *******************************************************************
// * gbcs-gas-meter.h
// *
// *
// * Copyright 2014 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#ifndef GBCS_GAS_METER_H_
#define GBCS_GAS_METER_H_

// Printing macros for plugin: GBCS Gas Meter
#define emberAfPluginGbcsGasMeterPrint(...)    emberAfAppPrint(__VA_ARGS__)
#define emberAfPluginGbcsGasMeterPrintln(...)  emberAfAppPrintln(__VA_ARGS__)
#define emberAfPluginGbcsGasMeterDebugExec(x)  emberAfAppDebugExec(x)
#define emberAfPluginGbcsGasMeterPrintBuffer(buffer, len, withSpace) emberAfAppPrintBuffer(buffer, len, withSpace)

/** @brief Reports attributes.
 *
 * If the mirror is ready to receive attribute reports and a report isn't already
 * in progress, this function will kick off the task of reporting
 * all attributes to the mirror.
 */
void emberAfPluginGbcsGasMeterReportAttributes(void);

#endif /* GBCS_GAS_METER_H_ */
