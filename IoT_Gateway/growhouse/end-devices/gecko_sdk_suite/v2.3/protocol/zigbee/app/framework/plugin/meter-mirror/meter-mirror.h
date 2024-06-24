// meter-mirror.h

// A bit confusing, the EMBER_AF_MANUFACTURER_CODE is actually the manufacturer
// code defined in AppBuilder.  This is usually the specific vendor of
// the local application.  It does not have to be "Ember's" (Silabs) manufacturer
// code, but that is the default.
#define EM_AF_APPLICATION_MANUFACTURER_CODE EMBER_AF_MANUFACTURER_CODE

uint8_t emAfPluginMeterMirrorGetMirrorsAllocated(void);

extern EmberEUI64 nullEui64;

#define EM_AF_MIRROR_ENDPOINT_END              \
  (EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START \
   + EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS)

bool emberAfPluginMeterMirrorGetEui64ByEndpoint(uint8_t endpoint,
                                                EmberEUI64 returnEui64);
bool emberAfPluginMeterMirrorIsMirrorUsed(uint8_t endpoint);
bool emberAfPluginMeterMirrorGetEndpointByEui64(EmberEUI64 eui64,
                                                uint8_t *returnEndpoint);
uint16_t emberAfPluginMeterMirrorRequestMirror(EmberEUI64 requestingDeviceIeeeAddress);
uint16_t emberAfPluginMeterMirrorRemoveMirror(EmberEUI64 requestingDeviceIeeeAddress);
