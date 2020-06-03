/** @file hal/micro/cortexm3/em35x/board/dev0680.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2008-2011 by Ember Corporation. All rights reserved.   *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 * @note The file dev0680.h is intended to be copied, renamed, and
 * customized for customer-specific hardware.
 *
 * The file dev0680.h is the default BOARD_HEADER file used with the breakout
 * board of the development kit.
 *
 * The EM35x on a dev0680 BoB has the following example GPIO configuration.
 * This board file and the default HAL setup reflects this configuration.
 * - PA0 - SC2MOSI
 * - PA1 - SC2MISO
 * - PA2 - SC2SCLK
 * - PA3 - SC2nSSEL
 * - PA4 - PTI_EN
 * - PA5 - PTI_DATA
 * - PA6 - LED (on RCM), or Radio HoldOff
 * - PA7 - LED (on RCM)
 * - PB0 - Power Amplifier shutdown control
 * - PB1 - SC1TXD
 * - PB2 - SC1RXD
 * - PB3 - SC1nCTS
 * - PB4 - SC1nRTS
 * - PB5 - TEMP_SENSE
 * - PB6 - Button (IRQB fixed to PB6)
 * - PB7 - Buzzer (also used for DataFlash Enable)
 * - PC0 - JTAG (JRST)
 * - PC1 - Power Amplifier antenna select control
 * - PC2 - JTAG (JTDO) / SWO
 * - PC3 - JTAG (JTDI)
 * - PC4 - JTAG (JTMS)
 * - PC5 - LED (on BoB)
 * - PC6 - Button (IRQC pointed to PC6)
 * - PC7 - TEMP_EN
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

/** @name Custom Baud Rate Definitions
 *
 * The following define is used with defining a custom baud rate for the UART.
 * This define provides a simple hook into the definition of
 * the baud rates used with the UART.  The baudSettings[] array in uart.c
 * links the BAUD_* defines with the actual register values needed for
 * operating the UART.  The array baudSettings[] can be edited directly for a
 * custom baud rate or another entry (the register settings) can be provided
 * here with this define.
 */
//@{
/**
 * @brief This define is the register setting for generating a baud of
 * 921600.  Refer to the EM35x datasheet's discussion on UART baud rates for
 * the equation used to derive this value.
 */
#define EMBER_SERIAL_BAUD_CUSTOM  13
//@} //END OF CUSTOM BAUD DEFINITIONS

/** @name LED Definitions
 *
 * The following are used to aid in the abstraction with the LED
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The ::HalBoardLedPins enum values should always be used when manipulating the
 * state of LEDs, as they directly refer to the GPIOs to which the LEDs are
 * connected.
 *
 * \b Note: LEDs 0 and 1 are on the RCM.
 *
 * \b Note: LED 2 is on the breakout board (dev0680).
 *
 * \b Note: LED 3 simply redirects to LED 2.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED0 = PORTA_PIN(6),
  BOARDLED1 = PORTA_PIN(7),
  BOARDLED2 = PORTC_PIN(5),
  BOARDLED3 = BOARDLED2,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};

/** @} END OF LED DEFINITIONS  */

/** @name Button Definitions
 *
 * The following are used to aid in the abstraction with the Button
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The BUTTONn macros should always be used with manipulating the buttons
 * as they directly refer to the GPIOs to which the buttons are connected.
 *
 * @note The GPIO number must match the IRQ letter
 */
//@{
/**
 * @brief The actual GPIO BUTTON0 is connected to.  This define should
 * be used whenever referencing BUTTON0.
 */
#define BUTTON0             PORTB_PIN(6)
/**
 * @brief The GPIO input register for BUTTON0.
 */
#define BUTTON0_IN          GPIO_PBIN
/**
 * @brief Point the proper IRQ at the desired pin for BUTTON0.
 * @note IRQB is fixed and as such does not need any selection operation.
 */
#define BUTTON0_SEL()       do {} while (0)
/**
 * @brief The interrupt service routine for BUTTON0.
 */
#define BUTTON0_ISR         halIrqBIsr
/**
 * @brief The interrupt configuration register for BUTTON0.
 */
#define BUTTON0_INTCFG      GPIO_INTCFGB
/**
 * @brief The interrupt enable bit for BUTTON0.
 */
#define BUTTON0_INT_EN_BIT  INT_IRQB
/**
 * @brief The interrupt flag bit for BUTTON0.
 */
#define BUTTON0_FLAG_BIT    INT_IRQBFLAG
/**
 * @brief The missed interrupt bit for BUTTON0.
 */
#define BUTTON0_MISS_BIT    INT_MISSIRQB

/**
 * @brief The actual GPIO BUTTON1 is connected to.  This define should
 * be used whenever referencing BUTTON1.
 */
#define BUTTON1             PORTC_PIN(6)
/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_IN          GPIO_PCIN
/**
 * @brief Point the proper IRQ at the desired pin for BUTTON1.
 * @note For this board, IRQC is pointed at PC6
 */
#define BUTTON1_SEL()       do { GPIO_IRQCSEL = PORTC_PIN(6); } while (0)
/**
 * @brief The interrupt service routine for BUTTON1.
 */
#define BUTTON1_ISR         halIrqCIsr
/**
 * @brief The interrupt configuration register for BUTTON1.
 */
#define BUTTON1_INTCFG      GPIO_INTCFGC
/**
 * @brief The interrupt enable bit for BUTTON1.
 */
#define BUTTON1_INT_EN_BIT  INT_IRQC
/**
 * @brief The interrupt flag bit for BUTTON1.
 */
#define BUTTON1_FLAG_BIT    INT_IRQCFLAG
/**
 * @brief The missed interrupt bit for BUTTON1.
 */
#define BUTTON1_MISS_BIT    INT_MISSIRQC
//@} //END OF BUTTON DEFINITIONS

/** @name Radio HoldOff
 *
 * When ::RADIO_HOLDOFF is defined, the GPIO configuration
 * will be initially setup by halInit() to enable Radio HoldOff
 * support on the designated RHO_GPIO as an input, replacing use
 * of that pin's default configuration.
 *
 * @note This define will override other settings for the RHO_GPIO.
 *
 * @note Radio HoldOff can also be enabled/disabled at runtime
 * via halSetRadioHoldOff().
 * The ::RADIO_HOLDOFF definition just controls the default
 * configuration at boot.
 */
//@{

/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable Radio HoldOff support.
 */
//#define RADIO_HOLDOFF  // Configure Radio HoldOff at bootup
//@} //END OF RADIO HOLDOFF

/** @name Radio HoldOff Configuration Definitions
 *
 * The following are used to aid in the abstraction with Radio
 * HoldOff (RHO).  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The Radio HoldOff input GPIO is abstracted like BUTTON0/1.
 */
//@{

/**
 * @brief The actual GPIO used to control Radio HoldOff.
 *
 * @note If ::RHO_GPIO is not defined, then Radio HoldOff
 * support will not be built in even for runtime use.
 */
#define RHO_GPIO              PORTA_PIN(6)
/**
 * @brief The GPIO signal level to assert Radio HoldOff (1=high, 0=low).
 */
#define RHO_ASSERTED          1
/**
 * @brief The GPIO configuration register for Radio HoldOff.
 */
#define RHO_CFG               GPIO_PACFGH
/**
 * @brief The GPIO input register for Radio HoldOff.
 */
#define RHO_IN                GPIO_PAIN
/**
 * @brief The GPIO output register for Radio HoldOff.
 */
#define RHO_OUT               GPIO_PAOUT
/**
 * @brief Point the proper IRQ at the desired pin for Radio HoldOff.
 * Remember there may be other things that might want to use this IRQ.
 */
#define RHO_SEL()             do { GPIO_IRQDSEL = RHO_GPIO; } while (0)
/**
 * @brief The interrupt service routine for Radio HoldOff.
 * Remember there may be other things that might want to use this IRQ.
 */
#define RHO_ISR               halIrqDIsr
/**
 * @brief The interrupt configuration register for Radio HoldOff.
 */
#define RHO_INTCFG            GPIO_INTCFGD
/**
 * @brief The interrupt enable bit for Radio HoldOff.
 */
#define RHO_INT_EN_BIT        INT_IRQD
/**
 * @brief The interrupt flag bit for Radio HoldOff.
 */
#define RHO_FLAG_BIT          INT_IRQDFLAG
/**
 * @brief The missed interrupt bit for Radio HoldOff.
 */
#define RHO_MISS_BIT          INT_MISSIRQD

/** @brief Configuration of GPIO for Radio HoldOff operation
 */
#define PWRUP_CFG_DFL_RHO_FOR_RHO   GPIOCFG_IN_PUD
#define PWRUP_OUT_DFL_RHO_FOR_RHO   GPIOOUT_PULLDOWN /* Deassert */
#define PWRDN_CFG_DFL_RHO_FOR_RHO   GPIOCFG_IN_PUD
#define PWRDN_OUT_DFL_RHO_FOR_RHO   GPIOOUT_PULLDOWN /* Deassert */

/** @brief Configuration of GPIO for default behavior
 */
#define PWRUP_CFG_DFL_RHO_FOR_DFL   GPIOCFG_OUT
#define PWRUP_OUT_DFL_RHO_FOR_DFL   1 /* LED default off */
#define PWRDN_CFG_DFL_RHO_FOR_DFL   GPIOCFG_OUT
#define PWRDN_OUT_DFL_RHO_FOR_DFL   1 /* LED off */

/** @brief The following definitions are helpers for managing
 *  Radio HoldOff and should not be modified.
 */
#if     (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
// Initial bootup configuration is for Radio HoldOff
  #define PWRUP_CFG_DFL_RHO           PWRUP_CFG_DFL_RHO_FOR_RHO
  #define PWRUP_OUT_DFL_RHO           PWRUP_OUT_DFL_RHO_FOR_RHO
  #define PWRDN_CFG_DFL_RHO           PWRDN_CFG_DFL_RHO_FOR_RHO
  #define PWRDN_OUT_DFL_RHO           PWRDN_OUT_DFL_RHO_FOR_RHO
  #define halInternalInitRadioHoldOff() halSetRadioHoldOff(true)
#else//!(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
// Initial bootup configuration is for default
  #define PWRUP_CFG_DFL_RHO           PWRUP_CFG_DFL_RHO_FOR_DFL
  #define PWRUP_OUT_DFL_RHO           PWRUP_OUT_DFL_RHO_FOR_DFL
  #define PWRDN_CFG_DFL_RHO           PWRDN_CFG_DFL_RHO_FOR_DFL
  #define PWRDN_OUT_DFL_RHO           PWRDN_OUT_DFL_RHO_FOR_DFL
  #define halInternalInitRadioHoldOff() /* no-op */
#endif//(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))

#ifdef  RHO_GPIO

  #define ADJUST_GPIO_CONFIG_DFL_RHO(enableRadioHoldOff)  do {                                 \
    ATOMIC(       /* Must read-modify-write so to be safe, use ATOMIC() */                     \
      if (enableRadioHoldOff) {       /* Radio HoldOff */                                      \
      /* Actual register state */                                                              \
      /*halGpioSetConfig(RHO_CFG, PWRUP_CFG_DFL_RHO_FOR_RHO);*/                                \
      RHO_CFG = RHO_CFG                                                                        \
                & ~(0x000F                   << ((RHO_GPIO & 3) * 4))                          \
                | (PWRUP_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO & 3) * 4));                         \
      RHO_OUT = RHO_OUT                                                                        \
                & ~(0x0001                   << ((RHO_GPIO & 7)  ))                            \
                | (PWRUP_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO & 7)  ));                           \
      /* Shadow register state */                                                              \
      gpioCfgPowerUp[RHO_GPIO >> 2] = gpioCfgPowerUp[RHO_GPIO >> 2]                            \
                                      & ~(0x000F                   << ((RHO_GPIO & 3) * 4))    \
                                      | (PWRUP_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO & 3) * 4));   \
      gpioOutPowerUp[RHO_GPIO >> 3] = gpioOutPowerUp[RHO_GPIO >> 3]                            \
                                      & ~(0x0001                   << ((RHO_GPIO & 7)  ))      \
                                      | (PWRUP_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO & 7)  ));     \
      gpioCfgPowerDown[RHO_GPIO >> 2] = gpioCfgPowerDown[RHO_GPIO >> 2]                        \
                                        & ~(0x000F                   << ((RHO_GPIO & 3) * 4))  \
                                        | (PWRDN_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO & 3) * 4)); \
      gpioOutPowerDown[RHO_GPIO >> 3] = gpioOutPowerDown[RHO_GPIO >> 3]                        \
                                        & ~(0x0001                   << ((RHO_GPIO & 7)  ))    \
                                        | (PWRDN_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO & 7)  ));   \
      RHO_INTCFG  = (0 << GPIO_INTFILT_BIT)         /* 0 = no filter  */                       \
                    | (3 << GPIO_INTMOD_BIT);       /* 3 = both edges */                       \
    } else {         /* default */                                                             \
      /* Actual register state */                                                              \
      /*halGpioSetConfig(RHO_CFG, PWRUP_CFG_DFL_RHO_FOR_DFL);*/                                \
      RHO_CFG = RHO_CFG                                                                        \
                & ~(0x000F                   << ((RHO_GPIO & 3) * 4))                          \
                | (PWRUP_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO & 3) * 4));                         \
      RHO_OUT = RHO_OUT                                                                        \
                & ~(0x0001                   << ((RHO_GPIO & 7)  ))                            \
                | (PWRUP_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO & 7)  ));                           \
      /* Shadow register state */                                                              \
      gpioCfgPowerUp[RHO_GPIO >> 2] = gpioCfgPowerUp[RHO_GPIO >> 2]                            \
                                      & ~(0x000F                   << ((RHO_GPIO & 3) * 4))    \
                                      | (PWRUP_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO & 3) * 4));   \
      gpioOutPowerUp[RHO_GPIO >> 3] = gpioOutPowerUp[RHO_GPIO >> 3]                            \
                                      & ~(0x0001                   << ((RHO_GPIO & 7)  ))      \
                                      | (PWRUP_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO & 7)  ));     \
      gpioCfgPowerDown[RHO_GPIO >> 2] = gpioCfgPowerDown[RHO_GPIO >> 2]                        \
                                        & ~(0x000F                   << ((RHO_GPIO & 3) * 4))  \
                                        | (PWRDN_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO & 3) * 4)); \
      gpioOutPowerDown[RHO_GPIO >> 3] = gpioOutPowerDown[RHO_GPIO >> 3]                        \
                                        & ~(0x0001                   << ((RHO_GPIO & 7)  ))    \
                                        | (PWRDN_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO & 7)  ));   \
      RHO_INTCFG  = 0;         /* disabled */                                                  \
    }                                                                                          \
      RHO_SEL();       /* Point IRQ at the desired pin */                                      \
      ) } while (0)

#endif//RHO_GPIO
//@} //END OF RADIO HOLDOFF CONFIGURATION DEFINITIONS

/** @name Temperature sensor ADC channel
 *
 * Define the analog input channel connected to the LM-20 temperature sensor.
 * The scale factor compensates for different platform input ranges.
 * PB5/ADC0 must be an analog input.
 * PC7 must be an output and set to a high level to power the sensor.
 *
 *@{
 */
/**
 * @brief The analog input channel to use for the temperature sensor.
 */
#define TEMP_SENSOR_ADC_CHANNEL ADC_SOURCE_ADC0_VREF2
/**
 * @brief The scale factor to compensate for different input ranges.
 */
#define TEMP_SENSOR_SCALE_FACTOR 1
/** @} END OF TEMPERATURE SENSOR ADC CHANNEL DEFINITIONS */

/** @name Packet Trace
 *
 * When ::PACKET_TRACE is defined, ::GPIO_PACFGH will automatically be setup by
 * halInit() to enable Packet Trace support on PA4 and PA5,
 * in addition to the configuration specified below.
 *
 * @note This define will override any settings for PA4 and PA5.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable Packet Trace support on the breakout board (dev0680).
 */
#define PACKET_TRACE  // We do have PACKET_TRACE support
//@} //END OF PACKET TRACE DEFINITIONS

/** @name ENABLE_OSC32K
 *
 * When ENABLE_OSC32K is defined, halInit() will configure system
 * timekeeping to utilize the external 32.768 kHz crystal oscillator
 * rather than the internal 1 kHz RC oscillator.
 *
 * On initial powerup the 32.768 kHz crystal oscillator will take a little
 * while to start stable oscillation. This only happens on initial powerup,
 * not on wake-from-sleep, since the crystal usually stays running in deep
 * sleep mode.
 *
 * When ENABLE_OSC32K is defined the crystal oscillator is started as part of
 * halInit(). After the crystal is started we delay for
 * OSC32K_STARTUP_DELAY_MS (time in milliseconds).  This delay allows the
 * crystal oscillator to stabilize before we start using it for system timing.
 *
 * If you set OSC32K_STARTUP_DELAY_MS to less than the crystal's startup time:
 *   - The system timer won't produce a reliable one millisecond tick before
 *     the crystal is stable.
 *   - You may see some number of ticks of unknown period occur before the
 *     crystal is stable.
 *   - halInit() will complete and application code will begin running, but
 *     any events based on the system timer will not be accurate until the
 *     crystal is stable.
 *   - An unstable system timer will only affect the APIs in system-timer.h.
 *
 * Typical 32.768 kHz crystals measured by Ember take about 400 milliseconds
 * to stabilize. Be sure to characterize your particular crystal's stabilization
 * time since crystal behavior can vary.
 */
//@{
#define OSC32K_STARTUP_DELAY_MS (0)

#if OSC32K_STARTUP_DELAY_MS > MAX_INT16U_VALUE
#error "OSC32K_STARTUP_DELAY_MS must fit in 16 bits."
#endif

/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable 32.768 kHz XTAL oscillator on the RCM plugged
 * into the Breakout board (dev0680).
 * Default is to disable 32.768 kHz XTAL and use 1 kHz RC oscillator instead.
 */
//#define ENABLE_OSC32K  // Enable 32.768 kHz osc instead of 1 kHz RC osc
//@} //END OF ENABLE OSC32K DEFINITIONS

/** @name ENABLE_ALT_FUNCTION_REG_EN
 *
 * When ENABLE_ALT_FUNCTION_REG_EN is defined, halInit() will enable the REG_EN
 * alternate functionality on PA7.  REG_EN is the special signal provided
 * by the EM35x's internal power controller which can be used to tell an
 * external power regulator when the EM35x is in deep sleep or not and as such
 * signal the external regulator to cut power.  This signal will override all
 * GPIO configuration and use of PA7.  When the alternate functionality is
 * not enabled, PA7 can be operated as a standard GPIO.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the REG_EN alternate function on PA7.
 * Default is to not enable REG_EN functionality on PA7.
 */
//#define ENABLE_ALT_FUNCTION_REG_EN
//@} //END OF ENABLE_ALT_FUNCTION_REG_EN DEFINITIONS

/** @name EEPROM_USES_SHUTDOWN_CONTROL
 *
 * When EEPROM_USES_SHUTDOWN_CONTROL is defined, logic is enabled in the
 * EEPROM driver which drives PB7 high upon EEPROM initialization.  In
 * Ember reference designs, PB7 acts as an EEPROM enable pin and therefore
 * must be driven high in order to use the EEPROM.  This option is intended
 * to be enabled when running app-bootloader on designs based on current
 * Ember reference designs.
 */
//@{
/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the logic that drives PB7 high in the EEPROM driver.
 * Default is to leave this logic disabled.
 */
//#define EEPROM_USES_SHUTDOWN_CONTROL
//@} //END OF EEPROM_USES_SHUTDOWN_CONTROL DEFINITIONS

/** @name GPIO Configuration Definitions
 *
 * The following are used to specify the GPIO configuration to establish
 * when Powered (POWERUP_), and when Deep Sleeping (POWERDOWN_).  The reason
 * for separate Deep Sleep settings is to allow for a slightly different
 * configuration that minimizes power consumption during Deep Sleep.  For
 * example, inputs that might float could be pulled up or down, and output
 * states chosen with care, e.g. to turn off LEDs or other devices that might
 * consume power or be unnecessary when Deep Sleeping.
 */
//@{

/** @name Packet Trace Configuration Defines
 *
 * Provide the proper set of pin configuration for when the Packet
 * Trace is enabled (look above for the define which enables it).  When Packet
 * Trace is not enabled, leave the two PTI pins in their default configuration.
 * If Packet Trace is not being used, feel free to set the pin configurations
 * as desired.  The config shown here is simply the Power On Reset defaults.
 *@{
 */
/**
 * @brief Give the packet trace configuration a friendly name.
 */
#ifdef  PACKET_TRACE
  #define PWRUP_CFG_PTI_EN    GPIOCFG_OUT_ALT
  #define PWRUP_OUT_PTI_EN    0
  #define PWRDN_CFG_PTI_EN    GPIOCFG_IN_PUD
  #define PWRDN_OUT_PTI_EN    GPIOOUT_PULLDOWN
  #define PWRUP_CFG_PTI_DATA  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_PTI_DATA  1
  #define PWRDN_CFG_PTI_DATA  GPIOCFG_IN_PUD
  #define PWRDN_OUT_PTI_DATA  GPIOOUT_PULLUP
#else//!PACKET_TRACE
  #define PWRUP_CFG_PTI_EN    GPIOCFG_IN
  #define PWRUP_OUT_PTI_EN    0
  #define PWRDN_CFG_PTI_EN    GPIOCFG_IN
  #define PWRDN_OUT_PTI_EN    0
  #define PWRUP_CFG_PTI_DATA  GPIOCFG_IN
  #define PWRUP_OUT_PTI_DATA  0
  #define PWRDN_CFG_PTI_DATA  GPIOCFG_IN
  #define PWRDN_OUT_PTI_DATA  0
#endif//PACKET_TRACE
//@} END OF Packet Trace Configuration Defines

/** @name 32kHz Oscillator, Button 1 and Temp En Configuration Defines
 *
 * When using the 32kHz, configure PC6 and PC7 for analog for the XTAL.
 *
 * When not using the 32kHz, configure PC6 and PC7 for Button1 and TEMP_EN.
 *@{
 */
/**
 * @brief Give GPIO PC6 configuration a friendly name.
 */
#ifdef ENABLE_OSC32K
//Use OCS32K configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRDN_OUT_BUTTON1  0
#else
//Use Button1 configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_IN_PUD
  #define PWRUP_OUT_BUTTON1  GPIOOUT_PULLUP /* Button needs a pullup */
  #define PWRDN_CFG_BUTTON1  GPIOCFG_IN_PUD
  #define PWRDN_OUT_BUTTON1  GPIOOUT_PULLUP /* Button needs a pullup */
#endif

/**
 * @brief Give GPIO PC7 configuration a friendly name.
 */
#ifdef  ENABLE_OSC32K
  #define CFG_TEMPEN         GPIOCFG_ANALOG
#else//!ENABLE_OSC32K
  #define CFG_TEMPEN         GPIOCFG_OUT
#endif//ENABLE_OSC32K
//@} END OF 32kHz Oscillator, Button 1 and Temp En Configuration Defines

/** @name LED2 Defines
 * Configure the pin for LED2.
 *@{
 */
/**
 * @brief Give the LED2 configuration a friendly name.
 */
#define PWRUP_CFG_LED2  GPIOCFG_OUT
#define PWRUP_OUT_LED2  1  /* LED default off */
#define PWRDN_CFG_LED2  GPIOCFG_OUT
#define PWRDN_OUT_LED2  1  /* LED default off */
//@} END OF LED2 Configuration Defines

/** @name GPIO Configuration Macros
 *
 * These macros define the GPIO configuration and initial state of the output
 * registers for all the GPIO in the powerup and powerdown modes.
 *@{
 */

//Each pin has 4 cfg bits.  There are 3 ports with 2 cfg registers per
//port since the cfg register only holds 2 pins (16bits).  Therefore,
//the cfg arrays need to be 6 entries of 16bits.
extern uint16_t gpioCfgPowerUp[6];
extern uint16_t gpioCfgPowerDown[6];
//Each pin has 1 out bit.  There are 3 ports with 1 out register per
//port (8bits).  Therefore, the out arrays need to be 3 entries of 8bits.
extern uint8_t gpioOutPowerUp[3];
extern uint8_t gpioOutPowerDown[3];
//A single mask variable covers all GPIO.
extern GpioMaskType gpioRadioPowerBoardMask;

/**
 * @brief Define the mask for GPIO relevant to the radio in the context
 * of power state.  Each bit in the mask indicates the corresponding GPIO
 * which should be affected when invoking halStackRadioPowerUpBoard() or
 * halStackRadioPowerDownBoard().
 */
#define DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE() \
  GpioMaskType gpioRadioPowerBoardMask = 0

/**
 * @brief Initialize GPIO powerup configuration variables.
 */
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES() \
  uint16_t gpioCfgPowerUp[6] = {            \
    (_replace_PACFGL_),                     \
    (_replace_PACFGH_),                     \
    (_replace_PBCFGL_),                     \
    (_replace_PBCFGH_),                     \
    (_replace_PCCFGL_),                     \
    (_replace_PCCFGH_)                      \
  }

/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                        \
  uint8_t gpioOutPowerUp[3] = {                                            \
    ((0                  << PA0_BIT)                                       \
     | (0                  << PA1_BIT)                                     \
     | (0                  << PA2_BIT)                                     \
     |                      /* nSSEL is default idle high */               \
     (1                  << PA3_BIT)                                       \
     | (PWRUP_OUT_PTI_EN   << PA4_BIT)                                     \
     | (PWRUP_OUT_PTI_DATA << PA5_BIT)                                     \
     | (PWRUP_OUT_DFL_RHO  << PA6_BIT)                                     \
     |                      /* LED default off */                          \
     (1                  << PA7_BIT)),                                     \
    ((1                  << PB0_BIT)                                       \
     | (1                  << PB1_BIT)                       /* SC1TXD  */ \
     | (1                  << PB2_BIT)                       /* SC1RXD  */ \
     | (1                  << PB3_BIT)                       /* SC1nCTS */ \
     | (0                  << PB4_BIT)                       /* SC1nRTS */ \
     | (0                  << PB5_BIT)                                     \
     |                      /* PB6 has button needing a pullup */          \
     (GPIOOUT_PULLUP     << PB6_BIT)                                       \
     | (0                  << PB7_BIT)),                                   \
    ((1                  << PC0_BIT)                                       \
     | (0                  << PC1_BIT)                                     \
     | (1                  << PC2_BIT)                                     \
     | (0                  << PC3_BIT)                                     \
     | (0                  << PC4_BIT)                                     \
     | (PWRUP_OUT_LED2     << PC5_BIT)                                     \
     | (PWRUP_OUT_BUTTON1  << PC6_BIT)                                     \
     |                      /* Temp Sensor default on */                   \
     (1                  << PC7_BIT))                                      \
  }

/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                    \
  uint16_t gpioCfgPowerDown[6] = {                                               \
    ((GPIOCFG_IN_PUD     << PA0_CFG_BIT)                                         \
     | (GPIOCFG_IN_PUD     << PA1_CFG_BIT)                                       \
     | (GPIOCFG_IN_PUD     << PA2_CFG_BIT)                                       \
     | (GPIOCFG_OUT        << PA3_CFG_BIT)),                                     \
    ((PWRDN_CFG_PTI_EN   << PA4_CFG_BIT)                                         \
     | (PWRDN_CFG_PTI_DATA << PA5_CFG_BIT)                                       \
     | (PWRDN_CFG_DFL_RHO  << PA6_CFG_BIT)                                       \
     | (GPIOCFG_OUT        << PA7_CFG_BIT)),                                     \
    ((GPIOCFG_OUT        << PB0_CFG_BIT)                                         \
     | (GPIOCFG_OUT        << PB1_CFG_BIT)                         /* SC1TXD  */ \
     | (GPIOCFG_IN_PUD     << PB2_CFG_BIT)                         /* SC1RXD  */ \
     | (GPIOCFG_IN_PUD     << PB3_CFG_BIT)),                       /* SC1nCTS */ \
    ((GPIOCFG_OUT        << PB4_CFG_BIT)                           /* SC1nRTS */ \
     |                         /* disable analog for sleep */                    \
     (GPIOCFG_IN_PUD     << PB5_CFG_BIT)                                         \
     | (GPIOCFG_IN_PUD     << PB6_CFG_BIT)                                       \
     |                         /* need to use pulldown for sleep */              \
     (GPIOCFG_IN_PUD     << PB7_CFG_BIT)),                                       \
    ((GPIOCFG_IN_PUD     << PC0_CFG_BIT)                                         \
     | (GPIOCFG_OUT        << PC1_CFG_BIT)                                       \
     | (GPIOCFG_OUT        << PC2_CFG_BIT)                                       \
     | (GPIOCFG_IN_PUD     << PC3_CFG_BIT)),                                     \
    ((GPIOCFG_IN_PUD     << PC4_CFG_BIT)                                         \
     | (PWRDN_CFG_LED2     << PC5_CFG_BIT)                                       \
     | (PWRDN_CFG_BUTTON1  << PC6_CFG_BIT)                                       \
     | (CFG_TEMPEN         << PC7_CFG_BIT))                                      \
  }

/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                        \
  uint8_t gpioOutPowerDown[3] = {                                            \
    ((GPIOOUT_PULLUP     << PA0_BIT)                                         \
     | (GPIOOUT_PULLUP     << PA1_BIT)                                       \
     | (GPIOOUT_PULLUP     << PA2_BIT)                                       \
     |                        /* nSSEL is idle high */                       \
     (1                  << PA3_BIT)                                         \
     |                        /* enable is idle low */                       \
     (PWRDN_OUT_PTI_EN   << PA4_BIT)                                         \
     |                        /* data is idle high */                        \
     (PWRDN_OUT_PTI_DATA << PA5_BIT)                                         \
     | (PWRDN_OUT_DFL_RHO  << PA6_BIT)                                       \
     |                        /* LED off */                                  \
     (1                  << PA7_BIT)),                                       \
    ((0                  << PB0_BIT)                                         \
     | (GPIOOUT_PULLUP     << PB1_BIT)                         /* SC1TXD  */ \
     | (GPIOOUT_PULLUP     << PB2_BIT)                         /* SC1RXD  */ \
     | (GPIOOUT_PULLDOWN   << PB3_BIT)                         /* SC1nCTS */ \
     | (GPIOOUT_PULLUP     << PB4_BIT)                         /* SC1nRTS */ \
     |                        /* tempsense needs pulldown */                 \
     (GPIOOUT_PULLDOWN   << PB5_BIT)                                         \
     |                        /* PB6 has button needing a pullup */          \
     (GPIOOUT_PULLUP     << PB6_BIT)                                         \
     |                        /* buzzer needs pulldown for sleep */          \
     (GPIOOUT_PULLDOWN   << PB7_BIT)),                                       \
    ((GPIOOUT_PULLUP     << PC0_BIT)                                         \
     | (0                  << PC1_BIT)                                       \
     | (1                  << PC2_BIT)                                       \
     | (GPIOOUT_PULLDOWN   << PC3_BIT)                                       \
     | (GPIOOUT_PULLDOWN   << PC4_BIT)                                       \
     | (PWRDN_OUT_LED2     << PC5_BIT)                                       \
     | (PWRDN_OUT_BUTTON1  << PC6_BIT)                                       \
     |                        /* Temp Sensor off */                          \
     (0                  << PC7_BIT))                                        \
  }

/**
 * @brief Set powerup GPIO configuration registers.
 */
#define SET_POWERUP_GPIO_CFG_REGISTERS() \
  GPIO_PACFGL = gpioCfgPowerUp[0];       \
  GPIO_PACFGH = gpioCfgPowerUp[1];       \
  GPIO_PBCFGL = gpioCfgPowerUp[2];       \
  GPIO_PBCFGH = gpioCfgPowerUp[3];       \
  GPIO_PCCFGL = gpioCfgPowerUp[4];       \
  GPIO_PCCFGH = gpioCfgPowerUp[5];

/**
 * @brief Set powerup GPIO output registers.
 */
#define SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerUp[0];                \
  GPIO_PBOUT = gpioOutPowerUp[1];                \
  GPIO_PCOUT = gpioOutPowerUp[2];

/**
 * @brief Set powerdown GPIO configuration registers.
 */
#define SET_POWERDOWN_GPIO_CFG_REGISTERS() \
  GPIO_PACFGL = gpioCfgPowerDown[0];       \
  GPIO_PACFGH = gpioCfgPowerDown[1];       \
  GPIO_PBCFGL = gpioCfgPowerDown[2];       \
  GPIO_PBCFGH = gpioCfgPowerDown[3];       \
  GPIO_PCCFGL = gpioCfgPowerDown[4];       \
  GPIO_PCCFGH = gpioCfgPowerDown[5];

/**
 * @brief Set powerdown GPIO output registers.
 */
#define SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS() \
  GPIO_PAOUT = gpioOutPowerDown[0];                \
  GPIO_PBOUT = gpioOutPowerDown[1];                \
  GPIO_PCOUT = gpioOutPowerDown[2];

/**
 * @brief External regulator enable/disable macro.
 */
#ifdef ENABLE_ALT_FUNCTION_REG_EN
  #define CONFIGURE_EXTERNAL_REGULATOR_ENABLE()  GPIO_DBGCFG |= GPIO_EXTREGEN;
#else
  #define CONFIGURE_EXTERNAL_REGULATOR_ENABLE()  GPIO_DBGCFG &= ~GPIO_EXTREGEN;
#endif
//@} END OF GPIO Configuration Macros

/** @name GPIO Wake Source Definitions
 *
 * A convenient define that chooses if this external signal can
 * be used as source to wake from deep sleep.  Any change in the state of the
 * signal will wake up the CPU.
 */
//@{
/**
 * @brief true if this GPIO can wake the chip from deep sleep, false if not.
 */
#define WAKE_ON_PA0   false
#define WAKE_ON_PA1   false
#define WAKE_ON_PA2   false
#define WAKE_ON_PA3   false
#define WAKE_ON_PA4   false
#define WAKE_ON_PA5   false
#define WAKE_ON_PA6   false
#define WAKE_ON_PA7   false
#define WAKE_ON_PB0   false
#define WAKE_ON_PB1   false
#ifdef SLEEPY_EZSP_ASH  // SC1RXD
  #define WAKE_ON_PB2   true
#else
  #define WAKE_ON_PB2   false
#endif
#define WAKE_ON_PB3   false
#define WAKE_ON_PB4   false
#define WAKE_ON_PB5   false
#define WAKE_ON_PB6   true   //BUTTON0
#define WAKE_ON_PB7   false
#define WAKE_ON_PC0   false
#define WAKE_ON_PC1   false
#define WAKE_ON_PC2   false
#define WAKE_ON_PC3   false
#define WAKE_ON_PC4   false
#define WAKE_ON_PC5   false
#define WAKE_ON_PC6   true   //BUTTON1
#define WAKE_ON_PC7   false
//@} //END OF GPIO Wake Source Definitions

//@} //END OF GPIO Configuration Definitions

/** @name Board Specific Functions
 *
 * The following macros exist to aid in the initialization, power up from sleep,
 * and power down to sleep operations.  These macros are responsible for
 * either initializing directly, or calling initialization functions for any
 * peripherals that are specific to this board implementation.  These
 * macros are called from halInit, halPowerDown, and halPowerUp respectively.
 */
//@{
/**
 * @brief Initialize the board.  This function is called from ::halInit().
 */
#ifndef EZSP_ASH
  #define halInternalInitBoard()   \
  do {                             \
    halInternalPowerUpBoard();     \
    halInternalRestartUart();      \
    halInternalInitButton();       \
    halInternalInitRadioHoldOff(); \
  } while (0)
#else
  #define halInternalInitBoard()   \
  do {                             \
    halInternalPowerUpBoard();     \
    halInternalRestartUart();      \
    halInternalInitRadioHoldOff(); \
  } while (0)
#endif

/**
 * @brief Power down the board.  This function is called from
 * ::halPowerDown().
 */
#define halInternalPowerDownBoard()            \
  do {                                         \
    /* Board peripheral deactivation */        \
    /* halInternalSleepAdc(); */               \
    SET_POWERDOWN_GPIO_OUTPUT_DATA_REGISTERS() \
    SET_POWERDOWN_GPIO_CFG_REGISTERS()         \
  } while (0)

/**
 * @brief Power up the board.  This function is called from
 * ::halPowerUp().
 */
#define halInternalPowerUpBoard()                            \
  do {                                                       \
    SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS()                 \
    SET_POWERUP_GPIO_CFG_REGISTERS()                         \
    /*The radio GPIO should remain in the powerdown state */ \
    /*until the stack specifically powers them up. */        \
    halStackRadioPowerDownBoard();                           \
    CONFIGURE_EXTERNAL_REGULATOR_ENABLE()                    \
    /* Board peripheral reactivation */                      \
    halInternalInitAdc();                                    \
  } while (0)
//@} //END OF BOARD SPECIFIC FUNCTIONS

#endif //__BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
