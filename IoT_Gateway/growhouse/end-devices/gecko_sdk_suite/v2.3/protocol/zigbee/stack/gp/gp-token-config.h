/*
 * File: gp-token-config.h
 * Description: Zigbee GP token definitions used by the stack.
 *
 * Author(s): Jeffrey Rosenberger, jeffrey.rosenberger@silabs.com
 *
 * Copyright 2015 Silicon Laboratories, Inc.                                *80*
 */
#if defined(DEFINETYPES)
//#if 1

typedef struct {
  uint8_t networkIndex;
  uint8_t nodeCapabilities;
  int8_t radioPower;
  uint8_t baseChannel;
  uint16_t localNodeId;
  uint16_t localPanId;
  uint16_t vendorId;
  uint8_t vendorString[7];
} tokTypeStackGpData;

typedef struct {
  uint8_t status;
  uint32_t options;
  //EmberGpAddress gpd;
  uint8_t gpAddress[8];
  uint8_t endpoint;
  //uint16_t assignedAlias;
  uint8_t securityOptions;
  uint8_t gpdKey[16];
  //EmberGpSinkListEntry sinkList[2];
  uint8_t sinkType[2];
  uint8_t sinkEUI[2][8];
  //uint16_t sinkNodeId[2];
} tokTypeStackGpProxyTableEntry;

typedef struct {
  uint8_t status;
  uint16_t options;
  //EmberGpAddress gpd;
  uint8_t gpAddress[8];
  uint8_t endpoint;
  //uint16_t assignedAlias;
  uint8_t securityOptions;
  uint8_t gpdKey[16];
  uint8_t sinkType[2];
  uint16_t groupList[2][2];
  uint32_t securityFrameCounter; // incomming FC fo rthe gpd
} tokTypeStackGpSinkTableEntry;

typedef uint32_t tokTypeGPDIncomingFC;

#endif //DEFINETYPES

#ifdef DEFINETOKENS

DEFINE_BASIC_TOKEN(STACK_GP_DATA,
                   tokTypeStackGpData,
{
  0xFF,                                                 // network index
  0xFF,                                                 // node capabilities
  0xFF,                                                 // radio power
  0xFF,                                                 // base channel
  0xFFFF,                                               // local node ID
  0xFFFF,                                               // local PAN ID
  0xFFFF,                                               // vendor ID
  { 0xFF, 0xFF, 0xFF, 0xFF,                             // vendor string
    0xFF, 0xFF, 0xFF },
})
DEFINE_INDEXED_TOKEN(STACK_GP_PROXY_TABLE,
                     tokTypeStackGpProxyTableEntry,
                     EMBER_GP_PROXY_TABLE_TOKEN_SIZE,
{
  0xFF,                                                 // status
  0xFFFFFFFF,                                           // options
  { 0xFF, 0xFF, 0xFF, 0xFF,                             // GPD address
    0xFF, 0xFF, 0xFF, 0xFF },
  0xFF,                                                 // endpoint
  //  0xFFFF,                         // assignedAlias
  0xFF,                                                 //security options
  { 0xFF, 0xFF, 0xFF, 0xFF,                             // GPD key
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF },
  { 0xFF, 0xFF },                                       //sinkType
  {
    { 0xFF, 0xFF, 0xFF, 0xFF,                               // sinkEUI
      0xFF, 0xFF, 0xFF, 0xFF },
    { 0xFF, 0xFF, 0xFF, 0xFF,                               // sinkEUI
      0xFF, 0xFF, 0xFF, 0xFF },
  },
  //                    {0xFFFF,                             //sink nodeID
  //                    0xFFFF
  //                    },
})

DEFINE_INDEXED_TOKEN(STACK_GP_SINK_TABLE,
                     tokTypeStackGpSinkTableEntry,
                     EMBER_GP_SINK_TABLE_TOKEN_SIZE,
{
  0xFF,                                                 // status
  0xFFFF,                                               // options
  { 0xFF, 0xFF, 0xFF, 0xFF,                             // GPD address
    0xFF, 0xFF, 0xFF, 0xFF },
  0xFF,                                                 // endpoint
  0xFF,                                                 //security options
  { 0xFF, 0xFF, 0xFF, 0xFF,                             // GPD key
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF },
  { 0xFF, 0xFF },                                       //sinkType
  {
    { 0xFFFF, 0xFFFF },                         // GroupList
    { 0xFFFF, 0xFFFF },                         // GroupList
  },
  0xFFFFFFFF,                                   // incoming FC for gpd
})

DEFINE_INDEXED_TOKEN(STACK_GP_INCOMING_FC,
                     tokTypeGPDIncomingFC,
                     EMBER_GP_INCOMING_FC_TOKEN_TABLE_SIZE,
                     { 0xFFFFFFFF })

#endif
