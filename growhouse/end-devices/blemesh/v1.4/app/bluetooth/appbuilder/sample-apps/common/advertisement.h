/***************************************************************************//**
 * @file
 * @brief Header file related to advertisement
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

#ifndef APP_ADV_H
#define APP_ADV_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************************//**
 * \defgroup adv Advertisement Code
 * \brief Advertisement API
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Advertisement
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup adv
 * @{
 **************************************************************************************************/

/***************************************************************************************************
   Public Macros and Definitions
***************************************************************************************************/

#define ADV_TX_POWER      0xc3
#define ADV_FLAGS_LEN     2
#define ADV_UUID_LEN      5
#define ADV_TX_POWER_LEN  2

/***************************************************************************************************
   Public Function Declarations
***************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Setup advertising.
 **************************************************************************************************/
void advSetup(void);

/**********************************************************************************************//**
*  \brief  Stop advertising current message and prepare for advertising a different message.
**************************************************************************************************/
void advSwitchAdvMessage(void);

/***********************************************************************************************//**
 *  \brief  Indicate that connection has started.
 **************************************************************************************************/
void advConnectionStarted(void);

/** @} (end addtogroup adv) */
/** @} (end addtogroup Advertisement) */

#endif /* APP_ADV_H */
