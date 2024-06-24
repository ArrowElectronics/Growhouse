// *******************************************************************
// * poll-control-client.c
// *
// *
// * Copyright 2012 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#include "../../include/af.h"
#include "poll-control-client.h"

static bool fastPolling = false;
static bool respondToCheckIn = true;
static uint16_t fastPollingTimeout = EMBER_AF_PLUGIN_POLL_CONTROL_CLIENT_DEFAULT_FAST_POLL_TIMEOUT;

void emAfSetFastPollingMode(bool mode)
{
  fastPolling = mode;
}

void emAfSetFastPollingTimeout(uint16_t timeout)
{
  fastPollingTimeout = timeout;
}

void emAfSetResponseMode(bool mode)
{
  respondToCheckIn = mode;
}

bool emberAfPollControlClusterCheckInCallback(void)
{
  emberAfPollControlClusterPrintln("RX: CheckIn");
  if (respondToCheckIn) {
    emberAfFillCommandPollControlClusterCheckInResponse(fastPolling,
                                                        fastPollingTimeout);
    emberAfSendResponse();
  }
  return true;
}

void emAfPollControlClientPrint(void)
{
  emberAfPollControlClusterPrintln("Poll Control Client:\n%p %p\n%p 0x%2x",
                                   "fast polling: ",
                                   fastPolling ? "on" : "off",
                                   "fast polling timeout: ",
                                   fastPollingTimeout);
}
