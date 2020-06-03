// *******************************************************************
// * gbcs-device-log-cli.c
// *
// *
// * Copyright 2014 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "gbcs-device-log.h"

#ifndef EMBER_AF_GENERATE_CLI
  #error The GBCS Device Log plugin is not compatible with the legacy CLI.
#endif

// plugin gbcs-device-log clear
void emAfPluginGbcsDeviceLogCliClear(void)
{
  emberAfPluginGbcsDeviceLogClear();
}

// plugin gbcs-device-log reset
void emAfPluginGbcsDeviceLogCliReset(void)
{
  emberAfPluginGbcsDeviceLogReset();
}

// plugin gbcs-device-log count
void emAfPluginGbcsDeviceLogCliCount(void)
{
  uint8_t count = emberAfPluginGbcsDeviceLogCount();
  emberAfPluginGbcsDeviceLogPrintln("The GBCS Device Log currently has %d entries", count);
}

// plugin gbcs-device-log store <EUI64> <deviceType:1>
void emAfPluginGbcsDeviceLogCliStore(void)
{
  EmberEUI64 deviceId;
  EmberAfGBCSDeviceLogInfo deviceInfo;
  emberAfCopyBigEndianEui64Argument(0, deviceId);
  deviceInfo.deviceType = (EmberAfGBCSDeviceType)emberUnsignedCommandArgument(1);
  if (emberAfPluginGbcsDeviceLogStore(deviceId, &deviceInfo)) {
    emberAfPluginGbcsDeviceLogPrintln("Device has been successfully stored in the device log");
  }
}

// plugin gbcs-device-log remove <EUI64>
void emAfPluginGbcsDeviceLogCliRemove(void)
{
  EmberEUI64 deviceId;
  emberAfCopyBigEndianEui64Argument(0, deviceId);
  if (emberAfPluginGbcsDeviceLogRemove(deviceId)) {
    emberAfPluginGbcsDeviceLogPrintln("Device has been successfully removed from the device log");
  }
}

// plugin gbcs-device-log get <EUI64>
void emAfPluginGbcsDeviceLogCliGet(void)
{
  EmberEUI64 deviceId;
  EmberAfGBCSDeviceLogInfo deviceInfo;
  emberAfCopyBigEndianEui64Argument(0, deviceId);
  if (emberAfPluginGbcsDeviceLogGet(deviceId, &deviceInfo)) {
    emberAfPluginGbcsDeviceLogPrint("EUI64=");
    emberAfPluginGbcsDeviceLogDebugExec(emberAfPrintBigEndianEui64(deviceId));
    emberAfPluginGbcsDeviceLogPrintln(", type=%d", deviceInfo.deviceType);
  }
}

// plugin gbcs-device-log exists <EUI64> <deviceType:1>
void emAfPluginGbcsDeviceLogCliExists(void)
{
  EmberEUI64 deviceId;
  EmberAfGBCSDeviceType deviceType = (EmberAfGBCSDeviceType)emberUnsignedCommandArgument(1);
  emberAfCopyBigEndianEui64Argument(0, deviceId);
  if (emberAfPluginGbcsDeviceLogExists(deviceId, deviceType)) {
    emberAfPluginGbcsDeviceLogPrintln("Device exists in the device log");
  } else {
    emberAfPluginGbcsDeviceLogPrintln("Device does not exist in the device log");
  }
}

// plugin gbcs-device-log print
void emAfPluginGbcsDeviceLogCliPrint(void)
{
  emberAfPluginGbcsDeviceLogPrintEntries();
}

// plugin gbcs-device-log is-sleepy <deviceType:1>
void emAfPluginGbcsDeviceLogCliIsSleepy(void)
{
  EmberAfGBCSDeviceType deviceType = (EmberAfGBCSDeviceType)emberUnsignedCommandArgument(0);
  if (emberAfPluginGbcsDeviceLogIsSleepyType(deviceType)) {
    emberAfPluginGbcsDeviceLogPrintln("Device type %d is a sleepy device type", deviceType);
  } else {
    emberAfPluginGbcsDeviceLogPrintln("Device type %d is not a sleepy device type", deviceType);
  }
}

// plugin gbcs-device-log fill
void emAfPluginGbcsDeviceLogCliFill(void)
{
  uint8_t i;
  EmberEUI64 deviceId = { 0, 0, 0, 0, 0, 0, 0, 0 };
  EmberAfGBCSDeviceLogInfo deviceInfo;

  emberAfPluginGbcsDeviceLogClear();
  for (i = 0; i < EMBER_AF_PLUGIN_GBCS_DEVICE_LOG_DEVICE_LOG_SIZE; i++) {
    deviceId[7] = i + 1;
    deviceInfo.deviceType = EMBER_AF_GBCS_ESME_DEVICE_TYPE;
    emberAfPluginGbcsDeviceLogStore(deviceId, &deviceInfo);
  }
}
