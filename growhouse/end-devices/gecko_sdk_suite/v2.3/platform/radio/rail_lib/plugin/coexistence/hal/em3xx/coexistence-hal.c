// -----------------------------------------------------------------------------
/// @file
/// @brief Radio coexistence EM3XX utilities
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
#include "coexistence-hal.h"

#ifdef PTA_GNT_GPIO
static void coexGntSel(void)
{
  PTA_GNT_SEL();
}
static COEX_HAL_GpioConfig_t ptaGntCfg = {
  .gpio = PTA_GNT_GPIO,           // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_GNT_GPIOCFG,         // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = PTA_GNT_ASSERTED,   // 0 if negated logic; 1 if positive logic
  .flagBit = PTA_GNT_FLAG_BIT,    // INT_IRQnFLAG (n=A/B/C/D)
  .intMissBit = PTA_GNT_MISS_BIT, // INT_MISSIRQn (n=A/B/C/D)
  .intCfg = &(PTA_GNT_INTCFG),    // GPIO_INTCFGn (n=A/B/C/D)
  .intEnBit = PTA_GNT_INT_EN_BIT, // INT_IRQn     (n=A/B/C/D)
  .intSel = &coexGntSel           // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
};
#endif //PTA_GNT_GPIO

#ifdef PTA_REQ_GPIO
static void coexReqSel(void)
{
  PTA_REQ_SEL();
}
static COEX_HAL_GpioConfig_t ptaReqCfg = {
  .gpio = PTA_REQ_GPIO,           // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_REQ_GPIOCFG,         // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = PTA_REQ_ASSERTED,   // 0 if negated logic; 1 if positive logic
  .flagBit = PTA_REQ_FLAG_BIT,    // INT_IRQnFLAG (n=A/B/C/D)
  .intMissBit = PTA_REQ_MISS_BIT, // INT_MISSIRQn (n=A/B/C/D)
  .intCfg = &(PTA_REQ_INTCFG),    // GPIO_INTCFGn (n=A/B/C/D)
  .intEnBit = PTA_REQ_INT_EN_BIT, // INT_IRQn     (n=A/B/C/D)
  .intSel = &coexReqSel           // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
};
#endif //PTA_GNT_GPIO

#ifdef PTA_PRI_GPIO
static COEX_HAL_GpioConfig_t ptaPriCfg = {
  .gpio = PTA_PRI_GPIO,           // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_PRI_GPIOCFG,         // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = PTA_PRI_ASSERTED,   // 0 if negated logic; 1 if positive logic
};
#endif //PTA_PRI_GPIO

#ifdef RHO_GPIO
static void (*rhoCallback)(void) = NULL;
static void rhoSel(void)
{
  RHO_SEL();
}
static COEX_HAL_GpioConfig_t rhoCfg = {
  .gpio = RHO_GPIO,               // PORTx_PIN(y) (x=A/B/C, y=0-7)
  .mode = PTA_GPIOCFG_INPUT,      // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  .polarity = RHO_ASSERTED,       // 0 if negated logic; 1 if positive logic
  .flagBit = RHO_FLAG_BIT,        // INT_IRQnFLAG (n=A/B/C/D)
  .intMissBit = RHO_MISS_BIT,     // INT_MISSIRQn (n=A/B/C/D)
  .intCfg = &(RHO_INTCFG),        // GPIO_INTCFGn (n=A/B/C/D)
  .intEnBit = RHO_INT_EN_BIT,     // INT_IRQn     (n=A/B/C/D)
  .intSel = &rhoSel               // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
};
// RHO_ISR definition might interfere with other definitions.
void RHO_ISR(void)
{
  rhoCallback();
}
bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &RHO_ISR;
  status = COEX_ConfigRadioHoldOff(gpioConfig);
  if (status) {
    rhoCallback = gpioConfig->config.cb;
  }
  return status;
}
#else //!RHO_ISR
bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig)
{
  return false;
}
#endif //RHO_ISR

static void (*reqCallback)(void) = NULL;
static void (*gntCallback)(void) = NULL;

void PTA_REQ_ISR(void)
{
  reqCallback();
}

void PTA_GNT_ISR(void)
{
  gntCallback();
}

static void setGpioFlag(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

  if (enabled) {
    NVIC_SetPendingIRQ((IRQn_Type)gpio->intEnBit);
  } else {
    EVENT_MISS->MISS = gpio->intMissBit;
    EVENT_GPIO->FLAG = gpio->flagBit;
  }
}

static void setInterruptConfig(COEX_GpioHandle_t gpioHandle)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
  COEX_GpioConfig_t *coexGpio = &(gpio->config);
  uint32_t interruptEdges = EVENT_GPIO_CFGA_FILT_DEFAULT; /* default = no filter  */
  bool intAsserted = (coexGpio->options & COEX_GPIO_OPTION_INT_ASSERTED) != 0U;
  bool intDeasserted = (coexGpio->options & COEX_GPIO_OPTION_INT_DEASSERTED) != 0U;

  if (gpio->polarity ? intAsserted : intDeasserted) {
    interruptEdges |= EVENT_GPIO_CFGx_MOD_RISING_EDGE;
  }
  if (gpio->polarity ? intDeasserted : intAsserted) {
    interruptEdges |= EVENT_GPIO_CFGx_MOD_FALLING_EDGE;
  }
  gpio->interruptEdges |= interruptEdges;
}

static void enableGpioInt(COEX_GpioHandle_t gpioHandle,
                          bool enabled,
                          bool *wasAsserted)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

  // Disable triggering and clear any stale events
  *(gpio->intCfg) = 0;                  //disable triggering
  NVIC_DisableIRQ((IRQn_Type)gpio->intEnBit);      //clear top level int enable
  NVIC_ClearPendingIRQ((IRQn_Type)gpio->intEnBit); //clear any pended event
  setGpioFlag(gpio, false);
  if (enabled && gpio->isr != NULL) {
    *(gpio->intCfg) = gpio->interruptEdges;
    gpio->intSel(); //point IRQ to the desired pin
    NVIC_EnableIRQ((IRQn_Type)gpio->intEnBit);  //set top level interrupt enable
  }
}

static void setGpio(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

  if (enabled == gpio->polarity) {
    halGpioSet(gpio->gpio);
  } else {
    halGpioClear(gpio->gpio);
  }
}

static void configGpio(COEX_GpioHandle_t gpioHandle, COEX_GpioConfig_t *coexGpio)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
  bool defaultAsserted = (coexGpio->options & COEX_GPIO_OPTION_DEFAULT_ASSERTED) != 0U;
  gpio->config = *coexGpio;

  // Only configure GPIO if it was not set up prior
  if ((coexGpio->options & COEX_GPIO_OPTION_SHARED) != 0U) {
    gpio->mode = GPIO_P_CFGz_Pxy_OUT_OD;
  } else if ((coexGpio->options & COEX_GPIO_OPTION_OUTPUT) != 0U) {
    gpio->mode = GPIO_P_CFGz_Pxy_OUT;
  } else {
    gpio->mode = GPIO_P_CFGz_Pxy_IN_PUD;
  }
  halGpioSetConfig(gpio->gpio, (HalGpioCfg_t)gpio->mode);
  setGpio(gpio, defaultAsserted);
  setInterruptConfig(gpio);
}

static bool isGpioOutSet(COEX_GpioHandle_t gpioHandle)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
  return !!halGpioReadOutput(gpio->gpio) == !!gpio->polarity;
}

static bool isGpioInSet(COEX_GpioHandle_t gpioHandle)
{
  COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
  return !!halGpioRead(gpio->gpio) == !!gpio->polarity;
}

static const COEX_HalCallbacks_t coexHalCallbacks = {
  .setGpio = &setGpio,
  .setGpioFlag = &setGpioFlag,
  .enableGpioInt = &enableGpioInt,
  .configGpio = &configGpio,
  .isGpioOutSet = &isGpioOutSet,
  .isGpioInSet = &isGpioInSet
};

void COEX_HAL_CallAtomic(COEX_AtomicCallback_t cb, void *arg)
{
  ATOMIC((*cb)(arg); )
}

bool COEX_HAL_ConfigRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &PTA_REQ_ISR;
  status = COEX_ConfigRequest(gpioConfig);
  if (status) {
    reqCallback = gpioConfig->config.cb;
  }
  return status;
}

bool COEX_HAL_ConfigPriority(COEX_HAL_GpioConfig_t *gpioConfig)
{
  return COEX_ConfigPriority(gpioConfig);
}

bool COEX_HAL_ConfigGrant(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &PTA_GNT_ISR;
  status = COEX_ConfigGrant(gpioConfig);
  if (status) {
    gntCallback = gpioConfig->config.cb;
  }
  return status;
}

bool COEX_HAL_ConfigPwmRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  return COEX_ConfigPwmRequest(gpioConfig);
}

void COEX_HAL_Init(void)
{
  COEX_SetHalCallbacks(&coexHalCallbacks);
  COEX_Options_t options = COEX_OPTION_NONE;

  #ifdef RHO_GPIO
  options |= COEX_OPTION_RHO_ENABLED;
  #endif //RHO_GPIO
  #ifdef ENABLE_PTA
  options |= COEX_OPTION_COEX_ENABLED;
  #endif //ENABLE_PTA
  #ifdef PTA_REQ_MAX_BACKOFF_MASK
  options |= (COEX_OPTION_MAX_REQ_BACKOFF_MASK
              & PTA_REQ_MAX_BACKOFF_MASK);
  #endif //PTA_REQ_MAX_BACKOFF_MASK
  #if PTA_REQ_GPIOCFG == GPIOCFG_OUT_OD
  options |= COEX_OPTION_REQ_SHARED;
  #endif //PTA_REQ_GPIOCFG == GPIOCFG_OUT_OD
  #if PTA_PRI_GPIOCFG == GPIOCFG_OUT_OD
  options |= COEX_OPTION_PRI_SHARED;
  #endif //PTA_PRI_GPIOCFG == GPIOCFG_OUT_OD
  #ifdef ENABLE_PTA_OPT_ABORT_TX
  options |= COEX_OPTION_TX_ABORT;
  #endif //ENABLE_PTA_OPT_ABORT_TX
  COEX_SetOptions(options);

  #ifdef PTA_REQ_GPIO
  COEX_HAL_ConfigRequest(&ptaReqCfg);
  #endif //PTA_REQ_GPIO
  #ifdef PTA_PRI_GPIO
  COEX_HAL_ConfigPriority(&ptaPriCfg);
  #endif //PTA_PRI_GPIO
  #ifdef PTA_PWM_REQ_GPIO
  COEX_HAL_ConfigPwmRequest(&ptaPwmReqCfg);
  #endif //PTA_PWM_REQ_GPIO
  #ifdef PTA_GNT_GPIO
  COEX_HAL_ConfigGrant(&ptaGntCfg);
  #endif //PTA_GNT_GPIO
  #ifdef RHO_GPIO
  COEX_HAL_ConfigRadioHoldOff(&rhoCfg);
  #endif //RHO_GPIO
}
