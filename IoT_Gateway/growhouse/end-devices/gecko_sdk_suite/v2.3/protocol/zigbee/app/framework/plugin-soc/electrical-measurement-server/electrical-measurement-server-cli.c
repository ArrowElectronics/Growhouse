// Copyright 2015 Silicon Laboratories, Inc.                                *80*

#include "app/framework/include/af.h"
#include EMBER_AF_API_POWER_METER

//HalPowerMeterCalibrationData calibrationData;

//******************************************************************************
// CLI function to read Active Power
//******************************************************************************
void emAfElectricalMeasurementServerReadActivePower(void)
{
  int32_t activePower;

  activePower = halGetActivePowerMilliW();
  emberAfAppPrintln("Active Power = %d mW", activePower);
}

//******************************************************************************
// CLI function to read Power Factor
//******************************************************************************
void emAfElectricalMeasurementServerReadPowerFactor(void)
{
  int8_t powerFactor;

  powerFactor = halGetPowerFactor();
  emberAfAppPrintln("Power Factor= %d", powerFactor);
}

//******************************************************************************
// CLI function to read RMS Voltage
//******************************************************************************
void emAfElectricalMeasurementServerReadVrms(void)
{
  uint32_t rmsVoltage;

  rmsVoltage = halGetVrmsMilliV();
  emberAfAppPrintln("RMS Voltage = %d mV", rmsVoltage);
}

//******************************************************************************
// CLI function to read RMS Current
//******************************************************************************
void emAfElectricalMeasurementServerReadIrms(void)
{
  uint32_t rmsCurrent;

  rmsCurrent = halGetCrmsMilliA();
  emberAfAppPrintln("RMS Current = %d mA", rmsCurrent);
}

//******************************************************************************
// CLI command to perform calibration of power meter sensor
//******************************************************************************
void emAfElectricalMeasurementServerCalibrate(void)
{
  int8_t temperatureDegree = emberSignedCommandArgument(0);

  emberAfAppPrintln("Temperature = %d DegreeC, Calibrating...",
                    temperatureDegree);
//  halPowerMeterCalibrateAll((int16_t)temperatureDegree * 100, &calibrationData);

//  emberAfAppPrintln("Done!");
//  emberAfAppPrintln("Calibrated DC Voltage offset = 0x%x",
//                    calibrationData.dcVoltageOffset);
//  emberAfAppPrintln("Calibrated DC Current offset = 0x%x",
//                    calibrationData.dcCurrentOffset);
//  emberAfAppPrintln("Calibrated AC Voltage offset = 0x%x",
//                    calibrationData.acVoltageOffset);
//  emberAfAppPrintln("Calibrated AC Current offset = 0x%x",
//                    calibrationData.acCurrentOffset);
//  emberAfAppPrintln("Calibrated temperature offset = 0x%x",
//                    calibrationData.temperatureOffset);
}
