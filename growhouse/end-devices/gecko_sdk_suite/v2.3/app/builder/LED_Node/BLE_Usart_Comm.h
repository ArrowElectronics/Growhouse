/*
 * BLE_Usart_Comm.h
 *
 *  Created on: 23-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   BLE_Usart_Comm.h
 *   @brief  Header file for communication protocol with BLE module
 *           over USART.
 *
 ******************************************************************************/

#ifndef BLE_USART_COMM_H_
#define BLE_USART_COMM_H_

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include EMBER_AF_API_DPOT_AD5206
/******************************************************************************/
/* MACROs		                                                              */
/******************************************************************************/
#define START_SEQ "A5A5,"
#define STOP_SEQ  ",5A5A"
#define AUTH_REQUEST			"A5A5,"

#define HAL_ZIGBEE_CONN_PIN						 	 (14U)
#define HAL_ZIGBEE_CONN_PORT						 (gpioPortD)

/* NC channel config MACRO */
#define LED_CHANNEL_NC 'N'
/******************************************************************************/
/* Structures	                                                              */
/******************************************************************************/
typedef struct __attribute__ ((__packed__)) LED_PAYLOAD
{
	int requestType;
	int ledNumber;
	int ledLevel[6];
	char rgbMapping[7];
}ledPayload;

/******************************************************************************/
/* ENUMs		                                                              */
/******************************************************************************/
typedef enum {
	REQUEST = 1,
	RESPONSE,
	CONTROL,
	RGB_MAPPING,
	ZIGBEE_PROVISION_STATE,
	ZIGBEE_UNPROVISION_STATE,
	BLE_PROVISION_STATE,
	BLE_UNPROVISION_STATE
}cmdType;

typedef enum {
	LED1 = 1,
	LED2,
	LED3,
	LED4,
	LED5,
	LED6,
	LED_ALL,
	LED_MAPPING,
	IS_ZIGBEE_PRIVISION,
	IS_BLE_PROVISION
}ledNumber;

/******************************************************************************/
/* Function Declaration                                                       */
/******************************************************************************/
int sendLevelChangeToBLEMeshNode(void);
int sendLedConfigurationToBLEMeshNode(void);
int sendProvisionStateToBLEMeshNode(int provisionState);

#endif /* BLE_USART_COMM_H_ */
