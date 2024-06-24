//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include EMBER_AF_API_AF_HEADER
#include EMBER_AF_API_NETWORK_CREATOR

EmberEventControl commissioningEventControl;

void commissioningEventHandler(void)
{
  EmberStatus status;

  emberEventControlSetInactive(commissioningEventControl);

  if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
    status = emberAfPluginNetworkCreatorStart(true); // centralized
    emberAfCorePrintln("%p network %p: 0x%X",
                       "Form centralized",
                       "start",
                       status);
  }
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
void emberAfHalButtonIsrCallback(uint8_t button,
                                 uint8_t state)
{
  if (state == BUTTON_RELEASED) {
    emberEventControlSetActive(commissioningEventControl);
  }
}
