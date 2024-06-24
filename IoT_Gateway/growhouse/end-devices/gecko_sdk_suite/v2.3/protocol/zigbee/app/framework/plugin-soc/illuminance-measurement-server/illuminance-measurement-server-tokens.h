// * Copyright 2015 by Silicon Laboratories. All rights reserved.           *80*

/**
 * Custom Application Tokens
 */
#define CREATOR_SI1141_MULTIPLIER  (0x000F)
#define NVM3KEY_SI1141_MULTIPLIER  (NVM3KEY_DOMAIN_USER | 0x000F)

#ifdef DEFINETOKENS
DEFINE_BASIC_TOKEN(SI1141_MULTIPLIER, uint8_t, 0)
#endif //DEFINETOKENS
