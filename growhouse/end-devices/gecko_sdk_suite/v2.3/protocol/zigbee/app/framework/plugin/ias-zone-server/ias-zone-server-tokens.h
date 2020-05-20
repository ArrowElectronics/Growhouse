// *******************************************************************
// * ias-zone-server-tokens.h
// *
// *
// * Copyright 2018 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

/**
 * Custom Application Tokens
 */
#define CREATOR_PLUGIN_IAS_ZONE_SERVER_ENROLLMENT_METHOD  (0x0020)
#define NVM3KEY_PLUGIN_IAS_ZONE_SERVER_ENROLLMENT_METHOD (NVM3KEY_DOMAIN_USER | 0x0020)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif  // DEFINETYPES

#ifdef DEFINETOKENS
// Define the actual token storage information here
DEFINE_BASIC_TOKEN(PLUGIN_IAS_ZONE_SERVER_ENROLLMENT_METHOD, uint8_t, 0xFF)

#endif  // DEFINETOKENS
