/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 * Header file for eeprom plugin API.
 *
 *****************************************************************************/

#ifndef EEPROM_H
#define EEPROM_H

/**
 * Initialize the underlying EEPROM part.
 *
 * Note that this API is called in every other public API in order to ensure
 * that the underlying EEPROM part is initialized.
 */
void emberAfPluginEepromInit(void);

/**
 * Note whether or not the underlying EEPROM part is initialized.
 *
 * This API is meant for cases where an external action initializes or
 * deinitializes the underlying EEPROM part. It will force the EEPROM plugin
 * to reinitialize the EEPROM next time an API is called.
 *
 * @param state True if the underlying EEPROM part is initialized, false
 *              otherwise.
 */
void emberAfPluginEepromNoteInitializedState(bool state);

/**
 * Get the word size of the underlying EEPROM part.
 *
 * @return The word size of the underlying EEPROM part.
 */
uint8_t emberAfPluginEepromGetWordSize(void);

/**
 * Get information about the underlying EEPROM part.
 *
 * @return Information about the underlying EEPROM part.
 */
const HalEepromInformationType* emberAfPluginEepromInfo(void);

/**
 * Write data to an address in the underlying EEPROM part.
 *
 * This API will cache partial word writes so that only a word-size multiple
 * number of bytes will be written at once. Any leftover byte(s) will be written
 * during the next call to this API or during a call to
 * ::emberAfPluginEepromFlushSavedPartialWrites.
 *
 * @param address The address at which the data should be written
 * @param data A pointer to the data to be written
 * @param totalLength The total length of the data to be written
 *
 * @return ::EEPROM_SUCCESS if the write operation succeeded, ::EEPROM_ERR
 *         otherwise.
 */
uint8_t emberAfPluginEepromWrite(uint32_t address, const uint8_t *data, uint16_t totalLength);

/**
 * Read data from an address in the underlying EEPROM part.
 *
 * @param address The address at which the data should be read
 * @param data A pointer to the buffer in which to read the data
 * @param totalLength The total length of the data to be read
 *
 * @return ::EEPROM_SUCCESS if the read operation succeeded, ::EEPROM_ERR
 *         otherwise.
 */
uint8_t emberAfPluginEepromRead(uint32_t address, uint8_t *data, uint16_t totalLength);

/**
 * Erase the EEPROM starting at an address.
 *
 * Erase has a 32-bit argument, since it's possible to erase more than a
 * uint16_t size chunk. Read and write have only uint16_t for length, because
 * you don't have enough RAM for the data buffer.
 *
 * @param address The address at which to start erasing the data
 * @param totalLength The total length of the data to be erased
 *
 * @return ::EEPROM_SUCCESS if the erase operation succeeded, ::EEPROM_ERR
 *         otherwise.
 */
uint8_t emberAfPluginEepromErase(uint32_t address, uint32_t totalLength);

/**
 * Return whether or not the underlying EEPROM part is in the middle of an
 * operation.
 *
 * @return True if the underlying EEPROM part is in the middle of an operation,
 *         false otherwise.
 */
bool emberAfPluginEepromBusy(void);

/**
 * Shutdown the underlying EEPROM part.
 *
 * @return True if shutdown was done, false if shutdown was not necessary.
 */
bool emberAfPluginEepromShutdown(void);

/**
 * Flush the cached partial word writes to the EEPROM part.
 *
 * This API will take all of the partial words that have not been written to the
 * EEPROM and write them padded with 0xFF bytes (in order to write a multiple of
 * the EEPROM word size).
 *
 * @return Either ::EEPROM_SUCCESS or ::EEPROM_ERR, depending on whether or not
 *         the flush operation succeeded.
 */
uint8_t emberAfPluginEepromFlushSavedPartialWrites(void);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#if defined(EMBER_TEST)
void emAfPluginEepromFakeEepromCallback(void);
#endif

// Currently there are no EEPROM/flash parts that we support that have a word size
// of 4.  The local storage bootloader has a 2-byte word size and that is the main
// thing we are optimizing for.
#define EM_AF_EEPROM_MAX_WORD_SIZE 2

typedef struct {
  uint32_t address;
  uint8_t data;
} EmAfPartialWriteStruct;

extern EmAfPartialWriteStruct emAfEepromSavedPartialWrites[];
bool emAfIsEepromInitialized(void);

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif // EEPROM_H
