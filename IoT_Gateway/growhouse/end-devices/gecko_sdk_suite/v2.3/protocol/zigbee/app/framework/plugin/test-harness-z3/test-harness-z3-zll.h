//
// test-harness-z3-zll.c
//
// Wednesday, December 9, 2015
//
// ZigBee 3.0 touchlink test harness functionality
//

// -----------------------------------------------------------------------------
// Internal constants

#define EM_AF_PLUGIN_TEST_HARNESS_Z3_ZLL_SERVER_TO_CLIENT_FRAME_CONTROL \
  (ZCL_CLUSTER_SPECIFIC_COMMAND                                         \
   | ZCL_FRAME_CONTROL_SERVER_TO_CLIENT                                 \
   | ZCL_DISABLE_DEFAULT_RESPONSE_MASK)
#define EM_AF_PLUGIN_TEST_HARNESS_Z3_ZLL_CLIENT_TO_SERVER_FRAME_CONTROL \
  (ZCL_CLUSTER_SPECIFIC_COMMAND                                         \
   | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER                                 \
   | ZCL_DISABLE_DEFAULT_RESPONSE_MASK)

// -----------------------------------------------------------------------------
// Framework-internal callbacks

void emAfPluginTestHarnessZ3ZllStackStatusCallback(EmberStatus status);

void emAfPluginTestHarnessZ3ZllNetworkFoundCallback(const EmberZllNetwork *networkInfo);
void emAfPluginTestHarnessZ3ZllScanCompleteCallback(EmberStatus status);
void emAfPluginTestHarnessZ3ZllCommandCallback(EmberMessageBuffer command,
                                               uint8_t startIndex,
                                               EmberEUI64 sourceEui64);
boolean emAfPluginTestHarnessZ3ZllModifyInterpanCommand(uint8_t *commandData,
                                                        uint8_t macHeaderLength,
                                                        void *data);
