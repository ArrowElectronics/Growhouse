/** @file ezsp-callbacks.c
 * @brief Convenience stubs for little-used EZSP callbacks.
 *
 * <!--Copyright 2006 by Ember Corporation. All rights reserved.         *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"

// *****************************************
// Convenience Stubs
// *****************************************

#ifndef EZSP_APPLICATION_HAS_WAITING_FOR_RESPONSE
void ezspWaitingForResponse(void)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_NO_CALLBACKS
void ezspNoCallbacks(void)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_TIMER_HANDLER
void ezspTimerHandler(uint8_t timerId)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_DEBUG_HANDLER
void ezspDebugHandler(uint8_t messageLength,
                      uint8_t *messageContents)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_CHILD_JOIN_HANDLER
void ezspChildJoinHandler(uint8_t index,
                          bool joining,
                          EmberNodeId childId,
                          EmberEUI64 childEui64,
                          EmberNodeType childType)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_TRUST_CENTER_JOIN_HANDLER
void ezspTrustCenterJoinHandler(EmberNodeId newNodeId,
                                EmberEUI64 newNodeEui64,
                                EmberDeviceUpdate status,
                                EmberJoinDecision policyDecision,
                                EmberNodeId parentOfNewNode)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ZIGBEE_KEY_ESTABLISHMENT_HANDLER
void ezspZigbeeKeyEstablishmentHandler(EmberEUI64 partner, EmberKeyStatus status)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_SWITCH_NETWORK_KEY_HANDLER
void ezspSwitchNetworkKeyHandler(uint8_t sequenceNumber)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_REMOTE_BINDING_HANDLER
void ezspRemoteSetBindingHandler(EmberBindingTableEntry *entry,
                                 uint8_t index,
                                 EmberStatus policyDecision)
{
}

void ezspRemoteDeleteBindingHandler(uint8_t index,
                                    EmberStatus policyDecision)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_POLL_COMPLETE_HANDLER
void ezspPollCompleteHandler(EmberStatus status)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_POLL_HANDLER
void ezspPollHandler(EmberNodeId childId)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ENERGY_SCAN_RESULT_HANDLER
void ezspEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ROUTE_RECORD_HANDLER
void ezspIncomingRouteRecordHandler(EmberNodeId source,
                                    EmberEUI64 sourceEui,
                                    uint8_t lastHopLqi,
                                    int8_t lastHopRssi,
                                    uint8_t relayCount,
                                    uint8_t *relayList)
{
}
// update the source route table on the host side.
void ezspChangeSourceRouteHandler(EmberNodeId newChildId,
                                  EmberNodeId newParentId,
                                  bool ourChild)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_BUTTON_HANDLER
void halButtonIsr(uint8_t button, uint8_t state)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_INCOMING_SENDER_EUI64_HANDLER
void ezspIncomingSenderEui64Handler(EmberEUI64 senderEui64)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ID_CONFLICT_HANDLER
void ezspIdConflictHandler(EmberNodeId id)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER
void ezspIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                              EmberEUI64 longId,
                                              uint8_t cost)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_INCOMING_ROUTE_ERROR_HANDLER
void ezspIncomingRouteErrorHandler(EmberStatus status, EmberNodeId target)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_BOOTLOADER_HANDLER
void ezspIncomingBootloadMessageHandler(EmberEUI64 longId,
                                        uint8_t lastHopLqi,
                                        int8_t lastHopRssi,
                                        uint8_t messageLength,
                                        uint8_t *messageContents)
{
}

void ezspBootloadTransmitCompleteHandler(EmberStatus status,
                                         uint8_t messageLength,
                                         uint8_t *messageContents)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_MAC_PASSTHROUGH_HANDLER
void ezspMacPassthroughMessageHandler(uint8_t messageType,
                                      uint8_t lastHopLqi,
                                      int8_t lastHopRssi,
                                      uint8_t messageLength,
                                      uint8_t *messageContents)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_MAC_FILTER_MATCH_HANDLER
void ezspMacFilterMatchMessageHandler(uint8_t filterIndexMatch,
                                      uint8_t legacyPassthroughType,
                                      uint8_t lastHopLqi,
                                      int8_t lastHopRssi,
                                      uint8_t messageLength,
                                      uint8_t *messageContents)
{
  ezspMacPassthroughMessageHandler(legacyPassthroughType,
                                   lastHopLqi,
                                   lastHopRssi,
                                   messageLength,
                                   messageContents);
}
#endif

#ifndef EZSP_APPLICATION_HAS_MFGLIB_HANDLER
void ezspMfglibRxHandler(
  uint8_t linkQuality,
  int8_t rssi,
  uint8_t packetLength,
  uint8_t *packetContents)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_RAW_HANDLER
void ezspRawTransmitCompleteHandler(EmberStatus status)
{
}
#endif

// Certificate Based Key Exchange (CBKE)
#ifndef EZSP_APPLICATION_HAS_GENERATE_CBKE_KEYS_HANDLER
void ezspGenerateCbkeKeysHandler(EmberStatus status,
                                 EmberPublicKeyData* ephemeralPublicKey)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_CALCULATE_SMACS_HANDLER
void ezspCalculateSmacsHandler(EmberStatus status,
                               EmberSmacData* initiatorSmac,
                               EmberSmacData* responderSmac)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_GENERATE_CBKE_KEYS_HANDLER_283K1
void ezspGenerateCbkeKeysHandler283k1(EmberStatus status,
                                      EmberPublicKey283k1Data* ephemeralPublicKey)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_CALCULATE_SMACS_HANDLER_283K1
void ezspCalculateSmacsHandler283k1(EmberStatus status,
                                    EmberSmacData* initiatorSmac,
                                    EmberSmacData* responderSmac)
{
}
#endif

// Elliptical Cryptography Digital Signature Algorithm (ECDSA)
#ifndef EZSP_APPLICATION_HAS_DSA_SIGN_HANDLER
void ezspDsaSignHandler(EmberStatus status,
                        uint8_t messageLength,
                        uint8_t* messageContents)
{
}
#endif

// Elliptical Cryptography Digital Signature Verification
#ifndef EZSP_APPLICATION_HAS_DSA_VERIFY_HANDLER
void ezspDsaVerifyHandler(EmberStatus status)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_FRAGMENT_SOURCE_ROUTE_HANDLER
EmberStatus ezspFragmentSourceRouteHandler(void)
{
  return EMBER_SUCCESS;
}
#endif

#ifndef EZSP_APPLICATION_HAS_CUSTOM_FRAME_HANDLER
void ezspCustomFrameHandler(uint8_t payloadLength, uint8_t* payload)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_STACK_TOKEN_CHANGED_HANDLER
void ezspStackTokenChangedHandler(uint16_t tokenAddress)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ZLL_NETWORK_FOUND_HANDLER
void ezspZllNetworkFoundHandler(const EmberZllNetwork* networkInfo,
                                const EmberZllDeviceInfoRecord* deviceInfo)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ZLL_SCAN_COMPLETE_HANDLER
void ezspZllScanCompleteHandler(EmberStatus status)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ZLL_ADDRESS_ASSIGNMENT_HANDLER
void ezspZllAddressAssignmentHandler(const EmberZllAddressAssignment* addressInfo)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_ZLL_TOUCH_LINK_TARGET_HANDLER
void ezspZllTouchLinkTargetHandler(const EmberZllNetwork* networkInfo)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER
void ezspCounterRolloverHandler(EmberCounterType type)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_GPEP_INCOMING_MESSAGE_HANDLER
void ezspGpepIncomingMessageHandler(
  // XXXAn EmberStatus value of EMBER_SUCCESS if the pairing process
  // succeeded and a pairing link has been established.
  EmberStatus status,
  // The index of the entry the pairing table corresponding to the pairing
  // link that was established during the pairing process.
  uint8_t gpdLink,
  // stuff
  uint8_t sequenceNumber,
  // The vendor information of the peer device. This parameter is non-NULL
  // only if the status parameter is EMBER_SUCCESS.
  EmberGpAddress *addr,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  EmberGpSecurityLevel gpdfSecurityLevel,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  EmberGpKeyType gpdfSecurityKeyType,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  bool autoCommissioning,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  bool rxAfterTx,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  uint32_t gpdSecurityFrameCounter,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  uint8_t gpdCommandId,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  uint32_t mic,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  uint8_t gpdCommandPayloadLength,
  // The application information of the peer device. This parameter is
  // non-NULL only if the status parameter is EMBER_SUCCESS.
  uint8_t *gpdCommandPayload)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_DGP_SENT_HANDLER
void ezspDGpSentHandler(EmberStatus status, uint8_t gpepHandle)
{
}
#endif

#ifndef EZSP_APPLICATION_HAS_DUTY_CYCLE_HANDLER
void ezspDutyCycleHandler(uint8_t channelPage,
                          uint8_t channel,
                          EmberDutyCycleState state,
                          uint8_t totalDevices,
                          EmberPerDeviceDutyCycle *arrayOfDeviceDutyCycles)
{
}
#endif
