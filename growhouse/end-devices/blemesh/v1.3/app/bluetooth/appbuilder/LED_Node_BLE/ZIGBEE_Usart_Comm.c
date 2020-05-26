/*
 * ZIGBEE_Usart_Comm.c
 *
 *  Created on: 02-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   ZIGBEE_Usart_Comm.c
 *   @brief  Implementation of communication protocol with Zigbee module
 *           over USART.
 *
 ******************************************************************************/


/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/

#include "uartdrv.h"
#include "ZIGBEE_Usart_Comm.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************/
/* MACROs		                                                              */
/******************************************************************************/

// Define receive/transmit operation queues
DEFINE_BUF_QUEUE(100, rxBufferQueue);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueue);

#define RXBUFSIZE    100                          /*  Buffer size for RX */
#define RECEIVE_CHUNK_SIZE 45	/* receive chunk size for USART */
#define HEADER_SIZE 14

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

/* USART handler for communication */
UARTDRV_HandleData_t handleData;
UARTDRV_Handle_t handle = &handleData;

/* USART data recive buffer */
uint8_t rxBuf[64] = {0};

/* counts 1ms timeTicks */
volatile uint32_t msTicks;

/* Zigbee availability status flag */
uint8_t zigbeeAvailable = 0;

/* UART data receive chunk size */
uint32_t receive_chunk_size = 0;

/* Global structure to store the LED intensity state of all
 * LED channel 1 to 6.
 */
LED_Configuration Current_LED_Configuration;

LED_Configuration Default_LED_Configuration =
{
    0x55, 0x01, 0x01, {'N', 'N', 'N', 'N', 'N', 'N'}, {50, 50, 50, 50, 50, 50}
};

/******************************************************************************/
/* Function Declaration                                                       */
/******************************************************************************/

static void receiveCallback(UARTDRV_Handle_t handle,
        Ecode_t transferStatus,
        uint8_t *data,
        UARTDRV_Count_t transferCount);

static int parse_request(char usart_buffer_ch);

extern void Delay(uint32_t dlyTicks);
extern void set_led_config_gatt_attribute(void);
extern int writeLedStateFlash(LED_Configuration *LED_Configuration_state);
/******************************************************************************/
/* Function Definition                                                        */
/******************************************************************************/

/***************************************************************************//**
 * @brief USART data receive call back
 *
 * @param -  handle - USART handler.
 * @param -  transferStatus - USART data transfer status
 * @param -  data - received data buffer pointer.
 * @param -  transferCount - Number of the byte data received.
 *
 * @return None.
 *******************************************************************************/
static void receiveCallback(UARTDRV_Handle_t handle,
        Ecode_t transferStatus,
        uint8_t *data,
        UARTDRV_Count_t transferCount)
{

	for(int byteCount = 0; byteCount < transferCount; byteCount++)
	{
		parse_request(data[byteCount]);
	}
	return;
}

/***************************************************************************//**
 * @brief USART initialization function
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void ZIGBEE_SerialInit(void)
{

  printf("Initializing USART for Zigbee module communication\r\n");

  /* Initialize UART driver handle */
  UARTDRV_InitUart_t initData = USART2_INIT;
  UARTDRV_InitUart(handle, &initData);

  /* store the chunk size */
  receive_chunk_size = sizeof(ledPayload) + HEADER_SIZE;
}

/***************************************************************************//**
 * @brief Write data to zigbee UART interface
 *
 * @param -  ptr - write buffer pointer.
 * @param -  len - Write data length in bytes.
 *
 * @return Number of byte return over USART.
 *******************************************************************************/
int writeZigbeeUART(unsigned char *ptr, int len)
{
  int txCount;

  for (txCount = 0; txCount < len; txCount++) {
	  UARTDRV_TransmitB(handle, (unsigned char*)ptr++, 1);
  }

  return len;
}

/***************************************************************************//**
 * @brief Request to get LED channel intensity data to zigbee module
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void ZigbeeUsartRequest(void)
{
	ledPayload ledRequestData;

	ledRequestData.requestType = REQUEST;
	ledRequestData.ledNumber = LED_ALL;
	ZigbeeUsartSend(&ledRequestData);
}

/***************************************************************************//**
 * @brief Request for zigbee module provision state
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void IsZigbeeProvisioned(void)
{
	ledPayload ledRequestData;

	ledRequestData.requestType = REQUEST;
	ledRequestData.ledNumber = IS_ZIGBEE_PRIVISION;
	ZigbeeUsartSend(&ledRequestData);
}

/***************************************************************************//**
 * @brief Request to get LED channel intensity data to zigbee module
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void sendLedConfigZigbee(void)
{
	ledPayload ledRequestData;

	printf("Sending LED configuration changes to zigbee module \r\n");
	ledRequestData.requestType = RGB_MAPPING;
	ledRequestData.ledNumber = LED_ALL;

	/* write updated LED configuration */
	memcpy(ledRequestData.rgbMapping,
			Current_LED_Configuration.LED_Configuration_Data,
			sizeof(Current_LED_Configuration.LED_Configuration_Data));

	  if(Current_LED_Configuration.Is_LED_Configuration_Set == 0x55)
	  {
		  ledRequestData.rgbMapping[6] = 0x00;
	  }
	  else if(Current_LED_Configuration.Is_LED_Configuration_Set == 0xAA)
	  {
		  ledRequestData.rgbMapping[6] = 0x01;
	  }

	/* send the request over UART to zigbee module */
	ZigbeeUsartSend(&ledRequestData);
}

/***************************************************************************//**
 * @brief Send BLE provision state to zigbee module
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void sendBLEProvisionState(void)
{
	ledPayload ledRequestData;

	printf("Send BLE provision state to zigbee module\r\n");
	if(Current_LED_Configuration.Is_BLE_Provisioned == 0x01)
	{
		ledRequestData.requestType = BLE_PROVISION_STATE;
	}
	else
	{
		ledRequestData.requestType = BLE_UNPROVISION_STATE;
	}
	ledRequestData.ledNumber = LED_ALL;

	/* send the request over UART to zigbee module */
	ZigbeeUsartSend(&ledRequestData);
}

/***************************************************************************//**
 * @brief Send the LED intensity data to zigbee module over USART
 *
 * @param -  pLedRequestData - Pointer to the led intensity data structure.
 *
 * @return None.
 *******************************************************************************/
void ZigbeeUsartSend(ledPayload *pLedRequestData)
{
	uint8_t length = 0;
	uint8_t comma = ',';
	uint8_t bufArray[10] = {0};

	length = sizeof(*pLedRequestData);

	/* send Start sequence */
	writeZigbeeUART((unsigned char*)START_SEQ, strlen(START_SEQ));
	snprintf((char *)bufArray, 5, "%u,", length);

	/* send length of the data to send */
	writeZigbeeUART(bufArray, 2);
	writeZigbeeUART(&comma, sizeof(unsigned char));

	/* send payload data over usart for light intensity request */
	writeZigbeeUART((unsigned char*) pLedRequestData, sizeof(*pLedRequestData));
	writeZigbeeUART(&comma, sizeof(unsigned char));

	/* send Stop sequence */
	writeZigbeeUART((unsigned char*)STOP_SEQ, strlen(STOP_SEQ));
}

/***************************************************************************//**
 * @brief Process the request received from USART
 *
 * @param -  ledReqData - pointer to the payload data structure
 *
 * @return status code.
 *******************************************************************************/
int processRequest(ledPayload *ledReqData)
{
	uint8_t loop = 0;
	if(ledReqData == NULL)
	{
		return -1;
	}
	switch(ledReqData->requestType)
	{
		case REQUEST:
			printf("No request msg expected from Zigbee module over UART\r\n");
			break;
		case RESPONSE:

			/* Store the LED channel updated level to current level */
			memcpy(Current_LED_Configuration.LED_Channel_intensity,
				ledReqData->ledLevel,
				sizeof(Current_LED_Configuration.LED_Channel_intensity));

		    /* store the updated LED status on Flash memory */
			writeLedStateFlash(&Current_LED_Configuration);

			/* set the LED channel intensity characteristics to latest value info */
			set_led_intensity_gatt_attribute();

			printf(
				"LED update response : %d %d, LED level: %d %d %d %d %d %d\r\n",
				ledReqData->requestType, ledReqData->ledNumber,
				Current_LED_Configuration.LED_Channel_intensity[0],
				Current_LED_Configuration.LED_Channel_intensity[1],
				Current_LED_Configuration.LED_Channel_intensity[2],
				Current_LED_Configuration.LED_Channel_intensity[3],
				Current_LED_Configuration.LED_Channel_intensity[4],
				Current_LED_Configuration.LED_Channel_intensity[5]);

			break;
		case CONTROL:
			printf("No Control msg expected from Zigbee module over UART\r\n");
			break;
		case RGB_MAPPING:
			printf("RGB mapping update request from Zigbee\r\n");

			/* Store the LED channel mapping updated from zigbee*/
			memcpy(Current_LED_Configuration.LED_Configuration_Data,
				ledReqData->rgbMapping,
				sizeof(Current_LED_Configuration.LED_Configuration_Data));

			if (ledReqData->rgbMapping[6] == 0x00)
			{
				Current_LED_Configuration.Is_LED_Configuration_Set = 0x55;
			}
			else if (ledReqData->rgbMapping[6] == 0x01)
			{
				Current_LED_Configuration.Is_LED_Configuration_Set = 0xAA;
			}

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

			/* store the updated LED status on Flash memory */
			writeLedStateFlash(&Current_LED_Configuration);

			printf(
				"LED channel mapping update  :flag: 0x%x map: %c %c %c %c %c %c\r\n",
				Current_LED_Configuration.Is_LED_Configuration_Set,
				Current_LED_Configuration.LED_Configuration_Data[0],
				Current_LED_Configuration.LED_Configuration_Data[1],
				Current_LED_Configuration.LED_Configuration_Data[2],
				Current_LED_Configuration.LED_Configuration_Data[3],
				Current_LED_Configuration.LED_Configuration_Data[4],
				Current_LED_Configuration.LED_Configuration_Data[5]);

			/* write updated LED configuration to GATT attributes */
			set_led_config_gatt_attribute();

			break;

		case ZIGBEE_PROVISION_STATE:
			printf("ZIGBEE provision state receive.\r\n");
			Current_LED_Configuration.Is_ZIGBEE_Provisioned = 0x01;

			/* store the updated LED status on Flash memory */
			writeLedStateFlash(&Current_LED_Configuration);

			break;

		case ZIGBEE_UNPROVISION_STATE:
			printf("ZIGBEE unprovision state receive.\r\n");
			Current_LED_Configuration.Is_ZIGBEE_Provisioned = 0x00;

			/* store the updated LED status on Flash memory */
			writeLedStateFlash(&Current_LED_Configuration);

			break;


		default:
			printf("Error: Invalid Request type\r\n");
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
void parser_erase(void)
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
static int parse_request(char usart_buffer_ch)
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
				case 1: //payload length
				{
					printf("length: %d\r\n", parameter_validate);
					break;
				}
				case 2: //Payload data
				{
					if(index_j < sizeof(ledPayload))
					{
						g_str_parameter_buffer[index_i][index_j] = ',';
						index_j++;
						return 0;
					}
					memset(ledReqData, 0x00, sizeof(ledPayload));
					ledReqData = (ledPayload*)g_str_parameter_buffer[1];

					printf(
						"Payload : %d %d, LED level: %d %d %d %d %d %d\r\n",
						ledReqData->requestType, ledReqData->ledNumber,
						ledReqData->ledLevel[0],
						ledReqData->ledLevel[1],
						ledReqData->ledLevel[2],
						ledReqData->ledLevel[3],
						ledReqData->ledLevel[4],
						ledReqData->ledLevel[5]);

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
			printf("complete request received over UART.\r\n");

			/* Process the request received from USART */
			processRequest(ledReqData);

			parser_erase();
		}
		if (request_complition_flag == false )
		{
			printf("Incomplete request received over UART.\r\n");
		}
	}
	if(!strcmp(start_sequence_buffer,AUTH_REQUEST) && g_int_start_sequence==0)
	{
		g_int_start_sequence = 1;
		g_int_parse_request_complete = 1;
	}
	return 0;
}

/***************************************************************************//**
 * @brief Process the Zigbee USART2 request
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void ZIGBEEUartProcessInput(void)
{
	if (ECODE_EMDRV_UARTDRV_OK
			== UARTDRV_Receive(handle, rxBuf, receive_chunk_size, receiveCallback)) {
	}
}

/***************************************************************************//**
 * @brief Check for Zigbee module presence in LED Node
 *
 * @param None.
 *
 * @return None.
 *******************************************************************************/
void IsZigbeeAvailable(void)
{
	uint8_t gpio_flag = 2;

	/* configure pushbutton PB0 and PB1 as inputs, with pull-up enabled */
	GPIO_PinModeSet(HAL_ZIGBEE_CONN_PORT, HAL_ZIGBEE_CONN_PIN,
			gpioModeInputPull, 1);

	Delay(100);

	/* Check the zigbee available status using gpio value */
	gpio_flag = GPIO_PinInGet(HAL_ZIGBEE_CONN_PORT, HAL_ZIGBEE_CONN_PIN);

	if(gpio_flag == 0)
	{
		zigbeeAvailable = 1;
		printf("Zigbee module is available \r\n");
		/* Set the SPI mux switch to let zigbee control the digital potentiometer over SPI */
		GPIO_PinModeSet(HAL_IN_BLE_PORT, HAL_IN_BLE_PIN, gpioModePushPull, 0);
	}
	else if(gpio_flag == 1)
	{
		zigbeeAvailable = 0;
		printf("Zigbee module not available \r\n");

		/* Set the SPI mux switch to get igital potentiometer control for BLE over SPI */
		GPIO_PinModeSet(HAL_IN_BLE_PORT, HAL_IN_BLE_PIN, gpioModePushPull, 1);
	}
	else
	{
		printf("Fail to read zigbee available status using GPIO \r\n");
	}

}
