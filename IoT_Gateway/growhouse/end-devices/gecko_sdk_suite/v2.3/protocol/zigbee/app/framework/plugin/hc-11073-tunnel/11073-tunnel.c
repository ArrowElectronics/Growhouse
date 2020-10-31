// Copyright 2007 - 2012 by Ember Corporation. All rights reserved.
//
//

#include "../../include/af.h"
#include "../../util/common.h"
#include "11073-tunnel.h"

/** @brief Transfer A P D U
 *
 *
 * @param apdu   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterTransferAPDUCallback(uint8_t* apdu)
{
  return false;
}

/** @brief Connect Request
 *
 *
 * @param connectControl   Ver.: always
 * @param idleTimeout   Ver.: always
 * @param managerTarget   Ver.: always
 * @param managerEndpoint   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterConnectRequestCallback(uint8_t connectControl,
                                                             uint16_t idleTimeout,
                                                             uint8_t* managerTarget,
                                                             uint8_t managerEndpoint)
{
  bool connected = false;
  bool preemptible = false;
  EmberAfStatus status;

  // Check to see if we are already connected by looking at connected attribute
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                      (uint8_t *)&connected,
                                      1);

  // if we are already connected send back connection status ALREADY_CONNECTED
  if (connected) {
    emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
      EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_ALREADY_CONNECTED);
    emberAfSendResponse();
    return true;
  }

  // if not already connected copy attributes
  connected = true;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                       (uint8_t *)&connected,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  preemptible = connectControl
                & EMBER_ZCL_11073_CONNECT_REQUEST_CONNECT_CONTROL_PREEMPTIBLE;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE,
                                       (uint8_t *)&preemptible,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_IDLE_TIMEOUT,
                                       (uint8_t*)&idleTimeout,
                                       ZCL_INT16U_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET,
                                       (uint8_t*)managerTarget,
                                       ZCL_IEEE_ADDRESS_ATTRIBUTE_TYPE);

  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_MANAGER_ENDPOINT,
                                       &managerEndpoint,
                                       ZCL_INT8U_ATTRIBUTE_TYPE);

  // if idle timer other than 0xffff, set timer to disconnect, reset timer when
  // rx data

  // Generate conection status connected back to manager
  emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
    EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_CONNECTED);
  emberAfSendResponse();

  return true;
}

/** @brief Disconnect Request
 *
 *
 * @param managerIEEEAddress   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterDisconnectRequestCallback(uint8_t* managerIEEEAddress)
{
  bool connected = false;
  EmberEUI64 currentManager;
  bool preemptible;
  EmberAfStatus status;

  // check to see if already connected
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                      (uint8_t *)&connected,
                                      1);

  // if not currently connected, generate connection status DISCONNECTED
  if (!connected) {
    emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
      EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_DISCONNECTED);
    emberAfSendResponse();
    return true;
  }

  // if is connected, is ieee address same or is pre-emptible set to true?
  status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                      CLUSTER_ID_11073_TUNNEL,
                                      ATTRIBUTE_11073_TUNNEL_PREEMPTIBLE,
                                      (uint8_t *)&preemptible,
                                      1);

  if (!preemptible) {
    status = emberAfReadServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                        CLUSTER_ID_11073_TUNNEL,
                                        ATTRIBUTE_11073_TUNNEL_MANAGER_TARGET,
                                        (uint8_t*)&currentManager,
                                        EUI64_SIZE);
    if (MEMCOMPARE(&currentManager, managerIEEEAddress, EUI64_SIZE) != 0) {
      emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
        EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_NOT_AUTHORIZED);
      emberAfSendResponse();
      return true;
    }
  }

  // Set attribute to disconnected
  connected = false;
  status = emberAfWriteServerAttribute(HC_11073_TUNNEL_ENDPOINT,
                                       CLUSTER_ID_11073_TUNNEL,
                                       ATTRIBUTE_11073_TUNNEL_CONNECTED,
                                       (uint8_t *)&connected,
                                       ZCL_BOOLEAN_ATTRIBUTE_TYPE);

  // If it is authorized, then we can disconnect.Within 12 seconds device must send
  // DISCONNECTED notification to the manager device. Connected attribute set to
  // false to manager.
  emberAfFillCommand11073ProtocolTunnelClusterConnectStatusNotification(
    EMBER_ZCL_11073_TUNNEL_CONNECTION_STATUS_DISCONNECTED);
  emberAfSendResponse();
  return true;

  // Send another DISCONNECTED connection event to sender of message. (may be same
  // as manager, may be some other device).

  return false;
}

/** @brief Connect Status Notification
 *
 *
 * @param connectStatus   Ver.: always
 */
bool emberAf11073ProtocolTunnelClusterConnectStatusNotificationCallback(uint8_t connectStatus)
{
  return false;
}
