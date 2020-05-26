#include "app/framework/include/af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "update-tc-link-key.h"
#include "app/framework/plugin/network-steering/network-steering.h"
#include "app/framework/plugin/network-steering/network-steering-internal.h"

// -----------------------------------------------------------------------------
// Helper macros and functions

void emberAfPluginSetTCLinkKeyUpdateTimerCommand(void)
{
  uint32_t timeInMilliseconds = (uint32_t)emberUnsignedCommandArgument(0);
  emberAfPluginSetTCLinkKeyUpdateTimerMilliSeconds(timeInMilliseconds);
}
