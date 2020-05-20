/***************************************************************************//**
 * @file
 * @brief Silabs BT Mesh NCP Target Application
 * This application is intended to be used with the iOS Silicon Labs
 * app for demonstration purposes
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include <ncp_gecko.h>
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>
#include "ncp_sec.h"

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>
#include "em_core.h"

/* Device initialization header */
#include "hal-config.h"

/* Uart initialization header */
#include "ncp_usart.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

void mesh_native_bgapi_init(void);
void mesh_ncp_bgapi_init(void);

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;
const gecko_configuration_t config = {
  .config_flags = 0,
#if (NCP_DEEP_SLEEP_ENABLED)
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
#endif
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .pa.config_enable = 1, // Enable high power PA
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#endif // (HAL_PA_ENABLE) && defined(FEATURE_PA_HIGH_POWER)
  .max_timers = 16,
};

__attribute__((weak)) void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
}

void mesh_ncp_bgapi_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

int main()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // NCP USART init
  ncp_usart_init();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  /* Initialize stack */
  gecko_stack_init(&config);
  ncp_gecko_bgapi_class_dfu_init();
  ncp_gecko_bgapi_class_system_init();
  ncp_gecko_bgapi_class_le_gap_init();
  ncp_gecko_bgapi_class_le_connection_init();
  ncp_gecko_bgapi_class_gatt_init();
  ncp_gecko_bgapi_class_gatt_server_init();
  ncp_gecko_bgapi_class_hardware_init();
  ncp_gecko_bgapi_class_flash_init();
  ncp_gecko_bgapi_class_test_init();
  ncp_gecko_bgapi_class_sm_init();
  mesh_ncp_bgapi_init();
  gecko_initCoexHAL();

  while (1) {
    struct gecko_cmd_packet *evt;
    ncp_handle_command();
    /* Check for stack event. */
    evt = gecko_peek_event();
    while (evt) {
      if (!ncp_handle_event(evt)) {
        bool pass = mesh_bgapi_listener(evt);
        if (pass) {
          ncp_transmit_enqueue(evt);
        }
      }
      // if a command is received, break and handle the command
      if (ncp_command_received()) {
        break;
      }
      evt = gecko_peek_event();
    }
    ncp_transmit();

    // If an NCP command received, do not sleep
    if (!ncp_command_received()) {
      CORE_CRITICAL_IRQ_DISABLE();
      gecko_sleep_for_ms(gecko_can_sleep_ms());
      CORE_CRITICAL_IRQ_ENABLE();
    }
  }
}
