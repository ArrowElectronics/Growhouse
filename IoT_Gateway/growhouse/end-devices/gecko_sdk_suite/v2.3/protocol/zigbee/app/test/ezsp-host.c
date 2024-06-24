// File: ezsp-host.c
//
// Description: A simple Host application for testing the EM260. A serial
// command processor allows different EZSP commands to be sent to the EM260.
//
// Copyright 2006 by Ember Corporation. All rights reserved.                *80*

#include PLATFORM_HEADER

#include "stack/include/ember-types.h"
#include "stack/include/packet-buffer.h"
#include "stack/include/error.h"

#include "hal/hal.h"

#include "plugin/serial/serial.h"
#include "app/util/serial/command-interpreter.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "app/util/ezsp/serial-interface.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "app/util/zigbee-framework/zigbee-device-host.h"
#include "app/util/zigbee-framework/fragment-host.h"
#include "app/framework/plugin/concentrator/source-route-host.h"

#ifdef EZSP_ASH
  #include "app/ezsp-host/ash/ash-host.h"
  #ifndef EMBER_TEST
    #include "app/ezsp-host/ezsp-host-ui.h" // for ezspProcessCommandOptions
  #endif
#endif

#ifndef APP_NAME
  #define APP_NAME "Host EZSP Test"
  #define EXE_NAME "host-ezsp-test"
#endif

#ifndef EXE_NAME
  #define EXE_NAME APP_NAME
#endif

#if defined(EZSP_HOST) && defined(UNIX_HOST)    \
  && !defined(EZSP_ASH) && !defined(EMBER_TEST) \
  && !defined(EZSP_USB)     // && defined(EZSP_SPI)

  #define UNIX_SPI_HOST

  #include "linux-serial.h"
  #include <errno.h>

bool halNcpEnableLogging(const char *fileName);
bool halNcpEndLogging(void);
bool halNcpOpenSemaphore(const char *name, bool create);
bool halNcpCloseSemaphore(void);

  #define SERIAL_PORT       SERIAL_PORT_CLI
  #define EZSP_INIT_TRIES   3

void spiPrintUsage(void)
{
  emberSerialPrintf(SERIAL_PORT, "usage: " EXE_NAME
                    " [--logfile <name>]"
                    " [--semaphore[-create] <name>]\r\n");
  emberSerialPrintf(SERIAL_PORT, "  example: " EXE_NAME
                    " --logfile spi.log"
                    " --semaphore-create /spi-semaphore\r\n");
}

const char *logfileName = EXE_NAME ".log";
const char *semaphoreName = NULL;
bool createSemaphore = false;

bool spiProcessCommandOptions(int argc, char* argv[])
{
  bool success = true;
  int i = 1;

  for (; success && i < argc && argv[i] != NULL; i++) {
    if (strcmp(argv[i], "--logfile") == 0 && i + 1 < argc) {
      i++;
      logfileName = argv[i];
    } else if (strcmp(argv[i], "--semaphore-create") == 0 && i + 1 < argc) {
      i++;
      semaphoreName = argv[i];
      createSemaphore = true;
    } else if (strcmp(argv[i], "--semaphore") == 0 && i + 1 < argc) {
      i++;
      semaphoreName = argv[i];
    } else {
      success = false;
    }
  }

  return success;
}

#else
  #define SERIAL_PORT       0
  #define EZSP_INIT_TRIES   1
#endif

//----------------------------------------------------------------
// Definitions

#define PROFILE_ID 0xC00F
#define ENDPOINT 1
#define CLUSTER_ID 0x12
#define PAN_ID 0x0216
#define TX_POWER 0
#define CHANNEL 19
#define USE_KEY_WHEN_JOINING false
#define BINDING_INDEX 0
#define MESSAGE_TAG 0x53
#define RADIUS 0
#define NONMEMBER_RADIUS 0
#define GROUP_ID 0x2233

#define elapsedTimeInt16u(oldTime, newTime) \
  ((uint16_t) ((uint16_t)(newTime) - (uint16_t)(oldTime)))

#define emberKeyContents(key) ((key)->contents)

//----------------------------------------------------------------
// Forward Declarations

static void run(void);
static void testSpi(void);
static void printCommandStatus(PGM_P commandName,
                               EmberStatus status);
static uint8_t getPayloadLength(void);
static void initializeApsFrame(EmberApsFrame *frame);

//----------------------------------------------------------------
// Global Variables

static uint8_t bufferOne[EZSP_MAX_FRAME_LENGTH];
static uint8_t bufferTwo[EZSP_MAX_FRAME_LENGTH];
static uint8_t bufferThree[255];
static uint16_t spiTestTotalCount = 0;
static uint8_t spiTestLength = 0;
static uint16_t spiTestInterval = 0;
static uint16_t spiTestCurrentCount = 0;
static uint16_t spiTestLastTime = 0;
static bool em260WokeUp = false;
static bool sendReply = false;
enum {
  NONE,
  MAX,
  MAX_PLUS_ONE
};
static uint8_t payloadType = NONE;
static bool delayOnRx = false;
static bool assertOnEzspError = true;

static void cleanupBeforeExit(void);

#define assertWithCleanup(condition) \
  if (!(condition)) {                \
    cleanupBeforeExit();             \
    assert(#condition == 0);         \
  } else {                           \
    (void) 0;                        \
  };

//----------------------------------------------------------------
// Buffers

uint8_t emPacketBufferCount = EMBER_PACKET_BUFFER_COUNT;
uint8_t emPacketBufferFreeCount = EMBER_PACKET_BUFFER_COUNT;
uint8_t emPacketBufferDataStorage[EMBER_PACKET_BUFFER_COUNT * 32];
uint8_t *emPacketBufferData = emPacketBufferDataStorage;
uint8_t emMessageBufferLengthsStorage[EMBER_PACKET_BUFFER_COUNT];
uint8_t *emMessageBufferLengths = emMessageBufferLengthsStorage;
uint8_t emMessageBufferReferenceCountsStorage[EMBER_PACKET_BUFFER_COUNT];
uint8_t *emMessageBufferReferenceCounts = emMessageBufferReferenceCountsStorage;
uint8_t emPacketBufferLinksStorage[EMBER_PACKET_BUFFER_COUNT];
uint8_t *emPacketBufferLinks = emPacketBufferLinksStorage;
uint8_t emPacketBufferQueueLinksStorage[EMBER_PACKET_BUFFER_COUNT];
uint8_t *emPacketBufferQueueLinks = emPacketBufferQueueLinksStorage;
void emRadioEnableAutoAck(bool enable)
{
}

//----------------------------------------------------------------
// Command Interpreter

static void getConfigCommand(EmberCommandState *state)
{
  EzspConfigId configId = emberUnsignedCommandArgument(state, 0);
  EzspStatus status;
  uint16_t value = 0xFFFF;
  status = ezspGetConfigurationValue(configId, &value);
  emberSerialPrintf(SERIAL_PORT,
                    "ID 0x%X: status 0x%X, value 0x%2X.\r\n",
                    configId, status, value);
}

static void setConfigCommand(EmberCommandState *state)
{
  EzspConfigId configId = emberUnsignedCommandArgument(state, 0);
  uint16_t value = emberUnsignedCommandArgument(state, 1);
  EzspStatus status;
  status = ezspSetConfigurationValue(configId, value);
  emberSerialPrintf(SERIAL_PORT,
                    "ID 0x%X, value 0x%2X: status 0x%X.\r\n",
                    configId, value, status);
}

static void nopCommand(EmberCommandState *state)
{
  ezspNop();
  emberSerialPrintf(SERIAL_PORT, "nop success.\r\n");
}

static void echoCommand(EmberCommandState *state)
{
  uint8_t i;
  uint8_t length = emberUnsignedCommandArgument(state, 0);
  uint8_t echoLength;
  MEMSET(bufferOne, 0xFF, EZSP_MAX_FRAME_LENGTH);
  MEMSET(bufferTwo, 0xFF, EZSP_MAX_FRAME_LENGTH);
  for (i = 0; i < length; i++) {
    bufferOne[i] = i;
  }
  echoLength = ezspEcho(length, bufferOne, bufferTwo);
  if (echoLength == length
      && MEMCOMPARE(bufferOne, bufferTwo, length) == 0) {
    emberSerialPrintf(SERIAL_PORT, "%d byte echo success.\r\n", length);
  } else {
    emberSerialPrintf(SERIAL_PORT, "%d byte echo failed.\r\n", length);
  }
}

static void scanCommand(EmberCommandState *state)
{
  EmberNetworkScanType type = emberUnsignedCommandArgument(state, 0);
  uint8_t duration = emberUnsignedCommandArgument(state, 1);
  EmberStatus status;
  status = emberStartScan(type,
                          EMBER_ALL_802_15_4_CHANNELS_MASK,
                          duration);
  printCommandStatus("emberStartScan", status);
}

static void formCommand(EmberCommandState *state)
{
  EmberNetworkParameters parameters;
  EmberStatus status;

  MEMSET(&parameters, 0, sizeof(EmberNetworkParameters));
  parameters.panId = PAN_ID;
  parameters.radioTxPower = TX_POWER;
  parameters.radioChannel = CHANNEL;

  status = emberFormNetwork(&parameters);
  printCommandStatus("emberFormNetwork", status);
  status = emberPermitJoining(0xFF);
  printCommandStatus("emberPermitJoining", status);
  assertWithCleanup(emberSetRadioPower(TX_POWER) == EMBER_SUCCESS);
}

static void joinCommand(EmberCommandState *state)
{
  EmberNetworkParameters parameters;
  EmberStatus status;
  EmberNodeType type;

  type = (state->command[4] == 0 ? EMBER_ROUTER : EMBER_SLEEPY_END_DEVICE);
  MEMSET(&parameters, 0, sizeof(EmberNetworkParameters));
  parameters.panId = PAN_ID;
  parameters.radioTxPower = TX_POWER;
  parameters.radioChannel = CHANNEL;

  status = emberJoinNetwork(type,
                            &parameters);
  printCommandStatus("emberJoinNetwork", status);
}

static void pollCommand(EmberCommandState *state)
{
  EmberStatus status = ezspPollForData(0, EMBER_EVENT_INACTIVE, 0);
  printCommandStatus("ezspPollForData", status);
}

static void unicastCommand(EmberCommandState *state)
{
  EmberNodeId destination = emberUnsignedCommandArgument(state, 0);
  uint8_t message[] = { 'u', 'n', 'i', 'c', 'a', 's', 't' };
  EmberApsFrame frame;
  EmberStatus status;
  uint8_t length = getPayloadLength();
  uint8_t i;
  for (i = 0; i < length; i++) {
    bufferOne[i] = message[i % sizeof(message)];
  }
  initializeApsFrame(&frame);
  frame.options = EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY
                  | EMBER_APS_OPTION_RETRY;

  status = ezspSendUnicast(EMBER_OUTGOING_DIRECT,
                           destination,
                           &frame,
                           MESSAGE_TAG,
                           length,
                           bufferOne,
                           &frame.sequence);
  printCommandStatus("ezspSendUnicast", status);
}

static void fragmentCommand(EmberCommandState *state)
{
  EmberNodeId destination = emberUnsignedCommandArgument(state, 0);
  uint8_t length = emberUnsignedCommandArgument(state, 1);
  uint8_t fragmentSize = emberUnsignedCommandArgument(state, 2);
  EmberApsFrame frame;
  EmberStatus status;
  uint8_t i;
  for (i = 0; i < length; i++) {
    bufferThree[i] = i;
  }
  initializeApsFrame(&frame);
  frame.options = EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY
                  | EMBER_APS_OPTION_RETRY;

  status = ezspFragmentSendUnicast(EMBER_OUTGOING_DIRECT,
                                   destination,
                                   &frame,
                                   fragmentSize,
                                   length,
                                   bufferThree);
  printCommandStatus("ezspFragmentSendUnicast", status);
}

static void broadcastCommand(EmberCommandState *state)
{
  uint8_t message[] = { 'b', 'r', 'o', 'a', 'd', 'c', 'a', 's', 't' };
  EmberApsFrame frame;
  EmberStatus status;
  uint8_t length = getPayloadLength();
  uint8_t i;
  for (i = 0; i < length; i++) {
    bufferOne[i] = message[i % sizeof(message)];
  }
  initializeApsFrame(&frame);
  frame.options = EMBER_APS_OPTION_NONE;

  status = ezspSendBroadcast(EMBER_BROADCAST_ADDRESS,
                             &frame,
                             RADIUS,
                             MESSAGE_TAG,
                             length,
                             bufferOne,
                             &frame.sequence);
  printCommandStatus("ezspSendBroadcast", status);
}

static void multicastCommand(EmberCommandState *state)
{
  EmberMulticastTableEntry tableEntry = { GROUP_ID, ENDPOINT };
  uint8_t message[] = { 'm', 'u', 'l', 't', 'i', 'c', 'a', 's', 't' };
  EmberApsFrame frame;
  EmberStatus status;
  uint8_t length = getPayloadLength();
  uint8_t i;
  if (length > 1) {
    // Maximum length for multicasts is 1 byte smaller than unicasts or
    // broadcasts.
    length--;
  }
  for (i = 0; i < length; i++) {
    bufferOne[i] = message[i % sizeof(message)];
  }
  initializeApsFrame(&frame);
  frame.options = EMBER_APS_OPTION_NONE;
  frame.groupId = GROUP_ID;

  status = ezspSetMulticastTableEntry(0, &tableEntry);
  printCommandStatus("ezspSetMulticastTableEntry", status);
  status = ezspSendMulticast(&frame,
                             RADIUS,
                             NONMEMBER_RADIUS,
                             MESSAGE_TAG,
                             length,
                             bufferOne,
                             &frame.sequence);
  printCommandStatus("ezspSendMulticast", status);
}

static void replyCommand(EmberCommandState *state)
{
  EzspDecisionId decision;
  sendReply = emberUnsignedCommandArgument(state, 0);
  decision = sendReply
             ? EZSP_HOST_WILL_SUPPLY_REPLY
             : EZSP_HOST_WILL_NOT_SUPPLY_REPLY;
  assertWithCleanup(ezspSetPolicy(EZSP_UNICAST_REPLIES_POLICY,
                                  decision) == EZSP_SUCCESS);
  emberSerialPrintf(SERIAL_PORT,
                    "Host will%p supply reply.\r\n",
                    sendReply ? "" : " not");
}

static void payloadCommand(EmberCommandState *state)
{
  payloadType = emberUnsignedCommandArgument(state, 0);
  emberSerialPrintf(SERIAL_PORT, "Payload type set to %u.\r\n", payloadType);
}

static void testCommand(EmberCommandState *state)
{
  if (spiTestTotalCount == 0) {
    spiTestTotalCount = emberUnsignedCommandArgument(state, 0);
    spiTestLength = emberUnsignedCommandArgument(state, 1);
    spiTestInterval = emberUnsignedCommandArgument(state, 2);
    spiTestCurrentCount = 0;
  } else {
    emberSerialPrintf(SERIAL_PORT, "Running test %d of %d.\r\n",
                      spiTestCurrentCount, spiTestTotalCount);
  }
}

static PGM_NO_CONST PGM_P sleepModeNames[] = {
  "idle", "deep sleep", "power down", "reserved"
};

static void sleepCommand(EmberCommandState *state)
{
  uint8_t newSleepMode = emberUnsignedCommandArgument(state, 0);
  if (newSleepMode == 4) {
    emberSerialPrintf(SERIAL_PORT, "Sleeping the host...");
    emberSerialWaitSend(SERIAL_PORT);
    halPowerDown();
    halSleep(SLEEPMODE_POWERDOWN);
    halPowerUp();
    emberSerialPrintf(SERIAL_PORT, " Awake!\r\n");
    emberSerialWaitSend(SERIAL_PORT);
  } else {
    emberSerialPrintf(SERIAL_PORT, "Changed sleep mode from %p to %p.\r\n",
                      sleepModeNames[ezspSleepMode],
                      sleepModeNames[newSleepMode]);
    ezspSleepMode = newSleepMode;
  }
}

static void wakeCommand(EmberCommandState *state)
{
  ezspWakeUp();
}

static void randomCommand(EmberCommandState *state)
{
  uint16_t random;
  ezspGetRandomNumber(&random);
  emberSerialPrintf(SERIAL_PORT, "%u\r\n", random);
}

static void debugCommand(EmberCommandState *state)
{
  uint8_t message[] = { 'I', 'n', 'S', 'i', 'g', 'h', 't' };
  bool binaryMessage = emberUnsignedCommandArgument(state, 0);
  ezspDebugWrite(binaryMessage, sizeof(message), message);
}

static void setKeyCommand(EmberCommandState *state)
{
  EmberInitialSecurityState security;
  EmberStatus status;
  EmberKeyData securityKey =
  { { 'E', 'M', '2', '6', '0', '-', 'E', 'Z', 'S', 'P', '-', 'E', 'M', '2', '6', '0' } };
  EmberKeyStruct checkKey;

  // EMBER_HAVE_PRECONFIGURED_KEY is always set on the TC regardless of whether
  // the Trust Center is expecting the device to have a preconfigured key.
  // This is the value for the Trust Center Link Key.
  // For the joining device, this will be the pre-configured key used
  // to join the network.
  security.bitmask = (EMBER_HAVE_PRECONFIGURED_KEY
                      | EMBER_STANDARD_SECURITY_MODE
                      | EMBER_TRUST_CENTER_GLOBAL_LINK_KEY
                      | EMBER_HAVE_NETWORK_KEY);
  security.networkKeySequenceNumber = 1;

  // For now, use the same key as the link and Network Key
  MEMCOPY(emberKeyContents(&(security.preconfiguredKey)),
          emberKeyContents(&(securityKey)),
          EMBER_ENCRYPTION_KEY_SIZE);
  MEMCOPY(emberKeyContents(&(security.networkKey)),
          emberKeyContents(&(securityKey)),
          EMBER_ENCRYPTION_KEY_SIZE);

  status = emberSetInitialSecurityState(&security);
  if (status != EMBER_SUCCESS) {
    printCommandStatus("emberSetInitialSecurityState", status);
  }
  status = emberGetKey(EMBER_CURRENT_NETWORK_KEY, &checkKey);
  if (status != EMBER_SUCCESS) {
    printCommandStatus("emberGetKey", status);
  }
  if (MEMCOMPARE(emberKeyContents(&(securityKey)),
                 emberKeyContents(&(checkKey.key)),
                 EMBER_ENCRYPTION_KEY_SIZE) == 0) {
    printCommandStatus("emberSetInitialSecurityState", status);
  } else {
    emberSerialPrintf(SERIAL_PORT, "Error reading back network key.\r\n");
  }
}

static void resetCommand(EmberCommandState *state)
{
  halReboot();
}

static void initCommand(EmberCommandState *state)
{
  EmberStatus status;
  status = emberNetworkInit(NULL);
  printCommandStatus("emberNetworkInit", status);
}

static void moveCommand(EmberCommandState *state)
{
  EmberStatus status;
  status = emberFindAndRejoinNetwork(true, 0);  // Assume we have the current NWK key
  printCommandStatus("emberFindAndRejoinNetwork", status);
}

static void timerCommand(EmberCommandState *state)
{
  uint8_t id = emberUnsignedCommandArgument(state, 0);
  uint16_t time = emberUnsignedCommandArgument(state, 1);
  EmberEventUnits units = emberUnsignedCommandArgument(state, 2);
  bool repeat = emberUnsignedCommandArgument(state, 3);
  EmberStatus status;
  status = ezspSetTimer(id, time, units, repeat);
  printCommandStatus("ezspSetTimer", status);
}

static void zdoCommand(EmberCommandState *state)
{
  uint8_t action = emberUnsignedCommandArgument(state, 0);
  if (action == 0) {
    uint16_t inputList[3] = { 1, 2, 0xAABB };
    uint16_t outputList[1] = { 0x1122 };
    EzspStatus status;
    status = ezspAddEndpoint(ENDPOINT,
                             PROFILE_ID,
                             0xBABA,
                             0x55,
                             3,
                             1,
                             inputList,
                             outputList);
    printCommandStatus("ezspAddEndpoint", status);
  } else {
    EmberStatus status;
    EmberNodeId target = emberUnsignedCommandArgument(state, 1);
    uint8_t targetEndpoint = emberUnsignedCommandArgument(state, 2);
    if (action == 1) {
      status = emberIeeeAddressRequest(target,
                                       true,
                                       0,
                                       EMBER_APS_OPTION_NONE);
      printCommandStatus("emberIeeeAddressRequest", status);
    } else {
      status = emberSimpleDescriptorRequest(target,
                                            targetEndpoint,
                                            EMBER_APS_OPTION_NONE);
      printCommandStatus("emberSimpleDescriptorRequest", status);
    }
  }
}

static void delayOnRxCommand(EmberCommandState *state)
{
  delayOnRx = emberUnsignedCommandArgument(state, 0);
  emberSerialPrintf(SERIAL_PORT,
                    "Host will%p delay on RX.\r\n",
                    delayOnRx ? "" : " not");
}

static void countersCommand(EmberCommandState *state)
{
  uint16_t values[EMBER_COUNTER_TYPE_COUNT];
  uint8_t i;
  for (i = 0; i < EMBER_COUNTER_TYPE_COUNT; i++) {
    values[i] = 0xEEEE;
  }
  ezspReadAndClearCounters(values);
  emberSerialPrintf(SERIAL_PORT, "Counters: ");
  for (i = 0; i < EMBER_COUNTER_TYPE_COUNT; i++) {
    emberSerialPrintf(SERIAL_PORT, "%2X ", values[i]);
  }
  emberSerialPrintf(SERIAL_PORT, ".\r\n");
}

static void mtorrCommand(EmberCommandState *state)
{
  EmberStatus status;
  status = emberSendManyToOneRouteRequest(EMBER_LOW_RAM_CONCENTRATOR, 0);
  printCommandStatus("emberSendManyToOneRouteRequest", status);
}

static void errorCommand(EmberCommandState *state)
{
  assertOnEzspError = emberUnsignedCommandArgument(state, 0);
  emberSerialPrintf(SERIAL_PORT, "Host will%p assert on EZSP error.\r\n",
                    assertOnEzspError ? "" : " not");
}

static void mfglibModeCommand(EmberCommandState *state)
{
  bool on = emberUnsignedCommandArgument(state, 0);
  if (on) {
    printCommandStatus("ezspMfglibStart", ezspMfglibStart(true));
  } else {
    printCommandStatus("ezspMfglibEnd", mfglibEnd());
  }
}

static void mfglibSetChannelCommand(EmberCommandState *state)
{
  printCommandStatus("mfglibSetChannel", mfglibSetChannel(CHANNEL));
}

static void setExtendedSecurityBitmaskCommand(EmberCommandState *state)
{
  uint16_t mask = emberUnsignedCommandArgument(state, 0);

  EmberStatus status = emberSetExtendedSecurityBitmask(mask);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT,
                      "Host set Extended Security Bitmask to 0x%2X.\r\n", mask);
  } else {
    emberSerialPrintf(SERIAL_PORT,
                      "Value not set, invalid call.\r\n");
  }
}

static void getExtendedSecurityBitmaskCommand(EmberCommandState *state)
{
  uint16_t mask;

  EmberStatus status = emberGetExtendedSecurityBitmask(&mask);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT,
                      "Host got Extended Security Bitmask, 0x%2X.\r\n", mask);
  } else {
    emberSerialPrintf(SERIAL_PORT,
                      "Value not available.\r\n");
  }
}

static void setShortAddressCommand(EmberCommandState *state)
{
  uint16_t nodeId = emberUnsignedCommandArgument(state, 0);

  EmberStatus status = emberSetNodeId(nodeId);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT,
                      "Host set short ID, 0x%2X.\r\n", nodeId);
  } else {
    emberSerialPrintf(SERIAL_PORT,
                      "ID not set, invalid call.\r\n");
  }
}

static void getShortAddressCommand(EmberCommandState *state)
{
  EmberNodeId nodeId = emberGetNodeId();

  emberSerialPrintf(SERIAL_PORT,
                    "Host got node short ID, 0x%2X.\r\n", nodeId);
}

static void setNetworkCommand(EmberCommandState *state)
{
  uint8_t nwkIndex = emberUnsignedCommandArgument(state, 0);

  EmberStatus status = emberSetCurrentNetwork(nwkIndex);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT,
                      "Host set network index success\r\n");
  } else {
    emberSerialPrintf(SERIAL_PORT, "Host set network index failed\r\n");
  }
}

static void getXNcpInfoCommand(EmberCommandState *state)
{
  EmberStatus status;
  uint16_t manufacturerId;
  uint16_t versionNumber;

  status  = ezspGetXncpInfo(&manufacturerId, &versionNumber);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT,
                      "XNCP library present, manufacturer ID:0x%2x version:0x%2x\r\n",
                      manufacturerId,
                      versionNumber);
  } else {
    emberSerialPrintf(SERIAL_PORT, "XNCP library not present\r\n");
  }
}

static void setConcentratorParametersCommand(EmberCommandState *state)
{
  bool on = (emberUnsignedCommandArgument(state, 0) == 1);
  uint16_t concentratorType = emberUnsignedCommandArgument(state, 1);
  uint16_t minTime = emberUnsignedCommandArgument(state, 2);
  uint16_t maxTime = emberUnsignedCommandArgument(state, 3);
  uint8_t rtErrThreshold = emberUnsignedCommandArgument(state, 4);
  uint8_t delFailThreshold = emberUnsignedCommandArgument(state, 5);
  uint8_t maxHops = emberUnsignedCommandArgument(state, 6);

  EmberStatus status = ezspSetConcentrator(on,
                                           concentratorType,
                                           minTime,
                                           maxTime,
                                           rtErrThreshold,
                                           delFailThreshold,
                                           maxHops);

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT, "Concentrator params set\r\n");
  }
}

//static void getEui64Command(EmberCommandState *state)
//{
//  EmberEUI64 eui64 = {0};
//
//  ezspGetEui64(eui64);
//
//  //if (status != EMBER_SUCCESS)
//  //{
//  //  emberSerialPrintf(SERIAL_PORT, "get_eui64: Get EUI64 failed with error 0x%X\r\n", status);
//  //}
//  //else
//  //{
//    emberSerialPrintf(SERIAL_PORT, "get_eui64: EUI64 ");
//    printEui64(SERIAL_PORT, eui64);
//    emberSerialPrintf(SERIAL_PORT, "\r\n");
//  //}
//}

static void networkStateCommand(EmberCommandState *state)
{
  emberSerialPrintf(SERIAL_PORT, "net_state: Network state 0x%X\r\n", ezspNetworkState());
}

static void networkParamsCommand(EmberCommandState *state)
{
  EmberNodeType nodeType;
  EmberNetworkParameters params;

  EmberStatus status = ezspGetNetworkParameters(&nodeType, &params);

  if (status != EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT, "net_param: Get network params failed with error 0x%X\r\n", status);
  } else {
    int i;

    emberSerialPrintf(SERIAL_PORT, "net_param: Node type 0x%X\r\n", nodeType);
    emberSerialPrintf(SERIAL_PORT, "net_param: Extended PAN ID 0x");
    for (i = 0; i < 8; i++) {
      emberSerialPrintf(SERIAL_PORT, "%X", params.extendedPanId[i]);
    }
    emberSerialPrintf(SERIAL_PORT, "\r\n");
    emberSerialPrintf(SERIAL_PORT, "net_param: PAN ID 0x%2X\r\n", params.panId);
    emberSerialPrintf(SERIAL_PORT, "net_param: Radio TX power %d\r\n", params.radioTxPower);
    emberSerialPrintf(SERIAL_PORT, "net_param: Radio channel %d\r\n", params.radioChannel);
    emberSerialPrintf(SERIAL_PORT, "net_param: Join method 0x%X\r\n", params.joinMethod);
    emberSerialPrintf(SERIAL_PORT, "net_param: Network manager ID 0x%2X\r\n", params.nwkManagerId);
    emberSerialPrintf(SERIAL_PORT, "net_param: Network update ID 0x%X\r\n", params.nwkUpdateId);
    emberSerialPrintf(SERIAL_PORT, "net_param: Channel mask 0x%4X\r\n", params.channels);
  }
}

static void networkLeaveCommand(EmberCommandState *state)
{
  EmberStatus status = ezspLeaveNetwork();

  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT, "net_leave: Left network\r\n");
  } else {
    emberSerialPrintf(SERIAL_PORT, "net_leave: Error 0x%X\r\n", status);
  }
}

#if defined(UNIX_HOST)
#include <stdlib.h> // for exit

static void cleanupBeforeExit(void)
{
  emberSerialCleanup();

#ifdef UNIX_SPI_HOST
  if (semaphoreName != NULL) {
    if (!halNcpCloseSemaphore()) {
      perror("Could not close semaphore");
    }
  }

  if (!halNcpEndLogging()) {
    perror("Could not close log file");
  }
#endif
}

static void exitCommand(EmberCommandState *state)
{
  cleanupBeforeExit();
  emberSerialPrintf(SERIAL_PORT, "Exiting\r\n");
  exit(0);
}
#else
void cleanupBeforeExit(void)
{
}
#endif

// declare help command here, define it after commands array (as it uses it)
static void helpCommand(EmberCommandState *state);

static EmberCommandEntry commands[] = {
  { "get", getConfigCommand, "u1" },
  // <ID>
  { "set", setConfigCommand, "u1u2" },
  // <ID> <value>
  { "nop", nopCommand, "", "Send an EZSP NOP frame to the NCP" },
  { "echo", echoCommand, "u1", "(<number of bytes>) Send an EZSP ECHO frame to the NCP and check that the response matches" },
  // <length>
  { "scan", scanCommand, "u1u1" },
  // <type> <duration>
  { "form", formCommand, "" },
  { "join", joinCommand, "" },
  { "join_sleepy", joinCommand, "" },
  { "poll", pollCommand, "" },
  { "unicast", unicastCommand, "u2" },
  // <node ID>
  { "fragment", fragmentCommand, "u2u1u1" },
  // <node ID> <length> <fragment size>
  { "broadcast", broadcastCommand, "" },
  { "multicast", multicastCommand, "" },
  { "reply", replyCommand, "u1" },
  // <reply?>
  { "payload", payloadCommand, "u1" },
  // <0 = zero, 1 = max, 2 = max + 1>
  { "test", testCommand, "u2u1u2", "(<total tests> <message length> <test interval in ms>) Test connection to EZSP-SPI NCP" },
  // <total tests> <message length> <test interval in ms>
  { "sleep", sleepCommand, "u1" },
  // <sleep mode>
  { "wake", wakeCommand, "" },
  { "rand", randomCommand, "" },
  { "debug", debugCommand, "u1" },
  // <binary?>
  { "setkey", setKeyCommand, "" },
  { "reset", resetCommand, "" },
  { "init", initCommand, "" },
  { "move", moveCommand, "" },
  { "timer", timerCommand, "u1u2u1u1" },
  // <id> <time> <units> <repeat>
  { "zdo", zdoCommand, "u1u2u1" },
  // <action> <target> <target endpoint>
  { "delay_on_rx", delayOnRxCommand, "u1" },
  // <delay?>
  { "counters", countersCommand, "" },
  { "mtorr", mtorrCommand, "" },
  // <node ID>
  { "error", errorCommand, "u1" },
  // <assert on ezsp error?>
  { "mfglib", mfglibModeCommand, "u1" },
  // 0 = mfglibMode off, 1 = mfglibMode on
  { "mfgSetChan", mfglibSetChannelCommand, "" },
  // set channel to use by mfglib

  // Sets the extended security bitmask to value v:u2
  { "set_bmask", setExtendedSecurityBitmaskCommand, "u2" },
  // Prints the extended security bitmask
  { "get_bmask", getExtendedSecurityBitmaskCommand, "" },

  // Set the node short ID
  { "set_addr", setShortAddressCommand, "u2" },

  // Get the node short ID
  { "get_addr", getShortAddressCommand, "" },

  // Set the application network index
  { "set_network", setNetworkCommand, "u1" },

  // Set the application network index
  { "xncp_info", getXNcpInfoCommand, "" },

  // <on/off> <concentratorType> <minTime> <maxTime> <routeErrorThreshold>
  // <deliveryFailureThreshold> <maxHops>
  { "concentrator", setConcentratorParametersCommand, "u1u2u2u2u1u1u1" },

  //{ "get_eui64", getEui64Command, "", "Get NCP's EUI"},

  { "net_state", networkStateCommand, "", "Get current network state" },
  { "net_param", networkParamsCommand, "", "Get current network parameters" },
  { "net_leave", networkLeaveCommand, "", "Leave current network" },

#if defined(UNIX_HOST)
  { "exit", exitCommand, "", "Exit the program" },
  { "quit", exitCommand, "", "Exit the program (synonym for exit)" },
  { "done", exitCommand, "", "Exit the program (synonym for exit)" },
#endif

  // help
  { "help", helpCommand, "", "Print out command and usage information" },

  { NULL, NULL, NULL, NULL }
};

static void helpCommand(EmberCommandState *state)
{
  int i;

  emberSerialPrintf(SERIAL_PORT, "\r\n");
  emberSerialPrintf(SERIAL_PORT, "Available commands (name [arguments] - description):\r\n");

  for (i = 0;
       commands[i].longName != NULL
       && commands[i].action != NULL
       && commands[i].argumentTypes != NULL;
       i++) {
    emberSerialPrintf(SERIAL_PORT,
                      "  %s [%s] - %s\r\n",
                      commands[i].longName,
                      commands[i].argumentTypes,
                      (commands[i].description == NULL) ? "<no description>" : commands[i].description);
  }

  emberSerialPrintf(SERIAL_PORT, "\r\n");
  emberSerialPrintf(SERIAL_PORT, "Arguments types:\r\n");
  emberSerialPrintf(SERIAL_PORT, "  b:   buffer\r\n");
  emberSerialPrintf(SERIAL_PORT, "  u1:  one-byte unsigned\r\n");
  emberSerialPrintf(SERIAL_PORT, "  u2:  two-byte unsigned\r\n");
  emberSerialPrintf(SERIAL_PORT, "  u4:  four-byte unsigned\r\n");
  emberSerialPrintf(SERIAL_PORT, "  s1:  one-byte signed\r\n");
  emberSerialPrintf(SERIAL_PORT, "  s2:  two-byte signed\r\n");
  emberSerialPrintf(SERIAL_PORT, "  *:   zero or more of the previous type.\r\n");
  emberSerialPrintf(SERIAL_PORT, "       If used, this must be the last specifier.\r\n");
  emberSerialPrintf(SERIAL_PORT, "  Buffer arguments are given as a string \"...\". There is currently no\r\n");
  emberSerialPrintf(SERIAL_PORT, "  way to include quote characters \' or \" in strings arguments. Buffer\r\n");
  emberSerialPrintf(SERIAL_PORT, "  arguments can also be given as a hex digit string using curly braces as\r\n");
  emberSerialPrintf(SERIAL_PORT, "  in {1A2B3C}. The hex digits are converted to bytes in pairs.\r\n");
  emberSerialPrintf(SERIAL_PORT, "  Integer arguments can be either decimal or hexidecimal. Decimal is the\r\n");
  emberSerialPrintf(SERIAL_PORT, "  default and a 0x prefix indicates a hexidecimal integer.\r\n");

  emberSerialPrintf(SERIAL_PORT, "\r\n");
  emberSerialPrintf(SERIAL_PORT, "Examples:\r\n");
  emberSerialPrintf(SERIAL_PORT, "  help - Print this help dialog\r\n");
  emberSerialPrintf(SERIAL_PORT, "  nop - Send an EZSP NOP frame to the NCP\r\n");
  emberSerialPrintf(SERIAL_PORT, "  echo 81 - Send an EZSP ECHO frame with 81 bytes of data to the NCP\r\n");
  emberSerialPrintf(SERIAL_PORT, "  echo 0x81 - Send an EZSP ECHO frame with 129 bytes (maximum value) of data to the NCP\r\n");
  emberSerialPrintf(SERIAL_PORT, "  test 10 0x10 100 - Test EZSP-SPI to the NCP by sending 10 frames of 16 bytes each 100 ms apart\r\n");
  emberSerialPrintf(SERIAL_PORT, "  get_addr - Get NCP's ZigBee short ID\r\n");
  emberSerialPrintf(SERIAL_PORT, "  set_addr 0xFFFD - Set NCP's ZigBee short ID to 0xFFFD\r\n");
  emberSerialPrintf(SERIAL_PORT, "  exit - Exit the program\r\n");

  emberSerialPrintf(SERIAL_PORT, "\r\n");
}

static void commandErrorHandler(EmberCommandState *state)
{
  emberSerialPrintf(SERIAL_PORT, "Error: %p\r\n",
                    emberCommandErrorNames[state->error]);
  emberSerialPrintf(SERIAL_PORT, "Type help for a list of commands\r\n");
}

#define MAX_COMMAND_ARGUMENTS 8
static uint16_t arguments[MAX_COMMAND_ARGUMENTS];
#define INTEGER_BASE 10

static EmberCommandState commandState = {
  commands,
  commandErrorHandler,
  arguments,
  MAX_COMMAND_ARGUMENTS,
  INTEGER_BASE
};

//----------------------------------------------------------------
// Initialization and Main Loop

int main(MAIN_FUNCTION_PARAMETERS)
{
  uint8_t stackType;
  uint16_t stackVersion;

#ifdef UNIX_SPI_HOST
  // check for command line arguments, may want to eventually move to getopt
  if (!spiProcessCommandOptions(argc, argv)) {
    spiPrintUsage();
    return 2;
  }

  if (!halNcpEnableLogging(logfileName)) {
    fprintf(stderr, "Log file \"%s\" could not be created or opened: %s\n", logfileName, strerror(errno));
    exit(1);
  }

  if (semaphoreName != NULL) {
    if (!halNcpOpenSemaphore(semaphoreName, createSemaphore)) {
      fprintf(stderr, "Semaphore \"%s\" could not be created or opened: %s\n", semaphoreName, strerror(errno));
      exit(1);
    }
  }
#endif

  halInit();
  INTERRUPTS_ON();

  #ifdef UNIX_SPI_HOST
  emberSerialSetPrompt(APP_NAME);
  #endif

  emberSerialInit(SERIAL_PORT, BAUD_19200, PARITY_NONE, 1);
  emberSerialPrintf(SERIAL_PORT, APP_NAME " Application\r\n");
  emberCommandReaderInit(&commandState);
#if (!defined(EMBER_TEST)) && (defined(EZSP_ASH))
  if (!ashProcessCommandOptions(argc, argv)) {
    return 2;
  }
  if (ezspInit() != EZSP_SUCCESS) {
    fprintf(stderr, "Failed to connect.\n");
    ashPrintUsage(argv[0]);
    return 1;
  }
#else

  int tries;
  EzspStatus initResult = ezspInit();

  for (tries = 1; initResult != EZSP_SUCCESS && tries < EZSP_INIT_TRIES; tries++) {
    emberSerialPrintf(SERIAL_PORT, "ezspInit failed, retrying\r\n");
    initResult = ezspInit();
  }

  assertWithCleanup(initResult == EZSP_SUCCESS);
#endif

  assertWithCleanup(ezspVersion(EZSP_PROTOCOL_VERSION, &stackType, &stackVersion)
                    == EZSP_PROTOCOL_VERSION);

  emberSetCurrentNetwork(0);
  assertWithCleanup(ezspSetConfigurationValue(EZSP_CONFIG_SECURITY_LEVEL,
                                              0) == EZSP_SUCCESS);
  // We set the security level for both networks with index 0 and 1. If the
  // NCP does not support multi-network, this second call will be redundant.
  emberSetCurrentNetwork(1);
  assertWithCleanup(ezspSetConfigurationValue(EZSP_CONFIG_SECURITY_LEVEL,
                                              0) == EZSP_SUCCESS);
  assertWithCleanup(ezspSetPolicy(EZSP_TRUST_CENTER_POLICY,
                                  EZSP_ALLOW_JOINS) == EZSP_SUCCESS);
  assertWithCleanup(ezspSetConfigurationValue(EZSP_CONFIG_FRAGMENT_WINDOW_SIZE,
                                              1) == EZSP_SUCCESS);
  assertWithCleanup(ezspSetConfigurationValue(EZSP_CONFIG_FRAGMENT_DELAY_MS,
                                              50) == EZSP_SUCCESS);
  ezspFragmentInit(255, bufferThree);
  emberSerialPrintf(SERIAL_PORT, "NCP Initialized\r\n");
  run();

  return 0;
}

static void run(void)
{
  while (true) {
    halResetWatchdog();
    emberProcessCommandInput(&commandState, SERIAL_PORT);
    simulatedTimePasses();
    ezspTick();
    if (ezspCallbackPending()) {
      ezspCallback();
    }
    if (spiTestTotalCount > 0) {
      testSpi();
    }
    if (em260WokeUp) {
      emberSerialPrintf(SERIAL_PORT, "EM260 is awake.\r\n");
      em260WokeUp = false;
    }
    ezspFragmentTick();
  }
}

//----------------------------------------------------------------
// SPI test code

static void testSpi(void)
{
  uint8_t i;
  EzspStatus status;
  uint16_t now = halCommonGetInt16uMillisecondTick();
  if (elapsedTimeInt16u(spiTestLastTime, now) < spiTestInterval) {
    return;
  }
  spiTestLastTime = now;
  serialSetCommandByte(EZSP_FRAME_CONTROL_INDEX, (EZSP_FRAME_CONTROL_COMMAND
                                                  | ezspSleepMode));
  serialSetCommandByte(EZSP_FRAME_ID_INDEX, EZSP_NOP);
  for (i = 0; i < spiTestLength; i++) {
    serialSetCommandByte(EZSP_PARAMETERS_INDEX + i, i);
  }
  serialSetCommandLength(EZSP_PARAMETERS_INDEX + spiTestLength);
  serialSendCommand();
  status = serialResponseReceived();
  while (status == EZSP_SPI_WAITING_FOR_RESPONSE
         || status == EZSP_NO_RX_DATA) {
    status = serialResponseReceived();
  }
  if (status == EZSP_SUCCESS) {
    uint8_t errorCount = 0;
    if (serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX)
        != EZSP_FRAME_CONTROL_RESPONSE) {
      emberSerialPrintf(SERIAL_PORT, "\r\ntest #%d: Control 0x%X.\r\n",
                        spiTestCurrentCount,
                        serialGetResponseByte(EZSP_FRAME_CONTROL_INDEX));
    }
    if (serialGetResponseByte(EZSP_FRAME_ID_INDEX) != EZSP_NOP) {
      emberSerialPrintf(SERIAL_PORT, "\r\ntest #%d: ID 0x%X.\r\n",
                        spiTestCurrentCount,
                        serialGetResponseByte(EZSP_FRAME_ID_INDEX));
    }
    for (i = 0; i < spiTestLength; i++) {
      if (serialGetResponseByte(EZSP_PARAMETERS_INDEX + i) != i) {
        errorCount++;
      }
    }
    if (errorCount > 0) {
      emberSerialPrintf(SERIAL_PORT, "\r\ntest #%d: %d errors.\r\n",
                        spiTestCurrentCount,
                        errorCount);
    }
  } else {
    emberSerialPrintf(SERIAL_PORT, "\r\ntest #%d: Status 0x%X.\r\n",
                      spiTestCurrentCount,
                      status);
  }
  spiTestCurrentCount++;
  if (spiTestCurrentCount == spiTestTotalCount) {
    emberSerialPrintf(SERIAL_PORT, "\r\nCompleted %d tests\r\n",
                      spiTestCurrentCount);
    spiTestTotalCount = 0;
  }
}

//----------------------------------------------------------------
// Utility

static void printCommandStatus(PGM_P commandName,
                               EmberStatus status)
{
  if (status == EMBER_SUCCESS) {
    emberSerialPrintf(SERIAL_PORT, "%p success.\r\n", commandName);
  } else {
    emberSerialPrintf(SERIAL_PORT, "Error: %p status 0x%X.\r\n",
                      commandName, status);
  }
}

static uint8_t getPayloadLength(void)
{
  switch (payloadType) {
    case NONE:
      return 0;
    case MAX:
      return ezspMaximumPayloadLength();
    case MAX_PLUS_ONE:
      return ezspMaximumPayloadLength() + 1;
  }
  return 0;
}

static void initializeApsFrame(EmberApsFrame *frame)
{
  MEMSET(frame, 0, sizeof(EmberApsFrame));
  frame->profileId = PROFILE_ID;
  frame->clusterId = CLUSTER_ID;
  frame->sourceEndpoint = ENDPOINT;
  frame->destinationEndpoint = ENDPOINT;
}

//----------------------------------------------------------------
// Callbacks

void halNcpIsAwakeIsr(bool isAwake)
{
  if (isAwake) {
    em260WokeUp = true;
  } else {
    em260WokeUp = false;
  }
}

void ezspErrorHandler(EzspStatus status)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspErrorHandler: status 0x%X.\r\n",
                    status);
  if (assertOnEzspError) {
    assertWithCleanup(false);
  }
}

void ezspStackStatusHandler(EmberStatus status)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspStackStatusHandler: status 0x%X.\r\n",
                    status);
}

void ezspNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                             uint8_t lastHopLqi,
                             int8_t lastHopRssi)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspNetworkFoundHandler: channel 0x%X, panId 0x%2X.\r\n",
                    networkFound->channel, networkFound->panId);
}

void ezspEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspEnergyScanResultHandler: channel 0x%X, RSSI %d.\r\n",
                    channel, maxRssiValue);
}

void ezspScanCompleteHandler(uint8_t channel,
                             EmberStatus status)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspScanCompleteHandler: channel 0x%X, status 0x%X.\r\n",
                    channel, status);
}

void ezspUnusedPanIdFoundHandler(EmberPanId panId, uint8_t channel)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspUnusedPanIdFoundHandler: panId 0x%2x, channel %d.\r\n",
                    panId, channel);
}

void ezspMessageSentHandler(EmberOutgoingMessageType type,
                            uint16_t indexOrDestination,
                            EmberApsFrame *apsFrame,
                            uint8_t messageTag,
                            EmberStatus status,
                            uint8_t messageLength,
                            uint8_t *messageContents)
{
  if (ezspFragmentMessageSent(apsFrame, status)) {
    return;
  }
  emberSerialPrintf(SERIAL_PORT,
                    "ezspMessageSentHandler: status 0x%X.\r\n",
                    status);
}

void ezspFragmentMessageSentHandler(EmberStatus status)
{
  emberSerialPrintf(SERIAL_PORT,
                    "ezspFragmentMessageSentHandler: status 0x%X.\r\n",
                    status);
}

void ezspIncomingMessageHandler(EmberIncomingMessageType type,
                                EmberApsFrame *apsFrame,
                                uint8_t lastHopLqi,
                                int8_t lastHopRssi,
                                EmberNodeId sender,
                                uint8_t bindingIndex,
                                uint8_t addressIndex,
                                uint8_t messageLength,
                                uint8_t *messageContents)
{
  uint16_t length = messageLength;

  if (ezspFragmentIncomingMessage(apsFrame,
                                  sender,
                                  &length,
                                  &messageContents)) {
    return;
  }
  emberSerialPrintf(SERIAL_PORT,
                    "ezspIncomingMessageHandler: sender 0x%2X.\r\n",
                    sender);
  if (apsFrame->profileId == EMBER_ZDO_PROFILE_ID) {
    if (apsFrame->clusterId == NETWORK_ADDRESS_RESPONSE
        || apsFrame->clusterId == IEEE_ADDRESS_RESPONSE) {
      uint8_t i;
      EmberEUI64 eui64;
      EmberNodeId nodeId = ezspDecodeAddressResponse(messageContents,
                                                     eui64);
      emberSerialPrintf(SERIAL_PORT,
                        "address response: 0x%2X = ",
                        nodeId);
      for (i = 0; i < EUI64_SIZE; i++) {
        emberSerialPrintf(SERIAL_PORT, "%X", eui64[i]);
      }
      emberSerialPrintf(SERIAL_PORT, ".\r\n");
    } else if (apsFrame->clusterId == SIMPLE_DESCRIPTOR_RESPONSE) {
      uint8_t *finger = messageContents + 11;
      uint8_t inputCount;
      uint8_t outputCount;
      uint8_t i;
      inputCount = *finger;
      finger++;
      emberSerialPrintf(SERIAL_PORT, "%u in: ", inputCount);
      for (i = 0; i < inputCount; i++) {
        emberSerialPrintf(SERIAL_PORT, "%2X ", HIGH_LOW_TO_INT(finger[1], finger[0]));
        finger += 2;
      }
      outputCount = *finger;
      finger++;
      emberSerialPrintf(SERIAL_PORT, ", %u out: ", outputCount);
      for (i = 0; i < outputCount; i++) {
        emberSerialPrintf(SERIAL_PORT, "%2X ", HIGH_LOW_TO_INT(finger[1], finger[0]));
        finger += 2;
      }
      emberSerialPrintf(SERIAL_PORT, ".\r\n");
    }
  }
  if (sendReply) {
    uint8_t message[] = { 'r', 'e', 'p', 'l', 'y' };
    EmberStatus status;
    uint8_t length = getPayloadLength();
    uint8_t i;

    for (i = 0; i < length; i++) {
      bufferOne[i] = message[i % sizeof(message)];
    }
    uint8_t relayCount;
    if (emberFindSourceRoute(sender, &relayCount, (uint16_t *)bufferThree)) {
      status = ezspSetSourceRoute(sender, relayCount, (uint16_t *)bufferThree);
      printCommandStatus("ezspSetSourceRoute", status);
    }
    status = ezspSendReply(sender, apsFrame, length, bufferOne);
    printCommandStatus("ezspSendReply", status);
  }
  if (apsFrame->options & EMBER_APS_OPTION_FRAGMENT) {
    uint16_t i;
    bool fail = false;
    for (i = 0; i < length; i++) {
      if (messageContents[i] != i) {
        fail = true;
      }
    }
    emberSerialPrintf(SERIAL_PORT, "fragmented: length %d, contents %p.\r\n",
                      length, fail ? "ERROR" : "ok");
  }
  if (delayOnRx) {
    ezspNop();
  }
}

void ezspMfglibRxHandler(uint8_t linkQuality,
                         int8_t rssi,
                         uint8_t packetLength,
                         uint8_t *packetContents)
{
  // Replace the first byte of the CRC with a nul so that we can print
  // this as a string.
  packetContents[packetLength - 2] = 0;
  emberSerialPrintf(SERIAL_PORT,
                    "ezspMfglibRxHandler: bytes 0x%X \"%s\".\r\n",
                    packetLength,
                    packetContents + 1);// Drop the first byte because the
                                        // PHY may set a bit in it.
}

void ezspWaitingForResponse(void)
{
  uint16_t start = halCommonGetInt16uMillisecondTick();
  uint16_t now;
  do {
    simulatedTimePasses();
    now = halCommonGetInt16uMillisecondTick();
  } while (delayOnRx && elapsedTimeInt16u(start, now) < 20);
}
