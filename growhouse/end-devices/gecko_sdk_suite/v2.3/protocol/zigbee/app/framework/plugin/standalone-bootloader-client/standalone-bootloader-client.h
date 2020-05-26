void emAfStandaloneBootloaderClientPrintStatus(void);

void emAfStandaloneBootloaderClientGetInfo(uint16_t* bootloaderVersion,
                                           uint8_t* platformId,
                                           uint8_t* microId,
                                           uint8_t* phyId);

EmberStatus emAfStandaloneBootloaderClientLaunch(void);

void emAfStandaloneBootloaderClientGetMfgInfo(uint16_t* mfgIdReturnValue,
                                              uint8_t* boardNameReturnValue);

void emAfStandaloneBootloaderClientGetKey(uint8_t* returnData);

uint32_t emAfStandaloneBootloaderClientGetRandomNumber(void);
