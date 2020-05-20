// *******************************************************************
// * simple-main.c
// *
// * This file provides a definition for main() for non-RTOS applications.
// *
// * Copyright 2014 Silicon Laboratories, Inc.                                 *80*
// *******************************************************************

#include PLATFORM_HEADER
#include "app/framework/include/af.h"

// Our entry point is typically main(), except during testing.
#ifdef EMBER_TEST
  #define MAIN nodeMain
#else
  #define MAIN main
#endif

int MAIN(MAIN_FUNCTION_PARAMETERS)
{
  halInit();
  emberAfMainInit();
  return emberAfMain(MAIN_FUNCTION_ARGUMENTS);
}
