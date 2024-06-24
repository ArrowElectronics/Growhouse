// Copyright 2017 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/util/af-event.h"
#include "em4.h"

void emberPluginEM4AboutToGoInEM4Callback(uint32_t sleepDurationMs)
{
  // This is called from the idle-sleep-soc plugin.
  // Needs to be implemented by app.
  emberPluginEM4BeforeEM4Callback();

  // The following should be called after application code
  RTCCRamData input;
  input.outgoingNwkFrameCounter = emberGetSecurityFrameCounter();
  input.incomingParentNwkFrameCounter = emberGetParentIncomingNwkFrameCounter();
  input.outgoingLinkKeyFrameCounter = emberGetApsFrameCounter();
  input.incomingLinkKeyFrameCounter = emberGetIncomingTcLinkKeyFrameCounter();

  // Set the timer on the hardware, and save the in/out NWK counter.
  halBeforeEM4(sleepDurationMs, input);
}

void emberPluginEM4ComingBackFromEM4Callback(void)
{
  // This is called before application code.
  RTCCRamData output = halAfterEM4();

  emberSetOutgoingNwkFrameCounter(output.outgoingNwkFrameCounter);
  EmberStatus status = emberSetParentIncomingNwkFrameCounter(output.incomingParentNwkFrameCounter);
  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("WARNING: Undefined behavior. Entered EM4 sleep mode without or before saving the parent info to the flash.");
  }

  emberSetOutgoingApsFrameCounter(output.outgoingLinkKeyFrameCounter);
  emberSetIncomingTcLinkKeyFrameCounter(output.incomingLinkKeyFrameCounter);

  // This is should restore the outgoing network counter
  // Needs to be implemented by the app
  emberPluginEM4AfterEM4Callback();
}
