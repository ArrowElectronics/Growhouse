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

#include <string.h>
#include "ncp_sec.h"
#include "ncp.h"
#include "ecp.h"
#include "ecdh.h"
#include "sha256.h"
#include "ccm.h"

#define KEY_SIZE 16
#define PUBLIC_KEY_SIZE 32

typedef struct conn_nonce{
  uint32_t counter;
  uint8_t counter_hi;
  uint8_t host_iv[4];
  uint8_t target_iv[4];
}conn_nonce_t;

typedef struct public_key{
  uint8_t x[PUBLIC_KEY_SIZE];
  uint8_t y[PUBLIC_KEY_SIZE];
}public_key_t;

static uint8_t ccm_key[KEY_SIZE];

static conn_nonce_t ncp_sec_counter_in;
static conn_nonce_t ncp_sec_counter_out;
static mbedtls_ecp_point public;
static mbedtls_mpi private;
static mbedtls_ecp_group ecp_grp;
static mbedtls_ccm_context ccm_ctx;

static uint8_t encrypted_msg[NCP_CMD_SIZE];
static uint8_t decrypted_msg[NCP_CMD_SIZE];

static enum ncp_sec_state ncp_sec_state;

static void increase_counter(conn_nonce_t* counter)
{
  if (counter->counter == UINT32_MAX) {
    counter->counter_hi++;
  }
  counter->counter++;
}

static int verify_public_key(const uint8_t *pub_x, const uint8_t *pub_y)
{
  mbedtls_ecp_point public_key;
  mbedtls_ecp_point_init(&public_key);

  mbedtls_mpi_read_binary(&public_key.X, pub_x, PUBLIC_KEY_SIZE);
  mbedtls_mpi_read_binary(&public_key.Y, pub_y, PUBLIC_KEY_SIZE);
  mbedtls_mpi_lset(&public_key.Z, 1);

  int ret = mbedtls_ecp_check_pubkey(&ecp_grp, &public_key);
  mbedtls_ecp_point_free(&public_key);

  return ret;
}

static int get_random_data(void *ctx, uint8_t *rand, size_t len)
{
  size_t i = 0;
  struct gecko_msg_system_get_random_data_rsp_t * rsp;
  while (len > 0) {
    if (len > 16) {
      rsp = gecko_cmd_system_get_random_data(16);
      memcpy(rand + i, rsp->data.data, rsp->data.len);
    } else {
      rsp = gecko_cmd_system_get_random_data(len);
      memcpy(rand + i, rsp->data.data, rsp->data.len);
    }
    i += rsp->data.len;
    len -= rsp->data.len;
  }
  return 0;
}

static int generate_ecc_keypair()
{
  int ret = 0;
  mbedtls_mpi_free(&private);
  mbedtls_mpi_init(&private);
  mbedtls_ecp_point_free(&public);
  mbedtls_ecp_point_init(&public);

  ret = mbedtls_ecdh_gen_public(&ecp_grp, &private, &public, get_random_data, NULL);
  return ret;
}

static int ecdh(const uint8_t *host_x, const uint8_t *host_y)
{
  int ret = 0;
  uint8_t sha265_input[PUBLIC_KEY_SIZE];
  uint8_t sha265_output[PUBLIC_KEY_SIZE];
  mbedtls_ecp_point public_key;
  mbedtls_ecp_point_init(&public_key);

  mbedtls_mpi_read_binary(&public_key.X, host_x, PUBLIC_KEY_SIZE);
  mbedtls_mpi_read_binary(&public_key.Y, host_y, PUBLIC_KEY_SIZE);
  mbedtls_mpi_lset(&public_key.Z, 1);

  mbedtls_mpi shared_secred;
  mbedtls_mpi_init(&shared_secred);    //shared secret
  mbedtls_mpi_lset(&shared_secred, 0);

  ret = mbedtls_ecdh_compute_shared(&ecp_grp, &shared_secred, &public_key, &private, get_random_data, NULL);
  mbedtls_ecp_point_free(&public_key);
  if (ret) {
    mbedtls_mpi_free(&shared_secred);
    return ret;
  }

  mbedtls_mpi_write_binary(&shared_secred, sha265_input, PUBLIC_KEY_SIZE);
  mbedtls_mpi_free(&shared_secred);

  ret = mbedtls_sha256_ret(sha265_input, PUBLIC_KEY_SIZE, sha265_output, 0);
  memcpy(ccm_key, sha265_output, KEY_SIZE);

  return ret;
}

errorcode_t ncp_sec_increase_security(uint8_t* public_keys, uint8_t *host_iv_to_target, uint8_t *host_iv_to_host)
{
  mbedtls_ecp_group_free(&ecp_grp);
  mbedtls_ecp_group_init(&ecp_grp);
  mbedtls_ecp_group_load(&ecp_grp, MBEDTLS_ECP_DP_SECP256R1);

  public_key_t *host_public = (public_key_t *)public_keys;
  if (verify_public_key(host_public->x, host_public->y)) {
    return bg_err_invalid_param;
  }

  ncp_sec_state = ncp_sec_unencrypted;

  ncp_sec_counter_in.counter = 0;
  ncp_sec_counter_in.counter_hi = 0;
  ncp_sec_counter_out.counter = 0;
  ncp_sec_counter_out.counter_hi = 0;

  memcpy(ncp_sec_counter_in.host_iv, host_iv_to_target, sizeof(ncp_sec_counter_in.host_iv));
  memcpy(ncp_sec_counter_out.host_iv, host_iv_to_host, sizeof(ncp_sec_counter_out.host_iv));

  get_random_data(NULL, ncp_sec_counter_in.target_iv, sizeof(ncp_sec_counter_in.target_iv));
  get_random_data(NULL, ncp_sec_counter_out.target_iv, sizeof(ncp_sec_counter_out.target_iv));
  if (generate_ecc_keypair()) {
    memset(host_iv_to_target, 0, sizeof(ncp_sec_counter_in.target_iv));
    memset(host_iv_to_host, 0, sizeof(ncp_sec_counter_out.target_iv));
    memset(host_public, 0, sizeof(public_key_t));
    return bg_err_crypto;
  }

  if (ecdh(host_public->x, host_public->y)) {
    memset(host_iv_to_target, 0, sizeof(ncp_sec_counter_in.target_iv));
    memset(host_iv_to_host, 0, sizeof(ncp_sec_counter_out.target_iv));
    memset(host_public, 0, sizeof(public_key_t));
    return bg_err_crypto;
  }

  // Copy target data over the host data for response
  memcpy(host_iv_to_target, ncp_sec_counter_in.target_iv, sizeof(ncp_sec_counter_in.target_iv));
  memcpy(host_iv_to_host, ncp_sec_counter_out.target_iv, sizeof(ncp_sec_counter_out.target_iv));
  mbedtls_mpi_write_binary(&public.X, host_public->x, PUBLIC_KEY_SIZE);
  mbedtls_mpi_write_binary(&public.Y, host_public->y, PUBLIC_KEY_SIZE);

  mbedtls_ccm_free(&ccm_ctx);
  mbedtls_ccm_init(&ccm_ctx);
  mbedtls_ccm_setkey(&ccm_ctx, MBEDTLS_CIPHER_ID_AES, ccm_key, KEY_SIZE * 8);

  ncp_sec_state = ncp_sec_encrypted;
  return bg_err_success;
}

bool ncp_sec_is_encrypted(volatile void* msg)
{
  uint32_t cmd_header = *((uint32_t *)msg);
  return (BGLIB_MSG_ENCRYPTED(cmd_header) !=  0);
}

bool ncp_sec_validate_security(volatile uint8_t* msg)
{
  uint32_t cmd_header = *((uint32_t *)msg);

  uint8_t sensitive = gecko_is_sensitive_message(BGLIB_MSG_ID(cmd_header));
  if (sensitive && !BGLIB_MSG_ENCRYPTED(cmd_header)) {
    return false;
  }

  return true;
}

void ncp_sec_update_payload_length(struct gecko_cmd_packet* msg, uint16_t length)
{
  uint8_t* header = (uint8_t*)&msg->header;
  header[1] = length & 0xff;
  header[0] &= ~0x07;
  header[0] |= ((length & 0x0700) >> 8);
}

static struct gecko_cmd_packet* encrypt_message(struct gecko_cmd_packet* msg)
{
  uint16_t len = BGLIB_MSG_LEN(msg->header);
  memcpy((void*)encrypted_msg, msg, sizeof(msg->header));

  struct gecko_cmd_packet* new_msg = (struct gecko_cmd_packet*)encrypted_msg;
  ncp_sec_update_payload_length(new_msg, len + 9);
  new_msg->header |= BGLIB_BIT_ENCRYPTED;

  uint8_t auth_data[7];
  memcpy(auth_data, encrypted_msg, 2);
  memcpy(auth_data + 2, &ncp_sec_counter_out.counter, 4);
  auth_data[6] = ncp_sec_counter_out.counter_hi;

  //encrypt all data except two first bytes of header and counter which are signed
  int ret = mbedtls_ccm_encrypt_and_tag(&ccm_ctx, len + 2,
                                        (uint8_t *)&ncp_sec_counter_out, 13,
                                        auth_data, 7,
                                        (uint8_t *)msg + 2, encrypted_msg + 2,
                                        encrypted_msg + len + 4, 4);
  if (ret) {
    return NULL;
  }

  // add counter to end
  memcpy(encrypted_msg + len + 8, auth_data + 2, 5);

  increase_counter(&ncp_sec_counter_out);
  return new_msg;
}

struct gecko_cmd_packet* ncp_sec_process_response(struct gecko_cmd_packet* msg, bool cmd_was_encrypted)
{
  if (ncp_sec_state == ncp_sec_unencrypted || !cmd_was_encrypted) {
    return msg;
  }

  return encrypt_message(msg);
}

struct gecko_cmd_packet* ncp_sec_process_event(struct gecko_cmd_packet* msg)
{
  if (ncp_sec_state == ncp_sec_unencrypted || BGLIB_MSG_ID(msg->header) == gecko_evt_system_error_id) {
    return msg;
  }

  return encrypt_message(msg);
}

struct gecko_cmd_packet* ncp_sec_decrypt_command(struct gecko_cmd_packet* msg)
{
  // if not encrypted, just return the same
  if (ncp_sec_state == ncp_sec_unencrypted || !BGLIB_MSG_ENCRYPTED(msg->header)) {
    return msg;
  }

  uint16_t len = BGLIB_MSG_LEN(msg->header);
  memcpy((void*)decrypted_msg, msg, sizeof(msg->header));

  //verify counter to prevent replay attacks
  conn_nonce_t nonce;
  memcpy(&nonce.counter, (uint8_t *)msg + len + 4 - 5, 4);
  memcpy(&nonce.counter_hi, (uint8_t *)msg + len + 4 - 1, 1);
  memcpy(nonce.host_iv, ncp_sec_counter_in.host_iv, sizeof(ncp_sec_counter_in.host_iv));
  memcpy(nonce.target_iv, ncp_sec_counter_in.target_iv, sizeof(ncp_sec_counter_in.target_iv));
  if ((nonce.counter_hi > ncp_sec_counter_in.counter_hi || (nonce.counter_hi == ncp_sec_counter_in.counter_hi && nonce.counter >= ncp_sec_counter_in.counter)) == 0) {
    return NULL;
  }

  uint8_t auth_data[7];
  memcpy(auth_data, decrypted_msg, 2);
  memcpy(auth_data + 2, &nonce.counter, 4);
  auth_data[6] = nonce.counter_hi;

  //decrypt all data except two first bytes of header and counter which are signed
  int ret = mbedtls_ccm_auth_decrypt(&ccm_ctx, len + 2 - 9,
                                     (uint8_t *)&nonce, 13,
                                     auth_data, 7,
                                     (uint8_t *)msg + 2, decrypted_msg + 2,
                                     (uint8_t *)msg + len + 4 - 9, 4);
  if (ret) {
    return NULL;
  }

  struct gecko_cmd_packet* new_msg = (struct gecko_cmd_packet*)decrypted_msg;
  ncp_sec_update_payload_length(new_msg, len - 9);
  new_msg->header &= ~BGLIB_BIT_ENCRYPTED;

  // Update counter
  if (ncp_sec_counter_in.counter != nonce.counter || ncp_sec_counter_in.counter_hi != nonce.counter_hi) {
    ncp_sec_counter_in.counter = nonce.counter;
    ncp_sec_counter_in.counter_hi = nonce.counter_hi;
  }
  increase_counter(&ncp_sec_counter_in);
  return new_msg;
}

enum ncp_sec_state ncp_sec_get_state()
{
  return ncp_sec_state;
}
