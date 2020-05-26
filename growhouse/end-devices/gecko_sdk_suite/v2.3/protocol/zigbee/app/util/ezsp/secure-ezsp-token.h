// File: secure-ezsp-token.h

#ifdef EMBER_TEST
#include "app/xncp/xncp-token.h"
#endif

#define CREATOR_SECURE_EZSP_SECURITY_KEY 0x5240  // RE
#define NVM3KEY_SECURE_EZSP_SECURITY_KEY (NVM3KEY_DOMAIN_USER | 0x5240)

#ifdef DEFINETYPES
typedef struct {
  /** This is the key byte data. */
  uint8_t contents[EMBER_ENCRYPTION_KEY_SIZE];
} tokTypeSecureEzspSecurityKey;
#endif //DEFINETYPES

#ifdef DEFINETOKENS
DEFINE_BASIC_TOKEN(SECURE_EZSP_SECURITY_KEY,
                   tokTypeSecureEzspSecurityKey,
                   { { 0, } })
#endif //DEFINETOKENS
