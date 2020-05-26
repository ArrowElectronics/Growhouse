/***************************************************************************//**
 * @file
 * @brief Silicon Labs Bluetooth mesh light example
 * This example implements a Bluetooth mesh light node.
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

#include <stdio.h>

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "board_features.h"
#include "retargetserial.h"

/* Bluetooth stack headers */
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include <em_gpio.h>

#include "gpiointerrupt.h"

/* Device initialization header */
#include "hal-config.h"

/* Display Interface header */
#include "display_interface.h"

/* LED driver with support for PWM dimming */
#include "led_driver.h"

/* Application code */
#include "app.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/* watchdog support */
#include "em_wdog.h"

/* USART for serial communication with Zigbee */
#include "ZIGBEE_Usart_Comm.h"

/* SPI driver header for AD5206 */
#include "ad5206.h"

/* spi flash */
#include "mx25flash_spi.h"

/* bootloader header */
#include "api/btl_interface.h"
/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

volatile uint32_t msTicks; /* counts 1ms timeTicks */
extern uint8_t zigbeeAvailable;

/* Version specific Structure */
extern const ApplicationProperties_t bg_application_properties;

/* Watchdog init configuration */
WDOG_Init_TypeDef init =
{
 .enable = true,             /* Start watchdog when init done */
 .debugRun = false,          /* WDOG not counting during debug halt */
 .em2Run = true,             /* WDOG counting when in EM2 */
 .em3Run = true,             /* WDOG counting when in EM3 */
 .em4Block = false,          /* EM4 can be entered */
 .swoscBlock = false,        /* Do not block disabling LFRCO/LFXO in CMU */
 .lock = false,              /* Do not lock WDOG configuration (if locked, reset needed to unlock) */
 .clkSel = wdogClkSelULFRCO, /* Select 1kHZ WDOG oscillator */
 .perSel = wdogPeriod_8k,    /* Set the watchdog period to 8193 clock periods (ie ~8 seconds)*/
};

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

/// Maximum number of simultaneous Bluetooth connections
#define MAX_CONNECTIONS 2

/// Heap for Bluetooth stack
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

/// Bluetooth advertisement set configuration
///
/// At minimum the following is required:
/// * One advertisement set for Bluetooth LE stack (handle number 0)
/// * One advertisement set for Mesh data (handle number 1)
/// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
/// * One advertisement set for Mesh unprovisioned URI (handle number 3)
/// * N advertisement sets for Mesh GATT service advertisements
/// (one for each network key, handle numbers 4 .. N+3)
///
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

/// Priorities for bluetooth link layer operations
static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

/// Bluetooth stack configuration
const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};

/**
 *  this function is for Interrupt Service Routine for system tick counter
 */
void SysTick_Handler(void) {
  msTicks++; /* increment counter necessary in Delay()*/
}

/**
 *  this function is for Delays number of msTick Systicks (typically 1 ms)
 */
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;
  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

/***************************************************************************//**
 * Button initialization. Configure pushbuttons PB0, PB1 as inputs.
 ******************************************************************************/
static void button_init(void)
{
  // configure pushbutton PB0 and PB1 as inputs, with pull-up enabled
  GPIO_PinModeSet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN, gpioModeInputPull, 1);
  GPIO_PinModeSet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN, gpioModeInputPull, 1);
#ifdef FACTORY_RESET_BUTTON
  GPIO_PinModeSet(BSP_BUTTON_FACTORY_RESET_PORT, BSP_BUTTON_FACTORY_RESET_PIN, gpioModeInputPull, 1);
  GPIO_ExtIntConfig(BSP_BUTTON_FACTORY_RESET_PORT, BSP_BUTTON_FACTORY_RESET_PIN, BSP_BUTTON_FACTORY_RESET_PIN, true, true, true);
  GPIOINT_Init();
  GPIOINT_CallbackRegister(BSP_BUTTON_FACTORY_RESET_PIN, gpio_interrupt_handler_cb);
#endif //FACTORY_RESET_BUTTON
}

/***************************************************************************//**
 * Main function.
 ******************************************************************************/
int main(void)
{
  uint32_t btl_version = 0;
  uint32_t mfgId = 0;

  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);
  gecko_bgapi_classes_init();

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

  RETARGET_SerialInit();

  /* Setup SysTick Timer for 1 msec interrupts */
  if (SysTick_Config(SystemCoreClock / 1000)) while (1) ;

  /* Initialize UART for communication with Zigbee module */
  ZIGBEE_SerialInit();

  /* initialize AD5206 digital potentiometer over SPI */
  dpotAd5206Init();

  /* initialize LEDs and buttons. Note: some radio boards share the same GPIO for button & LED.
   * Initialization is done in this order so that default configuration will be "button" for those
   * radio boards with shared pins. LEDS_init() is called later as needed to (re)initialize the LEDs
   * */
  LEDS_init();
  button_init();

#ifdef DISPLAY_EVAL_KIT
  // Display Interface initialization
  DI_Init();
#endif //DISPLAY_EVAL_KIT

  /* Check for zigbee module presence in LED Node using the GPIO shared between
   * Zigbee and BLE modules. When the GPIO is LOW, Zigbee module is present otherwise the
   * zigbee module is not available */
  IsZigbeeAvailable();

  /* init watchdog */
  WDOG_Init(&init);

  btl_version = mainBootloaderTable->header.version & 0x000F;
  printf("Bootloader Version : %lu, Application Version : %lu\r\n",
                           btl_version,
                           bg_application_properties.app.version);

  MX25_RDID(&mfgId);
  printf("FLash memory manufactor ID: %x\r\n", mfgId);

  while (1) {

	/* Non-Blocking call for gecko events */
	struct gecko_cmd_packet *evt = gecko_peek_event();

	/* Blocking call for gecko events */
    //struct gecko_cmd_packet *evt = gecko_wait_event();

    if(evt != NULL) {
      bool pass = mesh_bgapi_listener(evt);
      if (pass) {
        handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
      }
    }

    if(zigbeeAvailable)
	{
        ZIGBEEUartProcessInput();
	}
  }
}
