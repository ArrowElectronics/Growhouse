/**************************************************************************//**
 * Copyright 2018 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef SILABS_TRUST_CENTER_KEEPALIVE_H
#define SILABS_TRUST_CENTER_KEEPALIVE_H

// The duration in milliseconds to wait between two successive keepalives.  The
// period shall be between five and 20 minutes, according to section 5.4.2.2.3.4
// of 105638r09.  The plugin option, specified in minutes, is converted here to
// milliseconds.
#define EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_DELAY_INTERVAL \
  (EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_INTERVAL * MILLISECOND_TICKS_PER_MINUTE)

// The number of unacknowledged keepalives permitted before declaring that the
// trust center is inaccessible and initiating a search for it.  Section
// 5.4.2.2.3.4 of 105638r09 specifies that this value shall be three.
#define EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_FAILURE_LIMIT 3

void emAfPluginTrustCenterKeepaliveReadAttributesResponseCallback(uint8_t *buffer,
                                                                  uint16_t bufLen);

void emAfSendKeepaliveSignal(void);

void emberAfPluginTrustCenterKeepaliveTickNetworkEventHandler(void);

bool emberAfPluginTrustCenterKeepaliveTimeoutCallback(void);

void emberAfPluginTrustCenterKeepaliveConnectivityRestoredCallback(void);

uint8_t emAfPluginTrustCenterKeepaliveGetBaseTimeMinutes(void);

uint16_t emAfPluginTrustCenterKeepaliveGetJitterTimeSeconds(void);

#endif // SILABS_TRUST_CENTER_KEEPALIVE_H
