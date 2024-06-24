//
// test-harness-z3-core.c
//
// August 3, 2015
// Refactored November 23, 2015
//
// ZigBee 3.0 core test harness functionality
//

#include "app/framework/include/af.h"
#include "app/framework/security/af-security.h"
#include "app/framework/util/attribute-storage.h" // emberAfResetAttributes()
#include "app/framework/util/af-main.h" // emAfCliVersionCommand()
#include "app/framework/security/security-config.h" // key definitions

#include "app/util/zigbee-framework/zigbee-device-common.h"

#ifdef EMBER_AF_API_ZLL_PROFILE
  #include EMBER_AF_API_ZLL_PROFILE // emberAfZllResetToFactoryNew()
#elif defined(EMBER_SCRIPTED_TEST)
  #include "../zll-commissioning/zll-commissioning.h"
#endif

#ifdef EMBER_AF_API_NETWORK_STEERING
  #include EMBER_AF_API_NETWORK_STEERING
#elif defined(EMBER_SCRIPTED_TEST)
  #include "../network-steering/network-steering.h"
#endif

#include "stack/include/packet-buffer.h"
#include "stack/include/network-formation.h"

#include "test-harness-z3-core.h"
#include "test-harness-z3-nwk.h"
#include "test-harness-z3-zll.h"
#include "test-harness-z3-zdo.h"
#include "test-harness-z3-mac.h"

#ifdef EZSP_HOST
// NCP
  #define addTransientLinkKey        ezspAddTransientLinkKey
#else
// SoC
  #define addTransientLinkKey        emberAddTransientLinkKey
#endif

// -----------------------------------------------------------------------------
// Constants

// -----------------------------------------------------------------------------
// Extern functions

uint8_t emMacHeaderLength(uint8_t *macFrame);
EmberMessageBuffer emPacketHeaderPayload(EmberMessageBuffer commandBuffer);

// -----------------------------------------------------------------------------
// Globals

EmAfPluginTestHarnessZ3DeviceMode emAfPluginTestHarnessZ3DeviceMode;
EmberEventControl emberAfPluginTestHarnessZ3OpenNetworkEventControl;
uint16_t emAfPluginTestHarnessZ3TouchlinkProfileId = EMBER_ZLL_PROFILE_ID;

// -----------------------------------------------------------------------------
// Util

uint32_t emAfPluginTestHarnessZ3GetSignificantBit(uint8_t commandIndex)
{
  uint32_t mask = emberUnsignedCommandArgument(commandIndex);
  uint32_t significantBit = (1UL << 31);

  while (!(significantBit & mask) && significantBit) {
    significantBit >>= 1;
  }

  return significantBit;
}

#if !defined(EZSP_HOST)
static uint8_t getApsFrameControl(EmberMessageBuffer commandBuffer,
                                  uint8_t commandIndex)
{
  uint8_t fc = 0xFF;

#ifndef EZSP_HOST
  uint8_t fcIndex = commandIndex;

  fcIndex -= (EUI64_SIZE           // source address
              + sizeof(uint32_t)   // frame counter
              + sizeof(uint8_t));  // frame control

  fcIndex -= (sizeof(uint8_t)     // aps counter
              + sizeof(uint8_t)); // fc!

  fc = emberGetLinkedBuffersByte(commandBuffer, fcIndex);
#endif /* EZSP_HOST */

  return fc;
}
#endif

// -----------------------------------------------------------------------------
// State

#define PRINTING_MASK_BEACONS BIT(0)
#define PRINTING_MASK_ZDO     BIT(1)
#define PRINTING_MASK_NWK     BIT(2)
#define PRINTING_MASK_APS     BIT(3)
#define PRINTING_MASK_ZCL     BIT(4)
static uint8_t printingMask = PRINTING_MASK_ZCL; // always print zcl commands

static EmberPanId networkCreatorPanId = 0xFFFF;

#if !defined(EZSP_HOST)
static uint8_t commandData[COMMAND_DATA_SIZE];
#endif

// -----------------------------------------------------------------------------
// Stack Callbacks

void emberIncomingCommandHandler(EmberZigbeeCommandType commandType,
                                 EmberMessageBuffer commandBuffer,
                                 uint8_t indexOfCommand,
                                 void *data)
{
#if !defined(EZSP_HOST)
  EmberStatus status;
  uint8_t commandId, commandDataLength;
  EmberApsFrame *apsFrame;

  // FIXME: protect against bad lengths.
  commandDataLength = emberMessageBufferLength(commandBuffer) - indexOfCommand;
  if (indexOfCommand + commandDataLength
      > emberMessageBufferLength(commandBuffer)) {
    return;
  }

  emberCopyFromLinkedBuffers(commandBuffer,
                             indexOfCommand,
                             commandData,
                             commandDataLength);
  commandId = commandData[0];
  switch (commandType) {
    case EMBER_ZIGBEE_COMMAND_TYPE_NWK:
      if (printingMask & PRINTING_MASK_NWK) {
        // FIXME: talk to Simon about sequence and security bytes.
        emberAfCorePrint("nwk:rx seq AC sec 28 cmd %X payload[",
                         commandId);
        emberAfCorePrintBuffer(commandData + 1, commandDataLength - 1, true); // spaces?
        emberAfCorePrintln("]");
      }
      if (commandId == NWK_LEAVE_COMMAND
          && emAfPluginTestHarnessZ3IgnoreLeaveCommands) {
        // Ignore the leave by turning off the request bit in the options.
        uint8_t newOptions
          = emberGetLinkedBuffersByte(commandBuffer, indexOfCommand + 1) & ~BIT(6);
        emberSetLinkedBuffersByte(commandBuffer,
                                  indexOfCommand + 1,
                                  newOptions);
      }
      break;
    case EMBER_ZIGBEE_COMMAND_TYPE_APS:
      if (printingMask & PRINTING_MASK_APS) {
        uint8_t fc = getApsFrameControl(commandBuffer, indexOfCommand);
        emberAfCorePrint("aps:rx seq AC fc %X cmd %X payload[",
                         fc,
                         commandId);
        emberAfCorePrintBuffer(commandData + 1, commandDataLength - 1, true); // spaces?
        emberAfCorePrintln("]");
      }
      break;
    case EMBER_ZIGBEE_COMMAND_TYPE_BEACON:
      if (printingMask & PRINTING_MASK_BEACONS) {
        // Cheat since we know where the pan id lives.
        uint16_t panId
          = emberAfGetInt16u(emberMessageBufferContents(commandBuffer), 9, 11);
        emberAfCorePrint("beacon:rx 0x%2X, AP 0x%p, EP ",
                         panId,
                         ((commandData[1] & BEACON_ASSOCIATION_PERMIT_BIT)
                          ? "1"
                          : "0"));
        emberAfCorePrintBuffer(commandData + BEACON_EXTENDED_PAN_ID_INDEX,
                               EXTENDED_PAN_ID_SIZE,
                               true); // spaces?
        emberAfCorePrintln("");

        // Print out the full payload, for the benefit of the beanshell tests
        emberAfCorePrint("payload[");
        uint8_t commandDataLength = emberMessageBufferLength(commandBuffer) - indexOfCommand;
        emberAfCorePrintBuffer(commandData,
                               commandDataLength,
                               true); // spaces?
        emberAfCorePrintln("]");
      }
      break;
    case EMBER_ZIGBEE_COMMAND_TYPE_MAC_COMMAND:
      if (printingMask & PRINTING_MASK_BEACONS) {
        if (commandId == BEACON_REQUEST_COMMAND) {
          emberAfCorePrintln("beacon-req:rx");
        }
      }
      break;
    case EMBER_ZIGBEE_COMMAND_TYPE_ZDO:
      apsFrame = (EmberApsFrame *)data;
      if (printingMask & PRINTING_MASK_ZDO) {
        emberAfCorePrint("zdo:t%4X:%p seq %X cmd %2X payload[",
                         emberAfGetCurrentTime(),
                         "rx",
                         commandData[0],
                         apsFrame->clusterId);
        emberAfCorePrintBuffer(commandData + ZDO_MESSAGE_OVERHEAD,
                               commandDataLength - ZDO_MESSAGE_OVERHEAD,
                               true); // spaces?
        emberAfCorePrintln("]");
      }
      status = emAfPluginTestHarnessZ3ZdoCommandResponseHandler(commandBuffer,
                                                                indexOfCommand,
                                                                apsFrame);
      if (status != EMBER_INVALID_CALL) {
        emberAfCorePrintln("%p: %p: cluster: 0x%02X status: 0x%X",
                           TEST_HARNESS_Z3_PRINT_NAME,
                           "ZDO negative command",
                           apsFrame->clusterId | 0x8000,
                           status);
      }
      break;
    case EMBER_ZIGBEE_COMMAND_TYPE_ZCL:
      if (printingMask & PRINTING_MASK_ZCL) {
        emberAfCorePrint("t%4x:rx len %d, ep %X, clus 0x1000 (ZLL Commissioning) FC %X seq %X cmd %X payload[",
                         emberAfGetCurrentTime(),
                         commandDataLength,
                         1,             // fake endpoint 1 for zll
                         commandData[0], // frame control
                         commandData[1], // sequence
                         commandData[2]); // command
        emberAfCorePrintBuffer(commandData + 3,
                               commandDataLength - 3,
                               true); // spaces?
        emberAfCorePrintln("]");
        emAfPluginTestHarnessZ3ZllCommandCallback(commandBuffer,
                                                  indexOfCommand,
                                                  data); // source eui64
      }
      break;
    case EMBER_ZIGBEE_COMMAND_TYPE_RAW_MAC:
      break;
    default:
      emberAfCorePrintln("Error: unrecognized incoming EmberZigbeeCommandType %X.", commandType);
  }
#endif // EZSP_HOST
}

void emberOutgoingCommandHandler(EmberZigbeeCommandType commandType,
                                 EmberMessageBuffer commandBuffer,
                                 uint8_t indexOfCommand,
                                 void *data)
{
#if !defined(EZSP_HOST)
  // Grab the command proper from the message buffer.
  // (this is the MAC command proper without the overhead - what indexOfCommand
  // should be pointing to)
  uint8_t commandDataLength = emberMessageBufferLength(commandBuffer) - indexOfCommand;
  assert(commandDataLength + EXTRA_COMMAND_BYTES <= COMMAND_DATA_SIZE);
  emberCopyFromLinkedBuffers(commandBuffer,
                             indexOfCommand,
                             commandData,
                             commandDataLength);

  switch (commandType) {
    case EMBER_ZIGBEE_COMMAND_TYPE_BEACON:
    {
      // For ZTT, we just need to mangle the Zigbee payload.
      emberAfDebugPrintln("emberOutgoingCH - sending a beacon");
      uint8_t payloadLength = BEACON_PAYLOAD_SIZE;
      emAfPluginTestHarnessZ3ModifyBeaconPayload(commandData + MAC_BEACON_SIZE,
                                                 &payloadLength);
      emberCopyToLinkedBuffers(commandData + MAC_BEACON_SIZE,
                               commandBuffer,
                               indexOfCommand + MAC_BEACON_SIZE,
                               BEACON_PAYLOAD_SIZE);
      if (payloadLength > BEACON_PAYLOAD_SIZE) {
        emberAppendToLinkedBuffers(commandBuffer,
                                   commandData + commandDataLength,
                                   payloadLength - BEACON_PAYLOAD_SIZE);
      } else if (payloadLength < BEACON_PAYLOAD_SIZE) {
        emberAfDebugPrintln("Old buffer length = %d", emberMessageBufferLength(commandBuffer));
        emberSetLinkedBuffersLength(commandBuffer, indexOfCommand + MAC_BEACON_SIZE + payloadLength);
        emberAfDebugPrintln("New buffer length = %d", emberMessageBufferLength(commandBuffer));
      }
      break;
    }

    case EMBER_ZIGBEE_COMMAND_TYPE_MAC_COMMAND:
    {
      // For ZTT, we may need to mangle both the MAC header and the Zigbee payload.
      uint8_t macHeaderLength = emMacHeaderLength(commandData);
      assert(macHeaderLength == commandDataLength);

      // Get the payload buffer, and append to the command data in our local array
      EmberMessageBuffer payloadBuffer = emPacketHeaderPayload(commandBuffer);
      uint8_t payloadDataLength = emberMessageBufferLength(payloadBuffer);
      assert(commandDataLength + payloadDataLength <= COMMAND_DATA_SIZE);
      emberCopyFromLinkedBuffers(payloadBuffer,
                                 0,
                                 commandData + commandDataLength,
                                 payloadDataLength);

      emberAfDebugPrint("mac hdr:tx %d bytes [", commandDataLength);
      emberAfDebugPrintBuffer(commandData, commandDataLength, true);
      emberAfDebugPrintln("]");
      emberAfDebugPrint("mac pay:tx %d bytes [", payloadDataLength);
      emberAfDebugPrintBuffer(commandData + commandDataLength, payloadDataLength, true);
      emberAfDebugPrintln("]");

      // look at the APS header to see if we have an interpan msg.
      if (commandData[macHeaderLength + 2] == 0x03) {
        emberAfCorePrintln("emberOutgoingCH - sending interpan msg");
        boolean messageModified = emAfPluginTestHarnessZ3ZllModifyInterpanCommand(commandData,
                                                                                  macHeaderLength,
                                                                                  data);
        if (messageModified) {
          // update the stack message buffers with the modified contents
          emberCopyToLinkedBuffers(commandData,
                                   commandBuffer,
                                   indexOfCommand,
                                   commandDataLength);
          emberCopyToLinkedBuffers(commandData + macHeaderLength,
                                   payloadBuffer,
                                   0,
                                   payloadDataLength);
        }
      }
      break;
    }

    default:
      emberAfCorePrintln("Error: unrecognized outgoing EmberZigbeeCommandType %X.", commandType);
  }
#endif // EZSP_HOST
}

// -----------------------------------------------------------------------------
// Printing

void emAfPluginTestHarnessZ3PrintingCommand(void)
{
  bool enabled = (emberStringCommandArgument(-1, NULL)[0] == 'e');
  uint8_t mask;

  switch (emberStringCommandArgument(-2, NULL)[0]) {
    case 'b':
      mask = PRINTING_MASK_BEACONS;
      break;
    case 'z':
      mask = PRINTING_MASK_ZDO;
      break;
    case 'n':
      mask = PRINTING_MASK_NWK;
      break;
    case 'a':
      mask = PRINTING_MASK_APS;
      break;
    default:
      mask = 0xFF;
  }

  if (enabled) {
    SETBITS(printingMask, mask);
  } else {
    CLEARBITS(printingMask, mask);
  }

  emberAfCorePrintln("%puccessfully %p printing.",
                     (mask == 0xFF ? "Uns" : "S"),
                     (enabled ? "enabled" : "disabled"));
}

// -----------------------------------------------------------------------------
// Framework Callbacks

void emberAfPluginTestHarnessZ3StackStatusCallback(EmberStatus status)
{
  if (status == EMBER_NETWORK_UP
      && emberAfGetNodeId() == EMBER_TRUST_CENTER_NODE_ID) {
    // Force the network open for joining if we are the trust center.
    // This is terrible security, but this plugin is for a terrible test
    // harness app, so I feel like it lines up.
    emberEventControlSetActive(emberAfPluginTestHarnessZ3OpenNetworkEventControl);
  }
}

void emberAfPluginTestHarnessZ3OpenNetworkEventHandler(void)
{
  emberEventControlSetInactive(emberAfPluginTestHarnessZ3OpenNetworkEventControl);

  if (emberAfGetNodeId() == EMBER_TRUST_CENTER_NODE_ID) {
    EmberEUI64 wildcardEui64 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
    EmberKeyData centralizedKey = ZIGBEE_PROFILE_INTEROPERABILITY_LINK_KEY;
    uint16_t transientKeyTimeoutS;

    // Make sure the trust center lets everyone on the network and
    // allows key requests.
    addTransientLinkKey(wildcardEui64, &centralizedKey);
#ifdef EZSP_HOST
    ezspSetPolicy(EZSP_TC_KEY_REQUEST_POLICY, EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_SEND_CURRENT_KEY);
    ezspGetConfigurationValue(EZSP_CONFIG_TRANSIENT_KEY_TIMEOUT_S,
                              &transientKeyTimeoutS);
#else
    emberTrustCenterLinkKeyRequestPolicy = EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_SEND_CURRENT_KEY;
    transientKeyTimeoutS = emberTransientKeyTimeoutS;
#endif

    // Make sure we reopen the network before the transient key disappears.
    // Add in a timing slop of a second to prevent any race conditions.
    emberEventControlSetDelayMS(emberAfPluginTestHarnessZ3OpenNetworkEventControl,
                                ((transientKeyTimeoutS * MILLISECOND_TICKS_PER_SECOND)
                                 - MILLISECOND_TICKS_PER_SECOND));
  }
}

// -----------------------------------------------------------------------------
// Core CLI commands

// plugin test-harness z3 reset
void emAfPluginTestHarnessZ3ResetCommand(void)
{
  emberAfZllResetToFactoryNew();

  emberAfCorePrintln("%p: %p: 0x%X",
                     TEST_HARNESS_Z3_PRINT_NAME,
                     "Reset",
                     EMBER_SUCCESS);
}

// plugin test-harness z3 set-device-mode <mode:1>
void emAfPluginTestHarnessZ3SetDeviceModeCommand(void)
{
  EmberStatus status = EMBER_BAD_ARGUMENT;
  EmAfPluginTestHarnessZ3DeviceMode mode
    = (EmAfPluginTestHarnessZ3DeviceMode)emberUnsignedCommandArgument(0);

  if (mode <= EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_MAX) {
    emAfPluginTestHarnessZ3DeviceMode = mode;
    status = EMBER_SUCCESS;
  }

  emberAfCorePrintln("%p: %p: 0x%X",
                     TEST_HARNESS_Z3_PRINT_NAME,
                     "Set device mode",
                     status);
}

// plugin test-harness z3 set-short-address
void emAfPluginTestHarnessZ3SetShortAddressCommand(void)
{
  emberAfCorePrintln("%p: %p: 0x%X",
                     TEST_HARNESS_Z3_PRINT_NAME,
                     "Set short address",
                     EMBER_INVALID_CALL);
}

// plugin test-harness z3 legacy-profile-id
void emAfPluginTestHarnessZ3LegacyProfileCommand(void)
{
  emAfPluginTestHarnessZ3TouchlinkProfileId = (emberStringCommandArgument(-1, NULL)[0] == 'e')
                                              ? EMBER_ZLL_PROFILE_ID
                                              : HA_PROFILE_ID;
}

// plugin test-harness z3 set-pan-id
void emAfPluginTestHarnessSetNetworkCreatorPanId(void)
{
  networkCreatorPanId = (EmberPanId)emberUnsignedCommandArgument(0);

  emberAfCorePrintln("Network Creator PAN ID = 0x%2X", networkCreatorPanId);
}

EmberPanId emberAfPluginNetworkCreatorGetPanIdCallback(void)
{
  return (networkCreatorPanId == 0xFFFF
          ? halCommonGetRandom()
          : networkCreatorPanId);
}

// plugin test-harness z3 platform
void emAfPluginTestHarnessZ3PlatformCommand(void)
{
  emberAfCorePrintln("Platform: Silicon Labs");
  emberAfCorePrint("EmberZNet ");
  emAfCliVersionCommand();
}

// plugin test-harness z3 install-code clear
// plugin test-harness z3 install-code set <code>
void emAfPluginTestHarnessZ3InstallCodeClearOrSetCommand(void)
{
  bool doClear = (emberStringCommandArgument(-1, NULL)[0] == 'c');

#ifndef EZSP_HOST
#if !defined(EMBER_AF_HAS_SECURITY_PROFILE_NONE)
  if (false == doClear) {
    EmberEUI64 eui64 = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }; //wild card
    EmberKeyData key;
    EmberStatus status;
    uint8_t code[16 + 2]; // 6, 8, 12, or 16 bytes plus two-byte CRC
    uint8_t length;

    length = emberCopyStringArgument(0, code, sizeof(code), false);

    // Convert the install code to a key
    status = emAfInstallCodeToKey(code, length, &key);

    if (EMBER_SUCCESS != status) {
      if (EMBER_SECURITY_DATA_INVALID == status) {
        emberAfAppPrintln("ERR: Calculated CRC does not match -len");
      } else if (EMBER_BAD_ARGUMENT == status) {
        emberAfAppPrintln("ERR: Install Code must be 8, 10, 14, or 18 bytes in "
                          "length");
      } else {
        emberAfAppPrintln("ERR: AES-MMO hash failed: 0x%x", status);
      }
      return;
    }

    // Add the key to transient key.
    status = addTransientLinkKey(eui64, &key);
    emberAfAppDebugExec(emAfPrintStatus("Set joining link key", status));
    emberAfAppPrintln("");
    emberAfAppFlush();
  } else {
    // We currently clear all the transient keys.
    emberClearTransientLinkKeys();
    emberAfAppPrintln("Success: Clear joining link key");
  }
#else
  emberAfAppPrintln("Security Core Library must be included to use this command.");
#endif
#else
  emberAfCorePrintln("%p: %p %p: 0x%X",
                     TEST_HARNESS_Z3_PRINT_NAME,
                     "Install code",
                     (doClear ? "clear" : "set"),
                     EMBER_INVALID_CALL);
#endif
}
