/*
 * growhouse-server.h: device descriptors (device types), cluster ids and attirbute ids
 * Copyright (c) 2018
 */
#ifndef __GROWHOUSE_SERVER_H__
#define __GROWHOUSE_SERVER_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <pthread.h>
#include <libgen.h>
#include <signal.h>
#include <mosquitto.h>
#include <cJSON.h>

/*TODO Add information about SCM and LED Node*/

#define GROWHOUSE_SERVICE_VERSION 6
#define ATTRIBUTE_DATATYPE_INT16_LEN  2
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

#define logBtGattInfo(fmt, args...) printf("%s:%d " fmt, __func__,__LINE__, ##args)
#define logBtGattErr(fmt, args...) printf("%s:%d " fmt, __func__,__LINE__, ##args)
#define logBtGattErrUsingErrNo(fmt, args...) printf("%s: " fmt ": %s", __func__, ##args, strerror(errno))
#endif

enum Constants {
	// Device Constants
	DEVICE_ID_ON_OFF_SWITCH = 0x0000,
	DEVICE_ID_LEVEL_CONTROL_SWITCH = 0x0001,
	DEVICE_ID_COLOR_DIMMER_SWITCH = 0x0105,
	DEVICE_ID_ON_OFF_LIGHT = 0x0100,
	DEVICE_ID_DIMMABLE_LIGHT = 0x0101,
	DEVICE_ID_COLOR_DIMMABLE_LIGHT = 0x0102,
	DEVICE_ID_HA_ON_OFF_LIGHT = 0x0103,
	DEVICE_ID_EXTENDED_COLOR_LIGHT = 0x0210,
	DEVICE_ID_COLOR_TEMPERATURE_LIGHT = 0x0220,
	DEVICE_TYPE_SENSOR = 0x0302,
	DEVICE_TYPE_CONTACT_SENSOR = 0x0402,
	DEVICE_TYPE_SMART_PLUG = 0x0051,
	DEVICE_TYPE_OCCUPANCY_SENSOR = 0x0107,

	//soil node device type
	DEVICE_TYPE_SOIL_NODE = 0xBF04,

	// soil node cluster
	SOIL_NODE_PH_CLUSTER = 0xFD00,
	SOIL_NODE_MOISTURE_CLUSTER = 0XFD01,
	SOIL_NODE_PH_ATTRIBUTE = 0x0000,
	SOIL_NODE_MOISTURE_ATTRIBUTE = 0x0000,

	// light shield device type
	DEVICE_TYPE_LIGHT_SHIELD = 0xBF02,

	// light shield cluster
	LIGHT_SHILED_CLUSTER = 0xFD02,
	LIGHT_SHILED_R_ATTRIBUTE = 0x0000,
	LIGHT_SHILED_G_ATTRIBUTE = 0x0001,
	LIGHT_SHILED_B_ATTRIBUTE = 0x0002,

	// power devices
	POWER_CONF_CLUSTER = 0x0001,
	POWER_BATTERY_VOLTAGE_ATTRIBUTE = 0x0020,
	POWER_BATTERY_VOLTAGE_TYPE = 0x0020,

	// scm devices
	DEVICE_TYPE_SCM = 0xBF03,
	SCM_TEMPERATURE_CLUSTER = 0x0402,
	SCM_PRESSURE_CLUSTER = 0x0403,
	SCM_HUMIDITY_CLUSTER = 0x0405,
	SCM_CARBON_MONOXIDE_CLUSTER = 0x040C,
	SCM_CARBON_DIOXIDE_CLUSTER = 0x040D,
	SCM_MEASURED_VALUE_ATTRIBUTE = 0x0000,

	// lednode rgb config cluster
	LED_NODE_RGB_CONFIG_COMMAND_CLUSTER = 0x0008,

	// Decisive Cluster # For In or Out
	IN_OUT_DECISION_CLUSTER = 0x0400,
	// ZigBee On/off Cluster
	ON_OFF_CLUSTER = 0x0006,
	// ZigBee Color-Control Cluster
	COLOR_CONTROL_CLUSTER = 0x0300,
	// ZigBee Level-Control Cluster
	LEVEL_CONTROL_CLUSTER = 0x0008,
	// ZigBee Identify Cluster
	IDENTIFY_CLUSTER = 0x0003,
	IDENTIFY_TIME_ATTRIBUTE = 0x0000,

	// custom enddevice firmware update
	CUSTOM_FIRMWARE_VERSION_CLUSTER = 0x0008,

	// ZigBee Bootloader Cluster
	BOOTLOADER_CLUSTER = 0x0019,
	FIRMWARE_VERSION_ATTRIBUTE = 0x0002,
	FIRMWARE_VERSION_TYPE = 0x0023,
	IMAGE_TYPE_ATTRIBUTE = 0x0008,
	IMAGE_TYPE_TYPE = 0x0021,
	MANUFACTURER_ID_ATTRIBUTE = 0x0007,
	MANUFACTURER_ID_TYPE = 0x0021,

	// ZigBee IAS Zone Cluster
	IAS_ZONE_CLUSTER = 0x0500,
	ZONESTATUS_VAL_ATTRIBUTE = 0x0002,
	ZONESTATUS_VAL_TYPE = 0x0030,

	// ZigBee Temperature Cluster
	TEMPERATURE_CLUSTER = 0x0402,
	TEMPERATURE_VAL_ATTRIBUTE = 0x0000,
	TEMPERATURE_VAL_TYPE = 0x0029,

	// Simple Metering Cluster
	SIMPLE_METERING_CLUSTER = 0x0702,
	SUMMATION_VAL_ATTRIBUTE = 0x0000,
	SUMMATION_VAL_TYPE = 0x0025,
	UNIT_OF_MEASURE_ATTRIBUTE = 0x0300,
	UNIT_OF_MEASURE_TYPE = 0x0030,
	MULTIPLIER_ATTRIBUTE = 0x0301,
	MULTIPLIER_TYPE = 0x0022,
	DIVISOR_ATTRIBUTE = 0x0302,
	DIVISOR_TYPE = 0x0022,
	SUMMATION_FORMATTING_ATTRIBUTE = 0x0303,
	SUMMATION_FORMATTING_TYPE = 0x0018,

	// Illuminance Cluster
	ILLUMINANCE_CLUSTER = 0x0400,
	ILLUMINANCE_VAL_ATTRIBUTE = 0x0000,
	ILLUMINANCE_VAL_TYPE = 0x0021,
	ILLUMINANCE_MINVAL_ATTRIBUTE = 0x0001,
	ILLUMINANCE_MAXVAL_ATTRIBUTE = 0x0002,

	// Humidity Cluster
	HUMIDITY_CLUSTER = 0x0405,
	HUMIDITY_VAL_ATTRIBUTE = 0x0000,
	HUMIDITY_VAL_TYPE = 0x0021,
	HUMIDITY_MINVAL_ATTRIBUTE = 0x0001,
	HUMIDITY_MAXVAL_ATTRIBUTE = 0x0002,

	// Occupancy Sensing Cluster
	OCCUPANCY_CLUSTER = 0x0406,
	OCCUPANCY_VAL_ATTRIBUTE = 0x0000,
	OCCUPANCY_VAL_TYPE = 0x0018,
	OCCUPANCY_SENSOR_TYPE_ATTRIBUTE = 0x0001,
	OCCUPANCY_SENSOR_TYPE_TYPE = 0x0030,

	// Electrical Measurement Cluster
	ELECTRICAL_CLUSTER = 0x0B04,
	RMS_VOLTAGE_VAL_ATTRIBUTE = 0x0505,
	RMS_VOLTAGE_VAL_TYPE = 0x21,
	RMS_CURRENT_VAL_ATTRIBUTE = 0x0508,
	RMS_CURRENT_VAL_TYPE = 0x21,
	ACTIVE_POWER_VAL_ATTRIBUTE = 0x050B,
	ACTIVE_POWER_VAL_TYPE = 0x29,
	ALARMS_MASK_ATTRIBUTE = 0x0800,
	ALARMS_MASK_TYPE = 0x19,
	VOLTAGE_OVERLOAD_ATTRIBUTE = 0x0801,
	VOLTAGE_OVERLOAD_TYPE = 0x29,
	CURRENT_OVERLOAD_ATTRIBUTE = 0x0802,
	CURRENT_OVERLOAD_TYPE = 0x29,
	ACTIVE_POWER_OVERLOAD_ATTRIBUTE = 0x0803,
	ACTIVE_POWER_OVERLOAD_TYPE = 0x29,

	// Diagnostics Cluster
	DIAGNOSTICS_CLUSTER = 0x0B05,
	LQI_VAL_ATTRIBUTE = 0x011C,
	LQI_VAL_TYPE = 0x20,
	RSSI_VAL_ATTRIBUTE = 0x011D,
	RSSI_VAL_TYPE = 0x28,

	// Endpoints
	SELF_ENDPOINT = 0,
	SMART_ENERGY_ENDPOINT = 220,
	GREEN_POWER_ENDPOINT = 242,

	// Commands
	ZONE_STATUS_CHANGE_NOTIFICATION_COMMAND_ID = 0x00,

	// Gateway Transport Defined Node States
	ND_JUST_JOINED =   0x00,
	ND_HAVE_ACTIVE =   0x01,
	ND_HAVE_EP_DESC =  0x02,
	ND_JOINED =        0x10,
	ND_UNRESPONSIVE =  0x11,
	ND_LEAVE_SENT =    0x20,
	ND_LEFT =          0x30,
	ND_UNKNOWN =       0xff,

	// Timing Defines
	SHUTDOWN_TIMER = 1000,
	GATEWAY_HEARTBEAT_FREQUENCY_MS = 5000,
	TEST_MESSAGE_IN_FLIGHT_TIMEOUT = 3000,
	SETUP_ZIGBEE_MS_THROTTLE = 200,
	BIND_POST_DELAY = 1000,
	UNBIND_POST_DELAY = 1000,
	MV_TO_LEVEL_DELAY = 1000,
	REPORTING_POST_DELAY = 200,
	GATEWAY_PROCESS_WATCHDOG_TIMER = 5000,
	OTA_BLOCKSENT_TIMEOUT = 60000, // 60s

	RSSI_LQI_REQ_DEVICE_LIMIT = 5,

	POSTDELAYMS_DEFAULT = 100,
	LINKKEY_START_INDEX = 150,
	CENTRALIZED_SECURITY = 1

};

//mosquitto handler function defination

int onDeviceListReceived(struct mosquitto *mosq, const struct mosquitto_message *message);
int newDeviceJoinedRequest(struct mosquitto *mosq, const struct mosquitto_message *message);
int provisionDevice(struct mosquitto *mosq , const struct mosquitto_message *message);
int onZCLResponse(struct mosquitto *mosq, const struct mosquitto_message *message);
int checkGatewayHeartbeat(struct mosquitto *mosq, const struct mosquitto_message *message);
int discoverDevices(struct mosquitto *mosq, const struct mosquitto_message *message);
int checkSeleneResponse(struct mosquitto *mosq, const struct mosquitto_message *message);
int deviceStateControl(struct mosquitto *mosq, const struct mosquitto_message *message);
int bleShuttingDownCallback(struct mosquitto *mosq, const struct mosquitto_message *message);
int seleneCommandHandler(struct mosquitto *mosq , const struct mosquitto_message *message);
int deviceUpdateHandler(struct mosquitto *mosq , const struct mosquitto_message *message);
void updateDeviceStatus(char * eui64, struct mosquitto * mosq);
void printProvisionedDeviceList();
void startDeviceMonitor();
#endif // __GROWHOUSE_SERVER_H__

/*
 * End growhouse-server.h
 */
