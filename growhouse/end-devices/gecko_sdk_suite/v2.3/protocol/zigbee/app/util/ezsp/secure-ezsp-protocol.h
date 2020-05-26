// File: secure-ezsp-protocol.h
//
// Description: Definitions useful for creating and decoding Secure EZSP frames.
//   <frame control:1>
//   <session ID:8>
//   <frame counter:4>
//   <nonce security level:1>
//   <frame id:1>
//   <parameters:0-120>
//   <mic:4>
//
// Copyright 2007-2017 by Ember Corporation. All rights reserved.           *80*

#ifndef SILABS_SECURE_EZSP_PROTOCOL_H
#define SILABS_SECURE_EZSP_PROTOCOL_H

#include "secure-ezsp-types.h"

//------------------------------------------------------------------------------
// Protocol Definitions

#define SECURE_EZSP_PROTOCOL_VERSION            0x01

// Secure EZSP autentication header + Frame ID + Secure EZSP MIC
#define SECURE_EZSP_MIN_FRAME_LENGTH            (14 + 1 + 4)
#define SECURE_EZSP_MAX_FRAME_LENGTH            200
#define SECURE_EZSP_FRAME_CONTROL_INDEX         0
#define SECURE_EZSP_SESSION_ID_INDEX            1
#define SECURE_EZSP_FRAME_COUNTER_INDEX         9
#define SECURE_EZSP_NONCE_SECURITY_LEVEL_INDEX  13
#define SECURE_EZSP_FRAME_ID_INDEX              14
#define SECURE_EZSP_PARAMETERS_INDEX            15

// Frame Control + Session ID + Frame Counter + Security Level
#define SECURE_EZSP_NONCE_SIZE                  14

// Nonce + EZSP header [ seq + FC + frame ID (0xFF) + extended FC ]
#define SECURE_EZSP_AUTHENTICATE_SIZE           (SECURE_EZSP_NONCE_SIZE + 4)

// Secure EZSP header - Frame ID byte as that is normally allocated
#define SECURE_EZSP_OVERHEAD_LENGTH             (SECURE_EZSP_MIN_FRAME_LENGTH - 1)

//------------------------------------------------------------------------------
// Constants and Variables

// Secure EZSP internal state machine
#define SECURE_EZSP_STATE_INITIAL                  0x00
#define SECURE_EZSP_STATE_PARAMETERS_PENDING       0x01
#define SECURE_EZSP_STATE_SYNCED                   0x02
#define SECURE_EZSP_STATE_INVALID                  0xFF

//------------------------------------------------------------------------------
// Internal functions and handlers.

EzspStatus emSecureEzspInit(void);
EzspStatus emSecureEzspDeInit(void);

bool emSecureEzspIsOn(void);
bool emSecureEzspParametersArePending(void);
bool emSecureEzspIsSecurityKeySet(void);

EzspStatus emSecureEzspEncode(void);
EzspStatus emSecureEzspDecode(void);
void emSecureEzspGenerateSessionIds(SecureEzspRandomNumber *hostRandomNumber,
                                    SecureEzspRandomNumber *ncpRandomNumber);
EzspStatus emSecureEzspReset(void);

// Setters
void emSecureEzspSetState(uint8_t state);
EzspStatus emSecureEzspSetSecurityKey(EmberKeyData *securityKey);
void emSecureEzspSetSecurityType(SecureEzspSecurityType securityType);
void emSecureEzspSetSecurityLevel(SecureEzspSecurityLevel securityLevel);

// Getters
uint8_t emSecureEzspGetState(void);
EmberKeyData* emSecureEzspGetSecurityKey(void);
SecureEzspSecurityType emSecureEzspGetSecurityType(void);

//------------------------------------------------------------------------------
// EZSP functions

// Set the Security Key of the Secure EZSP Protocol.
// Return: An EzspStatus value indicating success or the reason for failure.
EzspStatus ezspSetSecurityKey(
  // The key to use for the Secure EZSP Protocol.
  EmberKeyData *key,
  // The security type to be used for the Secure EZSP Protocol.
  SecureEzspSecurityType securityType);

// Set the Host-side Security Parameters of the Secure EZSP Protocol.
// Return: An EzspStatus value indicating success or the reason for failure.
EzspStatus ezspSetSecurityParameters(
  // The security level to be used for the Secure EZSP communication.
  SecureEzspSecurityLevel securityLevel,
  // The Host-side randomly generated number.
  SecureEzspRandomNumber *hostRandomNumber,
  // Return: The NCP-side randomly generated number.
  SecureEzspRandomNumber *returnNcpRandomNumber);

// Reset the NCP to factory default.
// Return: An EzspStatus value indicating success or the reason for failure.
EzspStatus ezspResetToFactoryDefaults(void);

// Get the security key status on the NCP: whether the security key is set or
// not and what the security type is.
// Return: An EzspStatus value indicating whether the security key is set or
// not.
EzspStatus ezspGetSecurityKeyStatus(
  // Return: The security type set at NCP for the Secure EZSP Protocol.
  SecureEzspSecurityType *returnSecurityType);

//------------------------------------------------------------------------------
// Debugging

#if defined(EMBER_TEST) || defined(SECURE_EZSP_SERIAL_PRINTF_DEBUG)
#define secureEzspSerialDebug fprintf
#else
#define secureEzspSerialDebug(...)
#endif // EMBER_TEST || SECURE_EZSP_SERIAL_PRINTF_DEBUG

#endif // SILABS_SECURE_EZSP_PROTOCOL_H
