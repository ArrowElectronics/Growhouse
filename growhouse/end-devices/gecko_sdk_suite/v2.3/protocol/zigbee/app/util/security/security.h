// *******************************************************************
//  security.h
//
//  functions for manipulating security for Trust Center and
//  non Trust Center nodes.
//
//  Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef SILABS_APP_UTIL_SECURITY_SECURITY_H
#define SILABS_APP_UTIL_SECURITY_SECURITY_H

#if !defined emberKeyContents
  #define emberKeyContents(key) (((EmberKeyData*)(key))->contents)
#endif

// Trust Center Functions
bool trustCenterInit(EmberKeyData* preconfiguredKey,
                     EmberKeyData* networkKey);
void trustCenterPermitJoins(bool allow);
bool trustCenterIsPermittingJoins(void);

// Non Trust Center functions
bool nodeSecurityInit(EmberKeyData* preconfiguredKey);

// Common functions
extern uint8_t addressCacheSize;
#define securityAddressCacheGetSize() (addressCacheSize + 0)
void securityAddressCacheInit(uint8_t securityAddressCacheStartIndex,
                              uint8_t securityAddressCacheSize);
void securityAddToAddressCache(EmberNodeId nodeId, EmberEUI64 nodeEui64);

#endif // SILABS_APP_UTIL_SECURITY_SECURITY_H
