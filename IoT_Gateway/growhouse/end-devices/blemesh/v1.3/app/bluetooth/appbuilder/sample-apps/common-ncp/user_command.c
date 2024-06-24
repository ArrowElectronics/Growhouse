/***********************************************************************************************//**
 * \file   user_command.c
 * \brief  Silabs Network Co-Processor (NCP) library
 *         This library allows customers create applications work in NCP mode.
 ***************************************************************************************************
 * <b> (C) Copyright 2017 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

#include "ncp_gecko.h"

/**
 * Default implementation to user command handling.
 *
 * @param payload the data payload
 */
void handle_user_command(const uint8_t* data)
{
  gecko_send_rsp_user_message_to_target(bg_err_not_implemented, 0, NULL);
}
