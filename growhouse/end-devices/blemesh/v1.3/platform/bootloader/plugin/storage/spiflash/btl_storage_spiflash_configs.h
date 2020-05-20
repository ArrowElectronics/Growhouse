/***************************************************************************//**
 * @file btl_storage_spiflash_configs.h
 * @brief Spiflash configuration
 * @author Silicon Labs
 * @version 1.6.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_STORAGE_SPIFLASH_CONFIGS
#define BTL_STORAGE_SPIFLASH_CONFIGS

#include "config/btl_config.h"

#include "btl_storage_spiflash.h"

/***************************************************************************//**
 * @addtogroup Storage
 * @{
 * @addtogroup SpiflashConfigs Spiflash Configurations
 * @brief Configuration parameters for SPI flashes
 * @details
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#define DEVICE_SIZE_256K                    (32768L)
#define DEVICE_SIZE_512K                    (65536L)
#define DEVICE_SIZE_1M                      (131072L)
#define DEVICE_SIZE_2M                      (262144L)
#define DEVICE_SIZE_4M                      (524288L)
#define DEVICE_SIZE_8M                      (1048576L)
#define DEVICE_SIZE_16M                     (2097152L)
#define DEVICE_SIZE_64M                     (8388608L)

// Pages are the write buffer granularity
#define DEVICE_PAGE_SIZE                    (256)
#define DEVICE_PAGE_MASK                    (255)
// Sectors are the erase granularity
// *except* for Numonyx parts which only support BLOCK erase granularity
#define DEVICE_SECTOR_SIZE                  (4096)
#define DEVICE_SECTOR_MASK                  (4095)
// Blocks define a larger erase granularity
#define DEVICE_BLOCK_SIZE_64K               (65536L)
#define DEVICE_BLOCK_MASK_64K               (65535L)
#define DEVICE_BLOCK_SIZE_32K               (32768L)
#define DEVICE_BLOCK_MASK_32K               (32767L)
// The flash word size in bytes
#define DEVICE_WORD_SIZE                    (1)

// JEDEC Manufacturer IDs
#define MFG_ID_SPANSION                     (0x01)
#define MFG_ID_WINBOND                      (0xEF)
#define MFG_ID_MACRONIX                     (0xC2)
#define MFG_ID_ATMEL                        (0x1F)
#define MFG_ID_ADESTO                       (0x1F)
#define MFG_ID_NUMONYX                      (0x20)
#define MFG_ID_ISSI                         (0x9D)

// JEDEC Device IDs
#define DEVICE_ID_SPANSION_8M               (0x4014)
#define DEVICE_ID_WINBOND_2M                (0x3012)
#define DEVICE_ID_WINBOND_4M                (0x3013)
#define DEVICE_ID_WINBOND_8M                (0x4014)
#define DEVICE_ID_MACRONIX_2M               (0x2012)
#define DEVICE_ID_MACRONIX_4M               (0x2013)
#define DEVICE_ID_MACRONIX_8M               (0x2014)
#define DEVICE_ID_MACRONIX_8M_LP            (0x2814)
#define DEVICE_ID_MACRONIX_16M              (0x2015)
#define DEVICE_ID_MACRONIX_16M_2V           (0x2535)
#define DEVICE_ID_MACRONIX_64M_LP           (0x2817)
#define DEVICE_ID_ATMEL_4M                  (0x4401)
#define DEVICE_ID_ATMEL_8M                  (0x4501)
#define DEVICE_ID_ADESTO_4M                 (0x8401)
#define DEVICE_ID_NUMONYX_2M                (0x2012)
#define DEVICE_ID_NUMONYX_4M                (0x2013)
#define DEVICE_ID_NUMONYX_8M                (0x2014)
#define DEVICE_ID_NUMONYX_16M               (0x2015)
#define DEVICE_ID_ISSI_256K                 (0x4009)
#define DEVICE_ID_ISSI_512K                 (0x4010)
#define DEVICE_ID_ISSI_1M                   (0x4011)
#define DEVICE_ID_ISSI_2M                   (0x4012)
#define DEVICE_ID_ISSI_4M                   (0x4013)

// Protocol commands
#define CMD_WRITE_ENABLE                    (0x06)
#define CMD_WRITE_DISABLE                   (0x04)
#define CMD_READ_STATUS                     (0x05)
#define CMD_WRITE_STATUS                    (0x01)
#define CMD_READ_DATA                       (0x03)
#define CMD_PAGE_PROG                       (0x02)
#define CMD_ERASE_SECTOR                    (0x20)
#define CMD_ERASE_BLOCK                     (0xD8)
#define CMD_ERASE_CHIP                      (0xC7)
#define CMD_POWER_DOWN                      (0xB9)
#define CMD_POWER_UP                        (0xAB)
#define CMD_JEDEC_ID                        (0x9F)
#define CMD_UNIQUE_ID                       (0x4B)

// Bitmasks for status register fields
#define STATUS_BUSY_MASK                    (0x01)
#define STATUS_WEL_MASK                     (0x02)

// These timings represent the worst case out of all chips supported by this
//  driver.  Some chips may perform faster.
// (in general Winbond is faster than Macronix is faster than Numonyx)
#define TIMING_POWERON_MAX_US               (10000)
#define TIMING_SLEEP_MAX_US                 (10)
#define TIMING_WAKEUP_MAX_US                (30)
#define TIMING_PROG_MAX_US                  (5000)
#define TIMING_WRITE_STATUS_MAX_US          (40000)
// (MS units are 1024Hz based)
#define TIMING_ERASE_SECTOR_MAX_MS          (410)
#define TIMING_ERASE_BLOCK_MAX_MS           (3072)

#define TIMING_ERASE_4K_MAX_MS              (410)
#define TIMING_ERASE_64K_MAX_MS             (3072)
#define TIMING_ERASE_SPANSION_8M_MAX_MS     (15360)
#define TIMING_ERASE_WINBOND_2M_MAX_MS      (2048)
#define TIMING_ERASE_WINBOND_4M_MAX_MS      (4096)
#define TIMING_ERASE_WINBOND_8M_MAX_MS      (6144)
#define TIMING_ERASE_MACRONIX_2M_MAX_MS     (3892)
#define TIMING_ERASE_MACRONIX_4M_MAX_MS     (7680)
#define TIMING_ERASE_MACRONIX_8M_MAX_MS     (15360)
#define TIMING_ERASE_MACRONIX_8M_LP_MAX_MS  (30720)
#define TIMING_ERASE_MACRONIX_16M_MAX_MS    (30720)
#define TIMING_ERASE_MACRONIX_16M_2V_MAX_MS (20480)
#define TIMING_ERASE_MACRONIX_64M_LP_MAX_MS (245760)
#define TIMING_ERASE_ATMEL_4M_MAX_MS        (7168)
#define TIMING_ERASE_ATMEL_8M_MAX_MS        (28672)
#define TIMING_ERASE_ADESTO_4M_MAX_MS       (12288)
#define TIMING_ERASE_NUMONYX_2M_MAX_MS      (6144)
#define TIMING_ERASE_NUMONYX_4M_MAX_MS      (10240)
#define TIMING_ERASE_NUMONYX_8M_MAX_MS      (20480)
#define TIMING_ERASE_NUMONYX_16M_MAX_MS     (40960)
#define TIMING_ERASE_ISSI_256K_MAX_MS       (512)
#define TIMING_ERASE_ISSI_512K_MAX_MS       (1024)
#define TIMING_ERASE_ISSI_1M_MAX_MS         (1536)
#define TIMING_ERASE_ISSI_2M_MAX_MS         (2048)
#define TIMING_ERASE_ISSI_4M_MAX_MS         (3072)

/** @endcond */

#ifdef DOXY_DOC_ONLY
/// Support all devices
#define BTL_STORAGE_SPIFLASH_ALL_DEVICES
#endif

#ifdef DOXY_DOC_ONLY
/// Support all Spansion devices
#define BTL_STORAGE_SPIFLASH_SPANSION_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_SPANSION_DEVICES)
/// Support Spansion S25FL208K
#define BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K  // 8MB
#endif

#ifdef DOXY_DOC_ONLY
/// Support all Winbond devices
#define BTL_STORAGE_SPIFLASH_WINBOND_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_WINBOND_DEVICES)
/// Support Winbond W25X20BV
#define BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV    // 2MB
/// Support Winbond W25Q80BV
#define BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV    // 8MB
#endif

#ifdef DOXY_DOC_ONLY
/// Support all Macronix devices
#define BTL_STORAGE_SPIFLASH_MACRONIX_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_DEVICES)
/// Support Macronix MX25L2006E
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E // 2MB
/// Support Macronix MX25L4006E
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E // 4MB
/// Support Macronix MX25L8006E
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E // 8MB
/// Support Macronix MX25R8035F
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F // 8MB Low Power
/// Support Macronix MX25L1606E
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E // 16MB
/// Support Macronix MX25U1635E
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E // 16MB 2V
/// Support Macronix MX25R6435F
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F // 64MB Low Power
#endif

#ifdef DOXY_DOC_ONLY
/// Support all Atmel devices
#define BTL_STORAGE_SPIFLASH_ATMEL_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_ATMEL_DEVICES) \
  || defined(BTL_STORAGE_SPIFLASH_ADESTO_DEVICES)
/// Support Atmel AT25DF041A
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A    // 4MB
/// Support Atmel AT25DF081A
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A    // 8MB
/// Support Adesto AT25SF041
#define BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041    // 4MB
#endif

#ifdef DOXY_DOC_ONLY
/// Support all Numonyx devices
#define BTL_STORAGE_SPIFLASH_NUMONYX_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_DEVICES)
/// Support Numonyx M26P20
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P20      // 2MB
/// Support Numonyx M26P40
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P40      // 4MB
/// Support Numonyx M26P80
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P80      // 8MB
/// Support Numonyx M26P16
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P16      // 16MB
#endif

#ifdef DOXY_DOC_ONLY
/// Support all ISSI devices
#define BTL_STORAGE_SPIFLASH_ISSI_DEVICES
#endif

#if defined(BTL_STORAGE_SPIFLASH_ISSI_DEVICES)
/// Support ISSI IS25LQ025B
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B     // 256kB
/// Support ISSI IS25LQ512B
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B     // 512kB
/// Support ISSI IS25LQ010B
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B     // 1MB
/// Support ISSI IS25LQ020B
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B     // 2MB
/// Support ISSI IS25LQ040B
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B     // 4MB
#endif

#if (defined(BTL_STORAGE_SPIFLASH_ALL_DEVICES)          \
  || (!defined(BTL_STORAGE_SPIFLASH_SPANSION_DEVICES)   \
  && !defined(BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K)  \
  && !defined(BTL_STORAGE_SPIFLASH_WINBOND_DEVICES)     \
  && !defined(BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV)    \
  && !defined(BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV)    \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_DEVICES)    \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E) \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E) \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E) \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F) \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E) \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E) \
  && !defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F) \
  && !defined(BTL_STORAGE_SPIFLASH_ATMEL_DEVICES)       \
  && !defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A)    \
  && !defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A)    \
  && !defined(BTL_STORAGE_SPIFLASH_NUMONYX_DEVICES)     \
  && !defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P20)      \
  && !defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P40)      \
  && !defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P80)      \
  && !defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P16)      \
  && !defined(BTL_STORAGE_SPIFLASH_ISSI_DEVICES)        \
  && !defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B)     \
  && !defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B)     \
  && !defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B)     \
  && !defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B)     \
  && !defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B)))   \
  && !defined(DOXY_DOC_ONLY)
#define BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K  // 8MB
#define BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV    // 2MB
#define BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV    // 8MB
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E // 2MB
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E // 4MB
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E // 8MB
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F // 8MB Low Power
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E // 16MB
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E // 16MB 2V
#define BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F // 64MB Low Power
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A    // 4MB
#define BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A    // 8MB
#define BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041    // 4MB
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P20      // 2MB
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P40      // 4MB
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P80      // 8MB
#define BTL_STORAGE_SPIFLASH_NUMONYX_M25P16      // 16MB
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B     // 256kB
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B     // 512kB
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B     // 1MB
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B     // 2MB
#define BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B     // 4MB
#endif

#if defined(BTL_STORAGE_SPIFLASH_SPANSION_S25FL208K)
/// Information for Spansion S25L208K
static const BootloaderStorageImplementationInformation_t spansion8MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_4K_MAX_MS,
  TIMING_ERASE_SPANSION_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "S25FL208K",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_WINBOND_W25X20BV)
/// Information for Winbond W25X20BV
static const BootloaderStorageImplementationInformation_t windbond2MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_WINBOND_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "W25X20BV",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_WINBOND_W25Q80BV)
/// Information for Winbond W25Q80BV
static const BootloaderStorageImplementationInformation_t windbond8MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_WINBOND_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "W25Q80BV",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L2006E)
/// Information for Macronix MX25L2006E
static const BootloaderStorageImplementationInformation_t macronix2MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "MX25L2006E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L4006E)
/// Information for Macronix MX25L4006E
static const BootloaderStorageImplementationInformation_t macronix4MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_4K_MAX_MS,
  TIMING_ERASE_MACRONIX_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "MX25L4006E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L8006E)
/// Information for Macronix MX25L8006E
static const BootloaderStorageImplementationInformation_t macronix8MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "MX25L8006E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R8035F)
/// Information for Macronix MX25R8035F
static const BootloaderStorageImplementationInformation_t macronix8MLPInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_8M_LP_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "MX25R8035F",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25L1606E)
/// Information for Macronix MX25L1606E
static const BootloaderStorageImplementationInformation_t macronix16MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_16M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_16M,
  "MX25L1606E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25U1635E)
/// Information for Macronix MX25U1635E
static const BootloaderStorageImplementationInformation_t macronix16M2VInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_16M_2V_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_16M,
  "MX25U1635E",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_MACRONIX_MX25R6435F)
/// Information for Macronix MX25R6435F
static const BootloaderStorageImplementationInformation_t macronix64MLPInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_MACRONIX_64M_LP_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_64M,
  "MX25R6435F",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF041A)
/// Information for Atmel AT25DF041A
static const BootloaderStorageImplementationInformation_t atmel4MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "AT25DF041A",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ATMEL_AT25DF081A)
/// Information for Atmel AT25DF081A
static const BootloaderStorageImplementationInformation_t atmel8MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ATMEL_8M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_8M,
  "AT25DF081A",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ADESTO_AT25SF041)
/// Information for Adesto AT25SF041
static const BootloaderStorageImplementationInformation_t adesto4MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ADESTO_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "AT25SF041",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P20)
/// Information for Numonyx M25P20
static const BootloaderStorageImplementationInformation_t numonyx2MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_2M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_2M,
  "M25P20",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P40)
/// Information for Numonyx M25P40
static const BootloaderStorageImplementationInformation_t numonyx4MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_4M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_4M,
  "M25P40",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P80)
/// Information for Numonyx M25P80
static const BootloaderStorageImplementationInformation_t numonyx8MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_8M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_8M,
  "M25P80",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_NUMONYX_M25P16)
/// Information for Numonyx M25P16
static const BootloaderStorageImplementationInformation_t numonyx16MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_BLOCK_MAX_MS,
  TIMING_ERASE_NUMONYX_16M_MAX_MS,
  DEVICE_BLOCK_SIZE_64K, // Numonyx does not support smaller sector erase
  DEVICE_SIZE_16M,
  "M25P16",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ025B)
/// Information for ISSI IS25LQ025B
static const BootloaderStorageImplementationInformation_t issi256KInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_256K_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_256K,
  "IS25LQ025B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ512B)
/// Information for ISSI IS25LQ512B
static const BootloaderStorageImplementationInformation_t issi512KInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_512K_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_512K,
  "IS25LQ512B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ010B)
/// Information for ISSI IS25LQ010B
static const BootloaderStorageImplementationInformation_t issi1MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_1M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_1M,
  "IS25LQ010B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ020B)
/// Information for ISSI IS25LQ020B
static const BootloaderStorageImplementationInformation_t issi2MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_2M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_2M,
  "IS25LQ020B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

#if defined(BTL_STORAGE_SPIFLASH_ISSI_IS25LQ040B)
/// Information for ISSI IS25LQ040B
static const BootloaderStorageImplementationInformation_t issi4MInfo = {
  BOOTLOADER_STORAGE_IMPL_INFO_VERSION,
  (BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED
   | BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED),
  TIMING_ERASE_SECTOR_MAX_MS,
  TIMING_ERASE_ISSI_4M_MAX_MS,
  DEVICE_SECTOR_SIZE,
  DEVICE_SIZE_4M,
  "IS25LQ040B",
  DEVICE_WORD_SIZE // word size in bytes
};
#endif

/** @} // addtogroup SpiflashConfigs */
/** @} // addtogroup Storage */

#endif // BTL_STORAGE_SPIFLASH_CONFIGS
