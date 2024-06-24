// Copyright 2014 Silicon Laboratories, Inc.

// Obtain a single reading of the VDD voltage (used to check for shutdown)
// Result is in millivolts.
uint16_t emberAfPluginLowVoltageShutdownGetVoltage(void);

// Enable or disable shutdown logic at runtime for testing purposes
void emberAfPluginLowVoltageShutdownEnable(bool enable);

// Check whether the shutdown logic has is enabled at runtime (as it can be
// artificially disabled)
bool emberAfPluginLowVoltageShutdownEnabled(void);

// Force a shutdown event (regardless of VDD) for testing
void emberAfPluginLowVoltageShutdownForceShutdown(void);
