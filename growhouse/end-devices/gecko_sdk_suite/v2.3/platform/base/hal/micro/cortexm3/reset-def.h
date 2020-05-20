/** @file hal/micro/cortexm3/reset-def.h
 * @brief Definitions for all the reset cause types
 *
 * <!-- Copyright 2009 by Ember Corporation.             All rights reserved.-->
 */

/** @addtogroup reset_def
 * Definitions for all the reset cause types.
 *
 * Reset cause types are built from a base definition and an extended.
 * definition. The base definitions allow working with entire categories of
 * resets while the extended definitions allow drilling down to very
 * specific causes.  The macros for the base and extended definitions are
 * combined for use in the code and equated to their combined numberical
 * equivalents.  In addition, exach base and extended definition is given
 * a corresponding 3 letter ASCII string to facilitate printing.  The ASCII
 * strings are best use with ::halGetExtendedResetString.
 *
 * For example:
 * \code
 * RESET_BASE_DEF(EXTERNAL,            0x03U,     "EXT")
 *   RESET_EXT_DEF(EXTERNAL, UNKNOWN,    0x00U,     "UNK")
 *   RESET_EXT_DEF(EXTERNAL, PIN,        0x01U,     "PIN")
 * \endcode
 * results in enums which includes the entries:
 * \code
 * RESET_EXTERNAL = 0x03U
 * RESET_EXTERNAL_PIN = 0x0301U
 * \endcode
 *
 * For a complete listing of all reset base and extended definitions, see
 * reset-def.h for source code.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// *******************
// This file is specifically kept wider than 80 columns in order to keep the information
//   well organized and easy to read by glancing down the columns
// *******************

// The reset types of the EM300 series have both a base type and an
//  extended type.  The extended type is a 16-bit value which has the base
//  type in the upper 8-bits, and the extended classification in the
//  lower 8-bits
// For backwards compatibility with other platforms, only the base type is
//  returned by the halGetResetInfo() API.  For the full extended type, the
//  halGetExtendedResetInfo() API should be called.

// RESET_BASE_DEF macros take the following parameters:
//    RESET_BASE_DEF(basename, value, string)  // description
//        basename - the name used in the enum definition, expanded as RESET_basename
//        value - the value of the enum definition
//        string - the reset string, must be 3 characters
//        description - a comment describing the cause

// RESET_EXT_DEF macros take the following parameters:
//    RESET_EXT_DEF(basename, extname, extvalue, string)  // description
//        basename - the base name used in the enum definition
//        extname - the extended name used in the enum definition, expanded as RESET_basename_extname
//        extvalue - the LSB value of the enum definition, combined with the value of the base value in the MSB
//        string - the reset string, must be 3 characters
//        description - a comment describing the cause

// *******************
// This file is specifically kept wider than 80 columns in order to keep the information
//   well organized and easy to read by glancing down the columns
// (Yes, this comment is mentioned multiple times in the file)
// *******************

// This file wants to use the bareword BOOTLOADER, which is sometimes a symbol.
// To allow this we will undefine the symbol here and restore it at the end of
// this file.
#ifdef BOOTLOADER
  #define SAVED_BOOTLOADER BOOTLOADER
  #undef BOOTLOADER
#endif








RESET_BASE_DEF(UNKNOWN, 0x00U, "UNK")                    // Underterminable cause
RESET_EXT_DEF(UNKNOWN, UNKNOWN, 0x00U, "UNK")               // Undeterminable cause

RESET_BASE_DEF(FIB, 0x01U, "FIB")                          // Reset originated from the FIB bootloader
RESET_EXT_DEF(FIB, GO, 0x00U, "GO ")                          // FIB bootloader caused a reset to main flash
RESET_EXT_DEF(FIB, BOOTLOADER, 0x01U, "BTL")                  // FIB bootloader is instructing ember bootloader to run
RESET_EXT_DEF(FIB, GO2, 0x02U, "GO2")                         // GO2 (unused)
RESET_EXT_DEF(FIB, GO3, 0x03U, "GO3")                         // GO3 (unused)
RESET_EXT_DEF(FIB, GO4, 0x04U, "GO4")                         // GO4 (unused)
RESET_EXT_DEF(FIB, GO5, 0x05U, "GO5")                         // GO5 (unused)
RESET_EXT_DEF(FIB, GO6, 0x06U, "GO6")                         // GO6 (unused)
RESET_EXT_DEF(FIB, GO7, 0x07U, "GO7")                         // GO7 (unused)
RESET_EXT_DEF(FIB, GO8, 0x08U, "GO8")                         // GO8 (unused)
RESET_EXT_DEF(FIB, GO9, 0x09U, "GO9")                         // GO9 (unused)
RESET_EXT_DEF(FIB, GOA, 0x0AU, "GOA")                         // GOA (unused)
RESET_EXT_DEF(FIB, GOB, 0x0BU, "GOB")                         // GOB (unused)
RESET_EXT_DEF(FIB, GOC, 0x0CU, "GOC")                         // GOC (unused)
RESET_EXT_DEF(FIB, GOD, 0x0DU, "GOD")                         // GOD (unused)
RESET_EXT_DEF(FIB, GOE, 0x0EU, "GOE")                         // GOE (unused)
RESET_EXT_DEF(FIB, GOF, 0x0FU, "GOF")                         // GOF (unused)
RESET_EXT_DEF(FIB, JUMP, 0x10U, "JMP")                        // FIB bootloader is jumping to a specific flash address
RESET_EXT_DEF(FIB, BAUDRATE, 0x11U, "BDR")                    // FIB bootloader detected a high baud rate, causes ember bootloader to run
RESET_EXT_DEF(FIB, UNPROTECT, 0x12U, "UPR")                   // Read protection disabled, flash should be erased
RESET_EXT_DEF(FIB, BOOTMODE, 0x13U, "BTM")                    // BOOTMODE was not held long enough
RESET_EXT_DEF(FIB, MISMATCH, 0x14U, "MSM")                    // MISMATCHED FIB Bootloader & Part Data
RESET_EXT_DEF(FIB, FATAL, 0x15U, "FTL")                       // FIB Fatal Error

RESET_BASE_DEF(BOOTLOADER, 0x02U, "BTL")                   // Reset relates to an Ember bootloader
RESET_EXT_DEF(BOOTLOADER, UNKNOWN, 0x00U, "UNK")               // Unknown bootloader cause (should never occur)
RESET_EXT_DEF(BOOTLOADER, GO, 0x01U, "GO ")                    // Bootloader caused reset telling app to run
RESET_EXT_DEF(BOOTLOADER, BOOTLOAD, 0x02U, "BTL")              // Application requested that bootloader runs
RESET_EXT_DEF(BOOTLOADER, BADIMAGE, 0x03U, "BAD")              // Application bootloader detect bad external upgrade image
RESET_EXT_DEF(BOOTLOADER, FATAL, 0x04U, "FTL")                 // Fatal Error or assert in bootloader
RESET_EXT_DEF(BOOTLOADER, FORCE, 0x05U, "FRC")                 // Forced bootloader activation
RESET_EXT_DEF(BOOTLOADER, OTAVALID, 0x06U, "OTA")              // OTA Bootloader mode activation
RESET_EXT_DEF(BOOTLOADER, DEEPSLEEP, 0x07U, "DSL")             // Bootloader initiated deep sleep



RESET_BASE_DEF(EXTERNAL, 0x03U, "EXT")                     // External reset trigger
RESET_EXT_DEF(EXTERNAL, UNKNOWN, 0x00U, "UNK")                // Unknown external cause (should never occur)
RESET_EXT_DEF(EXTERNAL, PIN, 0x01U, "PIN")                    // External pin reset

RESET_BASE_DEF(POWERON, 0x04U, "PWR")                      // Poweron reset type, supply voltage < power-on threshold
RESET_EXT_DEF(POWERON, UNKNOWN, 0x00U, "UNK")                 // Unknown poweron reset (should never occur)
RESET_EXT_DEF(POWERON, HV, 0x01U, "HV ")                      // High voltage poweron
RESET_EXT_DEF(POWERON, LV, 0x02U, "LV ")                      // Low voltage poweron

RESET_BASE_DEF(WATCHDOG, 0x05U, "WDG")                     // Watchdog reset occurred
RESET_EXT_DEF(WATCHDOG, UNKNWON, 0x00U, "UNK")                // Unknown watchdog reset (should never occur)
RESET_EXT_DEF(WATCHDOG, EXPIRED, 0x01U, "EXP")                // Watchdog timer expired
RESET_EXT_DEF(WATCHDOG, CAUGHT, 0x02U, "LWM")                 // Watchdog low watermark expired and caught extended info

RESET_BASE_DEF(SOFTWARE, 0x06U, " SW")                     // Software triggered reset
RESET_EXT_DEF(SOFTWARE, UNKNOWN, 0x00U, "UNK")                // Unknown software cause
RESET_EXT_DEF(SOFTWARE, REBOOT, 0x01U, "RBT")                 // General software reboot
RESET_EXT_DEF(SOFTWARE, DEEPSLEEP, 0x02U, "DSL")              // App initiated deep sleep

RESET_BASE_DEF(CRASH, 0x07U, "CRS")                        // Software crash
RESET_EXT_DEF(CRASH, UNKNOWN, 0x00U, "UNK")                   // Unknown crash
RESET_EXT_DEF(CRASH, ASSERT, 0x01U, "AST")                    // a self-check assert in the code failed

RESET_BASE_DEF(FLASH, 0x08U, "FSH")                        // Flash failure cause reset
RESET_EXT_DEF(FLASH, UNKNWON, 0x00U, "UNK")                   // Unknown flash failure
RESET_EXT_DEF(FLASH, VERIFY, 0x01U, "VFY")                    // Flash write verify failure
RESET_EXT_DEF(FLASH, INHIBIT, 0x02U, "INH")                   // Flash write inhibited: already written

RESET_BASE_DEF(FATAL, 0x09U, "BAD")                        // A non-recoverable fatal error occurred
RESET_EXT_DEF(FATAL, UNKNOWN, 0x00U, "UNK")                   // Unknown fatal error (should never occur)
RESET_EXT_DEF(FATAL, LOCKUP, 0x01U, "LCK")                    // CPU Core locked up
RESET_EXT_DEF(FATAL, CRYSTAL, 0x02U, "XTL")                   // 24MHz crystal failure
RESET_EXT_DEF(FATAL, OPTIONBYTE, 0x03U, "OBF")                // option byte complement error

RESET_BASE_DEF(FAULT, 0x0AU, "FLT")                        // A access fault occurred
RESET_EXT_DEF(FAULT, UNKNOWN, 0x00U, "UNK")                   // An unknown fault occurred
RESET_EXT_DEF(FAULT, HARD, 0x01U, "HRD")                      // Hard fault
RESET_EXT_DEF(FAULT, MEM, 0x02U, "MEM")                       // Memory protection violation
RESET_EXT_DEF(FAULT, BUS, 0x03U, "BUS")                       // Bus fault
RESET_EXT_DEF(FAULT, USAGE, 0x04U, "USG")                     // Usage fault
RESET_EXT_DEF(FAULT, DBGMON, 0x05U, "DBG")                    // Debug monitor fault
RESET_EXT_DEF(FAULT, PROTDMA, 0x06U, "DMA")                   // DMA RAM protection violation
RESET_EXT_DEF(FAULT, BADVECTOR, 0x07U, "VCT")                 // Uninitialized interrupt vector

// Restore the value of the BOOTLOADER symbol if we had to save it off in this
// file so that the word BOOTLOADER could be used.
#ifdef SAVED_BOOTLOADER
  #define BOOTLOADER SAVED_BOOTLOADER
  #undef SAVED_BOOTLOADER
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS
