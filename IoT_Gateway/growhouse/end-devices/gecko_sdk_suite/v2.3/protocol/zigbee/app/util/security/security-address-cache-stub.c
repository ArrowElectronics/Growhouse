// *******************************************************************
//  trust-center-address-cache-stub.c
//
//  stub functions for the trust center address cache.
//
//  Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

//------------------------------------------------------------------------------
// INCLUDES

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "stack/include/ember.h"
#include "stack/include/error.h"

uint8_t addressCacheSize = 0;

//------------------------------------------------------------------------------

void securityAddressCacheInit(uint8_t securityAddressCacheStartIndex,
                              uint8_t securityAddressCacheSize)
{
}

void securityAddToAddressCache(EmberNodeId nodeId, EmberEUI64 nodeEui64)
{
}
