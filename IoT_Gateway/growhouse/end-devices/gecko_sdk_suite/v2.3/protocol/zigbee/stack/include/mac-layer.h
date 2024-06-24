/**
 * @file mac-layer.h
 * @brief EmberZNet API for accessing and setting MAC configuration.
 * See @ref mac_layer for documentation.
 *
 * <!--Copyright 2018 Silicon Labs. All rights reserved.                 *80*-->
 */

#ifndef SILABS_MAC_LAYER_H
#define SILABS_MAC_LAYER_H

/**
 * @addtogroup mac_layer
 *
 * See mac-layer.h for source code.
 * @{
 */

/**@brief Get max number of MAC retries
 *
 * Returns the maximum number of no-ack retries that will be attempted
 *
 * @return uint8_t Max MAC retries
 */
uint8_t emberGetMaxMacRetries(void);

/** @} END addtogroup */

#endif // SILABS_MAC_LAYER_H
