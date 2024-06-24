// File: form-and-join-node-callbacks.c
//
// Description: default implementations of three EmberZNet callbacks
// for use with the form-and-join library on the node.  If the
// application does not need to implement these callbacks for
// itself, it can use this file to supply them.

#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros

#include "stack/include/ember.h"
#include "form-and-join.h"

void emberNetworkFoundHandler(EmberZigbeeNetwork *networkFound)
{
  uint8_t lqi;
  int8_t rssi;
  emberGetLastHopLqi(&lqi);
  emberGetLastHopRssi(&rssi);
  emberFormAndJoinNetworkFoundHandler(networkFound, lqi, rssi);
}

void emberScanCompleteHandler(uint8_t channel, EmberStatus status)
{
  emberFormAndJoinScanCompleteHandler(channel, status);
}

void emberEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue)
{
  emberFormAndJoinEnergyScanResultHandler(channel, maxRssiValue);
}
