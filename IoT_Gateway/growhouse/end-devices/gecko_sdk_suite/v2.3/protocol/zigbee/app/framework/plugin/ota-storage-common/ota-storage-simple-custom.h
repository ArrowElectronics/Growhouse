// *****************************************************************************
// * ota-storage-custom.h
// *
// * This defines the custom storage interface used by ota-storage-simple.c
// * to interact with the actual hardware where the images are stored.
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

bool emberAfCustomStorageInitCallback(void);
bool emberAfCustomStorageReadCallback(uint32_t offset,
                                      uint32_t length,
                                      uint8_t* returnData);
bool emberAfCustomStorageWriteCallback(const uint8_t* dataToWrite,
                                       uint32_t offset,
                                       uint32_t length);
