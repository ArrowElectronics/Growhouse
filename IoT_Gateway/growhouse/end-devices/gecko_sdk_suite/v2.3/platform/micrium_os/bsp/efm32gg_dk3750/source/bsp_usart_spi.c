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
*                                          USART SPI MODULE BSP
*
* File : bsp_usart_spi.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <kernel/include/os.h>
#include  <io/include/serial.h>
#include  <drivers/io/include/serial_drv.h>

#include  <platform/emdrv/spidrv/inc/spidrv.h>
#include  <bsp/siliconlabs/efm32gg_dk3750/source/bsp_mcu.h>
#include  <em_gpio.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                        LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiInit        (      SERIAL_CTRLR_ISR_HANDLE_FNCT   isr_fnct,
                                                             SERIAL_DRV                    *p_ser_drv);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiClkEn       (void);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiIO_Cfg      (void);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiIntCfg      (void);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiPwrCfg      (void);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiStart       (void);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiStop        (void);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiSlaveSel    (const  SERIAL_SLAVE_INFO             *p_slave_info);

static  CPU_BOOLEAN  BSP_Serial_USART0_SpiSlaveDesel  (const  SERIAL_SLAVE_INFO             *p_slave_info);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               BUS 0
*********************************************************************************************************
*/

const  SERIAL_CTRLR_BSP_API  BSP_Serial_USART0_SPI_API = {
    .Init           = BSP_Serial_USART0_SpiInit,
    .ClkEn          = BSP_Serial_USART0_SpiClkEn,
    .IO_Cfg         = BSP_Serial_USART0_SpiIO_Cfg,
    .IntCfg         = BSP_Serial_USART0_SpiIntCfg,
    .PwrCfg         = BSP_Serial_USART0_SpiPwrCfg,
    .Start          = BSP_Serial_USART0_SpiStart,
    .Stop           = BSP_Serial_USART0_SpiStop,
    .SlaveSel       = BSP_Serial_USART0_SpiSlaveSel,
    .SlaveDesel     = BSP_Serial_USART0_SpiSlaveDesel,
    .ClkFreqGet     = DEF_NULL,
    .DMA_API_Ptr    = DEF_NULL,
    .BSP_API_ExtPtr = DEF_NULL
};

const  SERIAL_CTRLR_HW_INFO_EXT_SILICONLABS  BSP_Serial_USART0_SPI_ExtHwInfo = {
    .PortLocationRx  = 1u,
    .PortLocationTx  = 1u,
    .PortLocationClk = 1u,
    .PortLocationCs  = 1u
};

const  SERIAL_CTRLR_DRV_INFO  BSP_Serial_SiliconLabs_Bus0_DrvInfo = {
    .HW_Info.SupportedMode =  SERIAL_CTRLR_MODE_SPI,
    .HW_Info.BaseAddr      =  0x4000C000u,
    .HW_Info.InfoExtPtr    = &BSP_Serial_USART0_SPI_ExtHwInfo,
    .BSP_API_Ptr           = &BSP_Serial_USART0_SPI_API,
    .DrvAPI_Ptr            = &Serial_CtrlrDrv_API_SiliconLabsGeckoSDK
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
*                                      BSP_Serial_USART0_SpiInit()
*
* Description : Initialize BSP.
*
* Argument(s) : isr_fnct    Driver ISR function to call.
*
*               p_ser_drv   Argument to pass to driver ISR handler.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiInit (SERIAL_CTRLR_ISR_HANDLE_FNCT   isr_fnct,
                                        SERIAL_DRV                    *p_ser_drv)
{
    PP_UNUSED_PARAM(isr_fnct);
    PP_UNUSED_PARAM(p_ser_drv);

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                      BSP_Serial_USART0_SpiClkEn()
*
* Description :Configures controller clock.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiClkEn ()
{
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                     BSP_Serial_USART0_SpiIO_Cfg()
*
* Description : Configures controller IO.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Return(s)   : $$$$ Add return value description.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiIO_Cfg ()
{
    BSP_PeripheralAccess(BSP_MICROSD, 1);

    GPIO_PinModeSet(gpioPortE, 4u, gpioModePushPull, 1u);

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                     BSP_Serial_USART0_SpiIntCfg()
*
* Description : Configures controller interrupts.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiIntCfg ()
{
    CPU_IntSrcHandlerSetKA(0u + 16u, DMA_IRQHandler);
    CPU_IntSrcEn(0u + 16u);

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                     BSP_Serial_USART0_SpiPwrCfg()
*
* Description : Configures controller power.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiPwrCfg ()
{
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                      BSP_Serial_USART0_SpiStart()
*
* Description : Starts serial bus.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiStart ()
{
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                      BSP_Serial_USART0_SpiStop()
*
* Description : Stops serial bus.
*
* Argument(s) : None.
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiStop ()
{
    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                    BSP_Serial_USART0_SpiSlaveSel()
*
* Description : Selects a slave.
*
* Argument(s) : p_slave_info    $$$$ Add description for 'p_slave_info'
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiSlaveSel (const  SERIAL_SLAVE_INFO  *p_slave_info)
{
    PP_UNUSED_PARAM(p_slave_info);

    GPIO->P[gpioPortE].DOUTCLR = DEF_BIT_04;

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                   BSP_Serial_USART0_SpiSlaveDesel()
*
* Description : Deselects a slave.
*
* Argument(s) : p_slave_info    $$$$ Add description for 'p_slave_info'
*
* Return(s)   : DEF_OK,   If call successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : None.
*********************************************************************************************************
*/

CPU_BOOLEAN  BSP_Serial_USART0_SpiSlaveDesel (const  SERIAL_SLAVE_INFO  *p_slave_info)
{
    PP_UNUSED_PARAM(p_slave_info);

    GPIO->P[gpioPortE].DOUTSET = DEF_BIT_04;

    return (DEF_OK);
}
