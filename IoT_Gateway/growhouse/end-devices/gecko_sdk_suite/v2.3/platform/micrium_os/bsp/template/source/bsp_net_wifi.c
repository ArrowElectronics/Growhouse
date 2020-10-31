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
*                            NETWORK BOARD SUPPORT PACKAGE (BSP) FUNCTIONS
*
* File : bsp_net_wifi.c
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                     DEPENDENCIES & AVAIL CHECK(S)
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IF_WIFI_AVAIL))


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <net/include/net_def.h>
#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#include  <net/include/net_if_wifi.h>
#include  <drivers/net/include/net_drv_wifi.h>

#include  <cpu/include/cpu.h>
#include  <cpu/include/cpu_int.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_Start       (NET_IF       *p_if,
                                       RTOS_ERR     *p_err);

static  void  BSP_NetWiFi_Stop        (NET_IF       *p_if,
                                       RTOS_ERR     *p_err);

static  void  BSP_NetWiFi_CfgGPIO     (NET_IF       *p_if,
                                       RTOS_ERR     *p_err);

static  void  BSP_NetWiFi_CfgInt      (NET_IF       *p_if,
                                       RTOS_ERR     *p_err);

static  void  BSP_NetWiFi_IntCtrl     (NET_IF       *p_if,
                                       CPU_BOOLEAN   en,
                                       RTOS_ERR     *p_err);

static  void  BSP_NetWiFi_ISR_Handler (void);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         INTERFACE NUMBER
*
* Note(s) : (1) (a) Each network controller maps to a unique network interface number.
*
*               (b) Network interface number variables SHOULD be initialized to 'NET_IF_NBR_NONE'.
*********************************************************************************************************
*/

static  NET_IF_NBR  BSP_NetWiFi_IF_Nbr = NET_IF_NBR_NONE;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                    NETWORK DEVICE BSP INTERFACE
*
* Note(s) : (1) Device board-support package (BSP) interface structures are used by the device driver to
*               call specific devices' BSP functions via function pointer instead of by name.  This enables
*               the network protocol suite to compile & operate with multiple instances of multiple devices
*               & drivers.
*********************************************************************************************************
*/

const  NET_DEV_BSP_WIFI  BSP_NetWiFi_BSP_API = {
    .Start      = &BSP_NetWiFi_Start,
    .Stop       = &BSP_NetWiFi_Stop,
    .CfgGPIO    = &BSP_NetWiFi_CfgGPIO,
    .CfgIntCtrl = &BSP_NetWiFi_CfgInt,
    .IntCtrl    = &BSP_NetWiFi_IntCtrl
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_NetWiFi_Start()
*
* Description : Start wireless hardware.
*
* Argument(s) : p_if    Pointer to interface to start the hardware.
*
*               p_err   Pointer to variable  that will receive the return error code from this function.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_Start (NET_IF    *p_if,
                                 RTOS_ERR  *p_err)
{

    /* TODO Insert code to start (power up) wireless spi device.   */

    PP_UNUSED_PARAM(p_if);
    PP_UNUSED_PARAM(p_err);
}


/*
*********************************************************************************************************
*                                            BSP_NetWiFi_Stop()
*
* Description : Stop wireless hardware.
*
* Argument(s) : p_if    Pointer to interface to start the hardware.
*
*               p_err   Pointer to variable  that will receive the return error code from this function.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_Stop (NET_IF    *p_if,
                                RTOS_ERR  *p_err)
{
    /* TODO Insert code to stop (power down) wireless spi device.   */

    PP_UNUSED_PARAM(p_if);
    PP_UNUSED_PARAM(p_err);
}


/*
*********************************************************************************************************
*                                          BSP_NetWiFi_CfgGPIO()
*
* Description : (1) Configure pins:
*
*                   (a) Configure SPI     pins.
*                   (b) Configure EXT ISR pin.
*                   (c) Configure PWR     pin.
*                   (d) Configure RST     pin.
*
* Argument(s) : p_if    Pointer to interface to start the hardware.
*
*               p_err   Pointer to variable  that will receive the return error code from this function.
*
* Return(s)   : None.
*
* Note(s)     : None
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_CfgGPIO (NET_IF    *p_if,
                                   RTOS_ERR  *p_err)
{
    /* TODO Insert code to configure each network interface's/device's GPIO.  */

                                                                /* ----------------- CFG EXT ISR PIN ------------------ */

                                                                /* ------------------- CFG CS PIN --------------------- */

                                                                /* ------------------- CFG PWR PIN -------------------- */
     PP_UNUSED_PARAM(p_if);
     PP_UNUSED_PARAM(p_err);
}


/*
*********************************************************************************************************
*                                       BSP_NetWiFi_CfgInt()
*
* Description : Assign external ISR to the ISR Handler.
*
* Argument(s) : p_if    Pointer to interface to start the hardware.
*
*               p_err   Pointer to variable  that will receive the return error code from this function.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_CfgInt (NET_IF   *p_if,
                                  RTOS_ERR *p_err)
{
    BSP_NetWiFi_IF_Nbr = p_if->Nbr;                             /* Configure this device's BSP instance with ...        */
                                                                /* ... specific interface number.                       */

    /* TODO Insert code to configure each network interface's/device's interrupt(s)/controller. */

                                                                /* Add EXT ISR pin to the ISR handler.                  */

    PP_UNUSED_PARAM(&BSP_NetWiFi_ISR_Handler);                  /* To prevent compiler warning. May be removed.         */
    PP_UNUSED_PARAM(p_err);
}


/*
*********************************************************************************************************
*                                        BSP_NetWiFi_IntCtrl()
*
* Description : Enable or disable external interrupt.
*
* Argument(s) : p_if    Pointer to interface to start the hardware.
*
*               en      Enable or disable the interrupt.
*
*               p_err   Pointer to variable that will receive the return error code from this function.
*
* Return(s)   : None.
*
* Note(s)     : None.
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_IntCtrl (NET_IF       *p_if,
                                   CPU_BOOLEAN   en,
                                   RTOS_ERR     *p_err)
{
    if (en == DEF_YES) {
        /* TODO Insert code to enable  the interface's/device's interrupt(s).    */
    } else if (en == DEF_NO) {
        /* TODO Insert code to disable the interface's/device's interrupt(s).    */
    }

    PP_UNUSED_PARAM(p_if);
    PP_UNUSED_PARAM(p_err);
}


/*
*********************************************************************************************************
*                                        BSP_NetWiFi_ISR_Handler()
*
* Description : BSP-level ISR handler for device receive & transmit interrupts.
*
* Argument(s) : None.
*
* Return(s)   : None.
*
* Note(s)     : (1) Each controller interrupt, or set of interrupts, MUST be handled by a unique
*                   BSP-level ISR handler which maps each specific controller interrupt to its
*                   corresponding network interface ISR handler.
*********************************************************************************************************
*/

static  void  BSP_NetWiFi_ISR_Handler (void)
{
    RTOS_ERR  err;


    RTOS_ERR_SET(err, RTOS_ERR_NONE);
    NetIF_ISR_Handler(BSP_NetWiFi_IF_Nbr, NET_DEV_ISR_TYPE_UNKNOWN, &err);

    /* TODO Insert code to clear WiFi device interrupt(s), if necessary.    */
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                   DEPENDENCIES & AVAIL CHECK(S) END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif  /* RTOS_MODULE_NET_IF_WIFI_AVAIL */
