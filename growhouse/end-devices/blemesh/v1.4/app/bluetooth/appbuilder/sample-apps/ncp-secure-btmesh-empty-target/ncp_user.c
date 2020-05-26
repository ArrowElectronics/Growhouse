/***********************************************************************************************//**
 * \file   itest_ncp_user.c
 * \brief  Silabs Network Co-Processor (NCP) library
 *         This library allows customers create applications work in NCP mode.
 ***************************************************************************************************
 * <b> (C) Copyright 2017 Silicon Labs, http://www.silabs.com</b>
 ***************************************************************************************************
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 **************************************************************************************************/

#include <stddef.h>
#include "ncp_user.h"
#include "ncp_sec.h"

uint8_t user_event_data_len;
uint8_t user_event_data[USER_EVENT_DATA_MAX_LEN];

/**
 * Override for integration testing.
 *
 * @param payload the data payload
 */
void handle_user_command(uint8_t* data)
{
  uint8array* payload = (uint8array*)data;
  errorcode_t result = bg_err_success;
  user_event_data_len = payload->len > USER_EVENT_DATA_MAX_LEN ? USER_EVENT_DATA_MAX_LEN : payload->len;
  memcpy(user_event_data, payload->data, user_event_data_len);
  struct user_cmd_packet* cmd = (struct user_cmd_packet*)user_event_data;
  switch (cmd->code) {
    case user_cmd_increase_security:
      if ((user_event_data_len - 1) != sizeof(struct user_msg_ncp_increase_security_t)) {
        gecko_send_rsp_user_message_to_target(bg_err_invalid_param, 0, NULL);
        break;
      }
      result = ncp_sec_increase_security(
        cmd->data.ncp_increase_security.public_keys,
        cmd->data.ncp_increase_security.host_iv_to_target,
        cmd->data.ncp_increase_security.host_iv_to_host);
      gecko_send_rsp_user_message_to_target(result, sizeof(struct user_msg_ncp_increase_security_t) + 1, user_event_data);
      break;
    default:
      // Start one shot timer for generating a user event
      gecko_cmd_hardware_set_soft_timer(1000, USER_EVENT_ID, 1);
      // Loop back payload.
      gecko_send_rsp_user_message_to_target(result, payload->len, payload->data);
      break;
  }
}
