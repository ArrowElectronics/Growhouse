/**
 * @file network-formation.h
 * See @ref network_formation for documentation.
 *
 * <!--Copyright 2004-2007 by Ember Corporation. All rights reserved.    *80*-->
 */

#ifndef SILABS_NETWORK_INFORMATION_H
#define SILABS_NETWORK_INFORMATION_H

/**
 * @addtogroup network_formation
 * @brief EmberZNet API for finding, forming, joining, and leaving
 * Zigbee networks.
 *
 * See network-formation.h for source code.
 * @{
 */

/** @brief Initializes the radio and the EmberZNet stack.
 *
 * Device configuration functions must be called before ::emberInit()
 * is called.
 *
 * @note The application must check the return value of this function. If the
 * initialization fails, normal messaging functions will not be available.
 * Some failure modes are not fatal, but the application must follow certain
 * procedures to permit recovery.
 * Ignoring the return code will result in unpredictable radio and API behavior.
 * (In particular, problems with association will occur.)
 *
 * @return An ::EmberStatus value indicating successful initialization or the
 * reason for failure.
 */
EmberStatus emberInit(void);

/** @brief A periodic tick routine that should be called:
 * - in the application's main event loop,
 * - as soon as possible after any radio interrupts, and
 * - after ::emberInit().
 */
void emberTick(void);

/** @brief Resumes network operation after a reboot.
 *
 *   It is required that this be called on boot prior to **any**
 *   network operations. It will initialize the networking system
 *   and attempt to resume the previous network identity and configuration.
 *   If the node was not previously joined, this routine should still be called.
 *
 *   If the node was previously joined to a network, it will retain its original
 *   type (e.g., coordinator, router, end device, and so on.)
 *
 *   ::EMBER_NOT_JOINED is returned
 *   if the node is not part of a network.
 *
 *.  This function has encapsulated the old behavior of emberNetworkInitExtended().
 *
 * @param networkInitStruct  Defines whether an orphan scan/rejoin request/or neither
 *   is performed during network initialization.
 *
 * @return An ::EmberStatus value that indicates one of the following:
 * - successful initialization,
 * - ::EMBER_NOT_JOINED if the node is not part of a network, or
 * - the reason for failure.
 */
EmberStatus emberNetworkInit(EmberNetworkInitStruct* networkInitStruct);

/** @brief Forms a new network by becoming the coordinator.
 *
 * @note If using security, the application must call
 *   ::emberSetInitialSecurityState() prior to joining the network.  This also
 *   applies when a device leaves a network and wants to form another one.

 * @param parameters  Specification of the new network.
 *
 * @return An ::EmberStatus value that indicates either the successful formation
 * of the new network, or the reason that the network formation failed.
 */
EmberStatus emberFormNetwork(EmberNetworkParameters *parameters);

/** @brief Tells the stack to allow other nodes to join the network
 * with this node as their parent. Joining is initially disabled by default.
 * This function may only be called after the node is part of a network
 * and the stack is up.
 *
 * @param duration  A value of 0x00 disables joining. A value of 0xFF
 *  enables joining. Any other value enables joining for that number of
 *  seconds.
 */
EmberStatus emberPermitJoining(uint8_t duration);

/** @brief Causes the stack to associate with the network using the
 * specified network parameters. It can take several seconds for the stack to
 * associate with the local network. Do not send messages until a call to the
 * ::emberStackStatusHandler() callback informs you that the stack is up.
 *
 * @note If using security, the application must call
 *   ::emberSetInitialSecurityState() prior to joining the network.  This also
 *   applies when a device leaves a network and wants to join another one.
 *
 * @param nodeType    Specification of the role that this node will have in
 *   the network. This role must not be ::EMBER_COORDINATOR. To be a coordinator,
 *   call ::emberFormNetwork().
 *
 * @param parameters  Specification of the network with which the node
 *   should associate.
 *
 * @return An ::EmberStatus value that indicates either:
 * - that the association process began successfully, or
 * - the reason for failure.
 */
EmberStatus emberJoinNetwork(EmberNodeType nodeType,
                             EmberNetworkParameters *parameters);

/** @brief Causes the stack to leave the current network.
 * This generates a call to the ::emberStackStatusHandler() callback to indicate
 * that the network is down. The radio will not be used until after a later call
 * to ::emberFormNetwork() or ::emberJoinNetwork().
 *
 * @return An ::EmberStatus value indicating success or reason for failure.
 * A status of ::EMBER_INVALID_CALL indicates that the node is either not
 * joined to a network or is already in the process of leaving.
 */
EmberStatus emberLeaveNetwork(void);

/** @brief Sends a Zigbee NWK leave command to the specified destination.
 *
 * @param destination is the node ID of the device that is being told to
 *   leave.
 *
 * @param flags is a bitmask indicating additional considerations for
 *   the leave request.  See the ::EmberLeaveRequestFlags enumeration for more
 *   information.  Multiple bits may be set.
 *
 * @return An ::EmberStatus value indicating success or reason for failure.
 * A status of ::EMBER_INVALID_CALL indicates that the node not currently
 * joined to the network, or the destination is the local node.  To tell
 * the local device to leave, use the ::emberLeaveNetwork() API.
 */
EmberStatus emberSendZigbeeLeave(EmberNodeId destination,
                                 EmberLeaveRequestFlags flags);

/** @brief The application may call this function when contact with the
 * network has been lost. The most common use case is when an end device
 * can no longer communicate with its parent and wishes to find a new one.
 * Another case is when a device has missed a Network Key update and no
 * longer has the current Network Key.
 *
 * Note that a call to ::emberPollForData() on an end device that has lost
 * contact with its parent will automatically call ::emberRejoinNetwork(true).
 *
 * The stack will call ::emberStackStatusHandler() to indicate that the network
 * is down, then try to re-establish contact with the network by performing
 * an active scan, choosing a network with matching extended pan ID, and
 * sending a Zigbee network rejoin request.  A second call to the
 * ::emberStackStatusHandler() callback indicates either the success
 * or the failure of the attempt.  The process takes
 * approximately 150 milliseconds per channel to complete.
 *
 * This call replaces the ::emberMobileNodeHasMoved() API from EmberZNet 2.x,
 * which used MAC association and consequently took half a second longer
 * to complete.
 *
 * @param haveCurrentNetworkKey  This parameter determines whether the request
 * to rejoin the Network is sent encrypted (true) or unencrypted (false). The
 * application should first try to use encryption. If that fails,
 * the application should call this function again and use no encryption.
 * If the unencrypted rejoin is successful then device will be in the joined but
 * unauthenticated state. The Trust Center will be notified of the rejoin and
 * send an updated Network encrypted using the device's Link Key. Sending the
 * rejoin unencrypted is only supported on networks using Standard Security
 * with link keys (i.e., ZigBee 2006 networks do not support it).
 *
 * @param channelMask A mask indicating the channels to be scanned.
 * See ::emberStartScan() for format details.
 *
 * @param reason An enumeration indicating why the rejoin occurred.
 * The stack will set the reason based on the ::EmberRejoinReason.
 * An application should use one of the APP_EVENT rejoin reasons.  The stack
 * will never use these.  Only if the function return code is EMBER_SUCCESS
 * will the rejoin reason be set.
 *
 * @return An ::EmberStatus value indicating success or reason for failure.
 */
EmberStatus emberFindAndRejoinNetworkWithReason(bool haveCurrentNetworkKey,
                                                uint32_t channelMask,
                                                EmberRejoinReason reason);

/** @brief This call is the same emberFindAndRejoinNetworkWithReason(). However, the
 *  reason is assumed to be ::EMBER_REJOIN_REASON_APP_EVENT_1.
 */
EmberStatus emberFindAndRejoinNetwork(bool haveCurrentNetworkKey,
                                      uint32_t channelMask);

/** @brief This call attempts to rejoin the network with a different device type.
 * @param haveCurrentNetworkKey This parameter determines whether the request
 * to rejoin the Network is sent encrypted (true) or unencrypted (false). The
 * application should first try to use encryption. If that fails,
 * the application should call this function again and use no encryption.
 * If the unencrypted rejoin is successful then device will be in the joined but
 * unauthenticated state. The Trust Center will be notified of the rejoin and
 * send an updated Network encrypted using the device's Link Key. Sending the
 * rejoin unencrypted is only supported on networks using Standard Security
 * with link keys (i.e., ZigBee 2006 networks do not support it).
 *
 * @param channelMask A mask indicating the channels to be scanned.
 * See ::emberStartScan() for format details.
 *
 * @param nodeType An enumeration indicating the device type to rejoin as. The
 * stack only accepts ::EMBER_END_DEVICE and ::EMBER_SLEEPY_END_DEVICE.
 */
EmberStatus emberFindAndRejoinNetworkWithNodeType(bool haveCurrentNetworkKey,
                                                  uint32_t channelMask,
                                                  EmberNodeType nodeType);

/** @brief Returns the enumeration for why a previous rejoin.
 */
EmberRejoinReason emberGetLastRejoinReason(void);

/** @brief A convenience function which calls ::emberFindAndRejoinNetwork()
 * with a channel mask value for scanning only the current channel.
 * Included for back-compatibility.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
EmberStatus emberRejoinNetwork(bool haveCurrentNetworkKey);

#else
#define emberRejoinNetwork(haveKey) emberFindAndRejoinNetwork((haveKey), 0)
#endif

/** @brief This function will start a scan. ::EMBER_SUCCESS signals that
 * the scan successfully started. Note that although a scan can be initiated
 * while the node is currently joined to a network, the node will generally
 * be unable to communicate with its PAN during the scan period.
 * Take care when performing scans of any significant duration while
 * presently joined to an existing PAN.
 *
 * Possible error responses and their meanings:
 * - ::EMBER_MAC_SCANNING, already scanning.
 * - ::EMBER_MAC_BAD_SCAN_DURATION, a duration value that is
 *   not 0..14 inclusive is set.
 * - ::EMBER_MAC_INCORRECT_SCAN_TYPE, an undefined
 *   scanning type is requested;
 * - ::EMBER_MAC_INVALID_CHANNEL_MASK, the channel mask did not specify any
 *   valid channels on the current platform.
 *
 * @param scanType     Indicates the type of scan to be performed.
 *  Possible values:  ::EMBER_ENERGY_SCAN, ::EMBER_ACTIVE_SCAN.
 *
 * @param channelMask  Bits set as 1 indicate that this particular channel
 * should be scanned. Bits set to 0 indicate that this particular channel
 * should not be scanned. For example, a channelMask value of 0x00000001
 * indicates that only channel 0 should be scanned. Valid channels range
 * from 11 to 26 inclusive. This translates to a channel mask value of 0x07
 * FF F8 00. As a convenience, a channelMask of 0 is reinterpreted as the
 * mask for the current channel.
 *
 * @param duration     Sets the exponent of the number of scan periods,
 * where a scan period is 960 symbols, and a symbol is 16 microseconds.
 * The scan will occur for ((2^duration) + 1) scan periods.  The value
 * of duration must be less than 15.  The time corresponding to the first
 * few values is as follows: 0 = 31 msec, 1 = 46 msec, 2 = 77 msec,
 * 3 = 138 msec, 4 = 261 msec, 5 = 507 msec, 6 = 998 msec.
 */
EmberStatus emberStartScan(EmberNetworkScanType scanType, uint32_t channelMask, uint8_t duration);

/** @brief Kicks off a procedure to find an unused panId on a low-activity
 * channel that is included in the passed-in channel mask. This procedure
 * uses the emberStartScan() API.
 *
 * @param channelMask The set of channels that will be scanned to find an
 * available panId.
 *
 * @param duration The length of time that will be spent scanning for an
 * available panId.
 */
EmberStatus emberFindUnusedPanId(uint32_t channelMask, uint8_t duration);

/** @brief Terminates a scan in progress.
 *
 * @return Returns ::EMBER_SUCCESS if successful.
 */
EmberStatus emberStopScan(void);

/** @brief Checks if radio_off scan mode is activated and radio has shut down as a result.
 * @return Returns ::true if radio has shut down successfully.
 * There is no EZSP/NCP implementation for this.
 */
bool emberIsRadioOffScanActivated(void);

/** @brief Indicates the status of the current scan. When the scan has
 * completed, the stack will call this function with status set to
 * ::EMBER_SUCCESS. Prior to the scan completing, the stack may call this
 * function with other status values. Non-EMBER_SUCCESS status values indicate
 * that the scan failed to start successfully on the channel indicated by the
 * channel parameter. The current scan is ongoing until the stack calls this
 * function with status set to ::EMBER_SUCCESS.
 *
 * @param channel  The channel on which the current error occurred.
 * Undefined for the case of ::EMBER_SUCCESS.
 *
 * @param status   The error condition that occurred on the current channel.
 * Value will be ::EMBER_SUCCESS when the scan has completed.
 */
void emberScanCompleteHandler(uint8_t channel, EmberStatus status);

/** @brief Reports the maximum RSSI value measured on the channel.
 *
 * @param channel       The 802.15.4 channel number on which the RSSI value.
 *   was measured.
 *
 * @param maxRssiValue  The maximum RSSI value measured (in units of dBm).
 *
 */
void emberEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue);

/** @brief Reports that a network was found and gives the network
 * parameters used for deciding which network to join.
 *
 * @param networkFound A pointer to a ::EmberZigbeeNetwork structure
 *   that contains the discovered network and its associated parameters.
 *
 */
void emberNetworkFoundHandler(EmberZigbeeNetwork *networkFound);

/** @brief This function returns an unused panID and channel pair
 * found via the find unused panId scan procedure.
 *
 * @param panId The unused panID.
 *
 * @param channel The channel which the unused panId was found on.
 */
void emberUnusedPanIdFoundHandler(EmberPanId panId, uint8_t channel);

/** @brief Indicates whether the stack is in the process of performing
 *  a rejoin.
 *
 * @return Returns true if the device is in the process of performing a rejoin.
 *         Returns false otherwise.
 */
bool emberStackIsPerformingRejoin(void);

/** @brief Indicates the reason why the device left the network (if any). This also
 *    will return the device that sent the leave message, if the leave
 *    was due to an over-the-air message.
 *
 *    If returnNodeIdThatSentLeave is a non-NULL pointer, the node ID
 *    of the device that sent the leave message will be written to the value
 *    indicated by the pointer. If the leave was not due to an over-the-air
 *    message (but an internal API call instead), EMBER_UNKNOWN_NODE_ID is
 *    returned.
 *
 *  @return Returns EmberLeaveReason enumeration, or EMBER_LEAVE_REASON_NONE
 *    if the device has not left the network.
 */
EmberLeaveReason emberGetLastLeaveReason(EmberNodeId* returnNodeIdThatSentLeave);

/** @brief Indicates the state of the permit joining in the MAC.
 */
bool emberGetPermitJoining(void);

/** @brief Sets the Network Update ID to the desired value. The Network Update ID
 *    value cannot be manually changed after a network is joined, so this
 *    function must be called before calling emberFormNetwork.
 *
 *  @note This Network Update ID should not normally be changed, as it is used
 *    by the stack to track changes in the network.
 *
 *  @return EMBER_SUCCESS if called when not on network, else EMBER_INVALID_CALL.
 */
EmberStatus emberSetNwkUpdateId(uint8_t nwkUpdateId);

/** @brief Sets the duration of a beacon jitter, in the units used by the 15.4
 *    scan parameter (((1 << duration) + 1) * 15 ms), when responding to a beacon
 *    request.
 *
 *  @note Since the recommended scan duration of a joining device is 3 for 2.4
 *    GHz devices, any value passed in to this function greater than 3 will be
 *    rejected.
 *
 *  @return EMBER_BAD_ARGUMENT if the argument is invalid, else EMBER_SUCCESS.
 */
EmberStatus emberSetBeaconJitterDuration(uint8_t beaconJitterDuration);

/**
 * @brief This function sets the policy decision for Trust Center (insecure)
 *   rejoins for devices using the well-known link key. If rejoining using the
 *   well-known key is allowed, it is disabled again after
 *   emAllowTcRejoinsUsingWellKnownKeyTimeoutSec seconds.
 */
void emberSetTcRejoinsUsingWellKnownKeyAllowed(bool allow);

/**
 * @brief This function gets the policy decision for Trust Center (insecure)
 *   rejoins for devices using the well-known link key.
 */
bool emberTcRejoinsUsingWellKnownKeyAllowed(void);

/*
 * brief Reschedules sending link status message, first one being sent immediately.
 */

void emberRescheduleLinkStatusMsg(void);

/** @brief Configures the minimum RSSI for receiving packets
 *
 * The minimum RSSI is used to classify good/bad parent signal strengths.
 * Any packet in the range of minimum RSSI + 30 db is considered to be "bad".
 * By default this value is set to -100. Currently no EZSP support.
 */
void emberSetMinRSSI(int8_t minRSSI);

/** @brief Enables/diables the parent classification algorithm on the stack
 *  side. Parent classification considers whether a received beacon indicates
 *  trust center connectivity and long uptime on the network.
 *
 */
void emberSetParentClassificationEnabled(bool enabled);

/** @brief Gets the enable state the parent classification algorithm.
 *
 */
bool emberGetParentClassificationEnabled(void);

/** @brief Sets the short PAN ID the device will accept in a NLME Network Update command.
 *
 * If a NLME Network Update command is received by the device specifying
 * a short PAN ID that does not match with the given PAN ID, then the NLME Network
 * Update message will be ignored by the device. A value of 0xFFFF indicates
 * that any short PAN ID received in a NLME Network Update command will be accepted
 * which is also the default value set by the stack.
 *
 * @param panId     A pending network update short PAN ID.
 */
void emberSetPendingNetworkUpdatePanId(uint16_t panId);

/** @} END addtogroup */

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file network-formation.h is new and is described in @ref network_formation.
 * <ul>
 * <li> <b>New items</b>
 *   - emberFindAndRejoinNetwork() - replaced emberMobileNodeHasMoved()
 *   - emberRejoinNetwork()
 *   .
 * <li> <b>Items moved from ember.h</b>
 *   - emberEnergyScanResultHandler()
 *   - emberFormNetwork()
 *   - emberInit()
 *   - emberJoinNetwork()
 *   - emberLeaveNetwork()
 *   - emberNetworkFoundHandler()
 *   - emberNetworkInit()
 *   - emberPermitJoining()
 *   - emberScanCompleteHandler()
 *   - emberStartScan()
 *   - emberStopScan()
 *   - emberTick()
 * </ul>
 * HIDDEN -->
 */

#endif // SILABS_NETWORK_INFORMATION_H
