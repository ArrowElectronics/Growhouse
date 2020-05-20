// *******************************************************************
// * zll-utility-server.c
// *
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

#include "app/framework/include/af.h"
#include "app/framework/util/common.h"

static uint8_t getMaxLength(void)
{
  uint8_t maxLength = emberAfMaximumApsPayloadLength(EMBER_OUTGOING_DIRECT,
                                                     emberAfResponseDestination,
                                                     &emberAfResponseApsFrame);
  if (EMBER_AF_RESPONSE_BUFFER_LEN < maxLength) {
    maxLength = EMBER_AF_RESPONSE_BUFFER_LEN;
  }
  return maxLength;
}

bool emberAfZllCommissioningClusterGetGroupIdentifiersRequestCallback(uint8_t startIndex)
{
  EmberStatus sendStatus;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t total = emberAfPluginZllCommissioningGroupIdentifierCountCallback(endpoint);
  uint8_t i, *count, maxLength;

  emberAfZllCommissioningClusterPrintln("RX: GetGroupIdentifiersRequest 0x%x",
                                        startIndex);

  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                             | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                             | EMBER_AF_DEFAULT_RESPONSE_POLICY_RESPONSES),
                            ZCL_ZLL_COMMISSIONING_CLUSTER_ID,
                            ZCL_GET_GROUP_IDENTIFIERS_RESPONSE_COMMAND_ID,
                            "uu",
                            total,
                            startIndex);

  count = &appResponseData[appResponseLength];
  emberAfPutInt8uInResp(0); // temporary count

  maxLength = getMaxLength();
  for (i = startIndex; i < total && appResponseLength + 3 <= maxLength; i++) {
    EmberAfPluginZllCommissioningGroupInformationRecord record;
    if (emberAfPluginZllCommissioningGroupIdentifierCallback(endpoint,
                                                             i,
                                                             &record)) {
      emberAfPutInt16uInResp(record.groupId);
      emberAfPutInt8uInResp(record.groupType);
      (*count)++;
    }
  }

  sendStatus = emberAfSendResponse();
  if (EMBER_SUCCESS != sendStatus) {
    emberAfZllCommissioningClusterPrintln("ZLL: failed to send %s response: "
                                          "0x%x",
                                          "group_identifiers",
                                          sendStatus);
  }
  return true;
}

bool emberAfZllCommissioningClusterGetEndpointListRequestCallback(uint8_t startIndex)
{
  EmberStatus sendStatus;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t total = emberAfPluginZllCommissioningEndpointInformationCountCallback(endpoint);
  uint8_t i, *count, maxLength;

  emberAfZllCommissioningClusterPrintln("RX: GetEndpointListRequest 0x%x",
                                        startIndex);

  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                             | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                             | EMBER_AF_DEFAULT_RESPONSE_POLICY_RESPONSES),
                            ZCL_ZLL_COMMISSIONING_CLUSTER_ID,
                            ZCL_GET_ENDPOINT_LIST_RESPONSE_COMMAND_ID,
                            "uu",
                            total,
                            startIndex);

  count = &appResponseData[appResponseLength];
  emberAfPutInt8uInResp(0); // temporary count

  maxLength = getMaxLength();
  for (i = startIndex; i < total && appResponseLength + 8 <= maxLength; i++) {
    EmberAfPluginZllCommissioningEndpointInformationRecord record;
    if (emberAfPluginZllCommissioningEndpointInformationCallback(endpoint,
                                                                 i,
                                                                 &record)) {
      emberAfPutInt16uInResp(record.networkAddress);
      emberAfPutInt8uInResp(record.endpointId);
      emberAfPutInt16uInResp(record.profileId);
      emberAfPutInt16uInResp(record.deviceId);
      emberAfPutInt8uInResp(record.version);
      (*count)++;
    }
  }

  sendStatus = emberAfSendResponse();
  if (EMBER_SUCCESS != sendStatus) {
    emberAfZllCommissioningClusterPrintln("ZLL: failed to send %s response: "
                                          "0x%x",
                                          "endpoint_list",
                                          sendStatus);
  }
  return true;
}
