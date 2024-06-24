// File: source-route-host.c
//
// Description: Example code for managing source routes on a host-based gateway.
// For node-based gateways refer to source-route.c.
//
// New source routes are added to the table in the
// ezspIncomingRouteRecordHandler() callback. Adding a route results in routes
// to all intermediate relays as well.
//
// Before sending a unicast message, the application must obtain the source
// route (using emberFindSourceRoute() provided in this file) and then call
// ezspSetSourceRoute().
//
// In this implementation, the maximum table size is 255 entries since a
// one-byte index is used and the index 0xFF is reserved.
//
// Copyright 2007 by Ember Corporation. All rights reserved.                *80*

#include PLATFORM_HEADER

#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "app/util/ezsp/ezsp-utils.h"
#include "app/util/ezsp/ezsp-host-configuration-defaults.h"
#include "source-route-common.h"

#ifdef EMBER_AF_PLUGIN_CONCENTRATOR_HOST_SOURCE_ROUTE_TABLE_SUUPORT
static SourceRouteTableEntry table[EZSP_HOST_SOURCE_ROUTE_TABLE_SIZE];
uint8_t sourceRouteTableSize = EZSP_HOST_SOURCE_ROUTE_TABLE_SIZE;
SourceRouteTableEntry *sourceRouteTable = table;
#else
uint8_t sourceRouteTableSize = 0;
SourceRouteTableEntry *sourceRouteTable = NULL;
#endif
// update the source route table on the host side.
void ezspChangeSourceRouteHandler(EmberNodeId newChildId,
                                  EmberNodeId newParentId,
                                  bool ourChild)
{
  if (sourceRouteTableSize == 0) {
    return;
  }
  if (ourChild) {
    sourceRouteAddEntry(newChildId, NULL_INDEX);
  } else {
    sourceRouteAddEntryWithCloserNextHop(newChildId, newParentId);
  }
}
void ezspIncomingRouteRecordHandler(EmberNodeId source,
                                    EmberEUI64 sourceEui,
                                    uint8_t lastHopLqi,
                                    int8_t lastHopRssi,
                                    uint8_t relayCount,
                                    uint8_t *relayList)
{
  uint8_t previous;
  uint8_t i;

  if (sourceRouteTableSize == 0) {
    return;
  }
  // The source of the route record is furthest from the gateway. We start there
  // and work closer.
  previous = sourceRouteAddEntry(source, NULL_INDEX);

  // Go through the relay list and add them to the source route table.
  for (i = 0; i < relayCount; i++) {
    EmberNodeId id = emberFetchLowHighInt16u(relayList + i * 2);
    // We pass the index of the previous entry to link the route together.
    previous = sourceRouteAddEntry(id, previous);
  }
}

// Note: We assume that the given relayList location is big enough to handle the
// longest source route.
bool emberFindSourceRoute(EmberNodeId destination,
                          uint8_t *relayCount,
                          uint16_t *relayList)
{
  uint8_t index = sourceRouteFindIndex(destination);

  if (index == NULL_INDEX) {
    return false;
  }

  // Fill in the relay list. The first relay in the list is the closest to the
  // destination (furthest from the gateway).
  *relayCount = 0;
  while (sourceRouteTable[index].closerIndex != NULL_INDEX) {
    index = sourceRouteTable[index].closerIndex;
    relayList[*relayCount] = sourceRouteTable[index].destination;
    *relayCount += 1;
  }
  return true;
}
