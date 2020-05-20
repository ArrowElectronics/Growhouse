//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"

int set_pressure = 45000;


void halPressureStartRead(void)
{
  halPressureReadingCompleteCallback(set_pressure, true);
}


