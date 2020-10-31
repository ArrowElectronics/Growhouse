// *******************************************************************
// * mfg-lib-tokens.h
// *
// *
// * Copyright 2014 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

/**
 * Custom Application Tokens
 */
#define CREATOR_MFG_LIB_ENABLED  (0x000d)
#define NVM3KEY_MFG_LIB_ENABLED  (NVM3KEY_DOMAIN_USER | 0x000d)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

DEFINE_BASIC_TOKEN(MFG_LIB_ENABLED, uint8_t, 0)

#endif //DEFINETOKENS
