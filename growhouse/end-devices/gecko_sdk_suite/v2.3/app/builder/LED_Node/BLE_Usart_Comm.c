/*
 * BLE_Usart_Comm.c
 *
 *  Created on: 12-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   BLE_Usart_Comm.c
 *   @brief  Implementation of communication protocol with BLE module
 *           over USART.
 *
 ******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "uartdrv.h"
#include <stdio.h>
#include "app/framework/include/af.h"
#include "BLE_Usart_Comm.h"

#ifdef EMBER_AF_PLUGIN_DPOT_AD5206
#include EMBER_AF_API_DPOT_AD5206
extern Ecode_t halAD5204DriverSetLedLevel(uint8_t Channel, uint8_t value);
extern bool isDefaultConfigurationSet;
extern bool isActualConfigurationSet;
#endif

/******************************************************************************/
/* MACROs		                                                              */
/******************************************************************************/

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

/* Flag for request over USART receive complete flag */
bool request_complition_flag = true;

/* buffer for identifying start and stop sequence */
char start_sequence_buffer[6];
int index_i, index_j;
int parameter_validate;

/* Buffer to store the received request parameter */
char g_str_parameter_buffer[7][256];

/* sequence is started flag */
int g_int_start_sequence;

/* req_complete
*  it represents whether request is complete or not
*/
int g_int_parse_request_complete ;

/* Global structure to store the LED intensity state and RGB Mapping of all
 * LED channel 1 to 6.
 */
LED_Configuration Current_LED_Configuration;

extern void SendLED_Configuration_ToCoordinator(uint8_t *LED_ConfigurationData, size_t length);
extern void SendLED_intensity_ToCoordinator(int *LED_intensityData, size_t length);

/******************************************************************************/
/* Function Declaration                                                       */
/******************************************************************************/
int parse_request(char usart_buffer_ch);

/******************************************************************************/
/* Function Definition                                                        */
/******************************************************************************/

/***************************************************************************//**
 * @brief Function to receive data over USART from BLE module.
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void BLEUartProcessInput(void)
{
	uint8_t next = 0;

    if(EMBER_SUCCESS  == emberSerialReadByte(HAL_SERIAL_PORT_USART2, &next))
    {
    	parse_request(next);
    }
}

/***************************************************************************//**
 * @brief Write data to BLE UART interface
 *
 * @param -  ledRequestData - pointer to the LED channel data structure.
 *
 * @return None.
 *******************************************************************************/
void BLEUsartResponse(ledPayload *pLedRequestData)
{
	uint8_t length = 0;

	length = sizeof(*pLedRequestData);

	COM_Printf(HAL_SERIAL_PORT_USART2, "%s", START_SEQ);
	COM_Printf(HAL_SERIAL_PORT_USART2, "%u,", length);
	COM_WriteData(HAL_SERIAL_PORT_USART2, (unsigned char*) pLedRequestData, sizeof(*pLedRequestData));
	COM_Printf(HAL_SERIAL_PORT_USART2, ",");
	COM_Printf(HAL_SERIAL_PORT_USART2, "%s", STOP_SEQ);
}

/***************************************************************************//**
 * @brief Write data to BLE UART interface
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void ZigbeeUsarttest(void)
{
	ledPayload ledRequestData;

	uint8_t length = 0;

	length = sizeof(ledRequestData);
	ledRequestData.requestType = RESPONSE;
	ledRequestData.ledNumber = LED3;
	ledRequestData.ledLevel[0] = 5;
	ledRequestData.ledLevel[1] = 6;
	ledRequestData.ledLevel[2] = 7;
	ledRequestData.ledLevel[3] = 8;
	ledRequestData.ledLevel[4] = 9;
	ledRequestData.ledLevel[5] = 10;


	COM_Printf(HAL_SERIAL_PORT_USART2, "%s", START_SEQ);
	COM_Printf(HAL_SERIAL_PORT_USART2, "%u,", length);
	COM_WriteData(HAL_SERIAL_PORT_USART2, (unsigned char*) &ledRequestData, sizeof(ledRequestData));
	COM_Printf(HAL_SERIAL_PORT_USART2, ",");
	COM_Printf(HAL_SERIAL_PORT_USART2, "%s", STOP_SEQ);
}

/***************************************************************************//**
 * @brief send provision state to BLE module over UART
 *
 * @param - None
 *
 * @return status code.
 *******************************************************************************/
int sendProvisionStateToBLEMeshNode(int provisionState)
{
	ledPayload ledUpdateData;
	ledUpdateData.requestType = provisionState;
	ledUpdateData.ledNumber = LED_ALL;

	emberAfCorePrintln("In sendProvisionStateToBLEMeshNode: %d", provisionState);

	/* send LED intensity current value */
	memcpy(ledUpdateData.ledLevel,
			Current_LED_Configuration.LED_Channel_intensity,
			sizeof(Current_LED_Configuration.LED_Channel_intensity));

	BLEUsartResponse(&ledUpdateData);
	return 0;
}

/***************************************************************************//**
 * @brief send the LED channel level update information to BLE module over UART
 *
 * @param - None
 *
 * @return status code.
 *******************************************************************************/
int sendLevelChangeToBLEMeshNode(void)
{
	ledPayload ledUpdateData;
	ledUpdateData.requestType = RESPONSE;
	ledUpdateData.ledNumber = LED_ALL;

	/* send LED intensity current value */
	memcpy(ledUpdateData.ledLevel,
			Current_LED_Configuration.LED_Channel_intensity,
			sizeof(Current_LED_Configuration.LED_Channel_intensity));

	emberAfCorePrintln("LED level send over uart: %d %d %d %d %d %d",
			ledUpdateData.ledLevel[0],
			ledUpdateData.ledLevel[1],
			ledUpdateData.ledLevel[2],
			ledUpdateData.ledLevel[3],
			ledUpdateData.ledLevel[4],
			ledUpdateData.ledLevel[5]);

	BLEUsartResponse(&ledUpdateData);
	return 0;
}

/***************************************************************************//**
 * @brief send the LED channel level update information to BLE module over UART
 *
 * @param - None
 *
 * @return status code.
 *******************************************************************************/
int sendLedConfigurationToBLEMeshNode(void)
{
	ledPayload ledUpdateData;
	ledUpdateData.requestType = RGB_MAPPING;
	ledUpdateData.ledNumber = LED_MAPPING;

	/* send LED mapping also */
	memcpy(ledUpdateData.rgbMapping,
			Current_LED_Configuration.LED_Configuration_Data,
			sizeof(Current_LED_Configuration.LED_Configuration_Data));

	if(isActualConfigurationSet)
		ledUpdateData.rgbMapping[6] = 0x1;
	if(isDefaultConfigurationSet)
		ledUpdateData.rgbMapping[6] = 0x0;

	/* send the response over UART */
	BLEUsartResponse(&ledUpdateData);

	return 0;
}

/***************************************************************************//**
 * @brief Process the request received from USART
 *
 * @param -  ledReqData - pointer to the payload data structure
 *
 * @return status code.
 * TODO: This is just a place holder function. Complete once actual action
 * implemented.
 *******************************************************************************/
int processRequest(ledPayload *ledReqData)
{
	ledPayload ledRequestData;
	bool wasHandled = false;
	uint8_t led_channel_color[LED_CHANNELS];
	uint8_t loop = 0;

	if(ledReqData == NULL)
	{
		return -1;
	}
	switch(ledReqData->requestType)
	{
		case REQUEST:
			if(ledReqData->ledNumber == IS_ZIGBEE_PRIVISION)
			{
				if(Current_LED_Configuration.Is_ZIGBEE_Provisioned == 0x01)
				{
					ledRequestData.requestType = ZIGBEE_PROVISION_STATE;
				}
				else
				{
					ledRequestData.requestType = ZIGBEE_UNPROVISION_STATE;
				}

			}
			else
			{
				emberAfCorePrintln("Processing the request ...");
				ledRequestData.requestType = RESPONSE;
				ledRequestData.ledNumber = ledReqData->ledNumber;
				memcpy(ledRequestData.ledLevel,
					Current_LED_Configuration.LED_Channel_intensity,
					sizeof(Current_LED_Configuration.LED_Channel_intensity));
			}

			/* send the requested LED level to BLE module */
			BLEUsartResponse(&ledRequestData);

			break;
		case RESPONSE:

			break;
		case CONTROL:
#ifdef EMBER_AF_PLUGIN_DPOT_AD5206

		/* Check whether control request is for all the LED channel or its only for a
		 * single LED Channel.
		 */

		if(ledReqData->ledNumber == LED_ALL)
		{
			emberAfCorePrintln("CONTROL Req - for all the LED Channel received.");
			/* Set all the Intensity as per requested value */
			for(loop=0; loop < LED_CHANNELS; loop++)
			{
				halAD5204DriverSetLedLevel(loop+1, ledReqData->ledLevel[loop]);
			}
		}
		else
		{
			emberAfCorePrintln("CONTROL Req - Channel : %d , level : %d",
					ledReqData->ledNumber, ledReqData->ledLevel[ledReqData->ledNumber - 1]);
			wasHandled = halAD5204DriverSetLedLevel(
					ledReqData->ledNumber, ledReqData->ledLevel[ledReqData->ledNumber - 1]);
			if(wasHandled != true)
			{
				emberAfCorePrintln("Fail to set the LED level on digital potentiometer");
			}
		}
#endif

		/* Send the LED intesity change update to zigbee coordinator */
		SendLED_intensity_ToCoordinator(Current_LED_Configuration.LED_Channel_intensity, LED_CHANNELS);

			break;

		case RGB_MAPPING:
			emberAfCorePrintln("RGB mapping update request from BLE module");

			/* Store the LED channel mapping updated from BLE module*/
			memcpy(Current_LED_Configuration.LED_Configuration_Data,
					ledReqData->rgbMapping,
					sizeof(Current_LED_Configuration.LED_Configuration_Data));

			Current_LED_Configuration.Is_LED_Configuration_Set = 0xAA;
			isActualConfigurationSet = true;

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

			/* Update into Flash */
			updateLED_Configuration_intoFlash(&Current_LED_Configuration);

			MEMCOPY(led_channel_color, Current_LED_Configuration.LED_Configuration_Data, LED_CHANNELS);

			emberAfCorePrintln(
				"LED channel mapping update  : map: %c %c %c %c %c %c\n",
				ledReqData->rgbMapping[0],
				ledReqData->rgbMapping[1],
				ledReqData->rgbMapping[2],
				ledReqData->rgbMapping[3],
				ledReqData->rgbMapping[4],
				ledReqData->rgbMapping[5]);

			break;

		case BLE_PROVISION_STATE:
			emberAfCorePrintln("BLE provision state receive.");
			Current_LED_Configuration.Is_BLE_Provisioned = 0x01;
			Current_LED_Configuration.Is_LED_Configuration_Set = 0xAA;

			/* Update into Flash */
			updateLED_Configuration_intoFlash(&Current_LED_Configuration);

			break;

		case BLE_UNPROVISION_STATE:
			emberAfCorePrintln("BLE unprovision state receive.");
			Current_LED_Configuration.Is_BLE_Provisioned = 0x00;

			if(Current_LED_Configuration.Is_ZIGBEE_Provisioned == 0x00)
			{
				Current_LED_Configuration.Is_LED_Configuration_Set = 0x55;
			}

			/* Update into Flash */
			updateLED_Configuration_intoFlash(&Current_LED_Configuration);

			break;

		default:
			emberAfCorePrintln("Error: Invalid Request type: %d", ledReqData->requestType);
			return -1;

	}
	return 0;
}

/***************************************************************************//**
 * @brief initialize the USART request parser parameters
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void parser_erase()
{
	g_int_start_sequence = 0;
	index_i = 0;
	index_j = 0;
	memset(&g_str_parameter_buffer[0][0], 0x00, sizeof(g_str_parameter_buffer));
	memset(&start_sequence_buffer, 0x00, sizeof(start_sequence_buffer));
}

/***************************************************************************//**
 * @brief Parse the request/data received over USART from Zigbee module
 *
 * @param -  usart_buffer_ch - received char
 *
 * @return status of parsing in progress.
 *******************************************************************************/
int parse_request(char usart_buffer_ch)
{
	ledPayload *ledReqData;
	// make string of 5 char to compare with starting sequence "A5A5,"
	if(g_int_start_sequence == 0 )
	{
		start_sequence_buffer[0] = start_sequence_buffer[1];
		start_sequence_buffer[1] = start_sequence_buffer[2];
		start_sequence_buffer[2] = start_sequence_buffer[3];
		start_sequence_buffer[3] = start_sequence_buffer[4];
		start_sequence_buffer[4] = usart_buffer_ch;
		start_sequence_buffer[5] = '\0';
	}

	// parse data into structure if sequence already started
	if(g_int_start_sequence ==  1 )
	{
		if(usart_buffer_ch != ',')
		{
			g_str_parameter_buffer[index_i][index_j] = usart_buffer_ch;
			index_j++;
			g_str_parameter_buffer[index_i][index_j] = '\0';
		}
		else
		{

			if(index_i <= 1)
			{
				sscanf(g_str_parameter_buffer[index_i], "%u", &parameter_validate);
			}
			switch(index_i + 1)
			{
				case 1: //COMMAND(15) or QUERY(16)
				{
					emberAfCorePrintln("length: %d", parameter_validate);
					break;
				}
				case 2: //COMMAND protocol or QUERY
				{
					emberAfCorePrintln("Actual: %d and required: %d", index_j, sizeof(ledPayload));
					if(index_j < sizeof(ledPayload))
					{
						g_str_parameter_buffer[index_i][index_j] = ',';
						index_j++;
						return 0;
					}
					memset(ledReqData, 0x00, sizeof(ledPayload));
					ledReqData = (ledPayload*)g_str_parameter_buffer[1];
					emberAfCorePrintln("%s", g_str_parameter_buffer[1]);

					emberAfCorePrintln(
						"Payload : %d %d, LED level: %d %d %d %d %d %d, RGB_map: %c %c %c %c %c %c",
						ledReqData->requestType, ledReqData->ledNumber,
						ledReqData->ledLevel[0],
						ledReqData->ledLevel[1],
						ledReqData->ledLevel[2],
						ledReqData->ledLevel[3],
						ledReqData->ledLevel[4],
						ledReqData->ledLevel[5],
						ledReqData->rgbMapping[0],
						ledReqData->rgbMapping[1],
						ledReqData->rgbMapping[2],
						ledReqData->rgbMapping[3],
						ledReqData->rgbMapping[4],
						ledReqData->rgbMapping[5]);
					break;
				}
				default:
				{

				}
			}
			if(g_int_parse_request_complete != 0)
			{
				index_i++;
			}
			index_j = 0;
		}
		if(!strcmp(g_str_parameter_buffer[index_i], "5A5A"))		// ending sequence
		{
			g_int_parse_request_complete = 1;
			emberAfCorePrintln("complete request received.\n");

			/* Process the request received from USART */
			processRequest(ledReqData);

			parser_erase();

		}
		if (request_complition_flag == false )
		{
			emberAfCorePrintln("Incomplete request...\n");
		}
	}
	if(!strcmp(start_sequence_buffer,AUTH_REQUEST) && g_int_start_sequence==0)
	{
		g_int_start_sequence = 1;
		g_int_parse_request_complete = 1;
	}
	return 0;
}
