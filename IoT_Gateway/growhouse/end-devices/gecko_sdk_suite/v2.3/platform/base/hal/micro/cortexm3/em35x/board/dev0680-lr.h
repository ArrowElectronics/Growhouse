/** @file hal/micro/cortexm3/em35x/board/dev0680-lr.h
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 * @note The file dev0680-lr.h is intended to be copied, renamed, and
 * customized for customer-specific hardware.
 *
 * The file dev0680-lr.h is the default BOARD_HEADER file used with the breakout
 * board of the development kit, with a modification to enable the FEM on Long Range
 * CEL modules.
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
 * - PB0 - Power Amplifier shutdown control / TRACEDATA2
 * - PB1 - SC1TXD
 * - PB2 - SC1RXD
 * - PB3 - SC1nCTS
 * - PB4 - SC1nRTS
 * - PB5 - TEMP_SENSE
 * - PB6 - Button (IRQB fixed to PB6)
 * - PB7 - Buzzer (also used for DataFlash Enable)
 * - PC0 - JTAG (JRST) / TRACEDATA1
 * - PC1 - Power Amplifier antenna select control / TRACEDATA3
 * - PC2 - JTAG (JTDO) / SWO / TRACEDATA0
 * - PC3 - JTAG (JTDI) / TRACECLK
 * - PC4 - JTAG (JTMS) / SWDIO
 * - PC5 - LED (on BoB)
 * - PC6 - Button (IRQC pointed to PC6)
 * - PC7 - TEMP_EN
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

/** @brief Application Framework NCP Configuration Board Header
 *
 * This board header (dev0680) is not supported in framework NCP applications.
 * NCP applications must use either the dev0680spi or dev0680uart board headers
 * when creating custom NCP applications through the framework.
 */
#if defined(EMBER_AF_NCP) \
  || defined(EZSP_SPI)    \
  || defined(EZSP_ASH)    \
  || defined(SLEEPY_EZSP_ASH)
  #error "NCP applications must use either the dev0680spi or dev0680uart board header."
#endif

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
 * be used whenever referencing BUTTON1, such as controlling if pieces
 * are compiled in.
 * Remember there may be other things that might want to use IRQC.
 */
#define BUTTON1             PORTC_PIN(6)

/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_IN          GPIO_PCIN

/**
 * @brief Point the proper IRQ at the desired pin for BUTTON1.
 * Remember there may be other things that might want to use IRQC.
 * @note For this board, IRQC is pointed at PC6
 */
#define BUTTON1_SEL()       do { GPIO_IRQCSEL = PORTC_PIN(6); } while (0)

/**
 * @brief The interrupt service routine for BUTTON1.
 * Remember there may be other things that might want to use IRQC.
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

/** @name USB Power State
 *
 * Define if the USB is self powered or bus powered since the configuration
 * descriptor needs to report to the host the powered state.
 *
 * @note VBUS Monitoring is required for USB to function when the EM358 device
 * is configured as self-powered.
 */
//@{

/**
 * @brief The USB power state.
 *
 * Set the define USB_SELFPWRD_STATE:
 *   0 if the device is bus powered.
 *   1 if the device self powered.
 */
#ifdef USB_BUSPWRD
#define USB_SELFPWRD_STATE (0)
#else
#define USB_SELFPWRD_STATE (1)
#endif
//@}

/** @name USB Remote Wakeup Enable
 *
 * If the USB device needs to awake the host from suspend, then it needs
 * to have remote wakeup enable.
 *
 * @note The host can deny remote wakeup, keeping the device in suspend.
 *
 * If the device has remote wakeup enabled the configuration descriptor
 * needs to report this fact to the host.  Additionally, the USB core
 * in the chip needs to be directly told.  Set the define
 * USB_REMOTEWKUPEN_STATE to 0 if remote wake is disabled or 1 if enabled.
 */
//@{

/**
 * @brief USB Remote Wakeup Enable
 *
 * Set the define USB_REMOTEWKUPEN_STATE:
 *   0 remote wakeup is disabled.
 *   1 remote wakeup is enabled.
 */
#define USB_REMOTEWKUPEN_STATE (1)
//@}

/** @name USB Maximum Power Consumption
 *
 * The USB device must report the maximum power it will draw from the bus.
 * This is done via the bMaxPower parameter in the Configuration Descriptor
 * reported to the host.  The value used is in units of 2mA.
 *
 * Self-powered devices are low power devices and must draw less than 100mA.
 *
 * Systems that have components such as a FEM are likely to consume more than
 * 100mA and are considered high power and therefore must be bus-powered.
 */
//@{

/**
 * @brief USB Max Power parameter (bMaxPower) the driver will report to the
 * host in the Configuration Descriptor.
 */
#define USB_MAX_POWER (50)
//@}

/** @name USB Enumeration Control
 *
 * The following are used to aid in the abstraction of which GPIO is
 * used for controlloing the pull-up resistor for enumeation.
 *
 * The hardware setup connects the D+ signal to a GPIO via a 1.5kOhm
 * pull-up resistor.  Any GPIO can be used since it just needs to be a
 * simple push-pull output configuration.
 */
//@{

/**
 * @brief The actual GPIO ENUMCTRL is connected to.  The GPIO only needs to
 * be a simple push-pull output or input.
 */
#define ENUMCTRL  PORTA_PIN(2)

/**
 * @brief Set the GPIO's configuration to the provided state.  The two
 * states used are GPIOCFG_OUT when the device is enumerated and GPIOCFG_IN
 * when the device is not enumerated.
 */
#define ENUMCTRL_SETCFG(cfg) do { SET_REG_FIELD(GPIO_PACFGL, PA2_CFG, cfg); } while (0)

/**
 * @brief When the GPIO used for enumeration is configured as push-pull, this
 * macro makes it easy to set the output state high.
 */
#define ENUMCTRL_SET()       do { GPIO_PASET = PA2; } while (0)

/**
 * @brief When the GPIO used for enumeration is configured as push-pull, this
 * macro makes it easy to clear the output state low.
 */
#define ENUMCTRL_CLR()       do { GPIO_PACLR = PA2; } while (0)
//@} //USB Enumeration Control

/** @name USB VBUS Monitoring Support
 *
 * @note VBUS Monitoring is required for USB to function when the EM358 device
 * is configured as self-powered.
 *
 * The following are used to aid in the abstraction of which
 * GPIO and IRQ is used for VBUS Monitoring.
 *
 * Remember that IRQA and IRQB are fixed to GPIO PB0 and PB6 respectively
 * while IRQC and IRQD can be assigned to any GPIO.  Since USB's D- and D+
 * data pins are fixed to PA0 and PA1 respectively, SC2 can't be used so it
 * makes sense to allocate PA2 for enumeration control and PA3 for VBUS
 * monitoring.  Therefore, using PA3 for VBUS monitoring requires IRQC or IRQD.
 *
 * The driver will only try to use VBUSMON functionality if USB_SELFPWRD_STATE
 * is set to 1.
 */
//@{

/**
 * @brief The actual GPIO VBUSMON is connected to.  Remember that other
 * pieces might want to use PA3.
 *
 * Leaving VBUSMON undefined will keep VBUS Monitoring functionality
 * from being compiled in and not conflict with other pieces that
 * might want to use the GPIO or IRQ that VBUS Monitoring needs.
 */
#define VBUSMON  PA3

/**
 * @brief The GPIO input register for VBUSMON.
 */
#define VBUSMON_IN          GPIO_PAIN

/**
 * @brief The GPIO configuration needed for VBUSMON.  The configuration
 * needs to be a simple input that will monitor for edge tansitions.
 */
#define VBUSMON_SETCFG() do { SET_REG_FIELD(GPIO_PACFGL, PA3_CFG, GPIOCFG_IN); } while (0)

/**
 * @brief Point the proper IRQ at the desired pin for VBUSMON.  Remember that
 * other pieces that might want to use IRQC.
 * @note For this board, IRQC is pointed at PA3.
 */
#define VBUSMON_SEL()       do { GPIO_IRQDSEL = PORTA_PIN(3); } while (0)

/**
 * @brief The interrupt service routine for VBUSMON.  Remember that
 * other pieces that might want to use IRQC.
 */
#define VBUSMON_ISR         halIrqDIsr

/**
 * @brief The interrupt configuration register for VBUSMON.
 */
#define VBUSMON_INTCFG      GPIO_INTCFGD

/**
 * @brief The interrupt enable bit for VBUSMON.
 */
#define VBUSMON_INT_EN_BIT  INT_IRQD

/**
 * @brief The interrupt flag bit for VBUSMON.
 */
#define VBUSMON_FLAG_BIT    INT_IRQDFLAG

/**
 * @brief The missed interrupt bit for VBUSMON.
 */
#define VBUSMON_MISS_BIT    INT_MISSIRQD
//@} //USB VBUS Monitoring Support

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
//#define RHO_GPIO              PORTA_PIN(6)

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
    DECLARE_INTERRUPT_STATE;  /* Must read-modify-write so to be safe disable interrupts */    \
    DISABLE_INTERRUPTS();                                                                      \
    if (enableRadioHoldOff) {     /* Radio HoldOff */                                          \
      /* Actual register state */                                                              \
      /*halGpioSetConfig(RHO_CFG, PWRUP_CFG_DFL_RHO_FOR_RHO);*/                                \
      RHO_CFG = RHO_CFG                                                                        \
                & ~(0x000F                   << ((RHO_GPIO & 3) * 4))                          \
                | (PWRUP_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO & 3) * 4));                         \
      RHO_OUT = RHO_OUT                                                                        \
                & ~(0x0001                   << ((RHO_GPIO & 7)))                              \
                | (PWRUP_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO & 7)));                             \
      /* Shadow register state */                                                              \
      gpioCfgPowerUp[RHO_GPIO >> 2] = gpioCfgPowerUp[RHO_GPIO >> 2]                            \
                                      & ~(0x000F                   << ((RHO_GPIO & 3) * 4))    \
                                      | (PWRUP_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO & 3) * 4));   \
      gpioOutPowerUp[RHO_GPIO >> 3] = gpioOutPowerUp[RHO_GPIO >> 3]                            \
                                      & ~(0x0001                   << ((RHO_GPIO & 7)))        \
                                      | (PWRUP_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO & 7)));       \
      gpioCfgPowerDown[RHO_GPIO >> 2] = gpioCfgPowerDown[RHO_GPIO >> 2]                        \
                                        & ~(0x000F                   << ((RHO_GPIO & 3) * 4))  \
                                        | (PWRDN_CFG_DFL_RHO_FOR_RHO << ((RHO_GPIO & 3) * 4)); \
      gpioOutPowerDown[RHO_GPIO >> 3] = gpioOutPowerDown[RHO_GPIO >> 3]                        \
                                        & ~(0x0001                   << ((RHO_GPIO & 7)))      \
                                        | (PWRDN_OUT_DFL_RHO_FOR_RHO << ((RHO_GPIO & 7)));     \
      RHO_INTCFG  = (0 << GPIO_INTFILT_BIT)         /* 0 = no filter  */                       \
                    | (3 << GPIO_INTMOD_BIT);       /* 3 = both edges */                       \
    } else {         /* default */                                                             \
      /* Actual register state */                                                              \
      /*halGpioSetConfig(RHO_CFG, PWRUP_CFG_DFL_RHO_FOR_DFL);*/                                \
      RHO_CFG = RHO_CFG                                                                        \
                & ~(0x000F                   << ((RHO_GPIO & 3) * 4))                          \
                | (PWRUP_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO & 3) * 4));                         \
      RHO_OUT = RHO_OUT                                                                        \
                & ~(0x0001                   << ((RHO_GPIO & 7)))                              \
                | (PWRUP_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO & 7)));                             \
      /* Shadow register state */                                                              \
      gpioCfgPowerUp[RHO_GPIO >> 2] = gpioCfgPowerUp[RHO_GPIO >> 2]                            \
                                      & ~(0x000F                   << ((RHO_GPIO & 3) * 4))    \
                                      | (PWRUP_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO & 3) * 4));   \
      gpioOutPowerUp[RHO_GPIO >> 3] = gpioOutPowerUp[RHO_GPIO >> 3]                            \
                                      & ~(0x0001                   << ((RHO_GPIO & 7)))        \
                                      | (PWRUP_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO & 7)));       \
      gpioCfgPowerDown[RHO_GPIO >> 2] = gpioCfgPowerDown[RHO_GPIO >> 2]                        \
                                        & ~(0x000F                   << ((RHO_GPIO & 3) * 4))  \
                                        | (PWRDN_CFG_DFL_RHO_FOR_DFL << ((RHO_GPIO & 3) * 4)); \
      gpioOutPowerDown[RHO_GPIO >> 3] = gpioOutPowerDown[RHO_GPIO >> 3]                        \
                                        & ~(0x0001                   << ((RHO_GPIO & 7)))      \
                                        | (PWRDN_OUT_DFL_RHO_FOR_DFL << ((RHO_GPIO & 7)));     \
      RHO_INTCFG  = 0;         /* disabled */                                                  \
    }                                                                                          \
    RHO_SEL();         /* Point IRQ at the desired pin */                                      \
    RESTORE_INTERRUPTS();                                                                      \
} while (0)

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
 * @note ENABLE_OSC32K is mutually exclusive with
 * ENABLE_ALT_FUNCTION_NTX_ACTIVE since they define conflicting
 * usage of GPIO PC6.
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

/** @name DISABLE_INTERNAL_1V8_REGULATOR
 *
 * When DISABLE_INTERNAL_1V8_REGULATOR is defined, the internal regulator for
 * the 1.8 V supply (VREG_1V8) is disabled.  Disabling of VREG_1V8 will
 * prevent excess current draw.
 *
 * The disabling occurs early in the halInit() sequence so that current
 * consumption is always minimized and the configuration will be applied
 * across all power modes.
 *
 * @note Disabling VREG_1V8 on devices that are not externally powered will
 * prevent the device from operating normally.
 *
 * @note Disabling the regulator will cause ADC readings of external signals
 * to be wrong.  These exteranl signals include analog sources ADC0 thru ADC5
 * and VDD_PADS/4.  Internal signals such as radio calibration will not be
 * affected.  Using the ADC requires re-enabling VREG_1V8.
 *
 * Refer to the file micro-common.h for further description.
 *
 */
//@{

/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger for other code to disable the 1.8V regulator.
 */
//#define DISABLE_INTERNAL_1V8_REGULATOR
//@} //END OF DISABLE_INTERNAL_1V8_REGULATOR DEFINITION

/** @name ENABLE_ALT_FUNCTION_TX_ACTIVE
 *
 * When ENABLE_ALT_FUNCTION_TX_ACTIVE is defined, halInit() and halPowerUp()
 * will enable the TX_ACTIVE alternate functionality of PC5.  halPowerDown()
 * will configure PC5 to be a low output.  TX_ACTIVE can be used for
 * external PA power management and RF switching logic.  In transmit mode
 * the Tx baseband drives TX_ACTIVE high.  In receive mode the TX_ACTIVE
 * signal is low.  This define will override any settings for PC5.
 */
//@{

/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the TX_ACTIVE alternate function on PC5.
 * Default is to not enable TX_ACTIVE functionality on PC5.
 */
#define ENABLE_ALT_FUNCTION_TX_ACTIVE
//@} //END OF ENABLE_ALT_FUNCTION_TX_ACTIVE DEFINITIONS

/** @name ENABLE_ALT_FUNCTION_NTX_ACTIVE
 *
 * When ENABLE_ALT_FUNCTION_NTX_ACTIVE is defined, halInit() and halPowerUp()
 * will enable the nTX_ACTIVE alternate functionality of PC6.  halPowerDown()
 * will configure PC6 to be a low output.  nTX_ACTIVE can be used for
 * external PA power management and RF switching logic.  In transmit mode
 * the Tx baseband drives nTX_ACTIVE low.  In receive mode the nTX_ACTIVE
 * signal is high.  This define will override any settings for PC6.
 *
 * @note ENABLE_ALT_FUNCTION_NTX_ACTIVE is mutually exclusive with
 * ENABLE_OSC32K since they define conflicting usage of GPIO PC6.
 */
//@{

/**
 * @brief This define does not equate to anything.  It is used as a
 * trigger to enable the nTX_ACTIVE alternate function on PC6.
 * Default is to not enable nTX_ACTIVE functionality on PC6.
 */
// #define ENABLE_ALT_FUNCTION_NTX_ACTIVE
//@} //END OF ENABLE_ALT_FUNCTION_NTX_ACTIVE DEFINITIONS

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

/** @name 32kHz Oscillator and nTX_ACTIVE Configuration Defines
 *
 * Since the 32kHz Oscillator and nTX_ACTIVE both share PC6, their
 * configuration defines are linked and instantiated together.  Look above
 * for the defines that enable the 32kHz Oscillator and nTX_ACTIVE.
 *
 * @note ENABLE_OSC32K is mutually exclusive with
 * ENABLE_ALT_FUNCTION_NTX_ACTIVE since they define conflicting
 * usage of GPIO PC6.
 *
 * When using the 32kHz, configure PC6 and PC7 for analog for the XTAL.
 *
 * When using nTX_ACTIVE, configure PC6 for alternate output while awake
 * and a low output when deepsleeping.  Also, configure PC7 for TEMP_EN.
 *
 * When not using the 32kHz or nTX_ACTIVE, configure PC6 and PC7 for
 * Button1 and TEMP_EN.
 *@{
 */

/**
 * @brief Give GPIO PC6 configuration a friendly name.
 */
#if defined(ENABLE_OSC32K) && defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
//Oops!  Only one of these can be used at a time!
  #error ENABLE_OSC32K and ENABLE_ALT_FUNCTION_NTX_ACTIVE are mutually \
  exclusive.  They define conflicting usage for GPIO PC6.

#elif defined(ENABLE_OSC32K) && !defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
//Use OCS32K configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_ANALOG
  #define PWRDN_OUT_BUTTON1  0

#elif !defined(ENABLE_OSC32K) && defined(ENABLE_ALT_FUNCTION_NTX_ACTIVE)
//Use nTX_ACTIVE configuration
  #define PWRUP_CFG_BUTTON1  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_BUTTON1  0
  #define PWRDN_CFG_BUTTON1  GPIOCFG_OUT
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
//@} END OF 32kHz Oscillator and nTX_ACTIVE Configuration Defines

/** @name TX_ACTIVE Configuration Defines
 *
 * Provide the proper set of pin (PC5) configurations for when TX_ACTIVE is
 * enabled (look above for the define which enables it).  When TX_ACTIVE is
 * not enabled, configure the pin for LED2.
 *@{
 */

/**
 * @brief Give the TX_ACTIVE configuration a friendly name.
 */
#ifdef  ENABLE_ALT_FUNCTION_TX_ACTIVE
  #define PWRUP_CFG_LED2  GPIOCFG_OUT_ALT
  #define PWRUP_OUT_LED2  0
  #define PWRDN_CFG_LED2  GPIOCFG_OUT
  #define PWRDN_OUT_LED2  0
#else //!ENABLE_ALT_FUNCTION_TX_ACTIVE
  #define PWRUP_CFG_LED2  GPIOCFG_OUT
  #define PWRUP_OUT_LED2  1  /* LED default off */
  #define PWRDN_CFG_LED2  GPIOCFG_OUT
  #define PWRDN_OUT_LED2  1  /* LED default off */
#endif//ENABLE_ALT_FUNCTION_TX_ACTIVE
//@} END OF TX_ACTIVE Configuration Defines

/** @name USB Configuration Defines
 *
 * Provide the proper set of pin configuration for when USB is not enumerated.
 * Not enumerated primarily refers to the driver not being configured or deep
 * sleep.  The configuration used here is only for keeping the USB off the bus.
 * The GPIO configuration used when active is controlled by the USB driver
 * since the driver needs to control the enumeration process (which affects
 * GPIO state.)
 *
 * @note: Using USB requires Serial port 3 to be defined and is only possible
 * on EM3582/EM3586/EM3588/EM359 chips.
 *@{
 */

/**
 * @brief Give the USB configuration a friendly name.
 */
#if (!defined(EM_SERIAL3_DISABLED))                        \
  && (defined(CORTEXM3_EM3582) || defined(CORTEXM3_EM3586) \
  || defined(CORTEXM3_EM3588) || defined(CORTEXM3_EM359))
  #define PWRUP_CFG_USBDM      GPIOCFG_IN
  #define PWRUP_OUT_USBDM      0
  #define PWRUP_CFG_USBDP      GPIOCFG_IN
  #define PWRUP_OUT_USBDP      0
  #define PWRUP_CFG_ENUMCTRL   GPIOCFG_OUT
  #define PWRUP_OUT_ENUMCTRL   0
  #define PWRUP_CFG_VBUSMON    GPIOCFG_IN
  #define PWRUP_OUT_VBUSMON    0
  #define PWRDN_CFG_USBDM      GPIOCFG_IN
  #define PWRDN_OUT_USBDM      0
  #define PWRDN_CFG_USBDP      GPIOCFG_IN
  #define PWRDN_OUT_USBDP      0
  #define PWRDN_CFG_ENUMCTRL   GPIOCFG_OUT
  #define PWRDN_OUT_ENUMCTRL   0
  #define PWRDN_CFG_VBUSMON    GPIOCFG_IN
  #define PWRDN_OUT_VBUSMON    0
#else
  #define PWRUP_CFG_USBDM      GPIOCFG_OUT_ALT
  #define PWRUP_OUT_USBDM      0
  #define PWRUP_CFG_USBDP      GPIOCFG_IN
  #define PWRUP_OUT_USBDP      0
  #define PWRUP_CFG_ENUMCTRL   GPIOCFG_OUT_ALT
  #define PWRUP_OUT_ENUMCTRL   0
  #define PWRUP_CFG_VBUSMON    GPIOCFG_OUT
  #define PWRUP_OUT_VBUSMON    1
  #define PWRDN_CFG_USBDM      GPIOCFG_IN_PUD
  #define PWRDN_OUT_USBDM      GPIOOUT_PULLUP
  #define PWRDN_CFG_USBDP      GPIOCFG_IN_PUD
  #define PWRDN_OUT_USBDP      GPIOOUT_PULLUP
  #define PWRDN_CFG_ENUMCTRL   GPIOCFG_IN_PUD
  #define PWRDN_OUT_ENUMCTRL   GPIOOUT_PULLUP
  #define PWRDN_CFG_VBUSMON    GPIOCFG_OUT
  #define PWRDN_OUT_VBUSMON    1
#endif//
//@} END OF USB Configuration Defines

/**
 * @brief Give GPIO SC1 TXD and nRTS configurations friendly names.
 */
#if     SLEEPY_IP_MODEM_UART
  #define PWRUP_CFG_SC1_TXD  GPIOCFG_OUT     // Let UART driver manage OUT_ALT
  #define PWRDN_OUT_SC1_nRTS 0               // Let peer send data to wake
#else//!SLEEPY_IP_MODEM_UART
  #define PWRUP_CFG_SC1_TXD  GPIOCFG_OUT_ALT // Pre-set for UART operation
  #define PWRDN_OUT_SC1_nRTS 1               // Deassert nRTS when sleeping
#endif//SLEEPY_IP_MODEM_UART

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
#define DEFINE_GPIO_RADIO_POWER_BOARD_MASK_VARIABLE()                           \
  GpioMaskType gpioRadioPowerBoardMask = { (BIT32(PORTC_PIN(5)) /* FEM_CTX */   \
                                            | BIT32(PORTC_PIN(6)) /* FEM_CSD */ \
                                            ) }

/**
 * @brief Initialize GPIO powerup configuration variables.
 */
#define DEFINE_POWERUP_GPIO_CFG_VARIABLES()                                    \
  uint16_t gpioCfgPowerUp[6] = {                                               \
    ((PWRUP_CFG_USBDM    << PA0_CFG_BIT)                                       \
     | (PWRUP_CFG_USBDP    << PA1_CFG_BIT)                                     \
     | (PWRUP_CFG_ENUMCTRL << PA2_CFG_BIT)                                     \
     | (PWRUP_CFG_VBUSMON  << PA3_CFG_BIT)),                                   \
    ((PWRUP_CFG_PTI_EN   << PA4_CFG_BIT)                                       \
     | (PWRUP_CFG_PTI_DATA << PA5_CFG_BIT)                                     \
     | (PWRUP_CFG_DFL_RHO  << PA6_CFG_BIT)                                     \
     | (GPIOCFG_OUT        << PA7_CFG_BIT)),                                   \
    ((GPIOCFG_OUT        << PB0_CFG_BIT)                                       \
     | (PWRUP_CFG_SC1_TXD  << PB1_CFG_BIT)                       /* SC1TXD  */ \
     | (GPIOCFG_IN_PUD     << PB2_CFG_BIT)                       /* SC1RXD  */ \
     | (GPIOCFG_IN_PUD     << PB3_CFG_BIT)),                     /* SC1nCTS */ \
    ((GPIOCFG_OUT_ALT    << PB4_CFG_BIT)                         /* SC1nRTS */ \
     | (GPIOCFG_ANALOG     << PB5_CFG_BIT)                                     \
     | (GPIOCFG_IN_PUD     << PB6_CFG_BIT)                                     \
     | (GPIOCFG_OUT_ALT    << PB7_CFG_BIT)),                                   \
    ((GPIOCFG_IN         << PC0_CFG_BIT)                                       \
     | (GPIOCFG_OUT        << PC1_CFG_BIT)                                     \
     | (GPIOCFG_OUT_ALT    << PC2_CFG_BIT)                                     \
     | (GPIOCFG_IN         << PC3_CFG_BIT)),                                   \
    ((GPIOCFG_IN         << PC4_CFG_BIT)                                       \
     | (PWRUP_CFG_LED2     << PC5_CFG_BIT)                                     \
     | (GPIOCFG_OUT        << PC6_CFG_BIT)                                     \
     | (CFG_TEMPEN         << PC7_CFG_BIT))                                    \
  }

/**
 * @brief Initialize GPIO powerup output variables.
 */
#define DEFINE_POWERUP_GPIO_OUTPUT_DATA_VARIABLES()                        \
  uint8_t gpioOutPowerUp[3] = {                                            \
    ((PWRUP_OUT_USBDM    << PA0_BIT)                                       \
     | (PWRUP_OUT_USBDP    << PA1_BIT)                                     \
     | (PWRUP_OUT_ENUMCTRL << PA2_BIT)                                     \
     | (PWRUP_OUT_VBUSMON  << PA3_BIT)                                     \
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
    ((0                  << PC0_BIT)                                       \
     | (0                  << PC1_BIT)                                     \
     | (1                  << PC2_BIT)                                     \
     | (0                  << PC3_BIT)                                     \
     | (0                  << PC4_BIT)                                     \
     | (PWRUP_OUT_LED2     << PC5_BIT)                                     \
     | (1                  << PC6_BIT)                                     \
     |                      /* Temp Sensor default on */                   \
     (1                  << PC7_BIT))                                      \
  }

/**
 * @brief Initialize powerdown GPIO configuration variables.
 */
#define DEFINE_POWERDOWN_GPIO_CFG_VARIABLES()                                    \
  uint16_t gpioCfgPowerDown[6] = {                                               \
    ((PWRDN_CFG_USBDM    << PA0_CFG_BIT)                                         \
     | (PWRDN_CFG_USBDP    << PA1_CFG_BIT)                                       \
     | (PWRDN_CFG_ENUMCTRL << PA2_CFG_BIT)                                       \
     | (PWRDN_CFG_VBUSMON  << PA3_CFG_BIT)),                                     \
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
     | (GPIOCFG_OUT        << PC6_CFG_BIT)                                       \
     | (CFG_TEMPEN         << PC7_CFG_BIT))                                      \
  }

/**
 * @brief Initialize powerdown GPIO output variables.
 */
#define DEFINE_POWERDOWN_GPIO_OUTPUT_DATA_VARIABLES()                        \
  uint8_t gpioOutPowerDown[3] = {                                            \
    ((PWRDN_OUT_USBDM     << PA0_BIT)                                        \
     | (PWRDN_OUT_USBDP     << PA1_BIT)                                      \
     | (PWRDN_OUT_ENUMCTRL  << PA2_BIT)                                      \
     | (PWRDN_OUT_VBUSMON   << PA3_BIT)                                      \
     |                        /* enable is idle low */                       \
     (PWRDN_OUT_PTI_EN   << PA4_BIT)                                         \
     |                        /* data is idle high */                        \
     (PWRDN_OUT_PTI_DATA << PA5_BIT)                                         \
     | (PWRDN_OUT_DFL_RHO  << PA6_BIT)                                       \
     |                        /* LED off */                                  \
     (1                  << PA7_BIT)),                                       \
    ((0                  << PB0_BIT)                                         \
     | (1                  << PB1_BIT)                         /* SC1TXD  */ \
     | (GPIOOUT_PULLUP     << PB2_BIT)                         /* SC1RXD  */ \
     | (GPIOOUT_PULLUP   << PB3_BIT)                         /* SC1nCTS */   \
     | (PWRDN_OUT_SC1_nRTS << PB4_BIT)                         /* SC1nRTS */ \
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
     | (0                  << PC6_BIT)                                       \
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
 * @brief Set resume GPIO configuration registers. Identical to SET_POWERUP
 */
#define SET_RESUME_GPIO_CFG_REGISTERS() \
  SET_POWERUP_GPIO_CFG_REGISTERS()

/**
 * @brief Set resume GPIO output registers. Identical to SET_POWERUP
 */
#define SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS() \
  SET_POWERUP_GPIO_OUTPUT_DATA_REGISTERS()

/**
 * @brief Set suspend GPIO configuration registers. SET_POWERDOWN minus USB regs
 */
#define SET_SUSPEND_GPIO_CFG_REGISTERS() \
  /* GPIO_PACFGL USB untouched! */       \
  GPIO_PACFGH = gpioCfgPowerDown[1];     \
  GPIO_PBCFGL = gpioCfgPowerDown[2];     \
  GPIO_PBCFGH = gpioCfgPowerDown[3];     \
  GPIO_PCCFGL = gpioCfgPowerDown[4];     \
  GPIO_PCCFGH = gpioCfgPowerDown[5];

/**
 * @brief Set suspend GPIO output registers. SET_POWERDOWN minus USB regs
 */
#define SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS()     \
  GPIO_PAOUT = (GPIO_PAOUT & 0x0F) /*USB untouched*/ \
               | (gpioOutPowerDown[0] & 0xF0);       \
  GPIO_PBOUT = gpioOutPowerDown[1];                  \
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
#if SLEEPY_IP_MODEM_UART  // SC1RXD
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
#define halInternalInitBoard()     \
  do {                             \
    halInternalPowerUpBoard();     \
    halInternalInitRadioHoldOff(); \
    halInternalInitButton();       \
  } while (0)

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
 * @brief Suspend the board.  This function is called from
 * ::halSuspend().
 */
#define halInternalSuspendBoard()            \
  do {                                       \
    /* Board peripheral deactivation */      \
    /* halInternalSleepAdc(); */             \
    SET_SUSPEND_GPIO_OUTPUT_DATA_REGISTERS() \
    SET_SUSPEND_GPIO_CFG_REGISTERS()         \
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

/**
 * @brief Resume the board.  This function is called from
 * ::halResume().
 */
#define halInternalResumeBoard()                             \
  do {                                                       \
    SET_RESUME_GPIO_OUTPUT_DATA_REGISTERS()                  \
    SET_RESUME_GPIO_CFG_REGISTERS()                          \
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
