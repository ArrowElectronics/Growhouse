// *****************************************************************************
// * service-discovery.h
// *
// * Interface for initiating and processing ZDO service discovery
// * (match descriptor) requests and response.
// *
// * Copyright 2008 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#ifndef SILABS_SERVICE_DISCOVERY_H
#define SILABS_SERVICE_DISCOVERY_H

#include "../include/af.h"

// Internal Functions
void emAfServiceDiscoveryComplete(void);
void emAfServiceDiscoveryMatched(EmberNodeId nodeId,
                                 uint8_t endpointListLength,
                                 uint8_t* endpointList);

// Platform specific call to send a match descriptor request.
EmberStatus emAfSendMatchDescriptor(EmberNodeId target,
                                    EmberAfProfileId profileId,
                                    EmberAfClusterId clusterId,
                                    bool serverCluster);

bool emAfServiceDiscoveryIncoming(EmberNodeId sender,
                                  EmberApsFrame* apsFrame,
                                  const uint8_t* message,
                                  uint16_t length);

extern EmberEventControl emAfServiceDiscoveryEventControls[];
void emAfServiceDiscoveryComplete0(void);
void emAfServiceDiscoveryComplete1(void);
void emAfServiceDiscoveryComplete2(void);
void emAfServiceDiscoveryComplete3(void);

#if EMBER_SUPPORTED_NETWORKS == 1
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc NWK 0",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS \
  { &emAfServiceDiscoveryEventControls[0], emAfServiceDiscoveryComplete0 },
#elif EMBER_SUPPORTED_NETWORKS == 2
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc NWK 0",                             \
  "Svc Disc NWK 1",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                    \
  { &emAfServiceDiscoveryEventControls[0], emAfServiceDiscoveryComplete0 }, \
  { &emAfServiceDiscoveryEventControls[1], emAfServiceDiscoveryComplete1 },
#elif EMBER_SUPPORTED_NETWORKS == 3
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc NWK 0",                             \
  "Svc Disc NWK 1",                             \
  "Svc Disc NWK 2",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                    \
  { &emAfServiceDiscoveryEventControls[0], emAfServiceDiscoveryComplete0 }, \
  { &emAfServiceDiscoveryEventControls[1], emAfServiceDiscoveryComplete1 }, \
  { &emAfServiceDiscoveryEventControls[2], emAfServiceDiscoveryComplete2 },
#elif EMBER_SUPPORTED_NETWORKS == 4
  #define EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS \
  "Svc Disc NWK 0",                             \
  "Svc Disc NWK 1",                             \
  "Svc Disc NWK 2",                             \
  "Svc Disc NWK 3",
  #define EM_AF_SERVICE_DISCOVERY_EVENTS                                    \
  { &emAfServiceDiscoveryEventControls[0], emAfServiceDiscoveryComplete0 }, \
  { &emAfServiceDiscoveryEventControls[1], emAfServiceDiscoveryComplete1 }, \
  { &emAfServiceDiscoveryEventControls[2], emAfServiceDiscoveryComplete2 }, \
  { &emAfServiceDiscoveryEventControls[3], emAfServiceDiscoveryComplete3 },
#else
  #error
#endif

#endif // SILABS_SERVICE_DISCOVERY_H
