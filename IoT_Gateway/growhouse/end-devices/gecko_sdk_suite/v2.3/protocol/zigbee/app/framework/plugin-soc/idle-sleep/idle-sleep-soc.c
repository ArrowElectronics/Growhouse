// Copyright 2013 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/util/af-event.h"
#include "idle-sleep.h"

#if defined(EMBER_AF_HAS_RX_ON_WHEN_IDLE_NETWORK) && defined(EMBER_NO_IDLE_SUPPORT)
  #warning Idling and sleeping is not available on SoCs with RX-on-when-idle networks that do not support idling.
#else
  #define IDLE_AND_SLEEP_SUPPORTED
#endif

#if defined(EMBER_PLUGIN_EM4) && ((EMBER_SUPPORTED_NETWORKS == 1))
#error Supported Networks must be set to 1
#endif

#ifdef IDLE_AND_SLEEP_SUPPORTED
uint32_t lastWakeupMs = 0;

void emberAfPluginIdleSleepTickCallback(void)
{
  // Turn interrupts off so that we can safely determine whether we can sleep
  // or idle.  Both halSleepForMilliseconds and emberMarkTaskIdle forcibly
  // enable interrupts.  We can call neither function or exactly one of them
  // without risking a race condition.  Note that if we don't call either, we
  // have to enable interrupts ourselves.
  INTERRUPTS_OFF();

  // We can idle or sleep if some basic conditions are satisfied.  If not, we
  // stay awake and active.  Otherwise, we try to sleep or idle, in that order.
  if (emAfOkToIdleOrSleep()) {
#ifndef EMBER_AF_HAS_RX_ON_WHEN_IDLE_NETWORK
    // If the stack says we can nap, it means that we may sleep for some amount
    // of time.  Otherwise, we can't sleep at all, although we can try to idle.
    // Also skip to idle if the time since last wakeup<sleep backoff
    if (emberOkToNap()
        && ((halCommonGetInt32uMillisecondTick() - lastWakeupMs)
            > EMBER_AF_PLUGIN_IDLE_SLEEP_BACKOFF_SLEEP_MS)) {
      // If the stack says we can hiberate, it means we can sleep as long as we
      // want.  Otherwise, we cannot sleep longer than the duration to its next
      // event.  In either case, we will never sleep longer than the duration
      // to our next event.
      uint32_t durationMs = (emberOkToHibernate()
                             ? MAX_INT32U_VALUE
                             : emberMsToNextStackEvent());
      durationMs = emberMsToNextEvent(emAfEvents, durationMs);

      // If the sleep duration is below our minimum threshold, we don't bother
      // sleeping.  It takes time to shut everything down and bring everything
      // back up and, at some point, it becomes useless to sleep.  We also give
      // the application one last chance to stay awake if it wants.  If the
      // duration is long enough and the application says okay, we will sleep.
      // Otherwise, we will try to idle instead.
      if (EMBER_AF_PLUGIN_IDLE_SLEEP_MINIMUM_SLEEP_DURATION_MS <= durationMs
          && emberAfPluginIdleSleepOkToSleepCallback(durationMs)) {
        uint32_t attemptedDurationMs = durationMs;
        emberAfEepromShutdownCallback();
        emberStackPowerDown();
        // WARNING: The following block enables interrupts, so we will cease
        // to be atomic by the time it returns. One of
        // emberAfPluginIdleSleepRtosCallback() or halSleepForMilliseconds()
        // will enable interrupts.
        if (!emberAfPluginIdleSleepRtosCallback(&durationMs, true)) {
          halPowerDown();

#ifdef EMBER_ENABLE_EM4
          // Additional conditions must be implemented in emberAfOKToGoToEM4Callback()
          // such as emberOkToHibernate() or
          // (emberCurrentStackTasks() & EMBER_ASSOCIATING).
          if (!emberAfOKToGoToEM4Callback() || !emberOkToGoToLowPowerMode()) {
            halSleepForMilliseconds(&durationMs);
          } else {
            emberPluginEM4AboutToGoInEM4Callback(durationMs);
            halSleep(SLEEPMODE_HIBERNATE);
          }
#else
          halSleepForMilliseconds(&durationMs);
#endif // !EMBER_ENABLE_EM4

          halPowerUp();
        }
        emberStackPowerUp();
        emberAfEepromNoteInitializedStateCallback(false);
        emberAfPluginIdleSleepWakeUpCallback(attemptedDurationMs - durationMs);
        emberAfDebugPrintln("slept for %l ms", (attemptedDurationMs - durationMs));
        lastWakeupMs = halCommonGetInt32uMillisecondTick();
        return;
      }
    }
#endif // !EMBER_AF_HAS_RX_ON_WHEN_IDLE_NETWORK

#ifndef EMBER_NO_IDLE_SUPPORT
    // If we are here, it means we could not sleep, so we will try to idle
    // instead.  We give the application one last chance to stay active if it
    // wants.  Note that interrupts are still disabled at this point.
    if (emberAfPluginIdleSleepOkToIdleCallback()) {
      // We are going to try to idle.  Setting our task to idle will trigger a
      // check of every other task.  If the others are also idle, the device
      // will idle.  If not, it won't.  In either case, we set our task back to
      // active again.  This ensures that we are in control of idling and will
      // prevent the device from idling without our knowledge.
      // WARNING: The following function enables interrupts, so we will cease
      // to be atomic by the time it returns.
      bool idled = emberMarkTaskIdle(emAfTaskId);
      emberMarkTaskActive(emAfTaskId);
      if (idled) {
        emberAfPluginIdleSleepActiveCallback();
      }
      return;
    }
#endif // !EMBER_NO_IDLE_SUPPORT
  } // emAfOkToIdleOrSleep

  // If we are here, it means we did not sleep or idle.  Interrupts are still
  // disabled at this point, so we have to enable them again.
  INTERRUPTS_ON();
}

#else // !IDLE_AND_SLEEP_SUPPORTED

void emberAfPluginIdleSleepTickCallback(void)
{
}

#endif // IDLE_AND_SLEEP_SUPPORTED

// This is going to be called by af-main-soc
void emberAfCameBackFromEM4Callback(void)
{
  // This must call emberPluginEM4ComingBackFromEM4Callback to
  // connect the em4 plugin to the idle-sleep plugin.
#ifdef EMBER_ENABLE_EM4
  emberPluginEM4ComingBackFromEM4Callback();
#endif
}

// EMBER_NETWORK_INIT_PARENT_INFO_IN_TOKEN is not an optional macro, so it
// cannot be checked at the beginning. In general, we need to check for
// EMBER_NETWORK_INIT_PARENT_INFO_IN_TOKEN if we don't initialize
// it by default.
void emberAfPluginIdleSleepInitCallback(void)
{
#ifdef EMBER_ENABLE_EM4
  EmberNetworkInitStruct networkInitStruct = { EMBER_NETWORK_INIT_PARENT_INFO_IN_TOKEN };
  emberNetworkInit(&networkInitStruct);
#endif
}
