// Copyright 2016 Silicon Laboratories, Inc.                                *80*

#ifndef SILABS_BULB_UI_H
#define SILABS_BULB_UI_H

/** @brief Starts the search for joinable networks.
 *
 * Starts the bulb-UI search for joinable networks.  This
 * is not normally required, as the bulb-UI will do this automatically.
 * However, some plugins that interrupt the normal bulb-UI behavior,
 * such as the manufacturing library cluster server, may need to kick off
 * a network search when their task is complete.
 *
 */
void emberAfPluginBulbUiInitiateNetworkSearch(void);

#endif
