// *******************************************************************
// * connection-manager-tokens.h
// *
// *
// * Copyright 2015 Silicon Laboratories, Inc.                              *80*
// *******************************************************************

/**
 * Custom Application Tokens
 */
#define CREATOR_FORCE_SHORT_POLL (0x000e)
#define NVM3KEY_FORCE_SHORT_POLL (NVM3KEY_DOMAIN_USER | 0x000e)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

DEFINE_BASIC_TOKEN(FORCE_SHORT_POLL, bool, false)

#endif //DEFINETOKENS
