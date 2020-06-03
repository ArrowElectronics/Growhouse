// Copyright 2017 Silicon Laboratories, Inc.

#include "af.h"

#define R21_STACK_REVISION 21

#ifdef EZSP_HOST
  #define sendTrustCenterLinkKey ezspSendTrustCenterLinkKey
#else
  #define sendTrustCenterLinkKey emberSendTrustCenterLinkKey
#endif

void emberAfPluginDeviceDatabaseDiscoveryCompleteCallback(const EmberAfDeviceInfo *info)
{
  if (info->stackRevision < R21_STACK_REVISION) {
    EmberNodeId nodeId = emberLookupNodeIdByEui64(((EmberAfDeviceInfo *)info)->eui64);
    EmberStatus status = (nodeId == EMBER_NULL_NODE_ID
                          ? EMBER_ERR_FATAL
                          : sendTrustCenterLinkKey(nodeId, ((EmberAfDeviceInfo *)info)->eui64));
    emberAfCorePrint("Updating trust center link key of joining device ");
    emberAfPrintBigEndianEui64(info->eui64);
    emberAfCorePrintln(": 0x%X", status);
  }
}
