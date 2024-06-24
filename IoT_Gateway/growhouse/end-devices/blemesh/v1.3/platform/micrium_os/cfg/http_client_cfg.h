/*
*********************************************************************************************************
*                                             EXAMPLE CODE
*********************************************************************************************************
* Licensing:
*   The licensor of this EXAMPLE CODE is Silicon Laboratories Inc.
*
*   Silicon Laboratories Inc. grants you a personal, worldwide, royalty-free, fully paid-up license to
*   use, copy, modify and distribute the EXAMPLE CODE software, or portions thereof, in any of your
*   products.
*
*   Your use of this EXAMPLE CODE is at your own risk. This EXAMPLE CODE does not come with any
*   warranties, and the licensor disclaims all implied warranties concerning performance, accuracy,
*   non-infringement, merchantability and fitness for your application.
*
*   The EXAMPLE CODE is provided "AS IS" and does not come with any support.
*
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*
*   You can contact us at: https://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                       HTTP CLIENT CONFIGURATION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : http_client_cfg.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _HTTP_CLIENT_CFG_H_
#define  _HTTP_CLIENT_CFG_H_

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             TASK OPTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  HTTPc_CFG_MODE_ASYNC_TASK_EN                       DEF_ENABLED

#define  HTTPc_CFG_MODE_BLOCK_EN                            DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         HTTP FEATURE OPTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  HTTPc_CFG_PERSISTENT_EN                            DEF_ENABLED

#define  HTTPc_CFG_CHUNK_TX_EN                              DEF_ENABLED

#define  HTTPc_CFG_QUERY_STR_EN                             DEF_ENABLED

#define  HTTPc_CFG_HDR_RX_EN                                DEF_ENABLED

#define  HTTPc_CFG_HDR_TX_EN                                DEF_ENABLED

#define  HTTPc_CFG_FORM_EN                                  DEF_ENABLED

#define  HTTPc_CFG_USER_DATA_EN                             DEF_ENABLED

#define  HTTPc_CFG_WEBSOCKET_EN                             DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of http_client_cfg.h module include.             */
