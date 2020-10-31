// *******************************************************************
// * ami-demand-response-load-control.h
// *
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "load-control-event-table.h"

// needed for EmberSignatureData
#include "stack/include/ember-types.h"

#define EVENT_OPT_IN_DEFAULT  0x01

void afReportEventStatusHandler(uint32_t eventId,
                                uint8_t eventCode,
                                uint32_t startTime,
                                uint8_t criticalityLevelApplied,
                                int16_t coolingTempSetPointApplied,
                                int16_t heatingTempSetPointApplied,
                                int8_t avgLoadAdjPercent,
                                uint8_t dutyCycle,
                                uint8_t eventControl,
                                uint8_t messageLength,
                                uint8_t* message,
                                EmberSignatureData* signature);
