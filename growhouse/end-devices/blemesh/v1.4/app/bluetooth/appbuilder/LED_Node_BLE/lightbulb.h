/***************************************************************************//**
 * @file
 * @brief Lightbulb module
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

#ifndef LIGHTBULB_H
#define LIGHTBULB_H

/* Bluetooth stack headers */
#include "native_gecko.h"

/***********************************************************************************************//**
 * \defgroup Lightbulb
 * \brief Lightbulb state and associated mesh models.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Lightbulb
 * @{
 **************************************************************************************************/

/***************************************************************************//**
 * Handling of lightbulb timer events.
 *
 * @param[in] evt  Pointer to incoming event.
 ******************************************************************************/
void handle_lightbulb_timer_evt(struct gecko_cmd_packet *evt);

/***************************************************************************//**
 * Lightbulb state initialization.
 * This is called at each boot if provisioning is already done.
 * Otherwise this function is called after provisioning is completed.
 ******************************************************************************/
void lightbulb_state_init(void);

/** @} (end addtogroup Lightbulb) */

#endif /* LIGHTBULB_H */
