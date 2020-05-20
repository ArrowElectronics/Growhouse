/******************************************************************************/
/**
 * @file ezsp-host-configuration.h
 * @brief Configuration header for EZSP Host
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2015 Silicon Labs, www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/
// *******************************************************************
// Stack Profile Parameters

// *******************************************************************
// Stack Memory Requirements

#define EMBER_CHILD_TABLE_TOKEN_SIZE                32

#ifdef EMBER_USING_TREE_STACK
  #define EMBER_APS_INDIRECT_BINDING_TABLE_TOKEN_SIZE 32
#else
  #define EMBER_BINDING_TABLE_TOKEN_SIZE              32
#endif

#define EMBER_TRANSPORT_CONNECTION_COUNT            0

// *******************************************************************
// Application Handlers

#define EZSP_APPLICATION_HAS_TIMER_HANDLER

// *******************************************************************
// Application Configuration

#define EZSP_TOKEN_SIZE    8
#define EZSP_TOKEN_ENTRIES 8

#define EMBER_ASSERT_SERIAL_PORT 0
