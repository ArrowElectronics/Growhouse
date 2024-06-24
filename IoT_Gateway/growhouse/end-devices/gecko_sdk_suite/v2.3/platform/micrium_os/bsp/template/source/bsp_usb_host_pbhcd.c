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
*                             BOARD SUPPORT PACKAGE (BSP) - USB-HOST PBHCD
*
* File : bsp_usb_host_pbhcd.c
* Note(s) : (1) This is a template BSP file for a USB controller. The template contains BSP for USB Host
*               module and is inteded to be used with a Pipe Based Host Controller Driver (PBHCD).
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

#if  defined(RTOS_MODULE_USB_HOST_AVAIL) && \
     defined(RTOS_MODULE_USB_HOST_PBHCI_AVAIL)

#include  <usb/include/host/usbh_core.h>
#include  <drivers/usb/include/usbh_pbhcd.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ------------------ USB HOST FNCTS ------------------ */
static  CPU_BOOLEAN  BSP_USBH_Template_Init        (USBH_PBHCD_ISR_HANDLE_FNCT   isr_fnct,
                                                    USBH_PBHCI_HC_DRV           *p_pbhci_hc_drv);

static  CPU_BOOLEAN  BSP_USBH_Template_ClkCfg      (void);

static  CPU_BOOLEAN  BSP_USBH_Template_IO_Cfg      (void);

static  CPU_BOOLEAN  BSP_USBH_Template_IntCfg      (void);

static  CPU_BOOLEAN  BSP_USBH_Template_PwrCfg      (void);

static  CPU_BOOLEAN  BSP_USBH_Template_Start       (void);

static  CPU_BOOLEAN  BSP_USBH_Template_Stop        (void);

static  void         BSP_USBH_Template_ISR_Handler (void);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

static  USBH_PBHCD_ISR_HANDLE_FNCT   BSP_USBH_Template_ISR_Fnct;/* Function to call when int occurs.                    */
static  USBH_PBHCI_HC_DRV           *BSP_USBH_Template_DrvPtr;  /* Pointer to data to pass to ISR.                      */

                                                                /* ------------------ BSP API STRUCT ------------------ */
static  USBH_PBHCI_BSP_API  BSP_USBH_Template_BSP_API_Ptr = {
    BSP_USBH_Template_Init,
    BSP_USBH_Template_ClkCfg,
    BSP_USBH_Template_IO_Cfg,
    BSP_USBH_Template_IntCfg,
    BSP_USBH_Template_PwrCfg,
    BSP_USBH_Template_Start,
    BSP_USBH_Template_Stop,
    DEF_NULL
};

                                                                /* ----------------- PIPE INFO TABLE ------------------ */
USBH_PBHCI_PIPE_INFO  BSP_USBH_Template_PipeInfoTbl[] =
{
    {USBH_PIPE_INFO_TYPE_CTRL                            | USBH_PIPE_INFO_DIR_BOTH, 0u,  64u,  64u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 1u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 2u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 3u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 4u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 5u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 6u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 7u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 8u, 512u, 512u},
    {USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 9u, 512u, 512u},
    {DEF_BIT_NONE                                                                 , 0u,   0u,   0u}
};

                                                                /* ------------------ HW INFO STRUCT ------------------ */
const  USBH_PBHCI_HC_HW_INFO  BSP_USBH_TemplatePBHCI_HW_Info = {/* TODO: Fill this structure.                           */
    .HW_Info.BaseAddr                   =  0x00000000u,         /* TODO: Specify USB Controller register base address.  */
    .HW_Info.RH_Spd                     =  USBH_DEV_SPD_HIGH,   /* TODO: Specify maximum speed of USB controller.       */
    .HW_Info.HC_Type                    =  USBH_HC_TYPE_PIPE,
    .HW_Info.HW_DescDedicatedMemInfoPtr =  DEF_NULL,            /* TODO: Insert info on dedicated memory if any.        */
    .HW_Info.DataBufDedicatedMemInfoPtr =  DEF_NULL,

    .DrvAPI_Ptr                         =  DEF_NULL,            /* TODO: Insert ptr to appropriate driver API structure.*/
    .BSP_API_Ptr                        = &BSP_USBH_Template_BSP_API_Ptr,
    .PipeInfoTblPtr                     =  BSP_USBH_Template_PipeInfoTbl
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
*                                         BSP_USBH_Template_Init()
*
* Description : Initializes USB host BSP.
*
* Argument(s) : isr_fnct    Interrupt service rooutine to call when an interruption occurs.
*
*               p_hc_drv    Pointer to host controller driver information to pass to isr_fnct.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_Init (USBH_PBHCD_ISR_HANDLE_FNCT   isr_fnct,
                                             USBH_PBHCI_HC_DRV           *p_hc_drv)
{
    BSP_USBH_Template_ISR_Fnct = isr_fnct;
    BSP_USBH_Template_DrvPtr   = p_hc_drv;

    /* TODO: Add init instructions related to USB controller.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_USBH_Template_ClkCfg()
*
* Description : Initializes USB host controller's clock.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_ClkCfg (void)
{
    /* TODO: Perform hardware cfg related to USB controller's clock.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_USBH_Template_IO_Cfg()
*
* Description : Initializes USB host controller's IOs.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_IO_Cfg (void)
{
    /* TODO: Perform hardware cfg related to USB controller's I/Os.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_USBH_Template_IntCfg()
*
* Description : Initializes USB host controller's interrupts.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_IntCfg (void)
{
    /* TODO: Perform hardware cfg related to USB controller's interrupts.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_USBH_Template_PwrCfg()
*
* Description : Initializes USB host controller's power.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_PwrCfg (void)
{
    /* TODO: Perform hardware cfg related to USB controller's power.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_USBH_Template_Start()
*
* Description : Starts USB host BSP.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_Start (void)
{
    /* TODO: Perform any operation needed when USB controller is started.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         BSP_USBH_Template_Stop()
*
* Description : Stops USB host BSP.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_USBH_Template_Stop (void)
{
    /* TODO: Perform any operation needed when USB controller is stoped.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                     BSP_USBH_Template_ISR_Handler()
*
* Description : USB host controller interrupt service routine.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_USBH_Template_ISR_Handler (void)
{
    BSP_USBH_Template_ISR_Fnct(BSP_USBH_Template_DrvPtr);
}

#endif
