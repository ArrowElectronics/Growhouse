// *******************************************************************
// * mfg-lib-plugin.h
// *
// *
// * Copyright 2015 by Silicon Laboratories. All rights reserved.           *80*
// *******************************************************************

#ifndef SILABS_MANUFACTURING_LIBRARY_CLI_PLUGIN_H
#define SILABS_MANUFACTURING_LIBRARY_CLI_PLUGIN_H

/** @brief Indicates if the manufacturing library is currently running.
 *
 * Used to check whether the manufacturing library is currently running.
 * Do not initiate scan/join behavior when the manufacturing
 * library is currently running as this will cause a conflict and may result
 * in a fatal error.
 *
 * @return A ::bool value that is true if the manufacturing library is
 * running, or false if it is not.
 */
bool emberAfMfglibRunning(void);

/** @brief Indicates if the manufacturing library token has been set.
 *
 * Returns whether the manufacturing library token has currently
 * been set. Reference designs are programmed to initiate off scan/join
 * behavior as soon as the device has been powered up. Certain sleepy devices,
 * such as security sensors, may also use the UART for manufacturing, which
 * becomes inactive during normal operation. Setting this token will allow
 * the device to stay awake or hold off on normal joining behavior for a few
 * seconds to allow manufacturing mode to be enabled. The last step in the
 * manufacturing process would be to disable this token.
 *
 * Note: This token is disabled by default. To enable this token by default
 * in the application, edit the file
 * app/framework/plugin/mfg-lib/mfg-lib-tokens.h.
 *
 * @return A ::bool value that is true if the manufacturing library token
 * has been set or false if it has not been set.
 */
bool emberAfMfglibEnabled(void);

#endif
