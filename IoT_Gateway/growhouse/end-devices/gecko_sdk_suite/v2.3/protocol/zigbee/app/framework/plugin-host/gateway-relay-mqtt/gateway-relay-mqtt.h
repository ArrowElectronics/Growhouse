// Copyright 2017 Silicon Laboratories, Inc.                                *80*

#ifndef SILABS_CHANNEL_MQTT_H
#define SILABS_CHANNEL_MQTT_H

/** @brief Sets the heart beat interval.
 *
 * Sets the MQTT heart beat interval in milliseconds.
 *
 * @param intervalMs The heart beat interval in milliseconds.
 */
void emberPluginGatewayRelayMqttSetHeartBeat(uint16_t intervalMs);
/** @brief Function to publish device-table devices list
 *         on "gw/+/devices" mqtt topic
 */
void gatewayRelayMqttDeviceTablePrint();
#endif
