// Internal definitions for the OTA storage Linux.

typedef void (EmAfOtaStorageFileAddedHandler)(const EmberAfOtaHeader*);

typedef struct {
  bool memoryDebug;
  bool fileDebug;
  bool fieldDebug;
  bool ignoreFilesWithUnderscorePrefix;
  bool printFileDiscoveryOrRemoval;
  EmAfOtaStorageFileAddedHandler* fileAddedHandler;
} EmAfOtaStorageLinuxConfig;

void emAfOtaStorageGetConfig(EmAfOtaStorageLinuxConfig* currentConfig);
void emAfOtaStorageSetConfig(const EmAfOtaStorageLinuxConfig* newConfig);
