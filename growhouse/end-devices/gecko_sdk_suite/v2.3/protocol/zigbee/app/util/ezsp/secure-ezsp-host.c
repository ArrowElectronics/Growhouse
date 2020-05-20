/*
 * File: secure-ezsp-host.c
 * Description: Implements Host-side secure EZSP functionality.
 *
 * Author(s): Rehan Rasool, rehan.rasool@silabs.com
 *
 * Copyright 2016 by Silicon Labs. All rights reserved.
 */

#include PLATFORM_HEADER

#include "stack/include/ember-types.h"
#include "hal/micro/token.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/secure-ezsp-host.h"

//------------------------------------------------------------------------------
// Public APIs

EzspStatus emberSecureEzspSetSecurityKey(EmberKeyData *securityKey,
                                         SecureEzspSecurityType securityType)
{
  EzspStatus status;

  status = ezspSetSecurityKey(securityKey, securityType);

  if (status == EZSP_SUCCESS) {
    emSecureEzspSetSecurityKey(securityKey);
    emSecureEzspSetSecurityType(securityType);
    emSecureEzspSetState(SECURE_EZSP_STATE_PARAMETERS_PENDING);

    // Save new key to host token
    tokTypeSecureEzspSecurityKey tok;
    MEMMOVE(tok.contents,
            emberKeyContents(securityKey),
            EMBER_ENCRYPTION_KEY_SIZE);
    halCommonSetToken(TOKEN_SECURE_EZSP_SECURITY_KEY, &tok);
  }

  return status;
}

EzspStatus emberSecureEzspSetSecurityParameters(SecureEzspSecurityLevel securityLevel,
                                                SecureEzspRandomNumber *hostRandomNumber)
{
  EzspStatus status;
  SecureEzspRandomNumber ncpRandomNumber;

  status = ezspSetSecurityParameters(securityLevel, hostRandomNumber, &ncpRandomNumber);

  if (status == EZSP_SUCCESS) {
    emSecureEzspSetSecurityLevel(securityLevel);
    emSecureEzspGenerateSessionIds(hostRandomNumber, &ncpRandomNumber);
    emSecureEzspSetState(SECURE_EZSP_STATE_SYNCED);
  }

  return status;
}

EzspStatus emberSecureEzspResetToFactoryDefaults(void)
{
  EzspStatus status;

  status = ezspResetToFactoryDefaults();

  if (status == EZSP_SUCCESS) {
    emSecureEzspReset();
  }

  return status;
}

//------------------------------------------------------------------------------
// Internal functions and handlers.

EzspStatus emSecureEzspInit(void)
{
  SecureEzspSecurityType ncpSecurityType;
  EzspStatus hostSecurityKeyStatus;
  EzspStatus ncpSecurityKeyStatus = ezspGetSecurityKeyStatus(&ncpSecurityType);

  if (emSecureEzspIsSecurityKeySet()) {
    hostSecurityKeyStatus = EZSP_ERROR_SECURITY_KEY_ALREADY_SET;

    tokTypeSecureEzspSecurityKey tok;
    halCommonGetToken(&tok, TOKEN_SECURE_EZSP_SECURITY_KEY);
    MEMMOVE(emberKeyContents(emSecureEzspGetSecurityKey()),
            tok.contents,
            EMBER_ENCRYPTION_KEY_SIZE);
  } else {
    hostSecurityKeyStatus = EZSP_ERROR_SECURITY_KEY_NOT_SET;
  }

  secureEzspSerialDebug(stderr, "Secure EZSP Init; NCP: 0x%02X, Host: 0x%02X\n",
                        ncpSecurityKeyStatus,
                        hostSecurityKeyStatus);

  if (ncpSecurityKeyStatus == EZSP_ERROR_SECURITY_KEY_ALREADY_SET) {
    emSecureEzspSetState(SECURE_EZSP_STATE_PARAMETERS_PENDING);
  }

  emberSecureEzspInitCallback(ncpSecurityKeyStatus,
                              hostSecurityKeyStatus,
                              ncpSecurityType);
  return EZSP_SUCCESS;
}

EzspStatus emSecureEzspSetSecurityKey(EmberKeyData *securityKey)
{
  MEMMOVE(emberKeyContents(emSecureEzspGetSecurityKey()),
          emberKeyContents(securityKey),
          EMBER_ENCRYPTION_KEY_SIZE);
  return EZSP_SUCCESS;
}

EzspStatus emSecureEzspReset(void)
{
  // Reset token
  tokTypeSecureEzspSecurityKey tok;
  MEMSET(&tok, 0x00, sizeof(tokTypeSecureEzspSecurityKey));
  halCommonSetToken(TOKEN_SECURE_EZSP_SECURITY_KEY, &tok);

  emSecureEzspDeInit();
  return EZSP_SUCCESS;
}
