// *******************************************************************
// * process-cluster-message.c
// *
// * This file contains a function that processes cluster-specific
// * ZCL message.
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

// this file contains all the common includes for clusters in the zcl-util
#include "common.h"

// for pulling in defines dealing with EITHER server or client
#include "af-main.h"

// the EM260 host needs to include the config file
#ifdef EZSP_HOST
  #include "config.h"
#endif

//------------------------------------------------------------------------------
// Forward Declarations

EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand *cmd);

//------------------------------------------------------------------------------

bool emAfProcessClusterSpecificCommand(EmberAfClusterCommand *cmd)
{
  EmberAfStatus status;

  // if we are disabled then we can only respond to read or write commands
  // or identify cluster (see device enabled attr of basic cluster)
  if (!emberAfIsDeviceEnabled(cmd->apsFrame->destinationEndpoint)
      && cmd->apsFrame->clusterId != ZCL_IDENTIFY_CLUSTER_ID) {
    emberAfCorePrintln("%pd, dropping ep 0x%x clus 0x%2x cmd 0x%x",
                       "disable",
                       cmd->apsFrame->destinationEndpoint,
                       cmd->apsFrame->clusterId,
                       cmd->commandId);
    emberAfSendDefaultResponse(cmd, EMBER_ZCL_STATUS_FAILURE);
    return true;
  }

#ifdef ZCL_USING_KEY_ESTABLISHMENT_CLUSTER_CLIENT
  if (cmd->apsFrame->clusterId == ZCL_KEY_ESTABLISHMENT_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_SERVER_TO_CLIENT
      && emberAfKeyEstablishmentClusterClientCommandReceivedCallback(cmd)) {
    return true;
  }
#endif
#ifdef ZCL_USING_KEY_ESTABLISHMENT_CLUSTER_SERVER
  if (cmd->apsFrame->clusterId == ZCL_KEY_ESTABLISHMENT_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER
      && emberAfKeyEstablishmentClusterServerCommandReceivedCallback(cmd)) {
    return true;
  }
#endif

#ifdef ZCL_USING_OTA_BOOTLOAD_CLUSTER_CLIENT
  if (cmd->apsFrame->clusterId == ZCL_OTA_BOOTLOAD_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_SERVER_TO_CLIENT
      && emberAfOtaClientIncomingMessageRawCallback(cmd)) {
    return true;
  }
#endif
#ifdef ZCL_USING_OTA_BOOTLOAD_CLUSTER_SERVER
  if (cmd->apsFrame->clusterId == ZCL_OTA_BOOTLOAD_CLUSTER_ID
      && cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER
      && emberAfOtaServerIncomingMessageRawCallback(cmd)) {
    return true;
  }
#endif

  // Pass the command to the generated command parser for processing
  status = emberAfClusterSpecificCommandParse(cmd);
  if (status != EMBER_ZCL_STATUS_SUCCESS) {
    emberAfSendDefaultResponse(cmd, status);
  }

  return true;
}
