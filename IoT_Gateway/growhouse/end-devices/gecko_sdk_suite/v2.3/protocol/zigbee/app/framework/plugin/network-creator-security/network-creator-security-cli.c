// Copyright 2015 Silicon Laboratories, Inc.

#include "app/framework/include/af.h"

#include "network-creator-security.h"

#ifdef EZSP_HOST
// NCP
  #define addTransientLinkKey    ezspAddTransientLinkKey
  #define clearTransientLinkKeys ezspClearTransientLinkKeys
#else
// SoC
  #define addTransientLinkKey    emberAddTransientLinkKey
  #define clearTransientLinkKeys emberClearTransientLinkKeys
#endif

void emAfPluginNetworkCreatorSecuritySetJoiningLinkKeyCommand(void)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;

  emberCopyBigEndianEui64Argument(0, eui64);
  emberCopyKeyArgument(1, &keyData);

  status = addTransientLinkKey(eui64, &keyData);

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Set joining link key",
                     status);
}

void emAfPluginNetworkCreatorSecurityClearJoiningLinkKeyCommand(void)
{
  clearTransientLinkKeys();

  emberAfCorePrintln("%p: %p: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Clear joining link keys",
                     EMBER_SUCCESS);
}

void emAfPluginNetworkCreatorSecurityOpenOrCloseNetworkCommand(void)
{
  EmberStatus status;
  bool open = (emberStringCommandArgument(-1, NULL)[0] == 'o');

  status = (open
            ? emberAfPluginNetworkCreatorSecurityOpenNetwork()
            : emberAfPluginNetworkCreatorSecurityCloseNetwork());

  emberAfCorePrintln("%p: %p network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     (open ? "Open" : "Close"),
                     status);
}

void emAfPluginNetworkCreatorSecurityOpenNetworkWithKeyCommand(void)
{
  EmberEUI64 eui64;
  EmberKeyData keyData;
  EmberStatus status;

  emberCopyBigEndianEui64Argument(0, eui64);
  emberCopyKeyArgument(1, &keyData);
  status = emberAfPluginNetworkCreatorSecurityOpenNetworkWithKeyPair(eui64, keyData);

  emberAfCorePrintln("%p: Open network: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     status);
}
