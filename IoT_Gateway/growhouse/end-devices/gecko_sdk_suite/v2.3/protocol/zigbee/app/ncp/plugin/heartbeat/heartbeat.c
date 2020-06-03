// Copyright 2015 Silicon Laboratories, Inc.

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_HAL

void emberAfPluginHeartbeatTickCallback(void)
{
  static uint32_t lastMs = 0;
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  if (EMBER_AF_PLUGIN_HEARTBEAT_PERIOD_QS * MILLISECOND_TICKS_PER_QUARTERSECOND
      < elapsedTimeInt32u(lastMs, nowMs)) {
    halToggleLed(BOARD_HEARTBEAT_LED);
    lastMs = nowMs;
  }
}
