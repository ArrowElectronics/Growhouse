#define CREATOR_CUSTOM 0xE700
#define NVM3KEY_CUSTOM (NVM3KEY_DOMAIN_ZIGBEE | 0xE700)

#ifdef DEFINETYPES
typedef struct {
  uint8_t nodeType;
  uint16_t nodeId;
  uint16_t panId;
} tokTypeCustom;
#endif //DEFINETYPES

#ifdef DEFINETOKENS
DEFINE_BASIC_TOKEN(CUSTOM,
                   tokTypeCustom,
                   { 0x00, 0x0000, 0x0000 })
#endif //DEFINETOKENS
