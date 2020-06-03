// Copyright 2016 Silicon Laboratories, Inc.

#ifndef SILABS_FIND_AND_BIND_TARGET_H
#define SILABS_FIND_AND_BIND_TARGET_H

// -----------------------------------------------------------------------------
// Constants

#define EMBER_AF_PLUGIN_FIND_AND_BIND_TARGET_PLUGIN_NAME "Find and Bind Target"

// -----------------------------------------------------------------------------
// API

/** @brief Starts target finding and binding operations.
 *
 * A call to this function will commence the target finding and
 * binding operations. Specifically, the target will attempt to start
 * identifying on the endpoint that is passed as a parameter.
 *
 * @param endpoint The endpoint on which to begin target operations.
 *
 * @returns An ::EmberAfStatus value describing the success of the
 * commencement of the target operations.
 */
EmberAfStatus emberAfPluginFindAndBindTargetStart(uint8_t endpoint);

#endif /* __FIND_AND_BIND_TARGET_H__ */
