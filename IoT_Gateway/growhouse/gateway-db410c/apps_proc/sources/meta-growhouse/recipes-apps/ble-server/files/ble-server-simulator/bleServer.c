/*
 * bleServer.c
 * Copyright(c) 2018
 */

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "mosquitto.h"
#include "cJSON.h"

#define mqtt_host "localhost"
#define mqtt_port 1883

#define MQTT_PUB_DISCOVER_COMMAND       	"ble/gw/discoverCommand"
#define MQTT_PUB_PROVISIONED_DEVICE     	"ble/gw/provisionedDevice"
#define MQTT_SUB_DISCOVERED_DEVICE      	"gw/ble/discoveredDevice"
#define MQTT_SUB_BLEAPP_DISCOVER_COMMAND	"bleapp/bleserver/discovercommad"
#define MQTT_SUB_SELENE_PROPERTY		"bleapp/bleserver/seleneProperties"

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("connect callback, rc=%d\n", result);
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	bool match = 0;
	printf("Mqtt Receive : %d messaeg : %s for topic '%s'\n",
			message->payloadlen, (char*) message->payload, message->topic);

	mosquitto_topic_matches_sub(MQTT_SUB_DISCOVERED_DEVICE,
			message->topic, &match);
	if (match) {

		int fd = open("/home/root/payload", O_RDWR | O_CREAT | O_APPEND, 0666);
		if (fd < 0) {
			perror("open : ");
			return -1;
		}

		write(fd, message->payload, message->payloadlen);
		write(fd, "\n", 1);

		close(fd);
	}

	mosquitto_topic_matches_sub(MQTT_SUB_BLEAPP_DISCOVER_COMMAND,
			message->topic, &match);
	if (match) {
		mosquitto_publish(mosq, NULL, MQTT_PUB_DISCOVER_COMMAND, message->payloadlen, message->payload, 1, false);
	}

	mosquitto_topic_matches_sub(MQTT_SUB_SELENE_PROPERTY,
			message->topic, &match);
	if (match) {

		FILE * SeleneProperties = fopen ("/opt/selene/config/devices/self.properties","w+");
		if ( SeleneProperties == NULL ) {
			printf ("File not open\n");
			exit(1);
		}

		cJSON *node = cJSON_Parse(message->payload);

		char * deviceClass = cJSON_GetObjectItem(node, "deviceClass")->valuestring;
		fprintf (SeleneProperties, "deviceClass=%s\n", deviceClass);

		char * name = cJSON_GetObjectItem(node, "name")->valuestring;
		fprintf (SeleneProperties, "name=%s\n", name);

		char * uid = cJSON_GetObjectItem(node, "uid")->valuestring;
		fprintf (SeleneProperties, "uid=%s\n", uid);

		char * apikey = cJSON_GetObjectItem(node, "apikey")->valuestring;
		fprintf (SeleneProperties, "apiKey=%s\n", apikey);

		char * secretkey = cJSON_GetObjectItem(node, "secretkey")->valuestring;
		fprintf (SeleneProperties, "secretKey=%s\n", secretkey);

		char * iotConnectUrl = cJSON_GetObjectItem(node, "iotConnectUrl")->valuestring;
		fprintf (SeleneProperties, "iotConnectUrl=%s\n", iotConnectUrl);

		char * iotConnectMqtt = cJSON_GetObjectItem(node, "iotConnectMqtt")->valuestring;
		fprintf (SeleneProperties, "iotConnectMqtt=%s\n", iotConnectMqtt);

		char * enabled = cJSON_GetObjectItem(node, "enabled")->valuestring;
		fprintf (SeleneProperties, "enabled=%s\n", enabled);

		char * cloudTransferMode = cJSON_GetObjectItem(node, "cloudTransferMode")->valuestring;
		fprintf (SeleneProperties, "cloudTransferMode=%s\n", cloudTransferMode);

		char * cloudBatchSendingIntervalMs = cJSON_GetObjectItem(node, "cloudBatchSendingIntervalMs")->valuestring;
		fprintf (SeleneProperties, "cloudBatchSendingIntervalMs=%s\n", cloudBatchSendingIntervalMs);

		char * heartBeatIntervalMs = cJSON_GetObjectItem(node, "heartBeatIntervalMs")->valuestring;
		fprintf (SeleneProperties, "heartBeatIntervalMs=%s\n", heartBeatIntervalMs);

		char * iotConnectMqttVHost = cJSON_GetObjectItem(node, "iotConnectMqttVHost")->valuestring;
		fprintf (SeleneProperties, "iotConnectMqttVHost=%s\n", iotConnectMqttVHost);

		char * gatewayHid = cJSON_GetObjectItem(node, "hid")->valuestring;
		fprintf (SeleneProperties, "hid=%s\n", gatewayHid);

		// TODO :: add logic for db delete and restart selene
		//         instead of system command
		system("rm -rf /opt/selene/db/*");
		system("rm -rf /opt/selene/databus/*");
		system("rm -rf /opt/selene/log/*");
		system("systemctl restart selene");

		fclose(SeleneProperties);
		cJSON_Delete(node);
	}
}

int main(int argc, char *argv[])
{
	char clientid[24];
	struct mosquitto *mosq;
	int rc = 0;

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "bleServer_%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);
	if(!mosq){
		printf ("failed to start mosquitto\n");
		mosquitto_lib_cleanup();
		return -1;
	}
	if( access("/home/root/payload", F_OK ) != -1 )
		remove("/home/root/payload");

	if(mosq){
		mosquitto_connect_callback_set(mosq, connect_callback);
		mosquitto_message_callback_set(mosq, message_callback);

		// TODO: fix 3600
		if (mosquitto_connect(mosq, mqtt_host, mqtt_port, 3600)){
			mosquitto_lib_cleanup();
			printf("Unable to connect Mosquitto.\n");
			return 1;
		}
		mosquitto_subscribe(mosq, NULL, MQTT_SUB_DISCOVERED_DEVICE, 0);
		mosquitto_subscribe(mosq, NULL, MQTT_SUB_BLEAPP_DISCOVER_COMMAND, 0);
		mosquitto_subscribe(mosq, NULL, MQTT_SUB_SELENE_PROPERTY, 0);

		while(1){
			rc = mosquitto_loop(mosq, -1, 1);
			if(rc){
				printf("connection error!\n");
				sleep(10);
				mosquitto_reconnect(mosq);
			}
		}
		mosquitto_destroy(mosq);
	}

	mosquitto_lib_cleanup();

	return rc;
}

/*
 * End bleServer.c
 */
