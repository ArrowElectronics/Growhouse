//
// test-harness-z3-core.h
//
// August 3, 2015
// Refactored November 23, 2015
//
// ZigBee 3.0 core test harness functionality
//

// -----------------------------------------------------------------------------
// Constants

#define TEST_HARNESS_Z3_PRINT_NAME "TestHarnessZ3"

// -----------------------------------------------------------------------------
// Globals

enum {
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZR_NOT_ADDRESS_ASSIGNABLE     = 0x00,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZR_ADDRESS_ASSIGNABLE         = 0x01,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZED_NOT_ADDRESS_ASSIGNABLE    = 0x02,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_ZED_ADDRESS_ASSIGNABLE        = 0x03,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_SLEEPY_ZED_NOT_ADDRESS_ASSIGNABLE = 0x04,
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_SLEEPY_ZED_ADDRESS_ASSIGNABLE = 0x05,
};
typedef uint8_t EmAfPluginTestHarnessZ3DeviceMode;
extern EmAfPluginTestHarnessZ3DeviceMode emAfPluginTestHarnessZ3DeviceMode;
extern uint16_t emAfPluginTestHarnessZ3TouchlinkProfileId;

#define EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_MAX \
  EM_AF_PLUGIN_TEST_HARNESS_Z3_DEVICE_MODE_SLEEPY_ZED_ADDRESS_ASSIGNABLE
#define COMMAND_DATA_SIZE 128
#define EXTRA_COMMAND_BYTES 5

// -----------------------------------------------------------------------------
// Utility API

uint32_t emAfPluginTestHarnessZ3GetSignificantBit(uint8_t commandIndex);
