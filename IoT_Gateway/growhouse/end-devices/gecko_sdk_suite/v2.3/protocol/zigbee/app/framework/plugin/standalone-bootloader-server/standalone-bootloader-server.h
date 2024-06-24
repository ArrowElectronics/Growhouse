void emAfStandaloneBootloaderServerPrintTargetClientInfoCommand(void);
void emAfStandaloneBootloaderServerPrintStatus(void);

// Public API
EmberStatus emberAfPluginStandaloneBootloaderServerBroadcastQuery(void);
EmberStatus emberAfPluginStandaloneBootloaderServerStartClientBootload(EmberEUI64 longId,
                                                                       const EmberAfOtaImageId* id,
                                                                       uint16_t tag);
EmberStatus emberAfPluginStandaloneBootloaderServerStartClientBootloadWithCurrentTarget(const EmberAfOtaImageId* id,
                                                                                        uint16_t tag);
