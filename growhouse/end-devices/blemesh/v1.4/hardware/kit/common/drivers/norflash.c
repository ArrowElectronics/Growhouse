/***************************************************************************//**
 * @file
 * @brief Norflash driver
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include <stddef.h>

#include "em_common.h"
#include "em_ebi.h"
#include "norflash.h"
#include "bspconfig.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup NORFlash
 * @brief Driver for Spanion S29GL128P90FFIR13 and S29GL064N NOR Flash devices
 *
 * @details
 *  The NORFlash driver implements a common API for different types
 *  of NOR Flash devices. The current version of the driver is implemented based
 *  on Spanion S29GL128P90FFIR13 and S29GL064N (model 3). This driver supports
 *  non-uniform sectored flash architecture devices that has a top/bottom sector
 *  which is divided into a smaller sectors. Spansion S29GL128P90FFIR13 is a
 *  16MByte device organized in 128 sectors of 128KBytes each.
 *  S29GL064N (model 3) is using boot sectored flash architecture and is
 *  organized in 127 sectors of 64KBytes each, in addition to a top 8-8 Kbyte
 *  sectors. The module can easily be modified to suit other NOR Flash devices.
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
static volatile uint16_t     *flashBase;
static bool                  flashInitialized = false;
static NORFLASH_Info_TypeDef flashInfo = { 0 };

static bool bootSectorAddress(uint32_t addr);
static int  flashInterrogate(void);
static int  flashPoll(uint32_t addr, uint16_t data);
static void flashReset(void);
static void flashUnlockCmd(void);
static int  flashWriteBuffer(uint32_t sectorAddr, uint32_t addr,
                             uint16_t *data, uint32_t count);

/** @endcond */

/***************************************************************************//**
 * @brief
 *   Check if an address is valid for the flash device.
 *
 * @param[in] addr
 *   The address value to check.
 *
 * @return
 *   True if address value is valid, false otherwise.
 ******************************************************************************/
bool NORFLASH_AddressValid(uint32_t addr)
{
  if (!flashInitialized) {
    if (flashInterrogate() != NORFLASH_STATUS_OK) {
      return false;
    }
  }

  if ((addr >= flashInfo.baseAddress)
      && (addr < (flashInfo.baseAddress + flashInfo.deviceSize))) {
    return true;
  }

  return false;
}

/***************************************************************************//**
 * @brief
 *   Return a pointer to a @ref NORFLASH_Info_TypeDef, which contain
 *   vital flash device information.
 *
 * @return
 *   NULL if unable to initialize the flash if no prior call to
 *   @ref NORFLASH_Init() has been made.
 *   Pointer to a @ref NORFLASH_Info_TypeDef on success.
 ******************************************************************************/
NORFLASH_Info_TypeDef *NORFLASH_DeviceInfo(void)
{
  if (!flashInitialized) {
    if (flashInterrogate() != NORFLASH_STATUS_OK) {
      return NULL;
    }
  }

  return &flashInfo;
}

/***************************************************************************//**
 * @brief
 *   Erase entire flash device.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_EraseDevice(void)
{
  int status;

  if (!flashInitialized) {
    status = flashInterrogate();

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }
  }

  flashUnlockCmd();

  flashBase[0x555] = 0x80;
  flashBase[0x555] = 0xAA;
  flashBase[0x2AA] = 0x55;
  flashBase[0x555] = 0x10;

  return flashPoll(flashInfo.baseAddress, 0xFFFF);
}

/***************************************************************************//**
 * @brief
 *   Erase a sector in the flash device.
 *
 * @param[in] addr
 *   Address of the sector to erase.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_EraseSector(uint32_t addr)
{
  int status;

  if (!flashInitialized) {
    status = flashInterrogate();

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }
  }

  if (!NORFLASH_AddressValid(addr)) {
    EFM_ASSERT(false);
    return NORFLASH_INVALID_ADDRESS;
  }

  /* Mask off LSB's according to sectorsize to get sector start address */
  if (!bootSectorAddress(addr)) {
    addr = addr & ~(flashInfo.sectorSize - 1);
  } else {
    addr = addr & ~(flashInfo.bootSectorSize - 1);
  }

  flashUnlockCmd();

  flashBase[0x555] = 0x80;
  flashBase[0x555] = 0xAA;
  flashBase[0x2AA] = 0x55;

  *(volatile uint16_t*) addr = 0x30;

  return flashPoll(addr, 0xFFFF);
}

/***************************************************************************//**
 * @brief
 *   Initialize the NORFLASH module.
 *
 * @note
 *   This function should be called before any other NORFLASH_xxx() functions
 *   are used.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_Init(void)
{
  return flashInterrogate();
}

/***************************************************************************//**
 * @brief
 *   Program the flash device.
 *
 * @note
 *   It is assumed that the area to be programmed is erased.
 *
 * @param[in] addr
 *   The first address in the flash to be programmed.
 *
 * @param[in] data
 *   Pointer to the data to be programmed.
 *
 * @param[in] count
 *   Number of bytes to be programmed.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_Program(uint32_t addr, uint8_t *data, uint32_t count)
{
  int      status;
  uint32_t sectorAddress, burst;

  if (!flashInitialized) {
    status = flashInterrogate();

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }
  }

  if (!NORFLASH_AddressValid(addr)
      || !NORFLASH_AddressValid(addr + count - 1)) {
    EFM_ASSERT(false);
    return NORFLASH_INVALID_ADDRESS;
  }

  /* Check if odd byte aligned */
  if (addr & 1) {
    status = NORFLASH_ProgramByte(addr, *data);

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }

    addr++;
    data++;
    count--;
  }

  while (count >= 2) {
#if 0     /* Traditional write method, write one word at a time */
    status = NORFLASH_ProgramWord16(addr, (*(data + 1) << 8) | *data);

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }

    addr  += 2;
    data  += 2;
    count -= 2;

#else /* "Write Buffer" write method */
    /* Max write buffer "words" at a time, must not cross sector boundary */
    if (!bootSectorAddress(addr)) {
      sectorAddress = addr & ~(flashInfo.sectorSize - 1);

      burst = SL_MIN(flashInfo.writeBufferSize,
                     sectorAddress + flashInfo.sectorSize - addr);
    } else {
      sectorAddress = addr & ~(flashInfo.bootSectorSize - 1);

      burst = SL_MIN(flashInfo.writeBufferSize,
                     sectorAddress + flashInfo.bootSectorSize - addr);
    }
    burst = SL_MIN(burst, count & 0xFFFFFFFE);
    status = flashWriteBuffer(sectorAddress, addr, (uint16_t*) data, burst);

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }

    addr  += burst;
    data  += burst;
    count -= burst;
#endif
  }

  /* Check if a trailing odd byte aligned value must be programmed */
  if (count) {
    status = NORFLASH_ProgramByte(addr, *data);
  }

  return status;
}

/***************************************************************************//**
 * @brief
 *   Program a single byte in the flash device.
 *
 * @note
 *   It is assumed that the area to be programmed is erased.
 *
 * @param[in] addr
 *   The address to be programmed.
 *
 * @param[in] data
 *   The byte value to be programmed.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_ProgramByte(uint32_t addr, uint8_t data)
{
  uint16_t tmp;

  tmp = *(volatile uint16_t*)(addr & 0xFFFFFFFE);
  if (addr & 1) {
    tmp = (tmp & 0xFF) | (data << 8);
  } else {
    tmp = (tmp & 0xFF00) | data;
  }

  return NORFLASH_ProgramWord16(addr & 0xFFFFFFFE, tmp);
}

/***************************************************************************//**
 * @brief
 *   Program a word (16bit) in the flash device.
 *
 * @note
 *   It is assumed that the area to be programmed is erased.
 *
 * @param[in] addr
 *   The address to be programmed.
 *
 * @param[in] data
 *   The word value to be programmed.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_ProgramWord16(uint32_t addr, uint16_t data)
{
  int status;

  if (!flashInitialized) {
    status = flashInterrogate();

    if (status != NORFLASH_STATUS_OK) {
      return status;
    }
  }

  if (!NORFLASH_AddressValid(addr)
      || !NORFLASH_AddressValid(addr + 1)) {
    EFM_ASSERT(false);
    return NORFLASH_INVALID_ADDRESS;
  }

  if (addr & 1) {
    return NORFLASH_MISALIGNED_ADDRESS;
  }

  flashUnlockCmd();
  flashBase[0x555] = 0xA0;

  *(volatile uint16_t*) addr = data;

  return flashPoll(addr, data);
}

/***************************************************************************//**
 * @brief
 *   Program a word (32bit) in the flash device.
 *
 * @note
 *   It is assumed that the area to be programmed is erased.
 *
 * @param[in] addr
 *   The address to be programmed.
 *
 * @param[in] data
 *   The word value to be programmed.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
int NORFLASH_ProgramWord32(uint32_t addr, uint32_t data)
{
  int status;

  if (addr & 3) {
    return NORFLASH_MISALIGNED_ADDRESS;
  }

  status = NORFLASH_ProgramWord16(addr, data & 0xFFFF);

  if (status == NORFLASH_STATUS_OK) {
    addr  += 2;
    status = NORFLASH_ProgramWord16(addr, data >> 16);
  }

  return status;
}

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Checks if the address belongs to the boot sectored space
 *
 * @param[in] addr
 *   The address to be checked.
 *
 * @return
 *   true if the address belongs to the boot sectored space otherwise false.
 ******************************************************************************/
static bool bootSectorAddress(uint32_t addr)
{
  bool retVal = false;

  if ((flashInfo.deviceArc == 2
       && addr < flashInfo.bootSectorSize * flashInfo.bootSectorCount)
      || (flashInfo.deviceArc == 3
          && addr >= flashInfo.sectorSize * flashInfo.sectorCount)) {
    retVal = true;
  }

  return retVal;
}

/***************************************************************************//**
 * @brief
 *   Read flash device properties and initialize the flash information
 *   struct.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
static int flashInterrogate(void)
{
  flashInfo.baseAddress = EBI_BankAddress(BSP_CONFIG_NORFLASH_EBI_BANK);
  flashBase             = (volatile uint16_t*) flashInfo.baseAddress;
  flashReset();

  flashUnlockCmd();
  flashBase[0x555] = 0x90;      /* Autoselect command */

  /* Read device info */
  flashInfo.manufacturerId = flashBase[0x00];
  flashInfo.deviceId       = (flashBase[0x01] & 0xFF) << 16;
  flashInfo.deviceId      |= (flashBase[0x0E] & 0xFF) << 8;
  flashInfo.deviceId      |= flashBase[0x0F] & 0xFF;

  flashReset();

  flashBase[0x55] = 0x98;       /* CFI query command */

  /* Check for CFI compliant device */
  if ((flashBase[0x10] != 'Q')
      || (flashBase[0x11] != 'R')
      || (flashBase[0x12] != 'Y')) {
    flashReset();
    return NORFLASH_NOT_CFI_DEVICE;
  }

  /* Get device geometry info, flash sector region count */
  if (flashBase[0x2C] == 1) {
    flashInfo.sectorCount  = flashBase[0x2D];
    flashInfo.sectorCount |= (flashBase[0x2E] << 8) & 0xFF00;
    flashInfo.sectorSize   = flashBase[0x2F];
    flashInfo.sectorSize  |= (flashBase[0x30] << 8) & 0xFF00;

    flashInfo.deviceArc = flashBase[0x2C];
  } else if (flashBase[0x2C] == 2) {
    flashInfo.sectorCount  = flashBase[0x31];
    flashInfo.sectorCount |= (flashBase[0x32] << 8) & 0xFF00;
    flashInfo.sectorSize   = flashBase[0x33];
    flashInfo.sectorSize  |= (flashBase[0x34] << 8) & 0xFF00;

    flashInfo.bootSectorCount = flashBase[0x2D];
    flashInfo.bootSectorCount |= (flashBase[0x2E] << 8) & 0xFF00;
    flashInfo.bootSectorSize = flashBase[0x2F];
    flashInfo.bootSectorSize |= (flashBase[0x30] << 8) & 0xFF00;

    flashInfo.bootSectorCount += 1;
    flashInfo.bootSectorSize *= 256;

    flashInfo.deviceArc = flashBase[0x4F];

    if (flashInfo.deviceArc != 2 && flashInfo.deviceArc != 3) {
      flashReset();
      return NORFLASH_UNSUPPORTED_DEVICE;
    }
  } else {
    flashReset();
    return NORFLASH_UNSUPPORTED_DEVICE;
  }
  flashInfo.deviceSize   = flashBase[0x27];
  flashInfo.deviceSize   = 1 << flashInfo.deviceSize;

  flashInfo.sectorCount += 1;
  flashInfo.sectorSize  *= 256;
  flashInfo.writeBufferSize = 1 << flashBase[0x2A];

  flashReset();

  flashInitialized = true;

  return NORFLASH_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Read flash status after a program or erase operation has been performed.
 *   This functions returns when the program/erase operation has completed or
 *   if an error is encountered.
 *
 * @note
 *   This function relies on the internal timeout mechanism in the flash device.
 *   It is therefore possible that this function never returns if the flash
 *   device is malfunctioning.
 *
 * @param[in] addr
 *   The address to check.
 *
 * @param[in] data
 *   The word value to check.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
static int flashPoll(uint32_t addr, uint16_t data)
{
  #define TOGGLE_BIT     0x40
  #define TIMEOUT_BIT    0x20

  uint16_t flashData1, flashData2, flashData3;

  flashData1 = *(volatile uint16_t*) addr;
  flashData2 = *(volatile uint16_t*) addr;

  if ((flashData1 == data)
      && (flashData2 == data)) {
    return NORFLASH_STATUS_OK;
  }

  while (1) {
    flashData3 = *(volatile uint16_t*) addr;

    if ((((flashData1 ^ flashData2) & TOGGLE_BIT) == TOGGLE_BIT)
        && (((flashData2 ^ flashData3) & TOGGLE_BIT) == TOGGLE_BIT)
        && ((flashData1 & TIMEOUT_BIT) == TIMEOUT_BIT)) {
      /* DQ6 is still toggling and DQ5 (timeout) is set */
      flashReset();
      return NORFLASH_WRITE_TIMEOUT;
    }

    if ((((flashData1 ^ flashData2) & TOGGLE_BIT) != TOGGLE_BIT)
        || (((flashData2 ^ flashData3) & TOGGLE_BIT) != TOGGLE_BIT)) {
      /* DQ6 has stopped toggling, do at least two reads. */
      *(volatile uint16_t*) addr;
      if (*(volatile uint16_t*) addr == data) {
        return NORFLASH_STATUS_OK;
      }

      /* Code will typically end here if attempting to program a 0 to a 1 */
      flashReset();
      return NORFLASH_WRITE_FAILURE;
    }

    flashData1 = flashData2;
    flashData2 = flashData3;
  }
  #undef TOGGLE_BIT
  #undef TIMEOUT_BIT
}

/***************************************************************************//**
 * @brief
 *   Perform a flash reset command.
 ******************************************************************************/
static void flashReset(void)
{
  flashBase[0] = 0xF0;
}

/***************************************************************************//**
 * @brief
 *   Perform a flash unlock command.
 ******************************************************************************/
static void flashUnlockCmd(void)
{
  flashBase[0x555] = 0xAA;
  flashBase[0x2AA] = 0x55;
}

/***************************************************************************//**
 * @brief
 *   Perform a "program buffer" operation.
 *
 * @param[in] sectorAddr
 *   The address of the sector.
 *
 * @param[in] addr
 *   The first address to program.
 *
 * @param[in] data
 *   Pointer to the data to be programmed.
 *
 * @param[in] count
 *   Number of bytes to be programmed.
 *
 * @return
 *   @ref NORFLASH_STATUS_OK on success, an error code enumerated in
 *   @ref NORFLASH_Status_TypeDef on failure.
 ******************************************************************************/
static int flashWriteBuffer(uint32_t sectorAddr,
                            uint32_t addr,
                            uint16_t *data,
                            uint32_t count)
{
  uint32_t          i;
  volatile uint16_t *pDst;

  pDst  = (volatile uint16_t*) addr;
  count = (count / 2) - 1;

  flashUnlockCmd();
  *(volatile uint16_t*) sectorAddr = 0x25;         /* Write buffer command */
  *(volatile uint16_t*) sectorAddr = count;        /* Word16count - 1      */
  for (i = 0; i <= count; i++) {
    *pDst++ = *data++;
  }
  *(volatile uint16_t*) sectorAddr = 0x29;         /* Write confirm        */

  pDst--;
  data--;
  return flashPoll((uint32_t) pDst, *data);
}

/** @endcond */

/** @} (end group NORFlash) */
/** @} (end addtogroup kitdrv) */
