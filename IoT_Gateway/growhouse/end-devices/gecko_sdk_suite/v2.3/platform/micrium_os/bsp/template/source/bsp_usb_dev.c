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
*                               BOARD SUPPORT PACKAGE (BSP) - USB-DEVICE
*
* File : bsp_usb_dev.c
* Note(s) : (1) This is a template BSP file for a USB controller. The template contains BSP for USB
*               device module.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#define    BSP_USB_MODULE

#include  <common/include/lib_utils.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_utils.h>
#include  <rtos_description.h>

#ifdef  RTOS_MODULE_USB_DEV_AVAIL

#include  <usb/include/device/usbd_core.h>
#include  <drivers/usb/include/usbd_drv.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ----------------- USB DEVICE FNCTS ----------------- */
static  void  BSP_USBD_Template_Init        (USBD_DRV  *p_drv);

static  void  BSP_USBD_Template_Conn        (void);

static  void  BSP_USBD_Template_Disconn     (void);

static  void  BSP_USBD_Template_ISR_Handler (void);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           USB DEVICE CFGS
*********************************************************************************************************
*/

static  USBD_DRV  *BSP_USBD_Template_DrvPtr;                    /* USB device driver specific info.                     */


                                                                /* ---------- USB DEVICE ENDPOINTS INFO TBL ----------- */
static  USBD_DRV_EP_INFO  BSP_USBD_Template_EP_InfoTbl[] = {
    {USBD_EP_INFO_TYPE_CTRL                                                   | USBD_EP_INFO_DIR_OUT, 0u,   64u},
    {USBD_EP_INFO_TYPE_CTRL                                                   | USBD_EP_INFO_DIR_IN,  0u,   64u},
    {USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 1u, 1024u},
    {USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN,  1u, 1024u},
    {USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 2u, 1024u},
    {USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN,  2u, 1024u},
    {USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_OUT, 3u, 1024u},
    {USBD_EP_INFO_TYPE_ISOC | USBD_EP_INFO_TYPE_BULK | USBD_EP_INFO_TYPE_INTR | USBD_EP_INFO_DIR_IN,  3u, 1024u},
    {DEF_BIT_NONE                                                                                 ,   0u,    0u}
};

                                                                /* ---------- USB DEVICE DRIVER INFORMATION ----------- */
static  USBD_DRV_INFO  BSP_USBD_Template_DrvInfoPtr = {
    .BaseAddr   = 0x00000000u,                                  /* USB Controller register base address.                */
    .MemAddr    = 0x00000000u,                                  /* USB Controller deddicated mem base addr. 0 if none.  */
    .MemSize    = 0u,                                           /* USB Controller deddicated mem size. 0 if none.       */
    .Spd        = USBD_DEV_SPD_HIGH,                            /* USB Controller maximum speed.                        */
    .EP_InfoTbl = BSP_USBD_Template_EP_InfoTbl
};

                                                                /* ------------- USB DEVICE BSP STRUCTURE ------------- */
static  USBD_DRV_BSP_API   BSP_USBD_Template_BSP_API_Ptr = {
    BSP_USBD_Template_Init,
    BSP_USBD_Template_Conn,
    BSP_USBD_Template_Disconn
};

                                                                /* ----------- USB DEVICE HW INFO STRUCTURE ----------- */
const  USBD_DEV_CTRLR_HW_INFO  BSP_USBD_Template_HwInfo = {
    .DrvAPI_Ptr  =  DEF_NULL,                                   /* TODO: Insert ptr to appropriate driver API structure.*/
    .DrvInfoPtr  = &BSP_USBD_Template_DrvInfoPtr,
    .BSP_API_Ptr = &BSP_USBD_Template_BSP_API_Ptr
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       BSP_USBD_Template_Init()
*
* Description : USB device controller board-specific initialization.
*
*                   (1) Enable    USB dev ctrl registers  and bus clock.
*                   (2) Configure USB dev ctrl interrupts.
*                   (3) Disable   USB dev transceiver Pull-up resistor in D+ line.
*                   (4) Disable   USB dev transceiver clock.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_USBD_Template_Init (USBD_DRV  *p_drv)
{
    BSP_USBD_Template_DrvPtr = p_drv;

    /* TODO: Add initialization of USB controller related clk, IO, power, int, etc here.*/
}


/*
*********************************************************************************************************
*                                       BSP_USBD_Template_Conn()
*
* Description : Connect pull-up on DP.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_USBD_Template_Conn (void)
{
    /* TODO: Add code needed to be executed on a connect request.*/
}


/*
*********************************************************************************************************
*                                     BSP_USBD_Template_Disconn()
*
* Description : Disconnect pull-up on DP.
*
* Argument(s) : none.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_USBD_Template_Disconn (void)
{
    /* TODO: Add code needed to be executed on a disconnect request.*/
}


/*
*********************************************************************************************************
*                                     BSP_USBD_Template_ISR_Handler()
*
* Description : Provide p_drv pointer to the driver ISR Handler.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_USBD_Template_ISR_Handler (void)
{
    /* TODO: Add code needed to acknowledge interrupt.*/

    BSP_USBD_Template_DrvPtr->API_Ptr->ISR_Handler(BSP_USBD_Template_DrvPtr);
}
#endif
