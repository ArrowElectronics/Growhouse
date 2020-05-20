#include "led_driver.h"

#include "em_gpio.h"
#include "em_timer.h"
#include "em_cmu.h"

#include "hal-config-board.h"
#include "board_features.h"

#include "native_gecko.h"

// local variables
static uint16_t current_level;
static uint16_t start_level;
static uint16_t target_level;

static uint32_t level_transtime_ticks;
static uint32_t level_transtime_elapsed;
static uint8_t level_transitioning;

static uint16_t current_temperature;
static uint16_t start_temperature;
static uint16_t target_temperature;

static int16_t current_deltauv;
static int16_t start_deltauv;
static int16_t target_deltauv;

static uint32_t temp_transtime_ticks;
static uint32_t temp_transtime_elapsed;
static uint8_t temp_transitioning;

/* board specific route locations to map timer PWM outputs to right pins */
#include "led_routeloc.h"

/* LED polarity is active-low in those radio boards that share same pin for BUTTON/LED */
#ifdef FEATURE_LED_BUTTON_ON_SAME_PIN
#define LED_ACTIVE_LOW
#define LED_OFF_STATE 1
#define LED_ON_STATE  0
#else
#define LED_OFF_STATE 0
#define LED_ON_STATE  1
#endif

/* min and max usable values for the timer CC register */
#define MIN_BRIGHTNESS  0x0001
#define MAX_BRIGHTNESS  0xFFFE

// local functions

static void timer_init(void)
{
  TIMER_Init_TypeDef sTimerInit = TIMER_INIT_DEFAULT;
  TIMER_InitCC_TypeDef sInitCC = TIMER_INITCC_DEFAULT;

  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_TIMER1, true);

  sTimerInit.prescale = timerPrescale1;
  sTimerInit.clkSel = timerClkSelHFPerClk;
  sTimerInit.mode = timerModeUp;

  TIMER_Init(TIMER0, &sTimerInit);
  TIMER_Init(TIMER1, &sTimerInit);

  TIMER0->ROUTELOC0 = LED0_ROUTELOC | LED1_ROUTELOC;
  TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTEPEN_CC1PEN;

  sInitCC.mode = timerCCModePWM;
  TIMER_InitCC(TIMER0, 0, &sInitCC);
  TIMER_InitCC(TIMER0, 1, &sInitCC);

#ifdef LED_ACTIVE_LOW
  TIMER_CompareSet(TIMER0, 0, MAX_BRIGHTNESS);
  TIMER_CompareSet(TIMER0, 1, MAX_BRIGHTNESS);
#else
  TIMER_CompareSet(TIMER0, 0, MIN_BRIGHTNESS);
  TIMER_CompareSet(TIMER0, 1, MIN_BRIGHTNESS);
#endif

  TIMER_TopSet(TIMER0, 0xFFFF);

  TIMER_Enable(TIMER0, 1);

  NVIC_ClearPendingIRQ(TIMER0_IRQn);
  NVIC_EnableIRQ(TIMER0_IRQn);

  TIMER_TopSet(TIMER1, 0x9600);

  TIMER_Enable(TIMER1, 1);

  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);
}

static void timer_irq_enable(TIMER_TypeDef *timer, int enabled)
{
  if (enabled) {
    TIMER_IntEnable(timer, TIMER_IEN_OF); /* enable interrupt on overflow */
  } else {
    TIMER_IntDisable(timer, TIMER_IEN_OF); /* disable interrupt on overflow */
  }
}

void TIMER0_IRQHandler(void)
{
  static uint8_t sig_delay = 0;

  TIMER_IntClear(TIMER0, TIMER_IF_OF);

  if (level_transitioning == 0) {
    timer_irq_enable(TIMER0, 0);
    return;
  }

  level_transtime_elapsed++;

  if (level_transtime_elapsed >= level_transtime_ticks) {
    // transition complete
    level_transitioning = 0;
    current_level = target_level;
    sig_delay = 0;
    gecko_external_signal(EXT_SIGNAL_LED_LEVEL_CHANGED);
  } else {
    // calculate current PWM duty cycle based on elapsed transition time
    if (target_level >= start_level) {
      current_level = start_level + (target_level - start_level) * level_transtime_elapsed / level_transtime_ticks;
    } else {
      current_level = start_level - (start_level - target_level) * level_transtime_elapsed / level_transtime_ticks;
    }
    // when transition is ongoing, generate an event to application once every 100 iterations (~ 170ms)
    // the event is used to update display status and therefore the rate should not be too high
    if (sig_delay++ == 0) {
      gecko_external_signal(EXT_SIGNAL_LED_LEVEL_CHANGED);
    }
    if (sig_delay >= 100) {
      sig_delay = 0;
    }
  }

  TIMER_CompareSet(TIMER0, 0, current_level);
  TIMER_CompareSet(TIMER0, 1, current_level);
}

void TIMER1_IRQHandler(void)
{
  static uint8_t sig_delay = 0;

  TIMER_IntClear(TIMER1, TIMER_IF_OF);

  if (temp_transitioning == 0) {
    timer_irq_enable(TIMER1, 0);
    return;
  }

  temp_transtime_elapsed++;

  if (temp_transtime_elapsed >= temp_transtime_ticks) {
    // transition complete
    temp_transitioning = 0;
    current_temperature = target_temperature;
    current_deltauv = target_deltauv;
    sig_delay = 0;
    gecko_external_signal(EXT_SIGNAL_LED_TEMPERATURE_CHANGED);
  } else {
    // calculate current temperature based on elapsed transition time
    if (target_temperature >= start_temperature) {
      current_temperature = start_temperature + (target_temperature - start_temperature) * temp_transtime_elapsed / temp_transtime_ticks;
    } else {
      current_temperature = start_temperature - (start_temperature - target_temperature) * temp_transtime_elapsed / temp_transtime_ticks;
    }

    if (target_deltauv >= start_deltauv) {
      current_deltauv = start_deltauv + (target_deltauv - start_deltauv) * temp_transtime_elapsed / temp_transtime_ticks;
    } else {
      current_deltauv = start_deltauv - (start_deltauv - target_deltauv) * temp_transtime_elapsed / temp_transtime_ticks;
    }
    // when transition is ongoing, generate an event to application once every 100 iterations (~ 100ms)
    // the event is used to update display status and therefore the rate should not be too high
    if (sig_delay++ == 0) {
      gecko_external_signal(EXT_SIGNAL_LED_TEMPERATURE_CHANGED);
    }
    if (sig_delay >= 100) {
      sig_delay = 0;
    }
  }
}

// global functions
void LEDS_init(void)
{
  current_level = 0;
  start_level = 0;
  target_level = 0;

  // configure LED pins
  GPIO_PinModeSet(BSP_LED0_PORT, BSP_LED0_PIN, gpioModePushPull, LED_OFF_STATE);
  GPIO_PinModeSet(BSP_LED1_PORT, BSP_LED1_PIN, gpioModePushPull, LED_OFF_STATE);

  timer_init();
}

void LEDS_SetLevel(uint16_t level, uint16_t delay_ms)
{
  /* all LED adjustments go trough this function. The polarity is taken into account by
   * flipping the level setting for boards with active-low LED control */
#ifdef LED_ACTIVE_LOW
  level = 0xFFFF - level;
#endif

  /* for simplicity, LEDs are driven with PWM in all states, even 0% and 100%.
   * Therefore need to limit the min/max value used for the timer CC register */
  if (level < MIN_BRIGHTNESS) {
    level = MIN_BRIGHTNESS;
  } else if (level > MAX_BRIGHTNESS) {
    level = MAX_BRIGHTNESS;
  }

  // todo: irq disable?

  if (delay_ms == 0) {
    current_level = level;
    TIMER_CompareSet(TIMER0, 0, current_level);
    TIMER_CompareSet(TIMER0, 1, current_level);

    /* if a transition was in progress, cancel it */
    if (level_transitioning) {
      level_transitioning = 0;
      timer_irq_enable(TIMER0, 0);
    }

    gecko_external_signal(EXT_SIGNAL_LED_LEVEL_CHANGED);
    return;
  }

  // round delay up to nearest multiple of 10ms NOT NEEDED??
  if (delay_ms % 10) {
    delay_ms += 10 - delay_ms % 10;
  }

  // convert transition time from milliseconds to timer ticks
  level_transtime_ticks = delay_ms * 10 / 17; // 1.7 ms per timer iteration

  start_level = current_level;
  target_level = level;

  level_transtime_elapsed = 0;
  level_transitioning = 1;

  // enabling timer IRQ -> the PWM level is adjusted in timer interrupt
  // gradually until target level is reached.
  timer_irq_enable(TIMER0, 1);

  return;
}

void LEDS_SetTemperature(uint16_t temperature, int16_t deltauv, uint16_t delay_ms)
{
  if (temperature < MIN_TEMPERATURE) {
    temperature = MIN_TEMPERATURE;
  } else if (temperature > MAX_TEMPERATURE) {
    temperature = MAX_TEMPERATURE;
  }

  if (delay_ms == 0) {
    current_temperature = temperature;
    current_deltauv = deltauv;

    /* if a transition was in progress, cancel it */
    if (temp_transitioning) {
      temp_transitioning = 0;
      timer_irq_enable(TIMER1, 0);
    }

    gecko_external_signal(EXT_SIGNAL_LED_TEMPERATURE_CHANGED);
    return;
  }

  temp_transtime_ticks = delay_ms;

  start_temperature = current_temperature;
  target_temperature = temperature;

  start_deltauv = current_deltauv;
  target_deltauv = deltauv;

  temp_transtime_elapsed = 0;
  temp_transitioning = 1;

  // enabling timer IRQ -> the temperature is adjusted in timer interrupt
  // gradually until target level is reached.
  timer_irq_enable(TIMER1, 1);

  return;
}

void LEDS_SetState(int state)
{
  static int toggle = 0;

  switch (state) {
    case LED_STATE_OFF:
      LEDS_SetLevel(MIN_BRIGHTNESS, 0);
      break;
    case LED_STATE_ON:
      LEDS_SetLevel(MAX_BRIGHTNESS, 0);
      break;
    case LED_STATE_PROV:
      if (++toggle % 2) {
        LEDS_SetLevel(MIN_BRIGHTNESS, 0);
      } else {
        LEDS_SetLevel(MAX_BRIGHTNESS, 0);
      }
      break;

    default:
      break;
  }
}

uint16_t LEDS_GetLevel(void)
{
#ifdef LED_ACTIVE_LOW
  return(0xFFFF - current_level);
#else
  return(current_level);
#endif
}

uint16_t LEDS_GetTemperature(void)
{
  return(current_temperature);
}

uint16_t LEDS_GetDeltaUV(void)
{
  return(current_deltauv);
}
