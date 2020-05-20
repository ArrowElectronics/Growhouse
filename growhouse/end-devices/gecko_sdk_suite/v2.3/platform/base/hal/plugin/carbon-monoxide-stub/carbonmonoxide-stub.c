//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"

int set_carbonmonoxide = 45000;


void halCarbonmonoxideStartRead(void)
{
  halCarbonmonoxideReadingCompleteCallback(set_carbonmonoxide, true);
}


