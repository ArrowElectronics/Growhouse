/*
 * AD7798.c
 *
 *  Created on: 10-Oct-2018
 *      Author: einfochips
 */


/***************************************************************************//**
 *   @file   AD7798.c
 *   @brief  Implementation of AD7798 Driver.
 *
 ********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ********************************************************************************
 *   SVN Revision: 577
 *******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "AD7798.h"				// AD7798 definitions.
#include "app/framework/include/af.h"
#include "spidrv.h"
//#include <stdint.h>

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

int8_t statusReg, idReg, ioReg, gainAdc;
uint16_t modeReg, configReg, offsetReg, fullscaleReg, dataReg;

uint16_t adcValue[3];
float voltageValue[3], intensityValue[3], lightConcentration[3];

int barLine[3];

const uint8_t Channels[3] = { 1, 0, 2};

const uint8_t ColorPrint[] = { 31, 32, 34 };

const uint8_t Gain[8] = { 1, 2, 4, 8, 16, 32, 64, 128};

const float Lux_LSB[3] = {2.122, 2.124, 2.113};

const float Optimal_Levels[3] = {26909.0, 8880.0, 26909.0};

/* SPI handler */
SPIDRV_HandleData_t handleDataMaster;
SPIDRV_Handle_t handleMaster = &handleDataMaster;

/***************************************************************************//**
 * @brief Initializes the SPI as master interface
 *
 * @param None.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 *******************************************************************************/
int SPI_init_driver()
{
    SPIDRV_Init_t initDataMaster = SPIDRV_MASTER_USART2;
    initDataMaster.portLocationTx = _USART_ROUTELOC0_TXLOC_LOC17;
    initDataMaster.portLocationRx = _USART_ROUTELOC0_RXLOC_LOC17;
    initDataMaster.portLocationCs = _USART_ROUTELOC0_CSLOC_LOC17;
    initDataMaster.portLocationClk = _USART_ROUTELOC0_CLKLOC_LOC17;
    initDataMaster.dummyTxValue = 0x00; /* Note: Dummy byte 0xFF is not working. */
    initDataMaster.clockMode = spidrvClockMode3; /* SPI mode */
    return SPIDRV_Init( handleMaster, &initDataMaster );
}

/***************************************************************************//**
 * @brief Initializes the AD7798 and checks if the device is present.
 *
 * @param None.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: 1 - if initialization was successful (ID is 0x0B).
 *                           0 - if initialization was unsuccessful.
 *******************************************************************************/
unsigned char AD7798_Init(void)
{
    unsigned char status = 0x1;

    if ((AD7798_GetRegisterValue (AD7798_REG_ID, 1) & 0x0F) != AD7798_ID)
    {
        status = 0x0;
    }

    return(status);
}

/***************************************************************************//**
 * @brief Sends 32 consecutive 1's on SPI in order to reset the part.
 *
 * @param None.
 *
 * @return  None.
 *******************************************************************************/
void AD7798_Reset(void)
{
    unsigned char dataToSend[4] = {0xFF, 0xFF, 0xFF, 0xFF};

    SPIDRV_MTransmitB(handleMaster, dataToSend, 4);

    halCommonDelayMilliseconds(10);

}
/***************************************************************************//**
 * @brief Reads the value of the selected register
 *
 * @param regAddress - The address of the register to read.
 * @param size - The size of the register to read.
 *
 * @return data - The value of the selected register register.
 *******************************************************************************/
uint16_t AD7798_GetRegisterValue(unsigned char regAddress, unsigned char size)
{
    unsigned char data[size];
    uint16_t receivedData = 0x00;
    unsigned char byte;
    unsigned char rxBuffer[2] = {0};

    data[0] = AD7798_COMM_READ |  AD7798_COMM_ADDR(regAddress);
    SPIDRV_MTransmitB(handleMaster, (void *)&data[0], 1);
    SPIDRV_MReceiveB(handleMaster, (void *)rxBuffer, size);
    receivedData = rxBuffer[0];

    if(size > 1){

        for(byte = 1; byte < size; byte++){
            receivedData = (receivedData << (byte * 8) | rxBuffer[byte]);
        }
    }
    return receivedData;

}
/***************************************************************************//**
 * @brief Writes the value to the register
 *
 * @param -  regAddress - The address of the register to write to.
 * @param -  regValue - The value to write to the register.
 * @param -  size - The size of the register to write.
 *
 * @return  None.
 *******************************************************************************/
void AD7798_SetRegisterValue(uint8_t regAddress, uint16_t regValue,
        uint8_t size)
{
    uint8_t data[size + 1];
    uint8_t byte;
    uint16_t mask;

    data[0] = AD7798_COMM_WRITE |  AD7798_COMM_ADDR(regAddress);

    if(size == 1){

        mask = 0x00FF;

    } else{

        mask = 0xFF00;
    }

    for(byte = 1; byte <= size; byte++)
    {
        data[byte] = (uint8_t)((regValue & mask) >> ((size - byte) * 8));
        mask = mask >> (byte * 8);
    }

    SPIDRV_MTransmitB(handleMaster, data, (1 + size));
}
/***************************************************************************//**
 * @brief Reads /RDY bit of status reg.
 *
 * @param None.
 *
 * @return rdy	- 0 if RDY is 1.
 *              - 1 if RDY is 0.
 *******************************************************************************/
unsigned char AD7798_Ready(void)
{

    while((AD7798_GetRegisterValue( AD7798_REG_STAT,1) & 0x80) != 0x80);

    return(1);
}

/***************************************************************************//**
 * @brief Sets the operating mode of AD7798.
 *
 * @param mode - Mode of operation.
 *
 * @return  None.
 *******************************************************************************/
void AD7798_SetMode(unsigned long mode)
{
    unsigned long command;
    command = AD7798_GetRegisterValue(AD7798_REG_MODE,2);
    command &= ~AD7798_MODE_SEL(0xFF);
    command |= AD7798_MODE_SEL(mode);
    AD7798_SetRegisterValue(
            AD7798_REG_MODE,
            command,
            2
            );
}
/***************************************************************************//**
 * @brief Selects the channel of AD7798.
 *
 * @param  channel - ADC channel selection.
 *
 * @return  None.
 *******************************************************************************/
void AD7798_SetChannel(unsigned long channel)
{
    unsigned long command;
    command = AD7798_GetRegisterValue(AD7798_REG_CONF,2);
    command &= ~AD7798_CONF_CHAN(0xFF);
    command |= AD7798_CONF_CHAN(channel);
    AD7798_SetRegisterValue(
            AD7798_REG_CONF,
            command,
            2
            );
}

/***************************************************************************//**
 * @brief  Sets the gain of the In-Amp.
 *
 * @param  gain - Gain.
 *
 * @return  None.
 *******************************************************************************/
void AD7798_SetGain(uint16_t gain)
{
    uint16_t command;
    command = AD7798_GetRegisterValue(AD7798_REG_CONF,2);
    command &= ~AD7798_CONF_GAIN(0xFF);
    command |= AD7798_CONF_GAIN(gain);
    AD7798_SetRegisterValue(
            AD7798_REG_CONF,
            command,
            2
            );
}

/***************************************************************************//**
 * @brief  Sets the Filter.
 *
 * @param  filter - Filter.
 *
 * @return  None.
 *******************************************************************************/
void AD7798_SetFilter(unsigned int filter)
{
    unsigned long command;
    command = AD7798_GetRegisterValue(AD7798_REG_MODE,2);
    command &= ~AD7798_MODE_RATE(0x0F);
    command |= AD7798_MODE_RATE(filter);
    AD7798_SetRegisterValue(
            AD7798_REG_MODE,
            command,
            2
            );
}
/***************************************************************************//**
 * @brief Enables or disables the reference detect function.
 *
 * @param state - State of the reference detect function.
 *               Example: 0	- Reference detect disabled.
 *                        1	- Reference detect enabled.
 *
 * @return None.
 *******************************************************************************/
void AD7798_SetReference(unsigned char state)
{
    unsigned long command = 0;
    command = AD7798_GetRegisterValue(AD7798_REG_CONF,2);
    command &= ~AD7798_CONF_REFDET(1);
    command |= AD7798_CONF_REFDET(state);
    AD7798_SetRegisterValue(AD7798_REG_CONF,
            command,
            2);
}

/***************************************************************************//**
 * @brief Set conding mode in ADC whether Unipolar/Bipolar.
 *
 * @param mode - ADC mode (UNIPOLAR/BIPOLAR).
 *
 * @return None.
 *******************************************************************************/
void AD7798_SetCodingMode(uint8_t mode)
{
    uint16_t command;

    command = AD7798_GetRegisterValue(AD7798_REG_CONF,2);

    if(mode == AD7798_BIPOLAR){

        command &= ~AD7798_CONF_UNIPOLAR;

    } else if(mode == AD7798_UNIPOLAR){

        command |= AD7798_CONF_UNIPOLAR;
    }
    AD7798_SetRegisterValue(
            AD7798_REG_CONF,
            command,
            2
            );
}

/***************************************************************************//**
 * @brief Enable/Disable Burn Out current.
 *
 * @param select - Burn Out current state.
 *
 * @return None.
 *******************************************************************************/
void AD7798_SetBurnoutCurrent(uint8_t select)
{
    uint16_t command;

    command = AD7798_GetRegisterValue(AD7798_REG_CONF,2);

    if(select == AD7798_DISABLE)
        command &= ~AD7798_CONF_BO_EN;
    else if(select == AD7798_ENABLE)
        command |= AD7798_CONF_BO_EN;

    AD7798_SetRegisterValue(
            AD7798_REG_CONF,
            command,
            2
            );
}

/***************************************************************************//**
 * @brief Initialize Light Shield and configure it.
 *
 * @param None.
 *
 * @return data - The status of LightShield Initialization.
 *                0 - success, fail otherwise.
 *******************************************************************************/
int LightShield_Init(void)
{
    int ret = 0;
#if(USE_CALIBRATION == 1)
    uint8_t channel;
#endif
    emberAfCorePrintln("Resetting AD7798");
    //AD7798_Reset();
    //halCommonDelayMilliseconds(1000);
    if(AD7798_Init()){
        emberAfCorePrintln("AD7798 Init Successful");
        emberAfCorePrintln("AD7798 Set Codingmode");
        AD7798_SetCodingMode(AD7798_UNIPOLAR);
        emberAfCorePrintln("AD7798 Set mode");
        AD7798_SetMode(AD7798_MODE_SINGLE);
        emberAfCorePrintln("AD7798 Set Gain");
        AD7798_SetGain(ADC_GAIN);
        emberAfCorePrintln("AD7798 Set SPS");
        AD7798_SetFilter(ADC_SPS);
        emberAfCorePrintln("AD7798 Set REFDET ENA");
        AD7798_SetReference(AD7798_REFDET_ENA);
        emberAfCorePrintln("Light Shield init end");

    }
    else
    {
        emberAfCorePrintln("AD7798 Init FAIL");
        ret = -1;
    }

    gainAdc = Gain[ADC_GAIN];

#if(USE_CALIBRATION == 1) //If calibration is enabled?
    emberAfCorePrintln("Calibrate the system:");
    emberAfCorePrintln("");

    for(channel = 0; channel < CHANNELS; channel++){

        emberAfCorePrintln("\tCalibrate "); emberAfCorePrintln(colour[channel]); emberAfCorePrintln(" channel: be sure that "); emberAfCorePrintln(colour[channel]); emberAfCorePrintln(" photodiode is covered and press <ENTER>.");
        CN0397_Calibration(Channels[channel]);
        emberAfCorePrintln("\t\tChannel is calibrated!\n");

    }
    emberAfCorePrintln("System calibration complete!\n");
    emberAfCorePrintln("");
#endif
    return ret;
}

/***************************************************************************//**
 * @brief Read ADC data for specific ADC channel.
 *
 * @param adcChannel - The channel of ADC to read data from.
 * @param adcData - The read data value from ADC channel.
 *
 * @return None
 *******************************************************************************/
void LightShield_ReadADCData(uint8_t adcChannel, uint16_t *adcData)
{

    uint8_t channel;

    channel = 0x80 | adcChannel;

    //convFlag = 1;

    //digitalWrite(CS_PIN,LOW);

    AD7798_SetRegisterValue(AD7798_REG_MODE, 0x200A, 2);

    while((AD7798_GetRegisterValue( AD7798_REG_STAT,1) & channel) != channel);

    halCommonDelayMilliseconds(200);

    *adcData = AD7798_GetRegisterValue(AD7798_REG_DATA,2);

    //digitalWrite(CS_PIN,HIGH);

    //convFlag = 0;

}

/***************************************************************************//**
 * @brief Read ADC data for specific ADC channel.
 *
 * @param adcChannel - The channel of ADC to read data from.
 * @param adcData - The read data value from ADC channel.
 *
 * @return None
 *******************************************************************************/
void LightShield_ConvertToVoltage(uint16_t adcValue, float *voltage)
{

    *voltage = (float)(adcValue * V_REF)/(float)(_2_16 * gainAdc);

}

/***************************************************************************//**
 * @brief Calculate Light Intensity data in Lux from ADC reading.
 *
 * @param channel - The channel of ADC.
 * @param adcData - The read value from ADC channel.
 * @param intensity - Light intensity for that ADC channel.
 *
 * @return None
 *******************************************************************************/
void LightShield_CalcLightIntensity(uint8_t channel, uint16_t adcValue, float *intensity)
{

    *intensity = adcValue * Lux_LSB[channel];

}

/***************************************************************************//**
 * @brief Calculate Light concentration.
 *
 * @param channel - The channel of ADC.
 * @param adcData - The light intensity for that channel.
 * @param adcData - The calculated light concentration.
 *
 * @return None
 *******************************************************************************/
void LightShield_CalcLightConcentration(uint8_t channel, float intensity, float *conc)
{

    *conc = (intensity *100)/Optimal_Levels[channel];

}

/***************************************************************************//**
 * @brief Read Light intensity data for R,G and B light.
 *
 * @param RIntensity - Calculated R light intensity.
 * @param RIntensity - Calculated G light intensity.
 * @param RIntensity - Calculated B light intensity.
 *
 * @return None
 *******************************************************************************/
void LightShield_ReadData(float *RIntensity, float *GIntensity, float *BIntensity)
{
    uint8_t channel, rgbChannel;

    for(channel = 0; channel < CHANNELS; channel++){

        rgbChannel = Channels[channel];

        AD7798_SetChannel(channel);
        LightShield_ReadADCData(channel, &adcValue[rgbChannel]);
        LightShield_ConvertToVoltage(adcValue[rgbChannel], &voltageValue[rgbChannel]);
        LightShield_CalcLightIntensity(rgbChannel, adcValue[rgbChannel], &intensityValue[rgbChannel]);
        LightShield_CalcLightConcentration(rgbChannel, intensityValue[rgbChannel], &lightConcentration[rgbChannel]);
        LightShield_SetBar(lightConcentration[rgbChannel], &barLine[rgbChannel]);

    }
    *RIntensity = intensityValue[0];
    *GIntensity = intensityValue[1];
    *BIntensity = intensityValue[2];
}

/***************************************************************************//**
 * @brief Calibrate Light Shield ADC channel.
 *
 * @param channel - The channel of ADC to calibrate.
 * @param adcData - The read data value from ADC channel.
 *
 * @return None
 *******************************************************************************/
void LightShield_Calibration(uint8_t channel)
{

    uint16_t setValue;

    AD7798_SetChannel(channel);  /* select channel to calibrate */

    /* Perform system zero-scale calibration */
    setValue = AD7798_GetRegisterValue(AD7798_REG_MODE, 2);

    setValue &= ~(AD7798_MODE_SEL(0x07));
    setValue |= AD7798_MODE_SEL(AD7798_MODE_CAL_SYS_ZERO);
    AD7798_SetRegisterValue(AD7798_REG_MODE, setValue, 2);

    while((AD7798_GetRegisterValue( AD7798_REG_STAT,1) & channel) != channel);  /* wait for RDY bit to go low */
    while(AD7798_GetRegisterValue(AD7798_REG_MODE, 2) != 0x4005);    /* wait for ADC to go in idle mode */


}

/***************************************************************************//**
 * @brief Set color Bar for Light.
 *
 * @param conc - concentration.
 * @param line - line.
 *
 * @return None
 *******************************************************************************/
void LightShield_SetBar(float conc, int *line)
{
    float concLimit = 5.0;
    int i = 0, j;
    *line = 0;

    if (conc > 0.0){
        i = 1;
        *line = i;
    }

    for(j = 0; j< 20; j++){
        if(conc >= concLimit){
            *line = i+1;
        }
        concLimit +=5.0;
        i +=1;
    }

}
