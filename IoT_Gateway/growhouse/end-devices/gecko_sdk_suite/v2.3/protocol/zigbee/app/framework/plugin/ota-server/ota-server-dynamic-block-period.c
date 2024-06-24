/**************************************************************************//**
 * Copyright 2018 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#include "app/framework/include/af.h"
#include "ota-server-dynamic-block-period.h"

#ifdef EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT

// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

#define INVALID_ENDPOINT      0
#define UNKNOWN_UNITS         0xFF
#define SHORT_DELAY_SECONDS   10    // Client delay if not probed yet
#define LONG_DELAY_SECONDS    60    // Client delay if no OTA sessions available
#define TTL_DEFAULT_SECONDS   60    // Seconds of inactivity before client wiped

#define READ_ATTRIBUTES_RSP_MIN_LENGTH  3 // Attribute ID (2) + ZCL status (1)
#define READ_ATTRIBUTES_RSP_OTA_CLUSTER_REVISION_LENGTH \
  READ_ATTRIBUTES_RSP_MIN_LENGTH + 3      // Data Type (1) + Attribute Value (2)

// OTA cluster revision 1 is the initial revision. The initial revision used
// milliseconds. Cluster revision 2 introduced SE1.2b, where the corresponding
// ZCL document defined the units in seconds (note that SE1.2a is listed in
// revision 2 in ZCL7, but used milliseconds). Revision 3 is the latest, and the
// switch was made back to milliseonds. Though revision 2 includes two SE
// versions that disputed the units, for simplicity we'll treat revision 2 as
// the only revision that uses seconds
#define OTA_CLUSTER_REVISION_THAT_USES_SECONDS  2

// The OTA server code dynamically finds out if the OTA client is treating the
// Minimum Block Period field in milliseconds or seconds. When an OTA transfer
// starts, we query the client for how it treats the field and then store it in
// an array.
// Since a device consumes an entry in the array by starting an OTA transfer, we
// keep track of the last time it asked for a piece of the OTA file. This is to
// prevent a device from forever taking an entry in storage*, which could
// potentially prohibit another device from starting an OTA transfer if all
// storage indices are taken (this can happen if a client begins an OTA transfer
// and then stops, for whatever reason, midway through the process)
#ifndef EMBER_AF_PLUGIN_OTA_SERVER_MAX_SIMULTANEOUS_OTA_DOWNLOADS
  #define EMBER_AF_PLUGIN_OTA_SERVER_MAX_SIMULTANEOUS_OTA_DOWNLOADS 5
#endif
#define MAX_DOWNLOADS EMBER_AF_PLUGIN_OTA_SERVER_MAX_SIMULTANEOUS_OTA_DOWNLOADS

#define STATE_NONE          1 // We've never probed the client
#define STATE_IN_DISCOVERY  2 // We've sent a ReadAttributes message
#define STATE_USES_SECONDS  3 // RX ReadAttribute resp: client uses seconds
#define STATE_USES_MS       4 // RX ReadAttribute resp: client uses milliseconds

typedef struct {
  EmberNodeId nodeId;
  uint8_t     endpoint;
  uint8_t     unitDiscoveryState; // STATE_ values
  uint32_t    ttlSec;             // Seconds of inactivity before entry is wiped
} EmAfOtaServerDownloadingNode;

EmAfOtaServerDownloadingNode downloadingNodes[MAX_DOWNLOADS];

static uint32_t lastSecondTick = 0;

#ifdef EMBER_TEST
 #define debugPrintln(...) emberAfOtaBootloadClusterPrintln(__VA_ARGS__)
#else
 #define debugPrintln(...)
#endif // EMBER_TEST

// -----------------------------------------------------------------------------
// Externs
// -----------------------------------------------------------------------------

extern uint8_t otaServerEndpoint;

// -----------------------------------------------------------------------------
// Forward Declarations
// -----------------------------------------------------------------------------

uint8_t getIndexForDownloadingNodeId(EmberNodeId nodeId);
void probeClientForMinBlockPeriodUnits(EmberNodeId nodeId, uint8_t endpoint);

// -----------------------------------------------------------------------------
// APIs
// -----------------------------------------------------------------------------

// Returns MAX_DOWNLOADS if not found
uint8_t getIndexForDownloadingNodeId(EmberNodeId nodeId)
{
  uint8_t nodeIndex;
  for (nodeIndex = 0; nodeIndex < MAX_DOWNLOADS; nodeIndex++) {
    if (downloadingNodes[nodeIndex].nodeId == nodeId) {
      break;
    }
  }
  return nodeIndex;
}

void emAfOtaServerDynamicBlockPeriodInit()
{
  // Initialize array to have no current OTA downloads
  uint8_t nodeIndex;
  for (nodeIndex = 0; nodeIndex < MAX_DOWNLOADS; nodeIndex++) {
    downloadingNodes[nodeIndex].nodeId = EMBER_NULL_NODE_ID;
    downloadingNodes[nodeIndex].unitDiscoveryState = STATE_NONE;
    downloadingNodes[nodeIndex].ttlSec = 0;
  }
}

void emAfOtaServerDynamicBlockPeriodTick()
{
  uint8_t nodeIndex;

  // First age the TTL field and purge any inactive clients
  uint32_t currentSecondTick = halCommonGetInt32uMillisecondTick()
                               / MILLISECOND_TICKS_PER_SECOND;
  uint32_t secondsElapsed = currentSecondTick - lastSecondTick;
  if (secondsElapsed) {
    lastSecondTick = currentSecondTick;
    for (nodeIndex = 0; nodeIndex < MAX_DOWNLOADS; nodeIndex++) {
      if (downloadingNodes[nodeIndex].ttlSec > 0) {
        if (downloadingNodes[nodeIndex].ttlSec > secondsElapsed) {
          downloadingNodes[nodeIndex].ttlSec -= secondsElapsed;
        } else { // Purge
          debugPrintln("OTA: purging 0x%2X from active downloads due to "
                       "inactivity",
                       downloadingNodes[nodeIndex].nodeId);
          downloadingNodes[nodeIndex].nodeId = EMBER_NULL_NODE_ID;
          downloadingNodes[nodeIndex].unitDiscoveryState = STATE_NONE;
          downloadingNodes[nodeIndex].ttlSec = 0;
        }
      }
    }
  }
}

// This function is called from ota-server whenever a client sends an Image
// Block Request. This function either returns SUCCESS, which indicates that we
// already know this client and know how it's treating the Minimum Block
// Request, or WAIT_FOR_DATA, which means either we don't know how the client is
// treating the field or it's the first time we see this node and don't have
// room for it in the active OTA downloads array
uint8_t emAfOtaServerCheckDynamicBlockPeriodDownload(EmberAfImageBlockRequestCallbackStruct *data)
{
  uint8_t nodeIndex;

  // Do we already know this node and how it treats the Minimum Block Period?
  for (nodeIndex = 0; nodeIndex < MAX_DOWNLOADS; nodeIndex++) {
    if (data->source == downloadingNodes[nodeIndex].nodeId) {
      if (downloadingNodes[nodeIndex].unitDiscoveryState >= STATE_USES_SECONDS) {
        // We already know this node and how it treats the Minimum Block Period
        downloadingNodes[nodeIndex].ttlSec = TTL_DEFAULT_SECONDS;
        return EMBER_ZCL_STATUS_SUCCESS;
      } else {
        // We know it but haven't probed it yet. Tell it to delay for a little
        data->waitTimeSecondsResponse = SHORT_DELAY_SECONDS;
        return EMBER_ZCL_STATUS_WAIT_FOR_DATA;
      }
    }
  }

  // We have never seen this node. Search for a free entry
  for (nodeIndex = 0; nodeIndex < MAX_DOWNLOADS; nodeIndex++) {
    if (EMBER_NULL_NODE_ID == downloadingNodes[nodeIndex].nodeId) {
      break;
    }
  }

  if (MAX_DOWNLOADS != nodeIndex) {
    // Free entry found. Mark it off. On the next server tick, we'll probe it
    downloadingNodes[nodeIndex].nodeId = data->source;
    downloadingNodes[nodeIndex].endpoint = data->clientEndpoint;
    downloadingNodes[nodeIndex].ttlSec = TTL_DEFAULT_SECONDS;
    data->waitTimeSecondsResponse = SHORT_DELAY_SECONDS;

    // We probe the client now, before sending the Image Block Response
    // This is due to the challenge with sleepies, who we know are at least
    // awake for some time during the Image Block Request exchange
    probeClientForMinBlockPeriodUnits(downloadingNodes[nodeIndex].nodeId,
                                      downloadingNodes[nodeIndex].endpoint);
  } else {
    // We've never seen this node, and we don't have room for an active OTA
    // session. Tell it to delay for a while
    data->waitTimeSecondsResponse = LONG_DELAY_SECONDS;
  }

  return EMBER_ZCL_STATUS_WAIT_FOR_DATA;
}

// This function is called when a client sends an UpgradeEndRequest, signalling
// that is is complete with its OTA download
void emAfOtaServerCompleteDynamicBlockPeriodDownload(EmberNodeId clientId)
{
  uint8_t nodeIndex = getIndexForDownloadingNodeId(clientId);
  if (nodeIndex < MAX_DOWNLOADS) {
    downloadingNodes[nodeIndex].nodeId = EMBER_NULL_NODE_ID;
    downloadingNodes[nodeIndex].unitDiscoveryState = STATE_NONE;
    downloadingNodes[nodeIndex].ttlSec = 0;
  }
}

// NOTE: this should only be called once we've discovered how the client is
// treating the Minimum Block Period field
bool emAfOtaServerDynamicBlockPeriodClientUsesSeconds(EmberNodeId clientId)
{
  bool usesSeconds = false;

  uint8_t nodeIndex = getIndexForDownloadingNodeId(clientId);

  if (nodeIndex < MAX_DOWNLOADS) {
    usesSeconds = (STATE_USES_SECONDS
                   == downloadingNodes[nodeIndex].unitDiscoveryState);
  }

  return usesSeconds;
}

void probeClientForMinBlockPeriodUnits(EmberNodeId nodeId, uint8_t endpoint)
{
  uint16_t attributeIds;
  attributeIds = ZCL_OTA_BOOTLOAD_CLUSTER_CLUSTER_REVISION_CLIENT_ATTRIBUTE_ID;
  emberAfFillCommandGlobalServerToClientReadAttributes(ZCL_OTA_BOOTLOAD_CLUSTER_ID,
                                                       &attributeIds,
                                                       sizeof(attributeIds));
  emberAfSetCommandEndpoints(otaServerEndpoint, endpoint);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, nodeId);
}

void emAfOtaServerReadAttributesResponseCallback(EmberNodeId nodeId,
                                                 uint8_t * buffer,
                                                 uint16_t bufLen)
{
  uint8_t nodeIndex = getIndexForDownloadingNodeId(nodeId);

  if (nodeIndex >= MAX_DOWNLOADS) {
    // We have no record of this node
    return;
  }

  // Each record in the response has a two-byte attribute id and a one-byte
  // status. If the status is SUCCESS, there will also be a one-byte type and
  // variable-length data.
  if (bufLen >= READ_ATTRIBUTES_RSP_MIN_LENGTH) {
    uint8_t bufIndex = 0;
    EmberAfAttributeId attributeId = (EmberAfAttributeId)emberAfGetInt16u(buffer,
                                                                          bufIndex,
                                                                          bufLen);
    bufIndex += 2;

    if (ZCL_OTA_BOOTLOAD_CLUSTER_CLUSTER_REVISION_CLIENT_ATTRIBUTE_ID
        != attributeId) {
      return;
    }

    // Replenish TTL
    downloadingNodes[nodeIndex].ttlSec = TTL_DEFAULT_SECONDS;

    EmberAfStatus zclStatus = (EmberAfStatus)emberAfGetInt8u(buffer,
                                                             bufIndex,
                                                             bufLen);
    bufIndex++;

    if ((EMBER_ZCL_STATUS_SUCCESS == zclStatus)
        && (bufLen >= READ_ATTRIBUTES_RSP_OTA_CLUSTER_REVISION_LENGTH)) {
      uint8_t dataType = emberAfGetInt8u(buffer, bufIndex, bufLen);
      bufIndex++;

      if (dataType == ZCL_INT16U_ATTRIBUTE_TYPE) {
        uint16_t otaClusterRevision = emberAfGetInt16u(buffer, bufIndex, bufLen);

        if (OTA_CLUSTER_REVISION_THAT_USES_SECONDS == otaClusterRevision) {
          emberAfOtaBootloadClusterPrintln("OTA client 0x%2X has OTA cluster "
                                           "revision %d and uses seconds",
                                           nodeId,
                                           otaClusterRevision);
          downloadingNodes[nodeIndex].unitDiscoveryState = STATE_USES_SECONDS;
        } else {
          emberAfOtaBootloadClusterPrintln("OTA client 0x%2X has OTA cluster "
                                           "revision %d and uses ms",
                                           nodeId,
                                           otaClusterRevision);
          downloadingNodes[nodeIndex].unitDiscoveryState = STATE_USES_MS;
        }
        return;
      }
    }
  }

  // For any failure condition, we default to using milliseconds for the client
  emberAfOtaBootloadClusterPrintln("Error in OTA client 0x%2X "
                                   "ReadAttributesResponse. Using ms",
                                   nodeId);
  downloadingNodes[nodeIndex].unitDiscoveryState = STATE_USES_MS;
}

#endif // EMBER_AF_PLUGIN_OTA_SERVER_DYNAMIC_MIN_BLOCK_PERIOD_SUPPORT
