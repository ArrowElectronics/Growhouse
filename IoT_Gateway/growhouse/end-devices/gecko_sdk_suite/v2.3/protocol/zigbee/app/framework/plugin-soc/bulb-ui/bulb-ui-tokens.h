/**
 * Custom Application Tokens
 */
#define CREATOR_REBOOT_MONITOR  (0x000c)
#define NVM3KEY_REBOOT_MONITOR  (NVM3KEY_DOMAIN_USER | 0x000c)

#define CREATOR_SHORT_REBOOT_MONITOR (0x000e)
#define NVM3KEY_SHORT_REBOOT_MONITOR (NVM3KEY_DOMAIN_USER | 0x000e)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

DEFINE_COUNTER_TOKEN(REBOOT_MONITOR, tokTypeStackBootCounter, 0)
DEFINE_COUNTER_TOKEN(SHORT_REBOOT_MONITOR, tokTypeStackBootCounter, 0)

#endif //DEFINETOKENS
