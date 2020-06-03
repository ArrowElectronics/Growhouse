// *****************************************************************************
// * security-support.c
// *
// * Provides support features for security, such as random number generation.
// *
// * Copyright 2017 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#include "app/framework/include/af.h"
#include "tool/random/random-number-generation.h"

EmberStatus emberAfGenerateRandomKey(EmberKeyData* result)
{
  if (platformRandomDataFunction(emberKeyContents(result),
                                 EMBER_ENCRYPTION_KEY_SIZE)) {
    return EMBER_INSUFFICIENT_RANDOM_DATA;
  }
  return EMBER_SUCCESS;
}

EmberStatus emberAfGenerateRandomData(uint8_t* result, uint8_t size)
{
  if (platformRandomDataFunction(result, size)) {
    return EMBER_INSUFFICIENT_RANDOM_DATA;
  }
  return EMBER_SUCCESS;
}
