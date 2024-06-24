// File: form-and-join-host-callbacks.c
//
// Description: default implementations of three EZSP callbacks
// for use with the form-and-join library on the host.  If the
// application does not need to implement these callbacks for
// itself, it can use this file to supply them.

#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros

#include "stack/include/ember-types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "form-and-join.h"

void ezspNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                             uint8_t lqi,
                             int8_t rssi)
{
  emberFormAndJoinNetworkFoundHandler(networkFound, lqi, rssi);
}

void ezspScanCompleteHandler(uint8_t channel, EmberStatus status)
{
  emberFormAndJoinScanCompleteHandler(channel, status);
}

void ezspEnergyScanResultHandler(uint8_t channel, int8_t rssi)
{
  emberFormAndJoinEnergyScanResultHandler(channel, rssi);
}
