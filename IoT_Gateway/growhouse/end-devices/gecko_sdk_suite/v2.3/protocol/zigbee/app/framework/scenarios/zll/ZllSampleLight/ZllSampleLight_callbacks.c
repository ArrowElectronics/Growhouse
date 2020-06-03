// Copyright 2014 Silicon Laboratories, Inc.
//
//

// This callback file is created for your convenience. You may add application code
// to this file. If you regenerate this file over a previous version, the previous
// version will be overwritten and any code you have added will be lost.

#include "app/framework/include/af.h"
#include "app/framework/plugin/zll-commissioning/zll-commissioning.h"

// Event control struct declarations
EmberEventControl identifyEventControl;
EmberEventControl networkEventControl;

static uint8_t PGM happyTune[] = {
  NOTE_B4, 1,
  0, 1,
  NOTE_B5, 1,
  0, 0
};
static uint8_t PGM sadTune[] = {
  NOTE_B5, 1,
  0, 1,
  NOTE_B4, 5,
  0, 0
};
static uint8_t PGM waitTune[] = {
  NOTE_B4, 1,
  0, 0
};

static bool onOff = false;
static uint32_t identifyDurationMs = 0; // milliseconds
#define DEFAULT_IDENTIFY_DURATION_MS (3 * MILLISECOND_TICKS_PER_SECOND)
#define LED BOARDLED1

void identifyEventHandler(void)
{
  if (identifyDurationMs == 0) {
    if (onOff) {
      halSetLed(LED);
    } else {
      halClearLed(LED);
    }
    emberEventControlSetInactive(identifyEventControl);
  } else {
    halToggleLed(LED);
    if (identifyDurationMs >= MILLISECOND_TICKS_PER_QUARTERSECOND) {
      identifyDurationMs -= MILLISECOND_TICKS_PER_QUARTERSECOND;
    } else {
      identifyDurationMs = 0;
    }
    emberEventControlSetDelayMS(identifyEventControl,
                                MILLISECOND_TICKS_PER_QUARTERSECOND);
  }
}

void networkEventHandler(void)
{
  if (emberAfZllScanForUnusedPanId() == EMBER_SUCCESS) {
    emberEventControlSetInactive(networkEventControl);
    halPlayTune_P(waitTune, true);
  } else {
    emberAfZllResetToFactoryNew();
    emberEventControlSetActive(networkEventControl);
    halPlayTune_P(sadTune, true);
  }
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be
 * notified of changes to the stack status and take appropriate action.  The
 * return code from this callback is ignored by the framework.  The framework
 * will always process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
  if (status == EMBER_NETWORK_UP) {
    emberEventControlSetInactive(networkEventControl);
    halPlayTune_P(happyTune, true);
  } else if (status == EMBER_NETWORK_DOWN
             && emberAfNetworkState() == EMBER_NO_NETWORK) {
    emberEventControlSetActive(networkEventControl);
  }
  return false;
}

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void emberAfMainInitCallback(void)
{
  emberEventControlSetActive(networkEventControl);
}

/** @brief On/off Cluster Server Post Init
 *
 * Following resolution of the On/Off state at startup for this endpoint, perform any
 * additional initialization needed; e.g., synchronize hardware state.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPluginOnOffClusterServerPostInitCallback(uint8_t endpoint)
{
  // At startup, trigger a read of the attribute and possibly a toggle of the
  // LED to make sure they are always in sync.
  emberAfOnOffClusterServerAttributeChangedCallback(endpoint,
                                                    ZCL_ON_OFF_ATTRIBUTE_ID);
}

/** @brief Server Attribute Changed
 *
 * On/off cluster, Server Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfOnOffClusterServerAttributeChangedCallback(uint8_t endpoint,
                                                       EmberAfAttributeId attributeId)
{
  // When the on/off attribute changes, set the LED appropriately.  If an error
  // occurs, ignore it because there's really nothing we can do.
  if (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID) {
    if (emberAfReadServerAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff))
        == EMBER_ZCL_STATUS_SUCCESS) {
      if (onOff) {
        halSetLed(LED);
      } else {
        halClearLed(LED);
      }
    }
  }
}

/** @brief Get Group Name
 *
 * This function returns the name of a group with the provided group ID,
 * should it exist.
 *
 * @param endpoint Endpoint  Ver.: always
 * @param groupId Group ID  Ver.: always
 * @param groupName Group Name  Ver.: always
 */
void emberAfPluginGroupsServerGetGroupNameCallback(uint8_t endpoint,
                                                   uint16_t groupId,
                                                   uint8_t *groupName)
{
}

/** @brief Set Group Name
 *
 * This function sets the name of a group with the provided group ID.
 *
 * @param endpoint Endpoint  Ver.: always
 * @param groupId Group ID  Ver.: always
 * @param groupName Group Name  Ver.: always
 */
void emberAfPluginGroupsServerSetGroupNameCallback(uint8_t endpoint,
                                                   uint16_t groupId,
                                                   uint8_t *groupName)
{
}

/** @brief Group Names Supported
 *
 * This function returns whether or not group names are supported.
 *
 * @param endpoint Endpoint  Ver.: always
 */
bool emberAfPluginGroupsServerGroupNamesSupportedCallback(uint8_t endpoint)
{
  return false;
}

/** @brief Ok To Sleep
 *
 * This function is called by the Idle/Sleep plugin before sleeping.  The
 * application should return true if the device may sleep or false otherwise.
 *
 * @param durationMs The maximum duration in milliseconds that the device will
 * sleep.  Ver.: always
 */
bool emberAfPluginIdleSleepOkToSleepCallback(uint32_t durationMs)
{
  return false;
}

/** @brief Wake Up
 *
 * This function is called by the Idle/Sleep plugin after sleeping.
 *
 * @param durationMs The duration in milliseconds that the device slept.
 * Ver.: always
 */
void emberAfPluginIdleSleepWakeUpCallback(uint32_t durationMs)
{
}

/** @brief Ok To Idle
 *
 * This function is called by the Idle/Sleep plugin before idling.  The
 * application should return true if the device may idle or false otherwise.
 *
 */
bool emberAfPluginIdleSleepOkToIdleCallback(void)
{
  return true;
}

/** @brief Active
 *
 * This function is called by the Idle/Sleep plugin after idling.
 *
 */
void emberAfPluginIdleSleepActiveCallback(void)
{
}

/** @brief Initial Security State
 *
 * This function is called by the ZLL Commissioning plugin to determine the
 * initial security state to be used by the device.  The application must
 * populate the ::EmberZllInitialSecurityState structure with a configuration
 * appropriate for the network being formed, joined, or started.  Once the
 * device forms, joins, or starts a network, the same security configuration
 * will remain in place until the device leaves the network.
 *
 * @param securityState The security configuration to be populated by the
 * application and ultimately set in the stack.  Ver.: always
 */
void emberAfPluginZllCommissioningInitialSecurityStateCallback(EmberZllInitialSecurityState *securityState)
{
  // By default, the plugin will configure the stack for the certification
  // encryption algorithm.  Devices that are certified should instead use the
  // master encryption algorithm and set the appropriate encryption key and
  // pre-configured link key.
}

/** @brief Touch Link Complete
 *
 * This function is called by the ZLL Commissioning plugin when touch linking
 * completes.
 *
 * @param networkInfo The ZigBee and ZLL-specific information about the
 * network and target.  Ver.: always
 * @param deviceInformationRecordCount The number of sub-device information
 * records for the target.  Ver.: always
 * @param deviceInformationRecordList The list of sub-device information
 * records for the target.  Ver.: always
 */
void emberAfPluginZllCommissioningTouchLinkCompleteCallback(const EmberZllNetwork *networkInfo,
                                                            uint8_t deviceInformationRecordCount,
                                                            const EmberZllDeviceInfoRecord *deviceInformationRecordList)
{
  halPlayTune_P(happyTune, true);
}

/** @brief Touch Link Failed
 *
 * This function is called by the ZLL Commissioning plugin if touch linking
 * fails.
 *
 * @param status The reason the touch link failed.  Ver.: always
 */
void emberAfPluginZllCommissioningTouchLinkFailedCallback(EmberAfZllCommissioningStatus status)
{
  halPlayTune_P(sadTune, true);
}

/** @brief Group Identifier Count
 *
 * This function is called by the ZLL Commissioning plugin to determine the
 * number of group identifiers in use by a specific endpoint on the device.
 * The total number of group identifiers on the device, which are shared by
 * all endpoints, is defined by ::EMBER_ZLL_GROUP_ADDRESSES.
 *
 * @param endpoint The endpoint for which the group identifier count is
 * requested.  Ver.: always
 */
uint8_t emberAfPluginZllCommissioningGroupIdentifierCountCallback(uint8_t endpoint)
{
  // Devices with multiple endpoints will have to distribute the group ids
  // available to the device among the individual endpoints.  This application
  // has one endpoint, so it uses all available group ids.
  return EMBER_ZLL_GROUP_ADDRESSES;
}

/** @brief Group Identifier
 *
 * This function is called by the ZLL Commissioning plugin to obtain
 * information about the group identifiers in use by a specific endpoint on
 * the device.  The application should populate the record with information
 * about the group identifier and return true.  If no information is available
 * for the given endpoint and index, the application should return false.
 *
 * @param endpoint The endpoint for which the group identifier is requested.
 * Ver.: always
 * @param index The index of the group on the endpoint.  Ver.: always
 * @param record The group information record.  Ver.: always
 */
bool emberAfPluginZllCommissioningGroupIdentifierCallback(uint8_t endpoint,
                                                          uint8_t index,
                                                          EmberAfPluginZllCommissioningGroupInformationRecord *record)
{
  // Devices with multiple endpoints will have to distribute the group ids
  // available to the device among the individual endpoints.  This application
  // has one endpoint, so it uses all available group ids in order.
  EmberTokTypeStackZllData token;
  emberZllGetTokenStackZllData(&token);
  record->groupId = token.myGroupIdMin + index;
  record->groupType = 0x00; // fixed at zero
  return true;
}

/** @brief Endpoint Information Count
 *
 * This function is called by the ZLL Commissioning plugin to determine the
 * number of remote endpoints controlled by a specific endpoint on the local
 * device.
 *
 * @param endpoint The local endpoint for which the remote endpoint
 * information count is requested.  Ver.: always
 */
uint8_t emberAfPluginZllCommissioningEndpointInformationCountCallback(uint8_t endpoint)
{
  // Devices that control remote endpoints individually will have to maintain
  // a list of those endpoints.  This application does not control remote
  // endpoints individually.
  return 0x00;
}

/** @brief Endpoint Information
 *
 * This function is called by the ZLL Commissioning plugin to obtain
 * information about the remote endpoints controlled by a specific endpoint on
 * the local device.  The application should populate the record with
 * information about the remote endpoint and return true.  If no information
 * is available for the given endpoint and index, the application should
 * return false.
 *
 * @param endpoint The local endpoint for which the remote endpoint
 * information is requested.  Ver.: always
 * @param index The index of the remote endpoint information on the local
 * endpoint.  Ver.: always
 * @param record The endpoint information record.  Ver.: always
 */
bool emberAfPluginZllCommissioningEndpointInformationCallback(uint8_t endpoint,
                                                              uint8_t index,
                                                              EmberAfPluginZllCommissioningEndpointInformationRecord *record)
{
  // Devices that control remote endpoints individually will have to maintain
  // a list of those endpoints.  This application does not control remote
  // endpoints individually.
  return false;
}

/** @brief Identify
 *
 * This function is called by the ZLL Commissioning plugin to notify the
 * application that it should take an action to identify itself.  This
 * typically occurs when an Identify Request is received via inter-PAN
 * messaging.
 *
 * @param durationS If the duration is zero, the device should exit identify
 * mode.  If the duration is 0xFFFF, the device should remain in identify mode
 * for the default time.  Otherwise, the duration specifies the length of time
 * in seconds that the device should remain in identify mode.  Ver.: always
 */
void emberAfPluginZllCommissioningIdentifyCallback(uint16_t durationS)
{
  if (durationS != 0) {
    halStackIndicatePresence();
  }
  identifyDurationMs = (durationS == 0xFFFF
                        ? DEFAULT_IDENTIFY_DURATION_MS
                        : durationS * MILLISECOND_TICKS_PER_SECOND);
  emberEventControlSetActive(identifyEventControl);
}

/** @brief Reset To Factory New
 *
 * This function is called by the ZLL Commissioning plugin when a request to
 * reset to factory new is received.  The plugin will leave the network, reset
 * attributes managed by the framework to their default values, and clear the
 * group and scene tables.  The application should perform any other necessary
 * reset-related operations in this callback, including resetting any
 * externally-stored attributes.
 *
 */
void emberAfPluginZllCommissioningResetToFactoryNewCallback(void)
{
}

/** @brief Join
 *
 * This callback is called by the ZLL Commissioning plugin when a joinable
 * network has been found.  If the application returns true, the plugin will
 * attempt to join the network.  Otherwise, the plugin will ignore the network
 * and continue searching.  Applications can use this callback to implement a
 * network blacklist.  Note that this callback is not called during touch
 * linking.
 *
 * @param networkFound   Ver.: always
 * @param lqi   Ver.: always
 * @param rssi   Ver.: always
 */
bool emberAfPluginZllCommissioningJoinCallback(EmberZigbeeNetwork *networkFound,
                                               uint8_t lqi,
                                               int8_t rssi)
{
  return true;
}

/** @brief Off With Effect
 *
 * This callback is called by the ZLL On/Off Server plugin whenever an
 * OffWithEffect command is received.  The application should implement the
 * effect and variant requested in the command and return
 * ::EMBER_ZCL_STATUS_SUCCESS if successful or an appropriate error status
 * otherwise.
 *
 * @param endpoint   Ver.: always
 * @param effectId   Ver.: always
 * @param effectVariant   Ver.: always
 */
EmberAfStatus emberAfPluginZllOnOffServerOffWithEffectCallback(uint8_t endpoint,
                                                               uint8_t effectId,
                                                               uint8_t effectVariant)
{
  // This sample application is not capable of implementing the effects and
  // variants, so it simply returns SUCCESS.  A real device should perform the
  // requested effect and variant and return an appropriate status.
  return EMBER_ZCL_STATUS_SUCCESS;
}
