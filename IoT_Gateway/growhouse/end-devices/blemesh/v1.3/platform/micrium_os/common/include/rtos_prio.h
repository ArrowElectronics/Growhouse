/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Common
*
*                               (c) Copyright 2017; Silicon Laboratories Inc.
*                                           https://www.micrium.com
*********************************************************************************************************
* Licensing:
*           YOUR USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS OF A MICRIUM SOFTWARE LICENSE.
*   If you are not willing to accept the terms of an appropriate Micrium Software License, you must not
*   download or use this software for any reason.
*   Information about Micrium software licensing is available at https://www.micrium.com/licensing/
*   It is your obligation to select an appropriate license based on your intended use of the Micrium OS.
*   Unless you have executed a Micrium Commercial License, your use of the Micrium OS is limited to
*   evaluation, educational or personal non-commercial uses. The Micrium OS may not be redistributed or
*   disclosed to any third party without the written consent of Silicon Laboratories Inc.
*********************************************************************************************************
* Documentation:
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*********************************************************************************************************
* Technical Support:
*   Support is available for commercially licensed users of Micrium's software. For additional
*   information on support, you can contact info@micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                               RTOS PRIO
*
* File : rtos_prio.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _RTOS_PRIO_H_
#define  _RTOS_PRIO_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <rtos_description.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                                   DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           TASK PRIORITIES
*********************************************************************************************************
*/

                                                                /* ------------- VERY HIGH PRIORITY TASKS ------------- */
#ifndef  KERNEL_TICK_TASK_PRIO_DFLT
#define  KERNEL_TICK_TASK_PRIO_DFLT                            4u
#endif

#ifndef  KERNEL_TMR_TASK_PRIO_DFLT
#define  KERNEL_TMR_TASK_PRIO_DFLT                             5u
#endif

#ifndef  KERNEL_STAT_TASK_PRIO_DFLT
#define  KERNEL_STAT_TASK_PRIO_DFLT                            6u
#endif


                                                                /* --------------- HIGH PRIORITY TASKS ---------------- */
#ifndef  CANOPEN_CORE_TASK_PRIO_DFLT
#define  CANOPEN_CORE_TASK_PRIO_DFLT                           9u
#endif

#ifndef  USBH_HUB_TASK_PRIO_DFLT
#define  USBH_HUB_TASK_PRIO_DFLT                              10u
#endif

#ifndef  USBH_PBHCI_TASK_PRIO_DFLT
#define  USBH_PBHCI_TASK_PRIO_DFLT                            11u
#endif

#ifndef  USBD_AUDIO_RECORD_TASK_PRIO_DFLT
#define  USBD_AUDIO_RECORD_TASK_PRIO_DFLT                     12u
#endif

#ifndef  USBD_AUDIO_PLAYBACK_TASK_PRIO_DFLT
#define  USBD_AUDIO_PLAYBACK_TASK_PRIO_DFLT                   13u
#endif

#ifndef  IO_SD_CORE_TASK_PRIO_DFLT
#define  IO_SD_CORE_TASK_PRIO_DFLT                            14u
#endif


                                                                /* -------------- MEDIUM PRIORITY TASKS --------------- */
#ifndef  NET_CORE_TASK_CFG_PRIO_DFLT
#define  NET_CORE_TASK_CFG_PRIO_DFLT                          16u
#endif

#ifndef  NET_CORE_IF_WIFI_TASK_PRIO_DFLT
#define  NET_CORE_IF_WIFI_TASK_PRIO_DFLT                      17u
#endif

#ifndef  HTTP_CLIENT_TASK_PRIO_DFLT
#define  HTTP_CLIENT_TASK_PRIO_DFLT                           18u
#endif

#ifndef  MQTT_CLIENT_TASK_PRIO_DFLT
#define  MQTT_CLIENT_TASK_PRIO_DFLT                           19u
#endif

#ifndef  IPERF_TASK_PRIO_DFLT
#define  IPERF_TASK_PRIO_DFLT                                 20u
#endif

#ifndef  TELNET_SERVER_SESSION_TASK_PRIO_DFLT
#define  TELNET_SERVER_SESSION_TASK_PRIO_DFLT                 21u
#endif

#ifndef  TELNET_SERVER_SRV_TASK_PRIO_DFLT
#define  TELNET_SERVER_SRV_TASK_PRIO_DFLT                     22u
#endif

#ifndef  TFTP_SERVER_TASK_PRIO_DFLT
#define  TFTP_SERVER_TASK_PRIO_DFLT                           23u
#endif

#ifndef  CLK_TASK_CFG_PRIO_DFLT
#define  CLK_TASK_CFG_PRIO_DFLT                               24u
#endif

#ifndef  USBH_ASYNC_TASK_PRIO_DFLT
#define  USBH_ASYNC_TASK_PRIO_DFLT                            25u
#endif

#ifndef  IO_SD_ASYNC_TASK_PRIO_DFLT
#define  IO_SD_ASYNC_TASK_PRIO_DFLT                           26u
#endif


                                                                /* ---------------- LOW PRIORITY TASKS ---------------- */
#ifndef  FS_MEDIA_POLL_TASK_PRIO_DFLT
#define  FS_MEDIA_POLL_TASK_PRIO_DFLT                         27u
#endif

#ifndef  NET_CORE_SVC_TASK_CFG_PRIO_DFLT
#define  NET_CORE_SVC_TASK_CFG_PRIO_DFLT                      28u
#endif

#ifndef  USBD_HID_TMR_TASK_PRIO_DFLT
#define  USBD_HID_TMR_TASK_PRIO_DFLT                          29u
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of rtos path module include.                     */
