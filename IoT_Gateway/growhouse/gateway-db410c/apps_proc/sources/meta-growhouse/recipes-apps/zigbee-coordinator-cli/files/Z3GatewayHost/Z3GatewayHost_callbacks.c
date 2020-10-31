// Copyright 2016 Silicon Laboratories, Inc.                                *80*

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"
#include "app/framework/plugin/concentrator/source-route-common.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "stack/include/trust-center.h"
#include "app/framework/plugin/device-table/device-table.h"
#include <stdlib.h>

/* Zigbee coordinator service version */
#define ZIGBEE_COORDINATOR_SERVICE_VERSION 6

// the number of tokens that can be written using ezspSetToken and read
// using ezspGetToken
#define MFGSAMP_NUM_EZSP_TOKENS 8
// the size of the tokens that can be written using ezspSetToken and
// read using ezspGetToken
#define MFGSAMP_EZSP_TOKEN_SIZE 8
// the number of manufacturing tokens
#define MFGSAMP_NUM_EZSP_MFG_TOKENS 11
// the size of the largest EZSP Mfg token, EZSP_MFG_CBKE_DATA
// please refer to app/util/ezsp/ezsp-enum.h
#define MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE 92

EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);

// Forward declarations of custom cli command functions
static void printSourceRouteTable(void);
static void mfgappTokenDump(void);
static void changeNwkKeyCommand(void);
static void printNextKeyCommand(void);
static void versionCommand(void);
static void setTxPowerCommand(void);
static void ConfigureLEDchannel(void);

static void startTransreceiver(void);
static void stopTransreceiver(void);
static bool stop = 0;
static uint8_t Tx_data = 1;
uint16_t transreceiver_destination = 0x0;           // end node Id
uint8_t TransmitBuffer[5];
EmberApsFrame TransreceiverFrame;

/*
 * Event to start transreceiver
 */
EmberEventControl emberAfPluginprescanTransreceiverEventControl;
void emberAfPluginprescanTransreceiverEventHandler(void);

EmberCommandEntry emberAfCustomCommands[] = {
  emberCommandEntryAction("print_srt", printSourceRouteTable, "", ""),
  emberCommandEntryAction("tokdump", mfgappTokenDump, "", ""),
  emberCommandEntryAction("changeNwkKey", changeNwkKeyCommand, "", ""),
  emberCommandEntryAction("printNextKey", printNextKeyCommand, "", ""),
  emberCommandEntryAction("version", versionCommand, "", ""),
  emberCommandEntryAction("txPower", setTxPowerCommand, "s", ""),
  emberCommandEntryAction("set-led-channel", ConfigureLEDchannel, "buuuuuu", "Set the LED color on channel."),
  emberCommandEntryAction("start-transreceiver", startTransreceiver, "v", ""),
  emberCommandEntryAction("stop-transreceiver", stopTransreceiver, "", ""),
  emberCommandEntryTerminator()
};

//// ******* test of token dump code

// the manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h
// the names are enumerated here to make it easier for the user
PGM_NO_CONST PGM_P ezspMfgTokenNames[] =
{
  "EZSP_MFG_CUSTOM_VERSION...",
  "EZSP_MFG_STRING...........",
  "EZSP_MFG_BOARD_NAME.......",
  "EZSP_MFG_MANUF_ID.........",
  "EZSP_MFG_PHY_CONFIG.......",
  "EZSP_MFG_BOOTLOAD_AES_KEY.",
  "EZSP_MFG_ASH_CONFIG.......",
  "EZSP_MFG_EZSP_STORAGE.....",
  "EZSP_STACK_CAL_DATA.......",
  "EZSP_MFG_CBKE_DATA........",
  "EZSP_MFG_INSTALLATION_CODE"
};

// IAS ACE Server side callbacks
bool emberAfIasAceClusterArmCallback(uint8_t armMode,
                                     uint8_t* armDisarmCode,
                                     uint8_t zoneId)
{
  uint16_t armDisarmCodeLength = emberAfStringLength(armDisarmCode);
  EmberNodeId sender = emberGetSender();
  uint16_t i;

  emberAfAppPrint("IAS ACE Arm Received %x", armMode);

  // Start i at 1 to skip over leading character in the byte array as it is the
  // length byte
  for (i = 1; i < armDisarmCodeLength; i++) {
    emberAfAppPrint("%c", armDisarmCode[i]);
  }
  emberAfAppPrintln(" %x", zoneId);

  emberAfFillCommandIasAceClusterArmResponse(armMode);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, sender);

  return true;
}

bool emberAfIasAceClusterBypassCallback(uint8_t numberOfZones,
                                        uint8_t* zoneIds,
                                        uint8_t* armDisarmCode)
{
  EmberNodeId sender = emberGetSender();
  uint8_t i;

  emberAfAppPrint("IAS ACE Cluster Bypass for zones ");

  for (i = 0; i < numberOfZones; i++) {
    emberAfAppPrint("%d ", zoneIds[i]);
  }
  emberAfAppPrintln("");

  emberAfFillCommandIasAceClusterBypassResponse(numberOfZones,
                                                zoneIds,
                                                numberOfZones);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, sender);

  return true;
}

// code to print out the source route table
static void printSourceRouteTable(void)
{
  uint8_t i;
  for (i = 0; i < sourceRouteTableSize; i++) {
    if (sourceRouteTable[i].destination != 0x0000) {
      emberAfCorePrintln("[ind]%x[dest]%2x[closer]%x[older]%x",
                         i,
                         sourceRouteTable[i].destination,
                         sourceRouteTable[i].closerIndex,
                         sourceRouteTable[i].olderIndex);
    }
    emberSerialWaitSend(APP_SERIAL);
  }
  emberAfCorePrintln("<print srt>");
  emberSerialWaitSend(APP_SERIAL);
}

// Called to dump all of the tokens. This dumps the indices, the names,
// and the values using ezspGetToken and ezspGetMfgToken. The indices
// are used for read and write functions below.
static void mfgappTokenDump(void)
{
  EmberStatus status;
  uint8_t tokenData[MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE];
  uint8_t index, i, tokenLength;

  // first go through the tokens accessed using ezspGetToken
  emberAfCorePrintln("(data shown little endian)");
  emberAfCorePrintln("Tokens:");
  emberAfCorePrintln("idx  value:");
  for (index = 0; index < MFGSAMP_NUM_EZSP_TOKENS; index++) {
    // get the token data here
    status = ezspGetToken(index, tokenData);
    emberAfCorePrint("[%d]", index);
    if (status == EMBER_SUCCESS) {
      // Print out the token data
      for (i = 0; i < MFGSAMP_EZSP_TOKEN_SIZE; i++) {
        emberAfCorePrint(" %X", tokenData[i]);
      }

      emberSerialWaitSend(APP_SERIAL);
      emberAfCorePrintln("");
    } else {
      // handle when ezspGetToken returns an error
      emberAfCorePrintln(" ... error 0x%x ...",
                         status);
    }
  }

  // now go through the tokens accessed using ezspGetMfgToken
  // the manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h
  // this file contains an array (ezspMfgTokenNames) representing the names.
  emberAfCorePrintln("Manufacturing Tokens:");
  emberAfCorePrintln("idx  token name                 len   value");
  for (index = 0; index < MFGSAMP_NUM_EZSP_MFG_TOKENS; index++) {
    // ezspGetMfgToken returns a length, be careful to only access
    // valid token indices.
    tokenLength = ezspGetMfgToken(index, tokenData);
    emberAfCorePrintln("[%x] %p: 0x%x:", index,
                       ezspMfgTokenNames[index], tokenLength);

    // Print out the token data
    for (i = 0; i < tokenLength; i++) {
      if ((i != 0) && ((i % 8) == 0)) {
        emberAfCorePrintln("");
        emberAfCorePrint("                                    :");
        emberSerialWaitSend(APP_SERIAL);
      }
      emberAfCorePrint(" %X", tokenData[i]);
    }
    emberSerialWaitSend(APP_SERIAL);
    emberAfCorePrintln("");
  }
  emberAfCorePrintln("");
}

static void changeNwkKeyCommand(void)
{
  EmberStatus status = emberAfTrustCenterStartNetworkKeyUpdate();

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Change Key Error %x", status);
  } else {
    emberAfCorePrintln("Change Key Success");
  }
}

static void dcPrintKey(uint8_t label, uint8_t *key)
{
  uint8_t i;
  emberAfCorePrintln("key %x: ", label);
  for (i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    emberAfCorePrint("%x", key[i]);
  }
  emberAfCorePrintln("");
}

static void printNextKeyCommand(void)
{
  EmberKeyStruct nextNwkKey;
  EmberStatus status;

  status = emberGetKey(EMBER_NEXT_NETWORK_KEY,
                       &nextNwkKey);

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Error getting key");
  } else {
    dcPrintKey(1, nextNwkKey.key.contents);
  }
}

static void versionCommand(void)
{
  emberAfCorePrintln("Version:  %d", ZIGBEE_COORDINATOR_SERVICE_VERSION);
  emberAfCorePrintln(" %s", __DATE__);
  emberAfCorePrintln(" %s", __TIME__);
  emberAfCorePrintln("");
}

static void setTxPowerCommand(void)
{
  int8_t dBm = (int8_t)emberSignedCommandArgument(0);

  emberSetRadioPower(dBm);
}

void SendConfigurationToendDevice(EmberEUI64 eui64, uint8_t *LED_Channel_Configuration, size_t configuration_length)
{
  uint16_t destination;           // LED node Id
  uint8_t srcEndpoint = 1;        // Source endpoint
  uint8_t dstEndpoint = 1;        // Destination endpoint
  uint16_t index = 0;

  EmberStatus status;
  uint8_t cmdIndex;
  uint8_t appZclBuffer[9];
  uint16_t appZclBufferLen;

  /* Get Network ID from eui64 from device table list */
  index = emberAfDeviceTableGetFirstIndexFromEui64(eui64);
  if(index == EMBER_AF_PLUGIN_DEVICE_TABLE_NULL_INDEX)
  {
    emberAfCorePrintln("Failed to get End device");
    return;
  }
  destination = emberAfDeviceTableGetNodeIdFromIndex(index);

  emberAfCorePrintln("destination : 0x%x",destination);

  EmberApsFrame ConfigurationApsFrame;
  ConfigurationApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
  ConfigurationApsFrame.clusterId = (uint16_t)0x0008;     //cluster ID
  ConfigurationApsFrame.sourceEndpoint = srcEndpoint;
  ConfigurationApsFrame.destinationEndpoint = dstEndpoint;

  appZclBuffer[0] = 0x09;                                 // ZCL Header frame control field
  appZclBuffer[1] = emberAfNextSequence();                // sequence number
  appZclBuffer[2] = 0x15;                                 // replace command ID

  //Amend Color data for all Six channel from index 3
  MEMMOVE(&appZclBuffer[3], LED_Channel_Configuration, configuration_length);

  appZclBufferLen = 9;

  //print buffer
  cmdIndex = (appZclBuffer[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) ? 4 : 2;

  emberAfGuaranteedPrintln("Msg: clus 0x%2x, cmd 0x%x, len %d",
		ConfigurationApsFrame.clusterId, appZclBuffer[cmdIndex], appZclBufferLen);
  emberAfGuaranteedPrint("buffer: ", appZclBufferLen);
  emberAfGuaranteedPrintBuffer(appZclBuffer, appZclBufferLen, true);

  //send buffer
  emAfApsFrameEndpointSetup(srcEndpoint, dstEndpoint);
  status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT, destination,
		&ConfigurationApsFrame, appZclBufferLen, appZclBuffer);
  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Error: CLI Send failed, status: 0x%X", status);
  }
}

void ConfigureLEDchannel(void) {

  /* We will initially configure no LED on all channel */
  uint8_t led_channel_color[6] = { 0xF, 0xF, 0xF, 0xF, 0xF, 0xF };

  EmberEUI64 eui64;       // LED node eui64

  emberAfCorePrintln("ConfigureLEDchannel");

  /* 0th Argument from command line */
  emberCopyBigEndianEui64Argument(0, eui64);

  /* Copy configured channel on LED, Here we are getting data command line */
  led_channel_color[5] = (uint8_t) emberUnsignedCommandArgument(6);
  led_channel_color[4] = (uint8_t) emberUnsignedCommandArgument(5);
  led_channel_color[3] = (uint8_t) emberUnsignedCommandArgument(4);
  led_channel_color[2] = (uint8_t) emberUnsignedCommandArgument(3);
  led_channel_color[1] = (uint8_t) emberUnsignedCommandArgument(2);
  led_channel_color[0] = (uint8_t) emberUnsignedCommandArgument(1);

  SendConfigurationToendDevice(eui64, led_channel_color, sizeof(led_channel_color));
}

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
       emberAfCorePrintln("Zigbee-Coordinator version = %d\n", ZIGBEE_COORDINATOR_SERVICE_VERSION);
       emberAfCorePrintln("End Device Capability:  %d\n", EMBER_MAX_END_DEVICE_CHILDREN);
}

static void stopTransreceiver()
{
	stop = 1;
}

static void startTransreceiver()
{
	stop = 0;
	Tx_data = 1;
	emberAfCorePrintln("Transreceiver test start");

	/* Get the destination address */
	transreceiver_destination = (EmberNodeId) emberUnsignedCommandArgument(0);
	emberAfCorePrintln("destination : 0x%x", transreceiver_destination);

	/* Fill Frame and transmit buffer */
	TransreceiverFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
	TransreceiverFrame.clusterId = (uint16_t) 0x0008;     //cluster ID
	TransreceiverFrame.sourceEndpoint = 1;
	TransreceiverFrame.destinationEndpoint = 1;

	TransmitBuffer[0] = 0x01;                     // ZCL Header frame control field
	TransmitBuffer[1] = emberAfNextSequence();    // sequence number
	TransmitBuffer[2] = 0x20;                     // command ID

	/* Schedule event */
	emberEventControlSetDelayMS(emberAfPluginprescanTransreceiverEventControl,
			1000);

}

// handler to send continous data to End node
void emberAfPluginprescanTransreceiverEventHandler(void)
{
	EmberStatus status;
	emberEventControlSetInactive(emberAfPluginprescanTransreceiverEventControl);

	TransmitBuffer[3] = Tx_data;
	//emberAfCorePrintln("Tx : %d",TransmitBuffer[3]);

	status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT,
			transreceiver_destination, &TransreceiverFrame, 4, TransmitBuffer);
	if (status != EMBER_SUCCESS)
		emberAfCorePrintln("Failed to send");

	Tx_data += 1;

	/* If data = 0 then make data = 1 */
	Tx_data = Tx_data ? Tx_data : 1;

	if (!stop)
		emberEventControlSetDelayMS(
				emberAfPluginprescanTransreceiverEventControl, 200);
}
