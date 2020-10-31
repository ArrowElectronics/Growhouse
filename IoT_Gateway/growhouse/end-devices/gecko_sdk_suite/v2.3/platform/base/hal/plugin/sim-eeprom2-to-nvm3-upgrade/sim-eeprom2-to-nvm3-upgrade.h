/** @file hal/plugin/sim-eeprom2-to-nvm3-upgrade/sim-eeprom2-to-nvm3-upgrade.h
 * @brief Upgrade support from Simulated EEPROM version 2 to NVM3.
 *
 * <!-- Copyright 2018 Silicon Laboratories, Inc.                        *80*-->
 */
#ifndef __SIM_EEPROM2_TO_NVM3_UPGRADE_H__
#define __SIM_EEPROM2_TO_NVM3_UPGRADE_H__

/** @addtogroup sim-eeprom2-to-nvm3-upgrade
 * The Simulated EEPROM2 to NVM3 upgrade code implements functions needed to
 * upgrade an Simulated EEPROM2 instance to an NVM3 instance while maintaining
 * any token data. The upgrade is done in-place as the 36 KB Simulated EEPROM2
 * instance is swapped with a 36 KB NVM3 instance.
 *
 *@{
 */
#ifdef NVM3_FLASH_PAGES
#if (NVM3_FLASH_PAGES != 18)
#error "ERROR: NVM3 flash size must be 36K when upgrading from SimEE2"
#endif
#endif

//application functions

/** @brief Upgrade SimEE2 to NVM3
 *
 */
EmberStatus halSimEeToNvm3Upgrade(void);

#endif //__SIM_EEPROM2_TO_NVM3_UPGRADE_H__

/**@} END sim-eeprom2-to-nvm3-upgrade group
 */
