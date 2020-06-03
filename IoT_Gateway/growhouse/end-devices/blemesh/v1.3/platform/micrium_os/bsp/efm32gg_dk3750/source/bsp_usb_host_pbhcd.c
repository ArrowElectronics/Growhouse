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
*                                            SILICON LABS
*                                           EFM32GG-DK3750
*
* File : bsp_usb_host_pbhcd.c
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

#include  "../include/bsp_int.h"
#include  "em_cmu.h"
#include  "em_gpio.h"


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  EFM32_REG_USBH_BASE_ADDR           (CPU_INT32U)(0x400C4000u)
#define  EFM32_REG_USBH_ROUTE             (*(CPU_REG32 *)(EFM32_REG_USBH_BASE_ADDR + 0x18u))

                                                                /* -------- EFM32 USB I/O ROUTE REGISTER BITS --------- */
#define  EFM32_USBH_ROUTE_BIT_DMPUPEN             DEF_BIT_02
#define  EFM32_USBH_ROUTE_BIT_VBUSENPEN           DEF_BIT_01
#define  EFM32_USBH_ROUTE_BIT_PHYPEN              DEF_BIT_00


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ------------------ USB HOST FNCTS ------------------ */
static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_Init  (USBH_PBHCD_ISR_HANDLE_FNCT   isr_fnct,
                                                    USBH_PBHCI_HC_DRV           *p_pbhci_hc_drv);

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_ClkCfg(void);

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_IO_Cfg(void);

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_IntCfg(void);

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_PwrCfg(void);

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_Start (void);

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_Stop  (void);

static  void         BSP_USBH_EFM32GG_IntHandler  (void);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

static  USBH_PBHCD_ISR_HANDLE_FNCT   BSP_USBH_EFM32GG_ISR_Fnct; /* Function to call when int occurs.                    */
static  USBH_PBHCI_HC_DRV           *BSP_USBH_EFM32GG_DrvPtr;   /* Pointer to data to pass to ISR.                      */

                                                                /* ------------------ BSP API STRUCT ------------------ */
static  USBH_PBHCI_BSP_API  BSP_USBH_EFM32GG_DK3750_BSP_API_Ptr = {
    BSP_USBH_EFM32GG_DK3750_Init,
    BSP_USBH_EFM32GG_DK3750_ClkCfg,
    BSP_USBH_EFM32GG_DK3750_IO_Cfg,
    BSP_USBH_EFM32GG_DK3750_IntCfg,
    BSP_USBH_EFM32GG_DK3750_PwrCfg,
    BSP_USBH_EFM32GG_DK3750_Start,
    BSP_USBH_EFM32GG_DK3750_Stop,
    DEF_NULL
};

                                                                /* ----------------- PIPE INFO TABLE ------------------ */
USBH_PBHCI_PIPE_INFO  BSP_USBH_EFM32GG_PipeInfoTbl[] =
{
    {USBH_PIPE_INFO_TYPE_CTRL                                                       | USBH_PIPE_INFO_DIR_BOTH, 0u,   64u},
    {USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 1u,  512u},
    {USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 2u,  512u},
    {USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 3u,  512u},
    {USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 4u,  512u},
    {USBH_PIPE_INFO_TYPE_CTRL | USBH_PIPE_INFO_TYPE_BULK | USBH_PIPE_INFO_TYPE_INTR | USBH_PIPE_INFO_DIR_BOTH, 5u,  512u},
    {DEF_BIT_NONE                                                                                            , 0u,    0u}
};

                                                                /* ------------------ HW INFO STRUCT ------------------ */
const  USBH_PBHCI_HC_HW_INFO  BSP_USBH_EFM32GG_PBHCI_HW_Info = {
    .HW_Info.BaseAddr                   =  0x40100000u,         /* USB Controller register base address.                */
    .HW_Info.RH_Spd                     =  USBH_DEV_SPD_FULL,   /* Maximum speed of USB controller.                     */
    .HW_Info.HC_Type                    =  USBH_HC_TYPE_PIPE,
    .HW_Info.HW_DescDedicatedMemInfoPtr =  DEF_NULL,            /* Insert info on dedicated memory if any.              */
    .HW_Info.DataBufDedicatedMemInfoPtr =  DEF_NULL,

    .DrvAPI_Ptr                         = &USBH_PBHCD_API_EFM32_OTG_FS_FIFO, /* Ptr to appropriate driver API structure.*/
    .BSP_API_Ptr                        = &BSP_USBH_EFM32GG_DK3750_BSP_API_Ptr,
    .PipeInfoTblPtr                     =  BSP_USBH_EFM32GG_PipeInfoTbl
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
*                                   BSP_USBH_EFM32GG_DK3750_Init()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_Init (USBH_PBHCD_ISR_HANDLE_FNCT   isr_fnct,
                                                   USBH_PBHCI_HC_DRV           *p_hc_drv)
{
    BSP_USBH_EFM32GG_ISR_Fnct = isr_fnct;
    BSP_USBH_EFM32GG_DrvPtr   = p_hc_drv;

    CPU_IntSrcDis(BSP_INT_ID_USB);                              /* Disable USB interrupt                                */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                  BSP_USBH_EFM32GG_DK3750_ClkCfg()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_ClkCfg (void)
{
    if (CMU_ClockSelectGet(cmuClock_HF) != cmuSelect_HFXO) {
        CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    }

                                                                /* Enable USB clock                                     */
    CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_USB |
                         CMU_HFCORECLKEN0_USBC;

    CMU_ClockSelectSet(cmuClock_USBC, cmuSelect_HFCLK);

    DEF_BIT_SET(EFM32_REG_USBH_ROUTE, (EFM32_USBH_ROUTE_BIT_VBUSENPEN |
                                       EFM32_USBH_ROUTE_BIT_PHYPEN));

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                  BSP_USBH_EFM32GG_DK3750_IO_Cfg()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_IO_Cfg (void)
{
    CMU_ClockEnable(cmuClock_GPIO, true );
    GPIO_PinModeSet(gpioPortF, 5, gpioModePushPull, 0 );        /* Enable VBUSEN pin                                    */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                  BSP_USBH_EFM32GG_DK3750_IntCfg()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_IntCfg (void)
{
                                                                /* Set handler interrupt request into interrupt channel */
    CPU_INT_SRC_HANDLER_SET_KA(BSP_INT_ID_USB,
                               BSP_USBH_EFM32GG_IntHandler);

    CPU_IntSrcDis(BSP_INT_ID_USB);                              /* Disable USB interrupt                                */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                  BSP_USBH_EFM32GG_DK3750_PwrCfg()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_PwrCfg (void)
{
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                   BSP_USBH_EFM32GG_DK3750_Start()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_Start (void)
{
    CPU_IntSrcEn(BSP_INT_ID_USB);                               /* Enable USB interrupt                                 */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                   BSP_USBH_EFM32GG_DK3750_Stop()
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

static  CPU_BOOLEAN  BSP_USBH_EFM32GG_DK3750_Stop (void)
{
    CPU_IntSrcDis(BSP_INT_ID_USB);                              /* Disable USB interrupt                                */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                    BSP_USBH_EFM32GG_IntHandler()
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

static  void  BSP_USBH_EFM32GG_IntHandler (void)
{
    BSP_USBH_EFM32GG_ISR_Fnct(BSP_USBH_EFM32GG_DrvPtr);
}

#endif
