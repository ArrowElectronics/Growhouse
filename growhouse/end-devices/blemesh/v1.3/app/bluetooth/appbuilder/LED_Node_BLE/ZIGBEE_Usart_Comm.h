/*
 * ZIGBEE_Usart_Comm.h
 *
 *  Created on: 22-Nov-2018
 *      Author: einfochips
 */

/***************************************************************************//**
 *   @file   ZIGBEE_Usart_Comm.h
 *   @brief  Header file for communication protocol with Zigbee module
 *           over USART.
 *
 ******************************************************************************/

#ifndef ZIGBEE_USART_COMM_H_
#define ZIGBEE_USART_COMM_H_

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
/* Device initialization header */
#include "hal-config.h"

/******************************************************************************/
/* MACROs		                                                              */
/******************************************************************************/
/*// just kept for future reference
// Size of the receive operation queue
#define EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS    6
// Size of the transmit operation queue
#define EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS    6
// Set to 1 to enable hardware flow control
#define EMDRV_UARTDRV_FLOW_CONTROL_ENABLE       1
// Maximum number of driver instances.
// This maximum applies only when EMDRV_UARTDRV_FLOW_CONTROL_ENABLE = 1
#define EMDRV_UARTDRV_MAX_DRIVER_INSTANCES      4
// UART software flow control code: request peer to start TX
#define UARTDRV_FC_SW_XON                       0x11
// UART software flow control code: request peer to stop TX
#define UARTDRV_FC_SW_XOFF                      0x13
*/

#define USART2_INIT                                                                                        \
{                                                                                                          \
  USART2,                                                               /* USART port                   */ \
  HAL_USART2_BAUD_RATE,                                                 /* Baud rate                    */ \
  BSP_USART2_TX_LOC,                                                    /* USART Tx pin location number */ \
  BSP_USART2_RX_LOC,                                                    /* USART Rx pin location number */ \
  (USART_Stopbits_TypeDef)USART_FRAME_STOPBITS_ONE,                     /* Stop bits                    */ \
  (USART_Parity_TypeDef)USART_FRAME_PARITY_NONE,                        /* Parity                       */ \
  (USART_OVS_TypeDef)USART_CTRL_OVS_X16,                                /* Oversampling mode            */ \
  false,                                                                /* Majority vote disable        */ \
  uartdrvFlowControlNone,                  	                            /* Flow control                 */ \
  BSP_USART2_CTS_PORT,                                                  /* CTS port number              */ \
  BSP_USART2_CTS_PIN,                                                   /* CTS pin number               */ \
  BSP_USART2_RTS_PORT,                                                  /* RTS port number              */ \
  BSP_USART2_RTS_PIN,                                                   /* RTS pin number               */ \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueue,                         /* RX operation queue           */ \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueue,                         /* TX operation queue           */ \
  BSP_USART2_CTS_LOC,                                                   /* CTS pin location             */ \
  BSP_USART2_RTS_LOC                                                    /* RTS pin location             */ \
}

#define START_SEQ "A5A5,"
#define STOP_SEQ  ",5A5A"
#define AUTH_REQUEST			"A5A5,"

#define LED_CHANNELS                6
/* Mapping of LED level 0 to 65535 into RDACs level 0 to 100. */
#define PERCENTAGE_FACTOR (0.001525902f)

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


/* First byte is for either configuration is written or not */
typedef struct LED_Configuration
{
    uint8_t Is_LED_Configuration_Set;
	uint8_t Is_ZIGBEE_Provisioned;
	uint8_t Is_BLE_Provisioned;
    uint8_t LED_Configuration_Data[LED_CHANNELS];
    int LED_Channel_intensity[LED_CHANNELS];
}LED_Configuration;

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
void ZIGBEE_SerialInit(void);
void ZigbeeUsartRequest(void);
void ZIGBEEUartProcessInput(void);
void IsZigbeeAvailable(void);
void ZigbeeLedControl(ledPayload *pLedRequestData);
void ZigbeeUsartSend(ledPayload *pLedRequestData);
void sendLedConfigZigbee(void);
void IsZigbeeProvisioned(void);
void sendBLEProvisionState(void);

#endif /* ZIGBEE_USART_COMM_H_ */
