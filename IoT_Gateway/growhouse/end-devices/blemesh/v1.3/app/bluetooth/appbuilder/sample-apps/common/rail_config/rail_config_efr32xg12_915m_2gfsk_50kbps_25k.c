// Copyright 2017 Silicon Laboratories, Inc.
//
//

/***************************************************************************//**
 * @file rail_config.c
 * @brief RAIL Configuration
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/
// =============================================================================
//
//  WARNING: Auto-Generated Radio Config  -  DO NOT EDIT
//  Radio Configurator Version: 2.11.0
//  RAIL Adapter Version: 2.0.4
//  RAIL Compatibility: 2.x
//
// =============================================================================
#include "em_common.h"
#include "rail_config.h"

const uint8_t generated_irCalConfig[] = {
  24, 71, 3, 6, 4, 16, 0, 1, 1, 3, 1, 6, 0, 16, 39, 0, 0, 5, 0, 1, 1, 0, 0, 0, 0
};

const uint32_t generated_phyInfo[] = {
  1UL,
  0x000F0F0F, // 15.0588235294
  (uint32_t) NULL,
  (uint32_t) generated_irCalConfig,
#ifdef RADIO_CONFIG_ENABLE_TIMING
  (uint32_t) &generated_timing,
#else
  (uint32_t) NULL,
#endif
};

const uint32_t generated[] = {
  0x01031FF0UL, 0x003F003FUL,
  /* 1FF4 */ 0x00000000UL,
  /* 1FF8 */ (uint32_t) generated_phyInfo,
  0x00020004UL, 0x00000000UL,
  /* 0008 */ 0x00000000UL,
  0x00020018UL, 0x0000000FUL,
  /* 001C */ 0x00000000UL,
  0x00070028UL, 0x00000000UL,
  /* 002C */ 0x00000000UL,
  /* 0030 */ 0x00000000UL,
  /* 0034 */ 0x00000000UL,
  /* 0038 */ 0x00000000UL,
  /* 003C */ 0x00000000UL,
  /* 0040 */ 0x00000700UL,
  0x00010048UL, 0x00000000UL,
  0x00020054UL, 0x00000000UL,
  /* 0058 */ 0x00000000UL,
  0x000400A0UL, 0x00004CFFUL,
  /* 00A4 */ 0x00000000UL,
  /* 00A8 */ 0x00004DFFUL,
  /* 00AC */ 0x00000000UL,
  0x00012000UL, 0x00000744UL,
  0x00012010UL, 0x00000000UL,
  0x00012018UL, 0x0000A001UL,
  0x00013008UL, 0x0100AC13UL,
  0x00023030UL, 0x00104000UL,
  /* 3034 */ 0x00000003UL,
  0x00013040UL, 0x00000000UL,
  0x000140A0UL, 0x0F0027AAUL,
  0x000140B8UL, 0x0023C000UL,
  0x000140F4UL, 0x00001020UL,
  0x00024134UL, 0x00000880UL,
  /* 4138 */ 0x000087F6UL,
  0x00024140UL, 0x008800E0UL,
  /* 4144 */ 0x4D52E6C1UL,
  0x00044160UL, 0x00000000UL,
  /* 4164 */ 0x00000000UL,
  /* 4168 */ 0x00000006UL,
  /* 416C */ 0x00000006UL,
  0x00086014UL, 0x00000010UL,
  /* 6018 */ 0x04000000UL,
  /* 601C */ 0x0002C00FUL,
  /* 6020 */ 0x000060C8UL,
  /* 6024 */ 0x000AD000UL,
  /* 6028 */ 0x03000000UL,
  /* 602C */ 0x00000000UL,
  /* 6030 */ 0x00000000UL,
  0x00066050UL, 0x00FF5FA0UL,
  /* 6054 */ 0x00000E2DUL,
  /* 6058 */ 0x00000108UL,
  /* 605C */ 0x00140012UL,
  /* 6060 */ 0x0000B16FUL,
  /* 6064 */ 0x00000000UL,
  0x000C6078UL, 0x06A0071BUL,
  /* 607C */ 0x00000000UL,
  /* 6080 */ 0x00220390UL,
  /* 6084 */ 0x00000000UL,
  /* 6088 */ 0x00000000UL,
  /* 608C */ 0x22140A04UL,
  /* 6090 */ 0x4F4A4132UL,
  /* 6094 */ 0x00000000UL,
  /* 6098 */ 0x00000000UL,
  /* 609C */ 0x00000000UL,
  /* 60A0 */ 0x00000000UL,
  /* 60A4 */ 0x00000000UL,
  0x000760E4UL, 0x04000080UL,
  /* 60E8 */ 0x00000000UL,
  /* 60EC */ 0x07830464UL,
  /* 60F0 */ 0x3AC81388UL,
  /* 60F4 */ 0x0006209CUL,
  /* 60F8 */ 0x00206100UL,
  /* 60FC */ 0x208556B7UL,
  0x00036104UL, 0x00108000UL,
  /* 6108 */ 0x00003020UL,
  /* 610C */ 0x0000BB88UL,
  0x00016120UL, 0x00000000UL,
  0x00077014UL, 0x000270FEUL,
  /* 7018 */ 0x00000300UL,
  /* 701C */ 0x834A0060UL,
  /* 7020 */ 0x00000000UL,
  /* 7024 */ 0x00000082UL,
  /* 7028 */ 0x00000000UL,
  /* 702C */ 0x000000D5UL,
  0x00027048UL, 0x0000383EUL,
  /* 704C */ 0x000025BCUL,
  0x00037070UL, 0x00120105UL,
  /* 7074 */ 0x00083019UL,
  /* 7078 */ 0x006D8480UL,

  0xFFFFFFFFUL,
};

RAIL_ChannelConfigEntryAttr_t generated_entryAttr = {
  { 0xFFFFFFFF }
};

const RAIL_ChannelConfigEntry_t generated_channels[] = {
  {
    .phyConfigDeltaAdd = NULL,
    .baseFrequency = 915000000,
    .channelSpacing = 1000000,
    .physicalChannelOffset = 0,
    .channelNumberStart = 0,
    .channelNumberEnd = 20,
    .maxPower = RAIL_TX_POWER_MAX,
    .attr = &generated_entryAttr
  },
};

const RAIL_ChannelConfig_t generated_channelConfig = {
  generated,
  NULL,
  generated_channels,
  1
};
const RAIL_ChannelConfig_t *channelConfigs[] = {
  &generated_channelConfig,
  NULL
};

//        _  _
//       | )/ )         Wireless
//    \\ |//,' __       Application
//    (")(_)-"()))=-    Software
//       (\\            Platform
