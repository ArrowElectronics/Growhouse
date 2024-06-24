/*
 * File: command-handlers-secure-ezsp-stub.c
 * Description: Implements secure EZSP command handler stubs.
 *
 * Author(s): Rehan Rasool, rehan.rasool@silabs.com
 *
 * Copyright 2017 by Silicon Labs. All rights reserved.
 */

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "app/util/ezsp/ezsp-protocol.h"

EzspStatus ezspSetSecurityKey(EmberKeyData *key,
                              SecureEzspSecurityType securityType)
{
  return EZSP_ERROR_INVALID_CALL;
}

EzspStatus ezspSetSecurityParameters(SecureEzspSecurityLevel securityLevel,
                                     SecureEzspRandomNumber *hostRandomNumber,
                                     SecureEzspRandomNumber *returnNcpRandomNumber)
{
  return EZSP_ERROR_INVALID_CALL;
}

EzspStatus ezspResetToFactoryDefaults(void)
{
  return EZSP_ERROR_INVALID_CALL;
}

EzspStatus ezspGetSecurityKeyStatus(SecureEzspSecurityType *returnSecurityType)
{
  return EZSP_ERROR_INVALID_CALL;
}
