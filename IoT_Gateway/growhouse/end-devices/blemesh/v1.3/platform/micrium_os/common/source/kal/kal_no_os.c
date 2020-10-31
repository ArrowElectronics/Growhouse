/*
*********************************************************************************************************
*                                             uC/OS-Common
*                                  Common Features for Micrium Stacks
*
*                          (c) Copyright 2013-2016; Micrium, Inc.; Weston, FL
*                    All rights reserved. Protected by international copyright laws.
*                                        http://www.micrium.com
*
*********************************************************************************************************
* Licensing Terms:
*
*   uC/OS is professional-grade software that is provided by Micrium in source form for evaluation only.
*   The evaluation period for the software is 45 days.  If you would like to use the software beyond this
*   period, you must contact Micrium for a license.
*   Micrium offers four license types:
*
*        Educational: This FREE license entitles its bearer to use the licensed source code in academic
*                     projects ONLY.
*
*        Maker:       This FREE license entitles its bearer to use the licensed source code in personal
*                     (non-commercial) projects ONLY.
*
*        Startup:     This FREE license entitles its bearer to use the licensed source code in
*                     commercial projects only if used by pre-revenue startups or startups meeting
*                     certain criteria. Contact sales@micrium.com to learn if you qualify.
*
*        Commercial:  This license type is required for any commercial entity. Contact sales@micrium.com
*                     to discuss licensing options.
*
*   Micrium reserves the right to refuse or revoke FREE software licenses for any reason.  Unlicensed use
*   of the software beyond the 45-day evaluation period is NOT allowed, and the software may NOT be
*   redistributed without written authorization from Micrium.  If you would like additional information
*   on licensing, you can contact info@micrium.com.
*
*   Please help us continue to provide the Embedded community with the finest software available.
*
*   Your honesty is greatly appreciated.
*********************************************************************************************************
* Documentation:
*   You can find user manuals, API references, release notes and more at: https://doc.micrium.com
*********************************************************************************************************
* Technical Support:
*   Support is available for commercially licensed users of Micrium's software.  For additional
*   information on support, you can contact info@micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                        KERNEL ABSTRACTION LAYER (KAL) - NO OPERATING SYSTEM
*
* File : kal_no_os.c
*********************************************************************************************************
* Note(s) : (1) Since no operating system is present, most of the present features are implemented
*               directly in the KAL.
*
*           (2) Since no operating system is present, active wait is used and may considerably reduce
*               performance.
*
*           (3) Since no operating system is present, it is assumed that there is no multi-tasking.
*               Therefore, the locking mechanism can be reduced to nothing, since the execution flow can
*               only be pre-empted by an interrupt and it is impossible to pend on a lock in an interrupt
*               context.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                     DEPENDENCIES & AVAIL CHECK(S)
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <rtos_description.h>

#if (!defined(RTOS_MODULE_KERNEL_AVAIL))


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>


#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_mem.h>
#include  <common/include/toolchains.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/common/common_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  LOG_DFLT_CH (COMMON, KAL)
#define  RTOS_MODULE_CUR                        RTOS_CFG_MODULE_COMMON
#define  KAL_NO_OS_MAX_TASK_NB                  1


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

                                                                /* ------------------- KAL SEM TYPE ------------------- */
typedef  struct  kal_sem {
    volatile  CPU_INT32U  Cnt;                                  /* Cnt used by sem.                                     */
} KAL_SEM;


typedef  struct  kal_task {
           void      (*TaskFnct) (void  *p_arg);
           void      *TaskEntryArg;                             /* Argument passed to task when it was created          */
    const  CPU_CHAR  *NamePtr;                                  /* Task name string.                                    */
} KAL_TASK;


                                                                /* ----------------- KAL INTERNAL DATA ---------------- */
typedef  struct  kal_data {
    MEM_SEG      *MemSegPtr;                                    /* Mem Seg to alloc from.                               */
    MEM_DYN_POOL  SemPool;                                      /* Pool for sems.                                       */
    KAL_TASK      TaskHandle;
    CPU_INT08U    TaskCnt;                                      /* Number of tasks allocated                            */
} KAL_DATA;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL CONSTANTS
*********************************************************************************************************
*********************************************************************************************************
*/

const  CPU_INT32U           KAL_Version           =  10000u;
const  KAL_TASK_HANDLE      KAL_TaskHandleNull    = KAL_OBJ_HANDLE_NULL;
const  KAL_LOCK_HANDLE      KAL_LockHandleNull    = KAL_OBJ_HANDLE_NULL;
const  KAL_SEM_HANDLE       KAL_SemHandleNull     = KAL_OBJ_HANDLE_NULL;
const  KAL_TMR_HANDLE       KAL_TmrHandleNull     = KAL_OBJ_HANDLE_NULL;
const  KAL_Q_HANDLE         KAL_QHandleNull       = KAL_OBJ_HANDLE_NULL;
const  KAL_TASK_REG_HANDLE  KAL_TaskRegHandleNull = KAL_OBJ_HANDLE_NULL;
const  KAL_MON_HANDLE       KAL_MonHandleNull     = KAL_OBJ_HANDLE_NULL;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              LOCAL TABLES
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*********************************************************************************************************
*/

static  KAL_DATA   *KAL_DataPtr = DEF_NULL;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

static  void  KAL_Init(RTOS_ERR  *p_err);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         KAL CORE API FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           KAL_FeatureQuery()
*
* @brief    Check if specified feature is available.
*
* @param    feature     Feature to query.
*
* @param    opt         Option associated with the feature requested.
*
* @return   DEF_YES, if feature is available.
*           DEF_NO, otherwise.
*********************************************************************************************************
*/

CPU_BOOLEAN  KAL_FeatureQuery (KAL_FEATURE  feature,
                               KAL_OPT      opt)
{
    CPU_BOOLEAN  is_en;


    is_en = DEF_NO;

    switch (feature) {
        case KAL_FEATURE_TASK_CREATE:                           /* ---------------------- TASKS ----------------------- */
        case KAL_FEATURE_TASK_DEL:
             is_en = DEF_YES;
             break;


        case KAL_FEATURE_LOCK_CREATE:                           /* ---------------------- LOCKS ----------------------- */
        case KAL_FEATURE_LOCK_ACQUIRE:
        case KAL_FEATURE_LOCK_RELEASE:
        case KAL_FEATURE_LOCK_DEL:
             is_en = DEF_YES;
             break;


        case KAL_FEATURE_SEM_CREATE:                            /* ----------------------- SEMS ----------------------- */
        case KAL_FEATURE_SEM_DEL:
        case KAL_FEATURE_SEM_POST:
             is_en = DEF_YES;
             break;


        case KAL_FEATURE_SEM_PEND:
             if (DEF_BIT_IS_SET(opt, KAL_OPT_PEND_NON_BLOCKING) == DEF_YES) {
                 is_en = DEF_YES;
             } else {
                 #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
                     is_en = DEF_YES;
                 #endif
             }
             break;


        case KAL_FEATURE_DLY:                                   /* ----------------------- DLYS ----------------------- */
             #if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
                 is_en = DEF_YES;
             #endif
             break;


        case KAL_FEATURE_PEND_TIMEOUT:
             is_en = DEF_YES;
             break;


        case KAL_FEATURE_TASK_REG:                              /* --------------------- TASK REGS -------------------- */
        case KAL_FEATURE_MON:                                  /* ---------------------- MONITORS --------------------- */
        case KAL_FEATURE_MON_DEL:
        case KAL_FEATURE_SEM_ABORT:
        case KAL_FEATURE_SEM_SET:
        case KAL_FEATURE_Q_CREATE:                              /* ---------------------- QUEUES ---------------------- */
        case KAL_FEATURE_Q_POST:
        case KAL_FEATURE_Q_PEND:
        case KAL_FEATURE_TICK_GET:                              /* ------------------- TICK CTR INFO ------------------ */
        case KAL_FEATURE_TMR:
        case KAL_FEATURE_CPU_USAGE_GET:
        default:
             break;
    }

    return (is_en);
}


/*
*********************************************************************************************************
*                                         TASK API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_TaskAlloc()
*
* @brief    Allocate a task object and its stack.
*
* @param    p_name      Pointer to name of the task.
*
* @param    p_stk_base  Pointer to start of task stack. If NULL, the stack will be allocated from
*                       the KAL memory segment.
*
* @param    stk_size    Size (in CPU_STK elements) of the task stack.
*
* @param    p_cfg       Pointer to KAL task configuration structure.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NONE
*                           - RTOS_ERR_NOT_SUPPORTED
*                           - RTOS_ERR_SEG_OVF
*
* @return   Allocated task's handle.
*
* @note     (1) This function will initialize KAL if KAL has not yet been initialized.
*********************************************************************************************************
*/

KAL_TASK_HANDLE  KAL_TaskAlloc (const  CPU_CHAR          *p_name,
                                       CPU_STK           *p_stk_base,
                                       CPU_SIZE_T         stk_size_bytes,
                                       KAL_TASK_EXT_CFG  *p_cfg,
                                       RTOS_ERR          *p_err)
{
    KAL_TASK_HANDLE  handle = KAL_TaskHandleNull;


    PP_UNUSED_PARAM(p_stk_base);
    PP_UNUSED_PARAM(stk_size_bytes);
                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);
    RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

    if (KAL_DataPtr == DEF_NULL) {
        KAL_Init(p_err);                                        /* KAl initialization (see Note #1).                    */
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (handle);
        }
    }

    if (KAL_DataPtr->TaskCnt < KAL_NO_OS_MAX_TASK_NB) {
        KAL_DataPtr->TaskHandle.NamePtr = p_name;
    } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    }

    return (handle);
}


/**
*********************************************************************************************************
*                                               KAL_TaskCreate()
*
* @brief    Register the task function.
*
* @param    task_handle     Handle of the task to create.
*
* @param    p_fnct          Pointer to task function.
*
* @param    p_task_arg      Pointer to argument that will be passed to task function (can be DEF_NULL).
*
* @param    prio            Task priority.
*
* @param    p_cfg           Pointer to KAL task configuration structure.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NONE
*                               - RTOS_ERR_NULL_PTR
*                               - RTOS_ERR_NOT_SUPPORTED
*                               - RTOS_ERR_NOT_INIT
*                               - RTOS_ERR_WOULD_OVF
*
* @note     (1) The task must be allocated prior to this call using KAL_TaskAlloc().
*********************************************************************************************************
*/

void  KAL_TaskCreate (KAL_TASK_HANDLE     task_handle,
                      void              (*p_fnct)(void  *p_arg),
                      void               *p_task_arg,
                      KAL_TASK_PRIO       prio,
                      KAL_TASK_EXT_CFG   *p_cfg,
                      RTOS_ERR           *p_err)
{
    PP_UNUSED_PARAM(task_handle);
    PP_UNUSED_PARAM(prio);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);
    RTOS_ASSERT_DBG_ERR_SET((p_fnct != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, ;);
    RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, ;);
    RTOS_ASSERT_DBG_ERR_SET((KAL_DataPtr != DEF_NULL), *p_err, RTOS_ERR_NOT_INIT, ;);

    if (KAL_DataPtr->TaskCnt < KAL_NO_OS_MAX_TASK_NB) {
        KAL_DataPtr->TaskHandle.TaskFnct     = p_fnct;
        KAL_DataPtr->TaskHandle.TaskEntryArg = p_task_arg;
        KAL_DataPtr->TaskCnt++;

        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    }
}


/**
*********************************************************************************************************
*                                           KAL_TaskRun()
*
* @brief    Single execution of the task function.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NONE
*********************************************************************************************************
*/

void  KAL_TaskRun (RTOS_ERR  *p_err)
{
                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);
    RTOS_ASSERT_DBG_ERR_SET((KAL_DataPtr->TaskHandle.TaskFnct != DEF_NULL), *p_err, RTOS_ERR_INVALID_HANDLE, ;);

    KAL_DataPtr->TaskHandle.TaskFnct(KAL_DataPtr->TaskHandle.TaskEntryArg);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}


/**
*********************************************************************************************************
*                                               KAL_TaskPrioSet()
*
* @brief    Change the priority of a task. Nothing to do as only one task is supported
*
* @param    task_handle     Handle of the task to change the priority.
*
* @param    prio            Task priority.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NONE
*********************************************************************************************************
*/

void  KAL_TaskPrioSet (KAL_TASK_HANDLE   task_handle,
                       KAL_TASK_PRIO     prio,
                       RTOS_ERR         *p_err)
{
    PP_UNUSED_PARAM(task_handle);
    PP_UNUSED_PARAM(prio);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}


/**
*********************************************************************************************************
*                                               KAL_TaskDel()
*
* @brief    Delete a task.
*
* @param    task_handle     Handle of the task to delete.
*********************************************************************************************************
*/

void  KAL_TaskDel (KAL_TASK_HANDLE  task_handle)
{
    PP_UNUSED_PARAM(task_handle);

    KAL_DataPtr->TaskHandle.NamePtr      = DEF_NULL;
    KAL_DataPtr->TaskHandle.TaskEntryArg = DEF_NULL;
    KAL_DataPtr->TaskHandle.TaskFnct     = DEF_NULL;
    KAL_DataPtr->TaskCnt--;
}


/**
*********************************************************************************************************
*                                           KAL_TaskUUID_Get()
*
* @brief    Get unique universal identificator (UUID) for current task.
*
* @note     (1) This function can only be used to obtain the UUID of the task that is currently
*               running.
*
* @note     (2) This function can be used to obtain the UUID of a task that has not been created by
*               KAL.
*********************************************************************************************************
*/

KAL_TASK_UUID  KAL_TaskUUID_Get (void)
{
    return (0u);
}


/*
*********************************************************************************************************
*                                         LOCK API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_LockCreate()
*
* @brief    Create a lock, which is unlocked by default. Without a kernel, nothing must be done.
*
* @param    p_name  Pointer to name of the lock.
*
* @param    p_cfg   Pointer to KAL lock configuration structure.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
*                   this function:
*                       - RTOS_ERR_NONE
*
* @return   Created lock handle.
*
* @note     (1) This function will initialize KAL if KAL has not yet been initialized.
*********************************************************************************************************
*/

KAL_LOCK_HANDLE  KAL_LockCreate (const  CPU_CHAR          *p_name,
                                        KAL_LOCK_EXT_CFG  *p_cfg,
                                        RTOS_ERR          *p_err)
{
    KAL_LOCK_HANDLE  handle = KAL_LockHandleNull;


    PP_UNUSED_PARAM(p_name);
    PP_UNUSED_PARAM(p_cfg);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

    if (KAL_DataPtr == DEF_NULL) {
        KAL_Init(p_err);                                        /* KAL initialization (see Note #1).                    */
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (handle);
        }
    } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    }

    return (handle);
}


/**
*********************************************************************************************************
*                                               KAL_LockAcquire()
*
* @brief    Acquire a lock. Without a kernel, nothing must be done.
*
* @param    lock_handle     Handle of the lock to acquire.
*
* @param    opt             Options available:
*                               - KAL_OPT_PEND_NONE:          block until timeout expires or lock is available.
*                               - KAL_OPT_PEND_BLOCKING:      block until timeout expires or lock is available.
*                               - KAL_OPT_PEND_NON_BLOCKING:  return immediately even if lock is not available.
*
* @param    timeout_ms      Timeout, in milliseconds. A value of 0 will never timeout.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NONE
*********************************************************************************************************
*/

void  KAL_LockAcquire (KAL_LOCK_HANDLE   lock_handle,
                       KAL_OPT           opt,
                       CPU_INT32U        timeout_ms,
                       RTOS_ERR         *p_err)
{
    PP_UNUSED_PARAM(lock_handle);
    PP_UNUSED_PARAM(opt);
    PP_UNUSED_PARAM(timeout_ms);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}


/**
*********************************************************************************************************
*                                               KAL_LockRelease()
*
* @brief    Release a lock. Without a kernel, nothing must be done.
*
* @param    lock_handle     Handle of the lock to release.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NONE
*********************************************************************************************************
*/

void  KAL_LockRelease (KAL_LOCK_HANDLE   lock_handle,
                       RTOS_ERR         *p_err)
{
    PP_UNUSED_PARAM(lock_handle);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/**
*********************************************************************************************************
*                                               KAL_LockDel()
*
* @brief    Delete a lock. Without a kernel, nothing must be done.
*
* @param    lock_handle     Handle of the lock to delete.
*********************************************************************************************************
*/

void  KAL_LockDel (KAL_LOCK_HANDLE  lock_handle)
{
    PP_UNUSED_PARAM(lock_handle);
}


/*
*********************************************************************************************************
*                                          SEM API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_SemCreate()
*
* @brief    Create a semaphore, with a count of 0.
*
* @param    p_name  Pointer to name of the semaphore.
*
* @param    p_cfg   Pointer to KAL semaphore configuration structure.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
*                   this function:
*                       - RTOS_ERR_NONE
*                       - RTOS_ERR_NOT_SUPPORTED
*
* @return   Created semaphore's handle.
*
* @note     (1) This function will initialize KAL if KAL has not yet been initialized.
*********************************************************************************************************
*/

KAL_SEM_HANDLE  KAL_SemCreate (const  CPU_CHAR         *p_name,
                                      KAL_SEM_EXT_CFG  *p_cfg,
                                      RTOS_ERR         *p_err)
{
    KAL_SEM_HANDLE   handle = KAL_SemHandleNull;
    KAL_SEM         *p_sem;
    RTOS_ERR         err_lib;


    PP_UNUSED_PARAM(p_name);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);
                                                                /* Make sure no unsupported cfg recv.                   */
    RTOS_ASSERT_DBG_ERR_SET((p_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_SUPPORTED, handle);

    if (KAL_DataPtr == DEF_NULL) {
        KAL_Init(p_err);                                        /* KAl initialization (see Note #1).                    */
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return(handle);
        }
    }

    p_sem = (KAL_SEM *)Mem_DynPoolBlkGet(&KAL_DataPtr->SemPool,
                                         &err_lib);
    if (RTOS_ERR_CODE_GET(err_lib) != RTOS_ERR_NONE) {
        return (handle);
    }

    p_sem->Cnt       =  0u;                                     /* Created sem is unvavail at creation.                 */
    handle.SemObjPtr = (void *)p_sem;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    return (handle);
}


/**
*********************************************************************************************************
*                                               KAL_SemPend()
*
* @brief    Pend on a semaphore.
*
* @param    sem_handle  Handle of the semaphore to pend on.
*
* @param    opt         Options available:
*                           - KAL_OPT_PEND_NONE:          block until timeout expires or semaphore is available.
*                           - KAL_OPT_PEND_BLOCKING:      block until timeout expires or semaphore is available.
*                           - KAL_OPT_PEND_NON_BLOCKING:  return immediately even if semaphore is not available.
*
* @param    timeout_ms  Timeout, in milliseconds. A value of 0 will never timeout.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NONE
*                           - RTOS_ERR_TIMEOUT
*                           - RTOS_ERR_WOULD_BLOCK
*                           - RTOS_ERR_OS
*                           - RTOS_ERR_NOT_AVAIL
*********************************************************************************************************
*/

void  KAL_SemPend (KAL_SEM_HANDLE   sem_handle,
                   KAL_OPT          opt,
                   CPU_INT32U       timeout_ms,
                   RTOS_ERR        *p_err)
{
    KAL_SEM  *p_sem;
    CPU_SR_ALLOC();


                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);                 /* Validate err ptr.                                    */

    RTOS_ASSERT_DBG_ERR_SET((KAL_SEM_HANDLE_IS_NULL(sem_handle) != DEF_YES), *p_err, RTOS_ERR_NULL_PTR, ;);

    RTOS_ASSERT_DBG_ERR_SET(((opt == KAL_OPT_PEND_NON_BLOCKING) || (opt == KAL_OPT_PEND_BLOCKING)), *p_err, RTOS_ERR_INVALID_ARG, ;);

    p_sem = (KAL_SEM *)sem_handle.SemObjPtr;

    if (DEF_BIT_IS_CLR(opt, KAL_OPT_PEND_NON_BLOCKING) == DEF_YES) {
                                                                /* Blocking call.                                       */
        if (timeout_ms == 0u) {
            while (p_sem->Cnt == 0u) {                          /* Loop until sem is posted.                            */
                CPU_CRITICAL_ENTER();
                if (p_sem->Cnt != 0u) {
                   p_sem->Cnt--;                                /* Dec sem cnt.                                         */
                }
                CPU_CRITICAL_EXIT();
            }
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        } else {
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
            {
                CPU_TS_TMR       ts;
                CPU_TS_TMR       ts_prev;
                CPU_INT32U       ts_delta;
                CPU_INT32U       timeout_ts;
                CPU_TS_TMR_FREQ  tmr_freq;
                RTOS_ERR         err_cpu;


                tmr_freq = CPU_TS_TmrFreqGet(&err_cpu);
                if (RTOS_ERR_CODE_GET(err_cpu) != RTOS_ERR_NONE) {
                    RTOS_ERR_SET(*p_err, RTOS_ERR_OS);
                    return;
                }

                timeout_ts = timeout_ms * (tmr_freq / 1000u);   /* Calc timeout ts val, depending on tmr freq.          */
                ts_delta   = 0u;
                ts_prev    = CPU_TS_TmrRd();
                while (ts_delta < timeout_ts) {                 /* Loop until sem is avail or timeout expires.          */
                    CPU_CRITICAL_ENTER();
                    if (p_sem->Cnt != 0u) {
                        p_sem->Cnt--;                           /* Dec sem cnt.                                         */
                        CPU_CRITICAL_EXIT();
                        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
                        return;
                    }
                    CPU_CRITICAL_EXIT();

                    ts = CPU_TS_TmrRd();
                    if (ts >= ts_prev) {                        /* Update ts_delta val.                                 */
                        ts_delta += ts - ts_prev;
                    } else {                                    /* Wrap-around.                                          */
                        ts_delta += DEF_GET_U_MAX_VAL(CPU_TS_TMR) - ts_prev + ts;
                    }
                    ts_prev = ts;
                }
                RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
            }
#else
            RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
#endif
        }
    } else {                                                    /* Non-blocking call.                                   */
        CPU_CRITICAL_ENTER();
        if (p_sem->Cnt != 0u) {
            p_sem->Cnt--;
            RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);
        }
        CPU_CRITICAL_EXIT();
    }
}


/**
*********************************************************************************************************
*                                               KAL_SemPost()
*
* @brief    Post a semaphore.
*
* @param    sem_handle  Handle of the semaphore to post.
*
* @param    opt         Options available:
*                       KAL_OPT_POST_NONE:     wake only the highest priority task pending on semaphore.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NONE
*                           - RTOS_ERR_WOULD_OVF
*********************************************************************************************************
*/

void  KAL_SemPost (KAL_SEM_HANDLE   sem_handle,
                   KAL_OPT          opt,
                   RTOS_ERR        *p_err)
{
    KAL_SEM  *p_sem;
    CPU_SR_ALLOC();


                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);                 /* Validate err ptr.                                    */

    RTOS_ASSERT_DBG_ERR_SET((KAL_SEM_HANDLE_IS_NULL(sem_handle) != DEF_YES), *p_err, RTOS_ERR_NULL_PTR, ;);

    RTOS_ASSERT_DBG_ERR_SET((opt == KAL_OPT_POST_NONE), *p_err, RTOS_ERR_INVALID_ARG, ;);

    p_sem = (KAL_SEM *)sem_handle.SemObjPtr;

    CPU_CRITICAL_ENTER();
    p_sem->Cnt++;
    if (p_sem->Cnt != 0u) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    }
    CPU_CRITICAL_EXIT();
}


/**
*********************************************************************************************************
*                                           KAL_SemPendAbort()
*
* @brief    Abort given semaphore and resume all the tasks pending on it. Not supported, without a kernel.
*
* @param    sem_handle  Handle of the semaphore to abort.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NOT_SUPPORTED
*********************************************************************************************************
*/

void  KAL_SemPendAbort (KAL_SEM_HANDLE   sem_handle,
                        RTOS_ERR        *p_err)
{
    PP_UNUSED_PARAM(sem_handle);

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
}


/**
*********************************************************************************************************
*                                               KAL_SemSet()
*
* @brief    Set value of semaphore.
*
* @param    sem_handle  Handle of the semaphore to set.
*
* @param    cnt         Count value to set semaphore.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NOT_SUPPORTED
*********************************************************************************************************
*/

void  KAL_SemSet (KAL_SEM_HANDLE   sem_handle,
                  CPU_INT08U       cnt,
                  RTOS_ERR        *p_err)
{
    PP_UNUSED_PARAM(sem_handle);
    PP_UNUSED_PARAM(cnt);

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
}


/**
*********************************************************************************************************
*                                               KAL_SemDel()
*
* @brief    Delete a semaphore.
*
* @param    sem_handle  Handle of the semaphore to delete.
*********************************************************************************************************
*/

void  KAL_SemDel (KAL_SEM_HANDLE  sem_handle)
{
    KAL_SEM   *p_sem;
    RTOS_ERR   err;


                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG((KAL_SEM_HANDLE_IS_NULL(sem_handle) == DEF_NO), RTOS_ERR_INVALID_HANDLE, ;);

    p_sem = (KAL_SEM *)sem_handle.SemObjPtr;

    Mem_DynPoolBlkFree(       &KAL_DataPtr->SemPool,
                       (void *)p_sem,
                              &err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, ;);
}


/*
*********************************************************************************************************
*                                          TMR API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_TmrCreate()
*
* @brief    Create a single-shot timer.
*
* @param    p_name          Pointer to name of the timer.
*
* @param    p_callback      Pointer to the callback function that will be called on completion of timer.
*
* @param    p_callback_arg  Argument passed to callback function.
*
* @param    interval_ms     If timer is 'one-shot', delay  used by the timer, in milliseconds.
*                           If timer is 'periodic', period used by the timer, in milliseconds.
*
* @param    p_cfg           Pointer to KAL timer configuration structure.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NOT_SUPPORTED
*
* @return   Created timer handle.
*********************************************************************************************************
*/

KAL_TMR_HANDLE  KAL_TmrCreate (const  CPU_CHAR   *p_name,
                               void             (*p_callback)(void  *p_arg),
                               void              *p_callback_arg,
                               CPU_INT32U         interval_ms,
                               KAL_TMR_EXT_CFG   *p_cfg,
                               RTOS_ERR          *p_err)
{
    KAL_TMR_HANDLE  handle = KAL_TmrHandleNull;


    PP_UNUSED_PARAM(p_name);
    PP_UNUSED_PARAM(p_callback);
    PP_UNUSED_PARAM(p_callback_arg);
    PP_UNUSED_PARAM(interval_ms);
    PP_UNUSED_PARAM(p_cfg);

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (handle);
}


/**
*********************************************************************************************************
*                                               KAL_TmrStart()
*
* @brief    Start timer.
*
* @param    tmr_handle  Handle of timer to start.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NOT_SUPPORTED
*********************************************************************************************************
*/

void  KAL_TmrStart (KAL_TMR_HANDLE   tmr_handle,
                    RTOS_ERR        *p_err)
{
    PP_UNUSED_PARAM(tmr_handle);

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
}


/*
*********************************************************************************************************
*                                           Q API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_QCreate()
*
* @brief    Create an empty queue.
*
* @param    p_name          Pointer to name of the queue.
*
* @param    max_msg_qty     Maximum number of message contained in the queue.
*
* @param    p_cfg           Pointer to KAL queue configuration structure.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NOT_SUPPORTED
*
* @return   Created queue handle.
*********************************************************************************************************
*/

KAL_Q_HANDLE  KAL_QCreate (const  CPU_CHAR       *p_name,
                                  KAL_MSG_QTY     max_msg_qty,
                                  KAL_Q_EXT_CFG  *p_cfg,
                                  RTOS_ERR       *p_err)
{
    KAL_Q_HANDLE  handle = KAL_QHandleNull;


                                                                /* Qs are not avail.                                    */
    PP_UNUSED_PARAM(p_name);
    PP_UNUSED_PARAM(max_msg_qty);
    PP_UNUSED_PARAM(p_cfg);

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (handle);
}


/**
*********************************************************************************************************
*                                               KAL_QPend()
*
* @brief    Pend/get first message of queue.
*
* @param    q_handle    Handle of the queue to pend on.
*
* @param    opt         Options available:
*                           - KAL_OPT_PEND_NONE:          block until timeout expires or message is available.
*                           - KAL_OPT_PEND_BLOCKING:      block until timeout expires or message is available.
*                           - KAL_OPT_PEND_NON_BLOCKING:  return immediately with or without message.
*
* @param    timeout_ms  Timeout, in milliseconds. A value of 0 will never timeout.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NOT_SUPPORTED
*
* @return   Pointer to message obtained, if any, if no error.
*           Null pointer, otherwise.
*********************************************************************************************************
*/

void  *KAL_QPend (KAL_Q_HANDLE   q_handle,
                  KAL_OPT        opt,
                  CPU_INT32U     timeout_ms,
                  RTOS_ERR      *p_err)
{
    PP_UNUSED_PARAM(q_handle);
    PP_UNUSED_PARAM(opt);
    PP_UNUSED_PARAM(timeout_ms);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (DEF_NULL);
}


/**
*********************************************************************************************************
*                                               KAL_QPost()
*
* @brief    Post message on queue.
*
* @param    q_handle    Handle of the queue on which to post message.
*
* @param    p_msg       Pointer to message to post.
*
* @param    opt         Options available:
*                       KAL_OPT_POST_NONE:     wake only the highest priority task pending on queue.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NOT_SUPPORTED
*********************************************************************************************************
*/

void  KAL_QPost (KAL_Q_HANDLE   q_handle,
                 void          *p_msg,
                 KAL_OPT        opt,
                 RTOS_ERR      *p_err)
{
    PP_UNUSED_PARAM(q_handle);
    PP_UNUSED_PARAM(p_msg);
    PP_UNUSED_PARAM(opt);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
}


/*
*********************************************************************************************************
*                                          MON API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_MonCreate()
*
* @brief    Create a monitor.
*
* @param    p_name      Pointer to name of the monitor.
*
* @param    p_mon_obj   Pointer to monitor object, if any. If DEF_NULL, monitor object will be
*                       allocated from monitor pool by KAL.
*
* @param    p_mon_data  Pointer to data that will be passed to callback functions.
*
* @param    p_cfg       Pointer to KAL monitor configuration structure. Reserved for future use, must
*                       be DEF_NULL.
*
* @param    p_err       Pointer to the variable that will receive one of the following error code(s)
*                       from this function:
*                           - RTOS_ERR_NOT_SUPPORTED
*
* @return   Created monitor handle.
*********************************************************************************************************
*/

KAL_MON_HANDLE  KAL_MonCreate (const CPU_CHAR         *p_name,
                                     void             *p_mon_obj,
                                     void             *p_mon_data,
                                     KAL_MON_EXT_CFG  *p_cfg,
                                     RTOS_ERR         *p_err)
{
    KAL_MON_HANDLE  handle = KAL_MonHandleNull;


    PP_UNUSED_PARAM(p_name);
    PP_UNUSED_PARAM(p_mon_obj);
    PP_UNUSED_PARAM(p_mon_data);
    PP_UNUSED_PARAM(p_cfg);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (handle);
}


/**
*********************************************************************************************************
*                                               KAL_MonOp()
*
* @brief    Execute an operation on a monitor.
*
* @param    mon_handle      Handle of the monitor on which operation is executed.
*
* @param    p_op_data       Pointer to arg that will be passed to callback functions.
*
* @param    on_enter_fnct   Function that will be called on entering the monitor.
*
* @param    on_eval_fnct    Function that will be called when evaluating if this task can run.
*
* @param    opt             Options for call.
*                               - KAL_OPT_MON_NONE            No option.
*                               - KAL_OPT_MON_NO_SCHED        Scheduler will not be called.
*
* @param    timeout_ms      Timeout, in milliseconds. A value of KAL_TIMEOUT_INFINITE will never timeout.
*
* @param    p_err           Pointer to the variable that will receive one of the following error
*                           code(s) from this function:
*                               - RTOS_ERR_NOT_SUPPORTED
*********************************************************************************************************
*/


void  KAL_MonOp (KAL_MON_HANDLE    mon_handle,
                 void             *p_arg,
                 KAL_MON_RES     (*on_enter_fnct)(void  *p_mon_data, void  *p_op_data),
                 KAL_MON_RES     (*on_eval_fnct) (void  *p_mon_data, void  *p_eval_op_data, void  *p_scan_op_data),
                 KAL_OPT           opt,
                 KAL_TICK          timeout_ms,
                 RTOS_ERR         *p_err)
{
    PP_UNUSED_PARAM(mon_handle);
    PP_UNUSED_PARAM(p_arg);
    PP_UNUSED_PARAM(on_enter_fnct);
    PP_UNUSED_PARAM(on_eval_fnct);
    PP_UNUSED_PARAM(opt);
    PP_UNUSED_PARAM(timeout_ms);

                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
}


/**
*********************************************************************************************************
*                                               KAL_MonDel()
*
* @brief    Delete a monitor.
*
* @param    mon_handle  Handle of the monitor to delete.
*********************************************************************************************************
*/

void  KAL_MonDel (KAL_MON_HANDLE  mon_handle)
{
    PP_UNUSED_PARAM(mon_handle);
}


/*
*********************************************************************************************************
*                                          DLY API FUNCTIONS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               KAL_Dly()
*
* @brief    Delay current task (in milliseconds).
*
* @param    dly_ms          Delay value, in milliseconds.
*
* @note     (1) This function is only available if time stamp is enabled. Otherwise, without an OS,
*               there is no other available way to measure time.
*********************************************************************************************************
*/

void  KAL_Dly (CPU_INT32U  dly_ms)
{
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
    CPU_TS_TMR       ts;
    CPU_TS_TMR       ts_prev;
    CPU_INT32U       ts_delta;
    CPU_INT32U       dly_ts;
    CPU_TS_TMR_FREQ  tmr_freq;
    RTOS_ERR         err_cpu;


    tmr_freq = CPU_TS_TmrFreqGet(&err_cpu);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(err_cpu) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, ;);

    dly_ts   = dly_ms * (tmr_freq / 1000u);                     /* Calc dly ts val, depending on tmr freq.              */
    ts_delta = 0u;
    ts_prev  = CPU_TS_TmrRd();
    while (ts_delta < dly_ts) {                                 /* Loop until dly expires.                              */
        ts = CPU_TS_TmrRd();
        if (ts > ts_prev) {                                     /* Update ts_delta val.                                 */
            ts_delta += ts - ts_prev;
        } else {                                                /* Wrap-around.                                          */
            ts_delta += DEF_GET_U_MAX_VAL(CPU_TS_TMR) - ts_prev + ts;
        }
        ts_prev = ts;
    }

#else
    PP_UNUSED_PARAM(dly_ms);

    RTOS_ASSERT_CRITICAL(0u, RTOS_ERR_ASSERT_CRITICAL_FAIL, ;); /* Fail call. See note #1.                              */
#endif
}


/**
*********************************************************************************************************
*                                               KAL_DlyTick()
*
* @brief    Delay current task (in ticks).
*
* @param    dly_ticks   Delay value, in ticks.
*
* @param    opt         Options available:
*                           - KAL_OPT_DLY_NONE:       apply a 'normal' delay.
*                           - KAL_OPT_DLY:            apply a 'normal' delay.
*                           - KAL_OPT_DLY_PERIODIC:   apply a periodic delay.
*
* @note     (1) This function can only be called if an OS is present, since the notion of 'tick' is
*               not present if no OS is available.
*********************************************************************************************************
*/

void  KAL_DlyTick (KAL_TICK  dly_ticks,
                   KAL_OPT   opt)
{
    PP_UNUSED_PARAM(dly_ticks);
    PP_UNUSED_PARAM(opt);

    RTOS_ASSERT_CRITICAL(0u, RTOS_ERR_ASSERT_CRITICAL_FAIL, ;); /* Fail call. See note #1.                              */
}


/*
*********************************************************************************************************
*                                       TASK REG API FUNCTIONS
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                           KAL_TaskRegCreate()
*
* @brief    Create a task register.
*
* @param    p_cfg   Pointer to KAL task register configuration structure.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
*                   this function:
*                       - RTOS_ERR_NOT_SUPPORTED
*
* @return   Created task register's handle.
*********************************************************************************************************
*/

KAL_TASK_REG_HANDLE  KAL_TaskRegCreate (KAL_TASK_REG_EXT_CFG  *p_cfg,
                                        RTOS_ERR              *p_err)
{
    KAL_TASK_REG_HANDLE  handle = KAL_TaskRegHandleNull;


    PP_UNUSED_PARAM(p_cfg);
                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (handle);

}


/**
*********************************************************************************************************
*                                               KAL_TaskRegGet()
*
* @brief    Get value from a task register.
*
* @param    task_handle         Handle of the task associated to the task register. Current task is used if NULL.
*
* @param    task_reg_handle     Handle of the task register to read.
*
* @param    p_err               Pointer to the variable that will receive one of the following error
*                               code(s) from this function:
*                                   - RTOS_ERR_NOT_SUPPORTED
*
* @return   Value read from the task register, if no error.
*           0, otherwise.
*********************************************************************************************************
*/

KAL_TASK_REG  KAL_TaskRegGet (KAL_TASK_HANDLE       task_handle,
                              KAL_TASK_REG_HANDLE   task_reg_handle,
                              RTOS_ERR             *p_err)
{
    PP_UNUSED_PARAM(task_handle);
    PP_UNUSED_PARAM(task_reg_handle);


                                                                /* ---------------- VALIDATE ARGUMENTS ---------------- */
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (0u);
}


/**
*********************************************************************************************************
*                                               KAL_TaskRegSet()
*
* @brief    Set value of task register.
*
* @param    task_handle         Handle of the task associated to the task register. Current task is used if NULL.
*
* @param    task_reg_handle     Handle of the task register to write to.
*
* @param    val                 Value to write in the task register.
*
* @param    p_err               Pointer to the variable that will receive one of the following error
*                               code(s) from this function:
*                                   - RTOS_ERR_NOT_SUPPORTED
*********************************************************************************************************
*/

void  KAL_TaskRegSet (KAL_TASK_HANDLE       task_handle,
                      KAL_TASK_REG_HANDLE   task_reg_handle,
                      KAL_TASK_REG          val,
                      RTOS_ERR             *p_err)
{
    PP_UNUSED_PARAM(task_handle);
    PP_UNUSED_PARAM(task_reg_handle);
    PP_UNUSED_PARAM(val);

    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
}


/**
*********************************************************************************************************
*                                               KAL_TickGet()
*
* @brief    Get value of OS' tick counter.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
*                   this function:
*                       - RTOS_ERR_NOT_SUPPORTED
*
* @return   OS tick counter's value.
*********************************************************************************************************
*/

KAL_TICK  KAL_TickGet (RTOS_ERR  *p_err)
{
    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);

    return (0u);
}


/**
*********************************************************************************************************
*                                               KAL_TickRateGet()
*
* @brief    Get value of OS' tick rate, in Hz.
*
* @return   OS tick rate's value.
*
* @note     (1) This function can only be called if an OS is present, since the notion of 'tick' is
*               not present if no OS is available.
*********************************************************************************************************
*/

KAL_TICK_RATE_HZ  KAL_TickRateGet (void)
{
    RTOS_ASSERT_CRITICAL(0u, RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);/* Fail call. See note #1.                              */
    return (0u);
}


/**
*********************************************************************************************************
*                                           KAL_CPU_UsageGet()
*
* @brief    Get value of the CPU Usage.
*
* @return   CPU usage's percentage represented as an integer from 0 (0%) to 10000 (100%).
*
* @note     (1) This function can only be called if an OS is present, since the notion of 'tick' is
*               not present if no OS is available.
*********************************************************************************************************
*/

KAL_CPU_USAGE  KAL_CPU_UsageGet (void)
{
    RTOS_ASSERT_CRITICAL(0u, RTOS_ERR_ASSERT_CRITICAL_FAIL, 0u);/* Fail call. See note #1.                              */
    return (0u);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               KAL_Init()
*
* @brief    Initializes the Kernel Abstraction Layer.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
*                   this function:
*                       - RTOS_ERR_NONE
*                       - RTOS_ERR_BLK_ALLOC_CALLBACK
*                       - RTOS_ERR_SEG_OVF
*
* @note     (1) This function will be called by the first function KAL_XXXCreate() called by the
*               application.
*********************************************************************************************************
*/

static  void  KAL_Init (RTOS_ERR  *p_err)
{
    MEM_SEG  *p_mem_seg;
    CPU_SR_ALLOC();


    RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    CPU_CRITICAL_ENTER();
    if (KAL_DataPtr == DEF_NULL) {
        p_mem_seg = Common_MemSegPtrGet();

        KAL_DataPtr = (KAL_DATA *)Mem_SegAlloc("KAL internal data",
                                               p_mem_seg,
                                               sizeof(KAL_DATA),
                                               p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            CPU_CRITICAL_EXIT();
            return;
        }

        Mem_DynPoolCreate("KAL sem pool",
                          &KAL_DataPtr->SemPool,
                           p_mem_seg,
                           sizeof(KAL_SEM),
                           sizeof(CPU_ALIGN),
                           0u,
                           LIB_MEM_BLK_QTY_UNLIMITED,
                           p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            CPU_CRITICAL_EXIT();
            return;
        }
        KAL_DataPtr->TaskCnt = 0;
        LOG_VRB(("KAL_Init successul."));
    }

    CPU_CRITICAL_EXIT();
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                   DEPENDENCIES & AVAIL CHECK(S) END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif  /* defined(RTOS_MODULE_KERNEL_AVAIL)) */
