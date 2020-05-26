/***************************************************************************//**
 * @file
 * @brief Silabs Network Co-Processor (NCP) security library
 * This library allows customers create applications work in NCP mode.
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

#ifndef NCP_SEC_H_
#define NCP_SEC_H_

#include <stdbool.h>
#include <stdint.h>
#include "ncp_gecko.h"
#include "bg_errorcodes.h"

enum ncp_sec_state {
  ncp_sec_unencrypted = 0,
  ncp_sec_encrypted,
  ncp_sec_encrypted_authenticated
};

errorcode_t ncp_sec_increase_security(uint8_t* public_keys, uint8_t *host_iv_to_target, uint8_t *host_iv_to_host);

/***************************************************************************//**
 * @brief
 *   Tells whether the given BGAPI message is encrypted.
 *
 * @param[in] msg
 *   Pointer to the BGAPI message.
 *
 * @return
 *   True if the message is encrypted, otherwise False.
 *
 ******************************************************************************/
bool ncp_sec_is_encrypted(volatile void* msg);

/***************************************************************************//**
 * @brief
 *   Validate if received packet has right level of security.
 *
 * @details
 *   Only certain unencrypted commands are allowed when the security is
 *   encrypted or higher. No encrypted commands are allowed when the security
 *   state is unencrypted.
 *
 * @param[in] msg
 *   Pointer to NCP command packet.
 *
 * @return
 *   True if security is valid, otherwise False.
 *
 ******************************************************************************/
bool ncp_sec_validate_security(volatile uint8_t* msg);

/***************************************************************************//**
 * @brief
 *   Set callback function for transmit.
 *
 * @details
 *   This function is used to update the payload length in NCP packet header.
 *
 * @param[in] msg
 *   Pointer to NCP packet.
 *
 * @param[in] length
 *   New length of payload.
 *
 ******************************************************************************/
void ncp_sec_update_payload_length(struct gecko_cmd_packet* msg, uint16_t length);

/***************************************************************************//**
 * @brief
 *   Process the response packet for security. The response will be encrypted
 *   if the command it is responding to was encrypted.
 *
 * @param[in] msg
 *   Pointer to the response packet
 * @param[in] cmd_was_encrypted
 *   Indicates whether the command this packet is responding to was encrypted
 *
 * @return
 *   Pointer to the packet ready to send over NCP transport
 ******************************************************************************/
struct gecko_cmd_packet* ncp_sec_process_response(struct gecko_cmd_packet* msg, bool cmd_was_encrypted);

/***************************************************************************//**
 * @brief
 * Process the event packet for security. The event will be encrypted
 * if the transport is in encryption mode.
 *
 * @param[in] msg
 *   Pointer to the event packet
 *
 * @return
 *   Pointer to the packet ready to send over NCP transport
 ******************************************************************************/
struct gecko_cmd_packet* ncp_sec_process_event(struct gecko_cmd_packet* msg);

/***************************************************************************//**
 * @brief
 *   Decrypt the command packet.
 *
 * @param[in] msg
 *   Pointer to NCP packet.
 *
 * @return
 *   Pointer to the new decrypted packet.
 ******************************************************************************/
struct gecko_cmd_packet* ncp_sec_decrypt_command(struct gecko_cmd_packet* msg);

/***************************************************************************//**
 * @brief
 *   Gets the current NCP security state
 *
 * @return
 *   the current NCP security state
 ******************************************************************************/
enum ncp_sec_state ncp_sec_get_state();

#endif /* NCP_SEC_H_ */
