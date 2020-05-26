/***************************************************************************//**
 * @file
 * @brief Nand Memory Controller BSP - Silicon Labs - Efm32Gg-Stk3700A
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

#if (defined(RTOS_MODULE_FS_AVAIL) && defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>

#include  <drivers/fs/include/fs_nand_ctrlr_drv.h>
#include  <fs/include/fs_nand_ctrlr_gen.h>
#include  <fs/include/fs_nand_ctrlr_gen_ext_soft_ecc.h>

//                                                                 Third Party Library Includes
#include  <em_cmu.h>
#include  <em_gpio.h>
#include  <em_ebi.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 *
 * Note(s) : (1) The NAND256W3A Chip is located at EBI Bank 0, with Base Address of 0x80000000u.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_BSP

#define  BSP_NAND_MEM_MAP_EBI_BASE_ADDR         EBI_MEM_BASE

/********************************************************************************************************
 *                                          PORT & PIN DEFINES
 *******************************************************************************************************/

//                                                                 ------------- 'ENABLE' PORT/PIN DEFINES ------------
#define  BSP_NAND_ALE_PORT                      gpioPortC       // Port C, Pin  1 - Address Latch Enable.
#define  BSP_NAND_ALE_PIN                       1u

#define  BSP_NAND_CLE_PORT                      gpioPortC       // Port C, Pin  2 - Command Latch Enable.
#define  BSP_NAND_CLE_PIN                       2u

#define  BSP_NAND_WP_PORT                       gpioPortD       // Port D, Pin 13 - Write Protect.
#define  BSP_NAND_WP_PIN                        13u

#define  BSP_NAND_CE_PORT                       gpioPortD       // Port D, Pin 14 - Chip Enable.
#define  BSP_NAND_CE_PIN                        14u

#define  BSP_NAND_RB_PORT                       gpioPortD       // Port D, Pin 15 - Ready / Busy.
#define  BSP_NAND_RB_PIN                        15u

#define  BSP_NAND_WE_PORT                       gpioPortF       // Port F, Pin  8 - Write Enable.
#define  BSP_NAND_WE_PIN                        8u

#define  BSP_NAND_RE_PORT                       gpioPortF       // Port F, Pin  9 - Read Enable.
#define  BSP_NAND_RE_PIN                        9u

#define  BSP_NAND_PE_PORT                       gpioPortB       // Port B, Pin 15 - Power Enable.
#define  BSP_NAND_PE_PIN                        15u

//                                                                 --------------- I/O PORT/PIN DEFINES ---------------
#define  BSP_NAND_IOn_PORT                      gpioPortE       // Port E - Port for All I/O Pins.

#define  BSP_NAND_IO0_PIN                       8u              // Pin  8 - I/O Pin 0. EBI_AD0.
#define  BSP_NAND_IO1_PIN                       9u              // Pin  9 - I/O Pin 1. EBI_AD1.
#define  BSP_NAND_IO2_PIN                       10u             // Pin 10 - I/O Pin 2. EBI_AD2.
#define  BSP_NAND_IO3_PIN                       11u             // Pin 11 - I/O Pin 3. EBI_AD3.
#define  BSP_NAND_IO4_PIN                       12u             // Pin 12 - I/O Pin 4. EBI_AD4.
#define  BSP_NAND_IO5_PIN                       13u             // Pin 13 - I/O Pin 5. EBI_AD5.
#define  BSP_NAND_IO6_PIN                       14u             // Pin 14 - I/O Pin 6. EBI_AD6.
#define  BSP_NAND_IO7_PIN                       15u             // Pin 15 - I/O Pin 7. EBI_AD7.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN BSP_FS_NAND_Init(FS_NAND_CTRLR_DRV_ISR_HANDLE_FNCT isr_fnct,
                                    FS_NAND_CTRLR_DRV                 *p_drv);

static CPU_BOOLEAN BSP_FS_NAND_ClkCfg(void);

static CPU_BOOLEAN BSP_FS_NAND_IO_Cfg(void);

static CPU_BOOLEAN BSP_FS_NAND_ChipSelEn(CPU_INT16U part_slave_id);

static CPU_BOOLEAN BSP_FS_NAND_ChipSelDis(CPU_INT16U part_slave_id);

static CPU_BOOLEAN BSP_FS_NAND_IsChipRdy(void);

/********************************************************************************************************
 *                                         INTERFACE STRUCTURE
 *******************************************************************************************************/
//                                                                 ---------------- NAND BSP API STRUCT ---------------
static const FS_NAND_CTRLR_BSP_API BSP_FS_NAND_BSP_API = {
  .Init = BSP_FS_NAND_Init,
  .ClkCfg = BSP_FS_NAND_ClkCfg,
  .IO_Cfg = BSP_FS_NAND_IO_Cfg,
  .IntCfg = DEF_NULL,
  .ChipSelEn = BSP_FS_NAND_ChipSelEn,
  .ChipSelDis = BSP_FS_NAND_ChipSelDis,
  .IsChipRdy = BSP_FS_NAND_IsChipRdy
};

//                                                                 ------------ CONTROLLER SPECIFIC CONFIG ------------
//                                                                 Ctrl Info : Soft-ECC Used, Hamming(Hsaio) ECC ref.
static const FS_NAND_CTRLR_GEN_HW_INFO BSP_FS_NAND_CtrlrGen_HwInfo = FS_NAND_CTRLR_GEN_HW_INFO_INIT(&FS_NAND_CtrlrGen_SoftEcc_Hamming_HwInfo,
                                                                                                    &FS_NAND_CtrlDrvAPI_Silabs_EFM32GG_EBI,
                                                                                                    &BSP_FS_NAND_BSP_API,
                                                                                                    EBI_BASE,
                                                                                                    BSP_NAND_MEM_MAP_EBI_BASE_ADDR,
                                                                                                    sizeof(CPU_ALIGN));

//                                                                 --------------- CHIP SPECIFIC CONFIG ---------------
//                                                                 Chip Info : 'STATIC' Configuration Specifications.
static const FS_NAND_PART_PARAM BSP_FS_NAND_PartParam_Cfg = {
  .BlkCnt = 2048u,                                              // Total Number of Blocks.
  .PgPerBlk = 32u,                                              // Number of Pages per Block.
  .PgSize = 512u,                                               // Size (in Octets) of each Page.
  .SpareSize = 16u,                                             // Size (Octets) of Spare Area Per Pg.
  .SupportsRndPgPgm = DEF_NO,                                   // Supports Random Page Programming.
  .NbrPgmPerPg = 3u,                                            // Nbr of Program Operations Per Pg.
  .BusWidth = 8u,                                               // Bus Width of NAND Device.
  .ECC_NbrCorrBits = 1u,                                        // 1-Bit Correction, 2-Bit Detection.
  .ECC_CodewordSize = 1u,                                       // Size (in Bytes) of _NbrCorrBits.
  .DefectMarkType = DEFECT_SPARE_B_6_W_1_PG_1_OR_2,             // 6th Byte in Spare of 1st Pg != FFh.
  .MaxBadBlkCnt = 40u,                                          // Max Number of Bad Blocks in Device.
  .MaxBlkErase = 100000u                                        // Max Number of Erase OPs per Blk.
};

//                                                                 Chip Info : Free Spare Map for Micron - NAND256W3A.
static const FS_NAND_FREE_SPARE_DATA BSP_FS_NAND_FreeSpareMapTbl[] = {
  { 1u, 15u },
  { -1, -1  }
};

//                                                                 Chip Info : NAND Memory Chip Config Structure.
static const FS_NAND_PART_HW_INFO BSP_FS_NAND_Part_HwInfo = {
  .PartParamPtr = &BSP_FS_NAND_PartParam_Cfg,
  .FreeSpareMap = BSP_FS_NAND_FreeSpareMapTbl,
  .PartSlaveID = 0u,
};
//                                                                 ------------- RTOS-FS SPECIFIC HW INFO -------------
//                                                                 HW Info : NAND Controller + Chip Specific Config(s).
const FS_NAND_HW_INFO BSP_FS_NAND_HwInfo = FS_NAND_HW_INFO_INIT(&BSP_FS_NAND_CtrlrGen_HwInfo,
                                                                &BSP_FS_NAND_Part_HwInfo);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          BSP_FS_NAND_Init()
 *
 * @brief    Initializes internal resources needed by the NAND BSP.
 *
 * @param    isr_fnct    Interrupt service routine to call when an interruption from the NAND memory
 *                       controller occurs.
 *
 * @param    p_drv       Pointer to NAND memory controller driver data structure. This must be passed
 *                       to the interrupt service routine 'isr_fnct'.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) This function will be called EVERY time the device is opened.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NAND_Init(FS_NAND_CTRLR_DRV_ISR_HANDLE_FNCT isr_fnct,
                                    FS_NAND_CTRLR_DRV                 *p_drv)
{
  EBI_Init_TypeDef ebi_nand_cfg;

  PP_UNUSED_PARAM(isr_fnct);                                    // EBI driver does not use interrupt for NAND xfers.
  PP_UNUSED_PARAM(p_drv);

  CMU_ClockEnable(cmuClock_EBI, DEF_ON);                        // Enable the EBI Clock Peripheral.

  ebi_nand_cfg.mode = ebiModeD8A8;                              // EBI Cfg : 8-bit Address + 8-bit Data Mode.
  ebi_nand_cfg.ardyPolarity = ebiActiveLow;                     // : Address Ready     Pin Polarity - Active Low
  ebi_nand_cfg.alePolarity = ebiActiveLow;                      // : Addr Latch Enable Pin Polarity - Active Low
  ebi_nand_cfg.wePolarity = ebiActiveLow;                       // : Write Enable      Pin Polarity - Active Low
  ebi_nand_cfg.rePolarity = ebiActiveLow;                       // : Read  Enable      Pin Polarity - Active Low
  ebi_nand_cfg.csPolarity = ebiActiveLow;                       // : Chip Select       Pin Polarity - Active Low
  ebi_nand_cfg.blPolarity = ebiActiveLow;                       // : Byte Lane         Pin Polarity - Active Low
  ebi_nand_cfg.blEnable = DEF_FALSE;                            // : Byte Lane Support  - Enable/Disabled.
  ebi_nand_cfg.noIdle = DEF_TRUE;                               // : Idle State "Insert" Between xfer - En/Dis.
  ebi_nand_cfg.ardyEnable = DEF_FALSE;                          // : Address Ready Support - Disable, Use GPIO.
  ebi_nand_cfg.ardyDisableTimeout = DEF_TRUE;                   // : Turn Off 32-Cycle Timeout Ability for ARDY.
  ebi_nand_cfg.banks = EBI_BANK0;                               // : Address Bank 0 Select.
  ebi_nand_cfg.csLines = 0u;                                    // : Chip Select Line 0 - Selected.
  ebi_nand_cfg.addrSetupCycles = 0u;                            // : Cycles Delayed after ALE is Set.
  ebi_nand_cfg.addrHoldCycles = 0u;                             // : Cycles Addr is Driven onto ADDRDAT B4 ALE.
  ebi_nand_cfg.addrHalfALE = DEF_FALSE;                         // : Disable Half-Cycle ALE Strobe in Addr Cycle
  ebi_nand_cfg.readSetupCycles = 0u;                            // : Cycles for Addr setup before RE is Set.
  ebi_nand_cfg.readStrobeCycles = 2u;                           // : Cycles for RE to be Held Active.
  ebi_nand_cfg.readHoldCycles = 1u;                             // : Cycles for CS to be Active after RE is Clr.
  ebi_nand_cfg.readPageMode = DEF_FALSE;                        // : Disable Page Mode Reads.
  ebi_nand_cfg.readPrefetch = DEF_FALSE;                        // : Disable Prefetching from Seq Address.
  ebi_nand_cfg.readHalfRE = DEF_FALSE;                          // : Disable Half-Cycle duration of RE Strobe.
  ebi_nand_cfg.writeSetupCycles = 0u;                           // : Cycles for Addr Setup before WE is Set.
  ebi_nand_cfg.writeStrobeCycles = 2u;                          // : Cycles for WE to be Held Active.
  ebi_nand_cfg.writeHoldCycles = 1u;                            // : Cycles for CS to be Active after WE is Clr.
  ebi_nand_cfg.writeBufferDisable = DEF_FALSE;                  // : Enable the Write Buffer.
  ebi_nand_cfg.writeHalfWE = DEF_FALSE;                         // : Disable Half-Cycle duration of WE Strobe.
  ebi_nand_cfg.aLow = ebiALowA24;                               // : Lower Addr Pin Limit - Addr 24 and Higher.
  ebi_nand_cfg.aHigh = ebiAHighA26;                             // : High  Addr Pin Limit - Addr 26 and Higher.
  ebi_nand_cfg.location = ebiLocation1;                         // : Use Pin Location #1.
  ebi_nand_cfg.enable = DEF_TRUE;                               // : EBI should be Enabled after Configuration.

  EBI_Init(&ebi_nand_cfg);                                      // Initialize the EBI Peripheral based on Config.

  DEF_BIT_SET(EBI->NANDCTRL, ebi_nand_cfg.banks);               // Select EBI memory bank connected to NAND flash.

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         BSP_FS_NAND_ClkCfg()
 *
 * @brief    Initializes clock(s) needed by the NAND memory controller.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) This function will be called EVERY time the device is opened.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NAND_ClkCfg(void)
{
  CMU_ClockEnable(cmuClock_GPIO, DEF_ON);                       // Enable the GPIO Clock Peripheral.

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                         BSP_FS_NAND_IO_Cfg()
 *
 * @brief    Initializes Input/Output needed by the NAND memory controller.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) This function will be called EVERY time the device is opened.
 *
 * @note     (2) If NAND flash device is connected to EBI, only some pins specific to NAND are configured
 *               here. Other requires NAND pins are configured by the EBI BSP which sets the pins shared
 *               by other external devices connected to the EBI.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NAND_IO_Cfg(void)
{
  //                                                               ----------------- PIN CONFIGURATION ----------------
  //                                                               See Note #2.
  GPIO_PinModeSet(BSP_NAND_ALE_PORT,                            // ALE Pin : Address Latch Enable Pin. Port C, Pin 1.
                  BSP_NAND_ALE_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_OFF);                                     // : Initial Setting for Output, Line is Low.

  GPIO_PinModeSet(BSP_NAND_CLE_PORT,                            // CLE Pin : Command Latch Enable Pin . Port C, Pin 2.
                  BSP_NAND_CLE_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_OFF);                                     // : Initial Setting for Output, Line is Low.

  GPIO_PinModeSet(BSP_NAND_WP_PORT,                             // WP Pin  : Write Protect Pin. Port D, Pin 13.
                  BSP_NAND_WP_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_ON);                                      // : Active Low Write-Protect, Initially 'OFF'.

  GPIO_PinModeSet(BSP_NAND_CE_PORT,                             // CE Pin  : Chip Enable Pin. Port D, Pin 14.
                  BSP_NAND_CE_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_ON);                                      // : Active Low Chip-Enable, Initially 'OFF'.

  GPIO_PinModeSet(BSP_NAND_RB_PORT,                             // R/B Pin : Ready / Busy Pin. Port D, Pint 15.
                  BSP_NAND_RB_PIN,
                  gpioModeInput,                                // : Input Mode Set.
                  DEF_OFF);                                     // : Input Pin, Not Needed.

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 0 - Port E, Pin  8. EBI_AD0.
                  BSP_NAND_IO0_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_OFF);                                     // : Initial Setting for Output, Line is Low.

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 1 - Port E, Pin  9. EBI_AD1.
                  BSP_NAND_IO1_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 2 - Port E, Pin 10. EBI_AD2.
                  BSP_NAND_IO2_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 3 - Port E, Pin 11. EBI_AD3.
                  BSP_NAND_IO3_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 4 - Port E, Pin 12. EBI_AD4.
                  BSP_NAND_IO4_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 5 - Port E, Pin 13. EBI_AD5.
                  BSP_NAND_IO5_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 6 - Port E, Pin 14. EBI_AD6.
                  BSP_NAND_IO6_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_IOn_PORT,                            // I/O Pin : I/O Pin 7 - Port E, Pin 15. EBI_AD7.
                  BSP_NAND_IO7_PIN,                             // : Same as Pin Above.
                  gpioModePushPull,
                  DEF_OFF);

  GPIO_PinModeSet(BSP_NAND_WE_PORT,                             // WE Pin  : Write Enable Pin. Port F, Pin 8.
                  BSP_NAND_WE_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_ON);                                      // : Initial Setting for Output, Line is High.

  GPIO_PinModeSet(BSP_NAND_RE_PORT,                             // RE Pin  : Read Enable Pin. Port F, Pin 9.
                  BSP_NAND_RE_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_ON);                                      // : Initial Setting for Output, Line is High.

  GPIO_PinModeSet(BSP_NAND_PE_PORT,                             // PE Pin  : Power Enable Pin. Port B, Pin 15.
                  BSP_NAND_PE_PIN,
                  gpioModePushPull,                             // : Push Pull Mode Set (Output).
                  DEF_ON);                                      // : Initial Setting for Output, Chip is 'ON'.

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                        BSP_FS_NAND_ChipSelEn()
 *
 * @brief    Selects the NAND flash device using the Chip Enable signal.
 *
 * @param    part_slave_id   Slave ID of the NAND chip in case parallel bus shared with other external
 *                           devices. See Note #1.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) If the NAND flash chip is the only device connected to the parallel bus, the slave
 *               ID can be ignored.
 *
 * @note     (2) The NAND flash is the only external parallel device connected to the EFM32GG MCU EBI
 *               peripheral on the EFM32GG-STK3700a board. Thus the slave ID can be ignored.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NAND_ChipSelEn(CPU_INT16U part_slave_id)
{
  PP_UNUSED_PARAM(part_slave_id);                               // See Note #2.

  GPIO_PinOutClear(BSP_NAND_CE_PORT, BSP_NAND_CE_PIN);          // Clr 'Chip Enable' Pin (Active Low - Port D, Pin 14).

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       BSP_FS_NAND_ChipSelDis()
 *
 * @brief    Unselects the NAND flash device using the Chip Enable signal.
 *
 * @param    part_slave_id   slave ID of the NAND chip in case parallel bus shared with other external
 *                           devices. See Note #1.
 *
 * @return   DEF_OK,   if successful.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) If the NAND flash chip is the only device connected to the parallel bus, the slave
 *               ID can be ignored.
 *
 * @note     (2) The NAND flash is the only external parallel device connected to the EFM32GG MCU EBI
 *               peripheral on the EFM32GG-STK3700a board. Thus the slave ID can be ignored.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NAND_ChipSelDis(CPU_INT16U part_slave_id)
{
  PP_UNUSED_PARAM(part_slave_id);                               // See Note #2.

  GPIO_PinOutSet(BSP_NAND_CE_PORT, BSP_NAND_CE_PIN);            // Set 'Chip Enable' Pin (Active Low - Port D, Pin 14).

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                        BSP_FS_NAND_IsChipRdy()
 *
 * @brief    Determines if the NAND flash device is ready using the Ready/Busy signal.
 *
 * @return   DEF_YES, if chip is ready.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN BSP_FS_NAND_IsChipRdy(void)
{
  CPU_BOOLEAN rdy = DEF_NO;

  //                                                               Test Ready/Busy output of NAND chip.
  if (DEF_BIT_IS_CLR(GPIO->P[BSP_NAND_RB_PORT].DIN, (1u << BSP_NAND_RB_PIN)) == DEF_YES) {
    rdy = DEF_NO;                                               // Ready/Busy low: rd, pgm or erase op in progress.
  } else {
    rdy = DEF_YES;                                              // Ready/Busy high: rd, pgm or erase op complete.
  }

  return (rdy);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_FS_AVAIL && RTOS_MODULE_FS_STORAGE_NAND_AVAIL
