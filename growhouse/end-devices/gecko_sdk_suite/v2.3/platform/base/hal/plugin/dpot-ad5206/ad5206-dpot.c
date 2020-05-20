/*
 * ad5206-dopt.c
 *
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 *
 *  Updated on: 28-Dec-2018
 *  	- Added support to configure LED six Channel dynamically.
 *  	- Added support to Send LED six channel configuration to co-ordinator
 *  	  and BLE mesh module.
 *  	- Added support to store LED six channel configuration and intensity
 *        level into Flash storage.
 *      Author: einfochips
 */

/***************************************************************************/
/**
 *   @file   ad5206-dpot.c
 *   @brief  Callback implement for AD5206 init and API to set LED intensity
 * 	     level when level receive from co-ordinator.
 *
 ***************************************************************************/

/***************************************************************************/
/* Include Files                                                           */
/***************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"
#include EMBER_AF_API_DPOT_AD5206
#include EMBER_AF_API_EEPROM

/******************************************************************************/
/* Macros                                                           */
/******************************************************************************/

#define PAGE_SIZE                   4096
#define EEPROM_SIZE					524288
#define LED_CONFIGURATION_ADDR      EEPROM_SIZE - PAGE_SIZE
//#define LED_CONFIGURATION_ADDR      0
//#define DEBUGMODE_ON

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
static bool init_done = false;

bool isDefaultConfigurationSet;
bool isActualConfigurationSet;

LED_Configuration Default_LED_Configuration =
{
	0x55, 0x00, 0x00, {'R', 'G', 'B', 'R', 'G', 'B'}, {50, 50, 50, 50, 50, 50}
};

/*
 * Event to Send Channel configuration
 */
EmberEventControl emberAfPluginDpotAd5206SendChannelConfigEventControl;
void emberAfPluginDpotAd5206SendChannelConfigEventHandler(void);

/*
 * Event to blink LEDs channel for 3 seconds
 */
EmberEventControl emberAfPluginDpotAd5206BlinkEventControl;
void emberAfPluginDpotAd5206BlinkEventHandler(void);

extern void ConfigureLEDchannel_FromFlash(void);
extern struct LED_Configuration Current_LED_Configuration;
/***************************************************************************/
/**
 * @brief Print LED configuration and intensity data block
 *
 * @param LED configuration block.
 *
 * @return None.
 ***************************************************************************/
static void printLEDconfigurationBlock(const struct LED_Configuration *block)
{
#ifdef DEBUGMODE_ON
	int loop = 0;

	emberAfCorePrintln("Is LED Default configuration Done : %s", (block->Is_LED_Configuration_Set == 0x55) ? "True" : "False" );
	emberAfCorePrintln("Is LED Actual configuration Done : %s", (block->Is_LED_Configuration_Set == 0xAA) ? "True" : "False" );

	for(loop = 0; loop < LED_CHANNELS; loop++)
	{
		emberAfCorePrintln("LED Configuration for %d Channel: %d, Data : %d", loop+1,
								block->LED_Configuration_Data[loop],
								block->LED_Channel_intensity[loop]);
	}

#endif
	return;
}

/***************************************************************************/
/**
 * @brief Print LED configuration
 *
 * @param LED configuration block.
 *
 * @return None.
 ***************************************************************************/
static void printLEDconfiguration(const struct LED_Configuration *block)
{
	emberAfCorePrintln("LED Configuration for Channel 1 to 6 : %c %c %c %c %c %c",
									block->LED_Configuration_Data[0],
									block->LED_Configuration_Data[1],
									block->LED_Configuration_Data[2],
									block->LED_Configuration_Data[3],
									block->LED_Configuration_Data[4],
									block->LED_Configuration_Data[5]);
}

/***************************************************************************/
/**
 * @brief Callback to initialize spi for AD5206 chip
 *
 * @param None.
 *
 * @return None.
 ***************************************************************************/
void emberAfPluginDpotAd5206InitCallback(void)
{
	Ecode_t ret = 0;
	int loop = 0;
	struct LED_Configuration Temp_LED_Configuration;

	isActualConfigurationSet = false;
	isDefaultConfigurationSet = false;

	// Initialization of SPI bus
	ret = spi_init();
	if (ret) {
		emberAfCorePrintln("SPI Initialization failed Return : %d",ret);
		init_done = false;
		spi_deinit();
		return;
	}
	emberAfCorePrintln("SPI Initialization Success");

	/*
	 * 	Storing LED configuration data into Flash. Structure for storing LED data is as below:
	 *
	 *  Starting from address 524288, We will store LED configuration data.
	 *
	 *  Byte	: Data
	 *  524288	: If 0x55 than LED configuration is set as default value.
	 *  		  If 0xAA than LED configuration is set by USER using either
	 *  		  BLE provision app or using BLE mesh application.
	 *  -----------------------------------------------------
	 *  524289	: LED1/Channel1 Configuration data
	 *  524290	: LED2/Channel2 Configuration data
	 *  	.	:	.
	 *  	.	:	.
	 *  524294	: LED6/Channel6 Configuration data
	 *  -----------------------------------------------------
	 *  524295	: LED1 Intensity Level
	 *  524296	: LED2 Intensity Level
	 *  	.	:	.
	 *  	.	:	.
	 *  524300	: LED6 Intensity Level
	 */

	/* Read configuration flag, Is configuration already set or not */
	emberAfPluginEepromRead( LED_CONFIGURATION_ADDR, (uint8_t *)&Temp_LED_Configuration , sizeof(LED_Configuration) );

	/* Consider to set Default configuration if ZigBee and BLE both are not provisioned */
	if(Temp_LED_Configuration.Is_ZIGBEE_Provisioned == 0x00 && Temp_LED_Configuration.Is_BLE_Provisioned == 0x0)
	{
		emberAfCorePrintln("Default Configuration is set.");
		isDefaultConfigurationSet = true;
	}
	else if( Temp_LED_Configuration.Is_ZIGBEE_Provisioned != 0x0 || Temp_LED_Configuration.Is_BLE_Provisioned != 0x0 )
	{
		emberAfCorePrintln("Actual Configuration is set.");
		isActualConfigurationSet = true;
	}

	/* Do nothing is Actual configuration is already set */
	if (!isActualConfigurationSet) {
		/* If Defaut configuration is also not set then update default configuration */
		if (!isDefaultConfigurationSet)
		{
			emberAfCorePrintln("Configuration is not set.");
			updateLED_Configuration_intoFlash(&Default_LED_Configuration);
			isDefaultConfigurationSet = true;
		}
	}

	emberAfPluginEepromRead(LED_CONFIGURATION_ADDR, (uint8_t *)&Temp_LED_Configuration, sizeof(LED_Configuration));
	printLEDconfiguration(&Temp_LED_Configuration);

	/* Set initial configuration value into current configuration structure */
	memcpy((uint8_t *)&Current_LED_Configuration, (uint8_t *)&Temp_LED_Configuration, sizeof(LED_Configuration));

	init_done = true;

	/* After reset, Initialize the Intensity as per previous value or Default value */
	for(loop=0; loop < LED_CHANNELS; loop++)
	{
		halAD5204DriverSetLedLevel(loop+1, Temp_LED_Configuration.LED_Channel_intensity[loop]);
	}

	/* set the initial BLE provision state to unprovision,
	 * as when the BLE module initialize it will send the actual state,
	 * and if the LED Node will not have the BLE module connected,
	 * then setting this BLE provision state true will discard the,
	 * dependency on BLE module as it's not available.
	 */
	Current_LED_Configuration.Is_BLE_Provisioned = 0x00;
}

/***************************************************************************/
/**
 * @brief API to set LED Configuration for appropriate channel
 *
 * @param Pointer of LED configuration block
 *
 ***************************************************************************/
void updateLED_Configuration_intoFlash(struct LED_Configuration *LED_ConfigurationBlock)
{
	struct LED_Configuration Temp_LED_Configuration;

	emberAfCorePrintln("updateLED_Configuration_intoFlash.");
	printLEDconfigurationBlock(LED_ConfigurationBlock);
	printLEDconfiguration(LED_ConfigurationBlock);

	memset(&Temp_LED_Configuration, 0, sizeof(LED_Configuration));

	/* Updating Current configuration structure whenever flash update request called */
	memcpy((uint8_t *)&Current_LED_Configuration, LED_ConfigurationBlock, sizeof(LED_Configuration));

	/* Erasing previous configuration */
	emberAfPluginEepromErase(LED_CONFIGURATION_ADDR, PAGE_SIZE);

	/* Writing New configuration */
	emberAfPluginEepromWrite( LED_CONFIGURATION_ADDR, (uint8_t *)LED_ConfigurationBlock, sizeof(LED_Configuration));

	/* Reading it back */
	emberAfPluginEepromRead(LED_CONFIGURATION_ADDR, (uint8_t *)&Temp_LED_Configuration, sizeof(LED_Configuration));

	if(memcmp(LED_ConfigurationBlock, (uint8_t *)&Temp_LED_Configuration, sizeof(LED_Configuration)) == 0)
	{
		emberAfCorePrintln("LED Configuration updated.");
		printLEDconfigurationBlock(&Temp_LED_Configuration);
	}
	else
	{
		emberAfCorePrintln("Failed to update LED configuration into flash memory");
	}
	if(LED_ConfigurationBlock->Is_LED_Configuration_Set == 0xAA)
	{
		isDefaultConfigurationSet = false;
		isActualConfigurationSet = true;
	}
	else if(LED_ConfigurationBlock->Is_LED_Configuration_Set == 0x55)
	{
		isDefaultConfigurationSet = true;
		isActualConfigurationSet = false;
	}
}

/***************************************************************************/
/**
 * @brief API to get LED Configuration for appropriate channel
 *
 * @param Pointer of LED configuration block
 *
 ***************************************************************************/
void getLED_configuration_FromFlash(struct LED_Configuration *LED_ConfigurationBlock)
{
	if(isActualConfigurationSet || isDefaultConfigurationSet)
	{
		emberAfPluginEepromRead(LED_CONFIGURATION_ADDR, (uint8_t *)LED_ConfigurationBlock, sizeof(LED_Configuration));
		printLEDconfigurationBlock(LED_ConfigurationBlock);
	}
}

/***************************************************************************/
/**
 * @brief API to set LED level for appropriate channel
 *
 * @param Channel: Digital potentiometer channel (1 to 6)
 * 	  value:   LED intensity level from 0 to 100
 *
 * @return Ecode_t: Error code
 ***************************************************************************/
Ecode_t halAD5204DriverSetLedLevel(uint8_t Channel, uint8_t value)
{
	Ecode_t status = false;
	uint8_t temp_value = value;
	struct LED_Configuration Temp_LED_Configuration;

	// Value Check, it should be less than or equal to 100
	if (value > MAX_LED_LEVEL)
		value = MAX_LED_LEVEL;

	// Mapping of LED level 0 to 100 into RDACs level 0 to 255.
	value = (int)(value * RDAC_MAPPING);

	if (init_done) {
		switch (Channel) {
		case DPOT_RDAC_CHANNEL1:
			/* LED number 1 is mapped with Channel 4(w4) */
			status = set_led_level(RDAC3_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL2:
			/* LED number 2 is mapped with Channel 2(w2) */
			status = set_led_level(RDAC1_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL3:
			/* LED number 3 is mapped with Channel 1(w1) */
			status = set_led_level(RDAC0_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL4:
			/* LED number 4 is mapped with Channel 3(w3) */
			status = set_led_level(RDAC2_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL5:
			/* LED number 5 is mapped with Channel 5(w5) */
			status = set_led_level(RDAC4_ADDR, value);
			break;

		case DPOT_RDAC_CHANNEL6:
			/* LED number 6 is mapped with Channel 6(w6) */
			status = set_led_level(RDAC5_ADDR, value);
			break;
		default:
			emberAfCorePrintln("Maximum only 6 channel supported");
			status = false;
			break;
		}
	} else {
		emberAfPluginDpotAd5206InitCallback();
	}

	/* START: Read and Update LED intensity level into flash */
	if (isActualConfigurationSet || isDefaultConfigurationSet)
	{
		// Read value from flash
		emberAfPluginEepromRead(LED_CONFIGURATION_ADDR, (uint8_t *)&Temp_LED_Configuration,
				sizeof(Temp_LED_Configuration));

		if (Temp_LED_Configuration.LED_Channel_intensity[Channel-1] != temp_value)
		{
			// Update into structure
			Temp_LED_Configuration.LED_Channel_intensity[Channel-1] = temp_value;
			updateLED_Configuration_intoFlash(&Temp_LED_Configuration);
		}
	}
	/* END: Read and Update LED intensity level into flash */

	return status;
}

/***************************************************************************/
/**
 * @brief handler to send LED Configuration to co-ordinator from Flash data
 *
 * @param None
 *
 * @return None
 *
 ***************************************************************************/
void emberAfPluginDpotAd5206SendChannelConfigEventHandler(void)
{
	emberEventControlSetInactive(emberAfPluginDpotAd5206SendChannelConfigEventControl);

	/*
	 * Check if device has joined to any network.
	 * If joined than send LED channel configuration data
	 */
	if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
		ConfigureLEDchannel_FromFlash();
		emberAfCorePrintln("LED Configuration data sent to Co-ordinator");
	}
}

/***************************************************************************/
/**
 * @brief API to Schedule LED configuration Send event to Co-ordinator
 *
 * @param None
 *
 * @return None
 *
 ***************************************************************************/
void scheduleLEDconfiguraionsendEvent(void) {

	/*
	 * Bug solving change:
	 * Scenario:
	 * ---------
	 * -> When LED node end device has joined with ZC and ZC
	 * was waiting for ACTIVE ENDPOIN RESPONSE to add LED node
	 * into device table.
	 * During this waiting period, device table entry for LED was
	 * not created for LED node but LED node have sent LED channel
	 * configuration value to ZC.
	 * So that LED channel configuration data at ZC was ignored.
	 *
	 * RootCause:
	 * ----------
	 * We have set schedule time to send LED configuration data to
	 * ZC was 1 second after LED node joined to ZC.
	 * And we found that some time ACTIVE ENDPOINT RESPONSE of LED
	 * node take time due to network congestion.
	 * And before ZC receives ACTIVE ENDPOINT RESPONSE from LED node
	 * it receives LED configuration data firstly.
	 *
	 * Solution:
	 * ---------
	 * Increase in schedule time have solved this issue,
	 *
	 */
	emberEventControlSetDelayMS( emberAfPluginDpotAd5206SendChannelConfigEventControl, 3000);
}

/***************************************************************************/
/**
 * @brief Handler to Blink LEDs for 3 seconds.
 *
 * @param None
 *
 * @return None
 *
 ***************************************************************************/
void emberAfPluginDpotAd5206BlinkEventHandler(void)
{

#define LED_BLINKING_COUNT 5
#define LED_BLINKING_ON_OFF_DELAY_MS 1000

	uint8_t channel_loop = 0;
	static uint8_t count = 0;
	emberEventControlSetInactive(emberAfPluginDpotAd5206BlinkEventControl);

	if(count %2 == 0)
	{
		/* LEDs On*/
		for(channel_loop = 0; channel_loop < LED_CHANNELS; channel_loop++)
			set_led_level(channel_loop, 255);

	} else {
		/* LEDs On*/
		for(channel_loop = 0; channel_loop < LED_CHANNELS; channel_loop++)
			set_led_level(channel_loop, 0);
		/* LEDs Off */
	}
	count ++;

	/* If count reached to max limit than stop blinking schedule */
	if(count > LED_BLINKING_COUNT)
	{
		count = 0;
		return;
	}

	emberEventControlSetDelayMS(
			emberAfPluginDpotAd5206BlinkEventControl,
				LED_BLINKING_ON_OFF_DELAY_MS);
}

/***************************************************************************/
/**
 * @brief API to Schedule LED Blinking
 *
 * @param None
 *
 * @return None
 *
 ***************************************************************************/
void StartLED_blinking(void) {
	/*
	 * Schedule LED Blinking event
	 */
	emberEventControlSetDelayMS( emberAfPluginDpotAd5206BlinkEventControl, 100);
}
