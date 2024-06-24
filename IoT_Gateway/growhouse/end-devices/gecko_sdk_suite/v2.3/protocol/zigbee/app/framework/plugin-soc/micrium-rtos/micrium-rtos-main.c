// *******************************************************************
// * micrium-rtos-main.c
// *
// *
// * Copyright 2016 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include PLATFORM_HEADER
#include "app/framework/include/af.h"
#include "rtcdriver.h"

#include <kernel/include/os.h>
#include "bsp_tick_rtcc.h"
#include "bsp_cpu.h"

#if defined(EMBER_AF_PLUGIN_BLE)
#include EMBER_AF_API_BLE
#endif // EMBER_AF_PLUGIN_BLE

#if defined(EMBER_AF_PLUGIN_MBEDTLS)
#include "mbedtls/threading.h"
#endif

#if (defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_POLL_CLI) && defined(ZA_CLI_FULL))
#define ENABLE_POLL_CLI
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_POLL_CLI && ZA_CLI_FULL

//------------------------------------------------------------------------------
// Tasks variables and defines

#define SYSTEM_START_TASK_PRIORITY              4
#define BLE_LINK_LAYER_TASK_PRIORITY            4
#define BLE_STACK_TASK_PRIORITY                 5
#define ZIGBEE_STACK_TASK_PRIORITY              6

// If NVM3 is enabled, its functions can be called from the System Start task
// and can use a max stack size of 380 bytes
#ifdef EMBER_AF_PLUGIN_NVM3
 #define NVM3_STACK_SIZE_USAGE  400
#else // EMBER_AF_PLUGIN_NVM3
 #define NVM3_STACK_SIZE_USAGE  0
#endif // EMBER_AF_PLUGIN_NVM3

#define SYSTEM_START_TASK_STACK_SIZE            (200 + NVM3_STACK_SIZE_USAGE)

static OS_TCB systemStartTaskControlBlock;
static CPU_STK systemStartTaskStack[SYSTEM_START_TASK_STACK_SIZE];

static OS_TCB zigbeeTaskControlBlock;
static CPU_STK zigbeeTaskStack[EMBER_AF_PLUGIN_MICRIUM_RTOS_ZIGBEE_STACK_SIZE];

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1)
static OS_TCB applicationTask1ControlBlock;
static CPU_STK applicationTask1Stack[EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_STACK_SIZE];
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2)
static OS_TCB applicationTask2ControlBlock;
static CPU_STK applicationTask2Stack[EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_STACK_SIZE];
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3)
static OS_TCB applicationTask3ControlBlock;
static CPU_STK applicationTask3Stack[EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_STACK_SIZE];
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3

//------------------------------------------------------------------------------
// Forward and external declarations

static void systemStartTask(void *p_arg);
static void wakeUpZigbeeStackTask(void);
static void initMicriumCpu(void);
void App_OS_SetAllHooks(void);

//------------------------------------------------------------------------------
// Main

int main(MAIN_FUNCTION_PARAMETERS)
{
  RTOS_ERR err;

  halInit();
  initMicriumCpu();
  emberAfMainInit();

  OS_TRACE_INIT();
  OSInit(&err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  App_OS_SetAllHooks();

  // Create the systemStart task which initializes things
  OSTaskCreate(&systemStartTaskControlBlock,
               "System Start",
               systemStartTask,
               NULL,
               SYSTEM_START_TASK_PRIORITY,
               &systemStartTaskStack[0],
               SYSTEM_START_TASK_STACK_SIZE / 10,
               SYSTEM_START_TASK_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  // Start the OS
  OSStart(&err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

//------------------------------------------------------------------------------
// Implemented callbacks

bool emberAfPluginIdleSleepRtosCallback(uint32_t *durationMs, bool sleepOk)
{
  uint32_t actualDurationMs = *durationMs;
  uint32_t deltaMs, startTicks = RTCDRV_GetWallClockTicks32();
  OS_TICK yieldTimeTicks = (OSCfg_TickRate_Hz * actualDurationMs) / 1000;
  RTOS_ERR err;
  CPU_TS ts;

  if (!sleepOk) {
    // TODO: disable deep sleep at the sleep manager.
    //rtosBlockDeepSleep();
  }

  INTERRUPTS_ON();

  // Yield the stack task.
  OSTaskSemPend(yieldTimeTicks, OS_OPT_PEND_BLOCKING, &ts, &err);

  if (!sleepOk) {
    // TODO: re-enable deep sleep at the sleep manager.
    //if (rtosGetDeepSleepBlockCount() > 0) {
    //  rtosUnblockDeepSleep();
    //}
  }

  deltaMs = RTCDRV_TicksToMsec(RTCDRV_GetWallClockTicks32() - startTicks);
  if ( deltaMs <= actualDurationMs ) {
    *durationMs = actualDurationMs - deltaMs;
  } else {
    *durationMs = 0;
  }

  return true;
}

bool emberRtosIdleHandler(uint32_t *idleTimeMs)
{
  return emberAfPluginIdleSleepRtosCallback(idleTimeMs, false);
}

void emberRtosStackWakeupIsrHandler(void)
{
  wakeUpZigbeeStackTask();
}

void halPendSvIsr(void)
{
  PendSV_Handler();
}

void halSysTickIsr(void)
{
  SysTick_Handler();
}

//------------------------------------------------------------------------------
// Static functions

// This can be called from ISR.
static void wakeUpZigbeeStackTask(void)
{
  RTOS_ERR err;

  OSTaskSemPost(&zigbeeTaskControlBlock,
                OS_OPT_POST_NONE,
                &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

static void zigbeeTask(void *p_arg)
{
  emberAfMain();
}

#if defined(ENABLE_POLL_CLI)

static void pollCliTimerCallback(void * p_tmr, void * p_arg)
{
  wakeUpZigbeeStackTask();
}

#define POLL_CLI_TASK_PERIOD_MS                 250

static void startPollCliTimer(void)
{
  static OS_TMR pollCliTimer;
  RTOS_ERR err;

  // Create a periodic software timer that wakes up the ZigBee task so that it
  // can poll the CLI.
  // TODO: for some reason the time params are working as they would be
  // expressed in 1/10 of a seconds units.
  OSTmrCreate(&pollCliTimer,
              "RSCHED Timer",
              POLL_CLI_TASK_PERIOD_MS / 100, // Delay
              POLL_CLI_TASK_PERIOD_MS / 100, // Period
              OS_OPT_TMR_PERIODIC,
              pollCliTimerCallback,
              NULL,
              &err);
  EFM_ASSERT(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  OSTmrStart(&pollCliTimer, &err);
  EFM_ASSERT(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

#endif // ENABLE_POLL_CLI

static void systemStartTask(void *p_arg)
{
  RTOS_ERR err;

  // Setup OS tick
#if (OS_CFG_DYN_TICK_EN == DEF_ENABLED)
  BSP_RTCC_TickInit();
#else
  CPU_INT32U cnts = (SystemCoreClockGet() / (CPU_INT32U)OSCfg_TickRate_Hz);
  OS_CPU_SysTickInit(cnts);
#endif

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_CPU_USAGE)
  OSStatTaskCPUUsageInit(&err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_CPU_USAGE

#if defined(EMBER_AF_PLUGIN_MBEDTLS)
  THREADING_setup();
#endif

  // Create ZigBee task.
  OSTaskCreate(&zigbeeTaskControlBlock,
               "Zigbee Stack",
               zigbeeTask,
               NULL,
               ZIGBEE_STACK_TASK_PRIORITY,
               &zigbeeTaskStack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_ZIGBEE_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_ZIGBEE_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#if defined(EMBER_AF_PLUGIN_BLE)
  gecko_init(emberAfPluginBleGetConfig());
  gecko_init_multiprotocol(NULL);
  BluetoothSetWakeupCallback(wakeUpZigbeeStackTask);
  bluetooth_start_task(BLE_LINK_LAYER_TASK_PRIORITY,
                       BLE_STACK_TASK_PRIORITY);
#endif // EMBER_AF_PLUGIN_BLE

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1)

  emberAfPluginMicriumRtosAppTask1InitCallback();

  // Create Application Task 1.
  OSTaskCreate(&applicationTask1ControlBlock,
               "Application (1)",
               emberAfPluginMicriumRtosAppTask1MainLoopCallback,
               NULL,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_PRIORITY,
               &applicationTask1Stack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2)

  emberAfPluginMicriumRtosAppTask2InitCallback();

  // Create Application Task 2.
  OSTaskCreate(&applicationTask2ControlBlock,
               "Application (2)",
               emberAfPluginMicriumRtosAppTask2MainLoopCallback,
               NULL,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_PRIORITY,
               &applicationTask2Stack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3)

  emberAfPluginMicriumRtosAppTask3InitCallback();

  // Create Application Task 3.
  OSTaskCreate(&applicationTask3ControlBlock,
               "Application (3)",
               emberAfPluginMicriumRtosAppTask3MainLoopCallback,
               NULL,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_PRIORITY,
               &applicationTask3Stack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3

#if defined(ENABLE_POLL_CLI)
  startPollCliTimer();
#endif // ENABLE_POLL_CLI

  // Done starting everyone else so let's exit
  OSTaskDel((OS_TCB *)0, &err);
}

// Register interrupt vectors with the OS
static void initMicriumCpu(void)
{
  BSP_CPUInit();
  // Radio Interrupts can optionally be configured non-kernel aware at this point
}
