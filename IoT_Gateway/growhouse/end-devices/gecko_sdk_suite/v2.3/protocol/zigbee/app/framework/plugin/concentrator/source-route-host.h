/**
 * @file: source-route-host.h
 *
 * Description: Declarations for managing source routes on a host-based gateway.
 *
 * Copyright 2007 by Ember Corporation. All rights reserved.                *80*
 */

/** Search for a source route to the given destination. If one is found, return
 * true and copy the relay list to the given location. If no route is found,
 * return false and don't modify the given location.
 */
bool emberFindSourceRoute(EmberNodeId destination,
                          uint8_t *relayCount,
                          uint16_t *relayList);
