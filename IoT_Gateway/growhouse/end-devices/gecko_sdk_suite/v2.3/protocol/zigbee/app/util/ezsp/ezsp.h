// File: ezsp.h
//
// Description: Host EZSP layer. Provides functions that allow the Host
// application to send every EZSP command to the NCP. The command and response
// parameters are defined in the datasheet.
//
// Copyright 2014 Silicon Laboratories, Inc.                                *80*

#ifndef SILABS_EZSP_H
#define SILABS_EZSP_H

// Reset the NCP and initialize the serial protocol (SPI or UART). After this
// function returns EZSP_SUCCESS, the NCP has finished rebooting and is ready
// to accept a command.
EzspStatus ezspInit(void);

// For ezsp-uart, must be called before setting sleep mode and enabling
// synchronous callbacks (read via ezspCallback()).
void ezspEnableNcpSleep(bool enable);

// Returns true if there are one or more callbacks queued up on the NCP
// awaiting collection.
bool ezspCallbackPending(void);

// The sleep mode to use in the frame control of every command sent. The Host
// application can set this to the desired NCP sleep mode. Subsequent commands
// will pass this value to the NCP.
extern uint8_t ezspSleepMode;

// Wakes the NCP up from deep sleep.
void ezspWakeUp(void);

// For ezsp-uart, indicate whether the Host can sleep without disrupting the
// uart protocol. In particular the Host may not sleep unless the NCP is
// already asleep.
bool ezspOkToSleep(void);

// The Host application must call this function periodically to allow the EZSP
// layer to handle asynchronous events.
void ezspTick(void);

// The EZSP layer calls this function after sending a command while waiting for
// the response. The Host application can use this function to perform any tasks
// that do not involve the NCP.
void ezspWaitingForResponse(void);

// Callback from the EZSP layer indicating that the transaction with the NCP
// could not be completed due to a serial protocol error or that the response
// received from the NCP reported an error. The status parameter provides more
// information about the error. This function must be provided by the Host
// application.
void ezspErrorHandler(EzspStatus status);

// Cleanly close down the serial protocol (SPI or UART). After this function has
// been called, ezspInit() must be called to resume communication with the
// NCP.
void ezspClose(void);

//----------------------------------------------------------------
// Functions with special handling

EzspStatus ezspGetVersionStruct(EmberVersion* returnData);

EzspStatus ezspSetEndpointFlags(uint8_t endpoint,
                                EzspEndpointFlags flags);
EzspStatus ezspGetEndpointFlags(uint8_t endpoint,
                                EzspEndpointFlags* returnFlags);

uint8_t ezspGetSourceRouteOverhead(EmberNodeId destination);

#define ezspEnableEndpoint(endpoint) \
  ezspSetEndpointFlags((endpoint), EZSP_ENDPOINT_ENABLED)

bool emberStackIsPerformingRejoin(void);
EmberStatus emberSetMfgSecurityConfig(uint32_t magicNumber,
                                      const EmberMfgSecurityStruct* settings);
EmberStatus emberGetMfgSecurityConfig(EmberMfgSecurityStruct* settings);
EmberStatus emberStartWritingStackTokens(void);
EmberStatus emberStopWritingStackTokens(void);
EmberStatus emberSetExtendedSecurityBitmask(EmberExtendedSecurityBitmask mask);
EmberStatus emberGetExtendedSecurityBitmask(EmberExtendedSecurityBitmask* mask);
EmberStatus emberSetNodeId(EmberNodeId nodeId);
void emberSetMaximumIncomingTransferSize(uint16_t size);
void emberSetMaximumOutgoingTransferSize(uint16_t size);
void emberSetDescriptorCapability(uint8_t capability);
uint8_t emberGetLastStackZigDevRequestSequence(void);
EmberStatus emberSendUnicastNetworkKeyUpdate(EmberNodeId targetShort,
                                             EmberEUI64  targetLong,
                                             EmberKeyData* newKey);
EmberStatus emberAesHashSimple(uint8_t totalLength,
                               const uint8_t* data,
                               uint8_t* result);
uint8_t emberGetCurrentNetwork(void);
EmberStatus emberSetCurrentNetwork(uint8_t index);
uint8_t emberGetCallbackNetwork(void);

EmberStatus emberFindAndRejoinNetworkWithReason(bool haveCurrentNetworkKey,
                                                uint32_t channelMask,
                                                EmberRejoinReason reason);
EmberStatus emberFindAndRejoinNetworkWithNodeType(bool haveCurrentNetworkKey,
                                                  uint32_t channelMask,
                                                  EmberNodeType nodeType);

EmberStatus emberFindAndRejoinNetwork(bool haveCurrentNetworkKey,
                                      uint32_t channelMask);

EmberRejoinReason emberGetLastRejoinReason(void);
EmberLeaveReason emberGetLastLeaveReason(EmberNodeId* id);
uint8_t getSourceRouteOverhead(uint8_t messageLength);
EmberStatus emberSetPreinstalledCbkeData283k1(EmberPublicKey283k1Data *caPublic,
                                              EmberCertificate283k1Data *myCert,
                                              EmberPrivateKey283k1Data *myKey);
#define EZSP_MAXIMIZE_PACKET_BUFFER_COUNT 0xFF

//----------------------------------------------------------------
// ZLL methods

bool emberIsZllNetwork(void);
void emberZllGetTokenStackZllData(EmberTokTypeStackZllData *token);
void emberZllGetTokenStackZllSecurity(EmberTokTypeStackZllSecurity *token);
void emberZllSetTokenStackZllData(EmberTokTypeStackZllData *token);
void emberZllSetNonZllNetwork(void);
EmberStatus emberZllSetSecurityStateWithoutKey(const EmberZllInitialSecurityState *securityState);
EmberStatus emberZllSetInitialSecurityState(const EmberKeyData *networkKey,
                                            const EmberZllInitialSecurityState *securityState);
EmberStatus emberZllFormNetwork(EmberZllNetwork* networkInfo,
                                int8_t radioTxPower);
EmberStatus emberZllStartScan(uint32_t channelMask,
                              int8_t radioPowerForScan,
                              EmberNodeType nodeType);
EmberStatus emberZllSetRxOnWhenIdle(uint32_t durationMs);
EmberStatus emberZllJoinTarget(const EmberZllNetwork* targetNetworkInfo);
EmberStatus emberSetLogicalAndRadioChannel(uint8_t channel);
uint8_t emberGetLogicalChannel(void);
void emberZllSetRadioIdleMode(EmberRadioPowerMode mode);
void emberSetZllNodeType(EmberNodeType nodeType);
void emberSetZllAdditionalState(uint16_t state);
bool emberZllOperationInProgress(void);
bool emberZllRxOnWhenIdleGetActive(void);
uint32_t emberGetZllPrimaryChannelMask(void);
uint32_t emberGetZllSecondaryChannelMask(void);
void emberSetZllPrimaryChannelMask(uint32_t mask);
void emberSetZllSecondaryChannelMask(uint32_t mask);

//----------------------------------------------------------------
#include "rename-ezsp-functions.h"
#include "command-prototypes.h"
#endif // __EZSP_H__
