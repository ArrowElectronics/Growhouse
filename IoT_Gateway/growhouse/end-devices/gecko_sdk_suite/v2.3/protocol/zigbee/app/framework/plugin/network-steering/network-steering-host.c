// Copyright 2015 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/plugin/network-steering/network-steering.h"
#include "app/framework/plugin/network-steering/network-steering-internal.h"

//============================================================================
// Globals

#define MAX_NETWORKS 16

static uint16_t storedNetworks[MAX_NETWORKS];
static bool memoryCleared = false;

//============================================================================
// Forward Declarations

//============================================================================

uint8_t emAfPluginNetworkSteeringGetMaxPossiblePanIds(void)
{
  return MAX_NETWORKS;
}

void emAfPluginNetworkSteeringClearStoredPanIds(void)
{
  memoryCleared = false;
}

uint16_t* emAfPluginNetworkSteeringGetStoredPanIdPointer(uint8_t index)
{
  if (index >= MAX_NETWORKS) {
    return NULL;
  }
  if (!memoryCleared) {
    MEMSET(storedNetworks, 0xFF, sizeof(uint16_t) * MAX_NETWORKS);
    memoryCleared = true;
  }
  return &(storedNetworks[index]);
}
