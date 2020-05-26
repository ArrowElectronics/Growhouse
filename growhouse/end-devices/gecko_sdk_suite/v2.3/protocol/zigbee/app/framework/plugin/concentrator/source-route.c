// File: source-route.c
//
// Description: Example code for managing source routes on a node-based gateway.
// For host-based gateways refer to source-route-host.c.
//
// New source routes are added to the table in the
// emberIncomingRouteRecordHandler() callback. Adding a route results in routes
// to all intermediate relays as well.
//
// For every outgoing packet, the stack calls emberAppendSourceRouteHandler().
// If a source route to the destination is found, it is added to the packet.
//
// In this implementation, the maximum table size is 255 entries since a
// one-byte index is used and the index 0xFF is reserved.
//
// Copyright 2007 by Ember Corporation. All rights reserved.                *80*

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "app/util/security/security.h"
#include "source-route-common.h"
// ZigBee protocol limitations effectively constrain us to a 11-hop source route
// in the worst case scenario (for a 12-hop delivery in total), so we enforce
// a threshold value on the relayCount and ignore source route operations beyond
// this threshold.  See explanatory comment in emberAppendSourceRouteHandler()
// for more details about this.
// This value could be reduced at the designer's discretion, but not increased
// beyond 11 without causing routing problems.
#define MAX_RELAY_COUNT    11
/*
 #ifndef EXTERNAL_TABLE
   static SourceRouteTableEntry table[EMBER_SOURCE_ROUTE_TABLE_SIZE];
   uint8_t sourceRouteTableSize = EMBER_SOURCE_ROUTE_TABLE_SIZE;
   SourceRouteTableEntry *sourceRouteTable = table;
 #endif
 */

//emzigbee-241-*
void emberChangeSourceRouteHandler(EmberNodeId newChildId,
                                   EmberNodeId newParentId,
                                   bool ourChild)
{
  if (!ourChild) {
    sourceRouteAddEntryWithCloserNextHop(newChildId, newParentId);
  } else {
    sourceRouteAddEntry(newChildId, NULL_INDEX);
  }

#ifdef EXTERNAL_TABLE//EMBER_AF_PLUGIN_CONCENTRATOR_HOST_SOURCE_ROUTE_TABLE
  emberHostChangeSourceRoute(newChildId,
                             newParentId,
                             ourChild);
#endif
}

void emberIncomingRouteRecordHandler(EmberNodeId source,
                                     EmberEUI64 sourceEui,
                                     uint8_t relayCount,
                                     EmberMessageBuffer header,
                                     uint8_t relayListIndex)
{
  uint8_t previous;
  uint8_t i;

  // If the following message has APS Encryption, our node will need to know
  // the IEEE Address of the source in order to properly decrypt.
  securityAddToAddressCache(source, sourceEui);

  if (sourceRouteTableSize == 0) {
#ifdef EXTERNAL_TABLE//EMBER_AF_PLUGIN_CONCENTRATOR_HOST_SOURCE_ROUTE_TABLE
    emberHostIncomingRouteRecord(source,
                                 sourceEui,
                                 relayCount,
                                 header,
                                 relayListIndex);
#endif

    return;
  }

  // Ignore over-sized source routes, since we can't reuse them reliably anyway.
  if (relayCount > MAX_RELAY_COUNT) {
#ifdef EXTERNAL_TABLE//EMBER_AF_PLUGIN_CONCENTRATOR_HOST_SOURCE_ROUTE_TABLE
    emberHostIncomingRouteRecord(source,
                                 sourceEui,
                                 relayCount,
                                 header,
                                 relayListIndex);
#endif

    return;
  }

  // The source of the route record is furthest from the gateway. We start there
  // and work closer.
  previous = sourceRouteAddEntry(source, NULL_INDEX);

  // Go through the relay list and add them to the source route table.
  for (i = 0; i < relayCount; i++) {
    uint8_t index = relayListIndex + ((uint8_t) (i << 1));
    EmberNodeId id = emberGetLinkedBuffersLowHighInt16u(header, index);
    // We pass the index of the previous entry to link the route together.
    previous = sourceRouteAddEntry(id, previous);
  }

#ifdef EXTERNAL_TABLE//EMBER_AF_PLUGIN_CONCENTRATOR_HOST_SOURCE_ROUTE_TABLE
  emberHostIncomingRouteRecord(source,
                               sourceEui,
                               relayCount,
                               header,
                               relayListIndex);
#endif
}

uint8_t emberAppendSourceRouteHandler(EmberNodeId destination,
                                      EmberMessageBuffer header)
{
#ifdef EXTERNAL_TABLE
  uint8_t notCached = emberAppendCachedSourceRouteFromHost(destination,
                                                           header);
  if (notCached != 1) {
    return notCached;
  }
#endif

  uint8_t foundIndex = sourceRouteFindIndex(destination);
  uint8_t relayCount = 0;
  uint8_t addedBytes;
  uint8_t bufferLength = emberMessageBufferLength(header);
  uint8_t i;

  if (foundIndex == NULL_INDEX) {
    return 0;
  }

  // Find out what the relay count is.
  i = foundIndex;
  while (sourceRouteTable[i].closerIndex != NULL_INDEX) {
    i = sourceRouteTable[i].closerIndex;
    relayCount++;
  }

  // ZigBee specification, excerpts from R22 section 3.6.3.3.1:
  // If there are no intermediate relay nodes, the frame shall be transmitted directly
  // to the routing destination without source routing by using the MCPS-DATA.request
  //primitive, with the DstAddr parameter value indicating the routing destination.
  if (!relayCount) {
    return 0;
  }

  // Per Ember Case 10096, we need to protect against oversized source
  // routes that will overflow the PHY packet length (127).  Since the
  // worst-case packet overhead is the APS-encrypted, NWK-encrypted,
  // tunneled key delivery sent by the Trust Center during
  // authentication of new devices, and this leaves enough only room
  // for an 11-hop source route frame, we abort our route-appending
  // operation if the resulting source route contains more than 11
  // relays.  Unless a route already exists to the destination, this
  // will likely result in the stack not sending the packet at all,
  // but this is better than forcing too much data into the packet
  // (which, as of this writing, will trigger an assert in mac.c and
  // cause a reset).

  if (MAX_RELAY_COUNT < relayCount) {
    return 0;
  }

  addedBytes = 2 + ((uint8_t) (relayCount << 1));

  if (header != EMBER_NULL_MESSAGE_BUFFER) {
    // Two bytes for the relay count and relay index.
    uint8_t bufferIndex = bufferLength;

    if (emberSetLinkedBuffersLength(header, bufferLength + addedBytes)
        != EMBER_SUCCESS) {
      return 0;
    }

    // Set the relay count and relay index.
    emberSetLinkedBuffersByte(header, bufferIndex++, relayCount);
    emberSetLinkedBuffersByte(header,
                              bufferIndex++,
                              (0 < relayCount
                               ? relayCount - 1
                               : 0));

    // Fill in the relay list. The first relay in the list is the closest to the
    // destination (furthest from the gateway).
    i = foundIndex;
    while (sourceRouteTable[i].closerIndex != NULL_INDEX) {
      i = sourceRouteTable[i].closerIndex;
      emberSetLinkedBuffersLowHighInt16u(header,
                                         bufferIndex,
                                         sourceRouteTable[i].destination);
      bufferIndex += 2;
    }
  }
  return addedBytes;
}

EmberStatus emberGetSourceRouteTableEntry(uint8_t index,
                                          EmberNodeId *destination,
                                          uint8_t *closerIndex)
{
  if (index != NULL_INDEX) {
    *destination = sourceRouteTable[index].destination;
    *closerIndex = sourceRouteTable[index].closerIndex;
    return EMBER_SUCCESS;
  }
  return EMBER_NOT_FOUND;
}
/* this was never called from host, it is always only called from soc and ncp*/
uint8_t emberGetSourceRouteOverhead(EmberNodeId destination)
{
  uint8_t overhead = 0;
  uint8_t index = sourceRouteFindIndex(destination);
  if (index != NULL_INDEX) {
    // The additional overhead required for network source routing (relay
    // count = 1, relay index = 1).  This does not include the size of the
    // relay list itself.
    overhead = 2;
    while (sourceRouteTable[index].closerIndex != NULL_INDEX) {
      // The additional overhead required per relay address for network
      // source routing.  This is in addition to the overhead included above.
      overhead += 2;
      index = sourceRouteTable[index].closerIndex;
    }
  }
  return overhead;
}
