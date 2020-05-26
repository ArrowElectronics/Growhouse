/***************************************************************************//**
 * @file NCP Security
 * @brief sl_security.h
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

 #ifndef SL_SECURITY_H
#define SL_SECURITY_H

/* Security state */
typedef enum {
  sl_security_state_unencrypted,
  sl_security_state_increase_security,
  sl_security_state_encrypted
}t_sl_security_state;

/***********************************************************************************************//**
 *  \brief  Initialize security module.
 *  \return  0 on success, -1 on failure.
 **************************************************************************************************/
int sl_security_init();

/***********************************************************************************************//**
 *  \brief  Function to reset security state.
 **************************************************************************************************/
void sl_security_reset();

/***********************************************************************************************//**
 *  \brief  Start security.
 **************************************************************************************************/
void sl_security_start();

//Ask for current security state
/***********************************************************************************************//**
 *  \brief  Function to Ask for current security state.
 *  \return  t_sl_security_state security state.
 **************************************************************************************************/
t_sl_security_state  sl_security_state();

//this function is called when security state changes

/***********************************************************************************************//**
 *  \brief  Callback which is called when security state changes.
 *  \param  state Security State.
 **************************************************************************************************/
void sl_security_state_change_cb(t_sl_security_state state);

/***********************************************************************************************//**
 *  \brief  Security handshake response.
 *  \param  public_key Public Key.
 *  \param host_iv_to_target Initialization Vector to Target.
 *  \param host_iv_to_target Initialization Vector to Host.
 **************************************************************************************************/
void sl_security_increase_security_rsp(uint8_t *public_key, uint8_t *target_iv_to_target, uint8_t *target_iv_to_host);

/***********************************************************************************************//**
 *  \brief  Function to decrypt a packet.
 *  \param  src pointer to ncoming encrypted packet.
 *  \param dst pointer to outgoing decrypted packet.
 *  \param len Length of data.
 **************************************************************************************************/
void sl_security_decrypt_packet(char *src, char *dst, unsigned *len);

/***********************************************************************************************//**
 *  \brief  Function to encrypt a packet.
 *  \param  src pointer to incoming unencrypted packet.
 *  \param dst pointer to outgoing encrypted packet.
 *  \param len Length of data.
 **************************************************************************************************/
void sl_security_encrypt_packet(char *src, char *dst, unsigned *len);

#endif
