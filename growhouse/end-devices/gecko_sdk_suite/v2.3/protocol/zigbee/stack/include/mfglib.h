/** @file mfglib.h
 * See @ref mfglib for documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.
 */

#ifndef SILABS_MFGLIB_H
#define SILABS_MFGLIB_H

/**
 * @addtogroup mfglib
 * @brief This is a manufacturing and functional test library for
 * testing and verifying the RF component of products at manufacture time.
 *
 * See mfglib.h for source code.
 *
 * Developers can optionally include this library in their
 * application code. The goal is that in most cases, this will
 * eliminate the need for developers to load multiple images into
 * their hardware at manufacturing time.
 *
 * This library can optionally be compiled into the developer's
 * production code and run at manufacturing time. Any interface to
 * the library is handled by the application.
 *
 * This library cannot assist in hardware start up.
 *
 * Many functions in this file return an ::EmberStatus value. See
 * error-def.h for definitions of all ::EmberStatus return values.
 * @{
 */

/** @brief Activates use of mfglib test routines and enables
 * the radio receiver to report packets it receives to the
 * caller-specified ::mfglibRxCallback() routine.
 *
 * It is legal to pass in a NULL.
 * These packets will not be passed up with a CRC failure.
 * The first byte of the packet in the callback is the length.
 * All other functions will return an error until
 * mfglibStart() has been called.
 *
 * @appusage Use this function to enter test mode.
 *
 * Note: This function should only be called shortly after
 *   initialization and prior to forming or joining a network.
 *
 * @param mfglibRxCallback A function pointer to callback routine that is
 * invoked whenever a valid packet is received. emberTick() must be called
 * routinely for this callback to function correctly.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS              if the mfg test mode has been enabled.
 * - ::EMBER_ERR_FATAL            if the mfg test mode is not available.  */
EmberStatus mfglibStart(void (*mfglibRxCallback)(uint8_t *packet, uint8_t linkQuality, int8_t rssi));

/** @brief Deactivates use of @ref mfglib test routines.
 *
 * This restores the hardware to the state it was in prior to mfglibStart() and
 * stops receiving packets started by mfglibStart() at the same time.
 *
 * @appusage Use this function to exit the mfg test mode.
 *
 * Note: It may be desirable to also reboot after use of manufacturing
 *   mode to ensure all application state is properly re-initialized.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS           if the mfg test mode has been exited.
 * - ::EMBER_ERR_FATAL         if the mfg test mode cannot be exited.  */
EmberStatus mfglibEnd(void);

/** @brief Starts transmitting the tone feature of the radio.
 *
 * In this mode, the radio will transmit an unmodulated
 * tone on the currently set channel and power level. Upon
 * successful return, the tone will be transmitting. To stop
 * transmitting a tone, the application must call mfglibStopTone(),
 * allowing it the flexibility to determine its own criteria for tone
 * duration, such as time, event, and so on.
 *
 * @appusage Use this function to transmit a tone.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS          if the transmit tone has started.
 * - ::EMBER_ERR_FATAL        if the tone cannot be started.  */
EmberStatus mfglibStartTone(void);

/** @brief Stops transmitting a tone started by mfglibStartTone().
 *
 * @appusage Use this function to stop transmitting a tone.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS          if the transmit tone has stopped.
 * - ::EMBER_ERR_FATAL        if the tone cannot be stopped.  */
EmberStatus mfglibStopTone(void);

/** @brief Starts transmitting a random stream of characters to enable
 * the measurement of radio modulation.
 *
 * @appusage Use this function to enable the measurement of radio
 * modulation.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS          if the transmit stream has started.
 * - ::EMBER_ERR_FATAL        if the stream cannot be started.  */
EmberStatus mfglibStartStream(void);

/** @brief Stops transmitting a random stream of characters started by
 * mfglibStartStream().
 *
 * @appusage Use this function to end the measurement of radio
 * modulation.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS          if the transmit stream has stopped.
 * - ::EMBER_ERR_FATAL        if the stream cannot be stopped.  */
EmberStatus mfglibStopStream(void);

/** @brief Sends a single packet, (repeat + 1) times.
 *
 * @appusage Use this function to send raw data. Note that <em>packet</em> array
 * must be word-aligned (begin at even address), such that
 * <em>((((uint16_t)packet) & 1) == 0)</em> holds true. (This is generally done
 * by either declaring <em>packet</em> as a local variable or putting it in a
 * global declaration immediately following the declaration of an uint16_t.)
 *
 * @param packet A packet to be sent.
 * First byte of the packet is always the length byte, the value of which does
 * not include itself but does include the 16-bit CRC in the length calculation.
 * The CRC gets appended automatically by the radio as it transmits the packet,
 * so the host does not need to provide this as part of packetContents.
 * The total length of packet contents (Length Byte+1) going out the radio
 * should not be >128 or <6 bytes.
 * Note that the packet array should not include the CRC, as this is appended
 * by the radio automatically.
 *
 * @param repeat The number of times to repeat sending the packet
 * after having been sent once. A value of 0 means send once and don't repeat.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS                 if the packet was sent.
 * - ::EMBER_ERR_FATAL               if the mfg test mode is not available or TONE or STREAM test is running.*/
EmberStatus mfglibSendPacket(uint8_t * packet, uint16_t repeat);

/** @brief Selects the radio channel. The channel range is from 11 to 26.
 *
 * Customers can set any valid channel they want.
 * Calibration occurs if this is the first time after power up.
 *
 * @appusage Use this function to change channels.
 *
 * @param chan Valid values depend upon the radio used.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS               if the channel has been set.
 * - ::EMBER_PHY_INVALID_CHANNEL   if the channel requested is invalid.
 * - ::EMBER_ERR_FATAL             if the mfg test mode is not available or TONE or STREAM test is running.*/
EmberStatus mfglibSetChannel(uint8_t chan);

/** @brief Returns the current radio channel, as previously
 * set via mfglibSetChannel().
 *
 * @appusage Use this function to get current channel.
 *
 * @return Current channel. */
uint8_t mfglibGetChannel(void);

/** @brief First select the transmit power mode, and then
 * include a method for selecting the radio transmit power.
 *
 * Valid power settings depend upon the specific radio in use. Ember
 * radios have discrete power settings, and then requested power is
 * rounded to a valid power setting. The actual power output is
 * available to the caller via mfglibGetPower().
 *
 * @appusage Use this function to adjust the transmit power.
 *
 * @param txPowerMode Boost mode or external PA.
 *
 * @param power Power in units of dBm, which can be negative.
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS               if the power has been set.
 * - ::EMBER_PHY_INVALID_POWER     if the power requested is invalid.
 * - ::EMBER_ERR_FATAL             if the mfg test mode is not available or TONE or STREAM test is running.*/
EmberStatus mfglibSetPower(uint16_t txPowerMode, int8_t power);

/** @brief Returns the current radio power setting as
 * previously set via mfglibSetPower().
 *
 * @appusage Use this function to get current power setting.
 *
 * @return The current power setting. */
int8_t mfglibGetPower(void);

/** @brief Sets manufacturing library options.
 *
 * @appusage Use this function to set manufacturing library options.
 *
 * @param options bitmask.  0 == non-CSMA transmits, 1 == CSMA transmits
 *
 * @return One of the following:
 * - ::EMBER_SUCCESS                 if the options have been set.
 * - ::EMBER_BAD_ARGUMENT            if any options are unavailable.
 * - ::EMBER_ERR_FATAL               if the mfg test mode is not available or TONE or STREAM test is running.*/
EmberStatus mfglibSetOptions(uint8_t options);

/** @brief Returns the current manufacturing library options, as previously
 * set via mfglibSetOptions().
 *
 * @appusage Use this function to get library options.
 *
 * @return Current options based on the current test mode. */
uint8_t mfglibGetOptions(void);

/** @brief Set the synth offset in steps.
 *
 * Set the synth offset in 11.7 kHz steps on em3xx
 * or 9.375 kHz steps on EFR32. This function does NOT
 * write the new synth offset to the token, it only changes it in memory. It
 * can be changed as many times as preferred, and the setting will be lost when
 * a reset occurs. The value will survive deep sleep, but will not survive a
 * reset, thus it will not take effect in the bootloader. To make the value
 * permanent (and accessible to the bootloader), the TOKEN_MFG_SYNTH_FREQ_OFFSET
 * token must be written using the token API or em3xx_load --patch or
 * Simplicity COmmander.
 *
 * @appusage Use this function to compensate for tolerances in the crystal
 * oscillator or capacitors. This function does not effect a permanent change;
 * once a preferred offset is found, it must be written to a token using
 * the token API for it to be permanent.
 *
 * @param synOffset The number of steps to offset the carrier frequency
 * (may be negative).
 */
void mfglibSetSynOffset(int8_t synOffset);

/** @brief Get the current synth offset in steps.
 *
 * Steps are 11.7 kHz on em3xx and 9.375 kHz on EFR32. See
 * mfglibSetSynOffset() for details.
 * @return The synth offset.
 */
int8_t mfglibGetSynOffset(void);

/** @brief Run mod DAC calibration on the given channel for
 * the given amount of time.
 *
 * If the duration argument == 0, this test will run forever (until the
 * chip is reset).
 *
 * @appusage Use this function to run the active transmit part of
 * mod DAC calibration.
 *
 * @param channel   Selects the channel to transmit on.
 *
 * @param duration  Duration in ms, 0 == infinite.
 *
 * @return          None.
 */
void mfglibTestContModCal(uint8_t channel, uint32_t duration);

/** @} END addtogroup
 */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The entire file mfglib.h is new and is described in @ref mfglib.
 * <ul>
 * <li> <b>New items</b>
 * <ul>
 * <li>
 * </ul>
 * <li> <b>Changed items</b>
 * <ul>
 * <li>
 * </ul>
 * <li> <b>Removed items</b>
 * <ul>
 * <li>
 * </ul>
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_MFGLIB_H
