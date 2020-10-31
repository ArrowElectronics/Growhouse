/*
 * btgatt-server.c
 * Copyright(c) 2018
 */

/*
 * TODO: ble passphrase based
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <mosquitto.h>
#include <cJSON.h>
#include <syslog.h>
#include <signal.h>

#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"

#include "src/shared/mainloop.h"
#include "src/shared/util.h"
#include "src/shared/att.h"
#include "src/shared/queue.h"
#include "src/shared/timeout.h"
#include "src/shared/gatt-db.h"
#include "src/shared/gatt-server.h"


/* Define ble-server version */
#define BLE_SERVER_SERVICE_VERSION	6

#define UUID_GAP			0x1800
#define UUID_GATT			0x1801
#define UUID_GATEWAY_PROVISION		0x3A25
#define UUID_GATEWAY_ACCOUNT		0x3A26
#define UUID_GATEWAY_WIFI_MAC		0x3A27
#define UUID_GATEWAY_FW_VERSION		0X3A28
#define UUID_GATEWAY_CONNECTIVITY	0x3A29
#define UUID_DEVICE_PROVISION		0x3A35
#define UUID_DEVICE_DISCOVER_COMMAND	0x3A36
#define UUID_DEVICE_DISCOVERED_LIST	0x3A37
#define UUID_DEVICE_CONNECTIVITY	0x3A38
#define ATT_CID 4
#define LE_LINK 0x80

#define PRLOG(...) \
	do { \
		printf(__VA_ARGS__); \
	} while (0)

#define COLOR_OFF	"\x1B[0m"
#define COLOR_RED	"\x1B[0;91m"
#define COLOR_GREEN	"\x1B[0;92m"
#define COLOR_YELLOW	"\x1B[0;93m"
#define COLOR_BLUE	"\x1B[0;94m"
#define COLOR_MAGENTA	"\x1B[0;95m"
#define COLOR_BOLDGRAY	"\x1B[1;30m"
#define COLOR_BOLDWHITE	"\x1B[1;37m"

#define MQTT_PUB_DISCOVER_COMMAND       	"ble/gw/discoverCommand"
#define MQTT_PUB_PROVISIONED_DEVICE     	"ble/gw/provisionedDevice"
#define MQTT_PUB_BLE_SHUTTING_DOWN		"ble/gw/shuttingDown"
#define MQTT_SUB_DISCOVERED_DEVICE      	"gw/ble/discoveredDevice"
#define MQTT_SUB_GATEWAY_CONNECT_RESPONSE	"gw/ble/gatewayConnectivity"
#define MQTT_SUB_DEVICE_CONNECT_RESPONSE	"gw/ble/deviceConnectivity"
#define MQTT_SUB_GATEWAY_DISCONNECT		"gw/ble/gatewayDisconnect"

#define BEGIN_BLE_DATA				"---BEGIN BLE DATA---"
#define END_BLE_DATA				"----END BLE DATA----"

#define CMD_CHUNK_SIZE				4096
#define MAX_BLE_DATA_LEN			20

#define mqtt_host				"localhost"
#define mqtt_port				1883
/*
 * Default mosquitto.conf file sets the keepalive seconds
 * to be 60, so we set the keepailve interval of 120 seconds
 */
#define MOSQUITTO_PING_TIMEOUT			120

#define PROP_DEVICECLASS 			"deviceClass"
#define PROP_NAME 				"name"
#define PROP_UID 				"uid"
#define PROP_APIKEY 				"apiKey"
#define PROP_SECRETKEY				"secretKey"
#define PROP_IOTCONNECT_URL 			"iotConnectUrl"
#define PROP_IOTCONNECT_MQTT 			"iotConnectMqtt"
#define PROP_ENABLED 				"enabled"
#define PROP_CLOUD_TXFR_MODE 			"cloudTransferMode"
#define PROP_CLOUD_BATCH_SENDING_INTERVAL_MS 	"cloudBatchSendingIntervalMs"
#define PROP_HEARTBEAT_INTERVAL_MS 		"heartBeatIntervalMs"
#define PROP_IOTCONNECT_MQTT_VHOST 		"iotConnectMqttVHost"
#define PROP_HID 				"hid"
#define PROP_GROWHOUSE_URL			"growhouseUrl"
#define SELF_PROP_FILENAME 			"/opt/selene/config/devices/self.properties"
#define SELENE_VERSION_FILE			"/opt/selene/selene_version"

/* Error code */
#define BLE_WRITE_ERROR                    1

/* Mutex lock for sendind data over BLE */
pthread_mutex_t lock;
pthread_mutex_t rssi_lock;

/* enum for selene properties*/
enum self_properties_type {
	PROP_TYPE_MIN,
	PROP_TYPE_DEVICECLASS = PROP_TYPE_MIN,
	PROP_TYPE_NAME,
	PROP_TYPE_UID,
	PROP_TYPE_APIKEY,
	PROP_TYPE_SECRETKEY,
	PROP_TYPE_IOTCONNECT_URL,
	PROP_TYPE_IOTCONNECT_MQTT,
	PROP_TYPE_ENABLED,
	PROP_TYPE_CLOUD_TXFR_MODE,
	PROP_TYPE_CLOUD_BATCH_SENDING_INTERVAL_MS,
	PROP_TYPE_HEARTBEAT_INTERVAL_MS,
	PROP_TYPE_IOTCONNECT_MQTT_VHOST,
	PROP_TYPE_HID,
	PROP_TYPE_GROWHOUSE_URL,
	PROP_TYPE_MAX
};

const char * self_properties[] = {
	[PROP_TYPE_DEVICECLASS] = PROP_DEVICECLASS,
	[PROP_TYPE_NAME] = PROP_NAME,
	[PROP_TYPE_UID] = PROP_UID,
	[PROP_TYPE_APIKEY] = PROP_APIKEY,
	[PROP_TYPE_SECRETKEY] = PROP_SECRETKEY,
	[PROP_TYPE_IOTCONNECT_URL] = PROP_IOTCONNECT_URL,
	[PROP_TYPE_IOTCONNECT_MQTT] = PROP_IOTCONNECT_MQTT,
	[PROP_TYPE_ENABLED] = PROP_ENABLED,
	[PROP_TYPE_CLOUD_TXFR_MODE] = PROP_CLOUD_TXFR_MODE,
	[PROP_TYPE_CLOUD_BATCH_SENDING_INTERVAL_MS] = PROP_CLOUD_BATCH_SENDING_INTERVAL_MS,
	[PROP_TYPE_HEARTBEAT_INTERVAL_MS] = PROP_HEARTBEAT_INTERVAL_MS,
	[PROP_TYPE_IOTCONNECT_MQTT_VHOST] = PROP_IOTCONNECT_MQTT_VHOST,
	[PROP_TYPE_HID] = PROP_HID,
	[PROP_TYPE_GROWHOUSE_URL] = PROP_GROWHOUSE_URL
};

/******************** logging macros **************************************/
#if 1
#ifdef DEBUG_BT_GATT_SERVER
#define logBtGattVerbose(fmt, args...) syslog(LOG_ERR, "%s: " fmt, __func__, ##args)
#else
#define logBtGattVerbose(fmt, args...) do {} while(0)
#endif

#define logBtGattInfo(fmt, args...) syslog(LOG_INFO, "%s: " fmt, __func__, ##args)
#define logBtGattErr(fmt, args...) syslog(LOG_ERR, "%s: " fmt, __func__, ##args)
#define logBtGattErrUsingErrNo(fmt, args...) syslog(LOG_ERR, "%s: " fmt ": %s", __func__, ##args, strerror(errno))
#else
#if 0
#define logBtGattVerbose(fmt, args...) printf("%s: " fmt, __func__, ##args)
#else
#define logBtGattVerbose(fmt, args...) do {} while(0)
#endif

#define logBtGattInfo(fmt, args...) printf("%s: " fmt, __func__, ##args)
#define logBtGattErr(fmt, args...) printf("%s: " fmt, __func__, ##args)
#define logBtGattErrUsingErrNo(fmt, args...) printf("%s: " fmt ": %s", __func__, ##args, strerror(errno))
#endif

static const char test_device_name[] = "Very Long Test Device Name For Testing "
					"ATT Protocol Operations On GATT Server";
static bool verbose = false;
bool mainloop_exit = false;
static int min_rssi_count = 0;

struct server {
	int fd;
	struct bt_att *att;
	struct gatt_db *db;
	struct bt_gatt_server *gatt;

	uint8_t *device_name;
	size_t name_len;

	uint16_t gatt_svc_chngd_handle;
	bool svc_chngd_enabled;

	/* gateway specific */
	uint16_t gateway_handle;
	uint16_t gateway_account_handle;
	uint16_t gateway_connectivity_handle;
	uint16_t gateway_wifi_handle;

	/* device specific */
	uint16_t device_handle;
	uint16_t device_discover_handle;
	uint16_t device_list_handle;
	uint16_t device_connectivity_handle;

	/* mosquitto specific */
	struct mosquitto *mosq;

	/* message to be processed */
	unsigned int msg_len;
	char * msg_buf;

};

static void att_disconnect_cb(int err, void *user_data)
{
	logBtGattInfo ("Device disconnected: %s\n", strerror(err));

	mainloop_quit();
}

static void att_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG(COLOR_BOLDGRAY "%s" COLOR_BOLDWHITE "%s\n" COLOR_OFF, prefix,
			str);
}

static void gatt_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG(COLOR_GREEN "%s%s\n" COLOR_OFF, prefix, str);
}


static void gatt_service_changed_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	PRLOG("Service Changed Read called\n");

	gatt_db_attribute_read_result(attrib, id, 0, NULL, 0);
}

static void gatt_svc_chngd_ccc_read_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	struct server *server = user_data;
	uint8_t value[2];

	PRLOG("Service Changed CCC Read called\n");

	value[0] = server->svc_chngd_enabled ? 0x02 : 0x00;
	value[1] = 0x00;

	gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

static void gatt_svc_chngd_ccc_write_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		const uint8_t *value, size_t len,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	struct server *server = user_data;
	uint8_t ecode = 0;

	PRLOG("Service Changed CCC Write called\n");

	if (!value || len != 2) {
		ecode = BT_ATT_ERROR_INVALID_ATTRIBUTE_VALUE_LEN;
		goto done;
	}

	if (offset) {
		ecode = BT_ATT_ERROR_INVALID_OFFSET;
		goto done;
	}

	if (value[0] == 0x00)
		server->svc_chngd_enabled = false;
	else if (value[0] == 0x02)
		server->svc_chngd_enabled = true;
	else
		ecode = 0x80;

	PRLOG("Service Changed Enabled: %s\n",
			server->svc_chngd_enabled ? "true" : "false");

done:
	gatt_db_attribute_write_result(attrib, id, ecode);
}

/* device_provision_list_write_cb()
 *
 * \brief : this routine collects list of devices to be provision and publish
 *          to growhouse server.
 *
 * \param [in] attrib    : Pointer to the gatt attribute structure.
 * \param [in] offset    : Offset of data to be received.
 * \param [in] value     : Pointer to the value received from gatt client.
 * \param [in] len       : Length of data received.
 * \param [in] opcode    : Opration to be done.
 * \param [in] att       : Pointer to bt attribute structure.
 * \param [in] user_data : Pointer to user data passed.
 *
 * \return : none
 */
static void device_provision_list_write_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		const uint8_t *value, size_t len,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	struct server * server = user_data;
	uint8_t	ecode = 0;
	int status = MOSQ_ERR_INVAL;

	logBtGattInfo (" Provision device list callback\n");

	pthread_mutex_lock(&rssi_lock);
	if (value) {
		logBtGattInfo ("Received data len %lu :: %s\n", len, value);
		if (!strncmp((char *)value, BEGIN_BLE_DATA, strlen((char *)BEGIN_BLE_DATA))) {
			logBtGattInfo ("%s\n",BEGIN_BLE_DATA);
			if (server->msg_buf) {
				logBtGattInfo ("Already memory allocated... Free memory\n");
				free(server->msg_buf);
				server->msg_buf = NULL;
			}
			server->msg_buf = (char *)calloc(CMD_CHUNK_SIZE, sizeof(char));
			if ( server->msg_buf == NULL ) {
				logBtGattErr ("failed to allocate memory\n");
				gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
				pthread_mutex_unlock(&rssi_lock);
				return;
			}
			server->msg_len = CMD_CHUNK_SIZE;
			gatt_db_attribute_write_result(attrib, id, ecode);
			pthread_mutex_unlock(&rssi_lock);
			return;

		} else if (!strncmp((char *)value, END_BLE_DATA, strlen((char *)END_BLE_DATA))) {
			logBtGattInfo ("%s\n",END_BLE_DATA);
			status = mosquitto_publish(server->mosq, NULL, MQTT_PUB_PROVISIONED_DEVICE, strlen(server->msg_buf) + 1, server->msg_buf, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr ("could not publish to topic:%s err:%d", MQTT_PUB_PROVISIONED_DEVICE, status);
				ecode = BLE_WRITE_ERROR;
			} else {
				logBtGattInfo ("Successfully publish provision device list == \"%s\" on topic %s\n", server->msg_buf, MQTT_PUB_PROVISIONED_DEVICE);
			}
			free(server->msg_buf);
			server->msg_buf = NULL;
			gatt_db_attribute_write_result(attrib, id, ecode);
			pthread_mutex_unlock(&rssi_lock);
			return;

		} else if (server->msg_buf) {

			if (server->msg_len <= (strlen(server->msg_buf) + len )) {
				server->msg_buf = (char *)realloc(server->msg_buf, server->msg_len + CMD_CHUNK_SIZE);
				if ( server->msg_buf == NULL ) {
					logBtGattErr ("failed to allocate memory\n");
					gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
					pthread_mutex_unlock(&rssi_lock);
					return;
				}
				memset ( server->msg_buf + strlen (server->msg_buf) , 0x0, CMD_CHUNK_SIZE);
				server->msg_len += CMD_CHUNK_SIZE;
			}
			strncpy( (server->msg_buf + strlen(server->msg_buf)), (const char *)value, len);
			gatt_db_attribute_write_result(attrib, id, ecode);
		}
	} else {
		logBtGattErr ("Invalid pointer\n");
		gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
	}
	pthread_mutex_unlock(&rssi_lock);
}

/* device_discover_write_cb()
 *
 * \brief : this routine collects command for discover device and publish
 *          to growhouse server.
 *
 * \param [in] attrib    : Pointer to the gatt attribute structure.
 * \param [in] offset    : Offset of data to be received.
 * \param [in] value     : Pointer to the value received from gatt client.
 * \param [in] len       : Length of data received.
 * \param [in] opcode    : Opration to be done.
 * \param [in] att       : Pointer to bt attribute structure.
 * \param [in] user_data : Pointer to user data passed.
 *
 * \return : none
 */
static void device_discover_write_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		const uint8_t *value, size_t len,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	uint8_t	ecode = 0;
	struct server * server = user_data;
	int status = MOSQ_ERR_INVAL;
	logBtGattInfo ("Device discover command callback\n");

	pthread_mutex_lock(&rssi_lock);
	if (value) {

		logBtGattInfo ("Received data len %lu :: %s\n", len, value);
		if (!strncmp((char *)value, BEGIN_BLE_DATA, strlen((char *)BEGIN_BLE_DATA))) {
			logBtGattInfo ("%s\n", BEGIN_BLE_DATA);
			if (server->msg_buf) {
				logBtGattInfo ("Already memory allocated... Free memory\n");
				free(server->msg_buf);
				server->msg_buf = NULL;
			}
			server->msg_buf = (char *)calloc(CMD_CHUNK_SIZE, sizeof(char));
			if ( server->msg_buf == NULL ) {
				logBtGattErr ("failed to allocate memory\n");
				gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
				pthread_mutex_unlock(&rssi_lock);
				return;
			}
			server->msg_len = CMD_CHUNK_SIZE;
			gatt_db_attribute_write_result(attrib, id, ecode);
			pthread_mutex_unlock(&rssi_lock);
			return;

		} else if (!strncmp((char *)value, END_BLE_DATA, strlen((char *)END_BLE_DATA))) {
			logBtGattInfo ("%s\n", END_BLE_DATA);

			/* Publish to growhouse-server*/
			status = mosquitto_publish(server->mosq, NULL, MQTT_PUB_DISCOVER_COMMAND, strlen(server->msg_buf), server->msg_buf, 1, false);
			if ( status != MOSQ_ERR_SUCCESS){
				logBtGattErr ("could not publish to topic:%s err:%d", MQTT_PUB_DISCOVER_COMMAND, status);
				ecode = BLE_WRITE_ERROR;
			} else {
				logBtGattInfo ("Successfully publish Device discover command : \"%s\" on topic %s\n", server->msg_buf, MQTT_PUB_DISCOVER_COMMAND);
			}
			free(server->msg_buf);
			server->msg_buf = NULL;
			gatt_db_attribute_write_result(attrib, id, ecode);
			pthread_mutex_unlock(&rssi_lock);
			return;

		} else if (server->msg_buf) {
			if (server->msg_len <= (strlen(server->msg_buf) + len)) {
				server->msg_buf = (char *)realloc(server->msg_buf, server->msg_len + CMD_CHUNK_SIZE);
				if ( server->msg_buf == NULL ) {
					logBtGattErr ("failed to allocate memory\n");
					gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
					pthread_mutex_unlock(&rssi_lock);
					return;
				}
				memset ( server->msg_buf + strlen (server->msg_buf) , 0x0, CMD_CHUNK_SIZE);
				server->msg_len += CMD_CHUNK_SIZE;
			}
			strncpy( (server->msg_buf + strlen(server->msg_buf)), (const char *)value, len);
			gatt_db_attribute_write_result(attrib, id, ecode);
		}
	} else {
		logBtGattErr ("Invalid pointer\n");
		gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
	}
	pthread_mutex_unlock(&rssi_lock);
}

/* notifyMessageCallback()
 *
 * \brief : this callback function runs after successfully notify
 *          messages over ble and remove the locking to notify next
 *          message over ble.
 *
 * \param [in] user_data : Pointer to user data passed.
 *
 * \return : none
 */
static void notifyMessageCallback(void *user_data)
{
	logBtGattInfo("Notify Message successfully\n");
	pthread_mutex_unlock(&lock);
}

static void pthreadMutexTimedLock() {

	struct timespec timeoutTime;
	clock_gettime(CLOCK_REALTIME, &timeoutTime);
	timeoutTime.tv_sec += 1;
	pthread_mutex_timedlock(&lock, &timeoutTime);
}

/* deviceConnectivity()
 *
 * \brief : this routine notify device list which is successfully provisioned.
 *
 * \param [in] obj     : Pointer to user data provided in mosquitto_new.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : none
 */
static void deviceConnectivity(void * obj, const struct mosquitto_message *message)
{
	int offset = 0;
	int len = MAX_BLE_DATA_LEN;
	bool status = false;
	struct server * server = obj;

	logBtGattInfo ("Notify device connectivity callback\n");

	if (message == NULL) {
		logBtGattErr ("Invalid pointer\n");
		return;
	}

	pthread_mutex_lock(&rssi_lock);
	if ( server != NULL) {
		pthreadMutexTimedLock();
		usleep(100000);
		status = bt_gatt_server_send_indication(server->gatt,
				server->device_connectivity_handle,
				(const uint8_t *)BEGIN_BLE_DATA, strlen(BEGIN_BLE_DATA), notifyMessageCallback, NULL, NULL);
		if (!status) {
			logBtGattErr ("Failed to notify \"%s\"\n", BEGIN_BLE_DATA);
			pthread_mutex_unlock(&lock);
			pthread_mutex_unlock(&rssi_lock);
			return;
		}

		do {
			pthreadMutexTimedLock();
			usleep(100000);

			if ( (message->payloadlen - offset) < MAX_BLE_DATA_LEN)
				len = message->payloadlen - offset;

			status = bt_gatt_server_send_indication(server->gatt,
					server->device_connectivity_handle,
					message->payload+offset,len, notifyMessageCallback, NULL, NULL);
			if (status) {
				logBtGattInfo ("Sucessfully notify \"%s\" of len == %d\n", (char *)message->payload + offset, len);
			} else {
				logBtGattErr ("Failed to notify \"%s\" of len == %d\n", (char *)message->payload + offset, len);
				pthread_mutex_unlock(&lock);
				pthread_mutex_unlock(&rssi_lock);
				return;
			}
			offset += len;
		}while( offset < message->payloadlen);

		pthreadMutexTimedLock();
		usleep(100000);
		status = bt_gatt_server_send_indication(server->gatt,
				server->device_connectivity_handle,
				(const uint8_t *)END_BLE_DATA, strlen(END_BLE_DATA), notifyMessageCallback, NULL, NULL);
		if (!status) {
			logBtGattErr ("Failed to notify \"%s\"\n", END_BLE_DATA);
			pthread_mutex_unlock(&lock);
			pthread_mutex_unlock(&rssi_lock);
			return;
		}
	} else {
		logBtGattErr ("Invalid pointer\n");
	}
	pthread_mutex_unlock(&rssi_lock);
}

/* notifyDiscoveredDevices()
 *
 * \brief : this routine notify discovered devices list to gatt client.
 *
 * \param [in] obj     : Pointer to user data provided in mosquitto_new.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : none
 */

static void notifyDiscoveredDevices(void * obj, const struct mosquitto_message *message)
{
	bool status = false;
	int offset = 0;
	int len = MAX_BLE_DATA_LEN;
	struct server * server = obj;

	logBtGattInfo ("Notify discovered devices callback\n");

	if (message == NULL) {
		logBtGattErr ("Invalid Pointer\n");
		return;
	}

	pthread_mutex_lock(&rssi_lock);
	if ( server != NULL) {
		pthreadMutexTimedLock();
		usleep(100000);
		status = bt_gatt_server_send_indication(server->gatt,
				server->device_list_handle,
				(const uint8_t *)BEGIN_BLE_DATA,strlen(BEGIN_BLE_DATA),notifyMessageCallback, NULL, NULL);

		if (!status) {
			logBtGattErr ("Failed to notify \"%s\"\n", BEGIN_BLE_DATA);
			pthread_mutex_unlock(&rssi_lock);
			pthread_mutex_unlock(&lock);
			return;
		}
		logBtGattInfo ("Sucessfully notify \"%s\"",BEGIN_BLE_DATA);

		do {
			pthreadMutexTimedLock();
			usleep(100000);

			if ( (message->payloadlen - offset) < MAX_BLE_DATA_LEN)
				len = message->payloadlen - offset;

			status = bt_gatt_server_send_indication(server->gatt,
					server->device_list_handle,
					message->payload + offset,len,notifyMessageCallback, NULL, NULL);
			if (status) {
				logBtGattInfo ("Sucessfully notify \"%s\" of len == %d\n", (char *)message->payload + offset, len);
			} else {
				logBtGattErr ("Failed to  notify \"%s\" of len == %d\n", (char *)message->payload + offset, len);
				pthread_mutex_unlock(&rssi_lock);
				pthread_mutex_unlock(&lock);
				return;
			}

			offset += len;
		}while( offset < message->payloadlen);

		pthreadMutexTimedLock();
		usleep(100000);
		status = bt_gatt_server_send_indication(server->gatt,
				server->device_list_handle,
				(const uint8_t *)END_BLE_DATA, strlen(END_BLE_DATA),notifyMessageCallback, NULL, NULL);

		if (status == false){
			logBtGattErr ("Failed to notify \"%s\"\n", END_BLE_DATA);
			pthread_mutex_unlock(&rssi_lock);
			pthread_mutex_unlock(&lock);
			return;
		}
		logBtGattInfo ("Sucessfully notify \"%s\"",END_BLE_DATA);
	} else {
		logBtGattErr ("Invalid pointer\n");
	}
	pthread_mutex_unlock(&rssi_lock);
}

static void device_write_descriptor(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		const uint8_t *value, size_t len,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	logBtGattInfo ("Device discovered list callback enabled\n");
	gatt_db_attribute_write_result(attrib, id, 0);
}

/* updateSelenePropertyFile()
 *
 * \brief : this routine used to update self.properties file in selene.
 *
 * \param [in] payload :  Pointer to the data to be write in file.
 *
 * \return : 0 on success , -1 on failure.
 */
int updateSelenePropertyFile(char * payload)
{
	/* TODO: if gateway is registered, return from this function */

	FILE * SeleneProperties = fopen(SELF_PROP_FILENAME, "w+");
	if (SeleneProperties == NULL) {
		logBtGattErr("Failed to open file \"%s\"\n", SELF_PROP_FILENAME);
		return -1;
	}

	cJSON *node = cJSON_Parse(payload);
	if (node) {
		cJSON * cjson = NULL;
		char * prop_str = NULL;
		enum self_properties_type l_count = PROP_TYPE_MIN;

		for (l_count = PROP_TYPE_MIN; l_count < PROP_TYPE_MAX; l_count++) {
			cjson = cJSON_GetObjectItem(node, self_properties[l_count]);
			if (cjson && cjson->valuestring) {
				prop_str = cjson->valuestring;
				fprintf(SeleneProperties, "%s=%s\n", self_properties[l_count], prop_str);
				logBtGattInfo ("++++ adding value \"%s=%s\" in file\n", self_properties[l_count], prop_str);
			} else {
				logBtGattErr("Null json pointer\n");
				/*
				 * If control reaches here, it means that some property is
				 * not found. So, we need to empty the selene.properties file
				 */
				fclose(SeleneProperties);
				SeleneProperties = fopen(SELF_PROP_FILENAME, "w");
				if (SeleneProperties)
					fclose(SeleneProperties);
				return -1;
			}
		}
	} else {
		logBtGattErr("Could not parse json node\n");
		return -1;
	}
	// TODO :: add logic for db delete and restart selene
	//         instead of system command
	system("rm -rf /opt/selene/db/*");
	system("rm -rf /opt/selene/databus/*");
	system("rm -rf /opt/selene/log/*");
	system("systemctl restart selene");

	fclose(SeleneProperties);
	cJSON_Delete(node);

	return 0;
}

/* gateway_selene_version_read_cb()
 *
 * \brief : this callback function read selene version and
 *         send to gatt-client
 *
 * \param [in] attrib    : Pointer to the gatt attribute structure.
 * \param [in] offset    : Offset of data to be received.
 * \param [in] opcode    : Opration to be done.
 * \param [in] att       : Pointer to bt attribute structure.
 * \param [in] user_data : Pointer to user data passed.
 *
 * \return : none
 */
static void gateway_selene_version_read_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	uint8_t  value[] = "0.0.0";
	FILE * fptr = NULL;

	logBtGattInfo ("Gateway selene version Read called\n");

	if ( access(SELENE_VERSION_FILE, F_OK) != -1 ){

		if ((fptr = fopen(SELENE_VERSION_FILE, "r")) == NULL)
		{
			logBtGattErr ("Failed to open file");
		} else {
			// reads text until newline
			fscanf(fptr,"%[^\n]", value);
			logBtGattInfo ("Selene current version : %s", value);
			fclose(fptr);
		}
	}

	gatt_db_attribute_read_result(attrib, id, 0, value, sizeof(value));
}

/* gateway_account_write_cb()
 *
 * \brief : this callback function collects information for gateway provision
 *          and update to file.
 *
 * \param [in] attrib    : Pointer to the gatt attribute structure.
 * \param [in] offset    : Offset of data to be received.
 * \param [in] value     : Pointer to the value received from gatt client.
 * \param [in] len       : Length of data received.
 * \param [in] opcode    : Opration to be done.
 * \param [in] att       : Pointer to bt attribute structure.
 * \param [in] user_data : Pointer to user data passed.
 *
 * \return : none
 */
static void gateway_account_write_cb(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		const uint8_t *value, size_t len,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	struct server *server = user_data;
	uint8_t ecode = 0;
	int status = 0;
	logBtGattInfo ("Gateway information write callback\n");

	pthread_mutex_lock(&rssi_lock);
	if (value) {
		logBtGattInfo ("Received data len %lu :: %s\n", len, value);
		if (!strncmp((char *)value, BEGIN_BLE_DATA, strlen((char *)BEGIN_BLE_DATA))) {
			logBtGattInfo ("%s\n", BEGIN_BLE_DATA);
			if (server->msg_buf) {
				logBtGattInfo ("Already memory allocated... Free memory\n");
				free(server->msg_buf);
				server->msg_buf = NULL;
			}
			server->msg_buf = (char *)calloc(CMD_CHUNK_SIZE, sizeof(char));
			if ( server->msg_buf == NULL ) {
				logBtGattErr ("failed to allocate memory\n");
				gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
				pthread_mutex_unlock(&rssi_lock);
				return;
			}
			server->msg_len = CMD_CHUNK_SIZE;
			gatt_db_attribute_write_result(attrib, id, ecode);
			pthread_mutex_unlock(&rssi_lock);
			return;
		} else if (!strncmp((char *)value, END_BLE_DATA, strlen((char *)END_BLE_DATA))) {
			logBtGattInfo ("%s\n", END_BLE_DATA);
			logBtGattInfo ("Data to be write in file \"%s\"\n",server->msg_buf);
			/* Update gateway key information to file */
			status = updateSelenePropertyFile(server->msg_buf);
			if (status == -1) {
				logBtGattErr("Failed to update file \"%s\"\n", SELF_PROP_FILENAME);
				ecode = BLE_WRITE_ERROR;
			}
			free(server->msg_buf);
			server->msg_buf = NULL;
			gatt_db_attribute_write_result(attrib, id, ecode);
			pthread_mutex_unlock(&rssi_lock);
			return;
		} else if (server->msg_buf) {
			if (server->msg_len <= (strlen(server->msg_buf) + len)) {
				server->msg_buf = (char *)realloc(server->msg_buf, server->msg_len + CMD_CHUNK_SIZE);
				if ( server->msg_buf == NULL ) {
					logBtGattErr ("failed to allocate memory\n");
					gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
					pthread_mutex_unlock(&rssi_lock);
					return;
				}
				memset ( server->msg_buf + strlen (server->msg_buf) , 0x0, CMD_CHUNK_SIZE);
				server->msg_len += CMD_CHUNK_SIZE;
			}
			strncpy( (server->msg_buf + strlen(server->msg_buf)), (const char *)value, len);
			gatt_db_attribute_write_result(attrib, id, ecode);
		}
	} else {
		logBtGattErr ("Invalid pointer\n");
		gatt_db_attribute_write_result(attrib, id, BLE_WRITE_ERROR);
	}
	pthread_mutex_unlock(&rssi_lock);
}

static void gateway_write_descriptor(struct gatt_db_attribute *attrib,
		unsigned int id, uint16_t offset,
		const uint8_t *value, size_t len,
		uint8_t opcode, struct bt_att *att,
		void *user_data)
{
	logBtGattErr ("Gateway connectivity callback enabled\n");
	gatt_db_attribute_write_result(attrib, id, 0);
}

/* gatewayConnectivity()
 *
 * \brief : this routine send notification for gateway connectivity.
 *
 * \param [in] obj     : Pointer to user data provided in mosquitto_new.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : none
 */
static void gatewayConnectivity(void * obj, const struct mosquitto_message *message)
{
	bool status = false;
	uint8_t connectivity = 0;
	struct server * server = obj;

	if (message == NULL) {
		logBtGattErr ("Invalid pointer\n");
		return;
	}

	if ( server != NULL) {
		cJSON * node = cJSON_Parse(message->payload);
		if (node == NULL) {
			logBtGattErr("Failed to parse json\n");
			return;
		}
		char * result = cJSON_GetObjectItem(node, "gatewayConnectivity")->valuestring;
		if (!strcmp(result, "true"))
			connectivity = 1;

		pthread_mutex_lock(&rssi_lock);
		usleep(10000);
		status = bt_gatt_server_send_notification(server->gatt,
				server->gateway_connectivity_handle,
				&connectivity, sizeof(connectivity));
		pthread_mutex_unlock(&rssi_lock);
		if (status) {
			logBtGattInfo ("Sucessfully notify gateway connectivity = %d\n", connectivity);
		} else {
			logBtGattErr ("Failed to notify gateway connectivity = %d\n", connectivity);
		}
		cJSON_Delete(node);
	} else {
		logBtGattErr ("Invalid pointer\n");
		return;
	}
}

static void populate_gatt_service(struct server *server)
{
	bt_uuid_t uuid;
	struct gatt_db_attribute *service, *svc_chngd;

	/* Add the GATT service */
	bt_uuid16_create(&uuid, UUID_GATT);
	service = gatt_db_add_service(server->db, &uuid, true, 4);

	bt_uuid16_create(&uuid, GATT_CHARAC_SERVICE_CHANGED);
	svc_chngd = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_READ,
			BT_GATT_CHRC_PROP_READ | BT_GATT_CHRC_PROP_INDICATE,
			gatt_service_changed_cb,
			NULL, server);
	server->gatt_svc_chngd_handle = gatt_db_attribute_get_handle(svc_chngd);

	bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
	gatt_db_service_add_descriptor(service, &uuid,
			BT_ATT_PERM_READ | BT_ATT_PERM_WRITE,
			gatt_svc_chngd_ccc_read_cb,
			gatt_svc_chngd_ccc_write_cb, server);

	gatt_db_service_set_active(service, true);
}

/* populate_gateway_service()
 *
 * \brief : this routine used to create services and characteristics for
 *          gateway provision.
 *
 * \param [in] server : Pointer to instance of server.
 *
 * \return : none
 */
static void populate_gateway_service(struct server *server)
{
	bt_uuid_t uuid;
	struct gatt_db_attribute *service , *apikey;

	/* Add GatewayProvision Service */
	bt_uuid16_create(&uuid, UUID_GATEWAY_PROVISION);
	service = gatt_db_add_service(server->db, &uuid, true, 8);
	server->gateway_handle = gatt_db_attribute_get_handle(service);

	/* Add Gateway account key Characteristic */
	bt_uuid16_create(&uuid, UUID_GATEWAY_ACCOUNT);
	apikey = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_WRITE | BT_ATT_PERM_READ,
			BT_GATT_CHRC_PROP_WRITE | BT_GATT_CHRC_PROP_READ,
			gateway_selene_version_read_cb, gateway_account_write_cb, server);
	server->gateway_account_handle = gatt_db_attribute_get_handle(apikey);

	/* Add Gateway connectivity Characteristic */
	bt_uuid16_create(&uuid, UUID_GATEWAY_CONNECTIVITY);
	apikey = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_READ | BT_ATT_PERM_NONE ,
			BT_GATT_CHRC_PROP_READ |BT_GATT_CHRC_PROP_NOTIFY ,
			NULL, NULL, server);
	server->gateway_connectivity_handle = gatt_db_attribute_get_handle(apikey);

	bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
	gatt_db_service_add_descriptor(service, &uuid,
			BT_ATT_PERM_WRITE,
			NULL,gateway_write_descriptor, server);

	gatt_db_service_set_active(service, true);
}

/* populate_device_service()
 *
 * \brief : this routine used to create services and characteristics for
 *          devices provision.
 *
 * \param [in] server : Pointer to instance of server.
 *
 * \return : none
 */
static void populate_device_service(struct server *server)
{
	bt_uuid_t uuid;
	struct gatt_db_attribute *service , *device;

	/* Add Device Provision Service */
	bt_uuid16_create(&uuid, UUID_DEVICE_PROVISION);
	service = gatt_db_add_service(server->db, &uuid, true, 16);
	server->gateway_handle = gatt_db_attribute_get_handle(service);

	/* Add device discover command  Characteristic */
	bt_uuid16_create(&uuid, UUID_DEVICE_DISCOVER_COMMAND);
	device = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_WRITE ,
			BT_GATT_CHRC_PROP_WRITE,
			NULL, device_discover_write_cb, server);
	server->device_discover_handle = gatt_db_attribute_get_handle(device);

	/* Add discovered device list Characteristic */
	bt_uuid16_create(&uuid, UUID_DEVICE_DISCOVERED_LIST);
	device = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_WRITE | BT_ATT_PERM_READ,
			BT_GATT_CHRC_PROP_NOTIFY | BT_GATT_CHRC_PROP_WRITE | BT_GATT_CHRC_PROP_READ,
			NULL, device_provision_list_write_cb, server);
	server->device_list_handle = gatt_db_attribute_get_handle(device);

	bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
	gatt_db_service_add_descriptor(service, &uuid,
			BT_ATT_PERM_WRITE | BT_ATT_PERM_READ,
			NULL, device_write_descriptor, server);

	/* Add device connectivity Characteristic */
	bt_uuid16_create(&uuid, UUID_DEVICE_CONNECTIVITY);
	device = gatt_db_service_add_characteristic(service, &uuid,
			BT_ATT_PERM_WRITE ,
			BT_GATT_CHRC_PROP_NOTIFY | BT_GATT_CHRC_PROP_WRITE,
			NULL, NULL, NULL);
	server->device_connectivity_handle = gatt_db_attribute_get_handle(device);

	bt_uuid16_create(&uuid, GATT_CLIENT_CHARAC_CFG_UUID);
	gatt_db_service_add_descriptor(service, &uuid,
			BT_ATT_PERM_WRITE | BT_ATT_PERM_READ,
			NULL, device_write_descriptor, server);

	gatt_db_service_set_active(service, true);
}

static void populate_db(struct server *server)
{
	populate_gatt_service(server);
	populate_gateway_service(server);
	populate_device_service(server);
}

/* mosquitto_cleanup()
 *
 * \brief : this routine cleanup mosquitto instances.
 *
 * \param [in] mosq : Pointer to the instance of mosquitto.
 *
 * \return : none
 */
static void mosquitto_cleanup(struct mosquitto * mosq)
{
	if (mosq) {
		mosquitto_destroy(mosq);
	}
	mosquitto_lib_cleanup();

}

/* message_callback()
 *
 * \brief : this callback function called when received any message on
 *          subscribed topic.
 *
 * \param [in] mosq    : Pointer to the instance of mosquitto.
 * \param [in] obj     : Pointer to user data provided in mosquitto_new.
 * \param [in] message : Pointer to instance of mosquitto message.
 *
 * \return : none
 */
static void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	logBtGattInfo ("Mqtt Receive : %d messaeg : %s for topic '%s'\n",
			message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub(MQTT_SUB_DISCOVERED_DEVICE,
			message->topic, &match);
	if (match) {
		logBtGattInfo ("Discovered device received\n");
		notifyDiscoveredDevices(obj, message);
		return;
	}

	mosquitto_topic_matches_sub(MQTT_SUB_GATEWAY_CONNECT_RESPONSE,
			message->topic, &match);
	if (match) {
		logBtGattInfo ("Gateway connectivity received\n");
		gatewayConnectivity(obj, message);
		return;
	}

	mosquitto_topic_matches_sub(MQTT_SUB_DEVICE_CONNECT_RESPONSE,
			message->topic, &match);
	if (match) {
		logBtGattInfo ("Device connectivity received\n");
		deviceConnectivity(obj, message);
		return;
	}

	mosquitto_topic_matches_sub(MQTT_SUB_GATEWAY_DISCONNECT,
			message->topic, &match);
	if (match) {
		logBtGattInfo ("Gateway disconnect received\n");
		mainloop_quit();
		return;
	}
}

/* connect_callback()
 *
 * \brief : this callback function called when mosquitto service start.
 *
 * \param [in] mosq   : Pointer to the instance of mosquitto.
 * \param [in] obj    : Pointer to user data provided in mosquitto_new.
 *        [in] result : Return code for connection responce.
 *
 * \return : none
 */

static void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	int status = MOSQ_ERR_INVAL;
	logBtGattInfo ("mosquitto connect callback, rc=%d %p\n", result, mosq);
	do {
		logBtGattInfo ("Subscribe : %s\n",MQTT_SUB_DISCOVERED_DEVICE);
		status = mosquitto_subscribe(mosq, NULL, MQTT_SUB_DISCOVERED_DEVICE, 0);
		if (MOSQ_ERR_SUCCESS != status) {
			logBtGattErr("could not subscribe to topic:%s err:%d", MQTT_SUB_DISCOVERED_DEVICE, status);
			break;
		}

		logBtGattInfo ("Subscribe : %s\n",MQTT_SUB_GATEWAY_CONNECT_RESPONSE);
		status = mosquitto_subscribe(mosq, NULL, MQTT_SUB_GATEWAY_CONNECT_RESPONSE, 0);
		if (MOSQ_ERR_SUCCESS != status) {
			logBtGattErr("could not subscribe to topic:%s err:%d", MQTT_SUB_GATEWAY_CONNECT_RESPONSE, status);
			break;
		}

		logBtGattInfo ("Subscribe : %s\n",MQTT_SUB_DEVICE_CONNECT_RESPONSE);
		status = mosquitto_subscribe(mosq, NULL, MQTT_SUB_DEVICE_CONNECT_RESPONSE, 0);
		if (MOSQ_ERR_SUCCESS != status) {
			logBtGattErr("could not subscribe to topic:%s err:%d", MQTT_SUB_DEVICE_CONNECT_RESPONSE, status);
			break;
		}

		logBtGattInfo ("Subscribe : %s\n",MQTT_SUB_GATEWAY_DISCONNECT);
		status = mosquitto_subscribe(mosq, NULL, MQTT_SUB_GATEWAY_DISCONNECT, 0);
		if (MOSQ_ERR_SUCCESS != status) {
			logBtGattErr("could not subscribe to topic:%s err:%d", MQTT_SUB_GATEWAY_DISCONNECT, status);
			break;
		}

	} while(0);

	if (status != MOSQ_ERR_SUCCESS) {
		mosquitto_cleanup(mosq);
		mosq = NULL;
	}
}

/* btgatt_server_mosquitto_initialize()
 *
 * \brief : this routine initialize instance of mosquitto and set callbacks.
 *
 * \param [in] server : Pointer to instance of server
 *
 * \return : Instance of mosquitto
 */
static struct mosquitto * btgatt_server_mosquitto_initialize(void *server)
{
	char clientid[24] = {0};
	struct mosquitto *mosq = NULL;
	int status = MOSQ_ERR_INVAL;

	do {
		/* This function will always return success, so not checking its return value */
		mosquitto_lib_init();

		memset(clientid, 0, 24);
		snprintf(clientid, 24, "bleServer_%d", getpid());
		mosq = mosquitto_new(clientid, true, server);
		if (!mosq){
			logBtGattErr("failed to start mosquitto\n");
			break;
		}

		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

		status = mosquitto_connect(mosq, mqtt_host, mqtt_port, MOSQUITTO_PING_TIMEOUT);
		if (MOSQ_ERR_SUCCESS != status) {
			logBtGattErr("Unable to connect Mosquitto.\n");
			break;
		}

		status = mosquitto_loop_start(mosq);
		if (MOSQ_ERR_SUCCESS != status) {
			logBtGattErr("could not start mosquitto main loop err:%d", status);
			break;
		}
	} while(0);

	if (status != MOSQ_ERR_SUCCESS) {
		mosquitto_cleanup(mosq);
		mosq = NULL;
	}

	return mosq;
}

/* btgatt_server_mosquitto_destroy()
 *
 * \brief : this routine cleanup mosquitto instance.
 *
 * \param [in] mosq : Pointer to the instance of mosquitto
 *
 * \return : none
 */
static void btgatt_server_mosquitto_destroy(struct mosquitto * mosq)
{
	if (mosq) {

		// We need a short delay here, to prevent the Mosquitto library being
		//  torn down by the operating system before all the network operations
		//  are finished.
		sleep (1);

		mosquitto_disconnect(mosq);
		/*
		 * second argument true is the force
		 * flag to stop the mosquitto loop
		 */
		mosquitto_loop_stop(mosq, true);
		mosquitto_cleanup(mosq);
	}
}


static struct server *server_create(int fd, uint16_t mtu)
{
	struct server *server;
	size_t name_len = strlen(test_device_name);

	server = new0(struct server, 1);
	if (!server) {
		fprintf(stderr, "Failed to allocate memory for server\n");
		return NULL;
	}

	server->att = bt_att_new(fd, false);
	if (!server->att) {
		fprintf(stderr, "Failed to initialze ATT transport layer\n");
		goto fail;
	}

	if (!bt_att_set_close_on_unref(server->att, true)) {
		fprintf(stderr, "Failed to set up ATT transport layer\n");
		goto fail;
	}

	if (!bt_att_register_disconnect(server->att, att_disconnect_cb, NULL,
				NULL)) {
		fprintf(stderr, "Failed to set ATT disconnect handler\n");
		goto fail;
	}

	server->name_len = name_len + 1;
	server->device_name = malloc(name_len + 1);
	if (!server->device_name) {
		fprintf(stderr, "Failed to allocate memory for device name\n");
		goto fail;
	}

	memcpy(server->device_name, test_device_name, name_len);
	server->device_name[name_len] = '\0';

	server->fd = fd;
	server->db = gatt_db_new();
	if (!server->db) {
		fprintf(stderr, "Failed to create GATT database\n");
		goto fail;
	}

	server->gatt = bt_gatt_server_new(server->db, server->att, mtu);
	if (!server->gatt) {
		fprintf(stderr, "Failed to create GATT server\n");
		goto fail;
	}

	if (verbose) {
		bt_att_set_debug(server->att, att_debug_cb, "att: ", NULL);
		bt_gatt_server_set_debug(server->gatt, gatt_debug_cb,
				"server: ", NULL);
	}

	/* Random seed for generating fake Heart Rate measurements */
	srand(time(NULL));

	server->mosq = btgatt_server_mosquitto_initialize(server);
	if (!server->mosq) {
		logBtGattErr("Could not connect to mosquitto\n");
		goto fail;
	}

	/* bt_gatt_server already holds a reference */
	populate_db(server);

	return server;

fail:
	btgatt_server_mosquitto_destroy(server->mosq);
	gatt_db_unref(server->db);
	free(server->device_name);
	bt_att_unref(server->att);
	free(server);

	return NULL;
}

static void server_destroy(struct server *server)
{
	if (server) {
		btgatt_server_mosquitto_destroy(server->mosq);
		server->mosq = NULL;

		bt_gatt_server_unref(server->gatt);
		gatt_db_unref(server->db);

		free(server->device_name);
		free(server);
	}
}

static int l2cap_le_att_listen_and_accept(bdaddr_t *src, int sec,
		uint8_t src_type)
{
	int sk, nsk;
	struct sockaddr_l2 srcaddr, addr;
	socklen_t optlen;
	struct bt_security btsec;
	char ba[18];

	sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (sk < 0) {
		logBtGattErrUsingErrNo("Failed to create L2CAP socket");
		return -1;
	}

	/* Set up source address */
	memset(&srcaddr, 0, sizeof(srcaddr));
	srcaddr.l2_family = AF_BLUETOOTH;
	srcaddr.l2_cid = htobs(ATT_CID);
	srcaddr.l2_bdaddr_type = src_type;
	bacpy(&srcaddr.l2_bdaddr, src);

	if (bind(sk, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
		logBtGattErrUsingErrNo("Failed to bind L2CAP socket");
		goto fail;
	}

	/* Set the security level */
	memset(&btsec, 0, sizeof(btsec));
	btsec.level = sec;
	if (setsockopt(sk, SOL_BLUETOOTH, BT_SECURITY, &btsec,
				sizeof(btsec)) != 0) {
		fprintf(stderr, "Failed to set L2CAP security level\n");
		goto fail;
	}

	if (listen(sk, 10) < 0) {
		logBtGattErrUsingErrNo("Listening on socket failed");
		goto fail;
	}

	logBtGattErr("Started listening on ATT channel. Waiting for connections\n");

	memset(&addr, 0, sizeof(addr));
	optlen = sizeof(addr);
	nsk = accept(sk, (struct sockaddr *) &addr, &optlen);
	if (nsk < 0) {
		logBtGattErrUsingErrNo("Accept failed");
		goto fail;
	}

	bacpy(src, &addr.l2_bdaddr);

	ba2str(&addr.l2_bdaddr, ba);
	logBtGattErr("Connect from %s\n", ba);
	close(sk);

	return nsk;

fail:
	close(sk);
	return -1;
}


/* cmd_rssi()
 *
 * \brief : this routine reads the rssi value for device and if
 *          the value goes five times smaller then -96 then disconnect
 *          the device.
 *
 * \return : none
 */
static void cmd_rssi(int dev_id, bdaddr_t * ba_dst)
{
	struct hci_conn_info_req *cr;
	int8_t rssi;
	int dd;

	dd = hci_open_dev(dev_id);
	if (dd < 0) {
		logBtGattErr("HCI device open failed");
		return;
	}

	cr = malloc(sizeof(*cr) + sizeof(struct hci_conn_info));
	if (!cr) {
		logBtGattErr("Can't allocate memory");
		hci_close_dev(dd);
		return;
	}

	bacpy(&cr->bdaddr, ba_dst);

	cr->type = LE_LINK;
	if (ioctl(dd, HCIGETCONNINFO, (unsigned long) cr) < 0) {
		logBtGattErr("Get connection info failed");
		free(cr);
		hci_close_dev(dd);
		return;
	}

	if (hci_read_rssi(dd, htobs(cr->conn_info->handle), &rssi, 1000) < 0) {
		logBtGattErr("Read RSSI failed");
		free(cr);
		hci_close_dev(dd);
		return;
	}

	if (rssi > -96) {
		min_rssi_count = 0;
	} else {
		min_rssi_count++;
	}

	if (min_rssi_count >= 5) {
		logBtGattErr("Device is out of range");
		mainloop_quit();
	}

	free(cr);
	hci_close_dev(dd);
	usleep(100000);
}


#if 0
/* TODO: register signal */
static void signal_cb(int signum)
{
	switch (signum) {
		case SIGINT:
		case SIGTERM:
			mainloop_exit = true;
			mainloop_quit();
			break;
		default:
			break;
	}
}
#endif

struct rssi_thread_data {
	bool loop;
	bdaddr_t dst_addr;
};

void * rssi_thread_routine(void * data)
{
	struct rssi_thread_data * rssi = (struct rssi_thread_data *) data;
	if (!rssi)
		return NULL;

	while (rssi->loop) {
		pthread_mutex_lock(&rssi_lock);
		cmd_rssi(0, &rssi->dst_addr);
		pthread_mutex_unlock(&rssi_lock);
		usleep(5000000);
	}
	return NULL;
}

/*
 * ble-server
 * main routine
 */
int main(int argc, char *argv[])
{
	bdaddr_t src_addr = {0};
	int fd = -1;
	uint16_t mtu = 0;
	struct server *server = NULL;
	int status = MOSQ_ERR_INVAL;
	int ctl = 0;

	logBtGattInfo("Ble-server application started with version = %d\n", BLE_SERVER_SERVICE_VERSION);

	//	signal(SIGTERM, signal_cb);

	/* Open HCI socket  */
	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) {
		logBtGattErr("Can't open HCI socket.");
		exit(1);
	}

	/* Start HCI device */
	if (ioctl(ctl, HCIDEVUP, 0) < 0) {
		if (errno != EALREADY) {
			logBtGattErrUsingErrNo("Can't init device hci%d: %s (%d)\n",
					0, strerror(errno), errno);
			exit(1);
		}
	}

	/* fill hci0 address into src address */
	if (hci_devba(hci_devid("hci0"), &src_addr) < 0) {
		logBtGattErrUsingErrNo("Adapter not available");
		return EXIT_FAILURE;
	}

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		logBtGattErr("\n mutex init has failed\n");
		return EXIT_FAILURE;
	}

	if (pthread_mutex_init(&rssi_lock, NULL) != 0)
	{
		logBtGattErr("\n mutex init has failed\n");
		return EXIT_FAILURE;
	}

	while (mainloop_exit == false) {

		struct rssi_thread_data rssi;
		int ret = -1;
		pthread_t rssi_thread;

		rssi.loop = true;
		rssi.dst_addr = src_addr;

		fd = l2cap_le_att_listen_and_accept(&rssi.dst_addr, BT_SECURITY_LOW, BDADDR_LE_PUBLIC);
		if (fd < 0) {
			logBtGattErr ("Failed to accept L2CAP ATT connection\n");
			return EXIT_FAILURE;
		}

		mainloop_init();

		server = server_create(fd, mtu);
		if (!server) {
			close(fd);
			return EXIT_FAILURE;
		}


		memset(&rssi_thread, 0, sizeof(pthread_t));
		ret = pthread_create(&rssi_thread, NULL, rssi_thread_routine, &rssi);
		if (ret < 0) {
			logBtGattErr("loop cannot be started\n");
			server_destroy(server);
			server = NULL;
			return EXIT_SUCCESS;
		}

		mainloop_run();

		rssi.loop = false;
		pthread_join(rssi_thread, NULL);

		logBtGattInfo ("\n\nBLE Shutting down...\n");

		status = mosquitto_publish(server->mosq, NULL, MQTT_PUB_BLE_SHUTTING_DOWN, strlen("Shutting down"), "Shutting down", 1, false);
		if ( status != MOSQ_ERR_SUCCESS){
			logBtGattErr("could not publish to topic:%s err:%d", MQTT_PUB_BLE_SHUTTING_DOWN, status);
		}
		else {
			logBtGattInfo("successfully publish ble shutting down on topic : %s\n",MQTT_PUB_BLE_SHUTTING_DOWN);
		}
		server_destroy(server);
		server = NULL;
	}

	return EXIT_SUCCESS;
}
