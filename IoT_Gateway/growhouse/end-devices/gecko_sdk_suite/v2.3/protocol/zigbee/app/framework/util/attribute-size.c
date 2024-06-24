// *******************************************************************
// * attribute-size.c
// *
// * Contains storage and function for retrieving attribute size.
// *
// * Copyright 2009 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include PLATFORM_HEADER

#include "attribute-type.h"

static PGM uint8_t attributeSizes[] =
{
#include "attribute-size.h"
};

uint8_t emberAfGetDataSize(uint8_t dataType)
{
  uint8_t i;
  for (i = 0; (i + 1) < sizeof(attributeSizes); i += 2) {
    if (attributeSizes[i] == dataType) {
      return attributeSizes[i + 1];
    }
  }

  return 0;
}
