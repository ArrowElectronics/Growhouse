// Copyright 2017 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "rtos_bluetooth.h"
#include "gatt_db.h"

#include <kernel/include/os.h>

#define BLE_HEAP_SIZE DEFAULT_BLUETOOTH_HEAP(EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS)

#if defined(EMBER_AF_PLUGIN_BLE_PSSTORE_LIBRARY)
// This is needed in order to properly place the PSStore space at the top of
// flash (PSStore is not relocatable, so it needs to be at the top of flash).
VAR_AT_SEGMENT(NO_STRIPPING uint8_t psStore[FLASH_PAGE_SIZE * 2], __PSSTORE__);
#endif

static uint8_t bluetooth_stack_heap[BLE_HEAP_SIZE];

void BluetoothLLCallback();
void BluetoothUpdate();

static gecko_configuration_t config =
{
  .config_flags = GECKO_CONFIG_FLAG_RTOS,
  .sleep.flags = 0,
  .bluetooth.max_connections = EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap),
  .gattdb = &bg_gattdb_data,
  .scheduler_callback = BluetoothLLCallback,
  .stack_schedule_callback = BluetoothUpdate,
  .mbedtls.flags = GECKO_MBEDTLS_FLAGS_NO_MBEDTLS_DEVICE_INIT,
  .mbedtls.dev_number = 0,
};

gecko_configuration_t *emberAfPluginBleGetConfig(void)
{
  emberAfPluginBleGetConfigCallback(&config);

  // Re-assing the heap-related fields, just in case.
  config.bluetooth.max_connections = EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS;
  config.bluetooth.heap = bluetooth_stack_heap;
  config.bluetooth.heap_size = sizeof(bluetooth_stack_heap);

  return &config;
}

bool emberAfPluginBleHasEventPending(void)
{
  RTOS_ERR os_err;

  OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)BLUETOOTH_EVENT_FLAG_EVT_WAITING,
             0,
             OS_OPT_PEND_NON_BLOCKING
             + OS_OPT_PEND_FLAG_SET_ANY,
             NULL,
             &os_err);

  return (RTOS_ERR_CODE_GET(os_err) == RTOS_ERR_NONE);
}

void emberAfPluginBleInitCallback(void)
{
  // TODO
}

// This is run as part of the Zigbee task main loop.
void emberAfPluginBleTickCallback(void)
{
  RTOS_ERR os_err;

  OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)BLUETOOTH_EVENT_FLAG_EVT_WAITING,
             0,
             OS_OPT_PEND_NON_BLOCKING
             + OS_OPT_PEND_FLAG_SET_ANY
             + OS_OPT_PEND_FLAG_CONSUME,
             NULL,
             &os_err);

  if (RTOS_ERR_CODE_GET(os_err) == RTOS_ERR_NONE) {
    emberAfPluginBleEventCallback((struct gecko_cmd_packet*)bluetooth_evt);

    OSFlagPost(&bluetooth_event_flags,
               (OS_FLAGS)BLUETOOTH_EVENT_FLAG_EVT_HANDLED,
               OS_OPT_POST_FLAG_SET,
               &os_err);
  }
}
