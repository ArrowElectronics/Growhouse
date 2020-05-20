// *******************************************************************
// * af-main.h
// *
// *
// * Copyright 2007 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef SILABS_AF_MAIN_H
#define SILABS_AF_MAIN_H

#include CONFIGURATION_HEADER
#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros
#include "stack/include/ember-types.h"

#define MFG_STRING_MAX_LENGTH 16

typedef struct {
  EmberAfMessageSentFunction callback;
  uint8_t tag;
} CallbackTableEntry;

#if defined(EZSP_HOST)
bool emberAfMemoryByteCompare(const uint8_t* pointer, uint8_t count, uint8_t byteValue);
#else
bool emMemoryByteCompare(const uint8_t *bytes, uint8_t count, uint8_t target);

#define emberAfMemoryByteCompare(pointer, count, byteValue) \
  emMemoryByteCompare((pointer), (count), (byteValue))
#endif

// returnData must be MFG_STRING_MAX_LENGTH in length and
// is NOT expected to be NULL terminated (could be though)
void emberAfGetMfgString(uint8_t* returnData);

// Functions common to both SOC and Host versions of the application.
void emAfInitializeMessageSentCallbackArray(void);

EmberAfCbkeKeyEstablishmentSuite emberAfIsFullSmartEnergySecurityPresent(void);

#if defined(EZSP_HOST)
void emAfClearNetworkCache(uint8_t networkIndex);
#else
  #define emAfClearNetworkCache(index)
uint8_t emAfCopyMessageIntoRamBuffer(EmberMessageBuffer message,
                                     uint8_t *buffer,
                                     uint16_t bufLen);
#endif

#if defined EZSP_HOST
// utility for setting an EZSP config value and printing the result
EzspStatus emberAfSetEzspConfigValue(EzspConfigId configId,
                                     uint16_t value,
                                     PGM_P configIdName);

// utility for setting an EZSP policy and printing the result
EzspStatus emberAfSetEzspPolicy(EzspPolicyId policyId,
                                EzspDecisionId decisionId,
                                PGM_P policyName,
                                PGM_P decisionName);

// utility for setting an EZSP value and printing the result
EzspStatus emberAfSetEzspValue(EzspValueId valueId,
                               uint8_t valueLength,
                               uint8_t *value,
                               PGM_P valueName);

EmberStatus emberAfEzspSetSourceRoute(EmberNodeId id);

bool emberAfNcpNeedsReset(void);

#endif // EZSP_HOST

void emAfPrintStatus(PGM_P task,
                     EmberStatus status);

uint8_t emberAfGetSecurityLevel(void);
uint8_t emberAfGetKeyTableSize(void);
uint8_t emberAfGetBindingTableSize(void);
uint8_t emberAfGetAddressTableSize(void);
uint8_t emberAfGetChildTableSize(void);
uint8_t emberAfGetRouteTableSize(void);
uint8_t emberAfGetNeighborTableSize(void);
uint8_t emberAfGetStackProfile(void);
uint8_t emberAfGetSleepyMulticastConfig(void);

uint8_t emAfGetPacketBufferFreeCount(void);
uint8_t emAfGetPacketBufferTotalCount(void);

EmberStatus emberAfGetSourceRouteTableEntry(
  uint8_t index,
  EmberNodeId *destination,
  uint8_t *closerIndex);

uint8_t emberAfGetSourceRouteTableTotalSize(void);
uint8_t emberAfGetSourceRouteTableFilledSize(void);

EmberStatus emberAfGetChildData(uint8_t index,
                                EmberChildData* childData);

void emAfCliVersionCommand(void);

EmberStatus emAfPermitJoin(uint8_t duration,
                           bool broadcastMgmtPermitJoin);
void emAfStopSmartEnergyStartup(void);

bool emAfProcessZdo(EmberNodeId sender,
                    EmberApsFrame* apsFrame,
                    uint8_t* message,
                    uint16_t length);

void emAfIncomingMessageHandler(EmberIncomingMessageType type,
                                EmberApsFrame *apsFrame,
                                uint8_t lastHopLqi,
                                int8_t lastHopRssi,
                                uint16_t messageLength,
                                uint8_t *messageContents);
EmberStatus emAfSend(EmberOutgoingMessageType type,
                     uint16_t indexOrDestination,
                     EmberApsFrame *apsFrame,
                     uint8_t messageLength,
                     uint8_t *message,
                     uint8_t *messageTag,
                     EmberNodeId alias,
                     uint8_t sequence);
void emAfMessageSentHandler(EmberOutgoingMessageType type,
                            uint16_t indexOrDestination,
                            EmberApsFrame *apsFrame,
                            EmberStatus status,
                            uint16_t messageLength,
                            uint8_t *messageContents,
                            uint8_t messageTag);

void emAfStackStatusHandler(EmberStatus status);
void emAfNetworkInit(void);

// For testing purposes only, we suppress the normal call to emberNetworkInit()
// at reboot.  This allows us to call it manually later and prevent the node
// from immediately coming back up on the network after reboot.
#ifdef EMBER_AF_TC_SWAP_OUT_TEST
  #define EM_AF_NETWORK_INIT()
#else
  #define EM_AF_NETWORK_INIT() emAfNetworkInit()
#endif

#define emberAfCopyBigEndianEui64Argument emberCopyBigEndianEui64Argument
void emAfScheduleFindAndRejoinEvent(void);

extern const EmberEUI64 emberAfNullEui64;

void emberAfFormatMfgString(uint8_t* mfgString);

extern bool emberAfPrintReceivedMessages;

void emAfParseAndPrintVersion(EmberVersion versionStruct);
void emAfPrintEzspEndpointFlags(uint8_t endpoint);

// Old names
#define emberAfMoveInProgress() emberAfMoveInProgressCallback()
#define emberAfStartMove()      emberAfStartMoveCallback()
#define emberAfStopMove()       emberAfStopMoveCallback()

#endif // SILABS_AF_MAIN_H
