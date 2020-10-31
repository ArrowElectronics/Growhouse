// *******************************************************************
// * ota.h
// *
// *
// * Copyright 2009 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************
enum {
  OTA_USE_OFFSET_TIME,
  OTA_USE_UTC_TIME
};
#define OTA_UPGRADE_END_RESPONSE_RUN_NOW  0x00

#define MFG_ID_WILD_CARD        0xFFFF
#define IMAGE_TYPE_WILD_CARD    0xFFFF
#define IMAGE_TYPE_SECURITY     0xFFC0
#define IMAGE_TYPE_CONFIG       0xFFC1
#define IMAGE_TYPE_LOG          0xFFC2
#define FILE_VERSION_WILD_CARD  0xFFFFFFFF

#define ZIGBEE_2006_STACK_VERSION 0x0000
#define ZIGBEE_2007_STACK_VERSION 0x0001
#define ZIGBEE_PRO_STACK_VERSION  0x0002

// These apply to the field control of the Over-the-air messages
#define OTA_FIELD_CONTROL_HW_VERSION_PRESENT_BIT        0x01  // QueryNextImage
#define OTA_FIELD_CONTROL_NODE_EUI64_PRESENT_BIT        0x01  // ImgBlockRequest
#define OTA_FIELD_CONTROL_MIN_BLOCK_REQUEST_PRESENT_BIT 0x02  // ImgBlockRequest

// OTA Upgrade status as defined by OTA cluster spec.
enum {
  OTA_UPGRADE_STATUS_NORMAL,
  OTA_UPGRADE_STATUS_DOWNLOAD_IN_PROGRESS,
  OTA_UPGRADE_STATUS_DOWNLOAD_COMPLETE,
  OTA_UPGRADE_STATUS_WAIT,
  OTA_UPGRADE_STATUS_COUNTDOWN,
  OTA_UPGRADE_STATUS_WAIT_FOR_MORE,
  OTA_UPGRADE_STATUS_WAIT_TO_UPGRADE_VIA_OUT_OF_BAND,
};

#define OTA_TAG_UPGRADE_IMAGE                   0x0000
#define OTA_TAG_ECDSA_SIGNATURE                 0x0001
#define OTA_TAG_ECDSA_SIGNING_CERTIFICATE       0x0002
#define OTA_TAG_IMAGE_INTEGRITY_CODE            0x0003 // unused
#define OTA_TAG_PICTURE_DATA                    0x0004 // unused
#define OTA_TAG_ECDSA_SIGNATURE_283K1           0x0005
#define OTA_TAG_ECDSA_SIGNING_CERTIFICATE_283K1 0x0006
#define OTA_TAG_RESERVED_START                  0x0007
#define OTA_TAG_RESERVED_END                    0xEFFF
#define OTA_TAG_MANUFACTURER_SPECIFIC_START     0xF000
#define OTA_TAG_MANUFACTURER_SPECIFIC_END       0xFFFF

extern EmberNodeId upgradeServerNodeId;
extern PGM uint8_t emAfOtaMinMessageLengths[];

#define EM_AF_OTA_MAX_COMMAND_ID ZCL_QUERY_SPECIFIC_FILE_RESPONSE_COMMAND_ID

// Create a shorter name for printing to make the code more readable.
#define otaPrintln(...) emberAfOtaBootloadClusterPrintln(__VA_ARGS__)
#define otaPrint(...)   emberAfOtaBootloadClusterPrint(__VA_ARGS__)
#define otaPrintFlush()      emberAfOtaBootloadClusterFlush()

// -------------------------------------------------------
// Common to both client and server
// -------------------------------------------------------
EmberAfOtaImageId emAfOtaCreateEmberAfOtaImageIdStruct(uint16_t manufacturerId,
                                                       uint16_t imageType,
                                                       uint32_t fileVersion);

uint8_t emAfOtaParseImageIdFromMessage(EmberAfOtaImageId* returnId,
                                       const uint8_t* buffer,
                                       uint8_t length);

#if defined(EMBER_AF_PRINT_CORE)
void emAfPrintPercentageSetStartAndEnd(uint32_t startingOffset, uint32_t endOffset);
uint8_t emAfPrintPercentageUpdate(PGM_P prefixString,
                                  uint8_t updateFrequency,
                                  uint32_t currentOffset);
uint8_t emAfCalculatePercentage(uint32_t currentOffset, uint32_t imageSize);
#else
  #define emAfPrintPercentageSetStartAndEnd(x, y)
  #define emAfPrintPercentageUpdate(x, y, z)
  #define emAfCalculatePercentage(x, y) (0)
#endif
