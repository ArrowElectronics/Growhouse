/***************************************************************************//**
 * @file
 * @brief Nor Quad Spi Controller BSP - Silicon Labs - Template
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#if (defined(RTOS_MODULE_FS_AVAIL) && defined(RTOS_MODULE_FS_STORAGE_NOR_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>

#include  <drivers/fs/include/fs_nor_quad_spi_drv.h>
#include  <fs/include/fs_nor.h>
#include  <fs/include/fs_nor_quad_spi.h>
#include  <fs/include/fs_nor_phy_drv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                        LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_Init(FS_NOR_QUAD_SPI_ISR_HANDLE_FNCT isr_fnct,
                                           void                            *p_drv_data);

static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_ClkCfg(void);

static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_IO_Cfg(void);

static CPU_INT32U BSP_FS_NOR_QuadSPI_ClkFreqGet(void);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const FS_NOR_QUAD_SPI_BSP_API QuadSPI_Ctrlr_BSP_API = {
  .Init = BSP_FS_NOR_QuadSPI_Init,
  .ClkCfg = BSP_FS_NOR_QuadSPI_ClkCfg,
  .IO_Cfg = BSP_FS_NOR_QuadSPI_IO_Cfg,
  .IntCfg = DEF_NULL,
  .ChipSelEn = DEF_NULL,
  .ChipSelDis = DEF_NULL,
  .ClkFreqGet = BSP_FS_NOR_QuadSPI_ClkFreqGet
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const FS_NOR_QUAD_SPI_CTRLR_INFO BSP_FS_NOR_QuadSPICtrl_HwInfo = {
  .DrvApiPtr = (FS_NOR_QUAD_SPI_DRV_API *)&FS_NOR_QuadSpiDrvAPI_Silabs_EFM32GG11,
  .BspApiPtr = (FS_NOR_QUAD_SPI_BSP_API *)&QuadSPI_Ctrlr_BSP_API,
  .BaseAddr = 0x00000000,
  .AlignReq = sizeof(CPU_ALIGN),
  .FlashMemMapStartAddr = 0x00000000,
  .BusWidth = FS_NOR_SERIAL_BUS_WIDTH_SINGLE_IO
};

const FS_NOR_QUAD_SPI_HW_INFO BSP_FS_NOR_QuadSPI_HwInfo = {
  .PartHwInfo.PhyApiPtr = (CPU_INT32U)(&FS_NOR_PHY_MX25R_API),
  .PartHwInfo.ChipSelID = 0u,
  .CtrlrHwInfoPtr = &BSP_FS_NOR_QuadSPICtrl_HwInfo
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                             LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       BSP_FS_NOR_QuadSPI_Init()
 *
 * @brief    Initializes internal resources needed by the NOR QSPI BSP.
 *
 * @param    isr_fnct    Interrupt service routine to call when an interruption from the QSPI controller
 *                       occurs.
 *
 * @param    p_drv_data  Pointer to QSPI controller driver private data. This must be passed to the
 *                       interrupt service routine 'isr_fnct'.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_Init(FS_NOR_QUAD_SPI_ISR_HANDLE_FNCT isr_fnct,
                                           void                            *p_drv_data)
{
  PP_UNUSED_PARAM(isr_fnct);                                    // QSPI driver does not use interrupt for NOR xfers.
  PP_UNUSED_PARAM(p_drv_data);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                      BSP_FS_NOR_QuadSPI_ClkCfg()
 *
 * @brief    Initializes clock(s) needed by the QSPI controller.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_ClkCfg(void)
{
  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                      BSP_FS_NOR_QuadSPI_IO_Cfg()
 *
 * @brief    Initializes Input/Output needed by the QSPI controller.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NOR_QuadSPI_IO_Cfg(void)
{
  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                    BSP_FS_NOR_QuadSPI_ClkFreqGet()
 *
 * @brief    Get input clock frequency of Quad SPI controller.
 *
 * @return   Input clock frequency, in hertz.
 *******************************************************************************************************/
static CPU_INT32U BSP_FS_NOR_QuadSPI_ClkFreqGet(void)
{
  return (0u);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL && RTOS_MODULE_FS_STORAGE_NOR_AVAIL
