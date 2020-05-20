/***************************************************************************//**
 * @file BGAPI functions of NCP daemon
 * @brief sl_bgapi.h
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

 #ifndef SL_BGAPI_H
#define SL_BGAPI_H

#include "config.h"

typedef struct {
  int fd;
  unsigned recv_len;
  char recv_buf[MAX_PACKET_SIZE * 2];
}sl_bgapi_context_t;

/***********************************************************************************************//**
 *  \brief  Reset BGAPI.
 *  \param ctx BGAPI context to reset.
 **************************************************************************************************/
void sl_bgapi_reset(sl_bgapi_context_t *ctx);

/***********************************************************************************************//**
 *  \brief  Initialize BGAPI context.
 *  \param  ctx BGAPI context to initialize.
 *  \param fd file descriptor of socket.
 **************************************************************************************************/
void sl_bgapi_context_init(sl_bgapi_context_t *ctx, int fd);

/***********************************************************************************************//**
 *  \brief  Set NCP Target to a context.
 *  \param  ctx BGAPI context.
 **************************************************************************************************/
void sl_bgapi_set_ncp_target(sl_bgapi_context_t *ctx);

/***********************************************************************************************//**
 *  \brief  Receive BGAPI data.
 *  \param  ctx BGAPI context.
 *  \param buf pointer to new BGAPI data.
 *  \param len length of new BGAPI data.
 **************************************************************************************************/
void sl_bgapi_recv_data(sl_bgapi_context_t *ctx, char *buf, int len);

/***********************************************************************************************//**
 *  \brief  Process BGAPI callback.
 *  \param  ctx BGAPI context.
 *  \param buf pointer to new BGAPI data.
 *  \param len length of new BGAPI data.
 *  \param encrypted Encryption state.
 **************************************************************************************************/
void sl_bgapi_process_packet_cb(sl_bgapi_context_t *ctx, char *buf, int len, int encrypted);

/***********************************************************************************************//**
 *  \brief  Function to initate encrcyption with the target.
 *  \param  public_key Public Key.
 *  \param host_iv_to_target Initialization Vector to Target.
 *  \param host_iv_to_target Initialization Vector to Host.
 *  \return  0 on success, -1 on failure.
 **************************************************************************************************/
int sl_bgapi_user_cmd_increase_security(uint8_t *public_key, uint8_t *host_iv_to_target, uint8_t *host_iv_to_host);

#endif
