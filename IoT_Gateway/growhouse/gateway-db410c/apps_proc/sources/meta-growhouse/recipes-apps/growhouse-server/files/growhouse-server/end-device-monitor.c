#include "growhouse-server.h"

#define MQTT_PUB_DEVICE_STATUS          "selene/mqtt/device/monitor"
#define DEVICE_TIMEOUT                  720
#define DEVICE_MONITOR_CB_TIMER         180

// Global store provisioned device list
extern cJSON * provisionedDeviceList;
extern struct mosquitto * mosq;

/* sendDeviceStatus()
 *
 * \brief : this function receives the payload for device status
 *          and publish on topic to selene.
 *
 * \param [in] payload : Pointer to device status payload.
 * \param [in] mosq    : Pointer to instance of mosquitto.
 *
 * \return none
 */
void sendDeviceStatus(char * payload, struct mosquitto * mosq) {

	int status;

	status = mosquitto_publish(mosq, NULL, MQTT_PUB_DEVICE_STATUS, strlen(payload), payload, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_DEVICE_STATUS, status);
	} else {
		logBtGattInfo ("Successfully publish device status payload \"%s\" on topic %s\n", payload, MQTT_PUB_DEVICE_STATUS);
	}
}

/* deviceStatusMonitorCallback()
 *
 * \brief : this is a timer based callback function which
 *          create the device status payload and send status
 *          to selene
 *
 * \return none
 */
void deviceStatusMonitorCallback() {

	int l_count;
	char * ptr = NULL;
	struct timeval curTime;
	cJSON * deviceStatusNode = NULL;

	logBtGattInfo ("Device status monitor callback function called\n");
	gettimeofday(&curTime, NULL);
	if (deviceStatusNode == NULL ) {
		deviceStatusNode = cJSON_CreateArray();
		if (deviceStatusNode == NULL) {
			logBtGattErr("Failed to create json array\n");
			return;
		}
	}

	for (l_count = 0; l_count < cJSON_GetArraySize(provisionedDeviceList); l_count++) {

		cJSON *provisionedDeviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count);

		char * eui64String = cJSON_GetObjectItem(provisionedDeviceObject, "deviceUid")->valuestring;
		int prevtime = cJSON_GetObjectItem(provisionedDeviceObject, "timestamp")->valueint;

		if ((curTime.tv_sec - prevtime) > DEVICE_TIMEOUT) {

			/* Set device status as offline */
			cJSON * status = cJSON_GetObjectItem(provisionedDeviceObject, "status");
			if ( cJSON_IsTrue(status) == 1 ) {
				cJSON *bool_item = cJSON_CreateBool(false);
				cJSON_ReplaceItemInObject(provisionedDeviceObject, "status", bool_item);
			}
			cJSON * node = cJSON_CreateObject();
			cJSON_AddStringToObject(node, "deviceUid", eui64String);
			cJSON_AddBoolToObject(node, "status", false);
			cJSON_AddItemToArray(deviceStatusNode, node);
		} else {

			/* Set device status as online */
			cJSON * status = cJSON_GetObjectItem(provisionedDeviceObject, "status");
			if ( cJSON_IsTrue(status) != 1 ) {
				cJSON *bool_item = cJSON_CreateBool(true);
				cJSON_ReplaceItemInObject(provisionedDeviceObject, "status", bool_item);
			}
			cJSON * node = cJSON_CreateObject();
			cJSON_AddStringToObject(node, "deviceUid", eui64String);
			cJSON_AddBoolToObject(node, "status", true);
			cJSON_AddItemToArray(deviceStatusNode, node);
		}
	}

	/* Send device status payload to selene */
	if (cJSON_GetArraySize(deviceStatusNode) != 0 ) {
		ptr = cJSON_PrintUnformatted(deviceStatusNode);
		sendDeviceStatus(ptr, mosq);
		free(ptr);
	}
	cJSON_Delete(deviceStatusNode);
	deviceStatusNode = NULL;
}

/* updateDeviceStatus()
 *
 * \brief : this function updates the device status and time into the
 *          device status list.
 *
 * \param [in] eui64 : Uniq device id of end-device.
 * \param [in] mosq  : Pointer to instance of mosquitto.
 *
 * \return none
 */
void updateDeviceStatus(char * eui64, struct mosquitto * mosq) {

	int l_count;
	char * ptr = NULL;
	struct timeval curTime;
	cJSON * deviceStatusNode = NULL;

	gettimeofday(&curTime, NULL);

	logBtGattInfo("Update status for %s\n", eui64);

	/* Traverse loop and check status */
	for (l_count = 0; l_count < cJSON_GetArraySize(provisionedDeviceList); l_count++) {

		cJSON *provisionedDeviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count);
		char * eui64String = cJSON_GetObjectItem(provisionedDeviceObject, "deviceUid")->valuestring;

		if (!strncmp (eui64, eui64String, strlen(eui64String))) {
			cJSON * status = cJSON_GetObjectItem(provisionedDeviceObject, "status");
			if ( cJSON_IsTrue(status) == 1 ) {
				logBtGattInfo ("Device is already connected into network\n");
				cJSON *item = cJSON_CreateNumber(curTime.tv_sec);
				cJSON_ReplaceItemInObject(provisionedDeviceObject, "timestamp", item);

			} else {
				logBtGattInfo ("Device rejoined into network == %s\n", eui64);

				if (deviceStatusNode == NULL ) {
					deviceStatusNode = cJSON_CreateArray();
				}

				/* Update current time and status */
				cJSON *item = cJSON_CreateNumber(curTime.tv_sec);
				cJSON_ReplaceItemInObject(provisionedDeviceObject, "timestamp", item);
				cJSON *bool_item = cJSON_CreateBool(true);
				cJSON_ReplaceItemInObject(provisionedDeviceObject, "status", bool_item);

				/* Create payload */
				cJSON * node = cJSON_CreateObject();
				cJSON_AddStringToObject(node, "deviceUid", eui64String);
				cJSON_AddBoolToObject(node, "status", true);
				cJSON_AddItemToArray(deviceStatusNode, node);

				/* Send device status payload to selene */
				ptr = cJSON_PrintUnformatted(deviceStatusNode);
				sendDeviceStatus(ptr, mosq);
				free(ptr);
				cJSON_Delete(deviceStatusNode);
			}
			break;
		}
	}
}

/* startDeviceMonitor()
 *
 * \brief : this function register the timer based callback function
 *          and create timer.
 *
 * \return none
 */
void startDeviceMonitor() {

	int ret = 0;
	struct itimerspec value = {0};
	timer_t gTimerid = 0;

	/* Set signal Callback */
	(void) signal(SIGALRM, deviceStatusMonitorCallback);

	/* Set Timer Initial expiration */
	value.it_value.tv_sec = 1;
	/* Set Timer interval */
	value.it_interval.tv_sec = DEVICE_MONITOR_CB_TIMER;

	ret = timer_create (CLOCK_MONOTONIC, NULL, &gTimerid);
	if ( ret == -1 ) {
		logBtGattErr("Failed to create timer\n");
		return;
	}

	ret = timer_settime (gTimerid, 0, &value, NULL);
	if ( ret == -1 ) {
		logBtGattErr("Failed to set timer\n");
		return;
	}
}
