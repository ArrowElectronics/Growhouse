/*
 * File: command-handlers-secure-ezsp.c
 * Description: Secure EZSP NCP support code.
 *
 * Author(s): Rehan Rasool, rehan.rasool@silabs.com
 *
 * Copyright 2017 Silicon Laboratories, Inc.                                *80*
 */

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "app/util/ezsp/ezsp-protocol.h"

bool emRadioGetRandomNumbers(uint16_t *rn, uint8_t count);

EzspStatus ezspSetSecurityKey(EmberKeyData *key,
                              SecureEzspSecurityType securityType)
{
  EzspStatus status;

  if (securityType == SECURE_EZSP_SECURITY_TYPE_PERMANENT) {
    return EZSP_ERROR_INVALID_CALL;
  } else if (securityType == SECURE_EZSP_SECURITY_TYPE_TEMPORARY) {
    status = emSecureEzspSetSecurityKey(key);

    if (status == EMBER_SUCCESS) {
      emSecureEzspSetSecurityType(securityType);
    }

    return status;
  } else {
    return EZSP_ERROR_SECURITY_TYPE_INVALID;
  }
}

EzspStatus ezspSetSecurityParameters(SecureEzspSecurityLevel securityLevel,
                                     SecureEzspRandomNumber *hostRandomNumber,
                                     SecureEzspRandomNumber *returnNcpRandomNumber)
{
  if (securityLevel != SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32) {
    return EZSP_ERROR_SECURITY_PARAMETERS_INVALID;
  }

  if (!emSecureEzspIsSecurityKeySet()) {
    return EZSP_ERROR_SECURITY_KEY_NOT_SET;
  }

  if (!emSecureEzspParametersArePending()) {
    return EZSP_ERROR_SECURITY_PARAMETERS_ALREADY_SET;
  }

  // emRadioGetRandomNumbers() returns an array of 16-bit numbers, so
  // we must cast the pointer for our return value and reduce
  // the number of bytes we request by a factor of 2.
  if (!emRadioGetRandomNumbers((uint16_t*)(SecureEzspRandomNumberContents(returnNcpRandomNumber)),
                               SECURE_EZSP_RANDOM_NUMBER_SIZE / 2)) {
    return EZSP_ASH_NCP_FATAL_ERROR;
  }

  emSecureEzspSetSecurityLevel(securityLevel);
  emSecureEzspGenerateSessionIds(hostRandomNumber, returnNcpRandomNumber);

  return EZSP_SUCCESS;
}

EzspStatus ezspResetToFactoryDefaults(void)
{
  if (emSecureEzspGetSecurityType() == SECURE_EZSP_SECURITY_TYPE_PERMANENT) {
    return EZSP_ERROR_INVALID_CALL;
  }

  if (emSecureEzspGetState() == SECURE_EZSP_STATE_INITIAL) {
    return EZSP_ERROR_SECURITY_KEY_NOT_SET;
  }

  return emSecureEzspReset();
}

EzspStatus ezspGetSecurityKeyStatus(SecureEzspSecurityType *returnSecurityType)
{
  if (emSecureEzspIsSecurityKeySet()) {
    *returnSecurityType = emSecureEzspGetSecurityType();
    return EZSP_ERROR_SECURITY_KEY_ALREADY_SET;
  }
  return EZSP_ERROR_SECURITY_KEY_NOT_SET;
}
