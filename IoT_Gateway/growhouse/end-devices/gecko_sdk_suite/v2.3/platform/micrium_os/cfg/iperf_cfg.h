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
*                                          IPERF CONFIGURATION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : iperf_cfg.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _IPERF_CFG_H_
#define  _IPERF_CFG_H_

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            IPERF OPTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

#define  IPERF_CFG_BANDWIDTH_CALC_EN                        DEF_ENABLED

#define  IPERF_CFG_CPU_USAGE_MAX_CALC_EN                    DEF_ENABLED

#define  IPERF_CFG_SERVER_EN                                DEF_ENABLED

#define  IPERF_CFG_CLIENT_EN                                DEF_ENABLED

#define  IPERF_CFG_CLIENT_BIND_EN                           DEF_DISABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of iperf_cfg.h module include.                   */
