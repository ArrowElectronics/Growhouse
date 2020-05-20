/***************************************************************************//**
 * @file
 * @brief Network Ethernet BSP - Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IF_ETHER_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_def.h>
#include  <net/include/net_type.h>
#include  <net/include/net_if.h>
#include  <net/include/net_if_ether.h>
#include  <drivers/net/include/net_drv_ether.h>
#include  <drivers/net/include/net_drv_phy.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void BSP_NetEther_CfgClk(NET_IF   *p_if,
                                RTOS_ERR *p_err);

static void BSP_NetEther_CfgIntCtrl(NET_IF   *p_if,
                                    RTOS_ERR *p_err);

static void BSP_NetEther_CfgGPIO(NET_IF   *p_if,
                                 RTOS_ERR *p_err);

static CPU_INT32U BSP_NetEther_ClkFreqGet(NET_IF   *p_if,
                                          RTOS_ERR *p_err);

static void BSP_NetEther_ISR_Handler(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         INTERFACE NUMBER
 *
 * Note(s) : (1) (a) Each network controller maps to a unique network interface number.
 *
 *               (b) Network interface number variables SHOULD be initialized to 'NET_IF_NBR_NONE'.
 *******************************************************************************************************/

static NET_IF_NBR BSP_NetEther_IF_Nbr = NET_IF_NBR_NONE;

/********************************************************************************************************
 *                                  NETWORK CONTROLLER BSP INTERFACE
 *
 * Note(s) : (1) Board-support package (BSP) interface structures are used by the controller driver
 *               to call specific controller' BSP functions via function pointer instead of by name. This
 *               enables the network protocol suite to compile & operate with multiple instances of
 *               multiple controllers & drivers.
 *******************************************************************************************************/

static const NET_DEV_BSP_ETHER BSP_NetEther_BSP_API = {
  .CfgClk = BSP_NetEther_CfgClk,
  .CfgIntCtrl = BSP_NetEther_CfgIntCtrl,
  .CfgGPIO = BSP_NetEther_CfgGPIO,
  .ClkFreqGet = BSP_NetEther_ClkFreqGet
};

/********************************************************************************************************
 *                                  NETWORK CONTROLLER CONFIGURATION
 *
 * Note(s) : (1) (a) Buffer & memory sizes & alignments configured in number of octets.
 *               (b) Data bus size configured in number of bits.
 *
 *           (2) (a) All network buffer data area sizes MUST be configured greater than or equal to
 *                   NET_BUF_DATA_SIZE_MIN.
 *               (b) Large transmit buffer data area sizes MUST be configured greater than or equal to
 *                   small transmit buffer data area sizes.
 *               (c) Small transmit buffer data area sizes MAY need to be configured greater than or
 *                   equal to the specific interface's minimum packet size.
 *
 *           (3) (a) MUST configure at least one (1) large receive  buffer.
 *               (b) MUST configure at least one (1) transmit buffer, however, zero (0) large OR
 *                   zero (0) small transmit buffers MAY be configured.
 *
 *           (4) Some processors or controllers may be more efficient & may even REQUIRE that buffer data
 *               areas align to specific CPU-word/octet address boundaries in order to successfully
 *               read/write data from/to controller. Therefore, it is recommended to align controllers'
 *               buffer data areas to the processor's or controller's data bus width.
 *
 *           (5) Positive offset from base receive/transmit index, if required by controller (or driver) :
 *
 *               (a) (1) Some controller's may receive or buffer additional octets prior to the actual
 *                       received packet. Thus an offset may be required to ignore these additional octets :
 *
 *                       (A) If a controller does NOT receive or buffer any  additional octets prior to
 *                           received packets, then the default offset of '0' SHOULD be configured.
 *
 *                       (B) However, if a controller does receive or buffer additional octets prior to
 *                           received packets, then configure the controller's receive offset with the
 *                           number of additional octets.
 *
 *                   (2) Some controllers/drivers may require additional octets prior to the actual
 *                       transmit packet. Thus an offset may be required to reserve additional octets :
 *
 *                       (A) If a controller/driver does NOT require any  additional octets prior to
 *                           transmit packets, then the default offset of '0' SHOULD be configured.
 *
 *                       (B) However, if a controller/driver does require additional octets prior to
 *                           transmit packets, then configure the controller's transmit offset with the
 *                           number of additional octets.
 *
 *               (b) Since each network buffer data area allocates additional octets for its configured
 *                   offset(s), the network buffer data area size does NOT need to be increased by the
 *                   number of additional offset octets.
 *
 *           (6) Flags to configure (optional) controller features; bit-field flags logically OR'd :
 *
 *               (a) NET_DEV_CFG_FLAG_NONE           No device configuration flags selected.
 *
 *               (b) NET_DEV_CFG_FLAG_SWAP_OCTETS    Swap data octets [i.e. swap data words' high-order
 *                                                       octet(s) with data words' low-order octet(s),
 *                                                       & vice-versa] if required by device-to-CPU data
 *                                                       bus wiring &/or CPU endian word order.
 *
 *           (7) Network controllers with receive descriptors MUST configure the number of receive buffers
 *               greater than the number of receive descriptors.
 *******************************************************************************************************/

//                                                                 TODO : Modify structure according to your application needs and controller particularities.

static const NET_DEV_CFG_ETHER BSP_NetEther_CtrlrCfg = {
  .RxBufPoolType = NET_IF_MEM_TYPE_MAIN,                        // Desired receive buffer memory pool type :
                                                                // NET_IF_MEM_TYPE_MAIN : buffers allocated from main memory
                                                                // NET_IF_MEM_TYPE_DEDICATED : bufs alloc from dedicated memory
  .RxBufLargeSize = 1520u,                                      // Desired size of large receive buffers (in octets) [see Note #2].
  .RxBufLargeNbr = 20u,                                         // Desired number of large receive buffers [see Note #3a].
  .RxBufAlignOctets = 16u,                                      // Desired alignment of receive buffers (in octets) [see Note #4].
  .RxBufIxOffset = 0u,                                          // Desired offset from base receive index, if needed (in octets) [see Note #5a1].

  .TxBufPoolType = NET_IF_MEM_TYPE_MAIN,                        // Desired transmit buffer memory pool type :
                                                                // NET_IF_MEM_TYPE_MAIN : buffers allocated from main memory
                                                                // NET_IF_MEM_TYPE_DEDICATED : bufs alloc from dedicated memory
  .TxBufLargeSize = 1520u,                                      // Desired size of large transmit buffers (in octets) [see Note #2].
  .TxBufLargeNbr = 20u,                                         // Desired number of large transmit buffers [see Note #3b].
  .TxBufSmallSize = 60u,                                        // Desired size of small transmit buffers (in octets) [see Note #2].
  .TxBufSmallNbr = 10u,                                         // Desired number of small transmit buffers [see Note #3b].
  .TxBufAlignOctets = 16u,                                      // Desired alignment of transmit buffers (in octets) [see Note #4].
  .TxBufIxOffset = 0u,                                          // Desired offset from base transmit index, if needed (in octets) [see Note #5a2].

  .MemAddr = 0x00000000u,                                       // Base address of dedicated memory, if available.
  .MemSize = 0xFFFFu,                                           // Size of dedicated memory, if available (in octets).

  .Flags = NET_DEV_CFG_FLAG_NONE,                               // Desired option flags, if any (see Note #6).

  .RxDescNbr = 10u,                                             // Desired number of controller's receive descriptors (see Note #7).
  .TxDescNbr = 30u,                                             // Desired number of controller's transmit descriptors.

  .BaseAddr = 0x00000000u,                                      // Base address of controller's hardware/registers.

  .DataBusSizeNbrBits = 0u,                                     // Size of controller's data bus (in bits), if available.

  .HW_AddrStr = "00:0B:57:AA:00:72",                            // Desired MAC hardware address; may be NULL address or string ...
                                                                // ... if  address configured or set at run-time.

  .CfgExtPtr = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                      NETWORK PHY CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 TODO : Modify structure according to your hardware particularities.

static const NET_PHY_CFG_ETHER BSP_NetEther_PhyCfg = {
  .BusAddr = NET_PHY_ADDR_AUTO,                                 // Phy bus address.
  .BusMode = NET_PHY_BUS_MODE_RMII,                             // Phy bus mode.
  .Type = NET_PHY_TYPE_EXT,                                     // Phy type.
  .Spd = NET_PHY_SPD_AUTO,                                      // Auto-Negotiation determines link speed.
  .Duplex = NET_PHY_DUPLEX_AUTO,                                // Auto-Negotiation determines link duplex.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK CONTROLLER HARDWARE INFO
 *
 * Note(s) : (1) This structure is used to register the controller with the Platform Manager.
 *               See BSP_Init() in bsp.c
 *******************************************************************************************************/

//                                                                 TODO : Complete filling structure for registration of the network controller to the Platform Manager.

const NET_IF_ETHER_HW_INFO BSP_NetEther_HwInfo = {
  .DrvAPI_Ptr = DEF_NULL,                                       // TODO: Insert ptr to the ctrlr driver API structure.
  .BSP_API_Ptr = &BSP_NetEther_BSP_API,                         // PHY driver API function pointers
  .DevCfgPtr = &BSP_NetEther_CtrlrCfg,                          // Controller driver configuration structure
  .PHY_API_Ptr = DEF_NULL,                                      // TODO: Insert ptr to the Phy driver API structure.
  .PHY_CfgPtr = &BSP_NetEther_PhyCfg                            // PHY driver configuration structure
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           BSP_NetEther_CfgClk()
 *
 * Description : Configure clocks for the specified interface/controller.
 *
 * Argument(s) : p_if        Pointer to network interface to configure.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_NetEther_CfgClk(NET_IF   *p_if,
                                RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(p_err);

  //                                                               TODO Insert code to configure each network interface's/controller's clocks.
}

/********************************************************************************************************
 *                                         BSP_NetEther_CfgIntCtrl()
 *
 * Description : Configure interrupt controller for the specified interface/controller.
 *
 * Argument(s) : p_if        Pointer to network interface to configure.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) Phy interrupts NOT supported when using the Generic Ethernet Phy driver.
 *******************************************************************************************************/
static void BSP_NetEther_CfgIntCtrl(NET_IF   *p_if,
                                    RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_err);

  BSP_NetEther_IF_Nbr = p_if->Nbr;                              // Save the interface number for this controller.

  PP_UNUSED_PARAM(BSP_NetEther_ISR_Handler);                    // To prevent compiler warning. May be removed.

  //                                                               TODO Insert code to configure each network interface's/device's interrupt(s)/controller.
}

/********************************************************************************************************
 *                                          BSP_NetEther_CfgGPIO()
 *
 * Description : Configure general-purpose I/O (GPIO) for the specified interface/controller.
 *
 * Argument(s) : p_if        Pointer to network interface to configure.
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : None.
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static void BSP_NetEther_CfgGPIO(NET_IF   *p_if,
                                 RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(p_err);

  //                                                               TODO Insert code to configure each network interface's/controller's GPIO.
}

/********************************************************************************************************
 *                                         BSP_NetEther_ClkFreqGet()
 *
 * Description : Get controller clock frequency.
 *
 * Argument(s) : p_if        Pointer to network interface to get clock frequency.
 *
 *               p_err       pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : MAC controller clock divider frequency (in Hz).
 *
 * Note(s)     : None.
 *******************************************************************************************************/
static CPU_INT32U BSP_NetEther_ClkFreqGet(NET_IF   *p_if,
                                          RTOS_ERR *p_err)
{
  CPU_INT32U clk_freq;

  PP_UNUSED_PARAM(p_if);
  PP_UNUSED_PARAM(p_err);

  clk_freq = 0;                                                 // TODO Insert code to return each network interface's/controller's clock frequency.

  return (clk_freq);
}

/********************************************************************************************************
 *                                      BSP_NetEther_ISR_Handler()
 *
 * Description : BSP-level ISR handler for controller receive & transmit interrupts.
 *
 * Argument(s) : None.
 *
 * Return(s)   : None.
 *
 * Note(s)     : (1) Each controller interrupt, or set of interrupts, MUST be handled by a unique
 *                   BSP-level ISR handler which maps each specific controller interrupt to its
 *                   corresponding network interface ISR handler.
 *******************************************************************************************************/
static void BSP_NetEther_ISR_Handler(void)
{
  RTOS_ERR err;

  //                                                               TODO Insert code to handle each network interface's/controller's interrupt(s).

  NetIF_ISR_Handler(BSP_NetEther_IF_Nbr, NET_DEV_ISR_TYPE_UNKNOWN, &err);

  //                                                               TODO Insert code to clear each network interface's/controller's interrupt(s), if necessary.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IF_ETHER_AVAILL
