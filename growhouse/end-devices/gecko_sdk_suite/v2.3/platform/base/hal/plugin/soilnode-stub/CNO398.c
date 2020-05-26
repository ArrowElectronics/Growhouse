#include PLATFORM_HEADER
#include <math.h>
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"
#include "spidrv.h"
#include "CNO398.h"

Ecode_t spi_init() {
	SPIDRV_Init_t SPI_config = SPIDRV_MASTER_USART2;
	SPI_config.clockMode = spidrvClockMode3;
	SPI_config.portLocationTx = _USART_ROUTELOC0_TXLOC_LOC18;
	SPI_config.portLocationRx = _USART_ROUTELOC0_RXLOC_LOC16;
	SPI_config.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC17;
	SPI_config.portLocationCs = _USART_ROUTELOC0_CSLOC_LOC17;
	SPI_config.csControl = spidrvCsControlApplication;
	GPIO_PinModeSet(PH_PORT_CS, PH_PIN_CS, gpioModePushPull, 1);

	return SPIDRV_Init(SPI_master_adc, &SPI_config);
}

void spi_deinit() {
	SPIDRV_DeInit(SPI_master_adc);
	GPIO_PinModeSet(PH_PORT_CS, PH_PIN_CS, gpioModeDisabled, 1);
}

void set_reg_str(ad7124_st_reg* reg, uint8_t reg_no) {
	reg->size = ad7124_regs[reg_no].size;
	reg->addr = ad7124_regs[reg_no].addr;
	reg->rw = ad7124_regs[reg_no].rw;
	reg->value = ad7124_regs[reg_no].value;
}

bool adc_verify() {
	uint8_t reg_no = AD7124_ID;
	ad7124_st_reg reg;
	set_reg_str(&reg, reg_no);

	SPI_read_reg(&reg);
	if (reg.value != MANUFACTURE_ID) {
		emberAfCorePrintln("Man id : %u", reg.value);
		return SOIL_FAILURE;
	}
	return SOIL_SUCCESS;
}

Ecode_t SPI_read_reg(ad7124_st_reg* pReg) {
	uint8_t cmd = (ENABLE_COM | READ_IO | REG(pReg->addr)) & 0xFF;
	unsigned char rx_buffer[8] = { 0 };
	Ecode_t returnCode = 1;
	uint8_t count = 0;
	int32_t readValue = 0;

	// Low chip select line
	GPIO_PinOutClear(PH_PORT_CS, PH_PIN_CS);

	// Write address that needs to read
	returnCode = SPIDRV_MTransmitB(SPI_master_adc, (void *) &cmd, sizeof(cmd));
	if (returnCode != ECODE_EMDRV_SPIDRV_OK) {
		*rx_buffer = 0x00;
		return returnCode;
	}

	// Read buffer in rx_buffer
	returnCode = SPIDRV_MReceiveB(SPI_master_adc, (void *) rx_buffer,
			pReg->size);
	if (returnCode != ECODE_EMDRV_SPIDRV_OK) {
		*rx_buffer = 0x00;
		return returnCode;
	}

	readValue = (int32_t) rx_buffer[0];

	// Decoding of rx_buffer
	for (count = 1; count < pReg->size; count++) {
		readValue <<= 8;
		readValue |= rx_buffer[count];
	}

	// High Chip Select line
	GPIO_PinOutSet(PH_PORT_CS, PH_PIN_CS);

	pReg->value = readValue;
	return returnCode;
}

Ecode_t SPI_write_reg(ad7124_st_reg* pReg) {
	uint8_t cmd = (ENABLE_COM | WRITE_IO | REG(pReg->addr)) & 0xFF;
	uint8_t tx_buffer[8] = { 0 };
	uint8_t count = 0;
	Ecode_t returnCode = 1;
	int32_t regValue = pReg->value;

	// Chip select Low
	GPIO_PinOutClear(PH_PORT_CS, PH_PIN_CS);

	// Generate Tx buffer
	tx_buffer[0] = cmd;
	for (count = 0; count < pReg->size; count++) {
		tx_buffer[pReg->size - count] = (uint8_t) (regValue & 0xFF);
		regValue >>= 8;
	}

	// Transmit data
	returnCode = SPIDRV_MTransmitB(SPI_master_adc, (void *) tx_buffer,
			pReg->size + 1);

	// Chip select High
	GPIO_PinOutSet(PH_PORT_CS, PH_PIN_CS);

	return returnCode;
}

Ecode_t adc_init() {
	Ecode_t returnCode = 0;
	uint32_t setValue = 0;
	uint8_t reg_no = 0;
	ad7124_st_reg reg;

	returnCode = adc_init_regs();
	if (returnCode) {
		emberAfCorePrintln("ADC chips default register init failed ; %d",
				returnCode);
		return returnCode;
	}
	emberAfCorePrintln("ADC Chip init done");

	/*
	 * Select Config_1 register - pH
	 */
	reg_no = AD7124_Config_1;
	set_reg_str(&reg, reg_no);

	setValue = 0;
	/* Select bipolar operation */
	setValue |= AD7124_CFG_REG_BIPOLAR;
	/* Burnout current source off */
	setValue |= AD7124_CFG_REG_BURNOUT(0);
	setValue |= AD7124_CFG_REG_REF_BUFP;
	setValue |= AD7124_CFG_REG_REF_BUFM;
	setValue |= AD7124_CFG_REG_AIN_BUFP;
	setValue |= AD7124_CFG_REG_AINN_BUFM;
	/* REFIN1(+)/REFIN1(-). */
	setValue |= AD7124_CFG_REG_REF_SEL(0);
	/* gain1 */
	setValue |= AD7124_CFG_REG_PGA(0);
	setValue &= 0xFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	returnCode = SPI_write_reg(&reg);
	if (returnCode) {
		emberAfCorePrintln("Failed to select config register");
		return returnCode;
	}

	/*
	 * Set Channel_1 register 0x10 for pH
	 */
	reg_no = AD7124_Channel_1; /* pH */
	set_reg_str(&reg, reg_no);

	setValue = 0;
	/* Select setup2 */
	setValue |= AD7124_CH_MAP_REG_SETUP(2);
	/* Set AIN8 as positive input */
	setValue |= AD7124_CH_MAP_REG_AINP(6);
	/* Set gnd as negative input */
	setValue |= AD7124_CH_MAP_REG_AINM(7);
	/* Disable channel */
	setValue &= (~(uint32_t) AD7124_CH_MAP_REG_CH_ENABLE);
	setValue &= 0xFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	returnCode = SPI_write_reg(&reg);
	if (returnCode) {
		emberAfCorePrintln("Failed to set channel 1 for pH");
		return returnCode;
	}

	/*
	 * Set Channel_2 register 0x11 for moisture
	 */
	reg_no = AD7124_Channel_2; /* moisture */
	set_reg_str(&reg, reg_no);

	setValue = 0;
	/* Select setup2 */
	setValue |= AD7124_CH_MAP_REG_SETUP(1);
	/* Set AIN8 as positive input */
	setValue |= AD7124_CH_MAP_REG_AINP(8);
	/* Set gnd as negative input */
	setValue |= AD7124_CH_MAP_REG_AINM(19);
	/* Disable channel */
	setValue &= (~(uint32_t) AD7124_CH_MAP_REG_CH_ENABLE);
	setValue &= 0xFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	returnCode = SPI_write_reg(&reg);
	if (returnCode) {
		emberAfCorePrintln("Failed to set channel 2 for moisture");
		return returnCode;
	}

	/*
	 * Select IO_Control_1 register
	 */
	reg_no = AD7124_IOCon1;
	set_reg_str(&reg, reg_no);
	setValue = 0;

	/* enable AIN3 as digital output */
	setValue |= AD7124_8_IO_CTRL1_REG_GPIO_CTRL2;
	/* enable AIN4 as digital output */
	setValue |= AD7124_8_IO_CTRL1_REG_GPIO_CTRL3;
	/* source ain11 */
	setValue |= AD7124_IO_CTRL1_REG_IOUT_CH0(11);
	/* source ain12 */
	setValue |= AD7124_IO_CTRL1_REG_IOUT_CH1(12);
	/* set IOUT0 current to timeout_conversion500uA */
	setValue |= AD7124_IO_CTRL1_REG_IOUT0(0x4);
	/* set IOUT0 current to 500uA */
	setValue |= AD7124_IO_CTRL1_REG_IOUT1(0x4);
	setValue &= 0xFFFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	returnCode = SPI_write_reg(&reg);
	if (returnCode) {
		emberAfCorePrintln("Failed to set IO control 1");
		return returnCode;
	}

	/*
	 * Set IO_Control_2 register
	 */
	reg_no = AD7124_IOCon2;
	set_reg_str(&reg, reg_no);
	setValue = 0;

	/* enable AIN3 as digital output */
	setValue |= AD7124_8_IO_CTRL2_REG_GPIO_VBIAS7;
	setValue &= 0xFFFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	returnCode = SPI_write_reg(&reg);
	if (returnCode) {
		emberAfCorePrintln("Failed to set IO control 2");
		return returnCode;
	}

	/*
	 * Set ADC_Control 0x01 register
	 */
	/* Select ADC_Control register */
	reg_no = AD7124_ADC_Control;
	set_reg_str(&reg, reg_no);
	setValue = 0;

	/* set data status bit in order to check on which channel the
	 * conversion is
	 */
	setValue |= AD7124_ADC_CTRL_REG_DATA_STATUS;
	/* remove prev mode bits */
	setValue &= 0xFFC3;
	/* standby mode */
	setValue |= AD7124_ADC_CTRL_REG_MODE(2);
	setValue &= 0xFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	returnCode = SPI_write_reg(&reg);
	if (returnCode) {
		emberAfCorePrintln("Failed to set ADC control");
		return returnCode;
	}

	/* set filter register FS[10:0] bits in order to set the FS value to
	 * 192 from default value 384 to reduce the settling time for single
	 * moisture conversion.
	 */
	reg_no = AD7124_Filter_1; /* moisture - setup -1*/
	set_reg_str(&reg, reg_no);
	setValue = 0x0600C0;
	reg.value = setValue;

    /* Write data to ADC */
    returnCode = SPI_write_reg(&reg);
    if (returnCode) {
        emberAfCorePrintln("Failed to set Filter 1 for moisture");
        return returnCode;
    }

    /*
     * Select Config_2 register - pH
     */
    reg_no = AD7124_Config_2;
    set_reg_str(&reg, reg_no);

    setValue = 0;
    /* Select bipolar operation */
    setValue |= AD7124_CFG_REG_BIPOLAR;
    /* Burnout current source off */
    setValue |= AD7124_CFG_REG_BURNOUT(0);
    setValue |= AD7124_CFG_REG_REF_BUFP;
    setValue |= AD7124_CFG_REG_REF_BUFM;
    setValue |= AD7124_CFG_REG_AIN_BUFP;
    setValue |= AD7124_CFG_REG_AINN_BUFM;
    /* REFIN1(+)/REFIN1(-). */
    setValue |= AD7124_CFG_REG_REF_SEL(0);
    /* gain1 */
    setValue |= AD7124_CFG_REG_PGA(0);
    setValue &= 0xFFFF;
    reg.value = setValue;

    /* Write data to ADC */
    returnCode = SPI_write_reg(&reg);
    if (returnCode) {
        emberAfCorePrintln("Failed to select config register");
        return returnCode;
    }

	/* Setting pin mode for Moisture power gpio */
	GPIO_PinModeSet(ADP7118_ENABLE_PORT, ADP7118_ENABLE_PIN, gpioModePushPull, 0);

	/* Disable the channel-0 enable in default configuration */
	disable_channel(0);

	return returnCode;
}

Ecode_t adc_init_regs(void) {
	uint8_t reg_no;
	Ecode_t returnCode = 0;
	for (reg_no = AD7124_Status; reg_no < AD7124_Offset_0; reg_no++) {
		if (ad7124_regs[reg_no].rw == AD7124_RW) {
			returnCode = SPI_write_reg(&ad7124_regs[reg_no]);
			if (returnCode != 0) {
				emberAfCorePrintln("Failed to write init register");
				return returnCode;
			}
		}
	}
	return returnCode;
}

void set_digital_output(uint8_t channel, bool state) {
	uint32_t setValue = 0;
	uint8_t reg_no = AD7124_IOCon1;
	ad7124_st_reg reg;

	set_reg_str(&reg, reg_no);

	SPI_read_reg(&reg);

	setValue = reg.value;
	if (state)
		setValue |= ((AD7124_8_IO_CTRL1_REG_GPIO_DAT1) << channel);
	else
		setValue &= (~(AD7124_8_IO_CTRL1_REG_GPIO_DAT1 << channel));
	setValue &= 0xFFFFFF;
	reg.value = setValue;

	/* Write data to ADC */
	SPI_write_reg(&reg);

	halCommonDelayMilliseconds(1);
}

void enable_channel(uint8_t channel) {
	/* Select ADC_Control register */
	uint8_t reg_no = AD7124_Channel_0 + channel;
	ad7124_st_reg reg;
	set_reg_str(&reg, reg_no);
	SPI_read_reg(&reg);

	uint32_t setValue = reg.value;
	/* Enable channel0 */
	setValue |= (uint32_t) AD7124_CH_MAP_REG_CH_ENABLE;
	setValue &= 0xFFFF;
	/* Write data to ADC */
	reg.value = setValue;
	SPI_write_reg(&reg);

	halCommonDelayMilliseconds(1);
}

void disable_channel(uint8_t channel) {
	/* Select ADC_Control register */
	uint8_t reg_no = AD7124_Channel_0 + channel;
	ad7124_st_reg reg;
	set_reg_str(&reg, reg_no);
	SPI_read_reg(&reg);

	uint32_t setValue = reg.value;
	/* Enable channel0 */
	setValue &= (~(uint32_t) AD7124_CH_MAP_REG_CH_ENABLE);
	setValue &= 0xFFFF;
	/* Write data to ADC */
	reg.value = setValue;
	SPI_write_reg(&reg);

	halCommonDelayMilliseconds(1);
}

Ecode_t start_single_conversion() {
	Ecode_t returnCode;
	ad7124_st_reg reg;
	uint8_t reg_no = AD7124_ADC_Control;

	set_reg_str(&reg, reg_no);

	uint16_t regValue = 0x0584;
	regValue &= 0xFFFF;
	reg.value = regValue;

	returnCode = SPI_write_reg(&reg);
	if (returnCode)
		emberAfCorePrintln("Failed to start single conversion");


	return returnCode;
}

bool wait_conversion_complete() {
	int8_t ready = 0;
	uint8_t reg_no = AD7124_Status;
	ad7124_st_reg reg;
	uint32_t regValue = 0;
	uint32_t timeout = CONV_TIMEOUT;

	set_reg_str(&reg, reg_no);


	while (!ready && --timeout) {
		SPI_read_reg(&reg);
		regValue = reg.value;
		ready = ((regValue & AD7124_STATUS_REG_RDY) == 0);
	}
	return timeout ? 0 : 1;
}

uint32_t adc_read_channel(uint8_t channel) {
	uint32_t regValue = -1;
	ad7124_st_reg reg;
	uint8_t reg_no = AD7124_Data;

	set_reg_str(&reg, reg_no);

	/* enable channel */
	enable_channel(channel);

	if(channel == PH_CHANNEL)
	{
		halCommonDelayMilliseconds(80);
	}

	if (start_single_conversion()) {
		return regValue;
	}

	if (wait_conversion_complete()) {
		/* timed out */
		return regValue;
	}

	/* Read channel value */
	SPI_read_reg(&reg);
	regValue = reg.value;

	/* disable channel */
	disable_channel(channel);
	return regValue;
}

float data_to_voltage_bipolar(uint32_t data, uint8_t gain, float VREF) {
	data = data & 0xFFFFFF;
	return ((data / (float) (0xFFFFFF / 2)) - 1) * (VREF / gain);
}

float ADC_read_ph_value(void) {
	float pH = 0;
	int32_t data = 0;
	float volt = 0;
	emberAfCorePrintln("Reading Ph value");
	// Assuming room temperature TODO
	int temperature = 27;

	set_digital_output(PH_CHANNEL, true);

	/* read data from pH channel */
	data = adc_read_channel(PH_CHANNEL);

	volt = data_to_voltage_bipolar(data, 1, 3.3);

	pH = PH_ISO - ((volt - ZERO_POINT_TOLERANCE)
					/ ((2.303 * AVOGADRO * (temperature + KELVIN_OFFSET)) /
					FARADAY_CONSTANT));

	emberAfCorePrintln("pH volt(mVx10) : %d, pH : %d", (int32_t)(volt * 10000), (int32_t)(pH * 100));

	set_digital_output(PH_CHANNEL, false);

	return pH;
}

float ADC_read_moisture_value(void) {
	float moisture = 0;
	int32_t data = 0;
	float volt = 0;
	float moisture_mineral = 0;
	emberAfCorePrintln("Reading moisture value");

	/* Enable moisture power */
	GPIO_PinOutSet(ADP7118_ENABLE_PORT, ADP7118_ENABLE_PIN);

	set_digital_output(MOISTURE_CHANNEL, true);

	/* read data from pH channel */
	data = adc_read_channel(MOISTURE_CHANNEL);

	volt = data_to_voltage_bipolar(data, 1, 3.3);

	/* To measure moisture reading given by EC-5 moisture probe */
	//Potting Soil equation
	moisture = ( (0.00211 * (volt * 1000)) - 0.675) * 100;

	//Mineral Soil equation
	moisture_mineral = ((0.00119 * (volt*1000)) - 0.401) *100;

	emberAfCorePrintln("Moisture volt(mVx10) : %d, Mineral : %d, Potting : %d", (int32_t)(volt * 10000), (int32_t)(moisture_mineral * 100), (int32_t)(moisture * 100));

	set_digital_output(MOISTURE_CHANNEL, false);

	/* Disable moisture power */
	GPIO_PinOutClear(ADP7118_ENABLE_PORT, ADP7118_ENABLE_PIN);

	return moisture;
}
