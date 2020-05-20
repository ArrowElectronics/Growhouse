#ifndef CNO398_H
#define CNO398_H

#include "spidrv.h"
#include "hal-config.h"

#define ZERO_POINT_TOLERANCE (0.003)
#define PH_ISO (7)
#define AVOGADRO (8.314)
#define FARADAY_CONSTANT (96485.0)
#define KELVIN_OFFSET (273.1)
#define SENSOR_SETTLING_TIME  400

#define MANUFACTURE_ID 0x14

#define ENABLE_COM (0 << 7)
#define READ_IO (1 << 6)
#define WRITE_IO (0 << 6)
#define REG(x) ((x) & 0x3f)

/* Status Register bits */
#define AD7124_STATUS_REG_RDY          (1 << 7)
#define AD7124_STATUS_REG_ERROR_FLAG   (1 << 6)
#define AD7124_STATUS_REG_POR_FLAG     (1 << 4)
#define AD7124_STATUS_REG_CH_ACTIVE(x) ((x) & 0xF)

/* ADC_Control Register bits */
#define AD7124_ADC_CTRL_REG_DOUT_RDY_DEL   (1 << 12)
#define AD7124_ADC_CTRL_REG_CONT_READ      (1 << 11)
#define AD7124_ADC_CTRL_REG_DATA_STATUS    (1 << 10)
#define AD7124_ADC_CTRL_REG_CS_EN          (1 << 9)
#define AD7124_ADC_CTRL_REG_REF_EN         (1 << 8)
#define AD7124_ADC_CTRL_REG_POWER_MODE(x)  (((x) & 0x3) << 6)
#define AD7124_ADC_CTRL_REG_MODE(x)        (((x) & 0xF) << 2)
#define AD7124_ADC_CTRL_REG_CLK_SEL(x)    (((x) & 0x3) << 0)

/* IO_Control_1 Register bits */
#define AD7124_IO_CTRL1_REG_GPIO_DAT2     (1 << 23)
#define AD7124_IO_CTRL1_REG_GPIO_DAT1     (1 << 22)
#define AD7124_IO_CTRL1_REG_GPIO_CTRL2    (1 << 19)
#define AD7124_IO_CTRL1_REG_GPIO_CTRL1    (1 << 18)
#define AD7124_IO_CTRL1_REG_PDSW          (1 << 15)
#define AD7124_IO_CTRL1_REG_IOUT1(x)      (((x) & 0x7) << 11)
#define AD7124_IO_CTRL1_REG_IOUT0(x)      (((x) & 0x7) << 8)
#define AD7124_IO_CTRL1_REG_IOUT_CH1(x)   (((x) & 0xF) << 4)
#define AD7124_IO_CTRL1_REG_IOUT_CH0(x)   (((x) & 0xF) << 0)

/*IO_Control_1 AD7124-8 specific bits */
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT4     (1 << 23)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT3     (1 << 22)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT2     (1 << 21)
#define AD7124_8_IO_CTRL1_REG_GPIO_DAT1     (1 << 20)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL4    (1 << 19)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL3    (1 << 18)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL2    (1 << 17)
#define AD7124_8_IO_CTRL1_REG_GPIO_CTRL1    (1 << 16)

/* IO_Control_2 Register bits */
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS7   (1 << 15)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS6   (1 << 14)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS5   (1 << 11)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS4   (1 << 10)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS3   (1 << 5)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS2   (1 << 4)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS1   (1 << 1)
#define AD7124_IO_CTRL2_REG_GPIO_VBIAS0   (1)

/*IO_Control_2 AD7124-8 specific bits */
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS15  (1 << 15)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS14  (1 << 14)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS13  (1 << 13)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS12  (1 << 12)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS11  (1 << 11)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS10  (1 << 10)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS9   (1 << 9)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS8   (1 << 8)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS7   (1 << 7)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS6   (1 << 6)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS5   (1 << 5)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS4   (1 << 4)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS3   (1 << 3)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS2   (1 << 2)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS1   (1 << 1)
#define AD7124_8_IO_CTRL2_REG_GPIO_VBIAS0   (1 << 0)

/* ID Register bits */
#define AD7124_ID_REG_DEVICE_ID(x)   (((x) & 0xF) << 4)
#define AD7124_ID_REG_SILICON_REV(x) (((x) & 0xF) << 0)

/* Error Register bits */
#define AD7124_ERR_REG_LDO_CAP_ERR        (1 << 19)
#define AD7124_ERR_REG_ADC_CAL_ERR        (1 << 18)
#define AD7124_ERR_REG_ADC_CONV_ERR       (1 << 17)
#define AD7124_ERR_REG_ADC_SAT_ERR        (1 << 16)
#define AD7124_ERR_REG_AINP_OV_ERR        (1 << 15)
#define AD7124_ERR_REG_AINP_UV_ERR        (1 << 14)
#define AD7124_ERR_REG_AINM_OV_ERR        (1 << 13)
#define AD7124_ERR_REG_AINM_UV_ERR        (1 << 12)
#define AD7124_ERR_REG_REF_DET_ERR        (1 << 11)
#define AD7124_ERR_REG_DLDO_PSM_ERR       (1 << 9)
#define AD7124_ERR_REG_ALDO_PSM_ERR       (1 << 7)
#define AD7124_ERR_REG_SPI_IGNORE_ERR     (1 << 6)
#define AD7124_ERR_REG_SPI_SLCK_CNT_ERR   (1 << 5)
#define AD7124_ERR_REG_SPI_READ_ERR       (1 << 4)
#define AD7124_ERR_REG_SPI_WRITE_ERR      (1 << 3)
#define AD7124_ERR_REG_SPI_CRC_ERR        (1 << 2)
#define AD7124_ERR_REG_MM_CRC_ERR         (1 << 1)

/* Error_En Register bits */
#define AD7124_ERREN_REG_MCLK_CNT_EN           (1 << 22)
#define AD7124_ERREN_REG_LDO_CAP_CHK_TEST_EN   (1 << 21)
#define AD7124_ERREN_REG_LDO_CAP_CHK(x)        (((x) & 0x3) << 19)
#define AD7124_ERREN_REG_ADC_CAL_ERR_EN        (1 << 18)
#define AD7124_ERREN_REG_ADC_CONV_ERR_EN       (1 << 17)
#define AD7124_ERREN_REG_ADC_SAT_ERR_EN        (1 << 16)
#define AD7124_ERREN_REG_AINP_OV_ERR_EN        (1 << 15)
#define AD7124_ERREN_REG_AINP_UV_ERR_EN        (1 << 14)
#define AD7124_ERREN_REG_AINM_OV_ERR_EN        (1 << 13)
#define AD7124_ERREN_REG_AINM_UV_ERR_EN        (1 << 12)
#define AD7124_ERREN_REG_REF_DET_ERR_EN        (1 << 11)
#define AD7124_ERREN_REG_DLDO_PSM_TRIP_TEST_EN (1 << 10)
#define AD7124_ERREN_REG_DLDO_PSM_ERR_ERR      (1 << 9)
#define AD7124_ERREN_REG_ALDO_PSM_TRIP_TEST_EN (1 << 8)
#define AD7124_ERREN_REG_ALDO_PSM_ERR_EN       (1 << 7)
#define AD7124_ERREN_REG_SPI_IGNORE_ERR_EN     (1 << 6)
#define AD7124_ERREN_REG_SPI_SCLK_CNT_ERR_EN   (1 << 5)
#define AD7124_ERREN_REG_SPI_READ_ERR_EN       (1 << 4)
#define AD7124_ERREN_REG_SPI_WRITE_ERR_EN      (1 << 3)
#define AD7124_ERREN_REG_SPI_CRC_ERR_EN        (1 << 2)
#define AD7124_ERREN_REG_MM_CRC_ERR_EN         (1 << 1)

/* Channel Registers 0-15 bits */
#define AD7124_CH_MAP_REG_CH_ENABLE    (1 << 15)
#define AD7124_CH_MAP_REG_SETUP(x)     (((x) & 0x7) << 12)
#define AD7124_CH_MAP_REG_AINP(x)      (((x) & 0x1F) << 5)
#define AD7124_CH_MAP_REG_AINM(x)      (((x) & 0x1F) << 0)

/* Configuration Registers 0-7 bits */
#define AD7124_CFG_REG_BIPOLAR     (1 << 11)
#define AD7124_CFG_REG_BURNOUT(x)  (((x) & 0x3) << 9)
#define AD7124_CFG_REG_REF_BUFP    (1 << 8)
#define AD7124_CFG_REG_REF_BUFM    (1 << 7)
#define AD7124_CFG_REG_AIN_BUFP    (1 << 6)
#define AD7124_CFG_REG_AINN_BUFM   (1 << 5)
#define AD7124_CFG_REG_REF_SEL(x)  ((x) & 0x3) << 3
#define AD7124_CFG_REG_PGA(x)      (((x) & 0x7) << 0)

/* Filter Register 0-7 bits */
#define AD7124_FILT_REG_FILTER(x)        ((uint32_t)((x) & 0x7) << 21)
#define AD7124_FILT_REG_REJ60             ((uint32_t)1 << 20)
#define AD7124_FILT_REG_POST_FILTER(x)    ((uint32_t)((x) & 0x7) << 17)
#define AD7124_FILT_REG_SINGLE_CYCLE      ((uint32_t)1 << 16)
#define AD7124_FILT_REG_FS(x)             ((uint32_t)((x) & 0x7FF) << 0)

#define AD7124_CRC8_POLYNOMIAL_REPRESENTATION 0x07 /* x8 + x2 + x + 1 */
#define AD7124_DISABLE_CRC 0
#define AD7124_USE_CRC 1
#define AD7124_READ_DATA 2

#define INVALID_VAL -1 /* Invalid argument */
#define COMM_ERR    -2 /* Communication error on receive */
#define TIMEOUT     -3 /* A timeout has occured */

enum ad7124_registers {
	AD7124_Status = 0x00,
	AD7124_ADC_Control,
	AD7124_Data,
	AD7124_IOCon1,
	AD7124_IOCon2,
	AD7124_ID,
	AD7124_Error,
	AD7124_Error_En,
	AD7124_Mclk_Count,
	AD7124_Channel_0,
	AD7124_Channel_1,
	AD7124_Channel_2,
	AD7124_Channel_3,
	AD7124_Channel_4,
	AD7124_Channel_5,
	AD7124_Channel_6,
	AD7124_Channel_7,
	AD7124_Channel_8,
	AD7124_Channel_9,
	AD7124_Channel_10,
	AD7124_Channel_11,
	AD7124_Channel_12,
	AD7124_Channel_13,
	AD7124_Channel_14,
	AD7124_Channel_15,
	AD7124_Config_0,
	AD7124_Config_1,
	AD7124_Config_2,
	AD7124_Config_3,
	AD7124_Config_4,
	AD7124_Config_5,
	AD7124_Config_6,
	AD7124_Config_7,
	AD7124_Filter_0,
	AD7124_Filter_1,
	AD7124_Filter_2,
	AD7124_Filter_3,
	AD7124_Filter_4,
	AD7124_Filter_5,
	AD7124_Filter_6,
	AD7124_Filter_7,
	AD7124_Offset_0,
	AD7124_Offset_1,
	AD7124_Offset_2,
	AD7124_Offset_3,
	AD7124_Offset_4,
	AD7124_Offset_5,
	AD7124_Offset_6,
	AD7124_Offset_7,
	AD7124_Gain_0,
	AD7124_Gain_1,
	AD7124_Gain_2,
	AD7124_Gain_3,
	AD7124_Gain_4,
	AD7124_Gain_5,
	AD7124_Gain_6,
	AD7124_Gain_7,
	AD7124_REG_NO
};

enum ad7124_reg_access{
	AD7124_RW = 1,   /* Read and Write */
	AD7124_R  = 2,   /* Read only */
	AD7124_W  = 3,   /* Write only */
};

	/*! Device register info */
typedef struct _ad7124_st_reg {
	int32_t addr;
	int32_t value;
	int32_t size;
	int32_t rw;
} ad7124_st_reg;


/*! Array holding the info for the ad7124 registers - address,
 *  initial value, size and access type. */
static ad7124_st_reg ad7124_regs[57] = {
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	{0x01, 0x0000, 2, 1}, /* AD7124_ADC_Control */
	{0x02, 0x0000, 3, 2}, /* AD7124_Data */
	{0x03, 0x000000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x04,   1, 2}, /* AD7124_ID */
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	{0x07, 0x000040, 3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
	{0x09, 0x8001, 2, 1}, /* AD7124_Channel_0 */
	{0x0A, 0x0001, 2, 1}, /* AD7124_Channel_1 */
	{0x0B, 0x0001, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0x0001, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0x0001, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0x0001, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0x0001, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0x0001, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0x0001, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0x0001, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0x0001, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0x0001, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0x0001, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0x0001, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0x0001, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x0001, 2, 1}, /* AD7124_Channel_15 */
	{0x19, 0x0860, 2, 1}, /* AD7124_Config_0 */
	{0x1A, 0x0860, 2, 1}, /* AD7124_Config_1 */
	{0x1B, 0x0860, 2, 1}, /* AD7124_Config_2 */
	{0x1C, 0x0860, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0860, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0860, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0860, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0860, 2, 1}, /* AD7124_Config_7 */
	{0x21, 0x060180, 3, 1}, /* AD7124_Filter_0 */
	{0x22, 0x060180, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0x060180, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0x060180, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0x060180, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0x060180, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0x060180, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0x060180, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};

	/* AD7124 Register Map */
enum AD7124_reg_map  {
	COMM_REG     = 0x00,
	STATUS_REG   = 0x00,
	ADC_CTRL_REG = 0x01,
	DATA_REG     = 0x02,
	IO_CTRL1_REG = 0x03,
	IO_CTRL2_REG = 0x04,
	ID_REG       = 0x05,
	ERR_REG      = 0x06,
	ERREN_REG    = 0x07,
	CH0_MAP_REG  = 0x09,
	CH1_MAP_REG  = 0x0A,
	CH2_MAP_REG  = 0x0B,
	CH3_MAP_REG  = 0x0C,
	CH4_MAP_REG  = 0x0D,
	CH5_MAP_REG  = 0x0E,
	CH6_MAP_REG  = 0x0F,
	CH7_MAP_REG  = 0x10,
	CH8_MAP_REG  = 0x11,
	CH9_MAP_REG  = 0x12,
	CH10_MAP_REG = 0x13,
	CH11_MAP_REG = 0x14,
	CH12_MAP_REG = 0x15,
	CH13_MAP_REG = 0x16,
	CH14_MAP_REG = 0x17,
	CH15_MAP_REG = 0x18,
	CFG0_REG     = 0x19,
	CFG1_REG     = 0x1A,
	CFG2_REG     = 0x1B,
	CFG3_REG     = 0x1C,
	CFG4_REG     = 0x1D,
	CFG5_REG     = 0x1E,
	CFG6_REG     = 0x1F,
	CFG7_REG     = 0x20,
	FILT0_REG    = 0x21,
	FILT1_REG    = 0x22,
	FILT2_REG    = 0x23,
	FILT3_REG    = 0x24,
	FILT4_REG    = 0x25,
	FILT5_REG    = 0x26,
	FILT6_REG    = 0x27,
	FILT7_REG    = 0x28,
	OFFS0_REG    = 0x29,
	OFFS1_REG    = 0x2A,
	OFFS2_REG    = 0x2B,
	OFFS3_REG    = 0x2C,
	OFFS4_REG    = 0x2D,
	OFFS5_REG    = 0x2E,
	OFFS6_REG    = 0x2F,
	OFFS7_REG    = 0x30,
	GAIN0_REG    = 0x31,
	GAIN1_REG    = 0x32,
	GAIN2_REG    = 0x33,
	GAIN3_REG    = 0x34,
	GAIN4_REG    = 0x35,
	GAIN5_REG    = 0x36,
	GAIN6_REG    = 0x37,
	GAIN7_REG    = 0x38,
};

enum SOIL_STATUS {
	SOIL_SUCCESS = 0,
	SOIL_FAILURE = 1,
};

#define PH_PORT_CS 	BSP_USART2_CS_PORT
#define PH_PIN_CS 	BSP_USART2_CS_PIN

#define ADP7118_ENABLE_PORT gpioPortC
#define ADP7118_ENABLE_PIN  11U

#define CONV_TIMEOUT 	10000

#define PH_CHANNEL 1
#define MOISTURE_CHANNEL 2

static SPIDRV_HandleData_t master_adc;
static SPIDRV_Handle_t SPI_master_adc = &master_adc;

void halPhMoistureStartRead(void);

Ecode_t spi_init();
void spi_deinit();
bool adc_verify();
Ecode_t adc_init();
void enable_channel(uint8_t channel);
void disable_channel(uint8_t channel);

uint32_t adc_read_channel(uint8_t);
Ecode_t start_single_conversion();
bool wait_conversion_complete();
Ecode_t SPI_read_reg(ad7124_st_reg*);
Ecode_t SPI_write_reg(ad7124_st_reg*);
void channel_control(uint8_t, bool);
Ecode_t adc_init_regs(void);
void set_reg_str(ad7124_st_reg* reg, uint8_t reg_no);

float ADC_read_ph_value(void);
float ADC_read_moisture_value(void);

#endif
