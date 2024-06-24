/**
 * @file error-def.h
 * @brief Return-code definitions for EmberZNet stack API functions.
 *
 * See @ref status_codes for documentation.
 * <!--Author(s): Richard Kelsey (richard@ember.com) -->
 * <!--Lee Taylor (lee@ember.com) -->
 *
 * <!--Copyright 2006 by Ember Corporation. All rights reserved.         *80*-->
 */

/**
 * @addtogroup status_codes
 *
 * Many EmberZNet API functions return an ::EmberStatus value to indicate
 * the success or failure of the call.
 * Return codes are one byte long.
 * This page documents the possible status codes and their meanings.
 *
 * See error-def.h for source code.
 *
 * See also error.h for information on how the values for the return codes are
 * built up from these definitions.
 * The file error-def.h is separated from error.h because utilities will use this file
 * to parse the return codes.
 *
 * @note Do not include error-def.h directly. It is included by
 * error.h inside an enumeration typedef, which is in turn included by ember.h.
 *
 * @{
 */

#ifndef SILABS_ERROR_DEF_H
#define SILABS_ERROR_DEF_H

/**
 * @name Generic Messages
 * These messages are system wide.
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The generic "no error" message.
 */
#define EMBER_SUCCESS(0x00)
#else
DEFINE_ERROR(SUCCESS, (EmberStatus)  0U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The generic "fatal error" message.
 */
#define EMBER_ERR_FATAL(0x01)
#else
DEFINE_ERROR(ERR_FATAL, (EmberStatus) 0x01U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An invalid value was passed as an argument to a function.
 */
#define EMBER_BAD_ARGUMENT(0x02)
#else
DEFINE_ERROR(BAD_ARGUMENT, (EmberStatus) 0x02U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The requested information was not found.
 */
#define EMBER_NOT_FOUND(0x03)
#else
DEFINE_ERROR(NOT_FOUND, (EmberStatus) 0x03U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The manufacturing and stack token format in non-volatile memory
 * is different than what the stack expects (returned at initialization).
 */
#define EMBER_EEPROM_MFG_STACK_VERSION_MISMATCH(0x04)
#else
DEFINE_ERROR(EEPROM_MFG_STACK_VERSION_MISMATCH, (EmberStatus) 0x04U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The static memory definitions in ember-static-memory.h
 * are incompatible with this stack version.
 */
#define EMBER_INCOMPATIBLE_STATIC_MEMORY_DEFINITIONS(0x05)
#else
DEFINE_ERROR(INCOMPATIBLE_STATIC_MEMORY_DEFINITIONS, (EmberStatus) 0x05U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The manufacturing token format in non-volatile memory is
 * different than what the stack expects (returned at initialization).
 */
#define EMBER_EEPROM_MFG_VERSION_MISMATCH(0x06)
#else
DEFINE_ERROR(EEPROM_MFG_VERSION_MISMATCH, (EmberStatus) 0x06U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The stack token format in non-volatile memory is different
 * than what the stack expects (returned at initialization).
 */
#define EMBER_EEPROM_STACK_VERSION_MISMATCH(0x07)
#else
DEFINE_ERROR(EEPROM_STACK_VERSION_MISMATCH, (EmberStatus) 0x07U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@} // END Generic Messages

/**
 * @name Packet Buffer Module Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There are no more buffers.
 */
#define EMBER_NO_BUFFERS(0x18)
#else
DEFINE_ERROR(NO_BUFFERS, (EmberStatus) 0x18U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@} / END Packet Buffer Module Errors

/**
 * @name Serial Manager Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Specifies an invalid baud rate.
 */
#define EMBER_SERIAL_INVALID_BAUD_RATE(0x20)
#else
DEFINE_ERROR(SERIAL_INVALID_BAUD_RATE, (EmberStatus) 0x20U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Specifies an invalid serial port.
 */
#define EMBER_SERIAL_INVALID_PORT(0x21)
#else
DEFINE_ERROR(SERIAL_INVALID_PORT, (EmberStatus) 0x21U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Tried to send too much data.
 */
#define EMBER_SERIAL_TX_OVERFLOW(0x22)
#else
DEFINE_ERROR(SERIAL_TX_OVERFLOW, (EmberStatus) 0x22U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There wasn't enough space to store a received character
 * and the character was dropped.
 */
#define EMBER_SERIAL_RX_OVERFLOW(0x23)
#else
DEFINE_ERROR(SERIAL_RX_OVERFLOW, (EmberStatus) 0x23U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Detected a UART framing error.
 */
#define EMBER_SERIAL_RX_FRAME_ERROR(0x24)
#else
DEFINE_ERROR(SERIAL_RX_FRAME_ERROR, (EmberStatus) 0x24U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Detected a UART parity error.
 */
#define EMBER_SERIAL_RX_PARITY_ERROR(0x25)
#else
DEFINE_ERROR(SERIAL_RX_PARITY_ERROR, (EmberStatus) 0x25U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There is no received data to process.
 */
#define EMBER_SERIAL_RX_EMPTY(0x26)
#else
DEFINE_ERROR(SERIAL_RX_EMPTY, (EmberStatus) 0x26U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The receive interrupt was not handled in time and a
 * character was dropped.
 */
#define EMBER_SERIAL_RX_OVERRUN_ERROR(0x27)
#else
DEFINE_ERROR(SERIAL_RX_OVERRUN_ERROR, (EmberStatus) 0x27U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name MAC Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The MAC transmit queue is full.
 */
#define EMBER_MAC_TRANSMIT_QUEUE_FULL(0x39)
#else
// Internal
DEFINE_ERROR(MAC_TRANSMIT_QUEUE_FULL, (EmberStatus) 0x39U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief MAC header FCF error on receive.
 */
#define EMBER_MAC_UNKNOWN_HEADER_TYPE(0x3A)
#else
DEFINE_ERROR(MAC_UNKNOWN_HEADER_TYPE, (EmberStatus) 0x3AU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief MAC ACK header received.
 */
#define EMBER_MAC_ACK_HEADER_TYPE(0x3B)
#else
DEFINE_ERROR(MAC_ACK_HEADER_TYPE, (EmberStatus) 0x3BU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The MAC can't complete this task because it is scanning.
 */
#define EMBER_MAC_SCANNING(0x3D)
#else
DEFINE_ERROR(MAC_SCANNING, (EmberStatus) 0x3DU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief No pending data exists for a data poll.
 */
#define EMBER_MAC_NO_DATA(0x31)
#else
DEFINE_ERROR(MAC_NO_DATA, (EmberStatus) 0x31U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Attempts to scan when joined to a network.
 */
#define EMBER_MAC_JOINED_NETWORK(0x32)
#else
DEFINE_ERROR(MAC_JOINED_NETWORK, (EmberStatus) 0x32U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Scan duration must be 0 to 14 inclusive. Tried to
 * scan with an incorrect duration value.
 */
#define EMBER_MAC_BAD_SCAN_DURATION(0x33)
#else
DEFINE_ERROR(MAC_BAD_SCAN_DURATION, (EmberStatus) 0x33U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief emberStartScan was called with an incorrect scan type.
 */
#define EMBER_MAC_INCORRECT_SCAN_TYPE(0x34)
#else
DEFINE_ERROR(MAC_INCORRECT_SCAN_TYPE, (EmberStatus) 0x34U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief emberStartScan was called with an invalid channel mask.
 */
#define EMBER_MAC_INVALID_CHANNEL_MASK(0x35)
#else
DEFINE_ERROR(MAC_INVALID_CHANNEL_MASK, (EmberStatus) 0x35U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Failed to scan the current channel because
 * the relevant MAC command could not be transmitted.
 */
#define EMBER_MAC_COMMAND_TRANSMIT_FAILURE(0x36)
#else
DEFINE_ERROR(MAC_COMMAND_TRANSMIT_FAILURE, (EmberStatus) 0x36U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An ACK was expected following the transmission but
 * the MAC level ACK was never received.
 */
#define EMBER_MAC_NO_ACK_RECEIVED(0x40)
#else
DEFINE_ERROR(MAC_NO_ACK_RECEIVED, (EmberStatus) 0x40U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief MAC failed to transmit a message because it could not successfully
 * perform a radio network switch.
 */
#define EMBER_MAC_RADIO_NETWORK_SWITCH_FAILED(0x41)
#else
DEFINE_ERROR(MAC_RADIO_NETWORK_SWITCH_FAILED, (EmberStatus) 0x41U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An indirect data message timed out before a poll requested it.
 */
#define EMBER_MAC_INDIRECT_TIMEOUT(0x42)
#else
DEFINE_ERROR(MAC_INDIRECT_TIMEOUT, (EmberStatus) 0x42U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Simulated EEPROM Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The Simulated EEPROM is telling the application that
 * at least one flash page to be erased. The GREEN status means the
 * current page has not filled above the ::ERASE_CRITICAL_THRESHOLD.
 *
 * The application should call the function ::halSimEepromErasePage()
 * when it can to erase a page.
 */
#define EMBER_SIM_EEPROM_ERASE_PAGE_GREEN(0x43)
#else
DEFINE_ERROR(SIM_EEPROM_ERASE_PAGE_GREEN, (EmberStatus) 0x43U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The Simulated EEPROM is telling the application that
 * at least one flash page must be erased.  The RED status means the
 * current page has filled above the ::ERASE_CRITICAL_THRESHOLD.
 *
 * Due to the shrinking availability of write space, data could
 * be lost. The application must call the function ::halSimEepromErasePage()
 * as soon as possible to erase a page.
 */
#define EMBER_SIM_EEPROM_ERASE_PAGE_RED(0x44)
#else
DEFINE_ERROR(SIM_EEPROM_ERASE_PAGE_RED, (EmberStatus) 0x44U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The Simulated EEPROM has run out of room to write new data
 * and the data trying to be set has been lost. This error code is the
 * result of ignoring the ::SIM_EEPROM_ERASE_PAGE_RED error code.
 *
 * The application must call the function ::halSimEepromErasePage() to make room for
 * any further calls to set a token.
 */
#define EMBER_SIM_EEPROM_FULL(0x45)
#else
DEFINE_ERROR(SIM_EEPROM_FULL, (EmberStatus) 0x45U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//  Errors 46 and 47 are now defined below in the
//    flash error block (was attempting to prevent renumbering).

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Attempt 1 to initialize the Simulated EEPROM has failed.
 *
 * This failure means the information already stored in the Flash (or a lack
 * thereof), is fatally incompatible with the token information compiled
 * into the code image being run.
 */
#define EMBER_SIM_EEPROM_INIT_1_FAILED(0x48)
#else
DEFINE_ERROR(SIM_EEPROM_INIT_1_FAILED, (EmberStatus) 0x48U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Attempt 2 to initialize the Simulated EEPROM has failed.
 *
 * This failure means Attempt 1 failed, and the token system failed to
 * properly reload default tokens and reset the Simulated EEPROM.
 */
#define EMBER_SIM_EEPROM_INIT_2_FAILED(0x49)
#else
DEFINE_ERROR(SIM_EEPROM_INIT_2_FAILED, (EmberStatus) 0x49U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Attempt 3 to initialize the Simulated EEPROM has failed.
 *
 * This failure means one or both of the tokens ::TOKEN_MFG_NVDATA_VERSION or
 * ::TOKEN_STACK_NVDATA_VERSION were incorrect and the token system failed to
 * properly reload default tokens and reset the Simulated EEPROM.
 */
#define EMBER_SIM_EEPROM_INIT_3_FAILED(0x4A)
#else
DEFINE_ERROR(SIM_EEPROM_INIT_3_FAILED, (EmberStatus) 0x4AU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The Simulated EEPROM is repairing itself.
 *
 * While there's nothing for an app to do when the SimEE is going to
 * repair itself (SimEE has to be fully functional for the rest of the
 * system to work), alert the application to the fact that repair
 * is occurring.  There are debugging scenarios where an app might want
 * to know that repair is happening, such as monitoring frequency.
 * @note  Common situations will trigger an expected repair, such as
 *        using an erased chip or changing token definitions.
 */
#define EMBER_SIM_EEPROM_REPAIRING(0x4D)
#else
DEFINE_ERROR(SIM_EEPROM_REPAIRING, (EmberStatus) 0x4DU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Flash Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief A fatal error has occurred while trying to write data to the
 * Flash. The target memory attempting to be programmed is already programmed.
 * The flash write routines were asked to flip a bit from a 0 to 1, which is
 * physically impossible and the write was therefore inhibited. The data in
 * the Flash cannot be trusted after this error.
 */
#define EMBER_ERR_FLASH_WRITE_INHIBITED(0x46)
#else
DEFINE_ERROR(ERR_FLASH_WRITE_INHIBITED, (EmberStatus) 0x46U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief A fatal error has occurred while trying to write data to the
 * Flash and the write verification has failed.  Data in the Flash
 * cannot be trusted after this error and it is possible this error is the
 * result of exceeding the life cycles of the Flash.
 */
#define EMBER_ERR_FLASH_VERIFY_FAILED(0x47)
#else
DEFINE_ERROR(ERR_FLASH_VERIFY_FAILED, (EmberStatus) 0x47U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @description A fatal error has occurred while trying to write data to the
 * Flash possibly due to write protection or an invalid address. Data in
 * the Flash cannot be trusted after this error and it is possible this error
 * is the result of exceeding the life cycles of the Flash.
 */
#define EMBER_ERR_FLASH_PROG_FAIL(0x4B)
#else
DEFINE_ERROR(ERR_FLASH_PROG_FAIL, (EmberStatus) 0x4BU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @description A fatal error has occurred while trying to erase the Flash possibly
 * due to write protection. Data in the Flash cannot be trusted after
 * this error and it is possible this error is the result of exceeding the
 * life cycles of the Flash.
 */
#define EMBER_ERR_FLASH_ERASE_FAIL(0x4C)
#else
DEFINE_ERROR(ERR_FLASH_ERASE_FAIL, (EmberStatus) 0x4CU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Bootloader Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The bootloader received an invalid message (failed attempt
 * to go into bootloader).
 */
#define EMBER_ERR_BOOTLOADER_TRAP_TABLE_BAD(0x58)
#else
DEFINE_ERROR(ERR_BOOTLOADER_TRAP_TABLE_BAD, (EmberStatus) 0x58U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The bootloader received an invalid message (failed attempt to go
 * into the bootloader).
 */
#define EMBER_ERR_BOOTLOADER_TRAP_UNKNOWN(0x59)
#else
DEFINE_ERROR(ERR_BOOTLOADER_TRAP_UNKNOWN, (EmberStatus) 0x59U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The bootloader cannot complete the bootload operation because
 * either an image was not found or the image exceeded memory bounds.
 */
#define EMBER_ERR_BOOTLOADER_NO_IMAGE(0x05A)
#else
DEFINE_ERROR(ERR_BOOTLOADER_NO_IMAGE, (EmberStatus) 0x5AU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Transport Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The APS layer attempted to send or deliver a message
 * and failed.
 */
#define EMBER_DELIVERY_FAILED(0x66)
#else
DEFINE_ERROR(DELIVERY_FAILED, (EmberStatus) 0x66U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief This binding index is out of range for the current binding table.
 */
#define EMBER_BINDING_INDEX_OUT_OF_RANGE(0x69)
#else
DEFINE_ERROR(BINDING_INDEX_OUT_OF_RANGE, (EmberStatus) 0x69U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief This address table index is out of range for the current
 * address table.
 */
#define EMBER_ADDRESS_TABLE_INDEX_OUT_OF_RANGE(0x6A)
#else
DEFINE_ERROR(ADDRESS_TABLE_INDEX_OUT_OF_RANGE, (EmberStatus) 0x6AU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An invalid binding table index was given to a function.
 */
#define EMBER_INVALID_BINDING_INDEX(0x6C)
#else
DEFINE_ERROR(INVALID_BINDING_INDEX, (EmberStatus) 0x6CU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The API call is not allowed given the current state of the
 * stack.
 */
#define EMBER_INVALID_CALL(0x70)
#else
DEFINE_ERROR(INVALID_CALL, (EmberStatus) 0x70U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The link cost to a node is not known.
 */
#define EMBER_COST_NOT_KNOWN(0x71)
#else
DEFINE_ERROR(COST_NOT_KNOWN, (EmberStatus) 0x71U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The maximum number of in-flight messages (i.e.,
 * ::EMBER_APS_UNICAST_MESSAGE_COUNT) has been reached.
 */
#define EMBER_MAX_MESSAGE_LIMIT_REACHED(0x72)
#else
DEFINE_ERROR(MAX_MESSAGE_LIMIT_REACHED, (EmberStatus) 0x72U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The message to be transmitted is too big to fit into a
 * single over-the-air packet.
 */
#define EMBER_MESSAGE_TOO_LONG(0x74)
#else
DEFINE_ERROR(MESSAGE_TOO_LONG, (EmberStatus) 0x74U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The application is trying to delete or overwrite a binding
 * that is in use.
 */
#define EMBER_BINDING_IS_ACTIVE(0x75)
#else
DEFINE_ERROR(BINDING_IS_ACTIVE, (EmberStatus) 0x75U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The application is trying to overwrite an address table entry
 * that is in use.
 */
#define EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE(0x76)
#else
DEFINE_ERROR(ADDRESS_TABLE_ENTRY_IS_ACTIVE, (EmberStatus) 0x76U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An attempt was made to transmit during the suspend period.
 */
#define EMBER_TRANSMISSION_SUSPENDED(0x77)
#else
DEFINE_ERROR(TRANSMISSION_SUSPENDED, (EmberStatus) 0x77U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}
//

/**
 * @name  Green Power status codes
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Security match.
 */
#define EMBER_MATCH(0x78)
#else
DEFINE_ERROR(MATCH, (EmberStatus) 0x78U)
#endif //DOXYGEN_SHOULD_SKIP_THIS
#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Drop frame.
 */
#define EMBER_DROP_FRAME(0x79)
#else
DEFINE_ERROR(DROP_FRAME, (EmberStatus) 0x79U)
#endif //DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define EMBER_PASS_UNPROCESSED(0x7A)
#else
DEFINE_ERROR(PASS_UNPROCESSED, (EmberStatus) 0x7AU)
#endif //DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define EMBER_TX_THEN_DROP(0x7B)
#else
DEFINE_ERROR(TX_THEN_DROP, (EmberStatus) 0x7BU)
#endif //DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define EMBER_NO_SECURITY(0x7C)
#else
DEFINE_ERROR(NO_SECURITY, (EmberStatus) 0x7CU)
#endif //DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define EMBER_COUNTER_FAILURE(0x7D)
#else
DEFINE_ERROR(COUNTER_FAILURE, (EmberStatus) 0x7DU)
#endif //DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define EMBER_AUTH_FAILURE(0x7E)
#else
DEFINE_ERROR(AUTH_FAILURE, (EmberStatus) 0x7EU)
#endif //DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
#define EMBER_UNPROCESSED(0x7F)
#else
DEFINE_ERROR(UNPROCESSED, (EmberStatus) 0x7FU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}
//

/**
 * @name  HAL Module Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The conversion is complete.
 */
#define EMBER_ADC_CONVERSION_DONE(0x80)
#else
DEFINE_ERROR(ADC_CONVERSION_DONE, (EmberStatus) 0x80U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The conversion cannot be done because a request is being
 * processed.
 */
#define EMBER_ADC_CONVERSION_BUSY(0x81)
#else
DEFINE_ERROR(ADC_CONVERSION_BUSY, (EmberStatus) 0x81U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The conversion is deferred until the current request has been
 * processed.
 */
#define EMBER_ADC_CONVERSION_DEFERRED(0x82)
#else
DEFINE_ERROR(ADC_CONVERSION_DEFERRED, (EmberStatus) 0x82U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief No results are pending.
 */
#define EMBER_ADC_NO_CONVERSION_PENDING(0x84)
#else
DEFINE_ERROR(ADC_NO_CONVERSION_PENDING, (EmberStatus) 0x84U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Sleeping (for a duration) has been abnormally interrupted
 * and exited prematurely.
 */
#define EMBER_SLEEP_INTERRUPTED(0x85)
#else
DEFINE_ERROR(SLEEP_INTERRUPTED, (EmberStatus) 0x85U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  PHY Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The transmit hardware buffer underflowed.
 */
#define EMBER_PHY_TX_UNDERFLOW(0x88)
#else
DEFINE_ERROR(PHY_TX_UNDERFLOW, (EmberStatus) 0x88U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The transmit hardware did not finish transmitting a packet.
 */
#define EMBER_PHY_TX_INCOMPLETE(0x89)
#else
DEFINE_ERROR(PHY_TX_INCOMPLETE, (EmberStatus) 0x89U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An unsupported channel setting was specified.
 */
#define EMBER_PHY_INVALID_CHANNEL(0x8A)
#else
DEFINE_ERROR(PHY_INVALID_CHANNEL, (EmberStatus) 0x8AU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An unsupported power setting was specified.
 */
#define EMBER_PHY_INVALID_POWER(0x8B)
#else
DEFINE_ERROR(PHY_INVALID_POWER, (EmberStatus) 0x8BU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The requested operation cannot be completed because the radio
 * is currently busy, either transmitting a packet or performing calibration.
 */
#define EMBER_PHY_TX_BUSY(0x8C)
#else
DEFINE_ERROR(PHY_TX_BUSY, (EmberStatus) 0x8CU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The transmit attempt failed because all CCA attempts indicated that
 * the channel was busy.
 */
#define EMBER_PHY_TX_CCA_FAIL(0x8D)
#else
DEFINE_ERROR(PHY_TX_CCA_FAIL, (EmberStatus) 0x8DU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The software installed on the hardware doesn't recognize the
 * hardware radio type.
 */
#define EMBER_PHY_OSCILLATOR_CHECK_FAILED(0x8E)
#else
DEFINE_ERROR(PHY_OSCILLATOR_CHECK_FAILED, (EmberStatus) 0x8EU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The expected ACK was received after the last transmission.
 */
#define EMBER_PHY_ACK_RECEIVED(0x8F)
#else
DEFINE_ERROR(PHY_ACK_RECEIVED, (EmberStatus) 0x8FU)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Return Codes Passed to emberStackStatusHandler()
 * See also ::emberStackStatusHandler().
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The stack software has completed initialization and is ready
 * to send and receive packets over the air.
 */
#define EMBER_NETWORK_UP(0x90)
#else
DEFINE_ERROR(NETWORK_UP, (EmberStatus) 0x90U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The network is not operating.
 */
#define EMBER_NETWORK_DOWN(0x91)
#else
DEFINE_ERROR(NETWORK_DOWN, (EmberStatus) 0x91U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An attempt to join a network failed.
 */
#define EMBER_JOIN_FAILED(0x94)
#else
DEFINE_ERROR(JOIN_FAILED, (EmberStatus) 0x94U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief After moving, a mobile node's attempt to re-establish contact
 * with the network failed.
 */
#define EMBER_MOVE_FAILED(0x96)
#else
DEFINE_ERROR(MOVE_FAILED, (EmberStatus) 0x96U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An attempt to join as a router failed due to a Zigbee
 * versus Zigbee Pro incompatibility.  Zigbee devices joining Zigbee Pro networks
 * (or vice versa) must join as End Devices, not Routers.
 */
#define EMBER_CANNOT_JOIN_AS_ROUTER(0x98)
#else
DEFINE_ERROR(CANNOT_JOIN_AS_ROUTER, (EmberStatus) 0x98U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief The local node ID has changed. The application can get the new
 *  node ID by calling ::emberGetNodeId().
 */
#define EMBER_NODE_ID_CHANGED(0x99)
#else
DEFINE_ERROR(NODE_ID_CHANGED, (EmberStatus) 0x99U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief The local PAN ID has changed. The application can get the new PAN
 *  ID by calling ::emberGetPanId().
 */
#define EMBER_PAN_ID_CHANGED(0x9A)
#else
DEFINE_ERROR(PAN_ID_CHANGED, (EmberStatus) 0x9AU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief The channel has changed.
 */
#define EMBER_CHANNEL_CHANGED(0x9B)
#else
DEFINE_ERROR(CHANNEL_CHANGED, (EmberStatus) 0x9BU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief An attempt to join or rejoin the network failed because
 *  no router beacons could be heard by the joining node.
 */
#define EMBER_NO_BEACONS(0xAB)
#else
DEFINE_ERROR(NO_BEACONS, (EmberStatus) 0xABU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief An attempt was made to join a Secured Network using a pre-configured
 *  key, but the Trust Center sent back a Network Key in-the-clear when
 *  an encrypted Network Key was required. (::EMBER_REQUIRE_ENCRYPTED_KEY).
 */
#define EMBER_RECEIVED_KEY_IN_THE_CLEAR(0xAC)
#else
DEFINE_ERROR(RECEIVED_KEY_IN_THE_CLEAR, (EmberStatus) 0xACU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief An attempt was made to join a Secured Network, but the device did
 *  not receive a Network Key.
 */
#define EMBER_NO_NETWORK_KEY_RECEIVED(0xAD)
#else
DEFINE_ERROR(NO_NETWORK_KEY_RECEIVED, (EmberStatus) 0xADU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief After a device joined a Secured Network, a Link Key was requested
 *  (::EMBER_GET_LINK_KEY_WHEN_JOINING) but no response was ever received.
 */
#define EMBER_NO_LINK_KEY_RECEIVED(0xAE)
#else
DEFINE_ERROR(NO_LINK_KEY_RECEIVED, (EmberStatus) 0xAEU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief An attempt was made to join a Secured Network without a
 *  pre-configured key, but the Trust Center sent encrypted data using a
 *  pre-configured key.
 */
#define EMBER_PRECONFIGURED_KEY_REQUIRED(0xAF)
#else
DEFINE_ERROR(PRECONFIGURED_KEY_REQUIRED, (EmberStatus) 0xAFU)
#endif

//@}

/**
 * @name  Security Errors
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The passed key data is not valid. A key of all zeros or
 *   all F's are reserved values and cannot be used.
 */
#define EMBER_KEY_INVALID(0xB2)
#else
DEFINE_ERROR(KEY_INVALID, (EmberStatus) 0xB2U)
#endif // DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The chosen security level (the value of ::EMBER_SECURITY_LEVEL)
 *  is not supported by the stack.
 */
#define EMBER_INVALID_SECURITY_LEVEL(0x95)
#else
DEFINE_ERROR(INVALID_SECURITY_LEVEL, (EmberStatus) 0x95U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An error occurred when trying to encrypt at the APS Level.
 *
 * This error occurs either because the long address of the recipient can't be
 * determined from the short address (no entry in the binding table)
 * or there is no link key entry in the table associated with the destination,
 * or there was a failure to load the correct key into the encryption core.
 */
#define EMBER_APS_ENCRYPTION_ERROR(0xA6)
#else
DEFINE_ERROR(APS_ENCRYPTION_ERROR, (EmberStatus) 0xA6U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief There was an attempt to form or join a network with security
 * without calling ::emberSetInitialSecurityState() first.
 */
#define EMBER_SECURITY_STATE_NOT_SET(0xA8)
#else
DEFINE_ERROR(SECURITY_STATE_NOT_SET, (EmberStatus) 0xA8U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief There was an attempt to set an entry in the key table using
 *   an invalid long address. Invalid addresses include:
 *    - The local device's IEEE address
 *    - Trust Center's IEEE address
 *    - An existing table entry's IEEE address
 *    - An address consisting of all zeros or all F's
 */
#define EMBER_KEY_TABLE_INVALID_ADDRESS(0xB3)
#else
DEFINE_ERROR(KEY_TABLE_INVALID_ADDRESS, (EmberStatus) 0xB3U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief There was an attempt to set a security configuration that
 *   is not valid given the other security settings.
 */
#define EMBER_SECURITY_CONFIGURATION_INVALID(0xB7)
#else
DEFINE_ERROR(SECURITY_CONFIGURATION_INVALID, (EmberStatus) 0xB7U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief There was an attempt to broadcast a key switch too quickly after
 *    broadcasting the next network key. The Trust Center must wait at
 *    least a period equal to the broadcast timeout so that all routers
 *    have a chance to receive the broadcast of the new network key.
 */
#define EMBER_TOO_SOON_FOR_SWITCH_KEY(0xB8)
#else
DEFINE_ERROR(TOO_SOON_FOR_SWITCH_KEY, (EmberStatus) 0xB8U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief The received signature corresponding to the message that was passed
    to the CBKE Library failed verification and is not valid.
 */
#define EMBER_SIGNATURE_VERIFY_FAILURE(0xB9)
#else
DEFINE_ERROR(SIGNATURE_VERIFY_FAILURE, (EmberStatus) 0xB9U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief The message could not be sent because the link key corresponding
    to the destination is not authorized for use in APS data messages.
    APS Commands (sent by the stack) are allowed. To use it for encryption
    of APS data messages it must be authorized using a key agreement protocol
    (such as CBKE).
 */
#define EMBER_KEY_NOT_AUTHORIZED(0xBB)
#else
DEFINE_ERROR(KEY_NOT_AUTHORIZED, (EmberStatus) 0xBBU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief The security data provided was not valid, or an
 *    integrity check failed.
 */
#define EMBER_SECURITY_DATA_INVALID(0xBD)
#else
DEFINE_ERROR(SECURITY_DATA_INVALID, (EmberStatus) 0xBDU)
#endif

//@}

/**
 * @name  Miscellaneous Network Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The node has not joined a network.
 */
#define EMBER_NOT_JOINED(0x93)
#else
DEFINE_ERROR(NOT_JOINED, (EmberStatus) 0x93U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief A message cannot be sent because the network is currently
 *  overloaded.
 */
#define EMBER_NETWORK_BUSY(0xA1)
#else
DEFINE_ERROR(NETWORK_BUSY, (EmberStatus) 0xA1U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The application tried to send a message using an
 * endpoint that it has not defined.
 */
#define EMBER_INVALID_ENDPOINT(0xA3)
#else
DEFINE_ERROR(INVALID_ENDPOINT, (EmberStatus) 0xA3U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The application tried to use a binding that has been remotely
 * modified and the change has not yet been reported to the application.
 */
#define EMBER_BINDING_HAS_CHANGED(0xA4)
#else
DEFINE_ERROR(BINDING_HAS_CHANGED, (EmberStatus) 0xA4U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An attempt to generate random bytes failed because of
 * insufficient random data from the radio.
 */
#define EMBER_INSUFFICIENT_RANDOM_DATA(0xA5)
#else
DEFINE_ERROR(INSUFFICIENT_RANDOM_DATA, (EmberStatus) 0xA5U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** A Zigbee route error command frame was received indicating
 * that a source routed message from this node failed en route.
 */
#define EMBER_SOURCE_ROUTE_FAILURE(0xA9)
#else
DEFINE_ERROR(SOURCE_ROUTE_FAILURE, (EmberStatus) 0xA9U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** A Zigbee route error command frame was received indicating
 * that a message sent to this node along a many-to-one route
 * failed en route. The route error frame was delivered by
 * an ad-hoc search for a functioning route.
 */
#define EMBER_MANY_TO_ONE_ROUTE_FAILURE(0xAA)
#else
DEFINE_ERROR(MANY_TO_ONE_ROUTE_FAILURE, (EmberStatus) 0xAAU)
#endif

//@}

/**
 * @name  Miscellaneous Utility Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief A critical and fatal error indicating that the version of the
 * stack trying to run does not match with the chip it's running on. The
 * software (stack) on the chip must be replaced with software
 * compatible with the chip.
 */
#define EMBER_STACK_AND_HARDWARE_MISMATCH(0xB0)
#else
DEFINE_ERROR(STACK_AND_HARDWARE_MISMATCH, (EmberStatus) 0xB0U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief An index was passed into the function that was larger
 * than the valid range.
 */
#define EMBER_INDEX_OUT_OF_RANGE(0xB1)
#else
DEFINE_ERROR(INDEX_OUT_OF_RANGE, (EmberStatus) 0xB1U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief There are no empty entries left in the table.
 */
#define EMBER_TABLE_FULL(0xB4)
#else
DEFINE_ERROR(TABLE_FULL, (EmberStatus) 0xB4U)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The requested table entry has been erased and contains
 *   no valid data.
 */
#define EMBER_TABLE_ENTRY_ERASED(0xB6)
#else
DEFINE_ERROR(TABLE_ENTRY_ERASED, (EmberStatus) 0xB6U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The requested function cannot be executed because
 *   the library that contains the necessary functionality is not present.
 */
#define EMBER_LIBRARY_NOT_PRESENT(0xB5)
#else
DEFINE_ERROR(LIBRARY_NOT_PRESENT, (EmberStatus) 0xB5U)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The stack accepted the command and is currently processing
 * the request.  The results will be returned via an appropriate handler.
 */
#define EMBER_OPERATION_IN_PROGRESS(0xBA)
#else
DEFINE_ERROR(OPERATION_IN_PROGRESS, (EmberStatus) 0xBAU)
#endif

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief The EUI of the Trust center has changed due to a successful rejoin.
 *   The device may need to perform other authentication to verify the new TC
 *   is authorized to take over.
 */
#define EMBER_TRUST_CENTER_EUI_HAS_CHANGED(0xBC)
#else
DEFINE_ERROR(TRUST_CENTER_EUI_HAS_CHANGED, (EmberStatus) 0xBCU)
#endif

//@}

/**
 * @name  NVM3 Token Errors
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as no valid NVM3 page was found.
 */
#define EMBER_NVM3_TOKEN_NO_VALID_PAGES(0xC0)
#else
DEFINE_ERROR(NVM3_TOKEN_NO_VALID_PAGES, 0xC0)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the NVM3 instance was already opened with other parameters.
 */
#define EMBER_NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS(0xC1)
#else
DEFINE_ERROR(NVM3_ERR_OPENED_WITH_OTHER_PARAMETERS, 0xC1)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the NVM3 instance is not aligned properly in memory.
 */
#define EMBER_NVM3_ERR_ALIGNMENT_INVALID(0xC2)
#else
DEFINE_ERROR(NVM3_ERR_ALIGNMENT_INVALID, 0xC2)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *   as the size of the NVM3 instance is too small.
 */
#define EMBER_NVM3_ERR_SIZE_TOO_SMALL(0xC3)
#else
DEFINE_ERROR(NVM3_ERR_SIZE_TOO_SMALL, 0xC3)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that the initialization was aborted
 *    as the NVM3 page size is not supported.
 */
#define EMBER_NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED(0xC4)
#else
DEFINE_ERROR(NVM3_ERR_PAGE_SIZE_NOT_SUPPORTED, 0xC4)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that there was an error initializing
 * some of the tokens.
 */
#define EMBER_NVM3_ERR_TOKEN_INIT(0xC5)
#else
DEFINE_ERROR(NVM3_ERR_TOKEN_INIT, 0xC5)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application there has been an error when
 * attempting to upgrade SimEE tokens.
 */
#define EMBER_NVM3_ERR_UPGRADE(0xC6)
#else
DEFINE_ERROR(NVM3_ERR_UPGRADE, 0xC6)
#endif //DOXYGEN_SHOULD_SKIP_THIS

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief NVM3 is telling the application that there has been an unknown
 * error.
 */
#define EMBER_NVM3_ERR_UNKNOWN(0xC7)
#else
DEFINE_ERROR(NVM3_ERR_UNKNOWN, 0xC7)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@}

/**
 * @name  Application Errors
 * These error codes are available for application use.
 */
//@{

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief This error is reserved for customer application use.
 *  This will never be returned from any portion of the network stack or HAL.
 */
#define EMBER_APPLICATION_ERROR_0(0xF0)
#define EMBER_APPLICATION_ERROR_1(0xF1)
#define EMBER_APPLICATION_ERROR_2(0xF2)
#define EMBER_APPLICATION_ERROR_3(0xF3)
#define EMBER_APPLICATION_ERROR_4(0xF4)
#define EMBER_APPLICATION_ERROR_5(0xF5)
#define EMBER_APPLICATION_ERROR_6(0xF6)
#define EMBER_APPLICATION_ERROR_7(0xF7)
#define EMBER_APPLICATION_ERROR_8(0xF8)
#define EMBER_APPLICATION_ERROR_9(0xF9)
#define EMBER_APPLICATION_ERROR_10(0xFA)
#define EMBER_APPLICATION_ERROR_11(0xFB)
#define EMBER_APPLICATION_ERROR_12(0xFC)
#define EMBER_APPLICATION_ERROR_13(0xFD)
#define EMBER_APPLICATION_ERROR_14(0xFE)
#define EMBER_APPLICATION_ERROR_15(0xFF)
#else
DEFINE_ERROR(APPLICATION_ERROR_0, 0xF0)
DEFINE_ERROR(APPLICATION_ERROR_1, 0xF1)
DEFINE_ERROR(APPLICATION_ERROR_2, 0xF2)
DEFINE_ERROR(APPLICATION_ERROR_3, 0xF3)
DEFINE_ERROR(APPLICATION_ERROR_4, 0xF4)
DEFINE_ERROR(APPLICATION_ERROR_5, 0xF5)
DEFINE_ERROR(APPLICATION_ERROR_6, 0xF6)
DEFINE_ERROR(APPLICATION_ERROR_7, 0xF7)
DEFINE_ERROR(APPLICATION_ERROR_8, 0xF8)
DEFINE_ERROR(APPLICATION_ERROR_9, 0xF9)
DEFINE_ERROR(APPLICATION_ERROR_10, 0xFA)
DEFINE_ERROR(APPLICATION_ERROR_11, 0xFB)
DEFINE_ERROR(APPLICATION_ERROR_12, 0xFC)
DEFINE_ERROR(APPLICATION_ERROR_13, 0xFD)
DEFINE_ERROR(APPLICATION_ERROR_14, 0xFE)
DEFINE_ERROR(APPLICATION_ERROR_15, 0xFF)
#endif //DOXYGEN_SHOULD_SKIP_THIS

//@} // END name group

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * All error status codes defined in error-def.h
 * (and described in @ref status_codes) now have an \c EMBER_ prefix.
 * Status code changes also include the following:
 * <ul>
 * <li> <b>New items</b>
 *   - ::EMBER_ERR_FLASH_WRITE_INHIBITED
 *   - ::EMBER_ERR_FLASH_VERIFY_FAILED
 *   - ::EMBER_ADDRESS_TABLE_INDEX_OUT_OF_RANGE
 *   - ::EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE
 *   - ::EMBER_CANNOT_JOIN_AS_ROUTER
 *   - ::EMBER_INSUFFICIENT_RANDOM_DATA
 *   - ::EMBER_APS_ENCRYPTION_ERROR
 *   - ::EMBER_SECURITY_STATE_NOT_SET
 *   - ::EMBER_SOURCE_ROUTE_FAILURE
 *   - ::EMBER_MANY_TO_ONE_ROUTE_FAILURE
 *   .
 * <li> <b>Changed items</b>
 *   -
 *   -
 *   .
 * <li> <b>Removed items</b>
 *   - ::SIM_EEPROM_FLASH_WRITE_FAILED
 *   - ::TOO_MANY_CONNECTIONS
 *   - ::CONNECTION_OPEN
 *   - ::CONNECTION_FAILED
 *   - ::CONNECTION_CLOSED
 *   - ::CONNECTION_CLOSING
 *   - ::CONNECTION_NOT_YET_OPEN
 *   - ::INCOMING_SEQUENCED_MESSAGES_LOST
 *   - ::NODEID_INVALID
 *   .
 * </ul>
 * HIDDEN -->
 */

/**
 * <!-- HIDDEN
 * @page 3p0_to_350
 * <hr>
 * <ul>
 * <li> <b>Removed items</b>
 *   - ::EMBER_ERR_TOKEN_UNKNOWN
 *   - ::EMBER_ERR_TOKEN_EXISTS
 *   - ::EMBER_ERR_TOKEN_INVALID_SIZE
 *   - ::EMBER_ERR_TOKEN_READ_ONLY
 *   .
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_ERROR_DEF_H
