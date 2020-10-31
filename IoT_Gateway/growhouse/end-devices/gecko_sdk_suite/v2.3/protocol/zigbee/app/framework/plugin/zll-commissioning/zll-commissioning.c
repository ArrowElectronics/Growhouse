// Copyright 2016 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/common.h"
#include "zll-commissioning.h"

// AppBuilder already prevents multi-network ZLL configurations.  This is here
// as a reminder that the code below assumes that there is exactly one network
// and that it is ZigBee PRO.
#if EMBER_SUPPORTED_NETWORKS != 1
  #error ZLL is not supported with multiple networks.
#endif

// ZigBee 3.0 test harness hooks.
#ifdef EMBER_AF_PLUGIN_TEST_HARNESS_Z3
  #include "app/framework/plugin/test-harness-z3/test-harness-z3-zll.h"
#endif

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT
  #include EMBER_AF_API_NETWORK_CREATOR
  #include "app/framework/util/util.h" // emAfExtendedPanId
#endif

//------------------------------------------------------------------------------
// Globals

// The bits for cluster-specific command (0) and disable default response (4)
// are always set.  The direction bit (3) is only set for server-to-client
// commands (i.e., DeviceInformationResponse).  Some Philips devices still use
// the old frame format and set the frame control to zero.
#define ZLL_FRAME_CONTROL_LEGACY           0x00
#define ZLL_FRAME_CONTROL_CLIENT_TO_SERVER 0x11
#define ZLL_FRAME_CONTROL_SERVER_TO_CLIENT 0x19

#define ZLL_HEADER_FRAME_CONTROL_OFFSET   0 // one byte
#define ZLL_HEADER_SEQUENCE_NUMBER_OFFSET 1 // one byte
#define ZLL_HEADER_COMMAND_ID_OFFSET      2 // one byte
#define ZLL_HEADER_TRANSACTION_ID_OFFSET  3 // four bytes
#define ZLL_HEADER_OVERHEAD               7

#define ZLL_DEVICE_INFORMATION_RECORD_SIZE 16

#define isFactoryNew(state)         ((state) & EMBER_ZLL_STATE_FACTORY_NEW)
#define isRequestingPriority(state) ((state) & EMBER_ZLL_STATE_LINK_PRIORITY_REQUEST)

EmberEventControl emberAfPluginZllCommissioningTouchLinkEventControl;
static EmberZllNetwork network;
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static EmberZllDeviceInfoRecord subDevices[EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SUB_DEVICE_TABLE_SIZE];
static uint8_t subDeviceCount = 0;
static uint8_t channel;
static int8_t rssi;
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

enum {
  INITIAL                     = 0x0000,
  SCAN_FOR_TOUCH_LINK         = 0x0001,
  SCAN_FOR_DEVICE_INFORMATION = 0x0002,
  SCAN_FOR_IDENTIFY           = 0x0004,
  SCAN_FOR_RESET              = 0x0008,
  TARGET_NETWORK_FOUND        = 0x0010,
  ABORTING_TOUCH_LINK         = 0x0020,
  SCAN_COMPLETE               = 0x0040,
  TOUCH_LINK_TARGET           = 0x0080,
  FORMING_NETWORK             = 0x0100,
  RESETTING_TO_FACTORY_NEW    = 0x0200,
};
static uint16_t flags = INITIAL;
#define touchLinkInProgress()      (flags                            \
                                    & (SCAN_FOR_TOUCH_LINK           \
                                       | SCAN_FOR_DEVICE_INFORMATION \
                                       | SCAN_FOR_IDENTIFY           \
                                       | SCAN_FOR_RESET              \
                                       | TOUCH_LINK_TARGET))
#define scanForTouchLink()         (flags & SCAN_FOR_TOUCH_LINK)
#define scanForDeviceInformation() (flags & SCAN_FOR_DEVICE_INFORMATION)
#define scanForIdentify()          (flags & SCAN_FOR_IDENTIFY)
#define scanForReset()             (flags & SCAN_FOR_RESET)
#define targetNetworkFound()       (flags & TARGET_NETWORK_FOUND)
#define abortingTouchLink()        (flags & ABORTING_TOUCH_LINK)
#define scanComplete()             (flags & SCAN_COMPLETE)
#define touchLinkTarget()          (flags & TOUCH_LINK_TARGET)
#define formingNetwork()           (flags & FORMING_NETWORK)
#define resettingToFactoryNew()    (flags & RESETTING_TO_FACTORY_NEW)

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CENTRALIZED_NETWORK_SUPPORT
  #define CENTRALIZED_NETWORK_SUPPORT (true)
#else
  #define CENTRALIZED_NETWORK_SUPPORT (false)
#endif

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
uint16_t emAfZllIdentifyDurationS = EMBER_AF_PLUGIN_ZLL_COMMISSIONING_IDENTIFY_DURATION;
static const bool centralizedNetworkSupport = CENTRALIZED_NETWORK_SUPPORT;
#endif
uint8_t emAfZllExtendedPanId[EXTENDED_PAN_ID_SIZE] = EMBER_AF_PLUGIN_ZLL_COMMISSIONING_EXTENDED_PAN_ID;

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_STEALING_ALLOWED
  #define STEALING_ALLOWED (true)
#else
  #define STEALING_ALLOWED (false)
#endif
static const boolean stealingAllowed = STEALING_ALLOWED;

// Radio power modes. (from phy.h)
enum {
  EMBER_RADIO_POWER_MODE_RX_ON,
  EMBER_RADIO_POWER_MODE_ED_ON = EMBER_RADIO_POWER_MODE_RX_ON,
  EMBER_RADIO_POWER_MODE_OFF
};

PGM uint8_t emAfZllCommissioningPluginName[] = "ZLL Commissioning";
#define PLUGIN_NAME emAfZllCommissioningPluginName

#define PLUGIN_DEBUG
#if defined(PLUGIN_DEBUG)
  #define debugPrintln(...) emberAfCorePrintln(__VA_ARGS__)
  #define debugPrint(...)   emberAfCorePrint(__VA_ARGS__)
  #define debugExec(x) do { x; } while (0)
#else
  #define debugPrintln(...)
  #define debugPrint(...)
  #define debugExec(x)
#endif

// Limit this delay for scripted tests, to prevent the transaction timing out,
// especially for a host side test.
#ifdef EMBER_TEST
  #undef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_MILLISECONDS_DELAY
  #define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_MILLISECONDS_DELAY 2500
#endif

//------------------------------------------------------------------------------
// Forward Declarations

static void touchLinkComplete(void);
static void abortTouchLink(EmberAfZllCommissioningStatus reason);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static EmberStatus sendDeviceInformationRequest(uint8_t startIndex);
static EmberStatus sendIdentifyRequest(uint16_t identifyDuration);
static EmberStatus sendResetToFactoryNewRequest(void);
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static void deviceInformationRequestHandler(const EmberEUI64 source,
                                            uint32_t transaction,
                                            uint8_t startIndex);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static void deviceInformationResponseHandler(const EmberEUI64 source,
                                             uint32_t transaction,
                                             uint8_t numberOfSubDevices,
                                             uint8_t startIndex,
                                             uint8_t deviceInformationRecordCount,
                                             uint8_t *deviceInformationRecordList);
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static void identifyRequestHandler(const EmberEUI64 source,
                                   uint32_t transaction,
                                   uint16_t identifyDurationS);
static void resetToFactoryNewRequestHandler(const EmberEUI64 source,
                                            uint32_t transaction);
static bool amFactoryNew(void);
static bool isZllDecisionId(EmberZllPolicy decisionId);
static EmberNodeType getLogicalNodeType(void);
static void completeResetToFactoryNew(void);
static void setProfileInteropState(void);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static EmberStatus setRadioChannel(uint8_t newChannel);
static uint32_t getChannelMask(uint8_t purpose);
static int8_t targetCompare(const EmberZllNetwork *t1,
                            int8_t r1,
                            const EmberZllNetwork *t2,
                            int8_t r2);
static bool isSameNetwork(const EmberZllNetwork *network);
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RX_ALWAYS_ON
  #define RX_ON_PERIOD_MS 0xFFFFFFFFUL
#else
  #define RX_ON_PERIOD_MS (EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RX_ON_AT_STARTUP_PERIOD * 1000UL)
#endif

//------------------------------------------------------------------------------

static void initFactoryNew(void)
{
  // The initiation is only performed if we are factory new, i.e. not joined to
  // a centralized or distributed network.
  if (amFactoryNew()) {
    emberAfAppPrintln("ZllCommInit - device is factory new and not joined");

    // Turn the receiver on for a user-specified period, in order to allow incoming
    // scan requests.
    emberZllSetRxOnWhenIdle(RX_ON_PERIOD_MS);

    // Tune the radio to the user-specified default channel - this will be used
    // to listen for an incoming scan request.
    emberSetLogicalAndRadioChannel(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_DEFAULT_RADIO_CHANNEL);

    // Set the default ZLL node type, in case a factory new router is asked to form
    // its own network, in response to a Network Start request from an end device.
    emberSetZllNodeType((emAfCurrentZigbeeProNetwork->nodeType
                         == EMBER_COORDINATOR)
                        ? EMBER_ROUTER
                        : emAfCurrentZigbeeProNetwork->nodeType);

    // Set the default power for a new network.
    emberSetRadioPower(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RADIO_TX_POWER);

    // Set the security state here, in case of incoming scan requests, to ensure
    // that our security key bitmask matches that of the initiator.
    emberAfZllSetInitialSecurityState();

    // Set the address assignment capability bit to true if required
    // (i.e. if the initiator checkbox is selected in the plugin settings.)
    emberSetZllAdditionalState(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ADDITIONAL_STATE);

    // Set interop bit here in all cases.
    setProfileInteropState();
  }
}

void emberAfPluginZllCommissioningInitCallback(void)
{
#ifndef EZSP_HOST
  emberZllSetPolicy(stealingAllowed ? EMBER_ZLL_POLICY_ENABLED : EMBER_ZLL_POLICY_NO_TOUCHLINK_FOR_NFN);

  // Set the primary and secondary channel masks in all cases.
  emberSetZllPrimaryChannelMask(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_PRIMARY_CHANNEL_MASK);
  emberSetZllSecondaryChannelMask(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SECONDARY_CHANNEL_MASK);

  // Factory new initialization
  initFactoryNew();
#endif
}

void emberAfPluginZllCommissioningNcpInitCallback(bool memoryAllocation)
{
#ifdef EZSP_HOST
  if (!memoryAllocation) {
    emberAfSetEzspPolicy(EZSP_ZLL_POLICY,
                         stealingAllowed ? EMBER_ZLL_POLICY_ENABLED : EMBER_ZLL_POLICY_NO_TOUCHLINK_FOR_NFN,
                         "ZLL policy",
                         "enable");

    // Set the primary and secondary channel masks in all cases.
    emberSetZllPrimaryChannelMask(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_PRIMARY_CHANNEL_MASK);
    emberSetZllSecondaryChannelMask(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SECONDARY_CHANNEL_MASK);

    // Factory new initialization
    initFactoryNew();
  }
#endif
}

bool emberAfPluginInterpanPreMessageReceivedCallback(const EmberAfInterpanHeader *header,
                                                     uint8_t msgLen,
                                                     uint8_t *message)
{
  uint32_t transaction;
  uint8_t frameControl, commandId, msgIndex;

  // If the message isn't for the ZLL Commissioning cluster, drop it with an
  // indication that we didn't handle it.  After this, everything else will be
  // considered handled so that the message doesn't end up going through the
  // normal ZCL processing.
  if (header->profileId != EMBER_ZLL_PROFILE_ID
      || header->clusterId != ZCL_ZLL_COMMISSIONING_CLUSTER_ID) {
    return false;
  }

  if (header->messageType != EMBER_AF_INTER_PAN_UNICAST
      || !(header->options & EMBER_AF_INTERPAN_OPTION_MAC_HAS_LONG_ADDRESS)
      || msgLen < ZLL_HEADER_OVERHEAD) {
    return true;
  }

  // Verify that the frame control byte makes sense.  Accept only the legacy
  // format or simple client-to-server or server-to-client messages (i.e., no
  // manufacturer-specific commands, etc.)  For non-legacy messages, check that
  // the frame control is correct for the command.  The check is based on
  // DeviceInformationResponse because it is the only server-to-client command
  // we care about.
  frameControl = message[ZLL_HEADER_FRAME_CONTROL_OFFSET];
  commandId = message[ZLL_HEADER_COMMAND_ID_OFFSET];
  if (frameControl != ZLL_FRAME_CONTROL_LEGACY
      && (frameControl != ZLL_FRAME_CONTROL_CLIENT_TO_SERVER
          || commandId == ZCL_DEVICE_INFORMATION_RESPONSE_COMMAND_ID)
      && (frameControl != ZLL_FRAME_CONTROL_SERVER_TO_CLIENT
          || commandId != ZCL_DEVICE_INFORMATION_RESPONSE_COMMAND_ID)) {
    return true;
  }

  msgIndex = ZLL_HEADER_TRANSACTION_ID_OFFSET;
  transaction = emberAfGetInt32u(message, msgIndex, msgLen);
  msgIndex += 4;

  switch (commandId) {
    case ZCL_DEVICE_INFORMATION_REQUEST_COMMAND_ID:
      if (msgIndex + 1 <= msgLen) {
        uint8_t startIndex = emberAfGetInt8u(message, msgIndex, msgLen);
        deviceInformationRequestHandler(header->longAddress,
                                        transaction,
                                        startIndex);
      }
      break;
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
    case ZCL_DEVICE_INFORMATION_RESPONSE_COMMAND_ID:
      if (msgIndex + 3 <= msgLen) {
        uint8_t numberOfSubDevices, startIndex, deviceInformationRecordCount;
        numberOfSubDevices = emberAfGetInt8u(message, msgIndex, msgLen);
        msgIndex++;
        startIndex = emberAfGetInt8u(message, msgIndex, msgLen);
        msgIndex++;
        deviceInformationRecordCount = emberAfGetInt8u(message, msgIndex, msgLen);
        msgIndex++;
        if ((msgIndex
             + deviceInformationRecordCount * ZLL_DEVICE_INFORMATION_RECORD_SIZE)
            <= msgLen) {
          uint8_t *deviceInformationRecordList = message + msgIndex;
          deviceInformationResponseHandler(header->longAddress,
                                           transaction,
                                           numberOfSubDevices,
                                           startIndex,
                                           deviceInformationRecordCount,
                                           deviceInformationRecordList);
        }
      }
      break;
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
    case ZCL_IDENTIFY_REQUEST_COMMAND_ID:
      if (msgIndex + 2 <= msgLen) {
        uint16_t identifyDurationS = emberAfGetInt16u(message, msgIndex, msgLen);
        identifyRequestHandler(header->longAddress,
                               transaction,
                               identifyDurationS);
      }
      break;
    case ZCL_RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID:
      // Ignore reset request if we are a co-ordinator
      if (getLogicalNodeType() != EMBER_COORDINATOR) {
        resetToFactoryNewRequestHandler(header->longAddress, transaction);
      }
      break;
  }

  // All ZLL Commissioning cluster messages are considered handled, even if we
  // ended up dropping them because they were malformed, etc.
  return true;
}

void emberAfPluginZllCommissioningStackStatusCallback(EmberStatus status)
{
  // If we are forming a network for a router initiator, then we handle
  // this status separately.
  // During touch linking, EMBER_NETWORK_UP means the process is complete.  Any
  // other status, unless we're busy joining or rejoining, means that the touch
  // link failed.

  debugPrintln("ZllCommStackStatus: status = %X, flags = %X", status, flags);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT
  if (formingNetwork()) {
    if (status == EMBER_NETWORK_UP) {
      // Make ourselves not factory new, and reset the ZLL address and group space
      // we are using node id 0x0001 ourselves.
      EmberTokTypeStackZllData token;
      emberZllGetTokenStackZllData(&token);
      token.bitmask &= ~EMBER_ZLL_STATE_FACTORY_NEW;
      token.freeNodeIdMin = EMBER_ZLL_MIN_NODE_ID + 1;  // start with 0x0002
      token.freeNodeIdMax = EMBER_ZLL_MAX_NODE_ID;
      token.myGroupIdMin = (EMBER_ZLL_GROUP_ADDRESSES == 0
                            ? EMBER_ZLL_NULL_GROUP_ID
                            : EMBER_ZLL_MIN_GROUP_ID);
      token.freeGroupIdMin = EMBER_ZLL_MIN_GROUP_ID + EMBER_ZLL_GROUP_ADDRESSES;
      token.freeGroupIdMax = EMBER_ZLL_MAX_GROUP_ID;
      emberZllSetTokenStackZllData(&token);

      // Save the new channel.
      channel = emberGetLogicalChannel();

      // Switch back to the target's current channel to send the network request,
      // in case we formed on a different one.
      setRadioChannel(network.zigbeeNetwork.channel);

      // Set the target's new channel to our current channel.
      network.zigbeeNetwork.channel = channel;

      debugPrintln("Zll Comm stack status: our chan = %d, target chan = %d", channel, network.zigbeeNetwork.channel);

      // Kick the touchlinking event.
      emberEventControlSetActive(emberAfPluginZllCommissioningTouchLinkEventControl);
    } else {
      emberAfAppPrintln("%p%p%p0x%x",
                        "Error: ",
                        "Touch linking failed: ",
                        "could not form network: ",
                        status);
      abortTouchLink(EMBER_AF_ZLL_NETWORK_FORMATION_FAILED);
    }
    flags &= ~FORMING_NETWORK;
  } else
#endif /* EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT */

  if (touchLinkInProgress()) {
    if (status == EMBER_NETWORK_UP) {
      touchLinkComplete();
    } else if (status == EMBER_NETWORK_DOWN) {
      // We don't do anything here for a network down.
    } else {
      emberAfAppPrintln("%p%p%p: status = %X, flags = %X",
                        "Error: ",
                        "Touch linking failed: ",
                        "joining failed",
                        status, flags);
      abortTouchLink(EMBER_AF_ZLL_JOINING_FAILED);
    }
  } else if (resettingToFactoryNew()) {
    if (status == EMBER_NETWORK_DOWN) {
      completeResetToFactoryNew();
    }
  } else {
    emAfZllStackStatus(status);
  }
}

static void touchLinkComplete(void)
{
  EmberNodeType nodeType;
  EmberNetworkParameters parameters;
  flags = INITIAL;
  emberAfGetNetworkParameters(&nodeType, &parameters);
  network.zigbeeNetwork.channel = parameters.radioChannel;
  network.zigbeeNetwork.panId = parameters.panId;
  MEMMOVE(network.zigbeeNetwork.extendedPanId,
          parameters.extendedPanId,
          EXTENDED_PAN_ID_SIZE);
  network.zigbeeNetwork.nwkUpdateId = parameters.nwkUpdateId;
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  emberAfPluginZllCommissioningTouchLinkCompleteCallback(&network,
                                                         subDeviceCount,
                                                         (subDeviceCount == 0
                                                          ? NULL
                                                          : subDevices));
#else
  emberAfPluginZllCommissioningTouchLinkCompleteCallback(&network, 0, NULL);
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
}

EmberStatus emberAfZllSetInitialSecurityState(void)
{
  EmberKeyData networkKey;
  EmberZllInitialSecurityState securityState = {
    0, // bitmask - unused
    EMBER_ZLL_KEY_INDEX_CERTIFICATION,
    EMBER_ZLL_CERTIFICATION_ENCRYPTION_KEY,
    EMBER_ZLL_CERTIFICATION_PRECONFIGURED_LINK_KEY,
  };
  EmberStatus status;

  // We can only initialize security information while not on a network - this
  // also covers the case where we are joined as a coordinator.
  if (emberAfNetworkState() != EMBER_NO_NETWORK) {
    return EMBER_SUCCESS;
  }

  status = emberAfGenerateRandomKey(&networkKey);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("%p%p failed 0x%x",
                      "Error: ",
                      "Generating random key",
                      status);
    return status;
  }

  emberAfPluginZllCommissioningInitialSecurityStateCallback(&securityState);
  status = emberZllSetInitialSecurityState(&networkKey, &securityState);

  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("%p%p failed 0x%x",
                      "Error: ",
                      "Initializing security",
                      status);
  }
  return status;
}

static EmberNodeType getLogicalNodeType(void)
{
  EmberNodeType nodeType;
  EmberStatus status = emberAfGetNodeType(&nodeType);

  // Note, we only report as a coordinator if we are a currently
  // coordinator on a centralized network.
  if (status == EMBER_NOT_JOINED) {
    nodeType = emAfCurrentZigbeeProNetwork->nodeType;
    if (nodeType == EMBER_COORDINATOR) {
      nodeType = EMBER_ROUTER;
    }
  }
  return nodeType;
}

EmberStatus emAfZllFormNetwork(uint8_t channel, int8_t power, EmberPanId panId)
{
  // Create a distributed commissioning network using the ZLL link key.
  EmberZllNetwork network;
  MEMSET(&network, 0, sizeof(EmberZllNetwork));
  network.zigbeeNetwork.channel = channel;
  network.zigbeeNetwork.panId = panId;
  emberAfGetFormAndJoinExtendedPanIdCallback(network.zigbeeNetwork.extendedPanId);
  EmberTokTypeStackZllData token;
  emberZllGetTokenStackZllData(&token);
  network.state = token.bitmask & 0xffff;
  network.nodeType = getLogicalNodeType();
  emberAfZllSetInitialSecurityState();
  EmberStatus status = emberZllFormNetwork(&network, power);
  debugPrintln("%p: emberZllFormNetwork - status = %X, node type = %d", PLUGIN_NAME, status, network.nodeType);
  return status;
}

static EmberStatus startScan(uint8_t purpose)
{
  EmberStatus status = EMBER_INVALID_CALL;
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  if (touchLinkInProgress()) {
    emberAfAppPrintln("%pTouch linking in progress", "Error: ");
  } else {
    emberAfZllSetInitialSecurityState();
    channel = emberGetLogicalChannel();
    debugPrintln("startScan: purpose = %X, flags = %2X, channel = %d", purpose, flags, channel);
    status = emberZllStartScan(getChannelMask(purpose),
                               EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_POWER_LEVEL,
                               getLogicalNodeType());
    if (status == EMBER_SUCCESS) {
      flags = purpose;
    } else {
      emberAfAppPrintln("%p%p%p0x%x",
                        "Error: ",
                        "Touch linking failed: ",
                        "could not start scan: ",
                        status);
    }
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  return status;
}

EmberStatus emberAfZllInitiateTouchLink(void)
{
  return startScan(SCAN_FOR_TOUCH_LINK);
}

EmberStatus emberAfZllDeviceInformationRequest(void)
{
  return startScan(SCAN_FOR_DEVICE_INFORMATION);
}

EmberStatus emberAfZllIdentifyRequest(void)
{
  return startScan(SCAN_FOR_IDENTIFY);
}

EmberStatus emberAfZllResetToFactoryNewRequest(void)
{
  return startScan(SCAN_FOR_RESET);
}

void emberAfZllNoTouchlinkForNFN(void)
{
#ifndef EZSP_HOST
  emberZllSetPolicy(EMBER_ZLL_POLICY_NO_TOUCHLINK_FOR_NFN);
#else
  emberAfSetEzspPolicy(EZSP_ZLL_POLICY,
                       EMBER_ZLL_POLICY_NO_TOUCHLINK_FOR_NFN,
                       "ZLL policy with no touchlink for NFN",
                       "enable");
#endif
}

void emberAfZllDisable(void)
{
#ifndef EZSP_HOST
  emberZllSetPolicy(EMBER_ZLL_POLICY_DISABLED);
#else
  emberAfSetEzspPolicy(EZSP_ZLL_POLICY,
                       EMBER_ZLL_POLICY_DISABLED,
                       "ZLL processing",
                       "disable");
#endif
}

void emberAfZllEnable(void)
{
#ifndef EZSP_HOST
  emberZllSetPolicy(EMBER_ZLL_POLICY_ENABLED);
#else
  emberAfSetEzspPolicy(EZSP_ZLL_POLICY,
                       EMBER_ZLL_POLICY_ENABLED,
                       "ZLL processing",
                       "enable");
#endif
}

void emberAfZllAbortTouchLink(void)
{
  if (touchLinkInProgress()) {
    // If the scanning portion of touch linking is already finished, we can
    // abort right away.  If not, we need to stop the scan and wait for the
    // stack to inform us when the scan is done.
    emberAfAppPrintln("%p%p%p",
                      "Error: ",
                      "Touch linking failed: ",
                      "aborted by application");
    if (scanComplete()) {
      abortTouchLink(EMBER_AF_ZLL_ABORTED_BY_APPLICATION);
    } else {
      flags |= ABORTING_TOUCH_LINK;
      emberStopScan();
    }
  }
}

static void abortTouchLink(EmberAfZllCommissioningStatus reason)
{
  flags = INITIAL;
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  if (emberEventControlGetActive(emberAfPluginZllCommissioningTouchLinkEventControl)) {
    emberEventControlSetInactive(emberAfPluginZllCommissioningTouchLinkEventControl);
    if (network.numberSubDevices != 1) {
      emberZllSetRxOnWhenIdle(0); // restore original idle mode
    }
    sendIdentifyRequest(0x0000); // exit identify mode
  }
  EmberStatus status = emberSetRadioChannel(channel);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("%p%p0x%x",
                      "Error: ",
                      "could not restore channel: ",
                      status);
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  emberAfPluginZllCommissioningTouchLinkFailedCallback(reason);
}

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT

static EmberStatus formNetworkForRouterInitiator(void)
{
  EmberStatus status;

  // Initialize ZLL security.
  status = emberAfZllSetInitialSecurityState();
  if (status != EMBER_SUCCESS) {
    return status;
  }

  // Form network with a random extended PAN ID. The emAfExtendedPanId value
  // is global to the framework and used in the network-creator plugin.
  // Setting the channel masks below will force the network-creator API to
  // form on that channel.
  MEMSET(emAfExtendedPanId, 0xFF, EXTENDED_PAN_ID_SIZE);

  // Per the BDB spec, we scan on the ZLL primary channels.
  emAfPluginNetworkCreatorPrimaryChannelMask = emberGetZllPrimaryChannelMask();
  emAfPluginNetworkCreatorSecondaryChannelMask = 0;

  // Set node id for emberFormNetwork, to override random id generation.
  emberSetNodeId(EMBER_ZLL_MIN_NODE_ID);  // our node id = 0x0001

  debugPrintln("formNetworkForRouterInitiator: chan mask = %4X", emAfPluginNetworkCreatorPrimaryChannelMask);

  status = emberAfPluginNetworkCreatorStart(false); // distributed network

  if (status == EMBER_SUCCESS) {
    flags |= FORMING_NETWORK;
  }

  return status;
}

#endif /* EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT */

bool emberAfZllTouchLinkInProgress(void)
{
  return touchLinkInProgress();
}

void emberAfZllResetToFactoryNew(void)
{
  // The leave will cause the ZLL state to be set to 'factory new',
  // but after a short delay.
  flags |= RESETTING_TO_FACTORY_NEW;
  EmberStatus status = emberLeaveNetwork();
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("Error: Failed to leave network, status: 0x%X",
                      status);
    completeResetToFactoryNew();
  }
}

static void completeResetToFactoryNew(void)
{
  emberAfResetAttributes(EMBER_BROADCAST_ENDPOINT);
  emberAfGroupsClusterClearGroupTableCallback(EMBER_BROADCAST_ENDPOINT);
  emberAfScenesClusterClearSceneTableCallback(EMBER_BROADCAST_ENDPOINT);
  initFactoryNew();
  emberAfPluginZllCommissioningResetToFactoryNewCallback();
  flags = INITIAL;
}

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static boolean isTouchlinkPermitted(const EmberZllNetwork *networkInfo)
{
  // Basic checks for a Z3.0 touchlink initiator.
  if (networkInfo->nodeType == EMBER_UNKNOWN_DEVICE) {
    // The target must be a known device type.
    debugPrintln("isTouchlinkPermitted: Error - Target is unknown node type %d", networkInfo->nodeType);
    return false;
  }

  EmberNodeType nodeType;
  EmberStatus status = emberAfGetNodeType(&nodeType);
  EmberCurrentSecurityState securityState;
  if (status == EMBER_NOT_JOINED) { // unknown node type: not joined, joining, or leaving
    debugPrintln("isTouchlinkPermitted: initiator is not joined");
  } else if (emberGetCurrentSecurityState(&securityState) != EMBER_SUCCESS) {
    // It is possible that we're orphaned but attempting to rejoin here,
    // but we'll fail the touchlink anyway, since it would probably collide with
    // the rejoining procedure.
    debugPrintln("emberGetCurrentSecurityState failure, status = %X", status);
    return false;
  } else {
    // We're either joined or orphaned and not attempting to rejoin; in
    // either case we can proceed with the next stage of validation.
    debugPrintln("isTouchlinkPermitted: node type = %d, security bitmask = %2X,flags = %2X",
                 nodeType, securityState.bitmask, flags);
  }

  if (scanForReset() || scanForTouchLink()) {
    // When scanning for touch linking or reset, the initiator need not be
    // on a network, but if it is, then it may not be on a different centralized
    // network, unless the (non-compliant) centralized network support is enabled.
    if (status != EMBER_NOT_JOINED
        && (securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE) == 0
        && !isSameNetwork(networkInfo)
        && !centralizedNetworkSupport) {
      debugPrintln("isTouchlinkPermitted: Error - Initiator on centralized network");
      return false;
    }
  }

  if (scanForTouchLink() && status == EMBER_NOT_JOINED) {
    // When scanning for touch linking, and if the initiator is not on a network,
    // either initiator or target must be able to form a network. If centralized
    // network touchlink initiating is supported, the initiating device must
    // be coordinator-capable.
    if ((getLogicalNodeType() != EMBER_ROUTER && networkInfo->nodeType != EMBER_ROUTER)
        || (centralizedNetworkSupport
            && emAfCurrentZigbeeProNetwork->nodeType != EMBER_COORDINATOR)) {
      debugPrintln("isTouchlinkPermitted: Error - Neither end can form a network");
      debugPrintln("our type = %d, their type = %d", getLogicalNodeType(), networkInfo->nodeType);
      return false;
    }
  }
  return true;
}
#endif

void ezspZllNetworkFoundHandler(EmberZllNetwork *networkInfo,
                                bool isDeviceInfoNull,
                                EmberZllDeviceInfoRecord *deviceInfo,
                                uint8_t lastHopLqi,
                                int8_t lastHopRssi)
{
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  debugPrintln("ezspZllNwkFound: node type = %d, flags = %0x2X", networkInfo->nodeType, flags);
  if (touchLinkInProgress()) {
    if (isTouchlinkPermitted(networkInfo)) {
      if (!targetNetworkFound()
          || 0 < targetCompare(networkInfo, lastHopRssi, &network, rssi)) {
        MEMMOVE(&network, networkInfo, sizeof(EmberZllNetwork));
        subDeviceCount = 0;
        if (!isDeviceInfoNull) {
          MEMMOVE(&subDevices[0], deviceInfo, sizeof(EmberZllDeviceInfoRecord));
          subDeviceCount++;
        }
        rssi = lastHopRssi;
        flags |= TARGET_NETWORK_FOUND;
      }
    } else {
      debugPrintln("ezspZllNwkFound: touchlink is not permitted");
    }
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
}

#ifndef EZSP_HOST
void emberZllNetworkFoundHandler(const EmberZllNetwork *networkInfo,
                                 const EmberZllDeviceInfoRecord *deviceInfo)
{
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  debugPrintln("emberZllNwkFound: chan = %d, node type = %d, flags = %02X", networkInfo->zigbeeNetwork.channel, networkInfo->nodeType, flags);
  if (touchLinkInProgress()) {
    if (isTouchlinkPermitted(networkInfo)) {
      int8_t lastHopRssi;
      emberGetLastHopRssi(&lastHopRssi);
      if (!targetNetworkFound()
          || 0 < targetCompare(networkInfo, lastHopRssi, &network, rssi)) {
        MEMMOVE(&network, networkInfo, sizeof(EmberZllNetwork));
        subDeviceCount = 0;
        if (deviceInfo != NULL) {
          MEMMOVE(&subDevices[0], deviceInfo, sizeof(EmberZllDeviceInfoRecord));
          subDeviceCount++;
        }
        rssi = lastHopRssi;
        flags |= TARGET_NETWORK_FOUND;
      }
    } else {
      debugPrintln("emberZllNwkFound: touchlink is not permitted");
    }
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

#ifdef EMBER_AF_PLUGIN_TEST_HARNESS_Z3
  emAfPluginTestHarnessZ3ZllNetworkFoundCallback(networkInfo);
#endif
}
#endif // EZSP_HOST

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
static void processScanComplete(EmberStatus scanStatus)
{
  EmberStatus status;
  if (touchLinkInProgress()) {
    flags |= SCAN_COMPLETE;
    if (scanStatus != EMBER_SUCCESS) {
      emberAfAppPrintln("%p%p0x%x",
                        "Error: ",
                        "Touch linking failed: ",
                        scanStatus);
      abortTouchLink(EMBER_AF_ZLL_PREEMPTED_BY_STACK);
    } else if (abortingTouchLink()) {
      debugPrintln("processScanComplete: aborting due to preemption");
      abortTouchLink(EMBER_AF_ZLL_ABORTED_BY_APPLICATION);
    } else if (targetNetworkFound()) {
      status = setRadioChannel(network.zigbeeNetwork.channel);
      if (status != EMBER_SUCCESS) {
        emberAfAppPrintln("%p%p%p0x%x",
                          "Error: ",
                          "Touch linking failed: ",
                          "could not change channel: ",
                          status);
        abortTouchLink(EMBER_AF_ZLL_CHANNEL_CHANGE_FAILED);
        return;
      }

      // When scanning for the purposes of touch linking or requesting device
      // information and the target has more than one sub device, turn the
      // receiver on (so we can actually receive the response) and send out the
      // first request.  If the target only has one sub device, its data will
      // have already been received in the ScanRequest.
      if ((scanForTouchLink() || scanForDeviceInformation())
          && network.numberSubDevices != 1) {
        debugPrintln("processScanComplete: sending device info request");
        emberZllSetRxOnWhenIdle(EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_MILLISECONDS_DELAY);
        status = sendDeviceInformationRequest(0x00); // start index
        if (status != EMBER_SUCCESS) {
          emberAfAppPrintln("%p%p%p0x%x",
                            "Error: ",
                            "Touch linking failed: ",
                            "could not send device information request: ",
                            status);
          abortTouchLink(EMBER_AF_ZLL_SENDING_DEVICE_INFORMATION_REQUEST_FAILED);
          return;
        }
      }

      status = sendIdentifyRequest(emAfZllIdentifyDurationS);
      if (status != EMBER_SUCCESS) {
        emberAfAppPrintln("%p%p%p0x%x",
                          "Error: ",
                          "Touch linking failed: ",
                          "could not send identify request: ",
                          status);
        abortTouchLink(EMBER_AF_ZLL_SENDING_IDENTIFY_REQUEST_FAILED);
        return;
      }
      emberEventControlSetDelayMS(emberAfPluginZllCommissioningTouchLinkEventControl,
                                  EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_MILLISECONDS_DELAY);
    } else {
      emberAfAppPrintln("%p%p%p",
                        "Error: ",
                        "Touch linking failed: ",
                        "no networks were found");
      abortTouchLink(EMBER_AF_ZLL_NO_NETWORKS_FOUND);
    }
  } else {
    debugPrintln("processScanComplete: no touchlink in progress");
  }
}
#endif

void ezspZllScanCompleteHandler(EmberStatus scanStatus)
{
  debugPrintln("%p: scan complete", PLUGIN_NAME);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  processScanComplete(scanStatus);
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
}

void emberZllScanCompleteHandler(EmberStatus scanStatus)
{
  debugPrintln("%p: scan complete", PLUGIN_NAME);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  processScanComplete(scanStatus);
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

#ifdef EMBER_AF_PLUGIN_TEST_HARNESS_Z3
  emAfPluginTestHarnessZ3ZllScanCompleteCallback(scanStatus);
#endif
}

void ezspZllAddressAssignmentHandler(EmberZllAddressAssignment *addressInfo,
                                     uint8_t lastHopLqi,
                                     int8_t lastHopRssi)
{
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  if (touchLinkInProgress()) {
    network.nodeId = addressInfo->nodeId;
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
}

void emberZllAddressAssignmentHandler(const EmberZllAddressAssignment *addressInfo)
{
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  if (touchLinkInProgress()) {
    debugPrintln("emberZllAddressAssignmentHandler: node id = %2X", addressInfo->nodeId);
    network.nodeId = addressInfo->nodeId;
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
}

void ezspZllTouchLinkTargetHandler(EmberZllNetwork *networkInfo)
{
  MEMMOVE(&network, networkInfo, sizeof(EmberZllNetwork));
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  subDeviceCount = 0;
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  flags = TOUCH_LINK_TARGET;
}

void emberZllTouchLinkTargetHandler(const EmberZllNetwork *networkInfo)
{
  MEMMOVE(&network, networkInfo, sizeof(EmberZllNetwork));
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  subDeviceCount = 0;
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  flags = TOUCH_LINK_TARGET;
}

void emberAfPluginZllCommissioningTouchLinkEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginZllCommissioningTouchLinkEventControl);
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  debugPrintln("TouchlinkEventHandler: flags = %X", flags);
  if (touchLinkInProgress()) {
    EmberStatus status;

    sendIdentifyRequest(0x0000); // exit identify mode

    if (scanForTouchLink()) {
      // If we are not factory new, we want to bring the target into our
      // existing network, so we set the channel to our own channel, either
      // our original channel, or the channel we formed on, if we are (were)
      // a factory-new router initiator.
      // Otherwise, we'll use whatever channel the target is on presently.
      if (!amFactoryNew()) {
        network.zigbeeNetwork.channel = channel;
      }
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT
      else if (emAfCurrentZigbeeProNetwork != NULL
               && emAfCurrentZigbeeProNetwork->nodeType <= EMBER_ROUTER) {
        // If we are factory new and a router, then we need to form a network
        // to become non-factory new. See the BDB spec's touchlink procedure
        // for an initiator, steps 13, 21 and 22.
        status = formNetworkForRouterInitiator();
        if (status != EMBER_SUCCESS) {
          emberAfAppPrintln("%p%p%p0x%x",
                            "Error: ",
                            "Touch linking failed: ",
                            "could not start network formation: ",
                            status);
          abortTouchLink(EMBER_AF_ZLL_NETWORK_FORMATION_FAILED);
        }
        return;
      }
#endif /* EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ZIGBEE3_SUPPORT */
      else {
        // If we are a factory new (end device) initiator, then let the
        // target chose the channel.
        network.zigbeeNetwork.channel = 0;
      }
      emberAfZllSetInitialSecurityState();
      status = emberZllJoinTarget(&network);
      if (status != EMBER_SUCCESS) {
        // Check that it's not a non-fatal status, for example, if
        // emberZllJoinTarget() is sending a network update.
        if (status != EMBER_OPERATION_IN_PROGRESS) {
          emberAfAppPrintln("%p%p%p0x%x",
                            "Error: ",
                            "Touch linking failed: ",
                            "could not send start/join: ",
                            status);
        }
        abortTouchLink(EMBER_AF_ZLL_SENDING_START_JOIN_FAILED);
        return;
      }
    } else {
      if (scanForReset()) {
        status = sendResetToFactoryNewRequest();
        if (status != EMBER_SUCCESS) {
          emberAfAppPrintln("%p%p%p0x%x",
                            "Error: ",
                            "Touch linking failed: ",
                            "could not send reset: ",
                            status);
          abortTouchLink(EMBER_AF_ZLL_SENDING_RESET_TO_FACTORY_NEW_REQUEST_FAILED);
          return;
        }
      }
      emberSetRadioChannel(channel);
      touchLinkComplete();
    }
  }
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
}

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

static EmberStatus sendDeviceInformationRequest(uint8_t startIndex)
{
  EmberStatus status;
  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                             | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                             | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                            ZCL_ZLL_COMMISSIONING_CLUSTER_ID,
                            ZCL_DEVICE_INFORMATION_REQUEST_COMMAND_ID,
                            "wu",
                            network.securityAlgorithm.transactionId,
                            startIndex);
  status = emberAfSendCommandInterPan(0xFFFF,                // destination pan id
                                      network.eui64,
                                      EMBER_NULL_NODE_ID,    // node id - ignored
                                      0x0000,                // group id - ignored
                                      EMBER_ZLL_PROFILE_ID);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("%p%p failed 0x%x",
                      "Error: ",
                      "Device information request",
                      status);
  }
  return status;
}

static EmberStatus sendIdentifyRequest(uint16_t identifyDuration)
{
  EmberStatus status;
  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                             | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                             | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                            ZCL_ZLL_COMMISSIONING_CLUSTER_ID,
                            ZCL_IDENTIFY_REQUEST_COMMAND_ID,
                            "wv",
                            network.securityAlgorithm.transactionId,
                            identifyDuration);
  status = emberAfSendCommandInterPan(0xFFFF,                // destination pan id
                                      network.eui64,
                                      EMBER_NULL_NODE_ID,    // node id - ignored
                                      0x0000,                // group id - ignored
                                      EMBER_ZLL_PROFILE_ID);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("%p%p failed 0x%x",
                      "Error: ",
                      "Identify request",
                      status);
  }
  return status;
}

static EmberStatus sendResetToFactoryNewRequest(void)
{
  EmberStatus status;
  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                             | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                             | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                            ZCL_ZLL_COMMISSIONING_CLUSTER_ID,
                            ZCL_RESET_TO_FACTORY_NEW_REQUEST_COMMAND_ID,
                            "w",
                            network.securityAlgorithm.transactionId);
  status = emberAfSendCommandInterPan(0xFFFF,                // destination pan id
                                      network.eui64,
                                      EMBER_NULL_NODE_ID,    // node id - ignored
                                      0x0000,                // group id - ignored
                                      EMBER_ZLL_PROFILE_ID);
  if (status != EMBER_SUCCESS) {
    emberAfAppPrintln("%p%p failed 0x%x",
                      "Error: ",
                      "Reset to factory new request",
                      status);
  }
  return status;
}

#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

static void deviceInformationRequestHandler(const EmberEUI64 source,
                                            uint32_t transaction,
                                            uint8_t startIndex)
{
  EmberEUI64 eui64;
  EmberStatus status;
  uint8_t total = emberAfEndpointCount();
  uint8_t i, *count;

  emberAfZllCommissioningClusterPrintln("RX: DeviceInformationRequest 0x%4x, 0x%x",
                                        transaction,
                                        startIndex);

  emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                             | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT
                             | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                            ZCL_ZLL_COMMISSIONING_CLUSTER_ID,
                            ZCL_DEVICE_INFORMATION_RESPONSE_COMMAND_ID,
                            "wuu",
                            transaction,
                            total,
                            startIndex);

  emberAfGetEui64(eui64);
  count = &appResponseData[appResponseLength];
  emberAfPutInt8uInResp(0); // temporary count
  // EMAPPFWKV2-1450: only send up to 4 endpoint information chunks at a time.
  // Otherwise, the payload will be too large. ZCL6 says the following in
  // 13.3.2.2.2.4 regarding the device information record count field:
  //   "This value shall be in the range 0x00 – 0x05."
  for (i = startIndex; i < total && *count < 4; i++) {
    // If the profile interop bit in the ZllInformation bitmask is cleared,
    // then we know this is a legacy ZLL app, so we set the profile ID in our
    // response to the ZLL profile ID. If the bit is set, then the profile ID
    // should be set to the ZigBee 3.0 common profile ID.
    uint8_t deviceVersion = emberAfDeviceVersionFromIndex(i);
    uint8_t endpoint = emberAfEndpointFromIndex(i);
    EmberAfProfileId profileId = (network.state & EMBER_ZLL_STATE_PROFILE_INTEROP
                                  ? emberAfProfileIdFromIndex(i)
                                  : EMBER_ZLL_PROFILE_ID);
    emberAfPutBlockInResp(eui64, EUI64_SIZE);
    emberAfPutInt8uInResp(endpoint);
    emberAfPutInt16uInResp(profileId);
    emberAfPutInt16uInResp(emberAfDeviceIdFromIndex(i));
    emberAfPutInt8uInResp(deviceVersion);
    emberAfPutInt8uInResp(emberAfPluginZllCommissioningGroupIdentifierCountCallback(endpoint));
    emberAfPutInt8uInResp(i); // sort order
    (*count)++;
  }

  status = emberAfSendCommandInterPan(0xFFFF,                // destination pan id
                                      source,
                                      EMBER_NULL_NODE_ID,    // node id - ignored
                                      0x0000,                // group id - ignored
                                      EMBER_ZLL_PROFILE_ID);
  if (status != EMBER_SUCCESS) {
    emberAfZllCommissioningClusterPrintln("%p%p failed 0x%x",
                                          "Error: ",
                                          "Device information response",
                                          status);
  }
}

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

static void deviceInformationResponseHandler(const EmberEUI64 source,
                                             uint32_t transaction,
                                             uint8_t numberOfSubDevices,
                                             uint8_t startIndex,
                                             uint8_t deviceInformationRecordCount,
                                             uint8_t *deviceInformationRecordList)
{
  uint16_t deviceInformationRecordListLen = (deviceInformationRecordCount
                                             * ZLL_DEVICE_INFORMATION_RECORD_SIZE);
  uint16_t deviceInformationRecordListIndex = 0;
  uint8_t i;
  bool validResponse = (emberEventControlGetActive(emberAfPluginZllCommissioningTouchLinkEventControl)
                        && (network.securityAlgorithm.transactionId == transaction)
                        && MEMCOMPARE(network.eui64, source, EUI64_SIZE) == 0);

  emberAfZllCommissioningClusterFlush();
  emberAfZllCommissioningClusterPrint("RX: DeviceInformationResponse 0x%4x, 0x%x, 0x%x, 0x%x,",
                                      transaction,
                                      numberOfSubDevices,
                                      startIndex,
                                      deviceInformationRecordCount);
  emberAfZllCommissioningClusterFlush();
  for (i = 0; i < deviceInformationRecordCount; i++) {
    uint8_t *ieeeAddress;
    uint8_t endpointId;
    uint16_t profileId;
    uint16_t deviceId;
    uint8_t version;
    uint8_t groupIdCount;
    uint8_t sort;
    ieeeAddress = &deviceInformationRecordList[deviceInformationRecordListIndex];
    deviceInformationRecordListIndex += EUI64_SIZE;
    endpointId = emberAfGetInt8u(deviceInformationRecordList, deviceInformationRecordListIndex, deviceInformationRecordListLen);
    deviceInformationRecordListIndex++;
    profileId = emberAfGetInt16u(deviceInformationRecordList, deviceInformationRecordListIndex, deviceInformationRecordListLen);
    deviceInformationRecordListIndex += 2;
    deviceId = emberAfGetInt16u(deviceInformationRecordList, deviceInformationRecordListIndex, deviceInformationRecordListLen);
    deviceInformationRecordListIndex += 2;
    version = emberAfGetInt8u(deviceInformationRecordList, deviceInformationRecordListIndex, deviceInformationRecordListLen);
    deviceInformationRecordListIndex++;
    groupIdCount = emberAfGetInt8u(deviceInformationRecordList, deviceInformationRecordListIndex, deviceInformationRecordListLen);
    deviceInformationRecordListIndex++;
    sort = emberAfGetInt8u(deviceInformationRecordList, deviceInformationRecordListIndex, deviceInformationRecordListLen);
    deviceInformationRecordListIndex++;
    emberAfZllCommissioningClusterPrint(" [");
    emberAfZllCommissioningClusterDebugExec(emberAfPrintBigEndianEui64(ieeeAddress));
    emberAfZllCommissioningClusterPrint(" 0x%x 0x%2x 0x%2x 0x%x 0x%x 0x%x",
                                        endpointId,
                                        profileId,
                                        deviceId,
                                        version,
                                        groupIdCount,
                                        sort);
    emberAfZllCommissioningClusterFlush();

    if (validResponse
        && (subDeviceCount
            < EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SUB_DEVICE_TABLE_SIZE)) {
      MEMMOVE(subDevices[subDeviceCount].ieeeAddress, ieeeAddress, EUI64_SIZE);
      subDevices[subDeviceCount].endpointId = endpointId;
      subDevices[subDeviceCount].profileId = profileId;
      subDevices[subDeviceCount].deviceId = deviceId;
      subDevices[subDeviceCount].version = version;
      subDevices[subDeviceCount].groupIdCount = groupIdCount;
      subDeviceCount++;
    } else {
      emberAfZllCommissioningClusterPrint(" (ignored)");
    }
    emberAfZllCommissioningClusterPrint("]");
    emberAfZllCommissioningClusterFlush();
  }
  emberAfZllCommissioningClusterPrintln("");

  if (validResponse
      && (subDeviceCount
          < EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SUB_DEVICE_TABLE_SIZE)
      && subDeviceCount < numberOfSubDevices) {
    sendDeviceInformationRequest(startIndex + deviceInformationRecordCount);
  }
}

#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

static void identifyRequestHandler(const EmberEUI64 source,
                                   uint32_t transaction,
                                   uint16_t identifyDurationS)
{
  if (!isZllDecisionId(EMBER_ZLL_POLICY_NO_TOUCHLINK_FOR_NFN)
      || amFactoryNew()) {
    emberAfZllCommissioningClusterPrintln("RX: IdentifyRequest 0x%4x, 0x%2x",
                                          transaction,
                                          identifyDurationS);
    emberAfPluginZllCommissioningIdentifyCallback(identifyDurationS);
  } else {
    emberAfZllCommissioningClusterPrintln("ignoring identify request as touchlinking policy is disabled for NFN");
  }
}

static void resetToFactoryNewRequestHandler(const EmberEUI64 source,
                                            uint32_t transaction)
{
  emberAfZllCommissioningClusterPrintln("RX: ResetToFactoryNewRequest 0x%4x",
                                        transaction);
  if (!amFactoryNew()) {
    emberAfZllResetToFactoryNew();
  }
}

static bool amFactoryNew(void)
{
  EmberTokTypeStackZllData token;
  emberZllGetTokenStackZllData(&token);
  return isFactoryNew(token.bitmask);
}

static bool isZllDecisionId(EmberZllPolicy decisionId)
{
#ifdef EZSP_HOST
  if (ezspGetPolicy(EZSP_ZLL_POLICY, &decisionId) != EZSP_SUCCESS) {
    return false;
  }
  return (decisionId == EMBER_ZLL_POLICY_NO_TOUCHLINK_FOR_NFN);
#else
  return (decisionId == emberZllGetPolicy());
#endif
}

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

static bool isSameNetwork(const EmberZllNetwork *network)
{
  EmberNodeType nodeType;
  EmberNetworkParameters parameters;
  EmberStatus status = emberAfGetNetworkParameters(&nodeType, &parameters);

  // Note, we only check the Extended Pan Id, not the Short Pan Id, nor the
  // Network Update Id.
  return (status == EMBER_SUCCESS
          && (MEMCOMPARE(parameters.extendedPanId,
                         network->zigbeeNetwork.extendedPanId,
                         EXTENDED_PAN_ID_SIZE) == 0));
}

// In the new multi-network stack, setting the radio channel is no longer
// effective since the MAC will re-tune the radio at the next outgoing packet
// based on what is stored in the NetworkInfo struct.
static EmberStatus setRadioChannel(uint8_t newChannel)
{
  return emberSetLogicalAndRadioChannel(newChannel);
}

static uint32_t getChannelMask(uint8_t purpose)
{
  uint32_t channelMask = emberGetZllPrimaryChannelMask();
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_SECONDARY_CHANNELS
  // We always scan the secondary channels if configured to do so.
  channelMask |= emberGetZllSecondaryChannelMask();
#endif
  return channelMask;
}

// Returns an integer greater than, equal to, or less than zero, according to
// whether target t1 is better than, equal to, or worse than target t2 in terms
// of requested priority with corrected signal strength serving as tiebreaker.
static int8_t targetCompare(const EmberZllNetwork *t1,
                            int8_t r1,
                            const EmberZllNetwork *t2,
                            int8_t r2)
{
  // When considering two targets, if only one has requested priority, that one
  // is chosen.
  if (isRequestingPriority(t1->state)
      && !isRequestingPriority(t2->state)) {
    return 1;
  } else if (!isRequestingPriority(t1->state)
             && isRequestingPriority(t2->state)) {
    return -1;
  }

  // If the priority of both targets is the same, the correct signal strength
  // is considered.  The strongest corrected signal wins.
  emberAfDebugPrintln("Signal strengths: r1 = %d, t1->corr = %d, r2 = %d, t2->corr = %d",
                      r1, t1->rssiCorrection, r2, t2->rssiCorrection);
  if (r1 + t1->rssiCorrection < r2 + t2->rssiCorrection) {
    return -1;
  } else if (r2 + t2->rssiCorrection < r1 + t1->rssiCorrection) {
    return 1;
  }

  // If we got here, both targets are considered equal.
  return 0;
}
#endif //EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR

static void setProfileInteropState(void)
{
  EmberTokTypeStackZllData token;

  emberZllGetTokenStackZllData(&token);
  token.bitmask |= EMBER_ZLL_STATE_PROFILE_INTEROP;
  emberZllSetTokenStackZllData(&token);
}
