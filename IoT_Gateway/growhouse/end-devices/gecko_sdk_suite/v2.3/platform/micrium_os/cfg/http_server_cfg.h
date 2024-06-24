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
*                                       HTTP SERVER CONFIGURATION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : http_server_cfg.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _HTTP_SERVER_CFG_H_
#define  _HTTP_SERVER_CFG_H_

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            DEBUG OPTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  HTTPs_CFG_DBG_INFO_EN                              DEF_ENABLED

#define  HTTPs_CFG_CTR_STAT_EN                              DEF_ENABLED

#define  HTTPs_CFG_CTR_ERR_EN                               DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                         HTTP FEATURE OPTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  HTTPs_CFG_FS_PRESENT_EN                            DEF_ENABLED

#define  HTTPs_CFG_PERSISTENT_CONN_EN                       DEF_ENABLED

#define  HTTPs_CFG_HDR_RX_EN                                DEF_ENABLED

#define  HTTPs_CFG_HDR_TX_EN                                DEF_ENABLED

#define  HTTPs_CFG_QUERY_STR_EN                             DEF_ENABLED

#define  HTTPs_CFG_FORM_EN                                  DEF_ENABLED

#define  HTTPs_CFG_FORM_MULTIPART_EN                        DEF_ENABLED

#define  HTTPs_CFG_TOKEN_PARSE_EN                           DEF_ENABLED

#define  HTTPs_CFG_ABSOLUTE_URI_EN                          DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       DEFAULT FATAL ERROR HTML
*********************************************************************************************************
*********************************************************************************************************
*/

#define  HTTPs_CFG_HTML_DFLT_ERR_PAGE                       "<HTML><BODY><HEAD><TITLE>SYSTEM ERROR</TITLE></HEAD><H1>#{STATUS_CODE}: #{REASON_PHRASE}</H1>The operation cannot be completed.</BODY></HTML>"


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of http_server_cfg.h module include.             */
