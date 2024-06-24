/*
 * File: secure-ezsp-stub.c
 * Description: Implements secure EZSP stubs.
 *
 * Author(s): Rehan Rasool, rehan.rasool@silabs.com
 *
 * Copyright 2016 by Silicon Labs. All rights reserved.
 */

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "app/util/ezsp/ezsp-protocol.h"

EzspStatus emSecureEzspInit(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
EzspStatus emSecureEzspDeInit(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
bool emSecureEzspIsOn(void)
{
  return false;
}
EzspStatus emSecureEzspEncode(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
EzspStatus emSecureEzspDecode(void)
{
  return EZSP_ERROR_INVALID_CALL;
}
void emSecureEzspSetState(uint8_t state)
{
}
bool emSecureEzspParametersArePending(void)
{
  return false;
}
