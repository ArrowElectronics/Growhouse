/** @file hal/micro/cortexm3/sleep.c
 *
 * @brief EM3XX micro specific sleep functions.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                       *80* -->
 */

#include PLATFORM_HEADER
#include "hal/micro/micro-common.h"
#include "hal/micro/micro.h"
#include "hal/micro/cortexm3/micro-common.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/mfg-token.h"
#ifdef MINIMAL_HAL
  #define BYPASS_MPU(blah) blah
  #define EMBER_WAKE_PORT_A 0
  #define EMBER_WAKE_PORT_B 0
  #define EMBER_WAKE_PORT_C 0
#else
  #include BOARD_HEADER
  #include "hal/micro/cortexm3/mpu.h"
  #include "hal/micro/cortexm3/wake-gpio.h"
#endif //defined(MINIMAL_HAL)
#include "hal/micro/cortexm3/diagnostic.h"

//These two macros were inspired by CMSIS/Include/core_cm3.h NVIC routines.
#define NVIC_GET_ENABLED_IRQ(IRQn) \
  ((NVIC->ISER[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F))) ? 1 : 0)

#define NVIC_GET_PENDING_IRQ(IRQn) \
  ((NVIC->ISPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F))) ? 1 : 0)


















































































static WakeEvents halInternalWakeEvent = { .eventflags = 0 };

WakeEvents halGetWakeInfo(void)
{
  return halInternalWakeEvent;
}

#ifndef CORTEXM3_EM35X_GEN4 // HW bug fixed in GEN4
//forceSleepTmrCnt is a flag indicating that the system timer module
//should force a fake value for SLEEPTMR_CNT whenever the timer is
//accessed.  For FogBugz 11909/11920 workaround.
bool forceSleepTmrCnt = false;
//wakeupSleepTmrCnt is a capturing of the SLEEPTMR_CNT immediately upon
//waking up from deep sleep.  This value is used to know when the counter
//has ticked forward.  For FogBugz 11909/11920 workaround.
uint32_t wakeupSleepTmrCnt = 0;
#endif//CORTEXM3_EM35X_GEN4 // HW bug fixed in GEN4

void halInternalSleep(SleepModes sleepMode)
{
  //Timer restoring always takes place during the wakeup sequence.  We save
  //the state here in case SLEEPMODE_NOTIMER is invoked, which would disable
  //the clocks.
  uint32_t SLEEPTMR_CLKEN_SAVED = CMHV->SLEEPTMRCLKEN;

  //SLEEPMODE_POWERDOWN and SLEEPMODE_POWERSAVE are deprecated.  Remap them
  //to their appropriate, new mode name.
  if (sleepMode == SLEEPMODE_POWERDOWN) {
    sleepMode = SLEEPMODE_MAINTAINTIMER;
  } else if (sleepMode == SLEEPMODE_POWERSAVE) {
    sleepMode = SLEEPMODE_WAKETIMER;
  } else if (sleepMode == SLEEPMODE_HIBERNATE) {
    sleepMode = SLEEPMODE_NOTIMER;
  } else {
    // MISRA requires ..else if.. to have terminating else.
  }

  //This code assumes all wake source registers are properly configured.
  //As such, it should be called from halSleep() which configues the
  //wake sources.

  //The parameter gpioWakeSel is a bitfield composite of the GPIO wake
  //sources derived from the 3-6 ports, indicating which of the 24-48 GPIO
  //are configured as a wake source.
  WakeMask gpioWakeSel  = (GPIO->WAKE[0] << 0);
  gpioWakeSel |= (GPIO->WAKE[1] << 8);
  gpioWakeSel |= (GPIO->WAKE[2] << 16);
#ifdef EMBER_WAKE_PORT_D
  gpioWakeSel |= (GPIO->WAKE[3] << 24);
#endif
#ifdef EMBER_WAKE_PORT_E
  gpioWakeSel |= ((uint64_t)GPIO->WAKE[4] << 32);
#endif
#ifdef EMBER_WAKE_PORT_F
  gpioWakeSel |= ((uint64_t)GPIO->WAKE[5] << 40);
#endif

  //PB2 is also CMHV_WAKESEL_SC1.  Set this wake source if PB2's GPIO wake is set.
  if ((GPIO->WAKE[1] & GPIO_WAKE_Px2) != 0U) {
    CMHV->WAKESEL |= CMHV_WAKESEL_SC1;
  }

  //PA2 is also CMHV_WAKESEL_SC2.  Set this wake source if PA2's GPIO wake is set.
  if ((GPIO->WAKE[0] & GPIO_WAKE_Px2) != 0U) {
    CMHV->WAKESEL |= CMHV_WAKESEL_SC2;
  }

  //The CMHV_WAKESEL_IRQD source can come from any pin based on IRQD's sel register.
  if ((gpioWakeSel & BIT(GPIO->IRQDSEL)) != 0U) {
    CMHV->WAKESEL |= CMHV_WAKESEL_IRQD;
  }

  halInternalWakeEvent.eventflags = 0; //clear old wake events

  switch (sleepMode) {
    case SLEEPMODE_NOTIMER:
      //The sleep timer clock sources (both RC and XTAL) are turned off.
      //Wakeup is possible from only GPIO.  System time is lost.
      //NOTE: Timer restoring always takes place during the wakeup sequence.
      CMHV->SLEEPTMRCLKEN = 0;
      goto deepSleepCore;

    case SLEEPMODE_WAKETIMER:
      //The sleep timer clock sources remain running.  The RC is always
      //running and the 32kHz XTAL depends on the board header.  Wakeup
      //is possible from both GPIO and the sleep timer.  System time
      //is maintained.  The sleep timer is assumed to be configured
      //properly for wake events.
      //NOTE: This mode assumes the caller has configured the *entire*
      //      sleep timer properly.

      if ((EVENT_SLEEPTMR->CFG & EVENT_SLEEPTMR_CFG_WRAP) != 0U) {
        CMHV->WAKESEL |= CMHV_WAKESEL_SLEEPTMRWRAP;
      }
      if ((EVENT_SLEEPTMR->CFG & EVENT_SLEEPTMR_CFG_CMPB) != 0U) {
        CMHV->WAKESEL |= CMHV_WAKESEL_SLEEPTMRCMPB;
      }
      if ((EVENT_SLEEPTMR->CFG & EVENT_SLEEPTMR_CFG_CMPA) != 0U) {
        CMHV->WAKESEL |= CMHV_WAKESEL_SLEEPTMRCMPA;
      }
    //fall into SLEEPMODE_MAINTAINTIMER's sleep code:

    case SLEEPMODE_MAINTAINTIMER:
      //The sleep timer clock sources remain running.  The RC is always
      //running and the 32kHz XTAL depends on the board header.  Wakeup
      //is possible from only GPIO.  System time is maintained.
      //NOTE: System time is maintained without any sleep timer interrupts
      //      because the hardware sleep timer counter is large enough
      //      to hold the entire count value and not need a RAM counter.

      ////////////////////////////////////////////////////////////////////////////
      // Core deep sleep code
      ////////////////////////////////////////////////////////////////////////////
      deepSleepCore:
      // Interrupts *must* be/stay disabled for DEEP SLEEP operation
      // INTERRUPTS_OFF will use BASEPRI to disable all interrupts except
      // fault handlers.
      INTERRUPTS_OFF();
      // This is the point of no return.  From here on out, only the interrupt
      // sources available in CMHV->WAKESEL will be captured and propagated across
      // deep sleep.
      //stick all our saved info onto stack since it's only temporary
      {
        bool restoreWatchdog = halInternalWatchDogEnabled();
        bool skipSleep = false;

        #if defined(CORTEXM3_EM35X_GEN4)
        // On the 358x we must configure the CMLV->RAMRETAIN register to set which
        // blocks of RAM are maintained across deepsleep. This register is
        // reset during deepsleep so we must do this every time.
        uint32_t ramRetainValue = (1 << ((RAM_SIZE_B - _UNRETAINED_RAM_SEGMENT_SIZE)
                                         / RAM_RETAIN_BLOCK_SIZE)) - 1;
        CMLV->RAMRETAIN = ramRetainValue;

          #ifndef RETAIN_BITS
        // All CORTEXM3_EM35X_GEN4 devices have the same RETAIN_BITS size.
        // In CMSIS Device definitions, CMLV->RAMRETAIN exists but
        // RETAIN_BITS was not generated for the CMLV group.
            #define RETAIN_BITS (16)
          #endif
        // If we're retaining less than half the RAM then try to save power
        if ((ramRetainValue >> (RETAIN_BITS / 2)) == 0) {
          // This only works on the newest dies so check for that here
          uint8_t *ptr = (uint8_t*)(FIB_BOTTOM | MFG_PART_DATA_LOCATION);
          if (ptr[2] >= 0x01) {
            CMHV->RAMTEST |= CMHV_RAMTEST_REGB;   // Disable the unused RAM regulator
          }
        }
        #endif

        // Only three register blocks keep power across deep sleep:
        //  CM_HV, GPIO, SLOW_TIMERS
        //
        // All other register blocks lose their state across deep sleep:
        //  BASEBAND, MAC, SECURITY, SERIAL, TMR1, TMR2, EVENT, CM_LV, RAM_CTRL,
        //  AUX_ADC, CAL_ADC, FLASH_CONTROL, ITM, DWT, FPB, NVIC, TPIU
        //
        // The sleep code will only save and restore registers where it is
        // meaningful and necessary to do so.  In most cases, there must still
        // be a powerup function to restore proper state.
        //
        // NOTE: halPowerUp() and halPowerDown() will always be called before
        // and after this function.  halPowerDown and halPowerUp should leave
        // the modules in a safe state and then restart the modules.
        // (For example, shutting down and restarting Timer1)
        //
        //----BASEBAND
        //        reinitialized by emberStackPowerUp()
        //----MAC
        //        reinitialized by emberStackPowerUp()
        //----SECURITY
        //        reinitialized by emberStackPowerUp()
        //----SERIAL
        //        reinitialized by halPowerUp() or similar
        //----TMR1
        //        reinitialized by halPowerUp() or similar
        //----TMR2
        //        reinitialized by halPowerUp() or similar
        //----EVENT
        //SRC or FLAG interrupts are not saved or restored
        //MISS interrupts are not saved or restored
        //MAC_RX_INT_MASK - reinitialized by emberStackPowerUp()
        //MAC_TX_INT_MASK - reinitialized by emberStackPowerUp()
        //MAC_TIMER_INT_MASK - reinitialized by emberStackPowerUp()
        //BB_INT_MASK - reinitialized by emberStackPowerUp()
        //SEC_INT_MASK - reinitialized by emberStackPowerUp()
        uint32_t INT_SLEEPTMRCFG_SAVED = EVENT_SLEEPTMR->CFG;
        uint32_t INT_MGMTCFG_SAVED = EVENT_MGMT->CFG;
        //INT_TIM1CFG - reinitialized by halPowerUp() or similar
        //INT_TIM2CFG - reinitialized by halPowerUp() or similar
        //INT_SC1CFG - reinitialized by halPowerUp() or similar
        //INT_SC2CFG - reinitialized by halPowerUp() or similar
        //INT_ADCCFG - reinitialized by halPowerUp() or similar
        uint32_t GPIO_INTCFGA_SAVED = EVENT_GPIO->CFGA;
        uint32_t GPIO_INTCFGB_SAVED = EVENT_GPIO->CFGB;
        uint32_t GPIO_INTCFGC_SAVED = EVENT_GPIO->CFGC;
        uint32_t GPIO_INTCFGD_SAVED = EVENT_GPIO->CFGD;
        //SC1_INTMODE - reinitialized by halPowerUp() or similar
        //SC2_INTMODE - reinitialized by halPowerUp() or similar
        //----CM_LV
        uint32_t OSC24M_BIASTRIM_SAVED = CMLV->OSC24MBIASTRIM;
        uint32_t OSCHF_TUNE_SAVED = CMLV->OSCHFTUNE;
        uint32_t DITHER_DIS_SAVED = CMLV->DITHERDIS;
        //CMLV->OSC24MCTRL - reinitialized by halPowerUp() or similar
        //CPU_CLKSEL  - reinitialized by halPowerUp() or similar
        //TMR1_CLK_SEL - reinitialized by halPowerUp() or similar
        //TMR2_CLK_SEL - reinitialized by halPowerUp() or similar
        uint32_t PCTRACE_SEL_SAVED = CMLV->PCTRACESEL;
        #ifdef _CMLV_PERIPHERALDISABLE_RESETVALUE
        //Not every chip has CMLV->PERIPHERALDISABLE
        uint32_t PERIPHERAL_DISABLE_SAVED = CMLV->PERIPHERALDISABLE;
        #endif
        //----RAM_CTRL
        #if !defined(CORTEXM3_EM35X_GEN4)
        uint32_t MEM_PROT_0_SAVED = MEM_PROT_0_REG;
        uint32_t MEM_PROT_1_SAVED = MEM_PROT_1_REG;
        uint32_t MEM_PROT_2_SAVED = MEM_PROT_2_REG;
        uint32_t MEM_PROT_3_SAVED = MEM_PROT_3_REG;
        uint32_t MEM_PROT_4_SAVED = MEM_PROT_4_REG;
        uint32_t MEM_PROT_5_SAVED = MEM_PROT_5_REG;
        uint32_t MEM_PROT_6_SAVED = MEM_PROT_6_REG;
        uint32_t MEM_PROT_7_SAVED = MEM_PROT_7_REG;
        uint32_t MEM_PROT_EN_SAVED = MEM_PROT_EN_REG;
        #endif //!defined(CORTEXM3_EM35X_GEN4)
        //----AUX_ADC
        //        reinitialized by halPowerUp() or similar
        //----CAL_ADC
        //        reinitialized by emberStackPowerUp()
        //----FLASH_CONTROL
        //        configured on the fly by the flash library
        //----ITM
        //        reinitialized by halPowerUp() or similar
        //----DWT
        //        not used by software on chip
        //----FPB
        //        not used by software on chip
        //----NVIC
        //ST_CSR - fixed, restored by cstartup when exiting deep sleep
        //ST_RVR - fixed, restored by cstartup when exiting deep sleep
        uint32_t NVIC_ISER_SAVED = NVIC->ISER[0]; //mask against wake sources
        //INT_PENDSET - used below when overlapping interrupts and wake sources
        //NVIC_IPR_3to0 - fixed, restored by cstartup when exiting deep sleep
        //NVIC_IPR_7to4 - fixed, restored by cstartup when exiting deep sleep
        //NVIC_IPR_11to8 - fixed, restored by cstartup when exiting deep sleep
        //NVIC_IPR_15to12 - fixed, restored by cstartup when exiting deep sleep
        //NVIC_IPR_19to16 - fixed, restored by cstartup when exiting deep sleep
        uint32_t SCB_VTOR_SAVED = SCB->VTOR;
        //SCS_CCR - fixed, restored by cstartup when exiting deep sleep
        //SCS_SHPR_7to4 - fixed, restored by cstartup when exiting deep sleep
        //SCS_SHPR_11to8 - fixed, restored by cstartup when exiting deep sleep
        //SCS_SHPR_15to12 - fixed, restored by cstartup when exiting deep sleep
        //SCS_SHCSR - fixed, restored by cstartup when exiting deep sleep
        //----TPIU
        //        reinitialized by halPowerUp() or similar

        //emDebugPowerDown() should have shutdown the DWT/ITM/TPIU already.

        //freeze input to the GPIO from LV (alternate output functions freeze)
        CMHV->EVENTCTRL = CMHV_EVENTCTRL_LVFREEZE;
        //record GPIO state for wake monitoring purposes
        //By having a snapshot of GPIO state, we can figure out after waking
        //up exactly which GPIO could have woken us up.
        //Reading the three IN registers is done separately to avoid warnings
        //about undefined order of volatile access.
        WakeEvents GPIO_IN_SAVED;
        GPIO_IN_SAVED.events.portA = GPIO->P[0].IN;
        GPIO_IN_SAVED.events.portB = GPIO->P[1].IN;
        GPIO_IN_SAVED.events.portC = GPIO->P[2].IN;
      #ifdef EMBER_MICRO_PORT_D_GPIO
        GPIO_IN_SAVED.events.portD = GPIO->P[3].IN;
      #endif
      #ifdef EMBER_MICRO_PORT_E_GPIO
        GPIO_IN_SAVED.events.portE = GPIO->P[4].IN;
      #endif
      #ifdef EMBER_MICRO_PORT_F_GPIO
        GPIO_IN_SAVED.events.portF = GPIO->P[5].IN;
      #endif
        //reset the power up events by writing 1 to all bits.
        CMHV->PWRUPEVENT = 0xFFFFFFFFU;



        //By clearing the events, the wake up event capturing is activated.
        //At this point we can safely check our interrupt flags since event
        //capturing is now overlapped.  Up to now, interrupts indicate
        //activity, after this point, powerup events indicate activity.
        //If any of the interrupt flags are set, that means we saw a wake event
        //sometime while entering sleep, so we need to skip over sleeping
        //
        //--possible interrupt sources for waking:
        //  IRQA, IRQB, IRQC, IRQD
        //  SleepTMR CMPA, CMPB, Wrap
        //  WAKE_CORE (DebugIsr)
        //
        #if defined(EMBER_MICRO_PORT_B_GPIO)
          #if EMBER_MICRO_PORT_B_GPIO & PB0
        //check for IRQA interrupt and if IRQA (PB0) is wake source
        if ( NVIC_GET_PENDING_IRQ(IRQA_IRQn)
             && (GPIO->WAKE[1] & GPIO_WAKE_Px0)
             && (CMHV->WAKESEL & CMHV_WAKESEL_GPIO)) {
          skipSleep = true;
          //log IRQA as a wake event
          halInternalWakeEvent.eventflags |= BIT(PORTB_PIN(0));



        }
          #endif
          #if EMBER_MICRO_PORT_B_GPIO & PB6
        //check for IRQB interrupt and if IRQB (PB6) is wake source
        if ( NVIC_GET_PENDING_IRQ(IRQB_IRQn)
             && (GPIO->WAKE[1] & PB6)
             && (CMHV->WAKESEL & CMHV_WAKESEL_GPIO)) {
          skipSleep = true;
          //log IRQB as a wake event
          halInternalWakeEvent.eventflags |= BIT(PORTB_PIN(6));



        }
          #endif
        #endif
        //check for IRQC interrupt and if IRQC (GPIO->IRQCSEL) is wake source
        if ( NVIC_GET_PENDING_IRQ(IRQC_IRQn)
             && (gpioWakeSel & BIT(GPIO->IRQCSEL))
             && (CMHV->WAKESEL & CMHV_WAKESEL_GPIO)) {
          skipSleep = true;
          //log IRQC as a wake event
          halInternalWakeEvent.eventflags |= BIT(GPIO->IRQCSEL);



        }
        //check for IRQD interrupt and if IRQD (GPIO->IRQDSEL) is wake source
        if ( NVIC_GET_PENDING_IRQ(IRQD_IRQn)
             && (gpioWakeSel & BIT(GPIO->IRQDSEL))
             && ((CMHV->WAKESEL & CMHV_WAKESEL_GPIO)
                 || (CMHV->WAKESEL & CMHV_WAKESEL_IRQD))) {
          skipSleep = true;
          //log IRQD as a wake event
          halInternalWakeEvent.eventflags |= BIT(GPIO->IRQDSEL);



        }
        //check for SleepTMR CMPA interrupt and if SleepTMR CMPA is wake source
        if ((EVENT_SLEEPTMR->FLAG & EVENT_SLEEPTMR_FLAG_CMPA) && (CMHV->WAKESEL & CMHV_WAKESEL_SLEEPTMRCMPA)) {
          skipSleep = true;
          //log SleepTMR CMPA as a wake event
          halInternalWakeEvent.events.internal.bits.TIMER_WAKE_COMPA = true;



        }
        //check for SleepTMR CMPB interrupt and if SleepTMR CMPB is wake source
        if ((EVENT_SLEEPTMR->FLAG & EVENT_SLEEPTMR_FLAG_CMPB) && (CMHV->WAKESEL & CMHV_WAKESEL_SLEEPTMRCMPB)) {
          skipSleep = true;
          //log SleepTMR CMPB as a wake event
          halInternalWakeEvent.events.internal.bits.TIMER_WAKE_COMPB = true;



        }
        //check for SleepTMR WRAP interrupt and if SleepTMR WRAP is wake source
        if ((EVENT_SLEEPTMR->FLAG & EVENT_SLEEPTMR_FLAG_WRAP) && (CMHV->WAKESEL & CMHV_WAKESEL_SLEEPTMRWRAP)) {
          skipSleep = true;
          //log SleepTMR WRAP as a wake event
          halInternalWakeEvent.events.internal.bits.TIMER_WAKE_WRAP = true;



        }
        //check for Debug interrupt and if WAKE_CORE is wake source
        if (NVIC_GET_PENDING_IRQ(DEBUG_IRQn)
            && (CMHV->WAKESEL & CMHV_WAKESEL_CORE)) {
          skipSleep = true;
          //log WAKE_CORE as a wake event
          halInternalWakeEvent.events.internal.bits.WAKE_CORE_B = true;



        }

        //only propagate across deep sleep the interrupts that are both
        //enabled and possible wake sources
        {
          uint32_t wakeSourceInterruptMask = 0;

          if ((GPIO->WAKE[1] & GPIO_WAKE_Px0 /*PB0*/) != 0U) {
            wakeSourceInterruptMask |= (1 << IRQA_IRQn);



          }
          if ((GPIO->WAKE[1] & GPIO_WAKE_Px6 /*PB6*/) != 0U) {
            wakeSourceInterruptMask |= (1 << IRQB_IRQn);



          }
          if ((gpioWakeSel & BIT(GPIO->IRQCSEL)) != 0U) {
            wakeSourceInterruptMask |= (1 << IRQC_IRQn);



          }
          if ((gpioWakeSel & BIT(GPIO->IRQDSEL)) != 0U) {
            wakeSourceInterruptMask |= (1 << IRQD_IRQn);



          }
          if ((CMHV->WAKESEL & CMHV_WAKESEL_SLEEPTMRCMPA)
              || (CMHV->WAKESEL & CMHV_WAKESEL_SLEEPTMRCMPB)
              || (CMHV->WAKESEL & CMHV_WAKESEL_SLEEPTMRWRAP)) {
            wakeSourceInterruptMask |= (1 << SLEEPTMR_IRQn);



          }
          if ((CMHV->WAKESEL & CMHV_WAKESEL_CORE) != 0U) {
            wakeSourceInterruptMask |= (1 << DEBUG_IRQn);



          }

          NVIC_ISER_SAVED &= wakeSourceInterruptMask;
        }

















        //disable watchdog while sleeping (since we can't reset it asleep)
        halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);

        //The chip is not allowed to enter a deep sleep mode (which could
        //cause a core reset cycle) while CSYSPWRUPREQ is set.  CSYSPWRUPREQ
        //indicates that the debugger is trying to access sections of the
        //chip that would get reset during deep sleep.  Therefore, a reset
        //cycle could very easily cause the debugger to error and we don't
        //want that.  While the power management state machine will stall
        //if CSYSPWRUPREQ is set (to avoid the situation just described),
        //in this stalled state the chip will not be responsive to wake
        //events.  To be sensitive to wake events, we must handle them in
        //software instead.  To accomplish this, we request that the
        //CSYSPWRUPACK be inhibited (which will indicate the debugger is not
        //connected).  But, we cannot induce deep sleep until CSYSPWRUPREQ/ACK
        //go low and these are under the debuggers control, so we must stall
        //and wait here.  If there is a wake event during this time, break
        //out and wake like normal.  If the ACK eventually clears,
        //we can proceed into deep sleep.  The CMHV->CSYSPWRUPACKINHIBIT
        //functionality will hold off the debugger (by holding off the ACK)
        //until we are safely past and out of deep sleep.  The power management
        //state machine then becomes responsible for clearing
        //CMHV->CSYSPWRUPACKINHIBIT and responding to a CSYSPWRUPREQ with a
        //CSYSPWRUPACK at the right/safe time.
        CMHV->CSYSPWRUPACKINHIBIT = _CMHV_CSYSPWRUPACKINHIBIT_CSYSPWRUPACKINHIBIT_MASK;
        {
          //Use a local copy of CMHV->WAKESEL to avoid warnings from the compiler
          //about order of volatile accesses
          uint32_t wakeSel = CMHV->WAKESEL;
          //stall until a wake event or CSYSPWRUPREQ/ACK clears
          while ((CMHV->CSYSPWRUPACKSTATUS) && (!(CMHV->PWRUPEVENT & wakeSel))) {
          }
          //if there was a wake event, allow CSYSPWRUPACK and skip sleep
          if ((CMHV->PWRUPEVENT & wakeSel) != 0U) {
            CMHV->CSYSPWRUPACKINHIBIT = _CMHV_CSYSPWRUPACKINHIBIT_RESETVALUE;
            skipSleep = true;
          }
        }




        if (!skipSleep) {



          //FogBugz 7283 states that we must switch to the OSCHF when entering
          //deep sleep since using the 24MHz XTAL could result in RAM
          //corruption.  This switch must occur at least 2*24MHz cycles before
          //sleeping.
          //
          //FogBugz 8858 states that we cannot go into deep-sleep when the
          //chip is clocked with the 24MHz XTAL with a duty cycle as low as
          //70/30 since this causes power_down generation timing to fail.
          //
          //Hekla-S-003_Digital_specification.doc#21 states:  "Note that
          //in Lipari there was a requirement to set the clock into OSCHF
          //mode prior to entering deep sleep. This was a limitation brought
          //about from an early version of the RAM IP used in Lipari and does
          //not apply to Hekla so that requirement is removed."  But since
          //continuing to switch the clock does not dramatically affect
          //behavior and we prefer to keep our code as common as we can
          //between 3xx chips, we still switch the clock for 358x/359.
          CMLV->OSC24MCTRL &= ~CMLV_OSC24MCTRL_SEL;
          //If DS12 needs to be forced regardless of state, clear
          //REGEN_DSLEEP here.  This is hugely dangerous and
          //should only be done in very controlled chip tests.
          SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;      //enable deep sleep

#ifdef BOOTLOADER_OPEN
          void bootloaderInternalSaveContextAndDeepSleep(void);

          // We don't have interrupts in the bootloader, so we can't just use
          // SCS_ICSR to trigger a PendSV. So we manually call a function that
          // pushes all registers on the stack and enters deep sleep via a WFI
          // instruction.
          //

          // NOTE: Only set the reset reason in the bootloader. See bugzid 14827
          // for more information.
          // Also note that we leave the resetReason intact after waking up and restoring
          // since resetReason is used in cstartup-iar.c during startup.
          halResetInfo.crash.resetReason    = RESET_BOOTLOADER_DEEPSLEEP;
          halResetInfo.crash.resetSignature = RESET_VALID_SIGNATURE;

          bootloaderInternalSaveContextAndDeepSleep();
          //I AM ASLEEP.  WHEN EXECUTION RESUMES, CSTARTUP WILL RESTORE TO HERE

          extern bool sleepTimerInterruptOccurred;

          // In normal non-bootloader sleep situations here's what would happen
          // after we wakeup from deep sleep:
          //
          // 1. The EVENT_SLEEPTMR->CFG register is restored (search for
          //    EVENT_SLEEPTMR->CFG below). We know that this will have the
          //    INT_SLEEPTMRCMPA bit set, because that's what we use for sleep.
          // 2. This "if(sleep int triggered)?" test is true:
          //      if( (EVENT_SLEEPTMR->CFG&INT_SLEEPTMRCMPA) &&
          //          (halInternalWakeEvent&BIT(CMPA_INTERNAL_WAKE_EVENT_BIT)) )
          //    And because it's true, the code below forces the sleep interrupt
          //    like this:
          //      EVENT_SLEEPTMR->FORCE = EVENT_SLEEPTMR_FORCE_CMPA;
          // 3. The halSleepTimerIsr would clear EVENT_SLEEPTMR->CFG and then set
          //    sleepTimerInterruptOccurred to true.
          //
          // But we're in the bootloader deep sleep case, so we don't have that
          // ISR (or any interrupts, actually!).
          //
          // So we'll handle it ourselves here by:
          // 1. Setting sleepTimerInterruptOccurred to true.
          // 2. Clearing the INT_SLEEPTMRCFG_SAVED variable so when it's
          //    restored to EVENT_SLEEPTMR->CFG the "if(sleep int triggered)?"
          //    will be false, and the deep sleep interrupt won't be forced.
          sleepTimerInterruptOccurred = true;

          INT_SLEEPTMRCFG_SAVED = 0;

          // Invalidate the reset reason.
          // Bugzid-14827: Only do this when in a bootloader because otherwise
          // we will overwrite heap data like packetbuffers.
          halResetInfo.crash.resetSignature = RESET_INVALID_SIGNATURE;
#elif defined(MINIMAL_HAL)
          //Call the context save restore function to trigger a context save.
          halInternalContextSaveRestore(true);
          //I AM ASLEEP.  WHEN EXECUTION RESUMES, CSTARTUP WILL RESTORE TO HERE
#else
#ifndef MINIMAL_HAL
          // NOTE: Only bootloader sets the reset reason to allow deep sleep
          // wakeup to know upon wake to stay in bootloader.  App does not
          // set this, because this RAM is used for heap data (bugzid 14827),
          // however app makes sure wakeup won't be fooled by happenstance
          // if the heap actually contains the bootloader's pattern.
          if ( halResetInfo.crash.resetReason == RESET_BOOTLOADER_DEEPSLEEP
               && halResetInfo.crash.resetSignature == RESET_VALID_SIGNATURE ) {
            // temporarily corrupt resetSignature so wakeup returns here
            // to app rather than thinking it's the bootloader that slept
            halResetInfo.crash.resetSignature = RESET_INVALID_SIGNATURE;
            // Hack repurpose skipSleep local as signal to post-wakeup code
            skipSleep = true; // Signal to wakeup code below to restore
          }
#endif//MINIMAL_HAL

          //Call the context save restore function to trigger a context save.
          halInternalContextSaveRestore(true);
          //I AM ASLEEP.  WHEN EXECUTION RESUMES, CSTARTUP WILL RESTORE TO HERE

#ifndef MINIMAL_HAL
          // If signalled above that we had to corrupt heap temporarily
          // during sleep, restore it now upon wakeup!
          if (skipSleep) {
            halResetInfo.crash.resetSignature = RESET_VALID_SIGNATURE;
            skipSleep = false; // Done with repurposed local, restore it too
          }
#endif//MINIMAL_HAL
#endif // BOOTLOADER_OPEN

         #ifndef CORTEXM3_EM35X_GEN4 // HW bug fixed in GEN4
          //Save the value of the SLEEPTMR_CNT register immediately after
          //waking up from a real sleep.  For FogBugz 11909/11920 workaround.
          wakeupSleepTmrCnt = SLEEPTMR_CNTH << 16;
          wakeupSleepTmrCnt |= SLEEPTMR_CNTL;
         #endif//CORTEXM3_EM35X_GEN4 // HW bug fixed in GEN4
        } else {
          //Record the fact that we skipped sleep
          halInternalWakeEvent.events.internal.bits.SleepSkipped = true;
          //If this was a true deep sleep, we would have executed cstartup and
          //PRIMASK would be set right now.  If we skipped sleep, PRIMASK is not
          //set so we explicitely set it to guarantee the powerup sequence
          //works cleanly and consistently with respect to interrupt
          //dispatching and enabling.
          _setPriMask();
        }








        #ifdef _CMLV_PERIPHERALDISABLE_RESETVALUE
        //Re-disabling peripherals immediately after sleep helps save
        //a little more power.
        CMLV->PERIPHERALDISABLE = PERIPHERAL_DISABLE_SAVED;
        #endif

        //Clear the interrupt flags for all wake sources.  This
        //is necessary because if we don't execute an actual deep sleep cycle
        //the interrupt flags will never be cleared.  By clearing the flags,
        //we always mimick a real deep sleep as closely as possible and
        //guard against any accidental interrupt triggering coming out
        //of deep sleep.  (The interrupt dispatch code coming out of sleep
        //is responsible for translating wake events into interrupt events,
        //and if we don't clear interrupt flags here it's possible for an
        //interrupt to trigger even if it wasn't the true wake event.)
        EVENT_SLEEPTMR->FLAG = (EVENT_SLEEPTMR_FLAG_CMPA
                                | EVENT_SLEEPTMR_FLAG_CMPB
                                | EVENT_SLEEPTMR_FLAG_WRAP);
        EVENT_GPIO->FLAG = (EVENT_GPIO_FLAG_IRQA
                            | EVENT_GPIO_FLAG_IRQB
                            | EVENT_GPIO_FLAG_IRQC
                            | EVENT_GPIO_FLAG_IRQD);

        //immediately restore the registers we saved before sleeping
        //so IRQ and SleepTMR capture can be reenabled as quickly as possible
        //this is safe because our global interrupts are still disabled
        //other registers will be restored later

        #if (!(defined(MINIMAL_HAL) || defined(BOOTLOADER_OPEN)))
        //The minimal HAL (and therefore the SuperPHY release) doesn't
        //use the MPU, and neither does app-bootloader-open.
        halInternalEnableMPU();
        #endif //MINIMAL_HAL

        CMHV->SLEEPTMRCLKEN = SLEEPTMR_CLKEN_SAVED;
        EVENT_SLEEPTMR->CFG = INT_SLEEPTMRCFG_SAVED;
        EVENT_MGMT->CFG = INT_MGMTCFG_SAVED;
        EVENT_GPIO->CFGA = GPIO_INTCFGA_SAVED;
        EVENT_GPIO->CFGB = GPIO_INTCFGB_SAVED;
        EVENT_GPIO->CFGC = GPIO_INTCFGC_SAVED;
        EVENT_GPIO->CFGD = GPIO_INTCFGD_SAVED;
        CMLV->OSC24MBIASTRIM = OSC24M_BIASTRIM_SAVED;
        CMLV->OSCHFTUNE = OSCHF_TUNE_SAVED;
        CMLV->DITHERDIS = DITHER_DIS_SAVED;
        CMLV->PCTRACESEL = PCTRACE_SEL_SAVED;
        #if !defined(CORTEXM3_EM35X_GEN4)
        MEM_PROT_0_REG = MEM_PROT_0_SAVED;
        MEM_PROT_1_REG = MEM_PROT_1_SAVED;
        MEM_PROT_2_REG = MEM_PROT_2_SAVED;
        MEM_PROT_3_REG = MEM_PROT_3_SAVED;
        MEM_PROT_4_REG = MEM_PROT_4_SAVED;
        MEM_PROT_5_REG = MEM_PROT_5_SAVED;
        MEM_PROT_6_REG = MEM_PROT_6_SAVED;
        MEM_PROT_7_REG = MEM_PROT_7_SAVED;
        MEM_PROT_EN_REG = MEM_PROT_EN_SAVED;
        #endif //!defined(CORTEXM3_EM35X_GEN4)
        NVIC->ISER[0] = NVIC_ISER_SAVED;
        SCB->VTOR = SCB_VTOR_SAVED;

        //WAKE_CORE/INT_DEBUG and INT_IRQx is cleared by INT_PENDCLR below
        NVIC->ICPR[0] = 0xFFFFFFFFU;

        //Now that we're awake, normal interrupts are operational again
        //Take a snapshot of the new GPIO state and the EVENT register to
        //record our wake event
        halInternalWakeEvent.events.portA = GPIO->P[0].IN;
        halInternalWakeEvent.events.portB = GPIO->P[1].IN;
        halInternalWakeEvent.events.portC = GPIO->P[2].IN;
      #ifdef EMBER_MICRO_PORT_D_GPIO
        halInternalWakeEvent.events.portD = GPIO->P[3].IN;
      #endif
      #ifdef EMBER_MICRO_PORT_E_GPIO
        halInternalWakeEvent.events.portE = GPIO->P[4].IN;
      #endif
      #ifdef EMBER_MICRO_PORT_F_GPIO
        halInternalWakeEvent.events.portF = GPIO->P[5].IN;
      #endif
        //Only operate on power up events that are also wake events.  Power
        //up events will always trigger like an interrupt flag, so we have
        //to check them against events that are enabled for waking. (This is
        //a two step process because we're accessing two volatile values.)
        uint64_t powerUpEvents = CMHV->PWRUPEVENT;
        powerUpEvents &= CMHV->WAKESEL;

        halInternalWakeEvent.eventflags ^= GPIO_IN_SAVED.eventflags;
        halInternalWakeEvent.eventflags &= gpioWakeSel;
        //CMHV_PWRUPEVENT_SC1 is PB2 which is bit 10
        halInternalWakeEvent.eventflags |= ((powerUpEvents & CMHV_PWRUPEVENT_SC1) ? 1 : 0) << ((1 * 8) + 2);
        //CMHV_PWRUPEVENT_SC2 is PA2 which is bit 2
        halInternalWakeEvent.eventflags |= ((powerUpEvents & CMHV_PWRUPEVENT_SC2) ? 1 : 0) << ((0 * 8) + 2);
        //CMHV_PWRUPEVENT_IRQD is chosen by GPIO->IRQDSEL
        halInternalWakeEvent.eventflags |= ((powerUpEvents & CMHV_PWRUPEVENT_IRQD) ? 1 : 0) << (GPIO->IRQDSEL);
        halInternalWakeEvent.eventflags |= ((powerUpEvents
                                             & (CMHV_PWRUPEVENT_CSYSPWRUPREQ
                                                | CMHV_PWRUPEVENT_CDBGPWRUPREQ
                                                | CMHV_PWRUPEVENT_WAKECORE
                                                | CMHV_PWRUPEVENT_SLEEPTMRWRAP
                                                | CMHV_PWRUPEVENT_SLEEPTMRCOMPB
                                                | CMHV_PWRUPEVENT_SLEEPTMRCOMPA))
                                            << INTERNAL_WAKE_EVENT_BIT_SHIFT);
        //at this point wake events are fully captured and interrupts have
        //taken over handling all new events






        //Bring limited interrupts back online.  INTERRUPTS_OFF will use
        //BASEPRI to disable all interrupts except fault handlers.
        //PRIMASK is still set though (global interrupt disable) so we need
        //to clear that next.
        INTERRUPTS_OFF();





        //Now that BASEPRI has taken control of interrupt enable/disable,
        //we can clear PRIMASK to reenable global interrupt operation.
        _clearPriMask();





        //wake events are saved and interrupts are back on track,
        //disable gpio freeze
        CMHV->EVENTCTRL = _CMHV_EVENTCTRL_RESETVALUE;

        //restart watchdog if it was running when we entered sleep
        //do this before dispatching interrupts while we still have tight
        //control of code execution
        if (restoreWatchdog) {
          halInternalEnableWatchDog();
        }





        //Pend any interrupts associated with deep sleep wake sources.  The
        //restoration of INT_CFGSET above and the changing of BASEPRI below
        //is responsible for proper dispatching of interrupts at the end of
        //halSleep.
        //
        //
        //The WAKE_CORE wake source triggers a Debug Interrupt.  If INT_DEBUG
        //interrupt is enabled and WAKE_CORE is a wake event, then pend the
        //Debug interrupt (using the wake_core bit).
        if ( NVIC_GET_ENABLED_IRQ(DEBUG_IRQn)
             && (halInternalWakeEvent.events.internal.bits.WAKE_CORE_B)) {
          CMHV->WAKECORE = CMHV_WAKECORE_WAKECOREFIELD;



        }
        //
        //
        //The SleepTMR CMPA is linked to a real ISR.  If the SleepTMR CMPA
        //interrupt is enabled and CMPA is a wake event, then pend the CMPA
        //interrupt (force the second level interrupt).
        if ((EVENT_SLEEPTMR->CFG & EVENT_SLEEPTMR_CFG_CMPA)
            && (halInternalWakeEvent.events.internal.bits.TIMER_WAKE_COMPA)) {
          EVENT_SLEEPTMR->FORCE = EVENT_SLEEPTMR_FORCE_CMPA;



        }
        //
        //The SleepTMR CMPB is linked to a real ISR.  If the SleepTMR CMPB
        //interrupt is enabled and CMPB is a wake event, then pend the CMPB
        //interrupt (force the second level interrupt).
        if ((EVENT_SLEEPTMR->CFG & EVENT_SLEEPTMR_CFG_CMPB)
            && (halInternalWakeEvent.events.internal.bits.TIMER_WAKE_COMPB)) {
          EVENT_SLEEPTMR->FORCE = EVENT_SLEEPTMR_FORCE_CMPB;



        }
        //
        //The SleepTMR WRAP is linked to a real ISR.  If the SleepTMR WRAP
        //interrupt is enabled and WRAP is a wake event, then pend the WRAP
        //interrupt (force the second level interrupt).
        if ((EVENT_SLEEPTMR->CFG & EVENT_SLEEPTMR_CFG_WRAP)
            && (halInternalWakeEvent.events.internal.bits.TIMER_WAKE_WRAP)) {
          EVENT_SLEEPTMR->FORCE = EVENT_SLEEPTMR_FORCE_WRAP;



        }
        //
        //
        //The four IRQs are linked to a real ISR.  If any of the four IRQs
        //triggered, then pend their ISR
        //
        //If the IRQA interrupt mode is enabled and IRQA (PB0) is wake
        //event, then pend the interrupt.
        if (((EVENT_GPIO->CFGA & EVENT_GPIO_CFGA_MOD) != 0)
            && (halInternalWakeEvent.eventflags & BIT(PORTB_PIN(0)))) {
          NVIC_SetPendingIRQ(IRQA_IRQn);



        }
        //If the IRQB interrupt mode is enabled and IRQB (PB6) is wake
        //event, then pend the interrupt.
        if (((EVENT_GPIO->CFGB & EVENT_GPIO_CFGB_MOD) != 0)
            && (halInternalWakeEvent.eventflags & BIT(PORTB_PIN(6)))) {
          NVIC_SetPendingIRQ(IRQB_IRQn);



        }
        //If the IRQC interrupt mode is enabled and IRQC (GPIO->IRQCSEL) is wake
        //event, then pend the interrupt.
        if (((EVENT_GPIO->CFGC & EVENT_GPIO_CFGC_MOD) != 0)
            && (halInternalWakeEvent.eventflags & BIT(GPIO->IRQCSEL))) {
          NVIC_SetPendingIRQ(IRQC_IRQn);



        }
        //If the IRQD interrupt mode is enabled and IRQD (GPIO->IRQDSEL) is wake
        //event, then pend the interrupt.
        if (((EVENT_GPIO->CFGD & EVENT_GPIO_CFGD_MOD) != 0)
            && (halInternalWakeEvent.eventflags & BIT(GPIO->IRQDSEL))) {
          NVIC_SetPendingIRQ(IRQD_IRQn);



        }

       #ifndef CORTEXM3_EM35X_GEN4 // HW bug fixed in GEN4
        //Due to FogBugz 11909/11920, SLEEPTMR_CNT may not have updated yet so
        //we must ensure that the CNT register updates before returning.  It's
        //only necessary to wait for the CNT to update when we've gone to
        //sleep, the SLEEPTMR is enabled, and the sleep mode used a timer.
        //This code could delay for up to 1ms, but will return as soon as it
        //can.  In the situation where the chip slept for a known amount of
        //time, this code will not delay and instead the system timer will
        //report a fake, but accurate time.
        if ((!skipSleep)
            && (SLEEPTMR_CFG & SLEEPTMR_ENABLE)
            && (CMHV->SLEEPTMRCLKEN & CMHV_SLEEPTMRCLKEN_CLK10KEN)
            && (sleepMode != SLEEPMODE_NOTIMER)) {
          uint32_t currSleepTmrCnt;

          #ifdef BUG11909_WORKAROUND_C
          //THIS WORKAROUND IS NOT PROVEN 100% RELIABLE.  THIS SHOULD NOT BE
          //USED UNTIL IT IS PROVEN PERFECTLY RELIABLE.
          //This workaround attempts to force the SLEEPTMR_CNT to tick sooner
          //than normal.  It does so by toggling between the clock sources
          //to get the CNT to increment.  There is a chance the SLEEPTMR_CNT
          //could become random doing this!
          {
            currSleepTmrCnt = SLEEPTMR_CNTH << 16;
            currSleepTmrCnt |= SLEEPTMR_CNTL;
            if (currSleepTmrCnt == wakeupSleepTmrCnt) {
              uint32_t GPIO_PCOUT_SAVED = GPIO_PCOUT;
              uint32_t GPIO_PCCFGH_SAVED = GPIO_PCCFGH;
              uint32_t SLEEPTMR_CFG_SAVED = SLEEPTMR_CFG;
              //It is not necessary to do anything with CMHV->SLEEPTMRCLKEN.
              GPIO_PCSET = PC7;
              SET_REG_FIELD(GPIO_PCCFGH, PC7_CFG, GPIOCFG_OUT);
              do {
                //Toggling between RC/XTAL will produce a clock edge
                //into the timer and cause CNT to increment.
                SLEEPTMR_CFG ^= SLEEPTMR_CLKSEL;
                currSleepTmrCnt = SLEEPTMR_CNTH << 16;
                currSleepTmrCnt |= SLEEPTMR_CNTL;
              } while (currSleepTmrCnt == wakeupSleepTmrCnt);
              GPIO_PCOUT = GPIO_PCOUT_SAVED;
              GPIO_PCCFGH = GPIO_PCCFGH_SAVED;
              SLEEPTMR_CFG = SLEEPTMR_CFG_SAVED;
              forceSleepTmrCnt = false;
            }
          }
          #endif //BUG11909_WORKAROUND_C

          //Knowing that halSleepTimerIsr is about to be taken (when
          //interrupts get enabled) tells us that the chip woke up due
          //to the timer and therefore sleepTmrArtificalCnt is valid
          //and needs to be forced.  This allows us to bypass delaying
          //for SLEEPTMR_CNT to tick forward.  For FogBugz 11909/11920
          //workaround.
          if ( NVIC_GET_ENABLED_IRQ(SLEEPTMR_IRQn)
               && NVIC_GET_PENDING_IRQ(SLEEPTMR_IRQn)) {
            //sleepTmrArtificalCnt was set before sleeping
            //by halSleepForQuarterSeconds
            forceSleepTmrCnt = true;
          } else {
            uint32_t ticks = MAC_TIMER_TICKS(1000); // 1 millisecond
            uint32_t beginTime;
            forceSleepTmrCnt = false;
            //It is possible to be in a situation where the SLEEPTMR is no
            //longer ticking (32k XTAL issues).  To guard against getting
            //stuck in this loop, use the MAC Timer to timeout after 1ms (since
            //that is the maximum time this loop would normally delay for).
            MAC_TIMER_CTRL |= MAC_TIMER_CTRL_MAC_TIMER_EN;
            beginTime = MAC_TIMER;
            do {
              currSleepTmrCnt = SLEEPTMR_CNTH << 16;
              currSleepTmrCnt |= SLEEPTMR_CNTL;
            } while ((currSleepTmrCnt == wakeupSleepTmrCnt)
                     && (((MAC_TIMER - beginTime) & MAC_TIMER_MAC_TIMER_MASK) < ticks));
          }
        }
       #endif//CORTEXM3_EM35X_GEN4 // HW bug fixed in GEN4
      }






      //Mark the wake events valid just before exiting
      halInternalWakeEvent.events.internal.bits.WakeInfoValid = true;

      //We are now reconfigured, appropriate ISRs are pended, and ready to go,
      //so enable interrupts!
      INTERRUPTS_ON();





      break; //and deep sleeping is done!

    case SLEEPMODE_IDLE:
      //Only the CPU is idled.  The rest of the chip continues runing
      //normally.  The chip will wake from any interrupt.
    {
      bool restoreWatchdog = halInternalWatchDogEnabled();
      //disable watchdog while sleeping (since we can't reset it asleep)
      halInternalDisableWatchDog(MICRO_DISABLE_WATCH_DOG_KEY);
      //Normal ATOMIC/INTERRUPTS_OFF/INTERRUPTS_ON uses the BASEPRI mask
      //to juggle priority levels so that the fault handlers can always
      //be serviced.  But, the WFI instruction is only capable of
      //working with the PRIMASK bit.  Therefore, we have to switch from
      //using BASEPRI to PRIMASK to keep interrupts disabled so that the
      //WFI can return on an interrupt
      //Globally disable interrupts with PRIMASK
      _setPriMask();
      //Bring the BASEPRI up to 0 to allow interrupts (but still disabled
      //with PRIMASK)
      INTERRUPTS_ON();
      //an internal function call is made here instead of injecting the
      //"WFI" assembly instruction because injecting assembly code will
      //cause the compiler's optimizer to reduce efficiency.
        #ifdef FLASH_ACCESS_FLASH_LP
      BYPASS_MPU(
        uint32_t FLASH_ACCESS_SAVED = FLASH_ACCESS;
        FLASH_ACCESS &= ~FLASH_ACCESS_FLASH_LP;
        halInternalIdleSleep();
        FLASH_ACCESS = FLASH_ACCESS_SAVED;
        )
        #else //FLASH_ACCESS_FLASH_LP
      halInternalIdleSleep();
        #endif //FLASH_ACCESS_FLASH_LP
      //restart watchdog if it was running when we entered sleep
      if (restoreWatchdog) {
        halInternalEnableWatchDog();
      }
      //The WFI instruction does not actually clear the PRIMASK bit, it
      //only allows the PRIMASK bit to be bypassed.  Therefore, we must
      //manually clear PRIMASK to reenable all interrupts.
      _clearPriMask();
    }
    break;

    default:
      //Oops!  Invalid sleepMode parameter.
      assert(0);
  }
}

void halSleepWithOptions(SleepModes sleepMode, WakeMask wakeMask)
{
  //configure all GPIO wake sources when given a valid wakeMask
  if (wakeMask != WAKE_MASK_INVALID) {
    GPIO->WAKE[0] = (wakeMask >> 0) & EMBER_MICRO_PORT_A_GPIO;
    GPIO->WAKE[1] = (wakeMask >> 8) & EMBER_MICRO_PORT_B_GPIO;
    GPIO->WAKE[2] = (wakeMask >> 16) & EMBER_MICRO_PORT_C_GPIO;

    #ifdef EMBER_MICRO_PORT_D_GPIO
    GPIO->WAKE[3] = (wakeMask >> 24) & EMBER_MICRO_PORT_D_GPIO;
    #endif
    #ifdef EMBER_MICRO_PORT_E_GPIO
    GPIO->WAKE[4] = (wakeMask >> 32) & EMBER_MICRO_PORT_E_GPIO;
    #endif
    #ifdef EMBER_MICRO_PORT_F_GPIO
    GPIO->WAKE[5] = (wakeMask >> 40) & EMBER_MICRO_PORT_F_GPIO;
    #endif
  }

  //use the defines found in the board file to choose our wakeup source(s)
  CMHV->WAKESEL = 0;  //start with no wake sources

  //if any of the GPIO wakeup monitor bits are set, enable the top level
  //GPIO wakeup monitor
  if ((GPIO->WAKE[0])
      || (GPIO->WAKE[1])
      || (GPIO->WAKE[2])
      #ifdef EMBER_MICRO_PORT_D_GPIO
      || (GPIO->WAKE[3])
      #endif
      #ifdef EMBER_MICRO_PORT_E_GPIO
      || (GPIO->WAKE[4])
      #endif
      #ifdef EMBER_MICRO_PORT_F_GPIO
      || (GPIO->WAKE[5])
      #endif
      ) {
    CMHV->WAKESEL |= CMHV_WAKESEL_GPIO;
  }

  //NOTE: The use of WAKE_CDBGPWRUPREQ should not be necessary since asserting
  //CDBGPWRUPREQ will bring the chip to DS0 where the debug components are
  //maintained but it's not necessary to completely wake to the running state.

  //always wakeup when the debugger attempts to access the chip
  CMHV->WAKESEL |= CMHV_WAKESEL_CSYSPWRUPREQ;

  //always wakeup when the debug channel attempts to access the chip
  CMHV->WAKESEL |= CMHV_WAKESEL_CORE;
  //the timer wakeup sources are enabled below in POWERSAVE, if needed

  //wake sources are configured so do the actual sleeping
  halInternalSleep(sleepMode);
}

void halSleep(SleepModes sleepMode)
{
  //configure all GPIO wake sources
  WakeMask gpioWakeBitMask  = (EMBER_WAKE_PORT_A << 0)
                              | (EMBER_WAKE_PORT_B << 8)
                              | (EMBER_WAKE_PORT_C << 16)
                            #ifdef EMBER_WAKE_PORT_D
                              | (EMBER_WAKE_PORT_D << 24)
                            #endif
                            #ifdef EMBER_WAKE_PORT_E
                              | ((uint64_t) EMBER_WAKE_PORT_E << 32)
                            #endif
                            #ifdef EMBER_WAKE_PORT_F
                              | ((uint64_t) EMBER_WAKE_PORT_F << 40)
                            #endif
  ;

  halSleepWithOptions(sleepMode, gpioWakeBitMask);
}
