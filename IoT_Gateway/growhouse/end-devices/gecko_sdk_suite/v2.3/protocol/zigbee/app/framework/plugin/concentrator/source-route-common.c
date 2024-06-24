// File: source-route-common.c
//
// Description: Common code used for managing source routes on both node-based
// and host-based gateways. See source-route.c for node-based gateways and
// source-route-host.c for host-based gateways.
//
// Copyright 2007 by Ember Corporation. All rights reserved.                *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "source-route-common.h"

// The number of entries in use.
static uint8_t entryCount = 0;

// The index of the most recently added entry.
static uint8_t newestIndex = NULL_INDEX;

// Return the index of the entry with the specified destination.
uint8_t sourceRouteFindIndex(EmberNodeId id)
{
  if (sourceRouteTableSize == 0) {
    return NULL_INDEX;
  }
  uint8_t i;
  for (i = 0; i < entryCount; i++) {
    if (sourceRouteTable[i].destination == id) {
      return i;
    }
  }
  return NULL_INDEX;
}

void sourceRouteClearTable(void)
{
  entryCount = 0;
  newestIndex = NULL_INDEX;
  MEMSET(sourceRouteTable,
         0,
         sizeof(SourceRouteTableEntry) * sourceRouteTableSize);
}

// Create an entry with the given id or update an existing entry. furtherIndex
// is the entry one hop further from the gateway.

// CAUTION: make sure calling this function updates all the related entries,
// and not just one specific entry. Example correct use case could be seen in:
// emberIncomingRouteRecordHandler(....)
uint8_t sourceRouteAddEntry(EmberNodeId id, uint8_t furtherIndex)
{
  if (sourceRouteTableSize == 0) {
    return NULL_INDEX;
  }
  // See if the id already exists in the table.
  uint8_t index = sourceRouteFindIndex(id);
  uint8_t i;

  if (index == NULL_INDEX) {
    if (entryCount < sourceRouteTableSize) {
      // No existing entry. Table is not full. Add new entry.
      index = entryCount;
      entryCount += 1;
    } else {
      // No existing entry. Table is full. Replace oldest entry.
      index = newestIndex;
      while (sourceRouteTable[index].olderIndex != NULL_INDEX) {
        index = sourceRouteTable[index].olderIndex;
      }
    }
  }

  // Update the pointers (only) if something has changed.
  if (index != newestIndex) {
    for (i = 0; i < entryCount; i++) {
      if (sourceRouteTable[i].olderIndex == index) {
        sourceRouteTable[i].olderIndex = sourceRouteTable[index].olderIndex;
        break;
      }
    }
    sourceRouteTable[index].olderIndex = newestIndex;
    newestIndex = index;
  }
  // Add the entry.
  sourceRouteTable[index].destination = id;
  sourceRouteTable[index].closerIndex = NULL_INDEX;
  // The current index is one hop closer to the gateway than furtherIndex.
  if (furtherIndex != NULL_INDEX) {
    sourceRouteTable[furtherIndex].closerIndex = index;
  }

  // Return the current index to save the caller having to look it up.
  return index;
}

uint8_t sourceRouteAddEntryWithCloserNextHop(EmberNodeId newId,
                                             EmberNodeId closerNodeId)
{
  if (sourceRouteTableSize == 0) {
    return NULL_INDEX;
  }
  uint8_t closerIndex = sourceRouteFindIndex(closerNodeId);
  if (closerIndex != NULL_INDEX) {
    uint8_t index = sourceRouteAddEntry(newId, NULL_INDEX);
    if (index != NULL_INDEX) {
      // emzigbee-408
      // we refresh all the ancestors of the newId, instead of simply
      // putting the index of the newId's parent as its closerIndex
      uint8_t curFurthurIndex = index;
      uint8_t curCloserIndex = closerIndex;
      uint8_t nextFurthurIndex, nextCloserIndex, ancestorIndex;
      while (curCloserIndex != NULL_INDEX) {
        nextFurthurIndex = curCloserIndex;
        nextCloserIndex = sourceRouteTable[nextFurthurIndex].closerIndex;
        // next call refreshes the closerNodeId entry and the rest of the ancestors
        ancestorIndex = sourceRouteAddEntry(sourceRouteTable[curCloserIndex].destination, curFurthurIndex);
        if (ancestorIndex == index) { // our new entry has been replaced by rewritting one of its ancestors
          index = NULL_INDEX; // In this case we refreshed the entries of the new nodes ancestor
          // without receiving a new route record message and even without inserting the node that
          // triggered this whole refreshment process.
        }
        curFurthurIndex = nextFurthurIndex;
        curCloserIndex = nextCloserIndex;
      }
    }
    return index;
  }
  return NULL_INDEX;
}

void sourceRouteInit(void)
{
  entryCount = 0;
}

uint8_t sourceRouteGetCount(void)
{
  return entryCount;
}

uint8_t emberGetSourceRouteTableFilledSize(void)
{
  return entryCount;
}

uint8_t emberGetSourceRouteTableTotalSize(void)
{
  return sourceRouteTableSize;
}
