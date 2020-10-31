// *****************************************************************************
// * af-event.h
// *
// * Interface for the application event mechanism
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#ifndef SILABS_AF_EVENT_H
#define SILABS_AF_EVENT_H

#include "../include/af.h"
#include "stack/include/event.h"

#define MAX_TIMER_UNITS_HOST 0x7fff
#define MAX_TIMER_MILLISECONDS_HOST (MAX_TIMER_UNITS_HOST * MILLISECOND_TICKS_PER_MINUTE)

#if !defined(ATTRIBUTE_STORAGE_CONFIGURATION) && defined(EMBER_TEST)
  #include "af-event-test.h"
#else
  #include ATTRIBUTE_STORAGE_CONFIGURATION
#endif

// Task ids referenced in the sleep code
extern EmberTaskId emAfTaskId;

extern uint16_t emAfAppEventContextLength;
extern EmberAfEventContext emAfAppEventContext[];

extern PGM_P emAfEventStrings[];
extern EmberEventData emAfEvents[];

// A function used to retrieve the proper NCP timer duration and unit based on a given
// passed number of milliseconds.
void emAfGetTimerDurationAndUnitFromMS(uint32_t durationMs,
                                       uint16_t *duration,
                                       EmberEventUnits *units);

// A function (inverse of the above) to retrieve the number of milliseconds
// represented by a given timer duration and unit.
uint32_t emAfGetMSFromTimerDurationAndUnit(uint16_t duration,
                                           EmberEventUnits units);

//A function to initialize the Event mechanism used to drive the application framework.
void emAfInitEvents(void);

PGM_P emberAfGetEventString(uint8_t index);

#endif // SILABS_AF_EVENT_H
