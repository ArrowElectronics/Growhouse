/***************************************************************************//**
 * @file
 * @brief Network Configuration - Configuration Template File
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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_CFG_H_
#define  _NET_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                  NETWORK OPTIMIZATION CONFIGURATION
 *
 * Note(s) : (1) TCP-IP code may call optimized assembly functions. Optimized assembly files/functions
 *               must be included in the project to be enabled.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CFG_OPTIMIZE_ASM_EN                            DEF_DISABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                                     NETWORK DEBUG CONFIGURATION
 *
 * Note(s) : (1) Configure NET_DBG_CFG_MEM_CLR_EN to enable/disable the network protocol suite from
 *               clearing internal data structure memory buffers; a convenient feature while debugging.
 *
 *           (2) Configure NET_CTR_CFG_STAT_EN to enable/disable network protocol suite statistics
 *               counters.
 *
 *           (3) Configure NET_CTR_CFG_ERR_EN  to enable/disable network protocol suite error counters.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_DBG_CFG_MEM_CLR_EN                             DEF_DISABLED

#define  NET_CTR_CFG_STAT_EN                                DEF_DISABLED

#define  NET_CTR_CFG_ERR_EN                                 DEF_DISABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                                 NETWORK STATISTIC POOL CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_STAT_POOL_BUF_EN                               DEF_DISABLED

#define  NET_STAT_POOL_ARP_EN                               DEF_DISABLED

#define  NET_STAT_POOL_NDP_EN                               DEF_DISABLED

#define  NET_STAT_POOL_IGMP_EN                              DEF_DISABLED

#define  NET_STAT_POOL_MLDP_EN                              DEF_DISABLED

#define  NET_STAT_POOL_TMR_EN                               DEF_DISABLED

#define  NET_STAT_POOL_SOCK_EN                              DEF_DISABLED

#define  NET_STAT_POOL_CONN_EN                              DEF_DISABLED

#define  NET_STAT_POOL_TCP_CONN_EN                          DEF_DISABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                                NETWORK INTERFACE LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_IF_CFG_MAX_NBR_IF                              2

#define  NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS                   1u

#define  NET_IF_CFG_LOOPBACK_EN                             DEF_DISABLED

#define  NET_IF_CFG_WAIT_SETUP_READY_EN                     DEF_ENABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                                NETWORK INTERFACE LAYER CONFIGURATION
 *
 * Note(s) : (1) Address resolution protocol ONLY required for IPv4.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_ARP_CFG_CACHE_NBR                              5u

/********************************************************************************************************
 ********************************************************************************************************
 *                           NEIGHBOR DISCOVERY PROTOCOL LAYER CONFIGURATION
 *
 * Note(s) : (1) Neighbor Discovery Protocol ONLY required for IPv6.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_NDP_CFG_CACHE_NBR                              5u

#define  NET_NDP_CFG_DEST_NBR                               5u

#define  NET_NDP_CFG_PREFIX_NBR                             5u

#define  NET_NDP_CFG_ROUTER_NBR                             3u

/********************************************************************************************************
 ********************************************************************************************************
 *                            INTERNET PROTOCOL LAYER VERSION CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_IPv4_CFG_EN                                    DEF_ENABLED

#define  NET_IPv4_CFG_LINK_LOCAL_EN                         DEF_ENABLED

#define  NET_IPv4_CFG_IF_MAX_NBR_ADDR                       2u

#define  NET_IPv6_CFG_EN                                    DEF_ENABLED

#define  NET_IPv6_CFG_ADDR_AUTO_CFG_EN                      DEF_ENABLED

#define  NET_IPv6_CFG_DAD_EN                                DEF_ENABLED

#define  NET_IPv6_CFG_IF_MAX_NBR_ADDR                       4u

/********************************************************************************************************
 ********************************************************************************************************
 *                  INTERNET GROUP MANAGEMENT PROTOCOL(MULTICAST) LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_MCAST_CFG_IPv4_RX_EN                           DEF_ENABLED

#define  NET_MCAST_CFG_IPv4_TX_EN                           DEF_ENABLED

#define  NET_MCAST_CFG_HOST_GRP_NBR_MAX                     4u

/********************************************************************************************************
 ********************************************************************************************************
 *                                  NETWORK SOCKET LAYER CONFIGURATION
 *
 * Note(s) : (1) The maximum accept queue size represents the number of connection that can be queued by
 *               the stack before being accepted. For a TCP server when a connection is queued, it means
 *               that the SYN, ACK packet has been sent back, so the remote host can start transmitting
 *               data once the connection is queued and the stack will queue up all data received until
 *               the connection is accepted and the data is read.
 *
 *           (2) Receive and transmit queue size MUST be properly configured to optimize performance.
 *
 *               (a) It represents the number of bytes that can be queued by one socket. It's important
 *                   that all socket are not able to queue more data than what the device can hold in its
 *                   buffers.
 *
 *               (b) The size should be also a multiple of the maximum segment size (MSS) to optimize
 *                   performance. UDP MSS is 1470 and TCP MSS is 1460.
 *
 *               (c) RX and TX queue size can be reduce at runtime using socket option API.
 *
 *               (d) Window calculation example:
 *                       Number of TCP connection  : 2
 *                       Number of UDP connection  : 0
 *                       Number of RX large buffer : 10
 *                       Number of TX Large buffer : 6
 *                       Number of TX small buffer : 2
 *                       Size of RX large buffer   : 1518
 *                       Size of TX large buffer   : 1518
 *                       Size of TX small buffer   : 60
 *                       TCP MSS RX                = 1460
 *                       TCP MSS TX large buffer   = 1460
 *                       TCP MSS TX small buffer   = 0
 *                       Maximum receive  window   = (10 * 1460)           = 14600 bytes
 *                       Maximum transmit window   = (6  * 1460) + (2 * 0) = 8760  bytes
 *                       RX window size per socket = (14600 / 2)           =  7300 bytes
 *                       TX window size per socket = (8760  / 2)           =  4380 bytes
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_SOCK_CFG_SOCK_NBR_UDP                          2u

#define  NET_SOCK_CFG_SOCK_NBR_TCP                          5u

#define  NET_SOCK_CFG_SEL_EN                                DEF_ENABLED

#define  NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX                2u

#define  NET_SOCK_CFG_RX_Q_SIZE_OCTET                       4096u

#define  NET_SOCK_CFG_TX_Q_SIZE_OCTET                       4096u

/********************************************************************************************************
 ********************************************************************************************************
 *                          TRANSMISSION CONTROL PROTOCOL LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_TCP_CFG_EN                                     DEF_ENABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                              USER DATAGRAM PROTOCOL LAYER CONFIGURATION
 *
 * Note(s) : (1) Configure NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN to enable/disable discarding of UDP
 *               datagrams received with NO computed checksum:
 *                   When ENABLED,  ALL UDP datagrams received without a checksum are discarded.
 *                   When DISABLED, ALL UDP datagrams received without a checksum are flagged so that
 *                   application(s) may handle &/or discard.
 *
 *           (2) Configure NET_UDP_CFG_TX_CHK_SUM_EN to enable/disable transmitting UDP datagrams with
 *               checksums :
 *                   When ENABLED,  ALL UDP datagrams are transmitted with a computed checksum.
 *                   When DISABLED, ALL UDP datagrams are transmitted without a computed checksum.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN                  DEF_DISABLED

#define  NET_UDP_CFG_TX_CHK_SUM_EN                          DEF_DISABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                           NETWORK SECURITY MANAGER CONFIGURATION (SSL/TLS)
 *
 * Note(s) : (1) The network security layer can be enabled ONLY if the application project contains a
 *               secure module supported by Micrium OS Net such as:
 *                   - NanoSSL provided by Mocana.
 *                   - wolfSSL (formerly CyaSSL) provided by wolfSSL.
 *                   - mbed TLS.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_SECURE_CFG_MAX_NBR_SOCK_SERVER                 2u

#define  NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT                 2u

#define  NET_SECURE_CFG_MAX_CERT_LEN                        2048u

#define  NET_SECURE_CFG_MAX_KEY_LEN                         2048u

#define  NET_SECURE_CFG_MAX_NBR_CA                          1u

#define  NET_SECURE_CFG_MAX_CA_CERT_LEN                     2048u

/********************************************************************************************************
 ********************************************************************************************************
 *                                    NETWORK MODULES CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_DHCP_CLIENT_CFG_MODULE_EN                      DEF_ENABLED

#define  NET_DNS_CLIENT_CFG_MODULE_EN                       DEF_ENABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                              INTERFACE CHECKSUM OFFLOAD CONFIGURATION
 *
 * Note(s) : (1) A checksum offload configuration can be enabled only if all your interfaces support
 *               the specific checksum offload option.
 *
 *           (2) By default, a driver should enable all checksum offload options it supports.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_IPV4_CFG_CHK_SUM_OFFLOAD_RX_EN                 DEF_ENABLED

#define  NET_IPV4_CFG_CHK_SUM_OFFLOAD_TX_EN                 DEF_ENABLED

#define  NET_ICMP_CFG_CHK_SUM_OFFLOAD_RX_EN                 DEF_DISABLED

#define  NET_ICMP_CFG_CHK_SUM_OFFLOAD_TX_EN                 DEF_DISABLED

#define  NET_UDP_CFG_CHK_SUM_OFFLOAD_RX_EN                  DEF_ENABLED

#define  NET_UDP_CFG_CHK_SUM_OFFLOAD_TX_EN                  DEF_ENABLED

#define  NET_TCP_CFG_CHK_SUM_OFFLOAD_RX_EN                  DEF_ENABLED

#define  NET_TCP_CFG_CHK_SUM_OFFLOAD_TX_EN                  DEF_ENABLED

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of net_cfg.h module include.
