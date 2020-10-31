// Copyright 2016 Silicon Laboratories, Inc.

#ifndef SILABS_FIND_AND_BIND_INITIATOR_H
#define SILABS_FIND_AND_BIND_INITIATOR_H

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_FIND_AND_BIND_INITIATOR_PLUGIN_NAME "Find and Bind Initiator"

// -----------------------------------------------------------------------------
// API

/** @brief Starts initiator finding and binding operations.
 *
 * A call to this function will commence the initiator finding and
 * binding operations. Specifically, the initiator will attempt to start
 * searching for potential bindings that can be made with identifying
 * targets.
 *
 * @param endpoint The endpoint on which to begin initiator operations.
 *
 * @returns An ::EmberStatus value describing the success of the
 * commencement of the initiator operations.
 */
EmberStatus emberAfPluginFindAndBindInitiatorStart(uint8_t endpoint);

#endif /* __FIND_AND_BIND_INITIATOR_H__ */
