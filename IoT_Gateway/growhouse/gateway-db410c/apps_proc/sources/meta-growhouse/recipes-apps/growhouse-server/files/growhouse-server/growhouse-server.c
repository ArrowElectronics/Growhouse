/*
 * growhouse-server.c
 * Copyright(c) 2018
 */

/***************************** Header Files **********************************/
#include "growhouse-server.h"
#include "zigbee_attributes.h"
#include "zigbee_datatypes.h"
#include "factory-reset-monitor.h"
/***************************** End Header Files ******************************/


/****************************** Definations **********************************/
#define mqtt_host "localhost"
#define mqtt_port 1883

#define MQTT_SUB_DEVICE_JOINED 		"gw/+/devicejoined"
#define MQTT_SUB_ZCL_RESPONSE 		"gw/+/zclresponse"
#define MQTT_SUB_HEARTBEAT		"gw/+/heartbeat"
#define MQTT_SUB_DEVICES		"gw/+/devices"
#define MQTT_SUB_DISCOVER_COMMAND	"ble/gw/discoverCommand"
#define MQTT_SUB_PROVISIONED_DEVICE	"ble/gw/provisionedDevice"
#define MQTT_SUB_BLE_SHUTTING_DOWN	"ble/gw/shuttingDown"
#define MQTT_SUB_SELENE_RESPONSE	"selene/mqtt/router/seleneresponse"
#define MQTT_SUB_SELENE_STATE_CONTROL	"selene/mqtt/device/state/control"
#define MQTT_SUB_SELENE_CMD_HANDLE	"selene/mqtt/zwave/device/control"
#define MQTT_SUB_DEVICE_UPDATE_REQUEST	"selene/mqtt/router/deviceupdate"
#define MQTT_SUB_SERVICE_UPDATE_REQUEST "selene/mqtt/router/serviceupdate"

#define MQTT_PUB_DEVICE_REGISTER                "selene/mqtt/device/register"
#define MQTT_PUB_SELENE_REQUEST                 "selene/mqtt/router/selenereq"
#define MQTT_PUB_DEVICE_UPDATE_RESPONSE         "selene/mqtt/router/deviceupdate/response"
#define MQTT_PUB_SERVICE_UPDATE_RESPONSE        "selene/mqtt/router/serviceupdate/response"
#define MQTT_PUB_DISCOVERED_DEVICE              "gw/ble/discoveredDevice"
#define MQTT_PUB_GATEWAY_CONNECT_RESPONSE       "gw/ble/gatewayConnectivity"
#define MQTT_PUB_DEVICE_CONNECT_RESPONSE        "gw/ble/deviceConnectivity"
#define MQTT_PUB_GATEWAY_DISCONNECT             "gw/ble/gatewayDisconnect"

#define SELENE_GATEWAY_REGISTRATION_RESPONSE    "gateway_registration_response"
#define SELENE_DEVICE_REGISTRATION_RESPONSE     "device_registration"
#define SELENE_DEVICE_DELETION_RESPONSE         "device_deletion"
#define SELENE_DEVICE_LIST_RESPONSE             "Device_List_Response"
#define SELENE_GATEWAY_DELETION_RESPONSE        "gateway_deletion"

#define LED_STATE_CONTROL_CHANNEL_1             "led1"
#define LED_STATE_CONTROL_CHANNEL_2             "led2"
#define LED_STATE_CONTROL_CHANNEL_3             "led3"
#define LED_STATE_CONTROL_CHANNEL_4             "led4"
#define LED_STATE_CONTROL_CHANNEL_5             "led5"
#define LED_STATE_CONTROL_CHANNEL_6             "led6"

#define LED_RED_CONTROL                         "R"
#define LED_GREEN_CONTROL                       "G"
#define LED_BLUE_CONTROL                        "B"
#define LED_COOL_WHITE_CONTROL                  "CW"
#define LED_NEUTRAL_WHITE_CONTROL               "NW"
#define LED_WARM_WHITE_W_CONTROL                "WW"
#define LED_UV_A_CONTROL                        "UV A"
#define LED_UV_B_CONTROL                        "UV B"
#define LED_UV_C_CONTROL                        "UV C"
#define LED_ROYAL_BLUE_CONTROL                  "RB"
#define LED_YELLOW_CONTROL                      "Y"
#define LED_AMBER_CONTROL                       "A"
#define LED_ORANGE_CONTROL                      "O"
#define LED_DEEP_RED_CONTROL                    "DR"
#define LED_FAR_RED_CONTROL                     "FR"
#define LED_IR_CONTROL                          "IR"
#define LED_NOT_CONNECTED                       "NC"

#define LEDNODE_RGB_MAPPING_COMMAND             0x15
#define LEDNODE_CHANNEL_INTENSITY               0x16
#define DEVICE_UPDATE_COMMAND                   0x01
#define END_DEVICE_FIRMWARE_COMMAND             0x18

#define LEDNODE_FIRMWARE_UPDATE                 "/ota-files/LedNodeFirmware"
#define SOILNODE_FIRMWARE_UPDATE                "/ota-files/SoilNodeFirmware"
#define FIRMWARE_VERSION_FILE                   "/home/root/CurrentFirmwareVersion"
#define SERVICE_UPDATE_SCRIPT                   "/home/root/firmwareUpdateScript.sh"
#define GROWHOUSE_SERVER_SERVICE                "/home/root/growhouse-server"
#define BLE_SERVER_SERVICE                      "/home/root/ble-server"
#define ZIGBEE_COORDINATOR_SERVICE              "/home/root/zigbeeCoordinator"
#define MQTT_PROPERTIES_FILE                    "/opt/selene/config/devices/mqtt-router.properties"
#define SELENE_PROPERTIES_FILE                  "/opt/selene/config/devices/self.properties"
#define SOILNODE_SOFTWARE_PRODUCT_NAME          "GrowhouseGWSoilNode"
#define LEDNODE_SOFTWARE_PRODUCT_NAME           "GrowhouseGWLedNode"

/*
 * Default mosquitto.conf file sets the keepalive seconds
 * to be 60, so we set the keepailve interval of 120 seconds
 */
#define MOSQUITTO_PING_TIMEOUT 120

enum mqtt_topic {

	MQTT_TYPE_MIN,
	MQTT_TYPE_DEVICE_JOINED = MQTT_TYPE_MIN,
	MQTT_TYPE_ZCL_RESPONSE,
	MQTT_TYPE_HEARTBEAT,
	MQTT_TYPE_DEVICES,
	MQTT_TYPE_DISCOVER_COMMAND,
	MQTT_TYPE_PROVISIONED_DEVICE,
	MQTT_TYPE_BLE_SHUTTING_DOWN,
	MQTT_TYPE_SELENE_RESPONSE,
	MQTT_TYPE_SELENE_STATE_CONTROL,
	MQTT_TYPE_SELENE_CMD_HANDLE,
	MQTT_TYPE_DEVICE_UPDATE_REQUEST,
	MQTT_TYPE_SERVICE_UPDATE_REQUEST,
	MQTT_TYPE_MAX

};

const char * mqtt_topic_subscribe [] = {

	[MQTT_TYPE_DEVICE_JOINED] = MQTT_SUB_DEVICE_JOINED,
	[MQTT_TYPE_ZCL_RESPONSE] = MQTT_SUB_ZCL_RESPONSE,
	[MQTT_TYPE_HEARTBEAT] = MQTT_SUB_HEARTBEAT,
	[MQTT_TYPE_DEVICES] = MQTT_SUB_DEVICES,
	[MQTT_TYPE_DISCOVER_COMMAND] = MQTT_SUB_DISCOVER_COMMAND,
	[MQTT_TYPE_PROVISIONED_DEVICE] = MQTT_SUB_PROVISIONED_DEVICE,
	[MQTT_TYPE_BLE_SHUTTING_DOWN] = MQTT_SUB_BLE_SHUTTING_DOWN,
	[MQTT_TYPE_SELENE_RESPONSE] = MQTT_SUB_SELENE_RESPONSE,
	[MQTT_TYPE_SELENE_STATE_CONTROL] = MQTT_SUB_SELENE_STATE_CONTROL,
	[MQTT_TYPE_SELENE_CMD_HANDLE] = MQTT_SUB_SELENE_CMD_HANDLE,
	[MQTT_TYPE_DEVICE_UPDATE_REQUEST] = MQTT_SUB_DEVICE_UPDATE_REQUEST,
	[MQTT_TYPE_SERVICE_UPDATE_REQUEST] = MQTT_SUB_SERVICE_UPDATE_REQUEST
};

typedef struct mosquitto_message_handler {

	int * mosquitto_topic;
	int (* handler)(struct mosquitto *, const struct mosquitto_message*);

}MOSQUITTO_MSG_HANDLER;

static const MOSQUITTO_MSG_HANDLER mosquitto_message_handler_array[] = {

	{ .mosquitto_topic = (int *) MQTT_SUB_DEVICES,
		.handler = &onDeviceListReceived },
	{ .mosquitto_topic = (int *) MQTT_SUB_DEVICE_JOINED ,
		.handler = &newDeviceJoinedRequest },
	{ .mosquitto_topic = (int *) MQTT_SUB_PROVISIONED_DEVICE,
		.handler = &provisionDevice },
	{ .mosquitto_topic = (int *) MQTT_SUB_ZCL_RESPONSE,
		.handler = &onZCLResponse },
	{ .mosquitto_topic = (int *) MQTT_SUB_HEARTBEAT ,
		.handler = &checkGatewayHeartbeat },
	{ .mosquitto_topic = (int *) MQTT_SUB_DISCOVER_COMMAND,
		.handler = &discoverDevices },
	{ .mosquitto_topic = (int *) MQTT_SUB_SELENE_RESPONSE ,
		.handler = &checkSeleneResponse },
	{ .mosquitto_topic = (int *) MQTT_SUB_SELENE_STATE_CONTROL,
		.handler = &deviceStateControl },
	{ .mosquitto_topic = (int *) MQTT_SUB_BLE_SHUTTING_DOWN,
		.handler = &bleShuttingDownCallback },
	{ .mosquitto_topic = (int *) MQTT_SUB_SELENE_CMD_HANDLE,
		.handler = &seleneCommandHandler },
	{ .mosquitto_topic = (int *) MQTT_SUB_DEVICE_UPDATE_REQUEST,
		.handler = &deviceUpdateHandler },
	{ .mosquitto_topic = (int *) MQTT_SUB_SERVICE_UPDATE_REQUEST,
		.handler = &deviceUpdateHandler }
};

enum led_state_control {

	LED_CHANNEL_MIN = 0,
	LED_CHANNEL_1,
	LED_CHANNEL_2,
	LED_CHANNEL_3,
	LED_CHANNEL_4,
	LED_CHANNEL_5,
	LED_CHANNEL_6,
	LED_CHANNEL_MAX
};

const char * led_state_control_array [] = {

	[LED_CHANNEL_1] = LED_STATE_CONTROL_CHANNEL_1,
	[LED_CHANNEL_2] = LED_STATE_CONTROL_CHANNEL_2,
	[LED_CHANNEL_3] = LED_STATE_CONTROL_CHANNEL_3,
	[LED_CHANNEL_4] = LED_STATE_CONTROL_CHANNEL_4,
	[LED_CHANNEL_5] = LED_STATE_CONTROL_CHANNEL_5,
	[LED_CHANNEL_6] = LED_STATE_CONTROL_CHANNEL_6
};

enum led_RGB_control {
	R = 82,
	G = 71,
	B = 66,
	CW = 97,
	NW = 98,
	WW = 99,
	UV_A = 100,
	UV_B = 101,
	UV_C = 102,
	RB = 103,
	Y = 104,
	A = 105,
	O = 106,
	DR = 107,
	FR = 108,
	IR = 109,
	NC = 78
};

const char * led_RGB_control_array[] = {
	[R] = LED_RED_CONTROL,
	[G] = LED_GREEN_CONTROL,
	[B] = LED_BLUE_CONTROL,
	[CW] = LED_COOL_WHITE_CONTROL,
	[NW] = LED_NEUTRAL_WHITE_CONTROL,
	[WW] = LED_WARM_WHITE_W_CONTROL,
	[UV_A] = LED_UV_A_CONTROL,
	[UV_B] = LED_UV_B_CONTROL,
	[UV_C] = LED_UV_C_CONTROL,
	[RB] = LED_ROYAL_BLUE_CONTROL,
	[Y] = LED_YELLOW_CONTROL,
	[A] = LED_AMBER_CONTROL,
	[O] = LED_ORANGE_CONTROL,
	[DR] = LED_DEEP_RED_CONTROL,
	[FR] = LED_FAR_RED_CONTROL,
	[IR] = LED_IR_CONTROL,
	[NC] = LED_NOT_CONNECTED
};

typedef struct led_config {
        char * type;
        char key;

}LED_CHANNEL_TYPES;

LED_CHANNEL_TYPES led_channel_types[] = {

        { LED_RED_CONTROL          , 'R' },
        { LED_GREEN_CONTROL        , 'G' },
        { LED_BLUE_CONTROL         , 'B' },
        { LED_COOL_WHITE_CONTROL   , 'a' },
        { LED_NEUTRAL_WHITE_CONTROL, 'b' },
        { LED_WARM_WHITE_W_CONTROL , 'c' },
        { LED_UV_A_CONTROL         , 'd' },
        { LED_UV_B_CONTROL         , 'e' },
        { LED_UV_C_CONTROL         , 'f' },
        { LED_ROYAL_BLUE_CONTROL   , 'g' },
        { LED_YELLOW_CONTROL       , 'h' },
        { LED_AMBER_CONTROL        , 'i' },
        { LED_ORANGE_CONTROL       , 'j' },
        { LED_DEEP_RED_CONTROL     , 'k' },
        { LED_FAR_RED_CONTROL      , 'l' },
        { LED_IR_CONTROL           , 'm' },
        { LED_NOT_CONNECTED        , 'N' },
};

/*
 * Total number of function handler
 */
#define NUMBER_OF_FUNCTION  sizeof(mosquitto_message_handler_array)/sizeof(MOSQUITTO_MSG_HANDLER)

/****************************** End Definations ******************************/

/****************************** Global Declaration ***************************/

// Global Maintain all joined device list
cJSON *root = NULL;

// Global store provisioned device list
cJSON * provisionedDeviceList = NULL;

// store Firmware verison of endDevice
cJSON * deviceFirmwareVersionList = NULL;

// store Firmware update request for devices
cJSON * deviceUpdateRequestList = NULL;
cJSON * LednodeUpdate = NULL;
cJSON * SoilnodeUpdate = NULL;

char * gatewayEui64 = NULL;

char * deviceListCommand = "{\"type\" : \"Device_List\"}";
char * openNetworkCommand =  "{\"commands\":[{\"command\":\"plugin network-creator-security open-network\",\"postDelayMs\":100}]}";
char * closeNetworkCommand =  "{\"commands\":[{\"command\":\"plugin network-creator-security close-network\",\"postDelayMs\":100}]}";
char * gatewayConnectivitySuccess =  "{\"gatewayConnectivity\":\"true\"}";
char * gatewayConnectivityError =  "{\"gatewayConnectivity\":\"false\"}";

// Mutex lock
pthread_mutex_t lock;
pthread_mutex_t provision_lock;
int LednodeUpdateCount;
int SoilnodeUpdateCount;

int gatewayConnectivity = 0;
int provisionInprogress = 0;
struct mosquitto *mosq = NULL;

/*************************** End Global Declaration ***************************/

/************************* functions ******************************************/

/* connect_callback()
 *
 * \brief : this routine called when mosquitto service start
 *
 * \param [in] mosq   : Pointer to the mosquitto instance.
 *        [in] obj    : Pointer to user data provided in mosquitto_new.
 *        [in] result : Return code for connection responce.
 *
 * \return none
 */
void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	int l_count = 0;
	int status = MOSQ_ERR_INVAL;

	if ( result == 0 ) {
		logBtGattInfo ("Sucessfully connects to mosquitto service\n");
	} else {
		logBtGattErr ("Failed to connects mosquitto service\n");
	}

	if ( mosq == NULL ){
		logBtGattErr ("Invalid pointer\n");
		return;
	}
	for ( l_count = MQTT_TYPE_MIN ; l_count < MQTT_TYPE_MAX ; l_count++){

		status = mosquitto_subscribe(mosq, NULL, mqtt_topic_subscribe[l_count], 0);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not subscribe to topic:%s err:%d", mqtt_topic_subscribe[l_count], status);
			/* TODO : If any topic failed to subscribe then restart mosquitto and all services*/
		}
		logBtGattInfo ("Subscribe to topic : %s\n",mqtt_topic_subscribe[l_count]);
	}
	// Request selene to provide already provisioned device list
	status = mosquitto_publish(mosq, NULL, MQTT_PUB_SELENE_REQUEST, strlen(deviceListCommand), deviceListCommand, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_SELENE_REQUEST, status);
		return;
	}
	logBtGattInfo ("Successfully publish payload to get deviceList from selene on topic %s\n", MQTT_PUB_SELENE_REQUEST);
}

/* attributeDataToSigned()
 *
 * \brief : this routine convert atrribute data to signed.
 *
 * \param [in] val : attribute data.
 *        [in] len : length of data in bytes.
 *
 * \return signed data
 */
static long long attributeDataToSigned(unsigned long long val, int len)
{
	long long ans;
	if (((long long) (val << ((8 - len)*8))) < 0) {
		ans = -(((unsigned long long)-1 >> (8 - len)*8) - val + 1);
	}
	else
		ans = val;

	return ans;
}

/* l2b_endian()
 *
 * \brief : this routine convert data bytes into required len of data bytes
 *
 * \param [in] data : attribute data.
 *        [in] len_bytes : length of data in bytes.
 *
 * \return new_data
 */
static unsigned long long l2b_endian (unsigned long long data, unsigned int len_bytes)
{
	unsigned long long new_data = 0;
	int i = 0;

	for (i = 1; i <= len_bytes; i++) {
		new_data |= ((data >> (len_bytes - i)*8) & 0xff) << (i - 1)*8;
	}

	return new_data;
}

/* isEndpointSupported()
 *
 * \brief : this routine is used to check endpoints is present or not
 *          for joined device.
 *
 * \param [in] node : Pointer to received payload when device joined.
 *
 * \return : If endpoint is supported then return true else false.
 */
bool isEndpointSupported ( cJSON * node )
{
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return false;
	}

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	int gwendpoint = cJSON_GetObjectItem(deviceEndpoint, "endpoint")->valueint;

	return (gwendpoint == SELF_ENDPOINT ||
			gwendpoint == SMART_ENERGY_ENDPOINT ||
			gwendpoint == GREEN_POWER_ENDPOINT) ? false : true;
}

/* isDeviceDisplayable()
 *
 * \brief : this routine is used to check device state of device.
 *
 * \param [in] node : Pointer to received payload when device joined.
 *
 * \return : If device state is supported then return true else false.
 */
bool isDeviceDisplayable(cJSON * node) {

	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return false;
	}

	int deviceState = cJSON_GetObjectItem(node, "deviceState")->valueint;

	return (deviceState == ND_UNRESPONSIVE ||
			deviceState == ND_JOINED ||
			deviceState == ND_LEAVE_SENT ||
			deviceState == ND_LEFT) ? true : false;
}

/* isSleepyDevice()
 *
 * \brief : this routine is used to check whether device is sleepy or not.
 *
 * \param [in] node : Pointer to received payload when device joined.
 *
 * \return : If device is sleepy then return true else false.
 */
bool isSleepyDevice (cJSON * node) {

	int deviceType;

	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return false;
	}

	char * deviceTypeStr = cJSON_GetObjectItem(node, "deviceType")->valuestring;
	sscanf (&deviceTypeStr[2],"%x",&deviceType);

	return (deviceType == DEVICE_ID_ON_OFF_LIGHT ||
			deviceType == DEVICE_ID_DIMMABLE_LIGHT ||
			deviceType == DEVICE_ID_COLOR_DIMMABLE_LIGHT ||
			deviceType == DEVICE_ID_EXTENDED_COLOR_LIGHT ||
			deviceType == DEVICE_ID_HA_ON_OFF_LIGHT ||
			deviceType == DEVICE_ID_COLOR_TEMPERATURE_LIGHT ||
			deviceType == DEVICE_TYPE_SMART_PLUG ||
			deviceType == DEVICE_TYPE_LIGHT_SHIELD )? false : true;
}

/* isRelaySupported()
 *
 * \brief : this routine is used to check whether it is relay supported device.
 *
 * \param [in] deviceType : Type of device
 *
 * \return : If device is support relay then return true else false.
 */
bool isRelaySupported ( int deviceType ) {

	return (deviceType == DEVICE_TYPE_CONTACT_SENSOR ||
			deviceType == DEVICE_TYPE_OCCUPANCY_SENSOR) ? false : true;

}

/* growhouseCleanup()
 *
 * \brief : this routine disconnect mosquitto and free all json memory.
 *
 * \param [in] mosq  : Pointer to instance of mosquitto.
 *
 * \return none
 */
void growhouseCleanup(struct mosquitto *mosq) {

	logBtGattInfo ("Growhouse service Cleanup function called\n");
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	if ( root != NULL )
		cJSON_Delete(root);

	if ( provisionedDeviceList != NULL )
		cJSON_Delete(provisionedDeviceList);

	if ( deviceFirmwareVersionList != NULL)
		cJSON_Delete(deviceFirmwareVersionList);

	if ( deviceUpdateRequestList != NULL)
		cJSON_Delete(deviceUpdateRequestList);

}

/* printAllDeviceList()
 *
 * \brief : this routine print all device which is joined in zigbee network.
 *
 * \return none
 */
void printAllDeviceList() {

	char * ptr = NULL;
	ptr = cJSON_PrintUnformatted(root);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
	} else {
		logBtGattInfo("Devices :: %s\n", ptr);
		free (ptr);
	}
}

/* printProvisionedDeviceList()
 *
 * \brief : this routine print all device which is provisioned.
 *
 * \return none
 */
void printProvisionedDeviceList() {

	char * ptr = NULL;
	ptr = cJSON_PrintUnformatted(provisionedDeviceList);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
	} else {
		logBtGattInfo("Provisioned device list :: %s\n", ptr);
		free (ptr);
	}
}

/* sendFirmwareUpdateResponseToSelene()
 *
 * \brief : this routine send response to selene for firmware update
 *          and delete device from array of device list for firmware update.
 *
 * \param [in] mosq     : Pointer to instance of mosquitto.
 * \param [in] eui64    : Device uniq qui64 string.
 * \param [in] result   : Either success of failure string.
 * \param [in] message  : Message string for firmware update status.
 *
 * \return none
 */
void sendFirmwareUpdateResponseToSelene(struct mosquitto * mosq, char * eui64, char * result, char * message) {

	int l_count = 0;
	int status = MOSQ_ERR_INVAL;
	char * deviceUpdateEui64 = NULL;
	cJSON * device = NULL;
	char * ptr = NULL;

	/* send response to selene*/
	cJSON * response = cJSON_CreateObject();
	cJSON_AddStringToObject(response, "deviceUid", eui64);
	cJSON_AddStringToObject(response, "result", result);
	cJSON_AddStringToObject(response, "message", message);

	ptr = cJSON_PrintUnformatted(response);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(response);
		return;
	}
	status = mosquitto_publish(mosq, NULL, MQTT_PUB_DEVICE_UPDATE_RESPONSE, strlen(ptr), ptr, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_DEVICE_UPDATE_RESPONSE, status);
	} else {
		logBtGattInfo ("Successfully publish payload \"%s\" to selene on topic %s\n", ptr, MQTT_PUB_DEVICE_UPDATE_RESPONSE);
	}
	free (ptr);
	cJSON_Delete(response);

	/* Remove entry from firmware update request list */
	for (l_count = 0; l_count < cJSON_GetArraySize(deviceUpdateRequestList); l_count++) {

		device = cJSON_GetArrayItem(deviceUpdateRequestList, l_count);
		deviceUpdateEui64 = cJSON_GetObjectItem(device, "eui64")->valuestring;
		if ( !strncmp(eui64, deviceUpdateEui64, strlen(deviceUpdateEui64))) {
			/* Remove device from array of device update request */
			logBtGattInfo (" Remove device %s from the list\n", deviceUpdateEui64);
			cJSON_DeleteItemFromArray(deviceUpdateRequestList, l_count);
		}
		break;
	}
	ptr = cJSON_PrintUnformatted(deviceUpdateRequestList);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		return;
	} else {
		logBtGattInfo ("Updated Device list for firmware update request == %s\n", ptr);
		free(ptr);
	}
}

/* updateEndDeviceFirmware()
 *
 * \brief : this routine is called when Firmware update is initiated from cloud.
 *          Copy file from selene database to /ota-files directory and send
 *          command to zigbee coordinator to reload firmware file.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] node    : Payload received from selene
 *
 * \return 0 on success, -1 on failure
 */
int updateEndDeviceFirmware(struct mosquitto *mosq, cJSON * node) {

	int src_fd, dst_fd, n, err;
	int version = 0;
	int l_count = 0;
	int deviceVersion = 0;
	char * filePath = NULL;
	char * eui64String = NULL;
	char * ret = NULL;
	char * deviceEui64String = NULL;
	char src_path[256] = {0};
	char dst_path[50] = {0};
	char command[256] = {0};
	char mqttTopic[64] = {0};
	unsigned char buffer[4096] = {0};
	int status = MOSQ_ERR_INVAL;
	cJSON * deviceObject = NULL;

	/* Check if provisioning is in progress then restrict to firmware update */
	if ( provisionInprogress == 1 ) {
		logBtGattInfo ("Device provisioning is in progress");
		return -1;
	}

	filePath = cJSON_GetObjectItem(node, "filePath")->valuestring;
	strncpy(src_path, filePath, strlen(filePath));
	logBtGattInfo ("Firmware File == %s\n", filePath);

	if ((strstr(basename(src_path),"LED_Node_v") == NULL ) &&
		(strstr(basename(src_path),"Soil_Node_v") == NULL )) {
		logBtGattErr ("Filename format is wrong\n");
		return -1;
	}

	eui64String = cJSON_GetObjectItem(node, "deviceUid")->valuestring;
	logBtGattInfo ("Received firmware update for device = %s\n",eui64String);

	ret = strstr(strtok(basename(src_path), "."),"v");
	if ( ret == NULL ) {
		logBtGattErr("Failed to parse firmware version\n");
		return -1;
	}
	version = atoi (&ret[1]);
	logBtGattInfo ("Received firmware version = %d\n", version);

	/* Check If received firmware file version and device version is same then return from here */
	for (l_count = 0; l_count < cJSON_GetArraySize(deviceFirmwareVersionList); l_count++) {

		deviceObject = cJSON_GetArrayItem(deviceFirmwareVersionList, l_count);
		deviceEui64String = cJSON_GetObjectItem(deviceObject, "eui64")->valuestring;

		if (!strncmp (eui64String,&deviceEui64String[2],strlen(eui64String))) {

			deviceVersion = cJSON_GetObjectItem(deviceObject,"version")->valueint;
			logBtGattInfo ("Device is running on verison == %d\n", deviceVersion);
			/* If version is same then return from here with success Message */
			if ( deviceVersion == version ) {
				logBtGattInfo ("Device is running on same Firmware version\n");
				sendFirmwareUpdateResponseToSelene(mosq, deviceEui64String, "success", "Device is running on same Firmware version");
				/* Remove file from system */
				if( access(filePath, F_OK ) != -1 )
					remove (filePath);
				return 0;
			}
			break;
		}
	}

	/* Create array to store all device which is receives firmware update request */
	if (deviceUpdateRequestList == NULL ) {
		deviceUpdateRequestList = cJSON_CreateArray();
	}

	if (strstr(filePath,"LED_Node")) {

		logBtGattInfo ("Received update for lednode\n");

		LednodeUpdate = cJSON_CreateObject();
		cJSON_AddStringToObject(LednodeUpdate, "eui64", eui64String);
		cJSON_AddNumberToObject(LednodeUpdate, "version", version);
		cJSON_AddStringToObject(LednodeUpdate, "type", "LedNode");
		cJSON_AddItemToArray(deviceUpdateRequestList, LednodeUpdate);

		if ( LednodeUpdateCount != 0 ) {
			logBtGattInfo ("Already firmware is copied to ota-files directory\n");

			if( access(filePath, F_OK ) != -1 )
				remove (filePath);

			LednodeUpdateCount += 1;
			logBtGattInfo ("Total number of Lednode in update  == %d\n", LednodeUpdateCount);
			return 0;
		}

		/* Remove old firmware file */
		if( access(LEDNODE_FIRMWARE_UPDATE, F_OK ) != -1 )
			remove(LEDNODE_FIRMWARE_UPDATE);
		strncpy(dst_path, LEDNODE_FIRMWARE_UPDATE, strlen(LEDNODE_FIRMWARE_UPDATE));

	} else {
		logBtGattInfo ("Received update for soilnode\n");
		SoilnodeUpdate = cJSON_CreateObject();
		cJSON_AddStringToObject(SoilnodeUpdate, "eui64", eui64String);
		cJSON_AddNumberToObject(SoilnodeUpdate, "version", version);
		cJSON_AddStringToObject(SoilnodeUpdate, "type", "SoilNode");
		cJSON_AddItemToArray(deviceUpdateRequestList, SoilnodeUpdate);

		if ( SoilnodeUpdateCount != 0 ) {
			logBtGattInfo ("Already firmware is copied to ota-files directory\n");

			if( access(filePath, F_OK ) != -1 )
				remove (filePath);

			SoilnodeUpdateCount += 1;
			logBtGattInfo ("Total number of soilnode in update == %d\n", SoilnodeUpdateCount);
			return 0;
		}

		/* Remove old firmware file */
		if( access(SOILNODE_FIRMWARE_UPDATE, F_OK ) != -1 )
			remove(SOILNODE_FIRMWARE_UPDATE);
		strncpy(dst_path, SOILNODE_FIRMWARE_UPDATE, strlen(SOILNODE_FIRMWARE_UPDATE));
	}

	/* Copy file from selene directory to /ota-files */
	src_fd = open(filePath, O_RDONLY);
	dst_fd = open(dst_path, O_CREAT | O_WRONLY);

	if ( src_fd < 0 || dst_fd < 0 ) {
		logBtGattErr ("Failed to open file\n");
		return -1;
	}

	while (1) {
		err = read(src_fd, buffer, 4096);
		if (err == -1) {
			logBtGattErr("Error reading file.\n");
			return -1;
		}
		n = err;

		if (n == 0) break;

		err = write(dst_fd, buffer, n);
		if (err == -1) {
			logBtGattErr ("Error writing to file.\n");
			return -1;
		}
	}

	close(src_fd);
	close(dst_fd);

	if( access(filePath, F_OK ) != -1 )
		remove (filePath);

	snprintf (command , sizeof(command), "{\"commands\" : [{\"command\":\"plugin ota-storage-common reload\",\"postDelayMs\":100}]}");
	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		return -1;
	}
	logBtGattInfo ("Successfully publish payload \"%s\" on topic %s\n", command, mqttTopic);

	/* If all the above process run successfully then final update the count */
	if (strstr(filePath,"LED_Node")) {
		LednodeUpdateCount += 1;
		logBtGattInfo ("Total number of Lednode in update = %d\n", LednodeUpdateCount);
	} else {
		SoilnodeUpdateCount += 1;
		logBtGattInfo ("Total number of soilnode in update = %d\n", SoilnodeUpdateCount);
	}
	return 0;
}


/* sendServiceUpdateResponseToSelene()
 *
 * \brief : this routine send response to selene for firmware update
 *          of gateway services in case of successor failure.
 *
 * \param [in] mosq     : Pointer to instance of mosquitto.
 * \param [in] result   : Either success of failed string.
 * \param [in] message  : Message string for firmware update status.
 *
 * \return none
 */
void sendServiceUpdateResponseToSelene(struct mosquitto * mosq, char * result, char * message) {

	int status = MOSQ_ERR_INVAL;
	char * ptr = NULL;

	/* send response to selene*/
	cJSON * response = cJSON_CreateObject();
	cJSON_AddStringToObject(response, "result", result);
	cJSON_AddStringToObject(response, "message", message);
	ptr = cJSON_PrintUnformatted(response);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(response);
		return;
	}
	status = mosquitto_publish(mosq, NULL, MQTT_PUB_SERVICE_UPDATE_RESPONSE, strlen(ptr), ptr, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_SERVICE_UPDATE_RESPONSE, status);
	} else {
		logBtGattInfo ("Successfully publish payload \"%s\" to selene on topic %s\n", ptr, MQTT_PUB_SERVICE_UPDATE_RESPONSE);
	}
	free(ptr);
	cJSON_Delete(response);
}

/* updateGatewayServices()
 *
 * \brief : this routine is called when Firmware update is initiated from cloud.
 *          extract file from selene database to /home/root directory and initiate
 *          script to update gateway services.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] node    : Payload received from selene
 *
 * \return 0 on success, -1 on failure
 */
int updateGatewayServices(struct mosquitto *mosq, cJSON * node) {

	int version = 0;
	int childpid = 0;
	char path[256] = {0};
	char * filePath = NULL;
	char * ret = NULL;
	char src_path[256] = {0};
	FILE *fp = NULL;
	char * firmwareUpdateScript[] = {"sh", SERVICE_UPDATE_SCRIPT, NULL};

	logBtGattInfo (" Received update for growhouse services\n");

	/* Check if provisioning is in progress then restrict to firmware update */
	if ( provisionInprogress == 1 ) {
		logBtGattInfo ("Device provisioning is in progress");
		return -1;
	}

	filePath = cJSON_GetObjectItem(node, "filePath")->valuestring;
	strncpy(src_path, filePath, strlen(filePath));
	logBtGattInfo ("Firmware File == %s\n", filePath);

	/* Check if file is available or not */
	if( access(filePath, F_OK ) == -1 ) {
		logBtGattInfo ("%s is not avilable", filePath);
		return -1;
	}

	/* Check if filename format is match */
	if (strstr(basename(src_path),"Gateway_services_v") == NULL ) {
		logBtGattErr ("Filename format is wrong\n");
		return -1;
	}

	ret = strstr(strtok(basename(src_path), "."),"_v");
	if ( ret == NULL ) {
		logBtGattErr("Failed to parse firmware version\n");
		return -1;
	}
	version = atoi (&ret[2]);
	logBtGattInfo ("Received firmware version = %d\n", version);

	/* Compare current firmware version with new firmware version received */
	if ( GROWHOUSE_SERVICE_VERSION == version) {
		logBtGattInfo ("Services running on same firmware version\n");
		sendServiceUpdateResponseToSelene(mosq, "success", "Services running on same firmware version");
		return 0;
	}

	/* untar file to specific directory */
	sprintf ( path,"/bin/tar -xvf %s -C /home/root/", filePath);
	logBtGattInfo ("command == %s\n",path);
	if( fp = popen(path, "w") )
	{
		pclose(fp);
	} else {
		logBtGattErr("Failed to untar gateway services file");
		return -1;
	}

	/* Remove file from system */
	if( access(filePath, F_OK ) != -1 )
		remove(filePath);

	/* Check if all the files is availbale or not for service update */
	if ( (access(SERVICE_UPDATE_SCRIPT, F_OK ) == -1 ) || (access(GROWHOUSE_SERVER_SERVICE, F_OK ) == -1 )
			|| (access(BLE_SERVER_SERVICE, F_OK ) == -1 ) || (access(ZIGBEE_COORDINATOR_SERVICE, F_OK ) == -1 )) {
		logBtGattErr("All service file is not available for firmware update");
		return -1;
	}
	/* Run firmware update script to update all gateway services */
	if((childpid = fork()) == -1 )
	{
		logBtGattErr("Failed to fork");
		return -1;
	}
	else if(childpid == 0)
	{
		if( access(SERVICE_UPDATE_SCRIPT, F_OK ) != -1 ) {

			/* Cleanup memory */
			growhouseCleanup(mosq);

			if ( (execv("/bin/sh", firmwareUpdateScript)) == -1) {
				logBtGattErr ("Failed to run service update script\n");
				return -1;
                        } else {
				logBtGattInfo ("Successfully execute service update script\n");
			}

		} else {
			logBtGattErr ("Service update file is not available\n");
			return -1;
		}
        }
	return 0;
}

/* deviceUpdateHandler()
 *
 * \brief : this routine is called when Firmware update is initiated from cloud.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int deviceUpdateHandler(struct mosquitto *mosq, const struct mosquitto_message *message) {

	int ret = 0;
	char * type = NULL;
	char * eui64String = NULL;
	cJSON * node = NULL;
	char * filePath = NULL;

	logBtGattInfo ("Firmware update request received from selene\n");
	node = cJSON_Parse(message->payload);
	if ( node == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	type = cJSON_GetObjectItem(node, "type")->valuestring;
	logBtGattInfo ("Firmware update type == %s\n",type);
	filePath = cJSON_GetObjectItem(node, "filePath")->valuestring;

	if ( !strncmp(type, "update_device", strlen(type))) {

		eui64String = cJSON_GetObjectItem(node, "deviceUid")->valuestring;
		logBtGattInfo ("Device eui64String = %s\n",eui64String);
		ret = updateEndDeviceFirmware(mosq, node);
		if ( ret == -1 ){
			sendFirmwareUpdateResponseToSelene(mosq, eui64String, "failed", "Failed to update firmware file");
		}

	} else if (!strncmp(type, "update_service", strlen(type))) {

		ret = updateGatewayServices(mosq, node);
		if ( ret == -1 ){
			sendServiceUpdateResponseToSelene(mosq, "failed", "Failed to update gateway services");
		}
	} else {
		logBtGattErr("Received wrong update type\n");
		cJSON_Delete(node);
		return -1;
	}

	/* Remove file from system */
	if( access(filePath, F_OK ) != -1 )
		remove(filePath);

	cJSON_Delete(node);
	return 0;
}

/* zigbeeRemoveDevice()
 *
 * \brief : this routine publish command to remove device from gateway network.
 *
 * \param [in] mosq   : Pointer to instance of mosquitto.
 *        [in] nodeId : Network address of joined device.
 *
 * \return : 0 on success , -1 on failure
 */
int zigbeeRemoveDevice(struct mosquitto *mosq, char * nodeId) {

	char command[100]={0};
	char mqttTopic[32] = {0};
	int status = MOSQ_ERR_INVAL;

	snprintf (command , sizeof(command), "{\"commands\" : [{\"command\":\"zdo leave %s 0 0\",\"postDelayMs\":100}]}",nodeId);
	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		return -1;
	}
	logBtGattInfo ("Successfully publish device deletion payload \"%s\" on topic %s\n", command, mqttTopic);

	return 0;
}

/* zigbeeUnbindDevice()
 *
 * \brief : this routine publish unbind command to enddevice to clear binding-table
 of end device
 *
 * \param [in] mosq         : Pointer to instance of mosquitto.
 *        [in] deviceObject : Node of end device to be unbind.
 *
 * \return : 0 on success , -1 on failure
 */
int zigbeeUnbindDevice(struct mosquitto *mosq, cJSON *deviceObject) {

	int l_count,l_count1;
	int cluster_count = 0;
	int clusterId = 0;
	int status = MOSQ_ERR_INVAL;
	char command[256]={0};
	char mqttTopic[32] = {0};
	char * eui64String = NULL;
	char * nodeId = NULL;

	nodeId = cJSON_GetObjectItem(deviceObject, "nodeId")->valuestring;

	cJSON * deviceEndpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	cJSON *clusterInfo = cJSON_GetObjectItem(deviceEndpoint, "clusterInfo");
	cluster_count = cJSON_GetArraySize(clusterInfo);

	for ( l_count = 0; l_count < cluster_count ; l_count++) {

		cJSON *clusterElement = cJSON_GetArrayItem(clusterInfo, l_count);

		char *clusterIdStr = cJSON_GetObjectItem(clusterElement, "clusterId")->valuestring;
		sscanf (&clusterIdStr[2],"%x",&clusterId);

		for ( l_count1 = 0 ;  l_count1 < sizeof(ReportableZCLAttributeArray)/sizeof(ZCLATTRIBUTENAME) ; l_count1++) {

			if ( clusterId == ReportableZCLAttributeArray[l_count1].clusterId ) {
				snprintf (command, sizeof(command), "{\"commands\":[{\"command\":\"zdo unbind unicast %s {%s} 1 %s {%s} 1\",\"postDelayMs\":%d}]}", nodeId, &eui64String[2], clusterIdStr, gatewayEui64, UNBIND_POST_DELAY);

				snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
				status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
				if ( status != MOSQ_ERR_SUCCESS){
					logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
					return -1;
				}
				logBtGattInfo ("Successfully publish Unbind command \"%s\" on topic %s\n", command, mqttTopic);
				break;
			}
		}
	}

	return 0;
}

/* removeJoinedDevices()
 *
 * \brief : this routine remove device from gateway network which is not provision.
 *
 * \param [in] mosq : Pointer to instance of mosquitto.
 *
 * \return : 0 on success , -1 on failure
 */
int removeJoinedDevices(struct mosquitto *mosq){

	int l_count,l_count1;
	int provisionedDeviceCount = 0;
	int discoveredDeviceCount = 0;
	int status = 0;
	char * eui64String = NULL;
	char * nodeId = NULL;
	char * provisionedEui64String = NULL;
	cJSON * deviceObject = NULL;
	cJSON * deviceEndpoint = NULL;
	cJSON * provisionedDeviceObject = NULL;
	bool deleteDevice = true;

	if (mosq == NULL){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}

	/* If gateway is not connected to cloud then provisioned device list may be NULL.
	 * If provisioned devicelist is NULL then do not remove any devices from network
	 */
	if ( gatewayConnectivity == 0 ) {
		logBtGattInfo("Gateway is not connected to cloud.\n");
		return 0;
	}
	logBtGattInfo ("Pthread mutex lock\n");
	pthread_mutex_lock(&lock);

	printAllDeviceList();
	printProvisionedDeviceList();

	provisionedDeviceCount = cJSON_GetArraySize(provisionedDeviceList);
	discoveredDeviceCount = cJSON_GetArraySize(root);

	// compare Joined device list with provisioned device list
	for (l_count = 0 ; l_count < discoveredDeviceCount ; l_count++) {

		deleteDevice = true;
		deviceObject = cJSON_GetArrayItem(root, l_count);
		deviceEndpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");

		eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;
		nodeId = cJSON_GetObjectItem(deviceObject, "nodeId")->valuestring;

		for (l_count1 = 0 ; l_count1 < provisionedDeviceCount; l_count1++) {

			provisionedDeviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count1);
			provisionedEui64String = cJSON_GetObjectItem(provisionedDeviceObject, "deviceUid")->valuestring;
			/* If eui64 string doesn't match then remove device from network */
			if (!strncmp(provisionedEui64String, &eui64String[2], strlen(provisionedEui64String))) {
				deleteDevice = false;
				break;
			}
		}
		if (deleteDevice) {

			// No need to send unbind command as custom command is used to bind device
			/*
			status = zigbeeUnbindDevice(mosq, deviceObject);
			if ( status == -1){
				logBtGattErr ("Failed to unbind device : %s\n",nodeId);
				logBtGattInfo ("Pthread mutex unlock\n");
				pthread_mutex_unlock(&lock);
				return -1;
			}
			*/
			status = zigbeeRemoveDevice(mosq, nodeId);
			if ( status == -1){
				logBtGattErr ("Failed to remove device : %s\n",nodeId);
				logBtGattInfo ("Pthread mutex unlock\n");
				pthread_mutex_unlock(&lock);
				return -1;
			}
			// Remove device from device joined list
			cJSON_DeleteItemFromArray(root, l_count);
			printAllDeviceList();
			l_count--;
			discoveredDeviceCount--;
		}
	}

	logBtGattInfo ("Pthread mutex unlock\n");
	pthread_mutex_unlock(&lock);
	return status;
}

/* bleShuttingDownCallback()
 *
 * \brief : this routine called when ble gets shutting down callback.
 *
 * \param [in] mosq : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int bleShuttingDownCallback(struct mosquitto *mosq, const struct mosquitto_message *message){

	int status = MOSQ_ERR_INVAL;
	char mqttTopic[32] = {0};

	/* Provisioning is done */
	provisionInprogress = 0;

	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);

	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(closeNetworkCommand), closeNetworkCommand, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		return -1;
	}
	logBtGattInfo ("Successfully publish Discover devices payload \"%s\" on topic %s\n", closeNetworkCommand, mqttTopic);

	status = removeJoinedDevices(mosq);

	return status;
}

/* registerDevice()
 * \brief : Register Device function to create and publish device
 *  		registration payload to selene
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 *        [in] payload : Pointer to payload received when device joined.
 *
 * \return : 0 on success , -1 on failure
 */
int registerDevice(struct mosquitto *mosq, char * payload) {

	int l_count = 0,l_count1 = 0;
	int deviceType = 0;
	int deviceVersion = 0;
	int status = MOSQ_ERR_INVAL;
	cJSON *property = NULL;
	cJSON * deviceObject = NULL;
	char * deviceName = NULL;
	char * eui64String = NULL;
	char * deviceTypeStr = NULL;
	char * channel = NULL;
	char deviceVersionString[32] = {0};
	char * ptr = NULL;

	if (payload == NULL) {
		logBtGattErr ("Invalid pointer\n");
		return -1;
	}

	cJSON *node = cJSON_Parse(payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	deviceName = cJSON_GetObjectItem(node, "deviceName")->valuestring;

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	deviceTypeStr = cJSON_GetObjectItem(node, "deviceType")->valuestring;
	sscanf (&deviceTypeStr[2],"%x",&deviceType);

	// Get Device type name
	for ( l_count = 0 ; l_count < sizeof(deviceTypeArray)/sizeof(DEVICETYPE) ; l_count++) {

		if ( deviceType == deviceTypeArray[l_count].deviceTypeId ) {
			break;
		}
	}

	// Create device registration payload
	cJSON * deviceNode = cJSON_CreateObject();
	cJSON_AddStringToObject(deviceNode , "deviceUid", &eui64String[2]);
	cJSON_AddStringToObject(deviceNode , "deviceType", deviceTypeArray[l_count].deviceTypeName);
	cJSON_AddStringToObject(deviceNode , "deviceName", deviceName);

	if ( deviceType == DEVICE_ID_DIMMABLE_LIGHT ) {
		cJSON_AddStringToObject(deviceNode , "softwareName", LEDNODE_SOFTWARE_PRODUCT_NAME);
	} if ( deviceType == DEVICE_TYPE_SOIL_NODE ) {
		cJSON_AddStringToObject(deviceNode , "softwareName", SOILNODE_SOFTWARE_PRODUCT_NAME);
	}

	// Get software version
	for (l_count1 = 0; l_count1 < cJSON_GetArraySize(deviceFirmwareVersionList); l_count1++) {

		deviceObject = cJSON_GetArrayItem(deviceFirmwareVersionList, l_count1);
		char * deviceEui64String = cJSON_GetObjectItem(deviceObject, "eui64")->valuestring;
		if (!strncmp (eui64String, deviceEui64String, strlen(deviceEui64String))) {
			deviceVersion = cJSON_GetObjectItem(deviceObject,"version")->valueint;
			sprintf (deviceVersionString, "%d.0.0",deviceVersion);
			cJSON_AddStringToObject(deviceNode , "softwareVersion", deviceVersionString);
		}
	}

	property = cJSON_AddArrayToObject(deviceNode , "properties");

	if ( deviceType == DEVICE_ID_DIMMABLE_LIGHT ) {
		for ( l_count = LED_CHANNEL_1 ; l_count < LED_CHANNEL_MAX ; l_count++ ) {
			channel = cJSON_GetObjectItem(node, led_state_control_array[l_count])->valuestring;
			cJSON_AddStringToObject ( deviceNode, led_state_control_array[l_count], channel);
		}
	}

	ptr = cJSON_PrintUnformatted(deviceNode);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(node);
		cJSON_Delete(deviceNode);
		return -1;
	}
	status = mosquitto_publish(mosq, NULL, MQTT_PUB_DEVICE_REGISTER, strlen(ptr), ptr, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_DEVICE_REGISTER, status);
		free(ptr);
		cJSON_Delete(node);
		cJSON_Delete(deviceNode);
		return -1;
	}
	logBtGattInfo ("Successfully publish Device registration payload \"%s\" on topic %s \n", ptr, MQTT_PUB_DEVICE_REGISTER);

	free(ptr);
	cJSON_Delete(node);
	cJSON_Delete(deviceNode);
	return 0;
}

/* createDeviceTemplate()
 *
 * \brief : this routine will create payload contain other meta
 *          information about device like is device is sleepy or
 *          id relay is supported etc...
 *
 * \param [in] node :  payload received at time of device joined
 *
 * \return : 0 on success , -1 on failure
 */
int createDeviceTemplate ( cJSON * node ) {

	int l_count = 0;
	int l_count1 = 0;;
	int deviceType = 0;;
	int clusterId = 0;;
	int cluster_count = 0;
	bool retissleep = false;
	bool isrelaysupport = false;
	char * deviceTypeStr = NULL;
	char * ptr = NULL;

	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	deviceTypeStr = cJSON_GetObjectItem(node, "deviceType")->valuestring;
	sscanf (&deviceTypeStr[2],"%x",&deviceType);

	cJSON_AddStringToObject(node, "gatewayEui", gatewayEui64);
	cJSON_AddBoolToObject(node, "zclRequested", true);

	retissleep = isSleepyDevice (node);
	cJSON_AddBoolToObject(node, "sleepyDevice", retissleep);

	cJSON_AddBoolToObject(node, "otaUpdating", false);
	cJSON_AddNumberToObject(node, "otaTotalBytesSent", 0);
	cJSON_AddNumberToObject(node, "otaUpdatePercent", 0);
	cJSON_AddNumberToObject(node, "otaTargetImageSizeKB", 0);
	cJSON_AddNumberToObject(node, "otaTargetFirmwareVersion", 0);

	isrelaysupport = isRelaySupported (deviceType);
	cJSON_AddBoolToObject(node, "supportsRelay", isrelaysupport);

	cJSON *deviceendpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	cJSON *clusterInfo = cJSON_GetObjectItem(deviceendpoint, "clusterInfo");
	cluster_count = cJSON_GetArraySize(clusterInfo);

	if ( clusterInfo != NULL ) {

		cJSON *supportedCluster;
		supportedCluster = cJSON_AddArrayToObject(node,"supportedCluster");

		for (l_count = 0 ; l_count < cluster_count ;l_count++ ) {

			cJSON *clusterElement = cJSON_GetArrayItem(clusterInfo, l_count);

			char *clusterIdStr = cJSON_GetObjectItem(clusterElement, "clusterId")->valuestring;
			char *clusterType = cJSON_GetObjectItem(clusterElement, "clusterType")->valuestring;

			sscanf (&clusterIdStr[2],"%x",&clusterId);

			for (l_count1 = 0; l_count1 < sizeof(ZCLAttributeArray)/sizeof(ZCLATTRIBUTENAME); l_count1++) {
				if ( clusterId == ZCLAttributeArray[l_count1].clusterId) {

					cJSON *resolution = cJSON_CreateObject();

					cJSON_AddStringToObject(resolution, "clusterId", clusterIdStr);

					if ( clusterId < IN_OUT_DECISION_CLUSTER && clusterId != POWER_CONF_CLUSTER) {

						if (!strncmp(clusterType ,"In", strlen(clusterType))) {
							strncpy(clusterType,"Out", strlen("Out"));
						} else if(!strncmp(clusterType ,"Out", strlen(clusterType))) {
							strncpy(clusterType,"In", strlen("In"));
						}
					}
					cJSON_AddStringToObject(resolution, "clusterType", clusterType);
					cJSON_AddItemToArray(supportedCluster,resolution);

					break;
				}
			}
		}
	}

	ptr = cJSON_PrintUnformatted(node);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}
	logBtGattInfo ("Successfully createDeviceTemplate payload :: %s\n", ptr);
	free(ptr);

	return 0;
}

/* requestNodeDefaultAttributes()
 * \brief : this routine used to publish zcl command to gateway.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] node : payload received at the time device joined
 *
 * \return : 0 on success , -1 on failure
 */

int requestNodeDefaultAttributes(struct mosquitto * mosq, cJSON * node){

	char command[256] = {0};
	char mqttTopic[32] = {0};
	int l_count,l_count1;
	int clusterId = 0;;
	int cluster_count = 0;
	int endpoint = 0;
	int status = MOSQ_ERR_INVAL;
	char * eui64String = NULL;

	if ((mosq == NULL) || (node == NULL)){
		logBtGattErr("Invalid pointers\n");
		return -1;
	}

	cJSON *supportedCluster = cJSON_GetObjectItem(node, "supportedCluster");
	cluster_count = cJSON_GetArraySize(supportedCluster);

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	endpoint = cJSON_GetObjectItem(deviceEndpoint, "endpoint")->valueint;
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	for (l_count = 0 ; l_count < cluster_count ; l_count++) {

		cJSON *clusterElement = cJSON_GetArrayItem(supportedCluster, l_count);
		char *clusterIdStr = cJSON_GetObjectItem(clusterElement, "clusterId")->valuestring;
		sscanf (&clusterIdStr[2],"%x",&clusterId);

		for ( l_count1 = 0 ;  l_count1 < sizeof(ZCLAttributeArray)/sizeof(ZCLATTRIBUTENAME) ; l_count1++) {

			if ( clusterId == ZCLAttributeArray[l_count1].clusterId &&
					ZCLAttributeArray[l_count1].attributeId != -1 ) {

				if ( (clusterId == BOOTLOADER_CLUSTER ) && ( ZCLAttributeArray[l_count1].attributeId ==  FIRMWARE_VERSION_ATTRIBUTE||
							ZCLAttributeArray[l_count1].attributeId == IMAGE_TYPE_ATTRIBUTE||
							ZCLAttributeArray[l_count1].attributeId == MANUFACTURER_ID_ATTRIBUTE )) {

					snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"zcl global direction 0x1\",\"postDelayMs\":100},{\"command\": \"zcl global read  %s 0x%x\",\"postDelayMs\":100 },{\"command\": \"plugin device-table send {%s} 0x%d \",\"postDelayMs\":100}]}" ,clusterIdStr,ZCLAttributeArray[l_count1].attributeId , &eui64String[2] ,endpoint);

				} else {

					snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"zcl global direction 0x0\",\"postDelayMs\":100},{\"command\": \"zcl global read  %s 0x%x\",\"postDelayMs\":100 },{\"command\": \"plugin device-table send {%s} 0x%d \",\"postDelayMs\":100}]}", clusterIdStr,ZCLAttributeArray[l_count1].attributeId , &eui64String[2] ,endpoint);

				}
				snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
				status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
				if ( status != MOSQ_ERR_SUCCESS){
					logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
					return -1;
				}
				logBtGattInfo ("Successfully publish command \"%s\" on topic %s\n", command, mqttTopic);
			}
		}
	}
	return 0;
}

/* sendBindCommandToCoordinator()
 * \brief : Create and publish bind command to host
 *
 * \param [in] mosq         : Pointer to instance of mosquitto.
 * \param [in] eui64        : Unique id of joined device.
 *
 * \return : 0 on success , -1 on failure
 */
int sendBindCommandToCoordinator(struct mosquitto *mosq, char * eui64){

	char command[256]={0};
	char mqttTopic[32] = {0};
	int status = MOSQ_ERR_INVAL;

	snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom soilnode-bind-request {%s}\",\"postDelayMs\":100}]}",&eui64[2]);

	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		return -1;
	}
	logBtGattInfo ("Successfully publish bind command \"%s\" on topic %s\n", command, mqttTopic);
	return 0;
}

/* genericConfigureBind()
 * \brief : Create and publish bind command to host
 *
 * \param [in] mosq         : Pointer to instance of mosquitto.
 * \param [in] endpoint     : Device endpoint received at time of device joined.
 * \param [in] nodeId       : Network address of joined device.
 * \param [in] clusterIdStr : Cluster id received at time of device joined.
 * \param [in] eui64        : Unique id of joined device.
 *
 * \return : 0 on success , -1 on failure
 */
int genericConfigureBind(struct mosquitto *mosq, int endpoint, char *nodeId, char *clusterIdStr ,  char * eui64){

	char command[256]={0};
	char mqttTopic[32] = {0};
	int status = MOSQ_ERR_INVAL;

	snprintf (command, sizeof(command), "{\"commands\":[{\"command\":\"zdo bind %s 0x1 0x1 %s {%s} {%s}\"},{\"command\":\"plugin device-table send {%s} 0x1\",\"postDelayMs\":%d}]}",
			nodeId, clusterIdStr,&eui64[2],gatewayEui64,&eui64[2],BIND_POST_DELAY);

	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		return -1;
	}
	logBtGattInfo ("Successfully publish bind command \"%s\" on topic %s\n", command, mqttTopic);
	return 0;
}

/* bindAndSetNodeReportableAttributes()
 *
 * \brief : This routine checks if device has reportable attributes or not.
 *	    If yes then publish bind commands.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] node : Payload received when device joined.
 *
 * \return : 0 on success , -1 on failure
 */
int bindAndSetNodeReportableAttributes(struct mosquitto *mosq, cJSON * node) {

	int l_count,l_count1;
	int clusterId = 0;
	int cluster_count = 0;
	int endpoint = 0;
	int status = 0;
	int deviceType = 0;
	char * eui64String = NULL;
	char * nodeId = NULL;
	char * deviceTypeStr = NULL;

	if ((mosq == NULL) || (node == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}

	cJSON *supportedCluster = cJSON_GetObjectItem(node, "supportedCluster");
	cluster_count = cJSON_GetArraySize(supportedCluster);

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	endpoint = cJSON_GetObjectItem(deviceEndpoint, "endpoint")->valueint;
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;
	nodeId = cJSON_GetObjectItem(node, "nodeId")->valuestring;

	deviceTypeStr = cJSON_GetObjectItem(node, "deviceType")->valuestring;
	sscanf (&deviceTypeStr[2],"%x",&deviceType);

	/* If device type is soilnode then send custom binding command to soilnode
            to bind all the clutser by itself else do generic bind process for all other devices*/
	if ( deviceType == DEVICE_TYPE_SOIL_NODE ) {
		sendBindCommandToCoordinator(mosq, eui64String);
	} else {
		for (l_count = 0 ; l_count < cluster_count ; l_count++) {

			cJSON *clusterElement = cJSON_GetArrayItem(supportedCluster, l_count);

			char *clusterIdStr = cJSON_GetObjectItem(clusterElement, "clusterId")->valuestring;
			char *clusterType = cJSON_GetObjectItem(clusterElement, "clusterType")->valuestring;

			sscanf (&clusterIdStr[2],"%x",&clusterId);

			if ( (!strncmp(clusterType,"In",strlen(clusterType))) && ( clusterId != BOOTLOADER_CLUSTER )) {
				for ( l_count1 = 0 ;  l_count1 < sizeof(ReportableZCLAttributeArray)/sizeof(ZCLATTRIBUTENAME) ; l_count1++) {

					if ( clusterId == ReportableZCLAttributeArray[l_count1].clusterId &&
							ReportableZCLAttributeArray[l_count1].attributeId != -1 ) {
						status = genericConfigureBind(mosq, endpoint, nodeId, clusterIdStr, eui64String);
						if ( status == -1){
							logBtGattErr ("Failed to bind device : nodeId = %s\n", nodeId);
							return -1;
						}
					}
				}
			}
		}
	}
	return 0;
}

/* onNodeJoin()
 *
 * \brief : this routine called to to bind joined device.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] payload : Payload received when device joined.
 *
 * \return : 0 on success , -1 on failure
 */
int onNodeJoin (struct mosquitto *mosq, cJSON * payload) {

	int status = 0;
	bool retendpoint = false;
	bool retstate = false;

	if ((mosq == NULL) || (payload == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}
	retendpoint =  isEndpointSupported (payload);
	retstate = isDeviceDisplayable(payload);

	if (retendpoint && retstate) {
		status = createDeviceTemplate(payload);
		if ( status == -1 )
			return -1;

		status = requestNodeDefaultAttributes(mosq, payload);
		if (status == -1)
			return -1;

		status = bindAndSetNodeReportableAttributes(mosq, payload);
		if ( status == -1)
			return -1;
	}
	logBtGattInfo ("retendpoint == %d\nretstate== %d\n",retendpoint,retstate);

	return status;
}

/* formatAndAssignValue()
 *
 * \brief : this routine is used to formate attribute data.
 *
 * \param [in] property : Name of attribute data property.
 * \param [in] value    : Value of attribute.
 *
 * \return : formatValue
 */
double formatAndAssignValue (char * property , long value) {

	double formatValue = 0;
	if (!strncmp(property, "temperatureValue",strlen(property))){
		formatValue = (double)value/100;
	} else if (!strncmp(property, "batteryVoltage",strlen(property))){
		formatValue = (double)value/10;
	} else if ((!strncmp(property, "soilPh",strlen(property))) || (!strncmp(property, "soilMoisture",strlen(property)))) {
		formatValue = (double)value/100;
	} else
		formatValue = (double)value;

	return formatValue;
}

/* zclParseBufferRaw()
 * \brief : this routine parse data buffer received at time of reporting by
 *          enddevice according to attribute data type
 *
 * \param [in] attributeDataType : type of data received in zcl response payload
 * \param [in] attributeBuffer : data buffer received in zcl response payload
 *
 * \return attributeBufferLong : Parsed data
 */
long zclParseBufferRaw(char * attributeDataType , char * attributeBuffer) {

	int attributeDataTypeInt;
	long long attributeBufferLong = 0;
	sscanf (attributeDataType,"%x", &attributeDataTypeInt);

	attributeBufferLong = (long long) l2b_endian(strtoll(attributeBuffer, NULL, 16), ZCLDataTypesArray[attributeDataTypeInt].numBytes );

	if (ZCLDataTypesArray[attributeDataTypeInt].isSigned) {
		attributeBufferLong = attributeDataToSigned(attributeBufferLong, ZCLDataTypesArray[attributeDataTypeInt].numBytes);
	}

	return attributeBufferLong;

}

/* onAttributeUpdate()
 * \brief : function to receive attributes value
 *  		and create & publish telemetry payload to selene
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int onAttributeUpdate (struct mosquitto * mosq , const struct mosquitto_message * message) {

	int l_count1 = 0;
	int clusterId = 0;
	int attributeId = 0;
	int attributeDataTypeInt = 0;
	char * clusterIdStr = NULL;
	char * attributeIdStr = NULL;
	char * attributeBuffer = NULL;
	char * eui64 = NULL;
	char * attributeDataType = NULL;
	char * ptr = NULL;
	char property[64] = {0};
	char mqttTopic[64] = {0};
	long attributeBufferLong = 0;
	long pHAttributeBuffer = 0;
	long moistureAttributeBuffer = 0;
	double convertedValue = 0;
	int status = MOSQ_ERR_INVAL;

	if ((mosq == NULL) || (message == NULL)) {
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}
	cJSON * node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	clusterIdStr = cJSON_GetObjectItem(node, "clusterId")->valuestring;
	sscanf (clusterIdStr,"%x",&clusterId);

	attributeIdStr = cJSON_GetObjectItem(node, "attributeId")->valuestring;
	sscanf (attributeIdStr,"%x",&attributeId);

	attributeBuffer = cJSON_GetObjectItem(node, "attributeBuffer")->valuestring;

	attributeDataType = cJSON_GetObjectItem(node, "attributeDataType")->valuestring;
	sscanf (attributeDataType,"%x",&attributeDataTypeInt);

	cJSON * deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	eui64 = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	if ( clusterId == BOOTLOADER_CLUSTER) {
		logBtGattInfo ("Boot loader cluster found\n");
		return -1;
	}

	// To get attribute name
	for (l_count1 = 0;l_count1 < sizeof(ZCLAttributeArray) / sizeof(ZCLATTRIBUTENAME); l_count1++) {

		if (clusterId == ZCLAttributeArray[l_count1].clusterId
				&& attributeId == ZCLAttributeArray[l_count1].attributeId ) {
			break;
		}
	}

	// If friendly name not found
	if (l_count1 == sizeof(ZCLAttributeArray) / sizeof(ZCLATTRIBUTENAME)) {
		logBtGattErr ("AttributeId %s not found\n",attributeIdStr);
		cJSON_Delete(node);
		return -1;
	}

	// create payload to publish telemetry
	cJSON * telemetryNode = cJSON_CreateObject();
	if (telemetryNode == NULL) {
		logBtGattErr ("Failed to create json object\n");
		cJSON_Delete(node);
		return -1;
	}

	// parse soil pH and soil Moisture data
	if ( clusterId == SOIL_NODE_PH_CLUSTER ) {

		attributeBufferLong = strtoll(attributeBuffer, NULL, 16);
		pHAttributeBuffer = attributeBufferLong & 0x0000FFFF;
		moistureAttributeBuffer = (attributeBufferLong >> 16) & 0x0000FFFF;

		// Read Soil pH data
		attributeBufferLong = (long) l2b_endian(pHAttributeBuffer, ATTRIBUTE_DATATYPE_INT16_LEN);
		convertedValue = formatAndAssignValue("soilPh", attributeBufferLong);
		cJSON_AddNumberToObject(telemetryNode , "f|soilPh", convertedValue);

		// Read Soil Moisture data
		attributeBufferLong = (long) l2b_endian(moistureAttributeBuffer, ATTRIBUTE_DATATYPE_INT16_LEN);
		attributeBufferLong = attributeDataToSigned(attributeBufferLong, ATTRIBUTE_DATATYPE_INT16_LEN);
		convertedValue = formatAndAssignValue("soilMoisture", attributeBufferLong);
		cJSON_AddNumberToObject(telemetryNode , "f|soilMoisture", convertedValue);
	}
	else {
		// Parse buffer and return value string
		attributeBufferLong = zclParseBufferRaw(attributeDataType, attributeBuffer);

		convertedValue = formatAndAssignValue(ZCLAttributeArray[l_count1].attribute_name, attributeBufferLong);

		snprintf (property ,sizeof(property), "%c|%s",ZCLDataTypesArray[attributeDataTypeInt].dataFormat, ZCLAttributeArray[l_count1].attribute_name);

		cJSON_AddNumberToObject(telemetryNode , property, convertedValue);
	}
	snprintf (mqttTopic, sizeof(mqttTopic), "selene/mqtt/%s/tel",&eui64[2]);
	ptr = cJSON_PrintUnformatted(telemetryNode);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(node);
		cJSON_Delete(telemetryNode);
		return -1;
	}
	status = mosquitto_publish(mosq, NULL, mqttTopic , strlen(ptr), ptr, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		free(ptr);
		cJSON_Delete(node);
		cJSON_Delete(telemetryNode);
		return -1;
	}
	logBtGattInfo ("Successfully publish telemetry \"%s\" on topic ::  %s\n", ptr, mqttTopic);

	/* Update device status */
	if ( clusterId == SOIL_NODE_PH_CLUSTER)
		updateDeviceStatus(&eui64[2], mosq);

	free(ptr);
	cJSON_Delete(node);
	cJSON_Delete(telemetryNode);

	return 0;
}

/* deviceOtaRequest()
 * \brief : This function received ota request from end-device and
 *          and handle firmware version of end-device. If firmware
 *          version is change then give response to selene.
 *
 * \param [in] mosq  : Pointer to instance of mosquitto.
 * \param [in] node  : Pointer to instance of json payload.
 *
 * \return : 0 on success , -1 on failure
 */
int deviceOtaRequest(struct mosquitto * mosq ,cJSON *node) {

	int l_count, l_count1 = 0;
	int status = MOSQ_ERR_INVAL;
	bool clusterSpecific = false;
	char * commandData = NULL;
	char * eui64String = NULL;
	char * deviceUpdateEui64 = NULL;
	char * devicetype = NULL;
	char * ptr = NULL;
	char command[256] = {0};
	char mqttTopic[64] = {0};
	long attributeBufferLong = 0;
	long deviceVersion = 0;
	cJSON * deviceObject = NULL;
	cJSON * device = NULL;

	/* Device version is received when clusterSpecific is true. So if clusterSpecific
	is received false then return from here */
	clusterSpecific = cJSON_GetObjectItem(node, "clusterSpecific")->valueint;
	if ( clusterSpecific == false )
		return 0;

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	/*Update device status */
	updateDeviceStatus(&eui64String[2], mosq);

	commandData = cJSON_GetObjectItem(node, "commandData")->valuestring;

	attributeBufferLong = (long long) l2b_endian(strtoll(commandData, NULL, 16), 4);

	/* Create array to store device firmware version */
	if ( deviceFirmwareVersionList == NULL ) {
		logBtGattInfo ("Firmware version  Device list is null\n");
		deviceFirmwareVersionList = cJSON_CreateArray();
	}

	/* When zigbee coordinator received ota request of end-device then compare the version of device
           with previous firmware verison. If version is changed then send response to selene */
	for (l_count = 0; l_count < cJSON_GetArraySize(deviceFirmwareVersionList); l_count++) {

		deviceObject = cJSON_GetArrayItem(deviceFirmwareVersionList, l_count);
		char * deviceEui64String = cJSON_GetObjectItem(deviceObject, "eui64")->valuestring;

		/* If device match and version is different then update device list */
		if (!strncmp (eui64String,deviceEui64String,strlen(deviceEui64String))) {
			deviceVersion = cJSON_GetObjectItem(deviceObject,"version")->valueint;
			if ( deviceVersion == attributeBufferLong) {
				logBtGattInfo ("Device verison is same do not update\n");
			} else {
				logBtGattInfo ("Device verison is different\n");

				/* response here to cloud*/
				ptr = cJSON_PrintUnformatted(deviceUpdateRequestList);
				if ( ptr != NULL ) {
					logBtGattInfo ("Device update request list == %s\n", ptr);
					free(ptr);
				}

				ptr = cJSON_PrintUnformatted(deviceFirmwareVersionList);
				if ( ptr != NULL ) {
					logBtGattInfo ("Firmware Version list of device == %s\n", ptr);
					free(ptr);
				}

				for ( l_count1 = 0; l_count1 < cJSON_GetArraySize(deviceUpdateRequestList); l_count1++) {

					device = cJSON_GetArrayItem(deviceUpdateRequestList, l_count1);
					deviceUpdateEui64 = cJSON_GetObjectItem(device, "eui64")->valuestring;
					devicetype = cJSON_GetObjectItem(device, "type")->valuestring;
					if ( !strncmp(&eui64String[2], deviceUpdateEui64, strlen(deviceUpdateEui64))) {
						if ( !strcmp(devicetype, "LedNode")) {
							LednodeUpdateCount -= 1;
							logBtGattInfo ("Remaining Lednode device to update == %d\n", LednodeUpdateCount);
							if (LednodeUpdateCount == 0) {
								logBtGattInfo ("Remove file from system\n");
								remove(LEDNODE_FIRMWARE_UPDATE);
							}
						} else {
							SoilnodeUpdateCount -= 1;
							logBtGattInfo ("Remaining Soilnode device to update == %d\n", SoilnodeUpdateCount);
							if (SoilnodeUpdateCount == 0) {
								logBtGattInfo ("Remove file from system\n");
								remove(SOILNODE_FIRMWARE_UPDATE);
							}
						}

						/* send response to selene */
						sendFirmwareUpdateResponseToSelene(mosq, deviceUpdateEui64, "success", "Device updated successfully");
						/* send command to zigbee cooridnator to reload ota-files */
						snprintf (command , sizeof(command), "{\"commands\" : [{\"command\":\"plugin ota-storage-common reload\",\"postDelayMs\":100}]}");
						snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
						status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
						if ( status != MOSQ_ERR_SUCCESS){
							logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
						} else  {
							logBtGattInfo ("Successfully publish payload \"%s\" on topic %s\n", command, mqttTopic);
						}
					}
				}

				/* Remove device from firmware version list request */
				cJSON_DeleteItemFromArray(deviceFirmwareVersionList, l_count);
				l_count = cJSON_GetArraySize(deviceFirmwareVersionList);
				ptr = cJSON_PrintUnformatted(deviceUpdateRequestList);
				if ( ptr != NULL ) {
					logBtGattInfo ("Device update request list == %s\n", ptr);
					free(ptr);
				}
			}
			break;
		}
	}

	if (l_count == cJSON_GetArraySize(deviceFirmwareVersionList)){
		logBtGattInfo ("Device is not in the firmware version list\n");

		deviceObject = cJSON_CreateObject();

		cJSON_AddStringToObject(deviceObject, "eui64", eui64String);
		cJSON_AddNumberToObject(deviceObject, "version", attributeBufferLong);

		cJSON_AddItemToArray(deviceFirmwareVersionList, deviceObject);
	}

	ptr = cJSON_PrintUnformatted(deviceFirmwareVersionList);
	if ( ptr != NULL ) {
		logBtGattInfo ("Firmware Version list of device == %s\n", ptr);
		free(ptr);
	}
	return 0;
}

/* lednodeChannelIntensity()
 * \brief : function to parse channel intensity for lednode
 *          and send on cloud as telemetry.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] node    : Pointer to led configuration payload
 *
 * \return : 0 on success , -1 on failure
 */
int lednodeChannelIntensity(struct mosquitto * mosq ,cJSON *node) {

	char * commandData = NULL;
	char * eui64String = NULL;
	char * ptr = NULL;
	char property[64] = {0};
	char mqttTopic[64] = {0};
	int channelData[6] = {0};
	int led_count = 0;
	int status = MOSQ_ERR_INVAL;

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	commandData = cJSON_GetObjectItem(node, "commandData")->valuestring;
	sscanf (&commandData[2], "%2x %2x %2x %2x %2x %2x",
			&channelData[0],&channelData[1],&channelData[2],
			&channelData[3],&channelData[4],&channelData[5]);

	cJSON * telemetryNode = cJSON_CreateObject();
	for (led_count = LED_CHANNEL_1 ; led_count < LED_CHANNEL_MAX ;led_count++) {
		snprintf (property ,sizeof(property), "i|%s",led_state_control_array[led_count]);
		cJSON_AddNumberToObject(telemetryNode , property, channelData[led_count - 1]);
	}

	snprintf (mqttTopic, sizeof(mqttTopic), "selene/mqtt/%s/tel", &eui64String[2]);
	ptr = cJSON_PrintUnformatted(telemetryNode);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(telemetryNode);
		return -1;
	}

	status = mosquitto_publish(mosq, NULL, mqttTopic , strlen(ptr), ptr, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		free(ptr);
		cJSON_Delete(telemetryNode);
		return -1;
	}
	logBtGattInfo ("Successfully publish telemetry \"%s\" on topic ::  %s\n", ptr, mqttTopic);
	free(ptr);
	cJSON_Delete(telemetryNode);
	return 0;
}

/* lednodeRGBConfiguration()
 * \brief : function to to parse RGB configuration of lednode to
 *  	    ble app.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] node    : Pointer to led configuration payload
 *
 * \return : 0 on success , -1 on failure
 */
int lednodeRGBConfiguration(struct mosquitto * mosq ,cJSON *node) {

	char * commandData = NULL;
	char * eui64String = NULL;
	char * ptr = NULL;
	int channelData[6];
	int ledConfigurationEnable;
	int l_count = 0;
	int status = MOSQ_ERR_INVAL;
	cJSON * ledconfiguration = NULL;

	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	/* check if device is joined in zigbee networj or not
	 * If yes then send data to ble app
	 */
	for (l_count = 0; l_count < cJSON_GetArraySize(root); l_count++) {

		cJSON *joinedDeviceObject = cJSON_GetArrayItem(root, l_count);
		cJSON *joineddeviceEndpoint = cJSON_GetObjectItem(joinedDeviceObject, "deviceEndpoint");
		char * joinedEui64String = cJSON_GetObjectItem(joineddeviceEndpoint,"eui64")->valuestring;

		if (!strncmp(eui64String, joinedEui64String, strlen(joinedEui64String))) {

			commandData = cJSON_GetObjectItem(node, "commandData")->valuestring;
			sscanf (&commandData[2], "%2x %2x %2x %2x %2x %2x %2x",
					&channelData[0],&channelData[1],&channelData[2],
					&channelData[3],&channelData[4],&channelData[5],&ledConfigurationEnable);

			/* Create lednode and publish to ble app*/
			cJSON * lednode = cJSON_CreateObject();
			cJSON_AddStringToObject(lednode, "eui64", &eui64String[2]);
			cJSON_AddStringToObject(lednode, "deviceType", "LedNode");
			cJSON_AddNumberToObject(lednode, "ledconfigurationflag", ledConfigurationEnable);
			cJSON_AddItemToObject(lednode, "ledconfiguration", ledconfiguration = cJSON_CreateObject());

			for ( l_count = LED_CHANNEL_1 ; l_count < LED_CHANNEL_MAX ; l_count++){
				cJSON_AddStringToObject(ledconfiguration,led_state_control_array[l_count],led_RGB_control_array[channelData[l_count - 1]]);
			}

			// Publish from gateway to ble server
			ptr = cJSON_PrintUnformatted(lednode);
			if (ptr == NULL ) {
				logBtGattErr ("Failed to parse json\n");
				cJSON_Delete(lednode);
				return -1;
			}
			status = mosquitto_publish(mosq, NULL, MQTT_PUB_DISCOVERED_DEVICE, strlen(ptr), ptr , 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_DISCOVERED_DEVICE, status);
				free(ptr);
				cJSON_Delete(lednode);
				return -1;
			}
			logBtGattInfo ("Successfully publish device payload \"%s\" to ble-server on topic %s\n", ptr, MQTT_PUB_DISCOVERED_DEVICE);
			free(ptr);
			cJSON_Delete(lednode);
			return 0;
		}
	}
	return -1;
}

/* onZCLResponse()
 *
 * \brief : This callback function called when received attribute data over mqtt topic.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int onZCLResponse (struct mosquitto * mosq , const struct mosquitto_message * message) {

	int status = 0;
	char * commandIdString = NULL;
	char * clusterIdString = NULL;
	int commandId = 0;
	int clusterId = 0;

	if ((mosq == NULL) || (message == NULL)){
		logBtGattErr ("invalid pointers\n");
		return -1;
	}
	cJSON * node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}
	cJSON * commandIdProperty = cJSON_GetObjectItem(node, "commandId");

	if ( commandIdProperty != NULL ) {
		commandIdString = cJSON_GetObjectItem(node, "commandId")->valuestring;
		sscanf (commandIdString, "%x", &commandId);

		clusterIdString = cJSON_GetObjectItem(node, "clusterId")->valuestring;
		sscanf (clusterIdString, "%x", &clusterId);

		if ( (commandId == LEDNODE_RGB_MAPPING_COMMAND) && (clusterId == LED_NODE_RGB_CONFIG_COMMAND_CLUSTER)) {
			status = lednodeRGBConfiguration(mosq, node);
		} else if ((commandId == LEDNODE_CHANNEL_INTENSITY) && (clusterId == LED_NODE_RGB_CONFIG_COMMAND_CLUSTER)) {
			status = lednodeChannelIntensity(mosq, node);
		} else if (((commandId == DEVICE_UPDATE_COMMAND) && (clusterId == BOOTLOADER_CLUSTER)) ||
				((commandId == END_DEVICE_FIRMWARE_COMMAND) && (clusterId == CUSTOM_FIRMWARE_VERSION_CLUSTER))) {
			status = deviceOtaRequest(mosq, node);
		}
	} else {
		status = onAttributeUpdate(mosq, message);
	}
	cJSON_Delete(node);
	return status;
}

/* isDeviceProvisioned()
 *
 * \brief : this function check new joined device is already provisioned or not
 *
 * \param [in] joinedDevice : received payload when device joined
 *
 * \return : If device is already provisioned then return true else false.
 */
bool isDeviceProvisioned(cJSON * joinedDevice) {

	int l_count;
	bool ret = false;
	char * eui64String = NULL;

	cJSON *deviceEndpoint = cJSON_GetObjectItem(joinedDevice, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	for (l_count = 0; l_count < cJSON_GetArraySize(provisionedDeviceList); l_count++) {

		cJSON *provisionedDeviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count);
		char * provisionedEui64String = cJSON_GetObjectItem(provisionedDeviceObject, "deviceUid")->valuestring;
		if (!strncmp (&eui64String[2],provisionedEui64String,strlen(provisionedEui64String))) {
			logBtGattInfo ("device is already provisioned with eui64 = %s\n",provisionedEui64String);
			ret = true;
			break;
		}
	}
	return ret;
}

/* checkIfDeviceIsAlreadyJoined()
 *
 * \brief function to check Joined device is already present or not
 * 		  If device is already present then replace old payload with new payload in list.
 *
 * \param [in] node : payload received when device joined
 *
 * \return : If device is present then return true else false
 */
bool checkIfDeviceIsAlreadyJoined( cJSON *node) {

	int l_count;
	int ret = false;
	char * newEui64String = NULL;

	cJSON *newdeviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	newEui64String = cJSON_GetObjectItem(newdeviceEndpoint, "eui64")->valuestring;

	for (l_count = 0; l_count < cJSON_GetArraySize(root); l_count++) {

		cJSON *joinedDeviceObject = cJSON_GetArrayItem(root, l_count);
		cJSON *joineddeviceEndpoint = cJSON_GetObjectItem(joinedDeviceObject, "deviceEndpoint");
		char * joinedEui64String = cJSON_GetObjectItem(joineddeviceEndpoint,"eui64")->valuestring;

		if (!strncmp(newEui64String, joinedEui64String, strlen(newEui64String))) {

			cJSON_ReplaceItemInArray(root, l_count, node);
			ret = true;
			break;
		}
	}
	return ret;
}

/* checkGatewayHeartbeat()
 *
 * \brief function to check geteway network is up or not.
 *        if not then create and up gateway network
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int checkGatewayHeartbeat(struct mosquitto *mosq, const struct mosquitto_message * message) {

	char command[256] = {0};
	char mqttTopic[32] = {0};
	bool checkNetworkUp = 0;
	char * gatewayeui64 = NULL;
	int status = MOSQ_ERR_INVAL;

	if ((mosq == NULL) || (message == NULL)) {
		logBtGattErr ("Invalid pointer\n");
		return -1;
	}

	cJSON *node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	// set global gatewayeui64 string
	if (gatewayEui64 == NULL) {
		gatewayeui64 = cJSON_GetObjectItem(node, "eui64")->valuestring;
		gatewayEui64 = (char *)malloc(strlen(gatewayeui64) + 1);
		if(gatewayEui64 == NULL) {
			cJSON_Delete(node);
			logBtGattErr ("Failed to allocate memory\n");
			return -1;
		}
		memset(gatewayEui64, 0x0, strlen(gatewayeui64) + 1);
		strncpy(gatewayEui64, gatewayeui64, strlen(gatewayeui64));

		logBtGattInfo ("Gateway Eui64 string == %s\n",gatewayEui64);

		/* Send command to zigbee coordinator to get device list*/

		snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"plugin device-table print\",\"postDelayMs\":100}]}");
		snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
		status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			cJSON_Delete(node);
			return -1;
		}
		logBtGattInfo ("Successfully publish command to get device list from zigbee coordinator\n");
	}

	checkNetworkUp = cJSON_GetObjectItem(node, "networkUp")->valueint;

	if ( checkNetworkUp == false ) {
		snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"plugin network-creator start 1\",\"postDelayMs\":100}]}");
		snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
		status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			cJSON_Delete(node);
			return -1;
		}
		logBtGattInfo ("Successfully publish create and open zigbee coordinator network payload\n");
	}
	cJSON_Delete(node);
	return 0;
}

/* deviceStateControl()
 *
 * \brief : this routine is called to handle device state change request.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int deviceStateControl( struct mosquitto *mosq , const struct mosquitto_message *message){

	char * stateCommand = NULL;
	char * deviceUid = NULL;
	char command[256] = {0};
	char mqttTopic[64] = {0};
	char * value = NULL;
	int status = -1;
	int l_count;

	if ((mosq == NULL) || (message == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}

	cJSON *node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}
	stateCommand = cJSON_GetObjectItem(node, "command")->valuestring;
	deviceUid = cJSON_GetObjectItem(node, "deviceUid")->valuestring;

	cJSON * properties = cJSON_GetObjectItem(node, "properties");
	if ( properties  == NULL) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(node);
		return -1;
	}

	for ( l_count = LED_CHANNEL_1 ; l_count < LED_CHANNEL_MAX ; l_count++) {

		if (cJSON_GetObjectItem(properties, led_state_control_array[l_count])) {
			value = cJSON_GetObjectItem(properties, led_state_control_array[l_count])->valuestring;

			snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"zcl level-control mv-to-level %X 1 1 1\"},{\"command\": \"plugin device-table send {%s} 0x%d \",\"postDelayMs\":%d}]}", atoi(value), deviceUid, l_count, MV_TO_LEVEL_DELAY);
			snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
			status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
				cJSON_Delete(node);
				return -1;
			}
			logBtGattInfo ("Successfully publish command \"%s\" on topic ::  %s\n", command, mqttTopic);
		}
	}
	cJSON_Delete(node);
	return 0;
}

/* seleneCommandHandler()
 *
 * \brief : this routine is called to handle selene command request
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int seleneCommandHandler( struct mosquitto *mosq , const struct mosquitto_message *message){

	char * command = NULL;
	char * deviceUid = NULL;
	char mqttCommand[256] = {0};
	char mqttTopic[64] = {0};
	int discoveredDeviceCount = 0;
	int l_count;
	int status = -1;

	if (( mosq == NULL) || (message == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}
	cJSON *node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}
	command = cJSON_GetObjectItem(node, "command")->valuestring;
	deviceUid = cJSON_GetObjectItem(node, "deviceUid")->valuestring;

	if (!strncmp(command, "led_intensity",strlen(command))) {

		discoveredDeviceCount = cJSON_GetArraySize(root);
		for (l_count = 0 ; l_count < discoveredDeviceCount; l_count++) {

			cJSON * deviceObject = cJSON_GetArrayItem(root, l_count);
			cJSON * deviceEndpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");
			char * eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

			if (!strncmp(deviceUid,&eui64String[2],strlen(deviceUid))) {

				status = requestNodeDefaultAttributes(mosq, deviceObject);
				if (status == -1) {
					cJSON_Delete(node);
					return -1;
				}
				break;
			}
		}
	} else if (!strncmp(command, "getChannelIntensity",strlen(command))) {

		snprintf (mqttCommand, sizeof(mqttCommand), "{\"commands\":[{\"command\": \"custom get-led-intensity {%s}\",\"postDelayMs\":100}]}", deviceUid);
		snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
		status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(mqttCommand), mqttCommand, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			cJSON_Delete(node);
			return -1;
		}
		logBtGattInfo ("Successfully publish command \"%s\" on topic ::  %s\n", command, mqttTopic);
	}
	cJSON_Delete(node);
	return 0;
}

/* deviceDeletionRequest()
 *
 * \brief : this routine remove device from gateway network.
 *
 * \param [in] mosq         : Pointer to instance of mosquitto.
 * \param [in] deleteDevice : Payload of device deletion request received from cloud.`
 *
 * \return : 0 on success , -1 on failure
 */
int deviceDeletionRequest(struct mosquitto *mosq, cJSON * deleteDevice) {

	int l_count = 0;
	int discoveredDeviceCount = 0;
	int status = 0;
	char * eui64DeleteDevice = NULL;
	char * nodeId = NULL;

	eui64DeleteDevice = cJSON_GetObjectItem(deleteDevice, "deviceUid")->valuestring;

	// Remove device from device joined list
	discoveredDeviceCount = cJSON_GetArraySize(root);

	for (l_count = 0 ; l_count < discoveredDeviceCount; l_count++) {

		cJSON * deviceObject = cJSON_GetArrayItem(root, l_count);
		cJSON * deviceEndpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");
		char * eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

		if (!strncmp(eui64DeleteDevice,&eui64String[2],strlen(eui64DeleteDevice))) {
			nodeId = cJSON_GetObjectItem(deviceObject, "nodeId")->valuestring;

			// No need to send unbind command as custom command is used to bind device
			/*
			status = zigbeeUnbindDevice(mosq, deviceObject);
			if ( status == -1){
				logBtGattErr ("Failed to unbind device : %s\n",nodeId);
				return -1;
			}
			*/
			status = zigbeeRemoveDevice(mosq, nodeId);
			if ( status == -1){
				logBtGattErr ("Failed to remove device : %s\n",nodeId);
				return -1;
			}
			cJSON_DeleteItemFromArray(root, l_count);
			printAllDeviceList();
			break;
		}
	}

	// Remove device from provisioned list
	discoveredDeviceCount = cJSON_GetArraySize(provisionedDeviceList);

	for (l_count = 0 ; l_count < discoveredDeviceCount; l_count++) {

		cJSON * deviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count);
		char * eui64String = cJSON_GetObjectItem(deviceObject, "deviceUid")->valuestring;

		if (!strncmp(eui64DeleteDevice,eui64String,strlen(eui64DeleteDevice))) {
			cJSON_DeleteItemFromArray(provisionedDeviceList, l_count);
			printProvisionedDeviceList();
			break;
		}
	}
	return status;
}

/* checkSeleneResponse()
 *
 * \brief : This callback function called when received any request
 *          and response from selene.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int checkSeleneResponse( struct mosquitto *mosq , const struct mosquitto_message * message)
{
	int status = 0;
	int l_count;
	bool deviceAlreadyProvisioned = false;
	char * typeOfResponse = NULL;
	char * result = NULL;
	char * ptr = NULL;
	char * eui64String = NULL;
	char command[256] = {0};
	char mqttTopic[64] = {0};
	cJSON * devices = NULL;
	struct timeval curTime;
	struct timeval tv;

	if ((mosq == NULL) || (message == NULL)){
		logBtGattErr ("Invalid pointer\n");
		return -1;
	}

	cJSON *node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}
	typeOfResponse = cJSON_GetObjectItem(node, "type")->valuestring;

	if (!strncmp(typeOfResponse, SELENE_DEVICE_LIST_RESPONSE, strlen(SELENE_DEVICE_LIST_RESPONSE))) {

		devices = cJSON_GetObjectItem(node, "device_list");
		ptr = cJSON_PrintUnformatted(devices);
		if (ptr == NULL ) {
			logBtGattErr ("Failed to parse json\n");
			cJSON_Delete(node);
			return -1;
		}
		provisionedDeviceList = cJSON_Parse(ptr);
		if ( provisionedDeviceList == NULL ) {
			logBtGattErr ("Failed to parse json\n");
			cJSON_Delete(node);
			free(ptr);
			return -1;
		}
		free(ptr);
		/* Update provision device list for device timestamp and status */
		for (l_count = 0; l_count < cJSON_GetArraySize(provisionedDeviceList); l_count++) {

			cJSON *provisionedDeviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count);
			cJSON_AddNumberToObject(provisionedDeviceObject, "timestamp", 0);
			cJSON_AddBoolToObject(provisionedDeviceObject, "status", true);
			cJSON_DetachItemFromObject(provisionedDeviceObject, "deviceName");
			cJSON_DetachItemFromObject(provisionedDeviceObject, "deviceType");
		}

		printProvisionedDeviceList();
		if ( root != NULL ) {
			removeJoinedDevices(mosq);
		} else {
			logBtGattInfo ("Device list is NULL.\n");
		}

		/* Growhouse server will receive device list response only if selene successfully connects to
		 * mqtt server so assume gateway is connected to network
		 */
		gatewayConnectivity = 1;
		cJSON_Delete(node);

		/* start device status monitoring */
		startDeviceMonitor();

	} else if (!strncmp(typeOfResponse, SELENE_DEVICE_DELETION_RESPONSE, strlen(SELENE_DEVICE_DELETION_RESPONSE))) {
		status = deviceDeletionRequest(mosq, node);
		cJSON_Delete(node);

	} else if (!strncmp(typeOfResponse, SELENE_GATEWAY_REGISTRATION_RESPONSE, strlen(SELENE_GATEWAY_REGISTRATION_RESPONSE))) {

		result = cJSON_GetObjectItem(node, "result")->valuestring;
		if (!strcmp(result, "true")) {
			gatewayConnectivity = 1;

			status = mosquitto_publish(mosq, NULL, MQTT_PUB_GATEWAY_CONNECT_RESPONSE, strlen(gatewayConnectivitySuccess), (char*)gatewayConnectivitySuccess, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_GATEWAY_CONNECT_RESPONSE, status);
			} else {
				logBtGattInfo ("Successfully publish gateway registration response payload \"%s\" on topic %s\n", (char *)gatewayConnectivitySuccess, MQTT_PUB_GATEWAY_CONNECT_RESPONSE);
			}
		} else {

			status = mosquitto_publish(mosq, NULL, MQTT_PUB_GATEWAY_CONNECT_RESPONSE, strlen(gatewayConnectivityError), (char*)gatewayConnectivityError, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_GATEWAY_CONNECT_RESPONSE, status);
			} else {
				logBtGattInfo ("Successfully publish gateway registration response payload \"%s\" on topic %s\n", (char *)gatewayConnectivityError, MQTT_PUB_GATEWAY_CONNECT_RESPONSE);
			}

		}
		cJSON_Delete(node);

		if ( provisionedDeviceList == NULL ) {
			// Request selene to provide already provisioned device list
			status = mosquitto_publish(mosq, NULL, MQTT_PUB_SELENE_REQUEST, strlen(deviceListCommand), deviceListCommand, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_SELENE_REQUEST, status);
				return -1;
			}
			logBtGattInfo ("Successfully publish payload to get deviceList from selene on topic %s\n", MQTT_PUB_SELENE_REQUEST);
		}

	} else if (!strncmp(typeOfResponse, SELENE_DEVICE_REGISTRATION_RESPONSE, strlen(SELENE_DEVICE_REGISTRATION_RESPONSE))) {

		result = cJSON_GetObjectItem(node, "result")->valuestring;

		/* If device registration failed then remove device from network */
		if (!strcmp(result, "false")) {
			logBtGattInfo("Get device registartion failed. Remove device from network and send failed to ble app\n");
			deviceDeletionRequest(mosq, node);
		} else {
			eui64String = cJSON_GetObjectItem(node, "deviceUid")->valuestring;
			gettimeofday(&tv, NULL);
			cJSON * provisionDevice = cJSON_CreateObject();
			cJSON_AddStringToObject(provisionDevice, "deviceUid", eui64String);
			cJSON_AddNumberToObject(provisionDevice, "timestamp", tv.tv_sec);
			cJSON_AddBoolToObject(provisionDevice, "status", true);

			/* Check if device is already provisioned or not. If device is already provisioned then do not add into provisioned
			   Device list */
			for (l_count = 0; l_count < cJSON_GetArraySize(provisionedDeviceList); l_count++) {

				cJSON *provisionedDeviceObject = cJSON_GetArrayItem(provisionedDeviceList, l_count);
				char * provisionedEui64String = cJSON_GetObjectItem(provisionedDeviceObject, "deviceUid")->valuestring;
				if (!strncmp (eui64String, provisionedEui64String, strlen(provisionedEui64String))) {
					deviceAlreadyProvisioned = true;
					break;
				}
			}
			/* Add device to provisioned device list*/
			if (!deviceAlreadyProvisioned)
				cJSON_AddItemToArray(provisionedDeviceList, provisionDevice);

			printProvisionedDeviceList();
		}
		status = mosquitto_publish(mosq, NULL, MQTT_PUB_DEVICE_CONNECT_RESPONSE, message->payloadlen, message->payload, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_DEVICE_CONNECT_RESPONSE, status);
		} else {
			logBtGattInfo ("Successfully publish device registration response payload \"%s\" on topic %s\n", (char *)message->payload, MQTT_PUB_DEVICE_CONNECT_RESPONSE);
		}
		cJSON_Delete(node);
		pthread_mutex_unlock(&provision_lock);

	} else if (!strncmp(typeOfResponse, SELENE_GATEWAY_DELETION_RESPONSE, strlen(SELENE_GATEWAY_DELETION_RESPONSE))) {

		result = cJSON_GetObjectItem(node, "result")->valuestring;
		if (!strcmp(result, "true")) {

			/* Set parameters to again default*/
			gatewayConnectivity = 0;
			if (root != NULL ) {
				cJSON_Delete(root);
				root = NULL;
			}

			if ( provisionedDeviceList != NULL ) {
				cJSON_Delete(provisionedDeviceList);
				provisionedDeviceList = NULL;
			}

			if ( deviceFirmwareVersionList != NULL) {
				cJSON_Delete(deviceFirmwareVersionList);
				deviceFirmwareVersionList = NULL;
			}

			if ( deviceUpdateRequestList != NULL) {
				cJSON_Delete(deviceUpdateRequestList);
				deviceUpdateRequestList = NULL;
			}

			/* Send command to zigbee coordinator to get device list*/
			snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"plugin device-table print\",\"postDelayMs\":100}]}");
			snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
			status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			} else {
				logBtGattInfo ("Successfully publish command to get device list from zigbee coordinator\n");
			}

			/* Send command to ble-server to get disconnect */
			status = mosquitto_publish(mosq, NULL, MQTT_PUB_GATEWAY_DISCONNECT, strlen("disconnect"), "disconnect", 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			} else {
				logBtGattInfo ("Successfully publish command to disconnect ble\n");
			}

			/* Remove selene properties file*/
			if( access(SELENE_PROPERTIES_FILE, F_OK ) != -1 )
				remove(SELENE_PROPERTIES_FILE);

		} else {
			logBtGattErr ("Failed to delete gateway\n");
		}
		cJSON_Delete(node);
	}

	return status;
}

/* provisionDeviceThread()
 *
 * \brief : this thread will receive the list of devices to be provision.
 *          If device needs to provision then register device on arrow connect
 *          through selene otherwise remove device from network.

 * \param [in] argv : Pointer to instance of mosquitto message.
 *
 */
void * provisionDeviceThread(void * argv) {

	int l_count,l_count1,l_count2,l_count3;
	int status = 0;
	int newDeviceCount = 0;
	int discoveredDeviceCount = 0;
	bool alreadyProvisioned = false;
	char * deviceName = NULL;
	char mqttTopic[64] = {0};
	char command[256] = {0};
	cJSON * ledconfiguration = NULL;
	char ledChannelConfiguration[6] = {0};
	char * ledconfig = NULL;
	char * ptr = NULL;
	struct timespec timeoutTime;

	struct mosquitto_message * message = (struct mosquitto_message *) argv;

	if ((mosq == NULL) || (message == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return NULL;
	}

	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);

	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(closeNetworkCommand), closeNetworkCommand, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		free(message);
		return NULL;
	}
	logBtGattInfo ("Successfully publish Discover devices payload \"%s\" on topic %s\n", closeNetworkCommand, mqttTopic);

	cJSON *newDevices = cJSON_Parse(message->payload);
	if (newDevices == NULL) {
		logBtGattErr ("Failed to parse json\n");
		free(message);
		return NULL;
	}

	logBtGattInfo ("Pthread mutex lock\n");
	pthread_mutex_lock(&lock);

	newDeviceCount = cJSON_GetArraySize(newDevices);
	discoveredDeviceCount = cJSON_GetArraySize(root);

	// compare Provisioned device list with all joined device list
	for (l_count = 0 ; l_count < newDeviceCount ; l_count++) {

		cJSON *newDeviceObject = cJSON_GetArrayItem(newDevices, l_count);
		char * newEui64String = cJSON_GetObjectItem(newDeviceObject, "eui64")->valuestring;
		char * deviceType = cJSON_GetObjectItem(newDeviceObject, "deviceType")->valuestring;
		int decision = cJSON_GetObjectItem(newDeviceObject, "decision")->valueint;
		if (cJSON_GetObjectItem(newDeviceObject, "deviceName"))
			deviceName = cJSON_GetObjectItem(newDeviceObject, "deviceName")->valuestring;

		for (l_count1 = 0 ; l_count1 < discoveredDeviceCount; l_count1++) {

			cJSON * deviceObject = cJSON_GetArrayItem(root, l_count1);
			cJSON * deviceEndpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");
			char * eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;
			char * nodeId = cJSON_GetObjectItem(deviceObject, "nodeId")->valuestring;

			if (!strncmp(newEui64String,&eui64String[2],strlen(newEui64String))) {

				if (decision == true) {

					clock_gettime(CLOCK_REALTIME, &timeoutTime);
					timeoutTime.tv_sec += 10;
					/* Wait for maximum upto 10 sec for next device to be provisioned on arrow connect */
					pthread_mutex_timedlock(&provision_lock, &timeoutTime);

					cJSON_AddStringToObject (deviceObject, "deviceName", deviceName);
					if (!strcmp(deviceType, "LedNode")) {
						ledconfiguration = cJSON_GetObjectItem(newDeviceObject, "ledconfiguration");
						for (l_count2 = LED_CHANNEL_1 ; l_count2 < LED_CHANNEL_MAX ; l_count2++) {
							ledconfig = cJSON_GetObjectItem(ledconfiguration, led_state_control_array[l_count2])->valuestring;
							cJSON_AddStringToObject(deviceObject,led_state_control_array[l_count2],ledconfig);
							for ( l_count3 = 0 ;  l_count3 < sizeof(led_channel_types)/sizeof(LED_CHANNEL_TYPES) ; l_count3++) {
								if (!strncmp(ledconfig, led_channel_types[l_count3].type, strlen(ledconfig))) {

									ledChannelConfiguration[l_count2 - 1] = led_channel_types[l_count3].key;
									break;
								}
							}
						}
					}

					status = onNodeJoin(mosq, deviceObject);
					if (status == -1) {
						free(message);
						cJSON_Delete(newDevices);
						logBtGattInfo ("Pthread mutex unlock\n");
						pthread_mutex_unlock(&lock);
						return NULL;
					}
					// register device
					ptr = cJSON_PrintUnformatted(deviceObject);
					if (ptr == NULL ) {
						logBtGattErr ("Failed to parse json\n");
						free(message);
						cJSON_Delete(newDevices);
						pthread_mutex_unlock(&lock);
						return NULL;
					}
					status = registerDevice(mosq, ptr);
					if (status == -1) {
						free(message);
						cJSON_Delete(newDevices);
						free(ptr);
						logBtGattInfo ("Pthread mutex unlock\n");
						pthread_mutex_unlock(&lock);
						return NULL;
					}
					free(ptr);


					if (!strcmp(deviceType, "LedNode")) {

						snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom set-led-channel {%s} %x %x %x %x %x %x\",\"postDelayMs\":100}]}", newEui64String, ledChannelConfiguration[0], ledChannelConfiguration[1], ledChannelConfiguration[2],							ledChannelConfiguration[3], ledChannelConfiguration[4], ledChannelConfiguration[5]);

						snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
						status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
						if ( status != MOSQ_ERR_SUCCESS){
							logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
							free(message);
							cJSON_Delete(newDevices);
							logBtGattInfo ("Pthread mutex unlock\n");
							pthread_mutex_unlock(&lock);
							return NULL;
						}
						logBtGattInfo ("Successfully publish lednode configuration to zigbee coordinator : %s\n", command);

						/* Send command to get LedNode channel Intensity */
						memset(command, 0x0, sizeof(command));
						snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom get-led-intensity {%s}\",\"postDelayMs\":100}]}", newEui64String);
						status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
						if ( status != MOSQ_ERR_SUCCESS){
							logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
							free(message);
							cJSON_Delete(newDevices);
							logBtGattInfo ("Pthread mutex unlock\n");
							pthread_mutex_unlock(&lock);
							return NULL;
						}
						logBtGattInfo ("Successfully publish command to get LedNode channel Intensity : %s\n", command);
					}

				} else {
					/* If device is already provisioned and if received payload from growhouse mobile app
					   to remove the device then remove the device from zigbee network */
					//	alreadyProvisioned = isDeviceProvisioned(deviceObject);
					if (!alreadyProvisioned) {
						status = zigbeeRemoveDevice(mosq, nodeId);
						cJSON_DeleteItemFromArray(root, l_count1);
						printAllDeviceList();
					} else {
						logBtGattInfo ("Device is already provisioned eui64 == %s\n",eui64String);
					}
				}
				break;
			}
		}
	}


	free(message);
	cJSON_Delete(newDevices);
	logBtGattInfo ("Pthread mutex unlock\n");
	pthread_mutex_unlock(&lock);

	/* After provisioning remove all other devices from network */
	clock_gettime(CLOCK_REALTIME, &timeoutTime);
	timeoutTime.tv_sec += 10;
	pthread_mutex_timedlock(&provision_lock, &timeoutTime);
	status = removeJoinedDevices(mosq);
	if (status != 0) {
		logBtGattErr("Failed to remove devices\n");
	}

	pthread_mutex_unlock(&provision_lock);

	/* Provisioning is done */
	provisionInprogress = 0;
}

/* provisionDevice()
 *
 * \brief : this routine will receive list of devices to be register
 *          and create thread to provision devices.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int provisionDevice(struct mosquitto * mosq, const struct mosquitto_message *message) {

	int ret;
	pthread_t id;

	struct mosquitto_message * devicePayload = (struct mosquitto_message *)malloc(sizeof(struct mosquitto_message));
	if (devicePayload == NULL) {
		logBtGattErr("Failed to allocate memory\n");
		return -1;
	}
	mosquitto_message_copy(devicePayload, message);

	/* Create thread to provision devices */
	ret = pthread_create(&id, NULL, &provisionDeviceThread, (void *)devicePayload);
	if ( ret != 0) {
		logBtGattErr("Failed to create provision device thread\n");
		removeJoinedDevices(mosq);
		free(devicePayload);
		return -1;
	}
	return 0;
}

/* newDeviceJoinedRequest()
 *
 * \brief : this routine will check if device is already
 *          provisioned or not.If not then send to ble App
 *          If yes then bind device and register on cloud.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int newDeviceJoinedRequest(struct mosquitto *mosq, const struct mosquitto_message *message) {

	int deviceType = 0;
	int l_count = 0;
	bool deviceAlreadyJoined = 0;
	bool alreadyProvisioned = 0;
	char * eui64String = NULL;
	char * deviceTypeStr = NULL;
	int status = MOSQ_ERR_INVAL;
	char mqttTopic[64] = {0};
	char command[256] = {0};
	char * ptr = NULL;

	if ((mosq == NULL) || ( message == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}
	cJSON *node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	// Mutex lock for adding devices in joined device list
	logBtGattInfo ("Pthread mutex lock\n");
	pthread_mutex_lock(&lock);

	/* check if device is already present or not
	 * to avoid multiple joined request of same device
	 * */
	deviceAlreadyJoined = checkIfDeviceIsAlreadyJoined(node);
	// Add object to root array object
	if (!deviceAlreadyJoined)
		cJSON_AddItemToArray(root,node);

	logBtGattInfo ("Pthread mutex unlock\n");
	pthread_mutex_unlock(&lock);

	printAllDeviceList();

	// check device is already provisioned or not
	alreadyProvisioned = isDeviceProvisioned(node);
	cJSON *deviceEndpoint = cJSON_GetObjectItem(node, "deviceEndpoint");
	eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;

	deviceTypeStr = cJSON_GetObjectItem(node, "deviceType")->valuestring;
	sscanf (&deviceTypeStr[2],"%x",&deviceType);

	/* Send command to end-device to get firmware version */
	memset(command, 0x0, sizeof(command));
	snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom get-end-node-firmware {%s}\",\"postDelayMs\":100}]}", &eui64String[2]);
	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
	} else {
		logBtGattInfo ("Successfully publish command to get endnode firmware : %s\n", command);
	}

	if ( deviceType == DEVICE_ID_DIMMABLE_LIGHT ) {
		/* Send command to end-device to get channel configuration */
		memset(command, 0x0, sizeof(command));
		snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom get-led-configuration {%s}\",\"postDelayMs\":100}]}", &eui64String[2]);
		snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
		status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		} else {
			logBtGattInfo ("Successfully publish command to get LedNode channel Configuration : %s\n", command);
		}
		return 0;
	}

	// Get Device type name
	for ( l_count = 0 ; l_count < sizeof(deviceTypeArray)/sizeof(DEVICETYPE) ; l_count++) {

		if ( deviceType == deviceTypeArray[l_count].deviceTypeId ) {
			break;
		}
	}
	// create object to send data over ble
	cJSON * bleNode = cJSON_CreateObject();
	cJSON_AddStringToObject(bleNode , "eui64", &eui64String[2]);
	cJSON_AddStringToObject(bleNode , "deviceType", deviceTypeArray[l_count].deviceTypeName);

	ptr = cJSON_PrintUnformatted(bleNode);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(bleNode);
		return -1;
	}
	// Publsh from gateway to ble server
	status = mosquitto_publish(mosq, NULL, MQTT_PUB_DISCOVERED_DEVICE, strlen(ptr), ptr , 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_DISCOVERED_DEVICE, status);
		cJSON_Delete(bleNode);
		free(ptr);
		return -1;
	}
	logBtGattInfo ("Successfully publish device payload \"%s\" to ble-server on topic %s\n", ptr, MQTT_PUB_DISCOVERED_DEVICE);
	cJSON_Delete(bleNode);
	free(ptr);

	return 0;
}

/* onDeviceListReceived()
 *
 * \brief : this routine will get previously connected device list
 *          from zigbee coordinator.
 *
 * \param [in] payload : payload received from zigbee coordinator.
 *
 * \return : 0 on success , -1 on failure
 */
int onDeviceListReceived(struct mosquitto *mosq, const struct mosquitto_message * message) {

	int status = 0;
	int discoveredDeviceCount = 0;
	int l_count;
	int deviceType = 0;
	char * deviceTypeStr = NULL;
	char * eui64String = NULL;
	char mqttTopic[64] = {0};
	char command[256] = {0};
	char * ptr = NULL;
	cJSON * devices = NULL;

	if ((mosq == NULL) || (message == NULL)) {
		logBtGattErr ("Invalid pointer\n");
		return -1;
	}
	cJSON * node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}

	if ( root != NULL) {
		cJSON_Delete(root);
	}

	devices = cJSON_GetObjectItem(node, "devices");
	ptr = cJSON_PrintUnformatted(devices);
	if (ptr == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(node);
		return -1;
	}
	root = cJSON_Parse(ptr);
	if ( root == NULL ) {
		logBtGattErr ("Failed to parse json\n");
		cJSON_Delete(node);
		free(ptr);
		return -1;
	}
	free(ptr);

	discoveredDeviceCount = cJSON_GetArraySize(root);

	for (l_count = 0 ; l_count < discoveredDeviceCount; l_count++) {

		cJSON * deviceObject = cJSON_GetArrayItem(root, l_count);

		/* Get End-device firmware verison on startup */
		cJSON *deviceendpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");
		eui64String = cJSON_GetObjectItem(deviceendpoint, "eui64")->valuestring;

		deviceTypeStr = cJSON_GetObjectItem(deviceObject, "deviceType")->valuestring;
		sscanf (&deviceTypeStr[2],"%x",&deviceType);

		memset(command, 0x0, sizeof(command));
		snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom get-end-node-firmware {%s}\",\"postDelayMs\":100}]}", &eui64String[2]);
		snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
		status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
		} else {
			logBtGattInfo ("Successfully publish command to get endnode firmware : %s\n", command);
		}

		if ( deviceType == DEVICE_ID_DIMMABLE_LIGHT ) {
			/* Send command to get LedNode channel Intensity */
			memset(command, 0x0, sizeof(command));
			snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"custom get-led-intensity {%s}\",\"postDelayMs\":100}]}", &eui64String[2]);
			status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			} else {
				logBtGattInfo ("Successfully publish command to get LedNode channel Intensity : %s\n", command);
			}
		}
		status = createDeviceTemplate(deviceObject);
		if (status == -1) {
			cJSON_Delete(node);
			return -1;
		}
	}
	if ( provisionedDeviceList != NULL ) {
		status = removeJoinedDevices(mosq);
	} else {
		logBtGattInfo ("Provision device list is NULL\n");
	}

	cJSON_Delete(node);
	printAllDeviceList();
	return status;
}

/* discoverDevices()
 *
 * \brief : this routine will open or close zigbee coordinator network
 *          according to payload received from mobile app.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : 0 on success , -1 on failure
 */
int discoverDevices(struct mosquitto * mosq, const struct mosquitto_message *message){

	int enable = 0;
	int status = MOSQ_ERR_INVAL;
	char mqttTopic[32] = {0};

	if ((mosq == NULL) || (message == NULL)){
		logBtGattErr ("Invalid pointers\n");
		return -1;
	}

	/* If firmware update is in progress then restrict to provision device */
	if ((LednodeUpdateCount != 0 ) || (SoilnodeUpdateCount != 0)) {
		logBtGattInfo("Firmware update is in progress.\n");
		return -1;
	}

	cJSON * node = cJSON_Parse(message->payload);
	if (node == NULL) {
		logBtGattErr ("Failed to parse json\n");
		return -1;
	}
	enable = cJSON_GetObjectItem(node, "discoverDevices")->valueint;

	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	if (enable){
		if (gatewayConnectivity) {
			status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(openNetworkCommand), openNetworkCommand, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
				cJSON_Delete(node);
				return -1;
			}
			logBtGattInfo ("Successfully publish Discover devices payload \"%s\" on topic %s\n",openNetworkCommand, mqttTopic);

			/* Provisioning is in progres */
			provisionInprogress = 1;

		} else {

			status = mosquitto_publish(mosq, NULL, MQTT_PUB_GATEWAY_CONNECT_RESPONSE, strlen(gatewayConnectivityError), gatewayConnectivityError, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_GATEWAY_CONNECT_RESPONSE, status);
			} else {
				logBtGattInfo ("Successfully publish gateway connectivity error payload \"%s\" on topic %s\n", (char *)gatewayConnectivityError, MQTT_PUB_GATEWAY_CONNECT_RESPONSE);
			}
		}
	}
	else{

		/* Provisioning is done */
		provisionInprogress = 0;

		status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(closeNetworkCommand), closeNetworkCommand, 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
			cJSON_Delete(node);
			return -1;
		}
		logBtGattInfo ("Successfully publish Discover devices payload \"%s\" on topic %s\n", closeNetworkCommand, mqttTopic);

	}
	cJSON_Delete(node);
	return 0;
}

/* message_callback()
 *
 * \brief : this routine callback function called when received any message on
 *          subscribed mqtt topic.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 * \param [in] obj     : Pointer to user data provided in mosquitto_new.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return none
 */
void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	int l_count;
	int status = 0;
	mosquitto_topic_matches_sub(MQTT_SUB_HEARTBEAT, message->topic, &match);
	if (!match) {
		logBtGattInfo("\nMqtt Receive : %d message : %s for topic '%s'\n", message->payloadlen, (char*) message->payload, message->topic);
	}

	for ( l_count = 0; l_count < NUMBER_OF_FUNCTION ; l_count++) {

		mosquitto_topic_matches_sub((const char *) mosquitto_message_handler_array[l_count].mosquitto_topic, message->topic, &match);
		if (match) {
			status = mosquitto_message_handler_array[l_count].handler(mosq,message);
			if ( status == -1)
			{
				logBtGattErr ("Failed to handle message on topic :: %s\n", message->topic);
			}
			break;
		}
	}
}

/* factory_button_longPress_detected()
 *
 * \brief : this routine will remove all devices from zigbee network.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 *
 * \return none
 */
void factory_button_longPress_detected(struct mosquitto *mosq)
{
	int l_count;
	int discoveredDeviceCount = 0;
	int status = 0;
	char * eui64String = NULL;
	char * nodeId = NULL;
	char command[256] = {0};
	char mqttTopic[32] = {0};
	int childpid = 0;
	cJSON * deviceObject = NULL;
	cJSON * deviceEndpoint = NULL;
	char* arguments[] = { "sh", "/etc/factoryreset/factoryreset.sh", NULL };

	if (mosq == NULL){
		logBtGattErr ("Invalid pointers\n");
		return;
	}

	logBtGattInfo("Factory reset gateway.");
	pthread_mutex_lock(&lock);
	logBtGattInfo ("Pthread mutex lock\n");

	printAllDeviceList();
	discoveredDeviceCount = cJSON_GetArraySize(root);

	for (l_count = 0 ; l_count < discoveredDeviceCount ; l_count++) {

		deviceObject = cJSON_GetArrayItem(root, l_count);
		deviceEndpoint = cJSON_GetObjectItem(deviceObject, "deviceEndpoint");
		eui64String = cJSON_GetObjectItem(deviceEndpoint, "eui64")->valuestring;
		nodeId = cJSON_GetObjectItem(deviceObject, "nodeId")->valuestring;

		/* Unbind cluster of end device */
		/*
		// No need to send unbind command as custom command is used to bind device
		status = zigbeeUnbindDevice(mosq, deviceObject);
		if ( status == -1){
			logBtGattErr ("Failed to unbind device : %s\n",nodeId);
			logBtGattInfo ("Pthread mutex unlock\n");
			pthread_mutex_unlock(&lock);
			return;
		}
		*/
		/* Remove devices from zigbee Network */
		status = zigbeeRemoveDevice(mosq, nodeId);
		if ( status == -1){
			logBtGattErr ("Failed to remove device : %s\n",nodeId);
			logBtGattInfo ("Pthread mutex unlock\n");
			pthread_mutex_unlock(&lock);
			return;
		}

		/* Remove device from device joined list */
		cJSON_DeleteItemFromArray(root, l_count);
		l_count--;
		discoveredDeviceCount--;
	}
	printAllDeviceList();

	/* Clean network leave for zigbee coordinator */
	snprintf (command, sizeof(command), "{\"commands\":[{\"command\": \"network leave\",\"postDelayMs\":100}]}");
	snprintf (mqttTopic, sizeof(mqttTopic), "gw/%s/commands",gatewayEui64);
	status = mosquitto_publish(mosq, NULL, mqttTopic, strlen(command), command, 1, false);
	if ( status != MOSQ_ERR_SUCCESS){
		logBtGattErr("could not publish to topic:%s err:%d", mqttTopic, status);
	} else {
		logBtGattInfo ("Successfully publish \"network leave\" command zigbee coordinator\n");
	}

	/* Cleanup memory */
	growhouseCleanup(mosq);

	if((childpid = fork()) == -1 )
	{
		logBtGattErr("Failed to fork");
		return;
	}
	else if(childpid == 0)
	{
		execv("/bin/sh", arguments);
		exit(0);
	}

	pthread_mutex_unlock(&lock);
	logBtGattInfo ("Pthread mutex unlock\n");
	return;
}

/* checkIfGatewayServiceisUpdated()
 *
 * \brief : this function check if gateway service update or not.
 *          If updated then give response to selene.
 *
 * \param [in] mosq    : Pointer to instance of mosquitto.
 *
 * \return 0 on success, -1 on failure
 */
int checkIfGatewayServiceisUpdated(struct mosquitto *mosq) {

	int oldVersion = 0;
	char c[2];
	char command[256] = {0};
	FILE *fptr = NULL;
	FILE * fp = NULL;

	if( access(FIRMWARE_VERSION_FILE, F_OK ) != -1 ) {
		if ((fptr = fopen(FIRMWARE_VERSION_FILE, "r")) == NULL)
		{
			logBtGattErr ("Failed to open file\n");
			return -1;
		} else {
			// reads text until newline
			fscanf(fptr,"%[^\n]", c);
			oldVersion = atoi(c);
			logBtGattInfo (" oldverison = %d\n", oldVersion);
			fclose(fptr);
		}
	}
	if ( oldVersion != GROWHOUSE_SERVICE_VERSION ) {
		if ((fptr = fopen(FIRMWARE_VERSION_FILE, "w+")) == NULL)
		{
			logBtGattErr ("Failed to open file\n");
			return -1;
		} else {
			fprintf (fptr, "%d", GROWHOUSE_SERVICE_VERSION);
			fclose(fptr);

			/* Update mqtt properties with latest firmware version */
			snprintf (command, sizeof(command), "sed -i 's/softwareVersion.*/softwareVersion=%d.0.0/g' %s", GROWHOUSE_SERVICE_VERSION, MQTT_PROPERTIES_FILE);
			if( fp = popen(command, "w"))
			{
				logBtGattInfo ("Successfully update MQTT properties file\n");
				pclose(fp);
			} else {
				logBtGattErr("Failed to update mqtt properties file");
			}
		}
		sendServiceUpdateResponseToSelene(mosq, "success", "Services upgraded successfully");
	}
	return 0;
}


/*
 * Growhouse Server
 * main routine
 */
int main(int argc, char *argv[])
{
	char clientid[24];
	int rc = 0;
	pthread_t id = 0;

	logBtGattInfo("Growhouse server application started with version : %d\n", GROWHOUSE_SERVICE_VERSION);

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		logBtGattErr("Mutex init has failed\n");
		return -1;
	}

	if (pthread_mutex_init(&provision_lock, NULL) != 0)
	{
		logBtGattErr("Mutex init has failed\n");
		return -1;
	}

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "GrowhouseServer_%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);
	if(!mosq){
		mosquitto_lib_cleanup();
		logBtGattErr ("failed to start mosquitto\n");
		return -1;
	}

	/* Thread to handle Factory reset button */
	rc = pthread_create(&id,  NULL, &monitorFactoryReset, mosq);
	if(rc != 0)
	{
		logBtGattErr("Failed to create Factory reset monitor thread");
	}
	/* Thread created */

	if(mosq){

		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

		if (mosquitto_connect(mosq, mqtt_host, mqtt_port, MOSQUITTO_PING_TIMEOUT)){
			mosquitto_lib_cleanup();
			logBtGattErr("Unable to connect Mosquitto.\n");
			return 1;
		}

		/* Check if firmware is updated then send response to selene */
		if ( (checkIfGatewayServiceisUpdated(mosq)) == -1 ) {
			logBtGattErr ("Failed to check firmware version");
			return -1;
		}

		/*TODO : Add support for gracefully shoutdown appllication*/
		while(1){
			rc = mosquitto_loop(mosq, -1, 1);
			if(rc){
				logBtGattErr("connection error!\n");
				sleep(10);
				mosquitto_reconnect(mosq);
			}
		}
		mosquitto_disconnect(mosq);
		mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();

	return rc;
}

/************************* End functions **************************************/

/*
 * End growhouse-server.c
 */
