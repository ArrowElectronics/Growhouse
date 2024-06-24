// Copyright 2013 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/util/af-event.h"
#include "idle-sleep.h"

#if defined(EMBER_AF_PLUGIN_BLE)
#include EMBER_AF_API_BLE
#endif // EMBER_AF_PLUGIN_BLE

#ifdef EZSP_HOST
  #define MAX_SLEEP_VALUE_MS MAX_TIMER_MILLISECONDS_HOST
#else
  #define MAX_SLEEP_VALUE_MS 0xFFFFFFFFUL
#endif

#ifdef EMBER_AF_PLUGIN_IDLE_SLEEP_STAY_AWAKE_WHEN_NOT_JOINED
  #define STAY_AWAKE_WHEN_NOT_JOINED_DEFAULT true
#else
  #define STAY_AWAKE_WHEN_NOT_JOINED_DEFAULT false
#endif
bool emAfStayAwakeWhenNotJoined = STAY_AWAKE_WHEN_NOT_JOINED_DEFAULT;

bool emAfForceEndDeviceToStayAwake = false;

// NO PRINTFS.  This may be called in ISR context.
void emberAfForceEndDeviceToStayAwake(bool stayAwake)
{
  emAfForceEndDeviceToStayAwake = stayAwake;
}

#ifdef EMBER_AF_PLUGIN_IDLE_SLEEP_USE_BUTTONS
void emberAfHalButtonIsrCallback(uint8_t button, uint8_t state)
{
  if (state == BUTTON_PRESSED) {
    emberAfForceEndDeviceToStayAwake(button == BUTTON0);
  }
}
#endif

#ifdef EMBER_TEST
extern bool doingSerialTx[];
  #define simulatorDoingSerialTx(port) doingSerialTx[port]
#else
  #define simulatorDoingSerialTx(port) false
#endif

bool emAfOkToIdleOrSleep(void)
{
  uint8_t i;

  if (emAfForceEndDeviceToStayAwake) {
    return false;
  }

#if defined(EMBER_AF_PLUGIN_BLE)
  if (emberAfPluginBleHasEventPending()) {
    return false;
  }
#endif

  if (emAfStayAwakeWhenNotJoined) {
    bool awake = false;
    for (i = 0; !awake && i < EMBER_SUPPORTED_NETWORKS; i++) {
      if (emberAfPushNetworkIndex(i) == EMBER_SUCCESS) {
        awake = (emberAfNetworkState() != EMBER_JOINED_NETWORK);
        emberAfPopNetworkIndex();
      }
    }
    if (awake) {
      return false;
    }
  }

#ifdef EM_NUM_SERIAL_PORTS
  for (i = 0; i < EM_NUM_SERIAL_PORTS; i++) {
    if (!emberSerialUnused(i)
        && (emberSerialReadAvailable(i) != 0
            || emberSerialWriteUsed(i) != 0
            || simulatorDoingSerialTx(i))) {
      return false;
    }
  }
#else
  if (!emberSerialUnused(APP_SERIAL)
      && (emberSerialReadAvailable(APP_SERIAL) != 0
          || emberSerialWriteUsed(APP_SERIAL) != 0
          || simulatorDoingSerialTx(APP_SERIAL))) {
    return false;
  }
#endif

  return (emberAfGetCurrentSleepControlCallback() != EMBER_AF_STAY_AWAKE);
}

static EmberAfEventSleepControl defaultSleepControl = EMBER_AF_OK_TO_SLEEP;

EmberAfEventSleepControl emberAfGetCurrentSleepControlCallback(void)
{
  EmberAfEventSleepControl sleepControl = defaultSleepControl;
#ifdef EMBER_AF_GENERATED_EVENT_CONTEXT
  uint8_t i;
  for (i = 0; i < emAfAppEventContextLength; i++) {
    EmberAfEventContext *context = &emAfAppEventContext[i];
    if (emberEventControlGetActive(*context->eventControl)
        && sleepControl < context->sleepControl) {
      sleepControl = context->sleepControl;
    }
  }
#endif
  return sleepControl;
}

EmberAfEventSleepControl emberAfGetDefaultSleepControlCallback(void)
{
  return defaultSleepControl;
}

void emberAfSetDefaultSleepControlCallback(EmberAfEventSleepControl sleepControl)
{
  defaultSleepControl = sleepControl;
}
