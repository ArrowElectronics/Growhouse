// *****************************************************************************
// * network-find.h
// *
// * Routines for finding and joining any viable network via scanning, rather
// * than joining a specific network.
// *
// * Copyright 2017 by Ember Corporation. All rights reserved.              *80*
// *****************************************************************************

#ifndef NETWORK_FIND_DOT_H_INCLUDED
#define NETWORK_FIND_DOT_H_INCLUDED

#include "app/framework/include/af.h"

/** @brief Sets the channel mask for "find unused" and "find joinable".
 * Permitted pages are 0 and, if Sub-GHz channels are included, then also 28-31.
 */
EmberStatus emberAfSetFormAndJoinChannelMask(uint8_t page, uint32_t mask);

/** @brief Returns the channel mask for a given page.
 * Only the bottom 27 bits can be set. The top 5 bits are reserved for the page
 * number and are always zero in a returned channel mask. That leaves the value
 * 0xFFFFFFFF free to indicate an invalid page error.
 */
uint32_t emberAfGetFormAndJoinChannelMask(uint8_t page);

/** @brief Sets search mode for "find unused" and "find joinable".
 * Mode is a bitmask. Permitted values are set by the FIND_AND_JOIN_MODE_...
 * macros.
 */
EmberStatus emberAfSetFormAndJoinSearchMode(uint8_t mode);

/** @brief Returns the current search mode.
 */
uint8_t emberAfGetFormAndJoinSearchMode(void);

#endif // NETWORK_FIND_DOT_H_INCLUDED
