// -----------------------------------------------------------------------------
/// @file coexistence.c
/// @brief Radio coexistence utilities
///
/// @author Silicon Laboratories Inc.
/// @version 1.0.0
///
/// @section License
/// <b>(C) Copyright 2017 Silicon Laboratories, http://www.silabs.com</b>
///
/// This file is licensed under the Silabs License Agreement. See the file
/// "Silabs_License_Agreement.txt" for details. Before using this software for
/// any purpose, you must agree to the terms of that agreement.
///
// -----------------------------------------------------------------------------
#include "coexistence/common/coexistence.h"
#include "coexistence-hal.h"

#define coexReqAndGntIrqShared() \
  (reqCfg.cb == &COEX_GNT_ISR)

typedef struct COEX_ReqArgs {
  /** PTA request arguments*/
  COEX_ReqState_t *reqState;
  COEX_Req_t coexReq;
  COEX_ReqCb_t cb;
} COEX_ReqArgs_t;

static void (*setCoexPowerStateCallbackPtr)(bool powerUp);
static void (*setCoexReqCallbackPtr)(COEX_ReqArgs_t *reqArgs);
static void setCoexReqCallback(COEX_ReqArgs_t *reqArgs);
static const COEX_HalCallbacks_t *coexHalCallbacks;
static COEX_RadioCallback_t coexRadioCallback;
static COEX_RandomDelayCallback_t coexRandomDelayCallback;
static void coexNotifyRadio(void);
static void coexUpdateReqIsr(void);

/** PTA radio hold off GPIO configuration */
static COEX_GpioHandle_t rhoHandle = NULL;

/** PTA grant GPIO configuration */
static COEX_GpioHandle_t gntHandle = NULL;

/** PTA request GPIO configuration */
static COEX_GpioHandle_t reqHandle = NULL;

/** PTA PWM request GPIO configuration */
static COEX_GpioHandle_t pwmReqHandle = NULL;

/** PTA priority GPIO configuration */
static COEX_GpioHandle_t priHandle = NULL;

typedef struct COEX_Cfg {
  /** PTA request states*/
  COEX_ReqState_t *reqHead;
  volatile COEX_Req_t combinedRequestState;
  volatile bool radioOn : 1;
  volatile bool requestDenied : 1;
  volatile bool updateGrantInProgress : 1;
  COEX_Options_t options;
} COEX_Cfg_t;

static void COEX_REQ_ISR(void);
static void COEX_GNT_ISR(void);
static void COEX_RHO_ISR(void);

static COEX_Cfg_t coexCfg;

static COEX_GpioConfig_t rhoCfg = {
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_DEASSERTED),
  .cb = &COEX_RHO_ISR
};

static COEX_GpioConfig_t gntCfg = {
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_DEFAULT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_ASSERTED
                                  | COEX_GPIO_OPTION_INT_DEASSERTED),
  .cb = &COEX_GNT_ISR
};

static COEX_GpioConfig_t reqCfg = {
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_INT_DEASSERTED
                                  | COEX_GPIO_OPTION_OUTPUT),
  .cb = &COEX_REQ_ISR
};

static COEX_GpioConfig_t pwmReqCfg = {
  .options = (COEX_GpioOptions_t)(COEX_GPIO_OPTION_OUTPUT
                                  | COEX_GPIO_OPTION_SHARED)
};

static COEX_GpioConfig_t priCfg = {
  .options = COEX_GPIO_OPTION_OUTPUT
};

static bool gntWasAsserted = false;

void COEX_SetHalCallbacks(const COEX_HalCallbacks_t *callbacks)
{
  coexHalCallbacks = callbacks;
}

void COEX_SetRandomDelayCallback(COEX_RandomDelayCallback_t callback)
{
  coexRandomDelayCallback = callback;
}

void COEX_SetRadioCallback(COEX_RadioCallback_t callback)
{
  coexRadioCallback = callback;
}

static bool isReqShared(void)
{
  return (reqCfg.options & COEX_GPIO_OPTION_SHARED) != 0U;
}

static void coexReqRandomBackoff(void)
{
  if (coexRandomDelayCallback != NULL) {
    (*coexRandomDelayCallback)(coexCfg.options
                               & COEX_OPTION_MAX_REQ_BACKOFF_MASK);
  }
}

static void setGpio(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  if (coexHalCallbacks->setGpio != NULL && gpioHandle != NULL) {
    (*coexHalCallbacks->setGpio)(gpioHandle, enabled);
  }
}

static bool isGpioInSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  if (coexHalCallbacks->isGpioInSet != NULL && gpioHandle != NULL) {
    return (*coexHalCallbacks->isGpioInSet)(gpioHandle);
  }
  return defaultValue;
}

static bool getGpioOut(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  if (coexHalCallbacks->isGpioOutSet != NULL && gpioHandle != NULL) {
    return (*coexHalCallbacks->isGpioOutSet)(gpioHandle);
  }
  return defaultValue;
}

static void enableGpioInt(COEX_GpioHandle_t gpioHandle, bool *wasAsserted)
{
  if (coexHalCallbacks->enableGpioInt != NULL && gpioHandle != NULL) {
    (*coexHalCallbacks->enableGpioInt)(gpioHandle, true, wasAsserted);
  }
}

static void disableGpioInt(COEX_GpioHandle_t gpioHandle)
{
  if (coexHalCallbacks->enableGpioInt != NULL && gpioHandle != NULL) {
    (*coexHalCallbacks->enableGpioInt)(gpioHandle, false, NULL);
  }
}

static void setGpioFlag(COEX_GpioHandle_t gpioHandle)
{
  if (coexHalCallbacks->setGpioFlag != NULL && gpioHandle != NULL) {
    (*coexHalCallbacks->setGpioFlag)(gpioHandle, true);
  }
}

static void clearGpioFlag(COEX_GpioHandle_t gpioHandle)
{
  if (coexHalCallbacks->setGpioFlag != NULL && gpioHandle != NULL) {
    (*coexHalCallbacks->setGpioFlag)(gpioHandle, false);
  }
}

static void configGpio(COEX_GpioHandle_t gpioHandle,
                       COEX_GpioHandle_t *handlePtr,
                       COEX_GpioConfig_t *config)
{
  if (coexHalCallbacks->configGpio != NULL) {
    if (*handlePtr != NULL && config->cb != NULL) {
      disableGpioInt(*handlePtr);
    }
    *handlePtr = gpioHandle;
    if (gpioHandle != NULL) {
      coexHalCallbacks->configGpio(gpioHandle, config);
    }
  }
}

static void coexEventCallback(COEX_Events_t events)
{
  if (coexRadioCallback != NULL) {
    if (getGpioOut(priHandle, false)) {
      events |= COEX_EVENT_PRIORITY_ASSERTED;
    }
    coexRadioCallback(events);
  }
}

static void toggleCoexReq(void)
{
  if ( ((coexCfg.options
         & COEX_OPTION_PULSE_REQ_ON_RHO_RELEASE) != 0U) // Pulse request on RHO release selected
       && getGpioOut(reqHandle, false)          // REQUESTing and
       && !isGpioInSet(rhoHandle, false)          // RHO not asserted and
       && !isGpioInSet(gntHandle, true)) {        // GRANT not asserted
    setGpio(reqHandle, false);
    setGpio(reqHandle, true);
  }
}

void COEX_UpdateGrant(void)
{
  if (coexCfg.updateGrantInProgress) {
    // Prevent this function from being called recursively
    return;
  } else {
    coexCfg.updateGrantInProgress = true;
  }
  if (getGpioOut(reqHandle, false)) {    // GRANT phase
    bool newGnt = isGpioInSet(gntHandle, false);   // Sample GPIO once, now
    if (newGnt != gntWasAsserted) {
      gntWasAsserted = newGnt;
      coexNotifyRadio();
      // Issue callbacks on GRANT assert or negate
      // These are not one-shot callbacks
      COEX_ReqState_t* reqPtr;
      COEX_Req_t newState = (newGnt ? COEX_REQCB_GRANTED : COEX_REQCB_NEGATED);
      for (reqPtr = coexCfg.reqHead; reqPtr != NULL; reqPtr = reqPtr->next) {
        if ((reqPtr->cb != NULL) && ((reqPtr->coexReq & newState) != 0U)) {
          (*reqPtr->cb)(newState);
        }
      }
      if (!newGnt) {
        coexEventCallback(COEX_EVENT_GRANT_RELEASED);

        // Do we need this to meet GRANT -> REQUEST timing?
        // On GNT deassertion, pulse REQUEST to keep us going.
        // Don't want to revert to REQUEST phase here but stay in GRANT phase.
        // This seems dangerous in that it could allow a peer to assert their
        // REQUEST causing a conflict/race.
        if ((coexCfg.options & COEX_OPTION_PULSE_REQ_ON_GNT_RELEASE) != 0U) {
          setGpio(reqHandle, false);
          setGpio(reqHandle, true);
        }
      }
    }
  } else {                          // REQUEST phase
    if (coexReqAndGntIrqShared() && isReqShared()) {
      // External REQUEST deasserted so we can assert ours
      disableGpioInt(reqHandle);   // This is a one-shot event
      coexReqRandomBackoff();
      coexUpdateReqIsr();
    } else {
      // Ignore GRANT changes unless we are REQUESTing
    }
  }
  coexCfg.updateGrantInProgress = false;
}

// Triggered on both GRANT edges
static void COEX_GNT_ISR(void)
{
  clearGpioFlag(gntHandle);
  COEX_UpdateGrant();
}

// This IRQ is triggered on the negate REQUEST edge,
// needed only when REQUEST signal is shared,
// and not piggybacking GNT and REQ on same IRQ.
static void COEX_REQ_ISR(void)
{
  // External REQUEST deasserted so we can assert ours
  //clearGpioFlag(reqHandle) // Covered within disableGpioInt(reqHandle)
  disableGpioInt(reqHandle); // This is a one-shot event
  coexReqRandomBackoff();
  coexUpdateReqIsr();
}

// Public API
bool COEX_SetRequest(COEX_ReqState_t *reqState,
                     COEX_Req_t coexReq,
                     COEX_ReqCb_t cb)
{
  if (setCoexReqCallbackPtr == NULL || reqState == NULL) {
    return false;
  }
  COEX_ReqArgs_t reqArgs;
  reqArgs.reqState = reqState;
  reqArgs.coexReq = coexReq;
  reqArgs.cb = cb;
  COEX_HAL_CallAtomic((COEX_AtomicCallback_t)setCoexReqCallbackPtr,
                      &reqArgs);
  return true;
}

static void updateReqList(COEX_ReqState_t *reqState)
{
  bool reqFound = false;
  COEX_ReqState_t** current;
  COEX_Req_t combinedRequestState = reqState->coexReq;
  for (current = &coexCfg.reqHead; *current != NULL; current = &((*current)->next)) {
    if (*current == reqState) {
      reqFound = true;
      if (reqState->coexReq == COEX_REQ_OFF) {
        // remove disabled request from the list, it has nothing to combine
        *current = (*current)->next;
        reqState->next = NULL;
        if (*current == NULL) {
          // break out the loop if the tail was deleted
          break;
        }
      }
    }
    combinedRequestState |= (*current)->coexReq;
  }
  if (!reqFound && (reqState->coexReq != COEX_REQ_OFF)) {
    // Insert new non-OFF entry at head of list
    reqState->next = coexCfg.reqHead;
    coexCfg.reqHead = reqState;
  }
  coexCfg.combinedRequestState = combinedRequestState;
}

static void setCoexReqCallback(COEX_ReqArgs_t *reqArgs)
{
  COEX_ReqState_t *reqState = reqArgs->reqState;
  COEX_Req_t coexReq = reqArgs->coexReq;

  if (((coexReq & COEX_REQ_ON) == 0U)
      && ((reqState->coexReq & COEX_REQ_ON) != 0U)
      && (reqState->cb != NULL)
      && ((reqState->coexReq & COEX_REQCB_OFF) != 0U)) {
    (*reqState->cb)(COEX_REQCB_OFF);
  }
  reqState->cb = reqArgs->cb;
  if (reqState->coexReq != coexReq) {
    reqState->coexReq = coexReq;
    updateReqList(reqState);
    coexUpdateReqIsr();
  }
}

static void coexRadioHoldOffPowerDown(void)
{
  coexCfg.radioOn = false;
  // When sleeping radio, no need to monitor RHO anymore
  disableGpioInt(rhoHandle); //clear RHO top level int enable
}

static void coexRadioHoldOffPowerUp(void)
{
  coexCfg.radioOn = true;
  if (rhoHandle != NULL) {
    // When waking radio, set up initial state and resume monitoring
    disableGpioInt(rhoHandle); //ensure RHO interrupt is off
    rhoCfg.cb(); // Manually call ISR to assess current state
    enableGpioInt(rhoHandle, NULL); //enable RHO interrupt
  }
}

void setCoexPowerState(bool powerUp)
{
  if (setCoexPowerStateCallbackPtr != NULL) {
    setCoexPowerStateCallbackPtr(powerUp);
  }
}

static void setCoexPowerStateCallback(bool powerUp)
{
  if (powerUp) {
    coexRadioHoldOffPowerUp();
  } else {
    coexRadioHoldOffPowerDown();
  }
}

static void COEX_RHO_ISR(void)
{
  // Ack interrupt before reading GPIO to avoid potential of missing int
  clearGpioFlag(rhoHandle);
  if (rhoHandle != NULL) {
    // Notify Radio land of state change
    toggleCoexReq();
    coexNotifyRadio();
  }
}

static bool (*enableCoexistenceCb)(void) = NULL;

static bool enableCoexistence(void)
{
  bool enabled = ((coexCfg.options & COEX_OPTION_COEX_ENABLED) != 0U);
  COEX_ReqState_t* reqPtr;

  if (enabled) {
    setCoexReqCallbackPtr = &setCoexReqCallback;
  }
  for (reqPtr = coexCfg.reqHead; reqPtr != NULL; reqPtr = reqPtr->next) {
    COEX_SetRequest(reqPtr, COEX_REQ_OFF, NULL);
  }
  if (!enabled) {
    setCoexReqCallbackPtr = NULL;
  }
  (*coexEventCallback)(enabled
                       ? COEX_EVENT_COEX_ENABLED
                       : COEX_EVENT_COEX_DISABLED);
  coexNotifyRadio();
  return true;
}

static bool (*enableRadioHoldOffCb)(void) = NULL;

static bool enableRadioHoldOff(void)
{
  bool enabled = ((coexCfg.options & COEX_OPTION_RHO_ENABLED) != 0U);

  // Configure GPIO as input and if pulling, pull it toward deasserted state
  if (!enabled) {
    disableGpioInt(rhoHandle);
  }
  if (coexCfg.radioOn || !enabled) {
    coexNotifyRadio(); //Notify Radio land of current state
  }
  if (coexCfg.radioOn && enabled) {
    enableGpioInt(rhoHandle, NULL);
  }
  return true;
}

static bool coexHoldOffActive(void)
{
  return ((coexCfg.options & COEX_OPTION_COEX_ENABLED) != 0U)
         && ((!getGpioOut(reqHandle, true)       // not REQUESTing or
              || !isGpioInSet(gntHandle, true) ) );    // REQUEST not GRANTed
}

static bool radioHoldOffActive(void)
{
  return ((coexCfg.options & COEX_OPTION_RHO_ENABLED) != 0U)
         && isGpioInSet(rhoHandle, false);
}

static void coexNotifyRadio(void)
{
  bool coexRho = coexHoldOffActive() || radioHoldOffActive();
  if (!coexRho) {
    coexCfg.requestDenied = false;
  }
  coexEventCallback(coexRho ? COEX_EVENT_HOLDOFF_ENABLED : COEX_EVENT_HOLDOFF_DISABLED);
}

bool COEX_IsEnabled(void)
{
  return (rhoHandle != NULL)
         || (gntHandle != NULL)
         || (reqHandle != NULL);
}

bool COEX_ConfigRadioHoldOff(COEX_GpioHandle_t gpioHandle)
{
  setCoexPowerStateCallbackPtr = &setCoexPowerStateCallback;

  // Register chip specific RHO interrupt
  configGpio(gpioHandle, &rhoHandle, &rhoCfg);
  enableRadioHoldOffCb = &enableRadioHoldOff;
  enableRadioHoldOff();

  return true;
}

bool COEX_ConfigPriority(COEX_GpioHandle_t gpioHandle)
{
  if ((coexCfg.options & COEX_OPTION_PRI_SHARED) != 0U) {
    priCfg.options |= COEX_GPIO_OPTION_SHARED;
  } else {
    priCfg.options &= ~COEX_GPIO_OPTION_SHARED;
  }
  configGpio(gpioHandle, &priHandle, &priCfg);
  return true;
}

bool COEX_ConfigPwmRequest(COEX_GpioHandle_t gpioHandle)
{
  configGpio(gpioHandle, &pwmReqHandle, &pwmReqCfg);
  return true;
}

bool COEX_ConfigGrant(COEX_GpioHandle_t gpioHandle)
{
  configGpio(gpioHandle, &gntHandle, &gntCfg);
  return true;
}

bool COEX_ConfigRequest(COEX_GpioHandle_t gpioHandle)
{
  if ((coexCfg.options & COEX_OPTION_REQ_SHARED) != 0U) {
    reqCfg.options |= COEX_GPIO_OPTION_SHARED;
  } else {
    reqCfg.options &= ~COEX_GPIO_OPTION_SHARED;
  }
  configGpio(gpioHandle, &reqHandle, &reqCfg);

  enableCoexistenceCb = &enableCoexistence;
  enableCoexistence();

  return true;
}

bool COEX_SetOptions(COEX_Options_t options)
{
  COEX_Options_t changedOptions = (COEX_Options_t)(coexCfg.options ^ options);

  coexCfg.options = options;
  if ((changedOptions & COEX_OPTION_RHO_ENABLED) != 0U) {
    if (enableRadioHoldOffCb == NULL) {
      return false;
    }
    enableRadioHoldOffCb();
  }
  if ((changedOptions & COEX_OPTION_COEX_ENABLED) != 0U) {
    if (enableCoexistenceCb == NULL) {
      return false;
    }
    enableCoexistenceCb();
  }
  return true;
}

COEX_Options_t COEX_GetOptions(void)
{
  return coexCfg.options;
}

#ifdef COEX_HAL_FAST_REQUEST
__STATIC_INLINE void COEX_SetPriorityAndRequest(bool priority)
{
  if (priority) {
    COEX_HAL_SetRequest();
    COEX_HAL_SetPriority();
  } else {
    COEX_HAL_SetRequest();
    COEX_HAL_ClearPriority();
  }
}
__STATIC_INLINE void COEX_ClearRequestAndPriority(void)
{
  COEX_HAL_ClearPriority();
  COEX_HAL_ClearRequest();
}
#else //!COEX_HAL_FAST_REQUEST
__STATIC_INLINE void COEX_SetPriorityAndRequest(bool priority)
{
  setGpio(reqHandle, true);
  setGpio(priHandle, priority);
}
__STATIC_INLINE void COEX_ClearRequestAndPriority(void)
{
  setGpio(priHandle, false);
  setGpio(reqHandle, false);
}
#endif //!COEX_HAL_FAST_REQUEST

#if defined(__ICCARM__) /* IAR */
#pragma optimize=speed
#elif __clang__
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("-Ofast")))
#endif
// Must be called with interrupts disabled
static void coexUpdateReqIsr(void)
{
  COEX_Req_t combinedReqState = coexCfg.combinedRequestState; // Local non-volatile flavor avoids warnings
  bool myReq = ((combinedReqState & (COEX_REQ_ON | COEX_REQ_PWM)) != 0U); // I need to REQUEST
  bool force = ((combinedReqState & COEX_REQ_FORCE) != 0U); // (ignoring others)
  bool exReq; // external requestor?

  if (getGpioOut(reqHandle, false)) {  // in GRANT phase
    exReq = false;                // ignore external requestors
  } else {                        // in REQUEST phase
    clearGpioFlag(reqHandle);  // Before sampling REQUEST, avoids race
    exReq = isReqShared() && isGpioInSet(reqHandle, false);
  }
  if (myReq) {                    // want to assert REQUEST
    if (force || !exReq) {        // can assert REQUEST
      if (!getGpioOut(reqHandle, false)) {
        // Assume request denied until request is granted
        coexCfg.requestDenied = true;
      }
      clearGpioFlag(gntHandle);
      enableGpioInt(gntHandle, &gntWasAsserted);
      bool highPriority = ((combinedReqState & COEX_REQ_HIPRI) != 0U);
      // If COEX_REQ_ON is not set this must be a PWM request
      // If request is not shared map PWM requests to the standard request
      if (((combinedReqState & COEX_REQ_ON) != 0U) || !isReqShared() ) {
        COEX_SetPriorityAndRequest(highPriority);
      } else {
        setGpio(priHandle, highPriority);
      }
      setGpio(pwmReqHandle, true);
      // Issue callbacks on REQUEST assertion
      // These are one-shot callbacks
      COEX_ReqState_t* reqPtr;
      for (reqPtr = coexCfg.reqHead; reqPtr != NULL; reqPtr = reqPtr->next) {
        if ((reqPtr->cb != NULL) && ((reqPtr->coexReq & COEX_REQCB_REQUESTED) != 0U)) {
          reqPtr->coexReq &= ~COEX_REQCB_REQUESTED;
          (*reqPtr->cb)(COEX_REQCB_REQUESTED);
        }
      }
      setGpioFlag(gntHandle); // Manually force GRANT check if missed/no edge
    } else {                      // must wait for REQUEST
      enableGpioInt(reqHandle, NULL);
    }
  } else {                        // negate REQUEST
    if (getGpioOut(reqHandle, false) ) {
      COEX_Events_t coexEvents = COEX_EVENT_REQUEST_RELEASED;
      if (coexCfg.requestDenied) {
        coexCfg.requestDenied = false;
        coexEvents |= COEX_EVENT_REQUEST_DENIED;
      }
      coexEventCallback(coexEvents);
    }
    setGpio(pwmReqHandle, false);
    COEX_ClearRequestAndPriority();
    disableGpioInt(gntHandle);
    disableGpioInt(reqHandle);
  }
  coexNotifyRadio(); // Reassess (assert) RHO
}

void COEX_InitHalConfigOptions(void)
{
  COEX_Options_t options = COEX_OPTION_NONE;

  #ifdef BSP_COEX_RHO_PORT
  options |= COEX_OPTION_RHO_ENABLED;
  #endif //BSP_COEX_RHO_PORT
  #if defined(BSP_COEX_REQ_PORT) || defined(BSP_COEX_GNT_PORT)
  options |= COEX_OPTION_COEX_ENABLED;
  #endif //defined(BSP_COEX_REQ_PORT) || defined(BSP_COEX_GNT_PORT)
  #if HAL_COEX_REQ_BACKOFF
  options |= (COEX_OPTION_MAX_REQ_BACKOFF_MASK
              & HAL_COEX_REQ_BACKOFF);
  #endif //HAL_COEX_REQ_BACKOFF
  #if HAL_COEX_REQ_SHARED
  options |= COEX_OPTION_REQ_SHARED;
  #endif //HAL_COEX_REQ_SHARED
  #if HAL_COEX_PRI_SHARED
  options |= COEX_OPTION_PRI_SHARED;
  #endif //HAL_COEX_PRI_SHARED
  #if HAL_COEX_TX_ABORT
  options |= COEX_OPTION_TX_ABORT;
  #endif //HAL_COEX_TX_ABORT
  COEX_SetOptions(options);
}
