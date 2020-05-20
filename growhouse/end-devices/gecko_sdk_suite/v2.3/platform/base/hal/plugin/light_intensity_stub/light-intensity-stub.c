/*
 * AD7798.c
 *
 *  Created on: 10-Oct-2018
 *      Author: einfochips
 */

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "app/framework/include/af.h"
#include "spidrv.h"
#include "AD7798.h"				/* AD7798 definitions. */
#include "light-intensity-stub.h"

/******************************************************************************/
/* MACROs                                                                     */
/******************************************************************************/
#define AD7798_COMM_READ        (1 << 6)         /* Read Operation */
#define AD7798_COMM_ADDR(x)     (((x) & 0x7) << 3)   /* Register Address */
#define AD7798_REG_ID           4 /* ID Register       (RO, 8-bit) */
#define AD7798_ID               0x8

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
/* Global variable to store actual RGB light intensity data */
int set_RIntensity = 0;
int set_GIntensity = 0;
int set_BIntensity = 0;

/* sensor SPI init status */
static bool spiInitStatus = false;

//extern float voltageValue[3], intensityValue[3], lightConcentration[3];
float RIntensity = 0;
float GIntensity = 0;
float BIntensity = 0;

/***************************************************************************//**
 * @brief light intensity stub init callback.
 *
 * @param none.
 *
 * @return none.
 *******************************************************************************/
void emberAfPluginLightIntensityStubInitCallback(void)
{
    int returnCode = 0;
    uint32_t delay = 200; /* delay in msec */

    /* SPI initialization */
    returnCode = SPI_init_driver();

    if (returnCode == ECODE_EMDRV_SPIDRV_OK)
    {
        emberAfCorePrintln("Light Shield SPI initialization successfully.");
    }
    else
    {
        spiInitStatus = false;
        return;
    }
    halCommonDelayMilliseconds(delay);
    //AD7798_Reset();
    //halCommonDelayMilliseconds(delay);

    /* LightShield Init and configuration */
    returnCode = LightShield_Init();
    if(returnCode != 0)
    {
        spiInitStatus = false;
        return;
    }

    /* Feed watchdog */
    halResetWatchdog();

    spiInitStatus = true;

}

/***************************************************************************//**
 * @brief light intensity data read
 *
 * @param none.
 *
 * @return none.
 *******************************************************************************/
void halRGBIntensityStartRead(void)
{
    /* Check if the LightShield is not init properly then try it again */
    if(spiInitStatus != true)
    {
        /* Initialize Light Shield again */
        emberAfPluginLightIntensityStubInitCallback();
    }

    LightShield_ReadData(&RIntensity, &GIntensity, &BIntensity);

    //LightShield_DisplayData();
    set_RIntensity = (int)RIntensity;
    set_GIntensity = (int)GIntensity;
    set_BIntensity = (int)BIntensity;

    emberAfCorePrintln("hardware RI: %d, GI: %d, BI: %d", set_RIntensity, set_GIntensity, set_BIntensity);
    halRGBIntensityReadingCompleteCallback(set_RIntensity, set_GIntensity, set_BIntensity, spiInitStatus);
}
