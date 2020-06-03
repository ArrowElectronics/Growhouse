// *******************************************************************
// * zll-commissioning.h
// *
// *
// * Copyright 2010 by Ember Corporation. All rights reserved.              *80*
// *******************************************************************

/** @brief Generates a random network key and initializes the security state of
 * the device.
 *
 * This function is a convenience wrapper for ::emberZllSetInitialSecurityState,
 * which must be called before starting or joining a network. The plugin
 * initializes the security state for the initiator during touch linking. The
 * target must initialize its own security state prior to forming a network
 * either by using this function or by calling ::emberZllSetInitialSecurityState
 * directly.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllSetInitialSecurityState(void);

/** @brief Initiates the touch link procedure.
 *
 * This function causes the stack to broadcast a series of ScanRequest
 * commands via inter-PAN messaging. The plugin selects the target that
 * sent a ScanResponse command with the strongest RSSI and attempts to link with
 * it. If touch linking completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the network and the target. If touch linking fails, the plugin will
 * call ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllInitiateTouchLink(void);

/** @brief Initiates a touch link to retrieve information
 * about a target device.
 *
 * As with a traditional touch link, this function causes the stack to
 * broadcast messages to discover a target device. When the target is selected
 * (based on RSSI), the plugin will retrieve information about it by unicasting
 * a series of DeviceInformationRequest commands via inter-PAN messaging. If
 * the process completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the target. If touch linking fails, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllDeviceInformationRequest(void);

/** @brief Initiates a touch link to cause a target device to
 * identify itself.
 *
 * As with a traditional touch link, this function causes the stack to
 * broadcast messages to discover a target device. When the target is selected
 * (based on RSSI), the plugin will cause it to identify itself by unicasting
 * an IdentifyRequest command via inter-PAN messaging. If the process
 * completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the target. If touch linking fails, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllIdentifyRequest(void);

/** @brief Initiates a touch link for the purpose of resetting a target device.
 *
 * As with a traditional touch link, this function causes the stack to
 * broadcast messages to discover a target device. When the target is selected
 * (based on RSSI), the plugin will reset it by unicasting a
 * ResetToFactoryNewRequest command via inter-PAN messaging. If the process
 * completes successfully, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkCompleteCallback with information
 * about the target. If touch linking fails, the plugin will call
 * ::emberAfPluginZllCommissioningTouchLinkFailedCallback.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
EmberStatus emberAfZllResetToFactoryNewRequest(void);

/** @brief No touchlink for non-factory new device.
 *
 * This function will cause the NFN device to refuse network start/join
 * requests in case it receives them and will not allow touchlinking. This can be useful
 * to restrict touchlink stealing.
 */
void emberAfZllNoTouchlinkForNFN(void);

/** @brief Disables touchlink processing.
 *
 * This function will cause the device to refuse network start/join
 * requests if it receives them and will not allow touchlinking.
 * Note that this will have the effect of overriding the
 * emberAfZllNoTouchlinkForNFN function.
 */
void emberAfZllDisable(void);

/** @brief Enables touchlink processing.
 *
 * This function will cause the device to accept network start/join
 * requests if it receives them and will not allow touchlinking.
 * Note that this will have the effect of overriding the
 * emberAfZllNoTouchlinkForNFN function.
 */
void emberAfZllEnable(void);

/** @brief Aborts the touch link procedure.
 *
 * This function can be called to cancel the touch link procedure. This can be
 * useful, for example, if the touch link target is incorrect.
 */
void emberAfZllAbortTouchLink(void);

/** @brief Indicates whether a touch link procedure is currently in progress.
 *
 * @return True if a touch link is in progress or false otherwise.
 */
bool emberAfZllTouchLinkInProgress(void);

/** @brief Resets the local device to a factory new state.
 *
 * This function causes the device to leave the network and clear its
 * network parameters, resets its attributes to their default values, and clears
 * the group and scene tables.
 */
void emberAfZllResetToFactoryNew(void);

/** @brief Scans for joinable networks.
 *
 * This function will scan the primary channel set for joinable networks. If a
 * joinable network is found, the plugin will attempt to join to it. If no
 * joinable networks are found or if joining is not successful, the plugin will
 * scan the secondary channel set for joinable networks. If a joinable network
 * is found, the plugin will attempt to join to it. The plugin will only scan
 * the secondary channel set if
 * ::EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_SECONDARY_CHANNELS is defined.
 * Otherwise, scanning stops after the initial scan of the primary channel set.
 *
 * Routers and end devices should scan for joinable networks when directed by
 * the application.  Scanning for joinable networks enables classic Zigbee
 * commissioning with non-ZLL devices.
 *
 * Note, for Zigbee 3.0 compatability, an application should use
 * ::emberAfPluginNetworkSteeringStart rather than this routine, which may
 * be deprecated in a future release.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberAfZllScanForJoinableNetwork(void);
#else
  #define emberAfZllScanForJoinableNetwork emberAfStartSearchForJoinableNetworkCallback
#endif

/** @brief Scans for an unused PAN ID.
 *
 * This function scans the primary channel set for a channel with low
 * average energy and then selects a PAN ID that is not in use on that channel.
 * The plugin will then form a ZLL network on that channel with the chosen PAN
 * ID.
 *
 * Factory new routers may form a new ZLL network at startup, but this is not
 * mandatory, since a network will be created during touch linking if required.
 * Similarly, a router may chose to form a new ZLL network if classical ZigBee
 * commissioning has failed, but this is not mandatory either. However, if a router
 * choses not to form a network, then the application should call
 * ::emberAfPluginZllCommissioningInitCallback to ensure the radio is switched on
 * and is tuned to the default channel. Alternatively, the 'Rx always on' plugin
 * option, ::EMBER_AF_PLUGIN_ZLL_COMMISSIONING_RX_ALWAYS_ON, may be set to true.
 *
 * End devices should not use this API and should instead form ZLL networks via
 * touch linking (or to be precise, join a ZLL network formed by a router target
 * or a router initiator).
 *
 * Note, for Zigbee 3.0 compatability, a router device should use
 * ::emberAfPluginNetworkCreatorStart with centralizedNetwork=false to form
 * a distributed network rather than this routine, which may be deprecated
 * in a future release.
 *
 * @return An ::EmberStatus value that indicates the success or failure of the
 * command.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberAfZllScanForUnusedPanId(void);
#else
  #define emberAfZllScanForUnusedPanId emberAfFindUnusedPanIdAndFormCallback
#endif

// The exponent of the number of scan periods, where a scan period is 960
// symbols, and a symbol is 16 miscroseconds.  The scan will occur for
// ((2^duration) + 1) scan periods.  The ZLL specification requires routers to
// scan for joinable networks using a duration of 4.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SCAN_DURATION 4

#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_MILLISECONDS_DELAY \
  (EMBER_AF_PLUGIN_ZLL_COMMISSIONING_TOUCH_LINK_SECONDS_DELAY * MILLISECOND_TICKS_PER_SECOND)

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
  #define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ADDITIONAL_STATE EMBER_ZLL_STATE_ADDRESS_ASSIGNMENT_CAPABLE
#else
  #define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_ADDITIONAL_STATE EMBER_ZLL_STATE_NONE
#endif

// Internal APIs.
EmberStatus emAfZllFormNetwork(uint8_t channel, int8_t power, EmberPanId panId);
void emAfZllStackStatus(EmberStatus status);

#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_LINK_INITIATOR
extern uint16_t emAfZllIdentifyDurationS;
#endif
extern uint8_t emAfZllExtendedPanId[];
