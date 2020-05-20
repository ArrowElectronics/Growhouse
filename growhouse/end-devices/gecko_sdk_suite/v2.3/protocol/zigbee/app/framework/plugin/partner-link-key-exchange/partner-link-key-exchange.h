// *******************************************************************
// * partner-link-key-exchange.h
// *
// * Support of requesting link keys with another device.
// *
// * Copyright 2008 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

#ifndef SILABS_PARTNER_LINK_KEY_EXCHANGE_H
#define SILABS_PARTNER_LINK_KEY_EXCHANGE_H

#define EMBER_AF_PLUGIN_PARTNER_LINK_KEY_EXCHANGE_TIMEOUT_MILLISECONDS \
  (EMBER_AF_PLUGIN_PARTNER_LINK_KEY_EXCHANGE_TIMEOUT_SECONDS * MILLISECOND_TICKS_PER_SECOND)

extern bool emAfAllowPartnerLinkKey;

#endif // SILABS_PARTNER_LINK_KEY_EXCHANGE_H
