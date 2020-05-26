// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"
#include EMBER_AF_API_NETWORK_STEERING

/*
 * At bootup EndDevice will look for a network. And If network not found then it
 * will try to search a network at every 10 seconds.
 */
#define NETWORK_STEERING_DELAY_MS 1000*10

/*
 * Event to control network steering
 */
EmberEventControl emberAfPluginNetworkSteeringAutomateMonitorEventControl;
void emberAfPluginNetworkSteeringAutomateMonitorEventHandler(void);

/*
 * Schedule function for network steering.
 * This function will call at NETWORK_STEERING_DELAY_MS interval until
 * EndNode does not join into any network.
 */
void scheduleNetworkSteering(void);


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
	emberAfCorePrintln("Network Steering Completed: %p (0x%X)",
			(status == EMBER_SUCCESS ? "Join Success" : "FAILED"), status);
	emberAfCorePrintln("Finishing state: 0x%X", finalState);
	emberAfCorePrintln("Beacons heard: %d\nJoin Attempts: %d", totalBeacons,
			joinAttempts);
	if (status != EMBER_SUCCESS) {
		scheduleNetworkSteering();
	}
}

void emberAfPluginNetworkSteeringAutomateMonitorEventHandler(void)
{
	emberEventControlSetInactive(
			emberAfPluginNetworkSteeringAutomateMonitorEventControl);

	/*
	 * Check if device has joined to any network.
	 * - Start network steering if device is not joined to any network
	 */
	if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
		emberAfPluginNetworkSteeringStart();
		emberAfCorePrintln("Network Steering start");
	}
}

void scheduleNetworkSteering(void)
{
	emberEventControlSetDelayMS(
			emberAfPluginNetworkSteeringAutomateMonitorEventControl,
			NETWORK_STEERING_DELAY_MS);
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
	BootloaderInformation_t bootloaderInfo;
	bootloader_getInfo(&bootloaderInfo);
	bootloaderInfo.version &= 0x000F;
	emberAfCorePrintln("Bootloader version is : %d, Application Version is : %d",
			bootloaderInfo.version,
			EMBER_AF_PLUGIN_OTA_CLIENT_POLICY_FIRMWARE_VERSION);

	emberAfCorePrintln("emberAfMainInitCallback Network State : %d",emberAfNetworkState());

	/* Schedule network steering */
	scheduleNetworkSteering();
}
