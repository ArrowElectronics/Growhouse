/***************************************************************************//**
 * @file
 * @brief Net Winpcap BSP
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

#include  <net/include/net_if.h>
#include  <net/include/net_def.h>
#include  <net/source/cmd/net_cmd_args_parser_priv.h>
#include  <net/source/cmd/net_cmd_priv.h>

#include  <net/include/net_if_ether.h>
#include  <drivers/net/include/net_drv_ether.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                    NETWORK DEVICE CONFIGURATION
 *******************************************************************************************************/

NET_DEV_CFG_EXT_WINPCAP NetDev_Cfg_Ext_WinPcap = {
  .TaskCfg = {
    .Prio = 12u,
    .StkSizeElements = 1024,
    .StkPtr = DEF_NULL
  },
  .PC_IFNbr = 0u
};

NET_DEV_CFG_ETHER NetDev_Cfg_WinPcap_Dflt = {
  .RxBufPoolType = NET_IF_MEM_TYPE_MAIN,                        // Desired receive  buffer memory pool type :
                                                                // NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory
                                                                // NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory
  .RxBufLargeSize = 1518u,                                      // Desired size      of device's large receive  buffers (in octets) [see Note #2].
  .RxBufLargeNbr = 100u,                                        // Desired number    of device's large receive  buffers             [see Note #3a].
  .RxBufAlignOctets = 4u,                                       // Desired alignment of device's       receive  buffers (in octets) [see Note #4].
  .RxBufIxOffset = 0u,                                          // Desired offset from base receive  index, if needed   (in octets) [see Note #5a].

  .TxBufPoolType = NET_IF_MEM_TYPE_MAIN,                        // Desired transmit buffer memory pool type :
                                                                // NET_IF_MEM_TYPE_MAIN        buffers allocated from main memory
                                                                // NET_IF_MEM_TYPE_DEDICATED   buffers allocated from (device's) dedicated memory
  .TxBufLargeSize = 1518u,                                      // Desired size      of device's large transmit buffers (in octets) [see Note #2].
  .TxBufLargeNbr = 100u,                                        // Desired number    of device's large transmit buffers             [see Note #3b].
  .TxBufSmallSize = 60u,                                        // Desired size      of device's small transmit buffers (in octets) [see Note #2].
  .TxBufSmallNbr = 25u,                                         // Desired number    of device's small transmit buffers             [see Note #3b].
  .TxBufAlignOctets = 4u,                                       // Desired alignment of device's       transmit buffers (in octets) [see Note #4].
  .TxBufIxOffset = 0u,                                          // Desired offset from base transmit index, if needed   (in octets) [see Note #5b].

  .MemAddr = 0x00000000u,                                       // Base address   of dedicated memory, if available.
  .MemSize = 0u,                                                // Size           of dedicated memory, if available (in octets).

  .Flags = NET_DEV_CFG_FLAG_NONE,                               // Desired option flags, if any (see Note #6).

  .RxDescNbr = 1u,                                              // Desired number of device's receive  descriptors (see Note #7).
  .TxDescNbr = 1u,                                              // Desired number of device's transmit descriptors.

  .BaseAddr = 0xFFFFFFFFu,                                      // Base address   of device's hardware/registers.

  .DataBusSizeNbrBits = 0u,                                     // Size           of device's data bus (in bits), if available.

  .HW_AddrStr = "00:0B:57:AA:00:72",                            // Desired device hardware address; may be NULL address or string ...
                                                                // ... if  device hardware address configured or set at run-time.

  .CfgExtPtr = &NetDev_Cfg_Ext_WinPcap,
};

/********************************************************************************************************
 *                                      NETWORK IF HARDWARE INFO
 *******************************************************************************************************/

const NET_IF_ETHER_HW_INFO BSP_Winpcap_Ether = {
  .DrvAPI_Ptr = &NetDev_API_WinPcap,
  .BSP_API_Ptr = DEF_NULL,
  .PHY_API_Ptr = DEF_NULL,
  .DevCfgPtr = &NetDev_Cfg_WinPcap_Dflt,
  .PHY_CfgPtr = DEF_NULL
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IF_ETHER_AVAIL
