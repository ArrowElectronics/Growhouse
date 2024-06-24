/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 * Code for manipulating the EEPROM from the Application Framework
 * In particular, sleepies that use the EEPROM will require re-initialization
 * of the driver code after they wake up from sleep.  This code helps
 * manage the state of the driver.
 *
 *****************************************************************************/

#ifdef EMBER_AF_API_AF_HEADER // AFV2
  #include EMBER_AF_API_AF_HEADER
#else                         // AFV6
  #include PLATFORM_HEADER
  #include CONFIGURATION_HEADER
  #include EMBER_AF_API_HAL
  #ifdef EMBER_AF_API_DEBUG_PRINT
    #include EMBER_AF_API_DEBUG_PRINT
  #endif

  #ifdef EMBER_TEST
    #define EMBER_TEST_ASSERT(x) assert((x))
  #else
    #define EMBER_TEST_ASSERT(x)
  #endif
#endif
#include "eeprom.h"

//------------------------------------------------------------------------------
// Globals

static bool eepromInitialized = false;

// NOTE:
// In EmberZNet 4.3 we required the code required that the
// underlying EEPROM driver MUST have support for arbitrary page writes
// (i.e. writes that cross page boundaries and or are smaller than the page size)
// Specifically the OTA Storage EEPROM Driver plugin code for storing OTA images
// requires this.

// This is no longer a requirement due to the fact that we have formal page-erase
// support built into the OTA code.  However for systems using a read-modify-write
// driver we have support here.

// The App. Bootloader for the 35x SOC prior to 4.3 did NOT have read-modify-write support.
// Therefore the shared app. bootloader EEPROM routines CANNOT be used; a copy
// of the EEPROM driver must be included with the application to support the OTA
// cluster.
// The 4.3 App. bootloader for the 35x does have arbitrary page-write support
// and thus the shared EEPROM routines may be used on the 35x SOC.

// The 250 has no shared bootloader EEPROM routines and so the application
// must include a copy of the EEPROM driver.  The Host co-processor based models
// must also include an EEPROM driver in their application that has arbitrary
// page-write support.

#if defined(EZSP_HOST) \
  || defined(EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_EEPROM_ENABLE_SOC_APP_BOOTLOADER_COMPATIBILITY_MODE)
  #define eepromInit() halEepromInit()
  #define eepromWrite(address, data, len) \
  halEepromWrite((address), (data), (len))
  #define eepromRead(address, data, len) \
  halEepromRead((address), (data), (len))
  #define eepromErase(address, len) \
  halEepromErase((address), (len))
  #define eepromBusy() \
  halEepromBusy()
  #define eepromInfo() \
  halEepromInfo()
  #define eepromShutdown() \
  halEepromShutdown()

#else // EM35x SOC with 4.3 bootloader or later
  #define eepromInit() halAppBootloaderInit()
  #define eepromWrite(address, data, len) \
  halAppBootloaderWriteRawStorage((address), (data), (len))
  #define eepromRead(address, data, len) \
  halAppBootloaderReadRawStorage((address), (data), (len))
  #define eepromErase(address, len) \
  halAppBootloaderEraseRawStorage((address), (len))
  #define eepromBusy() \
  halAppBootloaderStorageBusy()
  #define eepromInfo() \
  halAppBootloaderInfo()
  #define eepromShutdown() \
  halAppBootloaderShutdown()
#endif

EmAfPartialWriteStruct emAfEepromSavedPartialWrites[EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT];

// We cache the word size because determining the size requires calling into the
// bootloader and potentially reading it from the part itself.  Several pieces
// of code reference the word size quite often and therefore this will reduce
// the performance hit of the code.
#define UNKNOWN_WORD_SIZE 0

typedef enum {
  PAGE_ERASE_NOT_NEEDED = 0,
  PAGE_ERASE_REQUIRED   = 1,
  PAGE_ERASE_UNKNOWN    = 0xFF,
} PageEraseStatus;

PageEraseStatus pageEraseStatus = PAGE_ERASE_UNKNOWN;

#define pageEraseRequired() (pageEraseStatus == PAGE_ERASE_REQUIRED)

#define INVALID_INDEX 0xFF
#define INVALID_ADDRESS 0xFFFFFFFF

#ifdef EMBER_SCRIPTED_TEST
  #define eepromReallyDebugPrintln(...)
#else
  #define eepromReallyDebugPrintln(...) emberAfCorePrintln(__VA_ARGS__)
#endif

//#define EEPROM_DEBUG true
#if defined(EEPROM_DEBUG)
  #define eepromDebugPrintln(...) eepromReallyDebugPrintln(__VA_ARGS__)
#else
  #define eepromDebugPrintln(...)
#endif

//------------------------------------------------------------------------------
// App framework init callback
void emberAfPluginEepromInitCallback(void)
{
  emberAfPluginEepromInit();
}

//------------------------------------------------------------------------------

// Sleepies will need a re-initialization of the driver after sleep,
// so this code helps manage that state and automatically re-init the driver
// if it is needed.

bool emAfIsEepromInitialized(void)
{
  return eepromInitialized;
}

uint8_t emberAfPluginEepromGetWordSize(void)
{
  static uint8_t wordSize = UNKNOWN_WORD_SIZE;
  if (wordSize == UNKNOWN_WORD_SIZE) {
    const HalEepromInformationType* part = emberAfPluginEepromInfo();
    wordSize = 1;

    // The word size in the part was added much later in the driver
    // support.  Therefore we assume the word size is 1 for those devices
    // that don't support the parameter because in prior releases
    // we only released drivers that supported that word size.
    if (part != NULL
        && part->version >= EEPROM_INFO_MIN_VERSION_WITH_WORD_SIZE_SUPPORT) {
      wordSize = part->wordSizeBytes;
    }
  }
  return wordSize;
}

void emberAfPluginEepromNoteInitializedState(bool state)
{
  eepromInitialized = state;
}

static void eepromFirstTimeInit(void)
{
  if (PAGE_ERASE_UNKNOWN == pageEraseStatus) {
    const HalEepromInformationType* eepromInfo = eepromInfo();
    if (eepromInfo != NULL) {
      pageEraseStatus = ((eepromInfo->capabilitiesMask
                          & EEPROM_CAPABILITIES_PAGE_ERASE_REQD)
                         ? PAGE_ERASE_REQUIRED
                         : PAGE_ERASE_NOT_NEEDED);

      MEMSET(emAfEepromSavedPartialWrites,
             0xFF,
             sizeof(EmAfPartialWriteStruct)
             * EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT);

      // We can't do partial writes with a word size above 2.
      if (pageEraseRequired()) {
        assert(emberAfPluginEepromGetWordSize() <= EM_AF_EEPROM_MAX_WORD_SIZE);
      }
    } else {
      // Legacy drivers without EEPROM info structs were all read-modify write,
      // so we know definitively that page erase is needed.
      pageEraseStatus = PAGE_ERASE_NOT_NEEDED;
    }
  }
}

void emberAfPluginEepromInit(void)
{
  if (emAfIsEepromInitialized()) {
    return;
  }

  uint8_t eepromStatus = eepromInit();
  (void)eepromStatus;
  eepromReallyDebugPrintln("EEPROM init: 0x%X", eepromStatus);
  emberAfPluginEepromNoteInitializedState(true);

  eepromFirstTimeInit();
}

const HalEepromInformationType* emberAfPluginEepromInfo(void)
{
  emberAfPluginEepromInit();
  return eepromInfo();
}

static void clearPartialWrite(uint8_t index)
{
  MEMSET(&(emAfEepromSavedPartialWrites[index]), 0xFF, sizeof(EmAfPartialWriteStruct));
}

static uint8_t checkForPreceedingPartialWrite(uint32_t address)
{
  uint8_t i;
  eepromDebugPrintln("checkForPreceedingPartialWrite() address: 0x%4X", address);
  for (i = 0; i < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT; i++) {
    eepromDebugPrintln("%d, address 0x%4X", i, emAfEepromSavedPartialWrites[i].address);
    if (emAfEepromSavedPartialWrites[i].address != INVALID_ADDRESS
        && (emAfEepromSavedPartialWrites[i].address + 1 == address)) {
      return i;
    }
  }
  return INVALID_INDEX;
}

static uint8_t addPartialWrite(const EmAfPartialWriteStruct* newPartialWrite)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT; i++) {
    if (emAfEepromSavedPartialWrites[i].address == INVALID_ADDRESS) {
      MEMMOVE(&(emAfEepromSavedPartialWrites[i]),
              newPartialWrite,
              sizeof(EmAfPartialWriteStruct));
      eepromDebugPrintln("Stored partial write at index %d", i);
      return EEPROM_SUCCESS;
    }
  }
  return EEPROM_ERR;
}

#if defined(EMBER_TEST)
void emAfPluginEepromFakeEepromCallback(void)
{
  // The fake (simulated) EEPROM gets initialized at a funny time,
  // after emberAfPluginEepromInit().  We must reinitialize the known
  // parameters of the EEPROM by this plugin in case it has chnaged.
  emberAfPluginEepromNoteInitializedState(false);
  emberAfPluginEepromInit();
}
#endif

uint8_t emberAfPluginEepromWrite(uint32_t address,
                                 const uint8_t *data,
                                 uint16_t totalLength)
{
  uint8_t status = EEPROM_SUCCESS;
  EmAfPartialWriteStruct tempPartialWrite = { INVALID_ADDRESS, 0xFF };
  emberAfPluginEepromInit();

  if (pageEraseRequired() && (emberAfPluginEepromGetWordSize() > 1)) {
    uint8_t partialWriteData[EM_AF_EEPROM_MAX_WORD_SIZE];
    uint8_t index = checkForPreceedingPartialWrite(address);
    if (index != INVALID_INDEX
        && index < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT) {
      eepromDebugPrintln("Previous partial word detected at 0x%4X", address);
      partialWriteData[1] = data[0];
      totalLength--;
      partialWriteData[0] = emAfEepromSavedPartialWrites[index].data;
      eepromDebugPrintln("eepromWrite() Address: 0x%4X, len: 2",
                         address - 1);
      status = eepromWrite(address - 1,
                           partialWriteData,
                           2);  // assume max of 2 byte word size
      EMBER_TEST_ASSERT(status == EEPROM_SUCCESS);
      if (status != 0) {
        return status;
      }
      data++;
      clearPartialWrite(index);
      address++;
    }
  }

  if (pageEraseRequired()
      && ((totalLength % emberAfPluginEepromGetWordSize()) != 0)) {
    // Assume 2 byte word size
    totalLength--;
    tempPartialWrite.address = address + totalLength;
    tempPartialWrite.data = data[totalLength];
  }

  eepromDebugPrintln("eepromWrite() Address: 0x%4X, len: %l",
                     address,
                     totalLength);
  if (totalLength) {
    status = eepromWrite(address, data, totalLength);
  }

  EMBER_TEST_ASSERT(status == EEPROM_SUCCESS);

  if (status == 0
      && tempPartialWrite.address != INVALID_ADDRESS) {
    eepromDebugPrintln("Caching partial word write at address 0x%4X",
                       tempPartialWrite.address);
    status = addPartialWrite(&tempPartialWrite);
    EMBER_TEST_ASSERT(status == EEPROM_SUCCESS);
  }

  return status;
}

uint8_t emberAfPluginEepromFlushSavedPartialWrites(void)
{
  uint8_t i;
  if (!pageEraseRequired()) {
    return EEPROM_SUCCESS;
  }
  emberAfPluginEepromInit();
  for (i = 0; i < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT; i++) {
    uint8_t partialWrite[] = { 0xFF, 0xFF };
    if (emAfEepromSavedPartialWrites[i].address != INVALID_ADDRESS) {
      uint8_t status;
      partialWrite[0] = emAfEepromSavedPartialWrites[i].data;
      status = eepromWrite(emAfEepromSavedPartialWrites[i].address,
                           partialWrite,
                           2);
      EMBER_TEST_ASSERT(status == EEPROM_SUCCESS);
      if (status != EEPROM_SUCCESS) {
        return status;
      }
    }
  }
  return EEPROM_SUCCESS;
}

uint8_t emberAfPluginEepromRead(uint32_t address,
                                uint8_t *data,
                                uint16_t totalLength)
{
  uint8_t status;
  emberAfPluginEepromInit();
  status = eepromRead(address, data, totalLength);

  EMBER_TEST_ASSERT(status == EEPROM_SUCCESS);

  return status;
}

uint8_t emberAfPluginEepromErase(uint32_t address, uint32_t totalLength)
{
  uint8_t i;
  uint8_t status;
  emberAfPluginEepromInit();
  status = eepromErase(address, totalLength);

  EMBER_TEST_ASSERT(status == EEPROM_SUCCESS);

  for (i = 0; i < EMBER_AF_PLUGIN_EEPROM_PARTIAL_WORD_STORAGE_COUNT; i++) {
    if (emAfEepromSavedPartialWrites[i].address != INVALID_ADDRESS
        && emAfEepromSavedPartialWrites[i].address >= address
        && emAfEepromSavedPartialWrites[i].address < (address + totalLength)) {
      clearPartialWrite(i);
    }
  }

  return status;
}

bool emberAfPluginEepromBusy(void)
{
  emberAfPluginEepromInit();
  return eepromBusy();
}

// Returns true if shutdown was done, returns false if shutdown was not
// necessary.
bool emberAfPluginEepromShutdown(void)
{
  if (!emAfIsEepromInitialized()) {
    return false;
  }

  eepromShutdown();
  emberAfPluginEepromNoteInitializedState(false);
  return true;
}
