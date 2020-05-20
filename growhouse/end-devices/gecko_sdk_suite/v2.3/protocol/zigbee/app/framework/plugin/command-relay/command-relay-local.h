// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#ifndef SILABS_COMMAND_RELAY_LOCAL_H
#define SILABS_COMMAND_RELAY_LOCAL_H

void emAfPluginCommandRelayAddBind(EmberEUI64 inEui,
                                   uint8_t inEndpoint,
                                   EmberEUI64 outEui,
                                   uint8_t outEndpoint);

void emAfPluginCommandRelayPrint(void);
void emAfPluginCommandRelayRemove(EmberAfPluginCommandRelayDeviceEndpoint* inDeviceEndpoint,
                                  EmberAfPluginCommandRelayDeviceEndpoint* outDeviceEndpoint);
void emAfPluginCommandRelayRemoveDeviceByEui64(EmberEUI64 eui64);

#endif //__COMMAND_RELAY_LOCAL_H
