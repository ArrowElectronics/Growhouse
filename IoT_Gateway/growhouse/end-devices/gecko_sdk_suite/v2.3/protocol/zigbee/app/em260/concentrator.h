/*
 * File: concentrator.h
 * Description: Concentrator support for the NCP.
 *
 * Author(s): Maurizio Nanni, maurizio.nanni@ember.com
 *
 * Copyright 2012 by Ember Corporation. All rights reserved.
 */

#ifndef SILABS_APP_EM260_CONCENTRATOR_H
#define SILABS_APP_EM260_CONCENTRATOR_H

#include "app/util/ezsp/ezsp-frame-utilities.h"

extern EmberEventControl emberAfPluginConcentratorSupportLibraryEventControl;
void emberAfPluginConcentratorSupportLibraryEventHandler(void);

void emConcentratorSupportRouteErrorHandler(EmberStatus status, EmberNodeId nodeId);

void emConcentratorSupportDeliveryErrorHandler(EmberOutgoingMessageType type,
                                               EmberStatus status);

EmberStatus emberAfPluginEzspZigbeeProSetConcentratorCommandCallback(bool on,
                                                                     uint16_t concentratorType,
                                                                     uint16_t minTime,
                                                                     uint16_t maxTime,
                                                                     uint8_t routeErrorThreshold,
                                                                     uint8_t deliveryFailureThreshold,
                                                                     uint8_t maxHops);

#define ADDRESS_DISCOVERY_DELAY_QS 2
#endif // SILABS_APP_EM260_CONCENTRATOR_H
