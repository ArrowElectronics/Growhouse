/*******************************************************************************
 * Over The Air [Cluster] Upgrade Image Storage, Common Routines
 *
 * This file contains common code that is not specific to the actual storage
 * mechanism (e.g. EEPROM, Unix filesystem, etc.) and should be included by
 * all implementations of a storage device.
 *
 *   Copyright 2010 by Ember Corporation. All rights reserved.              *80*
 ******************************************************************************/

// this file contains all the common includes for clusters in the util
#include "app/framework/util/common.h"
#include "app/framework/util/attribute-storage.h"
#include "enums.h"

#include "app/framework/plugin/ota-common/ota.h"
#include "app/framework/plugin/ota-storage-common/ota-storage.h"

#if defined (IMAGE_BUILDER)
// For our PC tool, we use a simpler #define to turn on this code.
  #define ZCL_USING_OTA_BOOTLOAD_CLUSTER_CLIENT
#endif

//------------------------------------------------------------------------------
// Globals

// We arbitrarily limit the maximum number of tags this code can support.
// For most images they will include an Upgrade Image tag, a certificate tag
// and a signature tag.  So this should be more than enough.  We limit this
// value in part to reduce the data stack usage with extra space that is
// never used.
// However if this isn't enough, the developer can override it.
#if !defined MAX_TAGS_IN_OTA_FILE
  #define MAX_TAGS_IN_OTA_FILE 6
#endif

//------------------------------------------------------------------------------
// API

EmberAfOtaImageId emAfOtaStorageGetImageIdFromHeader(const EmberAfOtaHeader* header)
{
  EmberAfOtaImageId id = INVALID_OTA_IMAGE_ID;
  id.manufacturerId = header->manufacturerId;
  id.imageTypeId = header->imageTypeId;
  id.firmwareVersion = header->firmwareVersion;

  if (headerHasUpgradeFileDest(header)) {
    MEMCOPY(id.deviceSpecificFileEui64,
            header->upgradeFileDestination,
            EUI64_SIZE);
  }

  return id;
}

// Although the header length is really 16-bit, we often want to use it to increment
// a 32-bit offset variable, so just make it 32-bit.

EmberAfOtaStorageStatus emAfOtaStorageGetHeaderLengthAndImageSize(const EmberAfOtaImageId* id,
                                                                  uint32_t *returnHeaderLength,
                                                                  uint32_t *returnImageSize)
{
  // The EmberAfOtaHeader struct is rather large, and on the 32-bit machines
  // all the 16-bit fields will be padded to 32-bits, thus increasing its size.
  // Most of the time we only want the header length and image size, so to
  // conserve the data stack this routine returns only those values.

  EmberAfOtaHeader fullHeader;
  EmberAfOtaStorageStatus status
    = emberAfOtaStorageGetFullHeaderCallback(id,
                                             &fullHeader);

  if (EMBER_AF_OTA_STORAGE_SUCCESS != status) {
    return status;
  }

  if (returnHeaderLength) {
    *returnHeaderLength = fullHeader.headerLength;
  }
  if (returnImageSize) {
    *returnImageSize = fullHeader.imageSize;
  }
  return status;
}

EmberAfOtaStorageStatus emAfOtaStorageGetTagOffsetAndSize(const EmberAfOtaImageId* id,
                                                          uint16_t tag,
                                                          uint32_t* returnTagOffset,
                                                          uint32_t* returnTagSize)
{
  EmberAfTagData tags[MAX_TAGS_IN_OTA_FILE];
  uint16_t totalTags;
  uint8_t i;
  bool found = false;
  uint32_t offset;
  //Initializer needed according to Lint
  uint32_t tagLength = 0;

  if (EMBER_AF_OTA_STORAGE_SUCCESS
      != emAfOtaStorageGetHeaderLengthAndImageSize(id,
                                                   &offset,
                                                   NULL) // image size return ptr (not needed)
      || (EMBER_AF_OTA_STORAGE_SUCCESS
          != emAfOtaStorageReadAllTagInfo(id,
                                          tags,
                                          MAX_TAGS_IN_OTA_FILE,
                                          &totalTags))) {
    return EMBER_AF_OTA_STORAGE_ERROR;
  }

  for (i = 0; !found && i < totalTags; i++) {
    offset += TAG_OVERHEAD;
    if (tags[i].id == tag) {
      tagLength = tags[i].length;
      found = true;
    } else {
      offset += tags[i].length;
    }
  }
  if (!found) {
    return EMBER_AF_OTA_STORAGE_ERROR;
  }

  *returnTagOffset = offset;
  *returnTagSize = tagLength;
  return EMBER_AF_OTA_STORAGE_SUCCESS;
}

EmberAfOtaStorageStatus emAfOtaStorageGetTagDataFromImage(const EmberAfOtaImageId* id,
                                                          uint16_t tag,
                                                          uint8_t* returnData,
                                                          uint32_t* returnDataLength,
                                                          uint32_t maxReturnDataLength)
{
  uint32_t offset;
  uint32_t tagLength;
  EmberAfOtaStorageStatus status;

  status =
    emAfOtaStorageGetTagOffsetAndSize(id,
                                      tag,
                                      &offset,
                                      &tagLength);
  if (status != EMBER_AF_OTA_STORAGE_SUCCESS) {
    return status;
  }

  if (tagLength > maxReturnDataLength) {
    return EMBER_AF_OTA_STORAGE_RETURN_DATA_TOO_LONG;
  }

  return emberAfOtaStorageReadImageDataCallback(id,
                                                offset,
                                                tagLength,
                                                returnData,
                                                returnDataLength);
}

EmberAfOtaStorageStatus emAfOtaStorageGetRawHeaderData(const EmberAfOtaImageId* id,
                                                       uint8_t* returnData,
                                                       uint32_t* returnDataLength,
                                                       uint32_t maxReturnDataLength)
{
  // We need to know the header size before we can get the raw data.
  uint32_t headerLength;
  EmberAfOtaStorageStatus status
    = emAfOtaStorageGetHeaderLengthAndImageSize(id,
                                                &headerLength,
                                                NULL); // image size ptr (don't care)

  if (status) {
    return status;
  }

  if (*returnDataLength < headerLength) {
    return EMBER_AF_OTA_STORAGE_RETURN_DATA_TOO_LONG;
  }

  return emberAfOtaStorageReadImageDataCallback(id,
                                                0,           // start offset
                                                headerLength,
                                                returnData,
                                                returnDataLength);
}

EmberAfOtaStorageStatus emAfOtaStorageReadAllTagInfo(const EmberAfOtaImageId* id,
                                                     EmberAfTagData* tagInfo,
                                                     uint16_t maxTags,
                                                     uint16_t* totalTags)
{
  uint8_t tagData[TAG_OVERHEAD];
  uint32_t returnedLength;
  uint32_t offset;
  uint32_t imageSize;
  EmberAfOtaStorageStatus status
    = emAfOtaStorageGetHeaderLengthAndImageSize(id,
                                                &offset,
                                                &imageSize);

  if (EMBER_AF_OTA_STORAGE_SUCCESS != status) {
    return status;
  }

  // Although there is a field in the file that tells us the max
  // number of tags in the file, we do not rely upon that.
  // We count the number of tags actually in the file.
  *totalTags = 0;

  while (offset < imageSize) {
    uint32_t lengthOfEmberAfOtaStorageStatusInFile;
    EmberAfOtaStorageStatus status =
      emberAfOtaStorageReadImageDataCallback(id,
                                             offset,
                                             TAG_OVERHEAD,
                                             tagData,
                                             &returnedLength);
    if (status != EMBER_AF_OTA_STORAGE_SUCCESS
        || TAG_OVERHEAD != returnedLength) {
      // File is too short or could not read the data on disk.
      return EMBER_AF_OTA_STORAGE_ERROR;
    }
    lengthOfEmberAfOtaStorageStatusInFile = ((uint32_t)(tagData[2])
                                             + ((uint32_t)(tagData[3]) << 8)
                                             + ((uint32_t)(tagData[4]) << 16)
                                             + ((uint32_t)(tagData[5]) << 24));
    if (*totalTags < maxTags) {
      tagInfo[*totalTags].id = tagData[0] + (tagData[1] << 8);
      tagInfo[*totalTags].length = lengthOfEmberAfOtaStorageStatusInFile;
    } else if (*totalTags > maxTags) {
      // To prevent bad data from making us go into an endless loop we will
      // stop processing tags once we surpass the number of max tags we
      // can return data for.
      return EMBER_AF_OTA_STORAGE_SUCCESS;
    }
    (*totalTags)++;
    offset += TAG_OVERHEAD + lengthOfEmberAfOtaStorageStatusInFile;
  }

  return EMBER_AF_OTA_STORAGE_SUCCESS;
}

bool emberAfIsOtaImageIdValid(const EmberAfOtaImageId* idToCompare)
{
  return (0 != MEMPGMCOMPARE(idToCompare,
                             &emberAfInvalidImageId,
                             sizeof(EmberAfOtaImageId)));
}
