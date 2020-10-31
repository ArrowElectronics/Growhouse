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
*                                           RTOS DESCRIPTION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : rtos_description.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _RTOS_DESCRIPTION_H_
#define  _RTOS_DESCRIPTION_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_opt_def.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       ENVIRONMENT DESCRIPTION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTOS_CPU_SEL                                       RTOS_CPU_SEL_SILABS_GECKO_AUTO

#define  RTOS_TOOLCHAIN_SEL                                 RTOS_TOOLCHAIN_AUTO

#define  RTOS_INT_CONTROLLER_SEL                            RTOS_INT_CONTROLLER_AUTO


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       RTOS MODULES DESCRIPTION
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ---------------------- KERNEL ---------------------- */
#define  RTOS_MODULE_KERNEL_AVAIL

                                                                /* ---------------------- COMMON ---------------------- */
#define  RTOS_MODULE_COMMON_CLK_AVAIL
#define  RTOS_MODULE_COMMON_SHELL_AVAIL

                                                                /* ------------------------ IO ------------------------ */
#define  RTOS_MODULE_IO_AVAIL

#define  RTOS_MODULE_IO_SERIAL_AVAIL
#define  RTOS_MODULE_IO_SERIAL_SPI_AVAIL

#define  RTOS_MODULE_IO_SD_AVAIL

                                                                /* ------------------- FILE SYSTEM -------------------- */
#define  RTOS_MODULE_FS_AVAIL

#define  RTOS_MODULE_FS_STORAGE_NAND_AVAIL
#define  RTOS_MODULE_FS_STORAGE_NOR_AVAIL
#define  RTOS_MODULE_FS_STORAGE_RAM_DISK_AVAIL
#define  RTOS_MODULE_FS_STORAGE_SCSI_AVAIL
#define  RTOS_MODULE_FS_STORAGE_SD_CARD_AVAIL
#define  RTOS_MODULE_FS_STORAGE_SD_SPI_AVAIL

                                                                /* --------------------- NETWORK ---------------------- */
#define  RTOS_MODULE_NET_AVAIL

#define  RTOS_MODULE_NET_IF_ETHER_AVAIL
#define  RTOS_MODULE_NET_IF_WIFI_AVAIL
#define  RTOS_MODULE_NET_SSL_TLS_AVAIL
#define  RTOS_MODULE_NET_SSL_TLS_MOCANA_NANOSSL_AVAIL
#define  RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL
#define  RTOS_MODULE_NET_HTTP_CLIENT_AVAIL
#define  RTOS_MODULE_NET_HTTP_SERVER_AVAIL
#define  RTOS_MODULE_NET_TELNET_SERVER_AVAIL
#define  RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
#define  RTOS_MODULE_NET_SMTP_CLIENT_AVAIL
#define  RTOS_MODULE_NET_SNTP_CLIENT_AVAIL
#define  RTOS_MODULE_NET_IPERF_AVAIL
#define  RTOS_MODULE_NET_FTP_CLIENT_AVAIL
#define  RTOS_MODULE_NET_TFTP_CLIENT_AVAIL
#define  RTOS_MODULE_NET_TFTP_SERVER_AVAIL

                                                                /* -------------------- USB DEVICE -------------------- */
#define  RTOS_MODULE_USB_DEV_AVAIL

#define  RTOS_MODULE_USB_DEV_AUDIO_AVAIL
#define  RTOS_MODULE_USB_DEV_CDC_AVAIL
#define  RTOS_MODULE_USB_DEV_ACM_AVAIL
#define  RTOS_MODULE_USB_DEV_EEM_AVAIL
#define  RTOS_MODULE_USB_DEV_HID_AVAIL
#define  RTOS_MODULE_USB_DEV_MSC_AVAIL
#define  RTOS_MODULE_USB_DEV_VENDOR_AVAIL

                                                                /* --------------------- USB HOST --------------------- */
#define  RTOS_MODULE_USB_HOST_AVAIL

#define  RTOS_MODULE_USB_HOST_PBHCI_AVAIL
#define  RTOS_MODULE_USB_HOST_CDC_AVAIL
#define  RTOS_MODULE_USB_HOST_ACM_AVAIL
#define  RTOS_MODULE_USB_HOST_AOAP_AVAIL
#define  RTOS_MODULE_USB_HOST_USB2SER_AVAIL
#define  RTOS_MODULE_USB_HOST_USB2SER_FTDI_AVAIL
#define  RTOS_MODULE_USB_HOST_USB2SER_SILABS_AVAIL
#define  RTOS_MODULE_USB_HOST_USB2SER_PROLIFIC_AVAIL
#define  RTOS_MODULE_USB_HOST_MSC_AVAIL
#define  RTOS_MODULE_USB_HOST_HID_AVAIL

                                                                /* --------------------- CAN BUS ---------------------- */
#define  RTOS_MODULE_CAN_BUS_AVAIL

                                                                /* --------------------- CANOPEN ---------------------- */
#define  RTOS_MODULE_CANOPEN_AVAIL


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of rtos_description.h module include.            */
