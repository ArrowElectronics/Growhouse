/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   IO
*
*                               (c) Copyright 2017; Silicon Laboratories Inc.
*                                           https://www.micrium.com
*********************************************************************************************************
* Licensing:
*           YOUR USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS OF A MICRIUM SOFTWARE LICENSE.
*   If you are not willing to accept the terms of an appropriate Micrium Software License, you must not
*   download or use this software for any reason.
*   Information about Micrium software licensing is available at https://www.micrium.com/licensing/
*   It is your obligation to select an appropriate license based on your intended use of the Micrium OS.
*   Unless you have executed a Micrium Commercial License, your use of the Micrium OS is limited to
*   evaluation, educational or personal non-commercial uses. The Micrium OS may not be redistributed or
*   disclosed to any third party without the written consent of Silicon Laboratories Inc.
*********************************************************************************************************
* Documentation:
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*********************************************************************************************************
* Technical Support:
*   Support is available for commercially licensed users of Micrium's software. For additional
*   information on support, you can contact info@micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                           SD CARD FNCT TEST
*
* File : sd_card_fnct_test.c
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       DEPENDENCIES & AVAIL CHECK(S)
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_IO_SD_AVAIL))

#if (!defined(RTOS_MODULE_IO_AVAIL))

#error IO SD module requires IO module. Make sure it is part of your project and that \
        RTOS_MODULE_IO_AVAIL is defined in rtos_description.h.

#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>


#include  <io/include/sd.h>
#include  <io/source/sd/sd_io_fnct_priv.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_IO


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
*                                           LOCAL DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/






/*
*********************************************************************************************************
*********************************************************************************************************
*                                               LOCAL TABLES
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


static  CPU_BOOLEAN  SD_IO_TestFnctProbe  (SD_BUS_HANDLE     bus_handle,
                                           SD_FNCT_HANDLE    fnct_handle,
                                    CPU_INT08U        sdio_fnct_if_code,
                                    void            **pp_fnct_data);

static  void         SD_IO_TestFnctConn   (SD_FNCT_HANDLE    fnct_handle,
                                    void             *p_fnct_data);

static  void         SD_IO_TestFnctInt    (SD_FNCT_HANDLE    fnct_handle,
                                    void             *p_fnct_data);

static  void         SD_IO_TestFnctDisconn(SD_FNCT_HANDLE    fnct_handle,
                                    void             *p_fnct_data);


/*
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*/


const  SD_IO_FNCT_DRV_API  SD_IO_TestFnctDrvAPI = {
    .CardFnctProbe   = SD_IO_TestFnctProbe,
    .CardFnctConn    = SD_IO_TestFnctConn,
    .CardFnctDisconn = SD_IO_TestFnctDisconn,
    .CardFnctInt     = SD_IO_TestFnctInt
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
KAL_SEM_HANDLE  sem;
void  SD_IO_TestTask (void *p_arg);
void  SD_IO_TestFnctInit(RTOS_ERR  *p_err)
{
    KAL_TASK_HANDLE  task_handle;


    SD_IO_FnctDrvReg(&SD_IO_TestFnctDrvAPI, p_err);

    sem = KAL_SemCreate("sem", DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
    }

    task_handle = KAL_TaskAlloc("test", DEF_NULL, 512u, DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
    }

    KAL_TaskCreate(task_handle, SD_IO_TestTask, DEF_NULL, 21u, DEF_NULL, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
    }
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

SD_BUS_HANDLE  BusHandle;
SD_FNCT_HANDLE  FnctHandle;
static  CPU_BOOLEAN  SD_IO_TestFnctProbe  (SD_BUS_HANDLE     bus_handle,
                                           SD_FNCT_HANDLE    fnct_handle,
                                           CPU_INT08U        sdio_fnct_if_code,
                                           void            **pp_fnct_data)
{
    BusHandle = bus_handle;
    FnctHandle = fnct_handle;

    return (DEF_OK);
}

#define  TEST_BUF_LEN  48u
CPU_INT08U  Buf[/*TEST_BUF_LEN*/4096u*2u], RefBuf[TEST_BUF_LEN];



#define  TEST_BLK_LEN  4u
#define  TEST_BLK_QTY  64u
CPU_INT08U  BlkBuf[TEST_BLK_LEN * TEST_BLK_QTY], RefBlkBuf[TEST_BLK_LEN * TEST_BLK_QTY];

static  void         SD_IO_TestFnctConn   (SD_FNCT_HANDLE    fnct_handle,
                                           void             *p_fnct_data)
{
    RTOS_ERR  err;
    KAL_SemPost(sem, KAL_OPT_POST_NONE, &err);

    printf("IO-SD app conn\r\n");
}

static  void         SD_IO_TestFnctInt    (SD_FNCT_HANDLE    fnct_handle,
                                    void             *p_fnct_data)
{

}

static  void         SD_IO_TestFnctDisconn(SD_FNCT_HANDLE    fnct_handle,
                                    void             *p_fnct_data)
{
    printf("IO-SD app disconn\r\n");
}



void  SD_IO_TestTask (void *p_arg)
{
    while (1) {
        RTOS_ERR  err;

        CPU_INT16U  cnt;
        CPU_ADDR  p_buf;


        KAL_SemPend(sem, KAL_OPT_PEND_NONE, 0, &err);

#if 1 // CM debug
        Mem_Set(Buf, 0xFF, TEST_BUF_LEN);
        Mem_Set(BlkBuf, 0xFF, TEST_BLK_LEN * TEST_BLK_QTY);

        p_buf = (CPU_ADDR)&Buf[0u];

        p_buf += 4096u - (p_buf % 4096u) -12u;

        SD_IO_FnctRd(BusHandle,
                     0u/*fnct_handle*/,
                     0u,
                     (CPU_INT08U *)p_buf,
                     TEST_BUF_LEN - 8u,
                     DEF_NO,
                    &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            CPU_SW_EXCEPTION(;);
        }

        for (cnt = 0u; cnt < TEST_BUF_LEN; cnt++) {
            RefBuf[cnt] = SD_IO_FnctRdByte(BusHandle,
                                            0u/*fnct_handle*/,
                                            cnt,
                                           &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }
        }

        if (!Mem_Cmp((CPU_INT08U *)p_buf, RefBuf, 8u)) {
            CPU_SW_EXCEPTION(;);
        }



        SD_IO_FnctRd(BusHandle,
                     0u/*fnct_handle*/,
                     2u,
                     Buf,
                     TEST_BUF_LEN,
                     DEF_YES,
                    &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            CPU_SW_EXCEPTION(;);
        }


    //    CPU_INT08U  TestWr[] = {0x00u, 0x00u, 0x0Du, 0x0Cu, 0x0Bu, 0x0Au, 0x09u, 0x08u, 0x07u, 0x06u, 0x05u, 0x04u, 0x03u, 0x02u, 0x01u, 0x00u};
    //    SD_IO_FnctWr(BusHandle,
    //                 0u/*fnct_handle*/,
    //                 2u,
    //                 TestWr,
    //                 1u,
    //                 DEF_NO,
    //                &err);
    //    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    //        CPU_SW_EXCEPTION(;);
    //    }
    //
    //
    //    SD_IO_FnctRd(BusHandle,
    //                 0u/*fnct_handle*/,
    //                 2u,
    //                 Buf,
    //                 4,
    //                 DEF_NO,
    //                &err);
    //    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    //        CPU_SW_EXCEPTION(;);
    //    }


        CPU_BOOLEAN  is_blk_mode_ok;

        is_blk_mode_ok = SD_IO_IsBlkOperSupported(BusHandle, &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            CPU_SW_EXCEPTION(;);
        }

        if (is_blk_mode_ok) {
            CPU_INT16U  current_blk_size, current_blk_size_fnct0;


            current_blk_size = SD_IO_FnctBlkSizeGet(BusHandle, FnctHandle, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }


            SD_IO_FnctBlkSizeSet(BusHandle, FnctHandle, TEST_BLK_LEN, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            current_blk_size = SD_IO_FnctBlkSizeGet(BusHandle, FnctHandle, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }


            current_blk_size_fnct0 = SD_IO_FnctBlkSizeGet(BusHandle, SD_IO_FNCT_0, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            SD_IO_FnctBlkSizeSet(BusHandle, SD_IO_FNCT_0, TEST_BLK_LEN, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            current_blk_size_fnct0 = SD_IO_FnctBlkSizeGet(BusHandle, SD_IO_FNCT_0, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            BlkBuf[0u] = 0u;
            BlkBuf[1u] = 0u;
            BlkBuf[2u] = 0u;
            BlkBuf[3u] = 0u;
            SD_IO_FnctWr(BusHandle, FnctHandle, 0x10u, BlkBuf, 4u, DEF_NO, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            BlkBuf[0u] = 0xAAu;
            BlkBuf[1u] = 0xBBu;
            BlkBuf[2u] = 0xCCu;
            BlkBuf[3u] = 0xEEu;
            SD_IO_FnctWrBlk(BusHandle, FnctHandle, 0x14u, BlkBuf, 1u, DEF_YES, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            SD_IO_FnctRdBlk(BusHandle, FnctHandle, 0x00u, RefBlkBuf, 4u, DEF_YES, &err);
            if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                CPU_SW_EXCEPTION(;);
            }

            for (cnt = 0u; cnt < 4u * TEST_BLK_LEN; cnt++) {
                RefBuf[cnt] = SD_IO_FnctRdByte(BusHandle,
                                                FnctHandle,
                                                cnt + 0x00u,
                                               &err);
                if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                    CPU_SW_EXCEPTION(;);
                }
            }

            if (!Mem_Cmp(RefBlkBuf, RefBuf, 4u * TEST_BLK_LEN)) {
                CPU_SW_EXCEPTION(;);
            }

            //SD_IO_FnctRdBlk(BusHandle, SD_IO_FNCT_0, 0u, BlkBuf, 2u, DEF_YES, &err);
            //if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            //    CPU_SW_EXCEPTION(;);
            //}



        }
#endif
    }
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                   DEPENDENCIES & AVAIL CHECK(S) END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif  /* (defined(RTOS_MODULE_IO_SD_AVAIL)) */
