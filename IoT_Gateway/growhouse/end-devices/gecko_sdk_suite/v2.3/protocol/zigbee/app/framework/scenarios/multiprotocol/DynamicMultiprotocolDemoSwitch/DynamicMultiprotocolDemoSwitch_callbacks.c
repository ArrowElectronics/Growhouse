//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h" // emberAfGetBindingTableSize
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "app/framework/plugin/dmp-ui-demo/dmp-ui.h"

#include EMBER_AF_API_NETWORK_STEERING
#include EMBER_AF_API_ZLL_PROFILE
#include EMBER_AF_API_FIND_AND_BIND_INITIATOR

#define SWITCH_ENDPOINT (1)
static bool intendDeassociate = false;

// Custom event stubs. Custom events will be run along with all other events in
// the application framework. They should be managed using the Ember Event API
// documented in stack/include/events.h

// Event control struct declarations
EmberEventControl commissioningEventControl;
EmberEventControl buttonEventControl;
EmberEventControl lcdPermitJoinEventControl;
EmberEventControl lcdMainMenuDisplayEventControl;

// Event function forward declarations
void commissioningEventHandler(void);
void buttonEventHandler(void);
void lcdPermitJoinEventHandler(void);
void lcdMainMenuDisplayEventHandler(void);

// Event function
void commissioningEventHandler(void)
{
  EmberStatus status;
  EmberNetworkStatus state = emberAfNetworkState();
  emberEventControlSetInactive(commissioningEventControl);

  switch (state) {
    case EMBER_NO_NETWORK:
    {
      EmberEUI64 wildcardEui64 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
      EmberKeyData centralizedKey = { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C, 0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 };
      emberAddTransientLinkKey(wildcardEui64, &centralizedKey);
      dmpUiDisplayZigBeeState(DMP_UI_JOINING);
      status = emberAfPluginNetworkSteeringStart();
      break;
    }

    case EMBER_JOINED_NETWORK_NO_PARENT:
    {
      status = emberFindAndRejoinNetwork(true, 0);
      break;
    }

    case EMBER_JOINING_NETWORK:
    case EMBER_LEAVING_NETWORK:
      break;

    case EMBER_JOINED_NETWORK:
      if (intendDeassociate == true) {
        status = emberLeaveNetwork();
        emberClearBindingTable();
      }
      break;
  }
}

void buttonEventHandler(void)
{
  EmberStatus status;
  emberEventControlSetInactive(buttonEventControl);

  EmberNetworkStatus state = emberAfNetworkState();
  if (state == EMBER_JOINED_NETWORK) {
    emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND
                               | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER),
                              ZCL_ON_OFF_CLUSTER_ID,
                              ZCL_TOGGLE_COMMAND_ID,
                              "");
    emberAfGetCommandApsFrame()->profileId           = emberAfProfileIdFromIndex(0);
    emberAfGetCommandApsFrame()->sourceEndpoint      = emberAfEndpointFromIndex(0);

    status = emberAfSendCommandUnicastToBindings();

    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("Sent toggle via binding: 0x%X", status);
      emberAfGetCommandApsFrame()->destinationEndpoint = 0x01;
      status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0); // It is assuemed that this switch is working with DMPLight demo
      emberAfCorePrintln("Sent toggle via ucast 0x%X", status);
    }
  }
}

void lcdMainMenuDisplayEventHandler(void)
{
  emberEventControlSetInactive(lcdMainMenuDisplayEventControl);
  dmpUiLightOff();
}

static void startFindingAndBindingForInitiator(void)
{
  EmberStatus status = emberAfPluginFindAndBindInitiatorStart(SWITCH_ENDPOINT);
  emberAfCorePrintln("Find and bind initiator %p: 0x%X", "start", status);
}

/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback(int8u button,
                                 int8u state)
{
  static uint16_t buttonPressTime;
  uint16_t currentTime = 0;

  if (button == BUTTON1) {
    if (state == BUTTON_PRESSED) {
      buttonPressTime = halCommonGetInt16uMillisecondTick();
    } else if (state == BUTTON_RELEASED) {
      currentTime = halCommonGetInt16uMillisecondTick();

      if ((currentTime - buttonPressTime) > 3000) {
        intendDeassociate = true;
      }
    }

    emberEventControlSetActive(commissioningEventControl);
  } else if (button == BUTTON0
             && state == BUTTON_PRESSED
             && !emberEventControlGetActive(buttonEventControl)) {
    emberEventControlSetActive(buttonEventControl);
  }
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
boolean emberAfStackStatusCallback(EmberStatus status)
{
  switch (status) {
    case EMBER_NETWORK_UP:
      dmpUiDisplayZigBeeState(DMP_UI_NETWORK_UP);
      break;

    case EMBER_NETWORK_DOWN:
      dmpUiLightOff();
      dmpUiDisplayZigBeeState(DMP_UI_NO_NETWORK);
      intendDeassociate = false;

      // when the switch leave the network, it need to clear the binding table
      // it will broadcast leave command, the DMP light will remove the binding
      // for the switch in its local binding
      if (emberNetworkState() == EMBER_NO_NETWORK) {
        emberClearBindingTable();
      }
      break;
  }

  return false;
}

/** @brief Complete
 *
 * This callback is fired by the initiator when the Find and Bind process is
 * complete.
 *
 * @param status Status code describing the completion of the find and bind
 * process Ver.: always
 */
void emberAfPluginFindAndBindInitiatorCompleteCallback(EmberStatus status)
{
  emberAfCorePrintln("Find and bind initiator %p: 0x%X", "complete", status);

  //if the switch has successfully found a light and set local binding entry for sending Onoff command
  //it then need to set binding on the light to enable the reporting feature on the light

  if (status == EMBER_SUCCESS) {
    EmberBindingTableEntry bindingEntry;
    uint8_t bindingTableSize = emberAfGetBindingTableSize();
    uint8_t i;

    for (i = 0; i < bindingTableSize; i++) {
      if (emberGetBinding(i, &bindingEntry) == EMBER_SUCCESS) {
        if ((bindingEntry.type == EMBER_UNICAST_BINDING)
            && (bindingEntry.clusterId == ZCL_ON_OFF_CLUSTER_ID)) {
          EmberNodeId destinationNodeId;
          destinationNodeId = emberLookupNodeIdByEui64(bindingEntry.identifier);
          emberBindRequest(destinationNodeId,
                           bindingEntry.identifier,
                           bindingEntry.remote,
                           ZCL_ON_OFF_CLUSTER_ID,
                           UNICAST_BINDING,
                           emberGetEui64(),
                           0,    // multicast group identifier - ignored
                           bindingEntry.local,
                           EMBER_AF_DEFAULT_APS_OPTIONS);
          break;
        }
      }
    }
  }
}

/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  emberAfCorePrintln("%p network %p: 0x%X", "Join", "complete", status);

  if (status != EMBER_SUCCESS) {
    dmpUiDisplayZigBeeState(DMP_UI_NO_NETWORK);
  } else {
    startFindingAndBindingForInitiator();
  }
}

/** @brief Touch Link Complete
 *
 * This function is called by the ZLL Commissioning plugin when touch linking
 * completes.
 *
 * @param networkInfo The ZigBee and ZLL-specific information about the network
 * and target. Ver.: always
 * @param deviceInformationRecordCount The number of sub-device information
 * records for the target. Ver.: always
 * @param deviceInformationRecordList The list of sub-device information
 * records for the target. Ver.: always
 */
void emberAfPluginZllCommissioningTouchLinkCompleteCallback(const EmberZllNetwork *networkInfo,
                                                            uint8_t deviceInformationRecordCount,
                                                            const EmberZllDeviceInfoRecord *deviceInformationRecordList)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Touchlink",
                     "complete",
                     EMBER_SUCCESS);

  startFindingAndBindingForInitiator();
}

/** @brief Touch Link Failed
 *
 * This function is called by the ZLL Commissioning plugin if touch linking
 * fails.
 *
 * @param status The reason the touch link failed. Ver.: always
 */
void emberAfPluginZllCommissioningTouchLinkFailedCallback(EmberAfZllCommissioningStatus status)
{
  emberAfCorePrintln("%p network %p: 0x%X",
                     "Touchlink",
                     "complete",
                     EMBER_ERR_FATAL);
}

/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
void emberAfMainTickCallback(void)
{
}

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
  dmpUiInit();
  dmpUiDisplayHelp();
  emberEventControlSetDelayMS(lcdMainMenuDisplayEventControl, 10000);
}

/** @brief Message Sent
 *
 * This function is called by the application framework from the message sent
 * handler, when it is informed by the stack regarding the message sent status.
 * All of the values passed to the emberMessageSentHandler are passed on to this
 * callback. This provides an opportunity for the application to verify that its
 * message has been sent successfully and take the appropriate action. This
 * callback should return a bool value of true or false. A value of true
 * indicates that the message sent notification has been handled and should not
 * be handled by the application framework.
 *
 * @param type   Ver.: always
 * @param indexOrDestination   Ver.: always
 * @param apsFrame   Ver.: always
 * @param msgLen   Ver.: always
 * @param message   Ver.: always
 * @param status   Ver.: always
 */
boolean emberAfMessageSentCallback(EmberOutgoingMessageType type,
                                   int16u indexOrDestination,
                                   EmberApsFrame* apsFrame,
                                   int16u msgLen,
                                   int8u* message,
                                   EmberStatus status)
{
  return false;
}

/** @brief Pre Message Received
 *
 * This callback is the first in the Application Framework's message processing
 * chain. The Application Framework calls it when a message has been received
 * over the air but has not yet been parsed by the ZCL command-handling code. If
 * you wish to parse some messages that are completely outside the ZCL
 * specification or are not handled by the Application Framework's command
 * handling code, you should intercept them for parsing in this callback.

 *   This callback returns a Boolean value indicating whether or not the message
 * has been handled. If the callback returns a value of true, then the
 * Application Framework assumes that the message has been handled and it does
 * nothing else with it. If the callback returns a value of false, then the
 * application framework continues to process the message as it would with any
 * incoming message.
        Note:   This callback receives a pointer to an
 * incoming message struct. This struct allows the application framework to
 * provide a unified interface between both Host devices, which receive their
 * message through the ezspIncomingMessageHandler, and SoC devices, which
 * receive their message through emberIncomingMessageHandler.
 *
 * @param incomingMessage   Ver.: always
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
  return false;
}

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
  return false;
}

/** @brief Report Attributes
 *
 * This function is called by the application framework when a Report Attributes
 * command is received from an external device.  The application should return
 * true if the message was processed or false if it was not.
 *
 * @param clusterId The cluster identifier of this command.  Ver.: always
 * @param buffer Buffer containing the list of attribute report records.  Ver.:
 * always
 * @param bufLen The length in bytes of the list.  Ver.: always
 */
boolean emberAfReportAttributesCallback(EmberAfClusterId clusterId,
                                        int8u *buffer,
                                        int16u bufLen)
{
  uint16_t bufIndex = 0;
  bool on = false;

  EmberAfAttributeId attributeId;
  attributeId = (EmberAfAttributeId)emberAfGetInt16u(buffer,
                                                     bufIndex,
                                                     bufLen);

  if ((clusterId == ZCL_ON_OFF_CLUSTER_ID) && (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID)) {
    on = buffer[3];
  }

  if (on == false) {
    dmpUiLightOff();
  } else {
    dmpUiLightOn();
  }

  emberAfSendDefaultResponse(emberAfCurrentCommand(), EMBER_ZCL_STATUS_SUCCESS);
  return TRUE;
}
