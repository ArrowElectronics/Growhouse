// *******************************************************************
// * attribute-table.h
// *
// * This file contains the definitions for the attribute table, its
// * sizes, count, and API.
// *
// * Copyright 2008 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef ZCL_UTIL_ATTRIBUTE_TABLE_H
#define ZCL_UTIL_ATTRIBUTE_TABLE_H

#include "../include/af.h"

#define ZCL_NULL_ATTRIBUTE_TABLE_INDEX 0xFFFF

// Remote devices writing attributes of local device
EmberAfStatus emberAfWriteAttributeExternal(uint8_t endpoint,
                                            EmberAfClusterId cluster,
                                            EmberAfAttributeId attributeID,
                                            uint8_t mask,
                                            uint16_t manufacturerCode,
                                            uint8_t* dataPtr,
                                            EmberAfAttributeType dataType);

void emberAfRetrieveAttributeAndCraftResponse(uint8_t endpoint,
                                              EmberAfClusterId clusterId,
                                              EmberAfAttributeId attrId,
                                              uint8_t mask,
                                              uint16_t manufacturerCode,
                                              uint16_t readLength);
EmberAfStatus emberAfAppendAttributeReportFields(uint8_t endpoint,
                                                 EmberAfClusterId clusterId,
                                                 EmberAfAttributeId attributeId,
                                                 uint8_t mask,
                                                 uint8_t *buffer,
                                                 uint8_t bufLen,
                                                 uint8_t *bufIndex);
void emberAfPrintAttributeTable(void);

bool emberAfReadSequentialAttributesAddToResponse(uint8_t endpoint,
                                                  EmberAfClusterId clusterId,
                                                  EmberAfAttributeId startAttributeId,
                                                  uint8_t mask,
                                                  uint16_t manufacturerCode,
                                                  uint8_t maxAttributeIds,
                                                  bool includeAccessControl);

EmberAfStatus emAfWriteAttribute(uint8_t endpoint,
                                 EmberAfClusterId cluster,
                                 EmberAfAttributeId attributeID,
                                 uint8_t mask,
                                 uint16_t manufacturerCode,
                                 uint8_t* data,
                                 EmberAfAttributeType dataType,
                                 bool overrideReadOnlyAndDataType,
                                 bool justTest);

EmberAfStatus emAfReadAttribute(uint8_t endpoint,
                                EmberAfClusterId cluster,
                                EmberAfAttributeId attributeID,
                                uint8_t mask,
                                uint16_t manufacturerCode,
                                uint8_t* dataPtr,
                                uint16_t readLength,
                                EmberAfAttributeType* dataType);

#endif // ZCL_UTIL_ATTRIBUTE_TABLE_H
