// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include EMBER_AF_API_NETWORK_STEERING
#include PLATFORM_HEADER
#include "BLE_Usart_Comm.h"

#ifdef EMBER_AF_PLUGIN_DPOT_AD5206
#include EMBER_AF_API_DPOT_AD5206
extern bool isDefaultConfigurationSet;
extern bool isActualConfigurationSet;
extern struct LED_Configuration Current_LED_Configuration;
#endif

#define LED_CONFIGURATION_UPDATE_COMMAND	0x15
#define LED_INTENSITY_REQUEST_COMMAND		0x16
#define LED_CONFIGURATION_REQUEST_COMMAND	0x17
#define LED_NODE_FIRMWARE_VERSION_REQUEST	0x18
#define LED_BLINKING_REQUEST				0x19

void ConfigureLEDchannel_CommandLine(void);
void ConfigureLEDchannel_FromFlash(void);
void SendLED_Configuration_ToCoordinator(uint8_t *LED_ConfigurationData, size_t length);
void SendLED_intensity_ToCoordinator(int *LED_intensityData, size_t length);
void SendLED_firmware_version_ToCoordinator();
void BlinkLeds(void);

extern void scheduleNetworkSteering(void);

/* Array for custom command */
EmberCommandEntry emberAfCustomCommands[] = {
		/*
		 * Use "custom set-led-channel 82 82 71 66 66 71" command in LED node ZigBee
		 * console to change LED color configuration command line
		 */
		emberCommandEntryAction("set-led-channel",
				ConfigureLEDchannel_CommandLine,
				"uuuuuu",
				"Set the LED color on channel."),
		emberCommandEntryAction("blink-led",
				BlinkLeds,
				"",
				"Blink All LEDs for 3 Seconds"),
		emberCommandEntryTerminator()
};

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
	struct LED_Configuration LED_Configuration_Block;
	uint8_t loop = 0;
	emberAfCorePrintln("emberAfStackStatusCallback Called. Status : %d",status);

	/* On network Leave request, writing default Configuration and
	 * clearing previously configured global variables.
	 */

	/*
	 * Scenario observed was:
	 * ----------------------
	 * When BLE mesh mobile application have set the configuration and
	 * after that ZigBee provision mobile application started.
	 *
	 * Issue was:
	 * ----------
	 * There might be possible that when LED node tried to join ZC
	 * using distributed key than there is Network join failed status
	 * occurred due to No network key received from  ZC.
	 *
	 * And because of that LED channel configuration data was erased
	 * which was previously set by BLE mesh application.
	 *
	 * Solution:
	 * ----------
	 * Prevented that LED erase on EMBER_JOIN_FAILED status through
	 * adding condition in if statement.
	 */

	if(emberAfNetworkState() == EMBER_NO_NETWORK && status == EMBER_NETWORK_DOWN)
	{
		emberAfCorePrintln("emberAfStackStatusCallback : Network Leave");

    	getLED_configuration_FromFlash(&LED_Configuration_Block);

		LED_Configuration_Block.Is_ZIGBEE_Provisioned = 0x00;
	    if(LED_Configuration_Block.Is_BLE_Provisioned != 0x01)
	    {
	    	/* Reset the LED intensity if BLE is in unprovision */
	    	LED_Configuration_Block.Is_LED_Configuration_Set = 0x55;

	        emberAfCorePrintln("Setting default intensity level on LED Channels");
	        /* Set the LED channel intensity to default state */
	        LED_Configuration_Block.LED_Channel_intensity[0] = 50;
	        LED_Configuration_Block.LED_Channel_intensity[1] = 50;
	        LED_Configuration_Block.LED_Channel_intensity[2] = 50;
	        LED_Configuration_Block.LED_Channel_intensity[3] = 50;
	        LED_Configuration_Block.LED_Channel_intensity[4] = 50;
	        LED_Configuration_Block.LED_Channel_intensity[5] = 50;

	        /* send the updated LED update status over UART to BLE module */
	        emberAfCorePrintln("Sending the LED channel intensity level update to BLE module");
	        sendLevelChangeToBLEMeshNode();
	        halCommonDelayMilliseconds(200);
	        /* After reset, Initialize the Intensity as per Default value */
	        for(loop=0; loop < LED_CHANNELS; loop++)
	        {
	            halAD5204DriverSetLedLevel(loop+1, LED_Configuration_Block.LED_Channel_intensity[loop]);
	        }

	        isActualConfigurationSet = false;
	        isDefaultConfigurationSet = true;
	        sendLedConfigurationToBLEMeshNode();
	        halCommonDelayMilliseconds(200);
	    }

	    updateLED_Configuration_intoFlash(&LED_Configuration_Block);

		/* send provision state to BLE module */
		sendProvisionStateToBLEMeshNode(ZIGBEE_UNPROVISION_STATE);
	}

	switch (status) {

	/* From Co-ordinator, send command to leave the network,
	 *
	 * So, in this scenario, network steering of device will schedule.
	 */
	case EMBER_NETWORK_DOWN:
		/* Start Network steering */
		scheduleNetworkSteering();
		break;

	case EMBER_NETWORK_UP:
		break;

	default:
		break;
	}
	return true;
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
	uint8_t RGB_colorMapping[6];
	uint8_t loop = 0;

	if (incomingApsFrame->clusterId == 0x0008) {
		switch (incomingMessage->message[2]) {
		case LED_CONFIGURATION_UPDATE_COMMAND:
			RGB_colorMapping[0] = incomingMessage->message[3];
			RGB_colorMapping[1] = incomingMessage->message[4];
			RGB_colorMapping[2] = incomingMessage->message[5];
			RGB_colorMapping[3] = incomingMessage->message[6];
			RGB_colorMapping[4] = incomingMessage->message[7];
			RGB_colorMapping[5] = incomingMessage->message[8];

			emberAfCorePrintln("New Mapping : %d, %d, %d, %d, %d, %d",
					RGB_colorMapping[0], RGB_colorMapping[1],
					RGB_colorMapping[2], RGB_colorMapping[3],
					RGB_colorMapping[4], RGB_colorMapping[5]);

			/* Get LED configuration block from Flash */
			getLED_configuration_FromFlash(&Current_LED_Configuration);

			/* Update actual configuration flag */
			Current_LED_Configuration.Is_LED_Configuration_Set = 0xAA;

			/* Update RGB mapping into LED configuration block */
			MEMCOPY(Current_LED_Configuration.LED_Configuration_Data,
					RGB_colorMapping, LED_CHANNELS);

			/* Check for the NC(Not Connected) channels and update
			 * the intensity to zero for all NC and save in Flash storage.
			 */
			for(loop=0; loop < LED_CHANNELS; loop++)
			{
				if(Current_LED_Configuration.LED_Configuration_Data[loop] == LED_CHANNEL_NC)
				{
					/* setting LED intensity value to 0 for NC channels in flash */
					Current_LED_Configuration.LED_Channel_intensity[loop] = 0;
				}
			}

			/* Update Configuration into Flash */
			updateLED_Configuration_intoFlash(&Current_LED_Configuration);

			sendLedConfigurationToBLEMeshNode();

			break;

		case LED_INTENSITY_REQUEST_COMMAND:
			emberAfCorePrintln("Send LED intensity level");
			emberAfCorePrintln("Level : %d, %d, %d, %d, %d, %d",
					Current_LED_Configuration.LED_Channel_intensity[0],
					Current_LED_Configuration.LED_Channel_intensity[1],
					Current_LED_Configuration.LED_Channel_intensity[2],
					Current_LED_Configuration.LED_Channel_intensity[3],
					Current_LED_Configuration.LED_Channel_intensity[4],
					Current_LED_Configuration.LED_Channel_intensity[5]);

			SendLED_intensity_ToCoordinator(
					Current_LED_Configuration.LED_Channel_intensity,
					LED_CHANNELS);

			/* when this message is received, we can consider the LED module as
			 * provisioned over zigbee network */
			emberAfCorePrintln("LED Node is provisioned.");
			Current_LED_Configuration.Is_ZIGBEE_Provisioned = 0x01;

			updateLED_Configuration_intoFlash(&Current_LED_Configuration);
			halCommonDelayMilliseconds(200);
			/* send provision state to BLE module */
			sendProvisionStateToBLEMeshNode(ZIGBEE_PROVISION_STATE);
			break;

		case LED_CONFIGURATION_REQUEST_COMMAND:
			emberAfCorePrintln("Send LED configuration data");
			/* Get LED Channel configuration data from flash and send the data to co-ordinator */
			ConfigureLEDchannel_FromFlash();
			break;

		case LED_NODE_FIRMWARE_VERSION_REQUEST:
			emberAfCorePrintln("Send LED firmware version");
			SendLED_firmware_version_ToCoordinator();
			break;
		case LED_BLINKING_REQUEST:
			emberAfCorePrintln("Blinking LEDs");
			StartLED_blinking();
			break;
		default:
			break;
		}
	}

	return false;
}

/***************************************************************************/
/**
 * @brief API to Send LED Firmware version data to co-ordinator
 *
 * @params	None
 *
 * @return	None
 ***************************************************************************/
void SendLED_firmware_version_ToCoordinator()
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
		appZclBuffer[2] = LED_NODE_FIRMWARE_VERSION_REQUEST;  // Command ID

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
 * @brief API to Send LED Intensity update data to co-ordinator
 *		  This API will get input parameters as LED channel intensity data
 *		  and length of data.
 *
 * @params 	LED_ConfigurationData : Pointer to array of LED channel intensity data
 * 			length : Length of LED channel inetsnity data array
 *
 * @return None
 ***************************************************************************/
void SendLED_intensity_ToCoordinator(int *LED_intensityData, size_t length)
{
	if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
		uint16_t destination = 0x0000; 	//	coordinator node Id
		uint8_t srcEndpoint = 1; 		//	Source endpoint
		uint8_t dstEndpoint = 1; 		//	Destination endpoint

		EmberStatus status = 0;
		uint8_t appZclBuffer[10];
		uint16_t appZclBufferLen = 0;

		EmberApsFrame ConfigurationApsFrame;

		ConfigurationApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
		ConfigurationApsFrame.clusterId = (uint16_t) 0x0008;	//cluster ID
		ConfigurationApsFrame.sourceEndpoint = srcEndpoint;
		ConfigurationApsFrame.destinationEndpoint = dstEndpoint;

		MEMSET(appZclBuffer, 0, sizeof(appZclBuffer));

		appZclBuffer[0] = 0x09; 							// ZCL Header frame control field
		appZclBuffer[1] = emberAfNextSequence();  			// sequence number
		appZclBuffer[2] = 0x16;  							// Command ID

		/* Assign the current LED channel intensity */
		appZclBuffer[3] = (uint8_t)LED_intensityData[0];
		appZclBuffer[4] = (uint8_t)LED_intensityData[1];
		appZclBuffer[5] = (uint8_t)LED_intensityData[2];
		appZclBuffer[6] = (uint8_t)LED_intensityData[3];
		appZclBuffer[7] = (uint8_t)LED_intensityData[4];
		appZclBuffer[8] = (uint8_t)LED_intensityData[5];

		appZclBufferLen = 3 + length;

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
 * @brief API to Send data to co-ordinator
 *		  This API will get input parameters as LED color configuration data
 *		  and length of data.
 *
 * @params 	LED_ConfigurationData : Pointer to array of configuraton data
 * 			length : Length of configuration data array
 *
 * @return None
 ***************************************************************************/
void SendLED_Configuration_ToCoordinator(uint8_t *LED_ConfigurationData, size_t length)
{
	if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
		uint16_t destination = 0x0000; 	//	coordinator node Id
		uint8_t srcEndpoint = 1; 		//	Source endpoint
		uint8_t dstEndpoint = 1; 		//	Destination endpoint

		EmberStatus status = 0;
		uint8_t appZclBuffer[10];
		uint16_t appZclBufferLen = 0;

		EmberApsFrame ConfigurationApsFrame;

		ConfigurationApsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
		ConfigurationApsFrame.clusterId = (uint16_t)0x0008;	//cluster ID
		ConfigurationApsFrame.sourceEndpoint = srcEndpoint;
		ConfigurationApsFrame.destinationEndpoint = dstEndpoint;

		appZclBuffer[0] = 0x09; 							// ZCL Header frame control field
		appZclBuffer[1] = emberAfNextSequence();  			// sequence number
		appZclBuffer[2] = 0x15;  							// Command ID

		//Amend Color data for all Six channel from index 3
		MEMCOPY(&appZclBuffer[3], LED_ConfigurationData, length);

		if(isDefaultConfigurationSet)
			appZclBuffer[9] = 0;
		else if(isActualConfigurationSet)
			appZclBuffer[9] = 1;

		appZclBufferLen = 3 + length + 1;

		//print buffer
		emberAfCorePrintln("Msg: cluster 0x%2x, Command 0x%x, Length %d",
				ConfigurationApsFrame.clusterId, appZclBuffer[2], appZclBufferLen);
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
 * @brief API to Configure LED color channel from command line user input.
 *		  This API will read data from command line input from user and send
 *		  that data to co-ordinator
 *
 * @param void
 *
 * @return None
 ***************************************************************************/

void ConfigureLEDchannel_CommandLine(void)
{
	struct LED_Configuration LED_Configuration_Block;

	/* We will initially configure no LED on all channel */
	uint8_t led_channel_color[LED_CHANNELS];

	emberAfCorePrintln("ConfigureLEDchannel");

	/* Copy configured channel on LED, Here we are getting data command line */
	led_channel_color[5] = (uint8_t) emberUnsignedCommandArgument(5);
	led_channel_color[4] = (uint8_t) emberUnsignedCommandArgument(4);
	led_channel_color[3] = (uint8_t) emberUnsignedCommandArgument(3);
	led_channel_color[2] = (uint8_t) emberUnsignedCommandArgument(2);
	led_channel_color[1] = (uint8_t) emberUnsignedCommandArgument(1);
	led_channel_color[0] = (uint8_t) emberUnsignedCommandArgument(0);

	/* Get LED configuration block from Flash */
	getLED_configuration_FromFlash(&LED_Configuration_Block);

	/* Update actual configuration flag */
	LED_Configuration_Block.Is_LED_Configuration_Set = 0xAA;

	/* Update the configuration block */
	MEMCOPY(LED_Configuration_Block.LED_Configuration_Data, led_channel_color, LED_CHANNELS);

	/* Update the same into flash and send it to co-ordinator */
	updateLED_Configuration_intoFlash(&LED_Configuration_Block);

	SendLED_Configuration_ToCoordinator(led_channel_color, sizeof(led_channel_color));
}

/***************************************************************************/
/**
 * @brief API to Configure LED color channel from Flash data
 *		  This API will read data from flash and send it to co-ordinator.
 * @param void
 *
 * @return None
 ***************************************************************************/
void ConfigureLEDchannel_FromFlash(void)
{
	struct LED_Configuration LED_Configuration_Block;
	uint8_t led_channel_color[LED_CHANNELS];

	/* Get LED configuration block from Flash */
	getLED_configuration_FromFlash(&LED_Configuration_Block);

	MEMCOPY(led_channel_color, LED_Configuration_Block.LED_Configuration_Data, LED_CHANNELS);

	/* Send LED configuration to co-ordinator */
	SendLED_Configuration_ToCoordinator(led_channel_color, sizeof(led_channel_color));
}

/** @brief Called when Button 0 is pressed long
 *
 * @param timePressedMs Amount of time button 0 was pressed.
 * @param pressedAtReset Was the button pressed at startup.
 */
void emberAfPluginButtonInterfaceButton0PressedLongCallback(uint16_t timePressedMs,
                                                            bool pressedAtReset)
{
	struct LED_Configuration LED_Configuration_Block;
	EmberStatus status = 0xF;
	uint8_t loop = 0;
	emberAfCorePrintln("Factory Reset: Long press detected");

	/* send provision state to BLE module */
	sendProvisionStateToBLEMeshNode(ZIGBEE_UNPROVISION_STATE);

	/* Updating flash RGB configuration */
	getLED_configuration_FromFlash(&LED_Configuration_Block);

	LED_Configuration_Block.Is_ZIGBEE_Provisioned = 0x00;
    if(LED_Configuration_Block.Is_BLE_Provisioned != 0x01)
    {
        /* Reset the LED intensity if BLE is in unprovision */
    	LED_Configuration_Block.Is_LED_Configuration_Set = 0x55;

        emberAfCorePrintln("Setting default intensity level on LED Channels");
        /* Set the LED channel intensity to default state */
        LED_Configuration_Block.LED_Channel_intensity[0] = 50;
        LED_Configuration_Block.LED_Channel_intensity[1] = 50;
        LED_Configuration_Block.LED_Channel_intensity[2] = 50;
        LED_Configuration_Block.LED_Channel_intensity[3] = 50;
        LED_Configuration_Block.LED_Channel_intensity[4] = 50;
        LED_Configuration_Block.LED_Channel_intensity[5] = 50;

        updateLED_Configuration_intoFlash(&LED_Configuration_Block);

        /* send the updated LED update status over UART to BLE module */
        emberAfCorePrintln("Sending the LED channel intensity level update to BLE module");
        halCommonDelayMilliseconds(200);
        sendLevelChangeToBLEMeshNode();

        /* After reset, Initialize the Intensity as per Default value */
        for(loop=0; loop < LED_CHANNELS; loop++)
        {
            halAD5204DriverSetLedLevel(loop+1, LED_Configuration_Block.LED_Channel_intensity[loop]);
        }

    	isActualConfigurationSet = false;
    	isDefaultConfigurationSet = true;
	halCommonDelayMilliseconds(200);
	sendLedConfigurationToBLEMeshNode();
    }

	updateLED_Configuration_intoFlash(&LED_Configuration_Block);

	/* Leaving Network */
	status = emberLeaveNetwork();
	emberAfCorePrintln("Status : %d",status);
}

void BlinkLeds(void)
{
	StartLED_blinking();
}
