// Copyright 2017 Silicon Laboratories, Inc.                                *80*

#include PLATFORM_HEADER //compiler/micro specifics, types

#include "app/framework/include/af.h"
#include "stack/include/ember-types.h"
#include "stack/include/error.h"

#include "hal/hal.h"
#include "app/framework/util/common.h"
#include "app/framework/plugin/device-table/device-table.h"
#include <string.h>
#include "app/framework/plugin-host/coap-server/coap-server.h"
#include "app/framework/plugin/device-table/device-table.h"
#include "app/framework/plugin-host/gateway-relay-coap/gateway-relay-coap-cbor.h"
#include "util/third_party/libcoap/include/coap/pdu.h"

static uint16_t returnCode = COAP_RESPONSE_OK;
#define RETURN_STRING_LENGTH 100

#define CLUSTER_STRING_LENGTH 6
static char clusterString[CLUSTER_STRING_LENGTH];

#define CLUSTER_LIST_START 11

#define ATTRIBUTE_BUFFER_ATTRIBUTE_ID_LOW_BYTE 0
#define ATTRIBUTE_BUFFER_ATTRIBUTE_ID_HIGH_BYTE 1
#define ATTRIBUTE_BUFFER_SUCCESS_CODE 2
#define ATTRIBUTE_BUFFER_DATA_TYPE 3
#define ATTRIBUTE_BUFFER_DATA_START 4

// read attribute response code.
static void setUpErrorCode(uint16_t error);

// --------------------------------
void emAfGatewayRelayCoapProcessIncomingCommand(uint16_t cluster,
                                                uint8_t commandId,
                                                int16_t portCounter,
                                                uint16_t endpoint);

static void sendAttributeReportMessage(uint16_t device,
                                       uint8_t endpoint,
                                       uint16_t cluster,
                                       uint16_t attribute,
                                       uint32_t data);

static void setReadAttributeResponseData(uint16_t device,
                                         uint8_t endpoint,
                                         uint16_t cluster,
                                         uint16_t attribute,
                                         uint32_t data);
static void createCborDataFromReadAttributeResponse(uint8_t *buffer,
                                                    uint8_t length);
static uint8_t getNextOptByte(void);

void resetOpt(void);
static void printData(uint8_t * data, uint16_t length, char *formatString);

bool isInt(uint8_t *buffer, uint8_t maxLength)
{
  uint8_t i;

  for (i = 0; i < maxLength; i++) {
    if (buffer[i] == 0) {
      return i != 0;
    }
    if (buffer[i] < '0'
        || buffer[i] > '9') {
      return false;
    }
  }

  return false; // too long
}

uint16_t stringToInt16(uint8_t *buffer)
{
  uint32_t data;
  uint16_t val16;

  sscanf(data, "%x", &data);

  val16 = (uint16_t) data;

  return val16;
}

void emberAfPluginGatewayRelayCoapInitCallback(void)
{
  setUpErrorCode(COAP_RESPONSE_NOT_FOUND);
}

// device-table callbacks and interface
void emberAfPluginDeviceTableInitialized(void)
{
  uint16_t i;
  // call to coap server to set up the channels.
  for (i = 0; i < PLUGIN_COAP_SERVER_MAX_PORTS; i++) {
    if (emberAfDeviceTableGetNodeIdFromIndex(i)
        != EMBER_AF_PLUGIN_DEVICE_TABLE_NULL_NODE_ID) {
      emberAfPluginCoapServerCreateDevice(i);
    }
  }
}

void emberAfPluginDeviceTableIndexRemovedCallback(uint16_t index)
{
  emberAfPluginCoapServerRemoveDevice(index);
}

void emberAfPluginDeviceTableIndexAddedCallback(uint16_t index)
{
  emberAfPluginCoapServerCreateDevice(index);
}

int emberAfPluginCoapServerCurrentPort(void);

static void handleAttributeRequest(uint16_t clusterId,
                                   int portCounter,
                                   uint16_t endpoint,
                                   uint8_t *ptr);

// response data
static uint8_t returnString[RETURN_STRING_LENGTH] = { 0, };

static void constructEndpointList(void)
{
  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH - 1);

  if (emAfPluginGatewayRelayCoapCborWriteArrayTag()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteUint8(1)) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }

  returnCode = COAP_RESPONSE_DATA;
}

static void constructClusterList(uint16_t deviceIndex)
{
  EmberAfPluginDeviceTableEntry *currentDevice;
  char direction;
  uint8_t i;

  currentDevice = emberAfDeviceTableFindDeviceTableEntry(deviceIndex);

  if (currentDevice->nodeId == EMBER_AF_PLUGIN_DEVICE_TABLE_NULL_NODE_ID) {
    setUpErrorCode(COAP_RESPONSE_NOT_FOUND);
    return;
  }

  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH - 1);

  emAfPluginGatewayRelayCoapCborWriteArrayTag();

  for (i = 0; i < EMBER_AF_PLUGIN_DEVICE_TABLE_CLUSTER_SIZE; i++) {
    if (currentDevice->clusterIds[i] == ZCL_NULL_CLUSTER_ID) {
      emAfPluginGatewayRelayCoapCborWriteBreak();
      returnCode = COAP_RESPONSE_DATA;
      return;
    }

    if (i >= currentDevice->clusterOutStartPosition) {
      direction = 'c';
    } else {
      direction = 's';
    }
    snprintf(clusterString,
             CLUSTER_STRING_LENGTH,
             "%c%x",
             direction,
             currentDevice->clusterIds[i]);
    if (emAfPluginGatewayRelayCoapCborWriteString(clusterString)) {
      setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
      return;
    }
  }
  emAfPluginGatewayRelayCoapCborWriteBreak();
  returnCode = COAP_RESPONSE_DATA;

  return;
}

// take a string of characters and make it into a list.
static void constructResourceList(char *list)
{
  uint8_t i;
  char resourceString[3];

  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH - 1);

  emAfPluginGatewayRelayCoapCborWriteArrayTag();
  resourceString[1] = 0;
  for (i = 0; i < strlen(list); i++) {
    resourceString[0] = list[i];
    emAfPluginGatewayRelayCoapCborWriteString(resourceString);
  }

  emAfPluginGatewayRelayCoapCborWriteBreak();

  returnCode = COAP_RESPONSE_DATA;
}

EmberStatus checkEndpoint(uint16_t endpoint, uint16_t portCounter)
{
  EmberAfPluginDeviceTableEntry *currentDevice
    = emberAfDeviceTableFindDeviceTableEntry(portCounter);

  if (currentDevice->endpoint != endpoint) {
    return EMBER_BAD_ARGUMENT;
  }

  return EMBER_SUCCESS;
}

static void setUpErrorCode(uint16_t error)
{
  returnCode = error;
  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH - 1);
}

static bool checkMethod(uint8_t method, uint8_t desiredMethod)
{
  if (method == 0
      || method == desiredMethod) {
    return false;
  } else {
    setUpErrorCode(COAP_RESPONSE_METHOD_NOT_ALLOWED);
    return true;
  }
}

uint16_t emberAfPluginGatewayRelayCoapReturnStringLength(void)
{
  //return strlen(returnString);
  return emAfPluginGatewayRelayCoapCborCurrentWriteDataLength();
}

uint16_t emberAfPluginGatewayRelayCoapReturnCode(void)
{
  return returnCode;
}

void emberAfPluginGatewayRelayCoapCopyReturnString(char *ptr)
{
  MEMCOPY((uint8_t *) ptr,
          (uint8_t *) returnString,
          emAfPluginGatewayRelayCoapCborCurrentWriteDataLength());

  returnString[0] = 0;
}

static bool getUint16Opttok(uint16_t *val16);

static volatile bool waitForResponse = true;

bool emberAfPluginCoapServerPrepareDelayedResponse(void);
static void fillAttributeRequestWildcard(uint16_t clusterId);

static void handleAttributeRequest(uint16_t clusterId,
                                   int portCounter,
                                   uint16_t endpoint,
                                   uint8_t *ptr)
{
  uint16_t attributeId;
  uint8_t attributeIdBuffer[2];

  printf("handleAttributeRequest %c\r\n", getNextOptByte());

  if (getNextOptByte() == 'C') {
    printf("handle wildcard\r\n");

    // this is a complex query.  Assume it is a wildcard for now.
    fillAttributeRequestWildcard(clusterId);
  } else {
    printf("handle single attribute\r\n");

    // asking for a single attribute.
    getUint16Opttok(&attributeId);
    attributeIdBuffer[0] = LOW_BYTE(attributeId);
    attributeIdBuffer[1] = HIGH_BYTE(attributeId);

    emberAfFillCommandGlobalClientToServerReadAttributes(clusterId,
                                                         attributeIdBuffer,
                                                         2);
  }

  if (portCounter < 0) {
    // message sent to 5683
    emberAfSetCommandEndpoints(1, 1);
    emberAfDeviceTableCommandIndexSend(endpoint - 1);
  } else {
    // port counter happens to be the index into the device table.
    emberAfSetCommandEndpoints(1, endpoint);
    emberAfDeviceTableCommandIndexSend((uint16_t) portCounter);
  }

  printf("send read request: %ld\r\n", halCommonGetInt32uMillisecondTick());

  // Inform coap server that we plan to send a response.
  emberAfPluginCoapServerPrepareDelayedResponse();
}

bool emAfGatewayRelayCoapReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                        uint8_t* buffer,
                                                        uint16_t bufLen)
{
  uint16_t currentData;
  uint16_t currentAttribute;
  uint8_t status;

  // Note:  for now, we assume the device has a single endpoint.
  uint16_t nodeId = emberGetSender();
  uint16_t device = emberAfDeviceTableGetIndexFromNodeId(nodeId);
  EmberAfPluginDeviceTableEntry *dtEntry;
  uint8_t endpoint;

  printf("receive read response: %ld\r\n", halCommonGetInt32uMillisecondTick());

  if (device == EMBER_AF_PLUGIN_DEVICE_TABLE_NULL_INDEX) {
    // don't have a valid index.  Need to return.
    return false;
  }

  dtEntry = emberAfDeviceTableFindDeviceTableEntry(device);
  endpoint = dtEntry->endpoint;

  createCborDataFromReadAttributeResponse(buffer, bufLen);

  emberAfPluginCoapServerSendDelayedResponse(
    returnString,
    emAfPluginGatewayRelayCoapCborCurrentWriteDataLength());

  emAfPluginGatewayRelayCoapCborWriteInit(returnString, 0);

  waitForResponse = false;

  return false;
}

bool emAfGatewayRelayCoapReportAttributesCallback(EmberAfClusterId clusterId,
                                                  uint8_t * buffer,
                                                  uint16_t bufLen)
{
  emAfGatewayRelayCoapReadAttributesResponseCallback(clusterId,
                                                     buffer,
                                                     bufLen);
  return false;
}

// ----------------------------------------------------------------------------
// Construct outgoing attributes request for a wildcard.
#define ATTRIBUTE_TERMINATOR 0xffff
typedef struct {
  uint16_t clusterId;
  uint16_t attributeArray[10];
} AttributeList;

AttributeList attributeList[] = {
  { 0x0006, { 0x0000, 0xfffd, ATTRIBUTE_TERMINATOR } },
  { 0x0008, { 0x0000, 0xfffd, ATTRIBUTE_TERMINATOR } },
  { 0x0300, { 0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0007, 0x0008, 0x000F,
              0xfffd, ATTRIBUTE_TERMINATOR } },
  { ATTRIBUTE_TERMINATOR, { 0x0000, 0xfffd, ATTRIBUTE_TERMINATOR } },
};

static uint16_t findEntry(uint16_t clusterId)
{
  uint16_t i = 0;

  while (attributeList[i].clusterId != clusterId
         && attributeList[i].clusterId != ATTRIBUTE_TERMINATOR) {
    i++;
  }
  return i;
}

uint16_t findNumAttributes(uint16_t entry)
{
  uint16_t i = 0;

  while (attributeList[entry].attributeArray[i] != ATTRIBUTE_TERMINATOR) {
    i++;
  }
  return i;
}

static void fillAttributeRequestWildcard(uint16_t clusterId)
{
  uint16_t entry = findEntry(clusterId);
  uint16_t numAttributes = findNumAttributes(entry);
  uint16_t i;
  uint16_t *attributes = attributeList[entry].attributeArray;
  uint8_t *buffer;

  buffer = malloc(numAttributes * 2);
  for (i = 0; i < numAttributes; i++) {
    buffer[2 * i] = LOW_BYTE(attributes[i]);
    buffer[2 * i + 1] = HIGH_BYTE(attributes[i]);
  }

  printf("ReadAttributeData: ");
  printData(buffer, numAttributes * 2, "%02x ");
  emberAfFillCommandGlobalClientToServerReadAttributes(clusterId,
                                                       buffer,
                                                       numAttributes * 2);

  free(buffer);
}

// ----------------------------------------------------------------------------
// Translate incoming attributes read response to outgoing dotdot equivalent
// message.
static void createCborDataFromReadAttributeResponse(uint8_t *buffer,
                                                    uint8_t length)
{
  uint8_t counter = 0;
  uint16_t attributeId;
  uint8_t successCode, attributeType;
  uint8_t data8u;
  uint16_t data16u;
  uint32_t data32u;
  int8_t data8s;
  int16_t data16s;

  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH);

  if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }

  while (counter < length) {
    attributeId = HIGH_LOW_TO_INT(buffer[ATTRIBUTE_BUFFER_ATTRIBUTE_ID_HIGH_BYTE],
                                  buffer[ATTRIBUTE_BUFFER_ATTRIBUTE_ID_LOW_BYTE]);
    printf("Attriubte Id %04x\r\n", attributeId);
    successCode = buffer[ATTRIBUTE_BUFFER_SUCCESS_CODE];
    attributeType = buffer[ATTRIBUTE_BUFFER_DATA_TYPE];
    buffer += ATTRIBUTE_BUFFER_DATA_START;
    counter += ATTRIBUTE_BUFFER_DATA_START;

    if (successCode == EMBER_ZCL_STATUS_SUCCESS) {
      switch (attributeType) {
        case ZCL_INT8U_ATTRIBUTE_TYPE:
        case ZCL_ENUM8_ATTRIBUTE_TYPE:
        case ZCL_DATA8_ATTRIBUTE_TYPE:
        case ZCL_BITMAP8_ATTRIBUTE_TYPE:
          data8u = *buffer;

          if (emAfPluginGatewayRelayCoapCborWriteUint16(attributeId)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteString("v")) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteUint8(data8u)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }

          buffer += 1;
          counter += 1;

          break;
        case ZCL_INT16U_ATTRIBUTE_TYPE:
        case ZCL_ENUM16_ATTRIBUTE_TYPE:
        case ZCL_DATA16_ATTRIBUTE_TYPE:
        case ZCL_BITMAP16_ATTRIBUTE_TYPE:
          data16u = HIGH_LOW_TO_INT(buffer[1], buffer[0]);

          if (emAfPluginGatewayRelayCoapCborWriteUint16(attributeId)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteString("v")) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteUint16(data16u)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }

          buffer += 2;
          counter += 2;

          break;
        case ZCL_INT32U_ATTRIBUTE_TYPE:
        case ZCL_DATA32_ATTRIBUTE_TYPE:
        case ZCL_BITMAP32_ATTRIBUTE_TYPE:
          data32u = buffer[3];

          data32u <<= 8;
          data32u += buffer[2];
          data32u <<= 8;
          data32u += buffer[1];
          data32u <<= 8;
          data32u += buffer[0];

          if (emAfPluginGatewayRelayCoapCborWriteUint16(attributeId)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteString("v")) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteUint32(data32u)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }

          buffer += 4;
          counter += 4;

          break;

          break;
        case ZCL_INT8S_ATTRIBUTE_TYPE:
          data8s = (int8_t) *buffer;

          if (emAfPluginGatewayRelayCoapCborWriteUint16(attributeId)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteString("v")) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteInt8(data8s)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }

          buffer += 1;
          counter += 1;

          break;
        case ZCL_INT16S_ATTRIBUTE_TYPE:
          data16s = (int16_t) HIGH_LOW_TO_INT(buffer[1], buffer[0]);

          if (emAfPluginGatewayRelayCoapCborWriteUint16(attributeId)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteString("v")) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteInt16(data16s)) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }

          buffer += 2;
          counter += 2;

          break;
        default:
          printf("error:  unknown attribute type\r\n");
          if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
            setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
          }
          return;
          break;
      }
    }
  }
  if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
}

// ----------------------------------------------------------------------------
// gateway rely coap heartbeat code
EmberEventControl emberAfPluginGatewayRelayCoapHeartbeatEventControl;
static uint16_t heartbeatPeriodSeconds = 0;

static void sendAttributeReportMessage(uint16_t device,
                                       uint8_t endpoint,
                                       uint16_t cluster,
                                       uint16_t attribute,
                                       uint32_t data);

void emberAfPluginGatewayRelayCoapHeartbeatEventHandler(void)
{
  uint16_t i;
  EmberAfPluginDeviceTableEntry *pEntry;

  emberEventControlSetDelayMS(emberAfPluginGatewayRelayCoapHeartbeatEventControl,
                              1000 * (uint32_t) heartbeatPeriodSeconds);

  for (i = 0; i < PLUGIN_COAP_SERVER_MAX_PORTS; i++) {
    pEntry = emberAfDeviceTableFindDeviceTableEntry(i);

    if (pEntry->nodeId != EMBER_AF_PLUGIN_DEVICE_TABLE_NULL_NODE_ID) {
      sendAttributeReportMessage(i, pEntry->endpoint, 8, 0, 200);
    }
  }
}

void emberAfPluginGatewayRelayCoapStartHeartbeat(uint16_t seconds)
{
  heartbeatPeriodSeconds = seconds;

  emberEventControlSetActive(emberAfPluginGatewayRelayCoapHeartbeatEventControl);
}

void emberAfPluginGatewayRelayCoapStopHeartbeat(void)
{
  emberEventControlSetInactive(emberAfPluginGatewayRelayCoapHeartbeatEventControl);
}

// read attribute response message.
static void setReadAttributeResponseData(uint16_t device,
                                         uint8_t endpoint,
                                         uint16_t cluster,
                                         uint16_t attribute,
                                         uint32_t data)
{
  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH);

  if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteUint16(attribute)) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteString("v")) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteUint32(data)) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
}

// ----------------------------------------------------------------------------
// gateway relay coap construct messages code
#define BUFFER_LENGTH 50
static uint8_t outgoingMessageUri[BUFFER_LENGTH];
static uint8_t outgoingMessagePayload[BUFFER_LENGTH];

static void sendAttributeReportMessage(uint16_t device,
                                       uint8_t endpoint,
                                       uint16_t cluster,
                                       uint16_t attribute,
                                       uint32_t data)
{
  uint16_t bytesUsed = 0;
  uint8_t *buffer = outgoingMessageUri;

  sprintf((char *) buffer, "zcl/e/%d/s%x/n", endpoint, cluster);

  emAfPluginGatewayRelayCoapCborWriteInit(returnString, RETURN_STRING_LENGTH);

  if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteString("r")) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteUint32(0)) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteString("a")) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteMapTag()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteUint16(attribute)) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteUint32(data)) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }
  if (emAfPluginGatewayRelayCoapCborWriteBreak()) {
    setUpErrorCode(COAP_RESPONSE_INTERNAL_ERROR);
  }

  bytesUsed += emAfPluginGatewayRelayCoapCborCurrentWriteDataLength();
}

// ----------------------------------------------------------------------------
// gateway relay coap parse Options
static uint16_t bufferLength;
static uint8_t returnToken[BUFFER_LENGTH];
static uint8_t *currentBuffer;
static uint16_t finger;
static uint8_t currentOption;

static uint8_t getNextOptByte(void)
{
  return currentBuffer[finger];
}

uint8_t *optTok(uint8_t *buffer, uint16_t length, uint8_t *option)
{
  uint8_t optionByte, optionLength, i;

  if (buffer != NULL) {
    currentBuffer = buffer;
    bufferLength = length;
    currentOption = COAP_OPTION_URI_PATH;
    finger = 0;
  }

  if (finger >= bufferLength) {
    return NULL;
  }

  optionByte = currentBuffer[finger++];

  if (optionByte == 0xff) {
    return NULL;
  }

  optionLength = optionByte & 0x0f;
  currentOption = currentOption + (optionByte >> 4);

  for (i = 0; i < optionLength; i++) {
    returnToken[i] = currentBuffer[finger++];
  }

  returnToken[i] = 0;
  *option = currentOption;
  return returnToken;
}

void resetOpt(void)
{
  finger = 0;
}

static bool getUint16Opttok(uint16_t *val16)
{
  uint32_t data = 0;
  uint8_t opt;
  uint8_t *ptr = optTok(NULL, 0, &opt);

  if (ptr == NULL) {
    return false;
  }

  sscanf((char *) ptr, "%x", &data);

  *val16 = (uint16_t) data;

  return true;
}

static bool getClusterOpttok(uint16_t *cluster, char *clientServer)
{
  uint32_t data;
  uint8_t opt;
  uint8_t *ptr = optTok(NULL, 0, &opt);

  if (ptr == NULL) {
    return false;
  }

  *clientServer = *ptr;

  ptr++;

  sscanf((char *) ptr, "%x", &data);

  *cluster = (uint16_t) data;

  return true;
}

static void printData(uint8_t * data, uint16_t length, char *formatString)
{
  uint16_t i;
  printf("Data: ");

  for (i = 0; i < length; i++) {
    printf(formatString, data[i]);
  }

  printf("\r\n");
}

bool emberAfGatewayRelayCoapParseDotdotMessage(uint8_t length,
                                               uint8_t *string,
                                               int portCounter,
                                               uint16_t method)
{
  uint8_t *ptr, option;
  uint16_t endpoint = 0xffff;
  uint16_t cluster;
  char clientServer;
  uint16_t commandId;

  uint16_t i, cborPayload = length, cborLength;

  printData(string, length, "%02x ");

  returnCode = COAP_RESPONSE_BAD_REQUEST; // bad request by default.

  for (i = 0; i < length; i++) {
    if (string[i] == 0xff) {
      cborPayload = i + 1;
      string[i] = ':';
      break;
    }
  }

  cborLength = length - cborPayload;

  if (cborLength > 0) {
    emAfPluginGatewayRelayCoapCborReadInit(string + cborPayload, cborLength);
  } else {
    emAfPluginGatewayRelayCoapCborReadInit(NULL, 0);
  }

  ptr = optTok(string, length, &option);

  if (ptr == NULL) {
    // respond with {0,"e"} here

    if (!checkMethod(method, COAP_REQUEST_GET)) {
      constructResourceList("e");
    }

    return false;
  }

  if (strncmp((char *) ptr, "e", 1) != 0) {
    if (isInt(ptr, 3)) {
      resetOpt();
    } else {
      // error:  only accept endpoint here.
      setUpErrorCode(404);
      return false;
    }
  }

  if (!getUint16Opttok(&endpoint)) {
    // respond with endpoints list here
    if (!checkMethod(method, COAP_REQUEST_GET)) {
      constructEndpointList();
    }
    return false;
  }

  // TBD: need a way to handle multiple endpoints here.
  if (portCounter == -1) {
    printf("Default port, switching to endpoint mode %d\r\n");
    portCounter = endpoint - 1;
    endpoint = 1;
  }

  if (checkEndpoint(endpoint, portCounter)) {
    setUpErrorCode(404);
    return false;
  }

  if (!getClusterOpttok(&cluster, &clientServer)) {
    // respond with cluster list here
    if (!checkMethod(method, COAP_REQUEST_GET)) {
      constructClusterList(portCounter);
    }
    return false;
  }

  ptr = optTok(NULL, 0, &option);
  if (ptr == NULL) {
    if (!checkMethod(method, COAP_REQUEST_GET)) {
      constructResourceList("ac");
    }
    return false;
  }

  switch (ptr[0]) {
    case 'c':
      if (!getUint16Opttok(&commandId)) {
        if (!checkMethod(method, COAP_REQUEST_GET)) {
          setUpErrorCode(COAP_RESPONSE_NOT_IMPLEMENTED);
          return false;
        }
      } else {
        if (!checkMethod(method, COAP_REQUEST_POST)) {
          emAfGatewayRelayCoapProcessIncomingCommand(cluster,
                                                     commandId,
                                                     (uint16_t) portCounter,
                                                     endpoint);
          returnCode = COAP_RESPONSE_CREATED;
        }
      }
      break;
    case 'a':
      if (!checkMethod(method, COAP_REQUEST_GET)) {
        handleAttributeRequest(cluster, portCounter, endpoint, ptr);
        returnCode = COAP_RESPONSE_CREATED;
      }
      break;
    default:
      emberAfCorePrintln("Unknown payload: %s", ptr);
      break;
  }

  return true;
}
