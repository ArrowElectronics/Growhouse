// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include EMBER_AF_API_NETWORK_STEERING
#include PLATFORM_HEADER

void readSoildata(void);
void SendSoilNode_firmware_version_ToCoordinator(void);

extern void scheduleNetworkSteering(void);
extern void halPhMoistureStartRead();
void BindingTableSetCommand(void);

#define SOIL_NODE_FIRMWARE_VERSION_REQUEST 0x18
#define SOIL_NODE_BIND_REQUEST 0x19

/* Array for custom command */
EmberCommandEntry emberAfCustomCommands[] = {
		/*
		 * Use "custom read-soil-data" command in Soil node ZigBee
		 * console to read soil node pH and Moisture data command line.
		 * If Soil node pH and Moisture clusters are binded with ZC than
		 * it will report that data to ZC also.
		 */
		emberCommandEntryAction("read-soil-data",
				readSoildata,
				"",
				"Read Soil pH and Moisture data"),
		emberCommandEntryTerminator()
};

void readSoildata()
{
	// Read pH and moisture value and send it to ZC
	halPhMoistureStartRead();
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
boolean emberAfStackStatusCallback(EmberStatus status)
{
	emberAfCorePrintln("emberAfStackStatusCallback Called. Status : %d",status);

	/*
	 *  Scenario    : When user wants to leave Soil Node, in that time if Soil Node is power off
	 *  Observation : In this scenario Cloud gets feedback that Soil Node device has been removed
	 *                (Cloud sends un-bind request and than after Leave request),
	 *                Cloud receives that command executed, but actually un-bind command was failed
	 *                and to send leave request co-ordinato waits for end device.
	 *                When SoilNode powered on it join with Co-Ordinator and
	 *                as soon as Soil Node joined with co-ordinator, co-ordinator stack have sent leave request
	 *                and Soil node left from network without clearing binding table.
	 *  Solution    : Whenever we receive leave request, we should clear the binding table.
	 */

	/* Clearing binding table on leave request */
	if(emberAfNetworkState() == EMBER_NO_NETWORK)
	{
		emberAfCorePrintln("emberAfStackStatusCallback : Clearing binding table");
		emberClearBindingTable();
	}

	switch (status) {

	case EMBER_NETWORK_UP:
		emberSetRadioPower(10);
		emberAfCorePrintln("Radio Power : %d", emberGetRadioPower());

		/* On network up clear the binding table and set the binding table again */
		emberClearBindingTable();
		BindingTableSetCommand();
		break;

	/* From Co-ordinator, send command to leave the network,
	 *
	 * So, in this scenario, network steering of device will schedule.
	 */
	case EMBER_NETWORK_DOWN:
		/* Start Network steering */
		scheduleNetworkSteering();
		break;

	default:
		break;
	}
	return true;
}

/** @brief Called when Button 0 is pressed long
 *
 * @param timePressedMs Amount of time button 0 was pressed.
 * @param pressedAtReset Was the button pressed at startup.
 */
void emberAfPluginButtonInterfaceButton0PressedLongCallback(uint16_t timePressedMs,
                                                            bool pressedAtReset)
{
	EmberStatus status = 0xF;
	emberAfCorePrintln("Factory Reset: Long press detected");
	/* Clearing binding table */
	emberClearBindingTable();

	/* Leaving Network */
	status = emberLeaveNetwork();
	emberAfCorePrintln("Status : %d",status);
}

/***************************************************************************/
/**
 * @brief API to Send Soil Node Firmware version data to co-ordinator
 *
 * @params	None
 *
 * @return	None
 ***************************************************************************/
void SendSoilNode_firmware_version_ToCoordinator()
{
	if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
		uint16_t destination = 0x0000;  //	coordinator node Id
		uint8_t srcEndpoint = 1;        //	Source endpoint
		uint8_t dstEndpoint = 1;        //	Destination endpoint

		EmberStatus status = 0;
		uint8_t appZclBuffer[12];
		uint16_t appZclBufferLen = 0;

		EmberApsFrame ConfigurationApsFrame;

		ConfigurationApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
		ConfigurationApsFrame.clusterId = (uint16_t) 0x0008;	//cluster ID
		ConfigurationApsFrame.sourceEndpoint = srcEndpoint;
		ConfigurationApsFrame.destinationEndpoint = dstEndpoint;

		MEMSET(appZclBuffer, 0, sizeof(appZclBuffer));

		appZclBuffer[0] = 0x01;                               // Cluster specific
		appZclBuffer[1] = emberAfNextSequence();              // sequence number
		appZclBuffer[2] = SOIL_NODE_FIRMWARE_VERSION_REQUEST; // Command ID

		/* Fill the buffer */
		appZclBuffer[3] = 0;	// fieldControl
		appZclBuffer[4] = LOW_BYTE(EMBER_AF_MANUFACTURER_CODE);
		appZclBuffer[5] = HIGH_BYTE(EMBER_AF_MANUFACTURER_CODE);
		appZclBuffer[6] = LOW_BYTE(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID);
		appZclBuffer[7] = HIGH_BYTE(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_IMAGE_TYPE_ID);
		appZclBuffer[8] = BYTE_0(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION);
		appZclBuffer[9] = BYTE_1(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION);
		appZclBuffer[10] = BYTE_2(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION);
		appZclBuffer[11] = BYTE_3(EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION);

		appZclBufferLen = 12;

		//print buffer
		emberAfCorePrintln("Msg: cluster 0x%2x, Command 0x%x, Length %d",
				ConfigurationApsFrame.clusterId, appZclBuffer[2],
				appZclBufferLen);
		emberAfCorePrintln("buffer: ");
		emberAfCorePrintBuffer(appZclBuffer, appZclBufferLen, true);

		status = emberAfSendUnicast(EMBER_OUTGOING_DIRECT, destination,
				&ConfigurationApsFrame, appZclBufferLen, appZclBuffer);

		if (status != EMBER_SUCCESS) {
			emberAfCorePrintln("Error: Send failed, status: 0x%X", status);
		}
	}
}

/***************************************************************************/
/**
 * @brief API  Soil Node set bind table
 *
 * @param	clusterId  : set bind table for this cluster id
 *
 * @param   index   : binding table index
 *(const EmberEUI64 eui64
 * @return	None
 ***************************************************************************/
void BindingTableSetCommand()
{
	uint8_t index ;
	uint16_t cluster_Array[2] = {0xFD00, 0X0001};
	EmberBindingTableEntry entry;
	EmberKeyStruct TrustCenter_entry;
	uint8_t endpoint = 1; //local endpoint
	EmberStatus status = emberAfPushEndpointNetworkIndex(endpoint);
	if (status == EMBER_SUCCESS)
	{
		entry.type = EMBER_UNICAST_BINDING;
		entry.local = endpoint;
		entry.remote = 1; //remote endpoint
		emberGetKey(EMBER_TRUST_CENTER_LINK_KEY, &TrustCenter_entry);
		MEMMOVE(entry.identifier, TrustCenter_entry.partnerEUI64, 8); //copy trust center eui64 in  entry,identifier
		entry.networkIndex = emberGetCurrentNetwork();
		for(index=0 ; index < 2 ;index++)
		{
			entry.clusterId = cluster_Array[index];
			status = emberSetBinding(index, &entry);
			emberAfAppPrintln("set bind %d: 0x%x", index, status);
		}
		//emberAfPopNetworkIndex();
	}
}

/** @brief Pre Message Received
 *
 * This callback is the first in the Application Framework's message processing
 * chain. The Application Framework calls it when a message has been received
 * over the air but has not yet been parsed by the ZCL command-handling code. If
 * you wish to parse some messages that are completely outside the ZCL
 * specification or are not handled by the Application Framework's command
 * handling code, you should intercept them for parsing in this callback.

 *   This callback returns a Boolean value indicating whether or not the message
 * has been handled. If the callback returns a value of true, then the
 * Application Framework assumes that the message has been handled and it does
 * nothing else with it. If the callback returns a value of false, then the
 * application framework continues to process the message as it would with any
 * incoming message.
        Note: 	This callback receives a pointer to an
 * incoming message struct. This struct allows the application framework to
 * provide a unified interface between both Host devices, which receive their
 * message through the ezspIncomingMessageHandler, and SoC devices, which
 * receive their message through emberIncomingMessageHandler.
 *
 * @param incomingMessage   Ver.: always
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
	EmberApsFrame *incomingApsFrame = incomingMessage->apsFrame;
	bool ret = false;
	if (incomingApsFrame->clusterId == 0x0008) {
			switch (incomingMessage->message[2]) {
			case SOIL_NODE_FIRMWARE_VERSION_REQUEST:
					emberAfCorePrintln("Send Soil node firmware version");
					SendSoilNode_firmware_version_ToCoordinator();
					ret = true;
					break;
			case SOIL_NODE_BIND_REQUEST:
				  emberAfCorePrintln("Send Soil node bind request");
				  BindingTableSetCommand();
				  ret = true;
				  break;
				default:
					ret = false;
					break;
			}
	}

	return ret;
}
