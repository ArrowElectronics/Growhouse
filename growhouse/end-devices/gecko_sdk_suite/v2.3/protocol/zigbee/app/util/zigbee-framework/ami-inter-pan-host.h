/**
 * @file ami-inter-pan-host.h
 * @brief Utilities for sending and receiving ZigBee AMI InterPAN messages.
 * See @ref message for documentation.
 *
 * @deprecated The ami-inter-pan library is deprecated and will be removed in a
 * future release.  Similar functionality is available in the Inter-PAN plugin
 * in Application Framework.
 *
 * <!--Copyright 2008 by Ember Corporation. All rights reserved.         *80*-->
 */

/** @addtogroup message
 *
 * See also ami-inter-pan-host.h for source code.
 * @{
 */

#ifndef AMI_INTER_PAN_HOST_H
#define AMI_INTER_PAN_HOST_H

/**
 * @brief The three types of inter-PAN messages.  The values are actually the
 * corresponding APS frame controls.
 * 0x03 is the special interPAN message type.  Unicast mode is 0x00,
 * broadcast mode is 0x08, and multicast mode is 0x0C.
 */
#define INTER_PAN_UNICAST   0x03
#define INTER_PAN_BROADCAST 0x0B
#define INTER_PAN_MULTICAST 0x0F

// Frame control, sequence, dest PAN ID, dest, source PAN ID, source.
#define MAX_INTER_PAN_MAC_SIZE (2 + 1 + 2 + 8 + 2 + 8)
//Short form has a short destination.

// NWK stub frame has two control bytes.
#define STUB_NWK_SIZE 2
#define STUB_NWK_FRAME_CONTROL 0x000B

// APS frame control, group ID, cluster ID, profile ID
#define MAX_STUB_APS_SIZE (1 + 2 + 2 + 2)

// Short form has no group ID.
#define MAX_INTER_PAN_HEADER_SIZE \
  (MAX_INTER_PAN_MAC_SIZE + STUB_NWK_SIZE + MAX_STUB_APS_SIZE)

/**
 * @brief A struct for keeping track of all of the interpan
 * header info.
 */
typedef struct {
  uint8_t messageType;            // one of the INTER_PAN_...CAST values

  // MAC addressing
  // For outgoing messages this is the destination.  For incoming messages
  // it is the source, which always has a long address.
  uint16_t panId;
  bool hasLongAddress;       // always true for incoming messages
  EmberNodeId shortAddress;
  EmberEUI64 longAddress;

  // APS data
  uint16_t profileId;
  uint16_t clusterId;
  uint16_t groupId;               // only used for INTER_PAN_MULTICAST
} InterPanHeader;

/**
 * @brief Create an interpan message.
 * message needs to have enough space for the message contents.
 * Upon return, the return value will be the length of the message, or
 * 0 in case of error
 */
uint8_t makeInterPanMessage(InterPanHeader *headerData,
                            uint8_t *message,
                            uint8_t maxLength,
                            uint8_t *payload,
                            uint8_t payloadLength);

/**
 * @brief This is meant to be called on the message passed
 * to emberMacPassthroughMessageHandler(...).  The header is parsed and
 * the various fields are written to the InterPanHeader.  The returned
 * value is the offset of the payload in the message, or 0 if the
 * message is not a correctly formed AMI interPAN message.
 */
uint8_t parseInterPanMessage(uint8_t *message,
                             uint8_t messageLength,
                             InterPanHeader *headerData);

#endif // AMI_INTER_PAN_HOST_H

/** @} END addtogroup */
