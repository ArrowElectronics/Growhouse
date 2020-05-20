// *******************************************************************
// * level-control.h
// *
// *
// * Copyright 2014 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

// Rate of level control tick execution.
// To increase tick frequency (for more granular updates of device state based
// on level), redefine EMBER_AF_PLUGIN_LEVEL_CONTROL_TICKS_PER_SECOND.
#ifndef EMBER_AF_PLUGIN_LEVEL_CONTROL_TICKS_PER_SECOND
  #define EMBER_AF_PLUGIN_LEVEL_CONTROL_TICKS_PER_SECOND 32
#endif
#define EMBER_AF_PLUGIN_LEVEL_CONTROL_TICK_TIME \
  (MILLISECOND_TICKS_PER_SECOND / EMBER_AF_PLUGIN_LEVEL_CONTROL_TICKS_PER_SECOND)
