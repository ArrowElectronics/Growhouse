/***************************************************************************//**
 * @file
 * @brief sl_bgapi.c
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

 #include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "sl_bgapi.h"
#include "sl_security.h"
#include "config.h"
#include "host_gecko.h"

static sl_bgapi_context_t *ncp_target = NULL;
static char tbuf[MAX_PACKET_SIZE * 2];
static unsigned tlen;

void sl_bgapi_reset(sl_bgapi_context_t *ctx)
{
  ctx->recv_len = 0;
}

void sl_bgapi_recv_data(sl_bgapi_context_t *ctx, char *buf, int len)
{
  uint32_t hdr;

  if (ctx->recv_len + len > sizeof(ctx->recv_buf)) {//buffer overflowing, move old data to at least fit new data
    memmove(&ctx->recv_buf[0], &ctx->recv_buf[ctx->recv_len], sizeof(ctx->recv_buf) - ctx->recv_len);
    ctx->recv_len -= len;
  }
  //copy new data at end of buffer
  memcpy(&ctx->recv_buf[ctx->recv_len], buf, len);
  ctx->recv_len += len;
  if (len < 1) {
    return;
  }
  //validate packet
  while (ctx->recv_len > BGLIB_MSG_HEADER_LEN) {
    memcpy(&hdr, ctx->recv_buf, BGLIB_MSG_HEADER_LEN);
    if (((ctx->recv_buf[0] & 0x20) == 0) || (BGLIB_MSG_LEN(hdr) > BGLIB_MSG_MAX_PAYLOAD)) {
      //invalid packet, drop 1st byte
      ctx->recv_len--;
      memmove(&ctx->recv_buf[0], &ctx->recv_buf[1], ctx->recv_len - 1);
      continue;
    }
    unsigned packet_len = BGLIB_MSG_LEN(hdr) + BGLIB_MSG_HEADER_LEN;
    if (packet_len > ctx->recv_len) {//not enough data for a packet
      return;
    }
    if (BGLIB_MSG_ENCRYPTED(hdr)) {
      tlen = packet_len;

      sl_security_decrypt_packet(&ctx->recv_buf[0], tbuf, &tlen);

      sl_bgapi_process_packet_cb(ctx, tbuf, tlen, 1);
    } else {
      switch (BGLIB_MSG_ID(hdr)) {
        case gecko_rsp_user_message_to_target_id:
          //response to security handshake
          if (packet_len == 80
              && ctx->recv_buf[4] == 0 && ctx->recv_buf[5] == 0
              && ctx->recv_buf[6] == 0x49 && ctx->recv_buf[7] == 0x0) {
            uint8_t public[64];
            uint8_t target_iv_to_target[4];
            uint8_t target_iv_to_host[4];
            memcpy(public, &ctx->recv_buf[8], 64);
            memcpy(target_iv_to_target, &ctx->recv_buf[72], 4);
            memcpy(target_iv_to_host, &ctx->recv_buf[76], 4);
            sl_security_increase_security_rsp(public, target_iv_to_target, target_iv_to_host);
          } else {
            printf("NCP Encryption Failed 0x%02x%02x\n", ctx->recv_buf[5], ctx->recv_buf[4]);
          }
          break;
        case gecko_evt_system_boot_id:
        case gecko_evt_dfu_boot_id:
          //ncp target is resetted, reset security system
          printf("NCP Target Reset\n");
          sl_security_reset();
          break;
      }
      //process packet
      sl_bgapi_process_packet_cb(ctx, &ctx->recv_buf[0], packet_len, 0);
    }

    //drop packet from buffer
    memmove(&ctx->recv_buf[0], &ctx->recv_buf[packet_len], ctx->recv_len - packet_len);
    ctx->recv_len -= packet_len;
  }
}
void sl_bgapi_context_init(sl_bgapi_context_t *ctx, int fd)
{
  ctx->fd = fd;
}
void sl_bgapi_set_ncp_target(sl_bgapi_context_t *ctx)
{
  ncp_target = ctx;
}
int sl_bgapi_user_cmd_increase_security(uint8_t *public_key, uint8_t *host_iv_to_target, uint8_t *host_iv_to_host)
{
  uint8_t bgapi_packet[78] = { 0x20, 0x4a, 0xff, 0x00, 0x49, 0x00 };
  memcpy(bgapi_packet + 6, public_key, 64);
  memcpy(bgapi_packet + 70, host_iv_to_target, 4);
  memcpy(bgapi_packet + 74, host_iv_to_host, 4);
  int len = sizeof(bgapi_packet);
  uint8_t *ptr = (uint8_t* )bgapi_packet;
  if (ncp_target != NULL) {
    while (len) {
      int l = write(ncp_target->fd, ptr, len);
      if (l < 0) {
        //TODO:Error
        return -1;
      }
      len -= l;
      ptr += l;
    }
  }
  return 0;
}
