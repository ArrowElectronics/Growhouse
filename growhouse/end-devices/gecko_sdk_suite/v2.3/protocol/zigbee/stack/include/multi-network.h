/**
 * @file multi-network.h
 * @brief EmberZNet API for multi-network support.
 * See @ref multi_network for documentation.
 *
 * <!--Copyright 2004-2012 by Ember Corporation. All rights reserved.    *80*-->
 */

#ifndef SILABS_MULTI_NETWORK_H
#define SILABS_MULTI_NETWORK_H

/**
 * @addtogroup multi_network
 *
 * See multi-network.h for source code.
 * @{
 */

/** @brief Returns the current network index.
 */
uint8_t emberGetCurrentNetwork(void);

/** @brief Sets the current network.
 *
 * @param index   The network index.
 *
 * @return ::EMBER_INDEX_OUT_OF_RANGE if the index does not correspond to a
 * valid network, and ::EMBER_SUCCESS otherwise.
 */
EmberStatus emberSetCurrentNetwork(uint8_t index);

/** @brief Can only be called inside an application callback.
 *
 * @return the index of the network the callback refers to. If this function
 * is called outside of a callback, it returns 0xFF.
 */
uint8_t emberGetCallbackNetwork(void);

/** @} END addtogroup */

#endif // SILABS_MULTI_NETWORK_H
