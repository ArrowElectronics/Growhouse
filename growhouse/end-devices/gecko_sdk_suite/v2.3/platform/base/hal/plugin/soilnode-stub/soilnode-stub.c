//   Copyright 2015 Silicon Laboratories, Inc.                              *80*

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"
#include "CNO398.h"

#ifdef EMBER_AF_PLUGIN_BATTERY_MONITOR
extern void emberAfPluginBatteryMonitorReadADCEventHandler(void);
#endif

extern void halPhMoistureReadingCompleteCallback(int16_t pH, int16_t moisture, bool readSuccess);

static bool init_done = false;

void emberAfPluginSoilnodeStubInitCallback(void)
{
	Ecode_t ret = 0;
	// Initialization of SPI bus
	ret = spi_init();
	if (ret)
	{
		emberAfCorePrintln("SPI Initialization failed Return : %d",ret);
		init_done = false;
		spi_deinit();
		return;
	}
	emberAfCorePrintln("SPI Initialization Success");

	/* Detection of ADC chip */
	ret = adc_verify();
	if (ret)
	{
		emberAfCorePrintln("ADC ID read failed. ADC not detected. Communication failure ? Return : %d",ret);
		spi_deinit();
		init_done = false;
		return;
	}
	emberAfCorePrintln("ADC Chip Detected");

	/* Initialization of ADC for pH and moisture TODO if RTD required*/
	ret = adc_init();
	if (ret) {
		emberAfCorePrintln("ADC Initialization failed Return : %d",ret);
		spi_deinit();
		init_done = false;
		return;
	}

	init_done = true;
}

void halPhMoistureStartRead()
{
	bool status 	= false;
	float pH		= 0;
	float moisture 	= 0;

	if(!init_done)
		emberAfPluginSoilnodeStubInitCallback();

	if (init_done) {
		pH = ADC_read_ph_value();
		/*
		 * To maintain float value multiplying pH with 100,
		 * To get actual value at gateway side we will divide
		 * received pH value over zigBee by 100.
		 */
		pH = pH * 100;
		emberAfCorePrintln("pH is : %d", (int16_t )(pH)); emberAfCoreFlush();
		emberAfCoreFlush();

		moisture = ADC_read_moisture_value();
		/*
		 * To maintain float value multiplying moisture with 100,
		 * To get actual value at gateway side we will divide
		 * received moisture value over zigBee by 100.
		 */
		moisture = moisture * 100;
		emberAfCorePrintln("Moisture is : %d",(int16_t)moisture);
		emberAfCoreFlush();

		status = true;
	} else {
		emberAfPluginSoilnodeStubInitCallback();
		status = false;
	}

	halPhMoistureReadingCompleteCallback ( (int16_t)pH, (int16_t)moisture, status );

	/* Read the Battery Voltage and Report to zigBee coordinator
	 * over Power configuration cluster */
	emberAfPluginBatteryMonitorReadADCEventHandler();
	spi_deinit();
	init_done = false;
}
