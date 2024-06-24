/*
 * bleApp.c
 * Copyright(c) 2018
 */

#include <stdio.h>
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "mosquitto.h"

struct mosquitto *mosq;

#define mqtt_host "localhost"
#define mqtt_port 1883

#define MQTT_PUB_BLEAPP_DISCOVER_COMMAND        "bleapp/bleserver/discovercommad"
#define MQTT_PUB_SELENE_PROPERTY                "bleapp/bleserver/seleneProperties"
#define MQTT_PUB_PROVISIONED_DEVICE             "ble/gw/provisionedDevice"

// discovery comman payload
char * startDiscoveryCommand = "{\"discoverDevices\":1}";
char * stopDiscoveryCommand = "{\"discoverDevices\":0}";

int main(int argc, const char * argv[]) {

	int decision = 0;
	int value = 0;
	char buffer[512] = { 0 };
	char command[512] = { 0 };
	char line[1024] = { 0 };
	char clientid[24] = { 0 };
	int val =0;
	int read = 0;
	unsigned int len = 0;

	mosquitto_lib_init();

	memset(clientid, 0, 24);
	snprintf(clientid, 23, "bleApp_%d", getpid());
	mosq = mosquitto_new(clientid, true, 0);
	if(!mosq){
		printf ("failed to start mosquitto\n");
		mosquitto_lib_cleanup();
		return -1;
	}

	// TODO: fix 3600
	if(mosquitto_connect(mosq, mqtt_host, mqtt_port, 3600)){
		printf ("failed to connect to mosquitto\n");
		mosquitto_lib_cleanup();
		return -1;
	}

	while (1) {

		printf("1. Send Discover command\n2. Provide Gateway Information to selene\n3. Provision device\n4. Exit\nSelect :");
		scanf("%d", &value);

		switch (value) {

		case 1: {

			printf ("\n1. Start discovery\n2. Stop discovery\nSelect : ");
			scanf("%d", &val);

			if ( val == 1)
				mosquitto_publish(mosq, NULL, MQTT_PUB_BLEAPP_DISCOVER_COMMAND,
						strlen(startDiscoveryCommand), startDiscoveryCommand, 1, false);
			else
				mosquitto_publish(mosq, NULL, MQTT_PUB_BLEAPP_DISCOVER_COMMAND,
						strlen(stopDiscoveryCommand), stopDiscoveryCommand, 1, false);
			}
			break;

		case 2: {

			cJSON * SeleneService = cJSON_CreateObject();

			cJSON_AddStringToObject(SeleneService, "deviceClass",
					"com.arrow.selene.device.self.SelfModule");

			printf("\nGateway Name\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "name", buffer);
			memset(buffer, 0x0, len);

			printf("\nGateway UID\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "uid", buffer);
			memset(buffer, 0x0, len);

			printf("\nAPI KEY\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "apikey", buffer);
			memset(buffer, 0x0, len);

			printf("\nSECRET KEY\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "secretkey", buffer);
			memset(buffer, 0x0, len);

			printf("\nIot connect URL\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "iotConnectUrl", buffer);
			memset(buffer, 0x0, len);

			printf("\nIot connect MQTT URL\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "iotConnectMqtt", buffer);
			memset(buffer, 0x0, len);

			printf("\nIot connect MQTT HOST\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "iotConnectMqttVHost", buffer);
			memset(buffer, 0x0, len);

			printf("\nGateway HID\n");
			fflush(stdin);
			scanf("%s", buffer);
			cJSON_AddStringToObject(SeleneService, "hid", buffer);
			memset(buffer, 0x0, len);

			cJSON_AddStringToObject(SeleneService, "enabled", "true");
			cJSON_AddStringToObject(SeleneService, "cloudTransferMode",
					"BATCH");
			cJSON_AddStringToObject(SeleneService,
					"cloudBatchSendingIntervalMs", "500");
			cJSON_AddStringToObject(SeleneService, "heartBeatIntervalMs",
					"60000");

			printf("node == %s\n", cJSON_Print(SeleneService));

			mosquitto_publish(mosq, NULL,
					MQTT_PUB_SELENE_PROPERTY,
					strlen(cJSON_PrintUnformatted(SeleneService)),
					cJSON_PrintUnformatted(SeleneService), 1, false);
		}
			break;

		case 3: {

			FILE* file = fopen("/home/root/payload", "r");
			if (file == NULL) {
				printf("file not found\n");
				exit(1);
			}

			cJSON *root;
			root = cJSON_CreateArray();

			while (fgets(line, sizeof(line), file)) {

				// First, parse the whole thing
				cJSON *node = cJSON_Parse(line);
				// Let's get some values
				char *name =
						cJSON_GetObjectItem(node, "deviceType")->valuestring;
				printf("device type = %s\n", name);

				name = cJSON_GetObjectItem(node, "eui64")->valuestring;
				printf("eui64 = %s\n", name);

				printf("\ndecision : 1/0\n");
				scanf("%d", &decision);
				cJSON_AddBoolToObject(node, "decision", decision);

				printf("\nSet Device Name :\n");
				scanf("%s", buffer);
				cJSON_AddStringToObject(node, "deviceName", buffer);
				cJSON_AddItemToArray(root, node);
			}
			printf("Publish Provision Device List :: \n %s\n",
					cJSON_Print(root));
			mosquitto_publish(mosq, NULL,
					MQTT_PUB_PROVISIONED_DEVICE,
					strlen(cJSON_PrintUnformatted(root)),
					cJSON_PrintUnformatted(root), 1, false);

			cJSON_Delete(root);

			if( access("/home/root/payload", F_OK ) != -1 )
				remove("/home/root/payload");

			fclose(file);

		}
			break;

		case 4: {
			mosquitto_lib_cleanup();
			exit(0);
		}
		default:
			break;
		}
	}
	return 0;
}

/*
 * End bleApp.c
 */
