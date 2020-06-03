/*
*********************************************************************************************************
*                                             EXAMPLE CODE
*********************************************************************************************************
* Licensing:
*   The licensor of this EXAMPLE CODE is Silicon Laboratories Inc.
*
*   Silicon Laboratories Inc. grants you a personal, worldwide, royalty-free, fully paid-up license to
*   use, copy, modify and distribute the EXAMPLE CODE software, or portions thereof, in any of your
*   products.
*
*   Your use of this EXAMPLE CODE is at your own risk. This EXAMPLE CODE does not come with any
*   warranties, and the licensor disclaims all implied warranties concerning performance, accuracy,
*   non-infringement, merchantability and fitness for your application.
*
*   The EXAMPLE CODE is provided "AS IS" and does not come with any support.
*
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*
*   You can contact us at: https://www.micrium.com
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                     NAND MEMORY CONTROLLER BSP
*
*                                              TEMPLATE
*
* File : bsp_fs_nand.c
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                     DEPENDENCIES & AVAIL CHECK(S)
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#if  defined(RTOS_MODULE_FS_AVAIL) && defined(RTOS_MODULE_FS_STORAGE_NAND_AVAIL)


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>

#include  <drivers/fs/include/fs_nand_ctrlr_drv.h>
#include  <fs/include/fs_nand_ctrlr_gen.h>
#include  <fs/include/fs_nand_ctrlr_gen_ext_soft_ecc.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_BSP

#define  BSP_MEM_CTRLR_BASE_ADDR            0x00000000u     /* TODO: change for the NAND memory controller base address of your MCU. */


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_Init      (FS_NAND_CTRLR_DRV_ISR_HANDLE_FNCT   isr_fnct,
                                            FS_NAND_CTRLR_DRV                  *p_drv);

static  CPU_BOOLEAN  BSP_FS_NAND_ClkCfg    (void);

static  CPU_BOOLEAN  BSP_FS_NAND_IO_Cfg    (void);

static  CPU_BOOLEAN  BSP_FS_NAND_IntCfg    (void);

static  CPU_BOOLEAN  BSP_FS_NAND_ChipSelEn (CPU_INT16U                          part_slave_id);

static  CPU_BOOLEAN  BSP_FS_NAND_ChipSelDis(CPU_INT16U                          part_slave_id);

static  CPU_BOOLEAN  BSP_FS_NAND_IsChipRdy (void);


/*
*********************************************************************************************************
*                                         INTERFACE STRUCTURE
*********************************************************************************************************
*/
                                                                /* ---------------- NAND BSP API STRUCT --------------- */
static const  FS_NAND_CTRLR_BSP_API  BSP_FS_NAND_BSP_API = {
    .Init       = BSP_FS_NAND_Init,
    .ClkCfg     = BSP_FS_NAND_ClkCfg,
    .IO_Cfg     = BSP_FS_NAND_IO_Cfg,
    .IntCfg     = BSP_FS_NAND_IntCfg,
    .ChipSelEn  = BSP_FS_NAND_ChipSelEn,
    .ChipSelDis = BSP_FS_NAND_ChipSelDis,
    .IsChipRdy  = BSP_FS_NAND_IsChipRdy
};

                                                                /* --------------- CHIP SPECIFIC CONFIG --------------- */
                                                                /* Chip Info : 'STATIC' Configuration Specifications.   */
static  const  FS_NAND_PART_PARAM  BSP_FS_NAND_PartParam_Cfg = {
    .BlkCnt           = 2048u,                                  /* Total Number of Blocks.                              */
    .PgPerBlk         = 32u,                                    /* Number of Pages per Block.                           */
    .PgSize           = 512u,                                   /* Size (in Octets) of each Page.                       */
    .SpareSize        = 16u,                                    /* Size (Octets) of Spare Area Per Pg.                  */
    .SupportsRndPgPgm = DEF_NO,                                 /* Supports Random Page Programming.                    */
    .NbrPgmPerPg      = 3u,                                     /* Nbr of Program Operations Per Pg.                    */
    .BusWidth         = 8u,                                     /* Bus Width of NAND Device.                            */
    .ECC_NbrCorrBits  = 1u,                                     /* 1-Bit Correction, 2-Bit Detection.                   */
    .ECC_CodewordSize = 1u,                                     /* Size (in Bytes) of _NbrCorrBits.                     */
    .DefectMarkType   = DEFECT_SPARE_B_6_W_1_PG_1_OR_2,         /* 6th Byte in Spare of 1st Pg != FFh.                  */
    .MaxBadBlkCnt     = 40u,                                    /* Max Number of Bad Blocks in Device.                  */
    .MaxBlkErase      = 100000u                                 /* Max Number of Erase OPs per Blk.                     */
};
                                                                /* TODO: define the free spare map specific to the NAND memory chip.*/
static const FS_NAND_FREE_SPARE_DATA     BSP_FS_NAND_FreeSpareMapTbl[] = {{0x04u,  4u},
                                                                          {0x14u,  4u},
                                                                          {0x24u,  4u},
                                                                          {0x34u,  4u},
                                                                          {  -1 , -1 }};

                                                                /* Chip Info : NAND Memory Chip Config Structure.       */
static  const  FS_NAND_PART_HW_INFO  BSP_FS_NAND_Part_HwInfo = {
    .PartParamPtr = &BSP_FS_NAND_PartParam_Cfg,
    .FreeSpareMap = BSP_FS_NAND_FreeSpareMapTbl,
    .PartSlaveID  = 0u,
};
                                                                /* NAND controller hardware info.                       */
static  const  FS_NAND_CTRLR_GEN_HW_INFO  BSP_FS_NAND_CtrlrGen_HwInfo = FS_NAND_CTRLR_GEN_HW_INFO_INIT(&FS_NAND_CtrlrGen_SoftEcc_Hamming_HwInfo,
                                                                                                       DEF_NULL,
                                                                                                       &BSP_FS_NAND_BSP_API,
                                                                                                       0x00000000,
                                                                                                       0x00000000,
                                                                                                       sizeof(CPU_ALIGN));

                                                                /* NAND info composed of NAND controller and memory chip*/
       const  FS_NAND_HW_INFO            BSP_FS_NAND_HwInfo           = FS_NAND_HW_INFO_INIT(&BSP_FS_NAND_CtrlrGen_HwInfo,
                                                                                             &BSP_FS_NAND_Part_HwInfo);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           BSP_FS_NAND_Init()
*
* Description : Initializes internal resources needed by the NAND BSP.
*
* Argument(s) : isr_fnct    Interrupt service routine to call when an interruption from the NAND memory
*                           controller occurs.
*
*               p_drv       Pointer to NAND memory controller driver data structure. This must be passed
*                           to the isr_fnct.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : (1) This function will be called EVERY time the device is opened.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_Init  (FS_NAND_CTRLR_DRV_ISR_HANDLE_FNCT   isr_fnct,
                                        FS_NAND_CTRLR_DRV                  *p_drv)
{
    PP_UNUSED_PARAM(isr_fnct);
    PP_UNUSED_PARAM(p_drv);

    /* TODO: add to create/allocate any software ressources required by your BSP. */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         BSP_FS_NAND_ClkCfg()
*
* Description : Initializes clock(s) needed by the NAND memory controller.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : (1) This function will be called EVERY time the device is opened.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_ClkCfg (void)
{
    /* TODO: perform hardware configuration related to the NAND memory controller's clock.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         BSP_FS_NAND_IO_Cfg()
*
* Description : Initializes Input/Output needed by the NAND memory controller.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : (1) This function will be called EVERY time the device is opened.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_IO_Cfg (void)
{
    /* TODO: perform hardware configuration related to the NAND memory controller's I/Os.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                         BSP_FS_NAND_IntCfg()
*
* Description : Initializes interrupts needed by the NAND memory controller.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_IntCfg (void)
{
    /* TODO: perform hardware configuration related to the NAND memory controller's interrupts.*/

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_FS_NAND_ChipSelEn()
*
* Description : Selects the NAND flash device using the Chip Enable signal.
*
* Argument(s) : part_slave_id   slave ID of the NAND chip in case parallel bus shared with other external
*                               devices. See Note #1.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : (1) If the NAND flash chip is the only device connected to the parallel bus, the slave
*                   ID can be ignored.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_ChipSelEn (CPU_INT16U  part_slave_id)
{
    PP_UNUSED_PARAM(part_slave_id);                             /* See Note #2.                                         */

    /* TODO: add code to select the NAND flash device using the Chip Enable signal. Usually an I/O register allows to do it. */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                       BSP_FS_NAND_ChipSelDis()
*
* Description : Unselects the NAND flash device using the Chip Enable signal.
*
* Argument(s) : part_slave_id   slave ID of the NAND chip in case parallel bus shared with other external
*                               devices. See Note #1.
*
* Return(s)   : DEF_OK,   if successful.
*               DEF_FAIL, otherwise.
*
* Note(s)     : (1) If the NAND flash chip is the only device connected to the parallel bus, the slave
*                   ID can be ignored.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_ChipSelDis (CPU_INT16U  part_slave_id)
{
    PP_UNUSED_PARAM(part_slave_id);                             /* See Note #1.                                         */

    /* TODO: add code to unselect the NAND flash device using the Chip Enable signal. Usually an I/O register allows to do it. */

    return (DEF_OK);
}


/*
*********************************************************************************************************
*                                        BSP_FS_NAND_IsChipRdy()
*
* Description : Determines if the NAND flash device is ready using the Ready/Busy signal.
*
* Argument(s) : none.
*
* Return(s)   : DEF_YES, if chip is ready.
*               DEF_NO,  otherwise.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_BOOLEAN  BSP_FS_NAND_IsChipRdy (void)
{
    CPU_BOOLEAN  rdy = DEF_NO;

    /* TODO: add code to test Ready/Busy output of NAND chip.                         */
    /* In general, Ready/Busy low means read, program or erase operation in progress. */
    /* Ready/Busy high means read, program or erase operation complete.               */


    return (rdy);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                   DEPENDENCIES & AVAIL CHECK(S) END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif  /* RTOS_MODULE_FS_AVAIL && RTOS_MODULE_FS_STORAGE_NAND_AVAIL*/

