// *****************************************************************************
// * MAC Address Filtering - Public APIs
// *
// * Copyright 2017 Silicon Laboratories, Inc.                              *80*
// *****************************************************************************

void emberAfPluginMacAddressFilteringInitCallback(void);

void emberAfPluginMacAddressFilteringInitCallback(void);

void emberAfPluginMacAddressFilteringPrintConfigCommand(void);

void emberAfPluginMacAddressFilteringFilterNoAddressCommand(void);

void emberAfPluginMacAddressFilteringAddShortAddressCommand(void);

void emberAfPluginMacAddressFilteringSetShortAddressListType(void);

void emberAfPluginMacAddressFilteringSetLongAddressListType(void);

void emberAfPluginMacAddressFilteringAddLongAddressCommand(void);

void emberAfPluginMacAddressFilteringClearShortAddressList(void);

void emberAfPluginMacAddressFilteringClearLongAddressList(void);

void emberAfPluginMacAddressFilteringAddPanIdCommand(void);

void emberAfPluginMacAddressFilteringPanIdDeleteEntry(void);

void emberAfPluginMacAddressFilteringClearPanIdList(void);

void emberAfPluginMacAddressFilteringSetPanIdListType(void);

void emberAfPluginMacAddressFilteringReset(void);

void emberAfPluginMacAddressFilteringClearAll(void);

void emberAfPluginMacAddressFilteringShortAddressDeleteEntry(void);

void emberAfPluginMacAddressFilteringLongAddressDeleteEntry(void);

EmberStatus emberAfPluginMacAddressFilteringGetStatsForShortAddress(uint8_t index,
                                                                    uint32_t* matchCountForIndexPtr);
void emberAfPluginMacAddressFilteringGetStats(boolean shortMode,
                                              uint32_t* allowedPacketCountPtr,
                                              uint32_t* droppedPacketCountPtr,
                                              uint32_t* totalPacketCountPtr);
