// Copyright 2014 Silicon Laboratories, Inc.
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "hal/plugin/infrared-led/infrared-led-driver-emit.h"

//****************************************************************************
// Macro definitions

#define TIM_OC1M_LOW   (0x4 << _TIM_CCMR1_TIM_OC1M_SHIFT)
#define TIM_OC1M_HIGH  (0x5 << _TIM_CCMR1_TIM_OC1M_SHIFT)
#define TIM_OC1M_PWM   (0x6 << _TIM_CCMR1_TIM_OC1M_SHIFT)

// These values match the EM3x platforms.
#define COMPENSATE_MARK_TIME    (5)
#define COMPENSATE_SPACE_TIME   (6)
#define COMPENSATE_FRAME_TIME   (186)

//****************************************************************************
// Variables

HalInternalInfraredLedEmitHeader_t halInternalInfraredLedEmitHeader;
HalInternalInfraredLedEmitTiming_t halInternalInfraredLedEmitTimingBuf[HAL_INFRARED_LED_EMIT_TIMING_BUFFER_SIZE];
uint8_t halInternalInfraredLedEmitTimingCnt;
uint8_t halInternalInfraredLedEmitBitIdxBuf[HAL_INFRARED_LED_EMIT_BITS_BUFFER_SIZE];
HalInternalInfraredLedEmitFrame_t halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_FRAME_TYPE_COUNT];
uint8_t halInternalInfraredLedEmitToggleT;
uint8_t halInternalInfraredLedEmitToggleS;
uint8_t halInternalInfraredLedEmitToggleFirst;
HalInternalInfraredLedEmitFrame_t halInternalInfraredLedEmitToggleBuf[HAL_INFRARED_LED_EMIT_TOGGLE_SIZE];

static uint8_t toggleOfs;

//****************************************************************************
// Static functions

// Delay the number of microseconds specified by delay.
static void delayMicrosecondsInt32u(uint32_t delay)
{
  while (delay > 0xFFFF) {
    halCommonDelayMicroseconds(0xFFFF);
    delay -= 0xFFFF;
  }
  halCommonDelayMicroseconds(delay);
}

// initialize the timer
static void initialize(void)
{
  // IR LED is driven by PB6, controlled by timer 1 channel 1
  // Assume SYSCLK is running off of a 24 MHz crystal, so PCLK is 12 MHz
  // disable counter clock
  TIM1->CR1 &= ~TIM_CR1_TIM_CEN;
  // disable all capture/compare channels
  TIM1->CCER = 0x0;
  // buffer auto-reload (must be on for PWM mode 1) and set rest to default
  TIM1->CR1 = TIM_CR1_TIM_ARBE | _TIM_CR1_RESETVALUE;
  // disable slave mode, reset to default
  TIM1->SMCR = _TIM_SMCR_RESETVALUE;
  // set up channel as compare output with buffer
  TIM1->CCMR1 = TIM_OC1M_PWM | TIM_CCMR1_TIM_OC1BE;
  // enable compare channel
  TIM1->CCER = TIM_CCER_TIM_CC1E;
  // set prescaler to give us finest resolution (0.083 microseconds)
  // counter clock CK_CNT is CK_PSC / (2 ^ TIM_PSC)
  TIM1->PSC = 0;
  // zero the reload register, set later based on carrier period
  TIM1->ARR = 0;
  // zero the compare register, set later based on carrier duty cycle
  TIM1->CCR1 = 0;
  // disable interrupts
  EVENT_TIM1->CFG = _EVENT_TIM_CFG_RESETVALUE;
  // clear pending interrupts
  EVENT_TIM1->FLAG = _EVENT_TIM_FLAG_RESETVALUE;
  // force update of registers and re-initialize counter
  TIM1->EGR |= TIM_EGR_TIM_UG;
  // enable counter clock
  TIM1->CR1 |= TIM_CR1_TIM_CEN;
}

// Called at frame begin. Initializes the timer.
static void frameBegin()
{
  // Reset PB6
  GPIO->P[1].CLR = GPIO_P_CLR_Px6;
  // place PB6 output under control of GPIO_PBOUT
  SET_CMSIS_REG_FIELD(GPIO->P[1].CFGH, GPIO_P_CFGH_Px6, _GPIO_P_CFGH_Px6_OUT);
  // Setup timer
  if (halInternalInfraredLedEmitHeader.bUseCarrier) {
    uint32_t carrierPeriod = halInternalInfraredLedEmitHeader.wCarrierPeriod;
    TIM1->ARR = (carrierPeriod * 12 + 4) / 8 - 1;
  } else {
    // Run at the highest possible frequency
    TIM1->ARR = 1;
  }
  // 30% carrier duty
  TIM1->CCR1 = TIM1->ARR / 3;
  // force update of registers and re-initialize counter
  TIM1->EGR |= TIM_EGR_TIM_UG;
}

// Called at frame end. Turns off the timer.
static void frameEnd()
{
  // Reset PB6
  GPIO->P[1].CLR = GPIO_P_CLR_Px6;
  // place PB6 output under control of GPIO_PBOUT
  SET_CMSIS_REG_FIELD(GPIO->P[1].CFGH, GPIO_P_CFGH_Px6, _GPIO_P_CFGH_Px6_OUT);
  // Setup timer
  TIM1->CCR1 = 0;
  TIM1->ARR = 0;
  TIM1->EGR |= TIM_EGR_TIM_UG;
}

// Start to emit mark.
static void emitFrameMark()
{
  if (halInternalInfraredLedEmitHeader.bUseCarrier) {
    // force update of registers and re-initialize counter
    TIM1->EGR |= TIM_EGR_TIM_UG;
    // place PB6 output under control of timer
    SET_CMSIS_REG_FIELD(GPIO->P[1].CFGH, GPIO_P_CFGH_Px6, _GPIO_P_CFGH_Px6_OUT_ALT);
  } else {
    // Set PB6
    GPIO->P[1].SET = GPIO_P_SET_Px6;
    // place PB6 output under control of GPIO_PBOUT
    SET_CMSIS_REG_FIELD(GPIO->P[1].CFGH, GPIO_P_CFGH_Px6, _GPIO_P_CFGH_Px6_OUT);
  }
}

// Start to emit space.
static void emitFrameSpace()
{
  // Reset PB6
  GPIO->P[1].CLR = GPIO_P_CLR_Px6;
  // place PB6 output under control of GPIO_PBOUT
  SET_CMSIS_REG_FIELD(GPIO->P[1].CFGH, GPIO_P_CFGH_Px6, _GPIO_P_CFGH_Px6_OUT);
}

// Emit one frame according to the frame definition.
static void emitFrame(HalInternalInfraredLedEmitFrame_t Frame)
{
  uint8_t  *pBitIdxBuf;
  uint8_t uTimIndex;
  uint32_t delayMark;
  uint32_t delaySpace;
  uint32_t comp = 0;

  if (Frame.uBitCount == 0) {
    return;
  }
  pBitIdxBuf = Frame.pBitIdxBuf;
  if (halInternalInfraredLedEmitHeader.bUseCarrier) {
    // Avoid start of next period by reducing the mark one input resolution step.
    comp = 4;
  }
  frameBegin();
  for (; Frame.uBitCount > 0; Frame.uBitCount--) {
    uTimIndex = (pBitIdxBuf != 0) ? *pBitIdxBuf : 0;
    delayMark = halInternalInfraredLedEmitTimingBuf[uTimIndex].mark;
    delayMark *= 4;   // Convert to usec.
    delaySpace = halInternalInfraredLedEmitTimingBuf[uTimIndex].space;
    delaySpace *= 4;   // Convert to usec.
    // output mark
    if (delayMark > 0) {
      delayMark -= (delayMark >= comp) ? comp : delayMark;
      delayMark -= ((delayMark >= COMPENSATE_MARK_TIME)
                    ? COMPENSATE_MARK_TIME
                    : delayMark);
      emitFrameMark();
      delayMicrosecondsInt32u(delayMark);
    }
    // output space
    if (delaySpace > 0) {
      delaySpace += comp;
      delaySpace -= ((delaySpace >= COMPENSATE_SPACE_TIME)
                     ? COMPENSATE_SPACE_TIME
                     : delaySpace);
      if (Frame.uBitCount == 1) {
        // Adjust the length of the last space
        delaySpace -= ((delaySpace >= COMPENSATE_FRAME_TIME)
                       ? COMPENSATE_FRAME_TIME
                       : delaySpace);
      }
      emitFrameSpace();
      delayMicrosecondsInt32u(delaySpace);
    }
    if (pBitIdxBuf != 0) {
      pBitIdxBuf++;
    }
  }
  frameEnd();
}

// Get the repeate frame modified with a toggle frame if the toggle is active.
static HalInternalInfraredLedEmitFrame_t getRepeateFrameModifiedWithToggleFrame()
{
  HalInternalInfraredLedEmitFrame_t frame;
  HalInternalInfraredLedEmitFrame_t toggle;

  frame = halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_REPEAT_FRAME];
  if ((halInternalInfraredLedEmitToggleS > 0) && (toggleOfs > 0)) {
    int i;

    toggle = halInternalInfraredLedEmitToggleBuf[toggleOfs - 1];
    for (i = 0; i < toggle.uBitCount; i++) {
      frame.pBitIdxBuf[halInternalInfraredLedEmitToggleFirst + i] = toggle.pBitIdxBuf[i];
    }
  }

  return frame;
}

//****************************************************************************
// Global functions

// Clears all variables and hardware used to emit ir.
void halInternalInfraredLedEmitClr(void)
{
  uint8_t i;

  // Clear variables
  halInternalInfraredLedEmitHeader.bRepeatMode = 0;
  halInternalInfraredLedEmitHeader.bUseCarrier = 0;
  halInternalInfraredLedEmitHeader.bMarkSpace = 0;
  halInternalInfraredLedEmitHeader.uRepeatCount = 0;
  halInternalInfraredLedEmitHeader.wCarrierPeriod = 0;
  halInternalInfraredLedEmitTimingCnt = 0;
  for (i = 0; i < HAL_INFRARED_LED_EMIT_TIMING_BUFFER_SIZE; i++) {
    halInternalInfraredLedEmitTimingBuf[i].mark = 0;
    halInternalInfraredLedEmitTimingBuf[i].space = 0;
  }
  for (i = 0; i < 3; i++) {
    halInternalInfraredLedEmitFrameBuf[i].pBitIdxBuf = 0;
    halInternalInfraredLedEmitFrameBuf[i].uBitCount = 0;
  }
  halInternalInfraredLedEmitToggleT = 0;
  halInternalInfraredLedEmitToggleS = 0;
  // Clear hardware
  initialize();
}

// Emits one press frame - if present.
void halInternalInfraredLedEmitPress(void)
{
  HalInternalInfraredLedEmitFrame_t frame;

  if (halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME].uBitCount > 0) {
    frame = halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_PRESS_FRAME];
  } else {
    frame = getRepeateFrameModifiedWithToggleFrame();
  }
  emitFrame(frame);
}

// Emits one repeate frame - if present
void halInternalInfraredLedEmitRepeat(void)
{
  HalInternalInfraredLedEmitFrame_t frame;

  frame = getRepeateFrameModifiedWithToggleFrame();
  emitFrame(frame);
}

// Emits one release frame - if present
void halInternalInfraredLedEmitRelease(void)
{
  HalInternalInfraredLedEmitFrame_t frame;

  frame = halInternalInfraredLedEmitFrameBuf[HAL_INFRARED_LED_EMIT_RELEASE_FRAME];
  emitFrame(frame);
}

// Resets the toggle counter.
void halInternalInfraredLedEmitToggleReset()
{
  toggleOfs = 0;
}

// Steps the toggle counter to the next toggle.
// If it passes the maximum value, it will start from 0 again.
void halInternalInfraredLedEmitToggleStepToNext()
{
  if (halInternalInfraredLedEmitToggleS > 0) {
    toggleOfs++;
    if (toggleOfs > halInternalInfraredLedEmitToggleS) {
      toggleOfs = 0;
    }
  }
}
