// *****************************************************************************
// * ota-storage-simple-driver.h
// *
// * The Simple Storage Module driver interface.  In other words, primitives
// * for reading / writing and storing data about the OTA file that is stored,
// * or is in the process of being downloaded and stored.
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

// TODO: put this gating back in once we have that mechanism in place for
// the generated CLI
//#if defined(EMBER_TEST)
void emAfOtaLoadFileCommand(void);
//#endif
