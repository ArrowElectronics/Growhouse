/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Kernel
*
*                               (c) Copyright 2009; Silicon Laboratories Inc.
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
*                                           EVENT FLAG MANAGEMENT
*
* File : os_flag.c
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

#if (defined(RTOS_MODULE_KERNEL_AVAIL))


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#define  MICRIUM_SOURCE
#include "../include/os.h"
#include "os_priv.h"

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_flag__c = "$Id: $";
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_CFG_FLAG_EN == DEF_ENABLED)
/**
*********************************************************************************************************
*                                               OSFlagCreate()
*
* @brief    This function is called to create an event flag group.
*
* @param    p_grp   Pointer to the event flag group to create. Your application is responsible
*                   for allocating storage for the flag group.
*
* @param    p_name  The name of the event flag group.
*
* @param    flags   Contains the initial value to store in the event flag group (typically 0).
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s)
*                   from this function:
*                       - RTOS_ERR_NONE
*                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
*********************************************************************************************************
*/

void  OSFlagCreate (OS_FLAG_GRP  *p_grp,
                    CPU_CHAR     *p_name,
                    OS_FLAGS      flags,
                    RTOS_ERR     *p_err)
{
    CPU_SR_ALLOC();


    OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ;);

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
        return;
    }
#endif

                                                                /* Not allowed to call from an ISR                      */
    OS_ASSERT_DBG_ERR_SET((OSIntNestingCtr == 0u), *p_err, RTOS_ERR_ISR, ;);

                                                                /* Validate 'p_grp'                                     */
    OS_ASSERT_DBG_ERR_SET((p_grp != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, ;);


    CPU_CRITICAL_ENTER();
#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
    p_grp->Type    = OS_OBJ_TYPE_FLAG;                          /* Set to event flag group type                         */
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_grp->NamePtr = p_name;
#else
    (void)&p_name;
#endif
    p_grp->Flags   = flags;                                     /* Set to desired initial value                         */
#if (OS_CFG_TS_EN == DEF_ENABLED)
    p_grp->TS      = 0u;
#endif
    OS_PendListInit(&p_grp->PendList);

#if (OS_CFG_DBG_EN == DEF_ENABLED)
    OS_FlagDbgListAdd(p_grp);
    OSFlagQty++;
#endif

    OS_TRACE_FLAG_CREATE(p_grp, p_name);

    CPU_CRITICAL_EXIT();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}


/**
*********************************************************************************************************
*                                               OSFlagDel()
*
* @brief    This function deletes an event flag group and readies all tasks pending on the event flag
*           group.
*
* @param    p_grp   Pointer to the event flag group.
*
* @param    opt     Determines delete options as follows:
*                       - OS_OPT_DEL_NO_PEND  Deletes the event flag group ONLY if no task is
*                                             pending.
*                       - OS_OPT_DEL_ALWAYS   Deletes the event flag group even if tasks are
*                                             waiting. In this case, all the pending tasks
*                                             will be made ready.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s)
*                   from this function:
*                       - RTOS_ERR_NONE
*                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
*                       - RTOS_ERR_OS_TASK_WAITING
*
* @return   == 0    If no tasks were waiting on the event flag group, or upon error.
*           >  0    If one or more tasks waiting on the event flag group are now ready and
*                   informed.
*          
* @note     (1) Use this function with care. Tasks that would normally expect the presence of
*               the event flag group MUST check the return code of OSFlagPost() and OSFlagPend().
*********************************************************************************************************
*/

OS_OBJ_QTY  OSFlagDel (OS_FLAG_GRP  *p_grp,
                       OS_OPT        opt,
                       RTOS_ERR     *p_err)
{
    OS_OBJ_QTY     nbr_tasks;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    CPU_SR_ALLOC();


    OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

    OS_TRACE_FLAG_DEL_ENTER(p_grp, opt);

#ifdef OS_SAFETY_CRITICAL_IEC61508
    if (OSSafetyCriticalStartFlag == DEF_TRUE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_OS_ILLEGAL_RUN_TIME);
        return (0u);
    }
#endif

                                                                /* Not allowed to call from an ISR                      */
    OS_ASSERT_DBG_ERR_SET((OSIntNestingCtr == 0u), *p_err, RTOS_ERR_ISR, 0u);

                                                                /* Make sure kernel is running.                         */
    OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_RUNNING), *p_err, RTOS_ERR_NOT_READY, 0u);

                                                                /* Validate 'p_grp'                                     */
    OS_ASSERT_DBG_ERR_SET((p_grp != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

                                                                /* Validate event group object                          */
    OS_ASSERT_DBG_ERR_SET((p_grp->Type == OS_OBJ_TYPE_FLAG), *p_err, RTOS_ERR_INVALID_TYPE, 0u);


    CPU_CRITICAL_ENTER();
    p_pend_list = &p_grp->PendList;
    nbr_tasks   = 0u;
    switch (opt) {
        case OS_OPT_DEL_NO_PEND:                                /* Delete group if no task waiting                      */
             if (p_pend_list->HeadPtr == DEF_NULL) {
#if (OS_CFG_DBG_EN == DEF_ENABLED)
                 OS_FlagDbgListRemove(p_grp);
                 OSFlagQty--;
#endif
                 OS_TRACE_FLAG_DEL(p_grp);

                 OS_FlagClr(p_grp);

                 CPU_CRITICAL_EXIT();

                 RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
             } else {
                 CPU_CRITICAL_EXIT();
                 RTOS_ERR_SET(*p_err, RTOS_ERR_OS_TASK_WAITING);
             }
             break;

        case OS_OPT_DEL_ALWAYS:                                 /* Always delete the event flag group                   */
#if (OS_CFG_TS_EN == DEF_ENABLED)
             ts = OS_TS_GET();                                  /* Get local time stamp so all tasks get the same time  */
#else
             ts = 0u;
#endif
             while (p_pend_list->HeadPtr != DEF_NULL) {         /* Remove all tasks from the pend list                  */
                 p_tcb = p_pend_list->HeadPtr;
                 OS_PendAbort(p_tcb,
                              ts,
                              OS_STATUS_PEND_DEL);
                 nbr_tasks++;
             }
#if (OS_CFG_DBG_EN == DEF_ENABLED)
             OS_FlagDbgListRemove(p_grp);
             OSFlagQty--;
#endif
             OS_TRACE_FLAG_DEL(p_grp);

             OS_FlagClr(p_grp);
             CPU_CRITICAL_EXIT();

             OSSched();                                         /* Find highest priority task ready to run              */
             RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
             break;

        default:
             CPU_CRITICAL_EXIT();
             OS_ASSERT_DBG_FAIL_EXEC(*p_err, RTOS_ERR_INVALID_ARG, 0u);
    }

    OS_TRACE_FLAG_DEL_EXIT(RTOS_ERR_CODE_GET(*p_err));

    return (nbr_tasks);
}


/**
*********************************************************************************************************
*                                               OSFlagPend()
*
* @brief          This function is called to wait for a combination of bits to be set in an event flag group.
*               Your application can wait for either ANY bit to be set or ALL bits to be set.
*
* @param          p_grp       Pointer to the event flag group.
*
* @param          flags       Bit pattern that indicates which bit(s) (i.e. flags) to wait for.
*                             The bits you want are specified by setting the corresponding bits in 'flags'
*                             (e.g. if your application waits for bits 0 and 1, then the 'flags' would
*                             contain 0x03).
*
* @param          timeout     Optional timeout (in clock ticks                            ) that your task will wait for the
*                             desired bit combination. If you specify 0, the task will wait forever at the
*                             specified event flag group, or until a message arrives.
*
* @param          opt         Specifies whether you want ALL bits to be set or ANY of the bits to be set.
*                             You can specify the 'ONE' of the following arguments:
*                                 - OS_OPT_PEND_FLAG_CLR_ALL    Wait for ALL bits in 'flags' to be clear. (0)
*                                 - OS_OPT_PEND_FLAG_CLR_ANY    Wait for ANY bit  in 'flags' to be clear. (0)
*                                 - OS_OPT_PEND_FLAG_SET_ALL    Wait for ALL bits in 'flags' to be set.   (1)
*                                 - OS_OPT_PEND_FLAG_SET_ANY    Wait for ANY bit  in 'flags' to be set.   (1)
*                             @n
*                             You can 'ADD' OS_OPT_PEND_FLAG_CONSUME if you want the event flag to be
*                             'consumed' by the call. For example, to wait for any flag in a group AND
*                             clear the flags that are present, set 'wait_opt' to:
*                             @n
*                             OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME
*                             @n
*                             You can also 'ADD' the type of pend with 'ONE' of the two options:
*                                 - OS_OPT_PEND_BLOCKING        Task will     block if flags are not available.
*                                 - OS_OPT_PEND_NON_BLOCKING    Task will NOT block if flags are not available.
*
* @param          p_ts        Pointer to a variable that receives the timestamp of when the event flag
*                             group was posted, aborted, or deleted. If you pass a NULL pointer
*                             (i.e. (CPU_TS *)0) then you will not get the timestamp. In other words,
*                             passing a NULL pointer is valid and indicates that you don't need the timestamp.
*
* @param          p_err       Pointer to the variable that will receive one of the following error code(s)
*                             from this function:
*                                 - RTOS_ERR_NONE
*                                 - RTOS_ERR_OS_OBJ_DEL
*                                 - RTOS_ERR_WOULD_BLOCK
*                                 - RTOS_ERR_OS_SCHED_LOCKED
*                                 - RTOS_ERR_ABORT
*                                 - RTOS_ERR_TIMEOUT
*
* @return        The flags in the event flag group that made the task ready or, 0 if a timeout or an error
*               occurred.
*********************************************************************************************************
*/

OS_FLAGS  OSFlagPend (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_TICK       timeout,
                      OS_OPT        opt,
                      CPU_TS       *p_ts,
                      RTOS_ERR     *p_err)
{
    CPU_BOOLEAN  consume;
    OS_FLAGS     flags_rdy;
    OS_OPT       mode;
    CPU_SR_ALLOC();


    OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

    OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts);

                                                                /* Not allowed to call from an ISR                      */
    OS_ASSERT_DBG_ERR_SET((OSIntNestingCtr == 0u), *p_err, RTOS_ERR_ISR, 0u);

                                                                /* Make sure kernel is running.                         */
    OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_RUNNING), *p_err, RTOS_ERR_NOT_READY, 0u);

                                                                /* Validate 'p_grp'                                     */
    OS_ASSERT_DBG_ERR_SET((p_grp != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

                                                                /* Validate 'opt'                                       */
    OS_ASSERT_DBG_ERR_SET(((opt ==  OS_OPT_PEND_FLAG_CLR_ALL)                                                        ||
                           (opt ==  OS_OPT_PEND_FLAG_CLR_ANY)                                                        ||
                           (opt ==  OS_OPT_PEND_FLAG_SET_ALL)                                                        ||
                           (opt ==  OS_OPT_PEND_FLAG_SET_ANY)                                                        ||
                           (opt == (OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME))                            ||
                           (opt == (OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME))                            ||
                           (opt == (OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME))                            ||
                           (opt == (OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME))                            ||
                           (opt == (OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_NON_BLOCKING))                            ||
                           (opt == (OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_NON_BLOCKING))                            ||
                           (opt == (OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_NON_BLOCKING))                            ||
                           (opt == (OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_NON_BLOCKING))                            ||
                           (opt == (OS_OPT_PEND_FLAG_CLR_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING)) ||
                           (opt == (OS_OPT_PEND_FLAG_CLR_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING)) ||
                           (opt == (OS_OPT_PEND_FLAG_SET_ALL | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING)) ||
                           (opt == (OS_OPT_PEND_FLAG_SET_ANY | OS_OPT_PEND_FLAG_CONSUME | OS_OPT_PEND_NON_BLOCKING))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

                                                                /* Validate event group object                          */
    OS_ASSERT_DBG_ERR_SET((p_grp->Type == OS_OBJ_TYPE_FLAG), *p_err, RTOS_ERR_INVALID_TYPE, 0u);


    if ((opt & OS_OPT_PEND_FLAG_CONSUME) != 0u) {               /* See if we need to consume the flags                  */
        consume = DEF_TRUE;
    } else {
        consume = DEF_FALSE;
    }

    if (p_ts != DEF_NULL) {
       *p_ts = 0u;                                              /* Initialize the returned timestamp                    */
    }

    mode = opt & OS_OPT_PEND_FLAG_MASK;
    CPU_CRITICAL_ENTER();
    switch (mode) {
        case OS_OPT_PEND_FLAG_SET_ALL:                          /* See if all required flags are set                    */
             flags_rdy = (p_grp->Flags & flags);                /* Extract only the bits we want                        */
             if (flags_rdy == flags) {                          /* Must match ALL the bits that we want                 */
                 if (consume == DEF_TRUE) {                     /* See if we need to consume the flags                  */
                     p_grp->Flags &= ~flags_rdy;                /* Clear ONLY the flags that we wanted                  */
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;             /* Save flags that were ready                           */
#if (OS_CFG_TS_EN == DEF_ENABLED)
                 if (p_ts != DEF_NULL) {
                    *p_ts = p_grp->TS;
                 }
#endif
                 CPU_CRITICAL_EXIT();                           /* Yes, condition met, return to caller                 */
                 RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
                 OS_TRACE_FLAG_PEND(p_grp);
                 OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                 return (flags_rdy);
             } else {                                           /* Block task until events occur or timeout             */
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {
                     CPU_CRITICAL_EXIT();
                     RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);/* Specified non-blocking so task would block           */
                     OS_TRACE_FLAG_PEND_FAILED(p_grp);
                     OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                     return ((OS_FLAGS)0);
                 } else {                                       /* Specified blocking so check is scheduler is locked   */
                     if (OSSchedLockNestingCtr > 0u) {          /* See if called with scheduler locked ...        */
                         CPU_CRITICAL_EXIT();                   /* ... can't PEND when locked                           */
                         RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
                         OS_TRACE_FLAG_PEND_FAILED(p_grp);
                         OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                         return (0u);
                     }
                 }
                                                                /* Lock the scheduler/re-enable interrupts              */
                 OS_FlagBlock(p_grp,
                              flags,
                              opt,
                              timeout);
                 CPU_CRITICAL_EXIT();
             }
             break;

        case OS_OPT_PEND_FLAG_SET_ANY:
             flags_rdy = (p_grp->Flags & flags);                /* Extract only the bits we want                        */
             if (flags_rdy != 0u) {                             /* See if any flag set                                  */
                 if (consume == DEF_TRUE) {                     /* See if we need to consume the flags                  */
                     p_grp->Flags &= ~flags_rdy;                /* Clear ONLY the flags that we got                     */
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;             /* Save flags that were ready                           */
#if (OS_CFG_TS_EN == DEF_ENABLED)
                 if (p_ts != DEF_NULL) {
                    *p_ts  = p_grp->TS;
                 }
#endif
                 CPU_CRITICAL_EXIT();                           /* Yes, condition met, return to caller                 */
                 RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
                 OS_TRACE_FLAG_PEND(p_grp);
                 OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                 return (flags_rdy);
             } else {                                           /* Block task until events occur or timeout             */
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {
                     CPU_CRITICAL_EXIT();
                     RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);/* Specified non-blocking so task would block           */
                     OS_TRACE_FLAG_PEND_FAILED(p_grp);
                     OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                     return ((OS_FLAGS)0);
                 } else {                                       /* Specified blocking so check is scheduler is locked   */
                     if (OSSchedLockNestingCtr > 0u) {          /* See if called with scheduler locked ...        */
                         CPU_CRITICAL_EXIT();                   /* ... can't PEND when locked                           */
                         RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
                         OS_TRACE_FLAG_PEND_FAILED(p_grp);
                         OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                         return ((OS_FLAGS)0);
                     }
                 }

                 OS_FlagBlock(p_grp,
                              flags,
                              opt,
                              timeout);
                 CPU_CRITICAL_EXIT();
             }
             break;

#if (OS_CFG_FLAG_MODE_CLR_EN == DEF_ENABLED)
        case OS_OPT_PEND_FLAG_CLR_ALL:                          /* See if all required flags are cleared                */
             flags_rdy = (OS_FLAGS)(~p_grp->Flags & flags);     /* Extract only the bits we want                        */
             if (flags_rdy == flags) {                          /* Must match ALL the bits that we want                 */
                 if (consume == DEF_TRUE) {                     /* See if we need to consume the flags                  */
                     p_grp->Flags |= flags_rdy;                 /* Set ONLY the flags that we wanted                    */
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;             /* Save flags that were ready                           */
#if (OS_CFG_TS_EN == DEF_ENABLED)
                 if (p_ts != DEF_NULL) {
                    *p_ts  = p_grp->TS;
                 }
#endif
                 CPU_CRITICAL_EXIT();                           /* Yes, condition met, return to caller                 */
                 RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
                 OS_TRACE_FLAG_PEND(p_grp);
                 OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                 return (flags_rdy);
             } else {                                           /* Block task until events occur or timeout             */
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {
                     CPU_CRITICAL_EXIT();
                     RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);/* Specified non-blocking so task would block           */
                     OS_TRACE_FLAG_PEND_FAILED(p_grp);
                     OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                     return ((OS_FLAGS)0);
                 } else {                                       /* Specified blocking so check is scheduler is locked   */
                     if (OSSchedLockNestingCtr > 0u) {          /* See if called with scheduler locked ...        */
                         CPU_CRITICAL_EXIT();                   /* ... can't PEND when locked                           */
                         RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
                         OS_TRACE_FLAG_PEND_FAILED(p_grp);
                         OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                         return (0);
                     }
                 }

                 OS_FlagBlock(p_grp,
                              flags,
                              opt,
                              timeout);
                 CPU_CRITICAL_EXIT();
             }
             break;

        case OS_OPT_PEND_FLAG_CLR_ANY:
             flags_rdy = (~p_grp->Flags & flags);               /* Extract only the bits we want                        */
             if (flags_rdy != 0u) {                             /* See if any flag cleared                              */
                 if (consume == DEF_TRUE) {                     /* See if we need to consume the flags                  */
                     p_grp->Flags |= flags_rdy;                 /* Set ONLY the flags that we got                       */
                 }
                 OSTCBCurPtr->FlagsRdy = flags_rdy;             /* Save flags that were ready                           */
#if (OS_CFG_TS_EN == DEF_ENABLED)
                 if (p_ts != DEF_NULL) {
                    *p_ts  = p_grp->TS;
                 }
#endif
                 CPU_CRITICAL_EXIT();                           /* Yes, condition met, return to caller                 */
                 RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
                 OS_TRACE_FLAG_PEND(p_grp);
                 OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                 return (flags_rdy);
             } else {                                           /* Block task until events occur or timeout             */
                 if ((opt & OS_OPT_PEND_NON_BLOCKING) != 0u) {
                     CPU_CRITICAL_EXIT();
                     RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_BLOCK);/* Specified non-blocking so task would block           */
                     OS_TRACE_FLAG_PEND_FAILED(p_grp);
                     OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                     return ((OS_FLAGS)0);
                 } else {                                       /* Specified blocking so check is scheduler is locked   */
                     if (OSSchedLockNestingCtr > 0u) {          /* See if called with scheduler locked ...        */
                         CPU_CRITICAL_EXIT();                   /* ... can't PEND when locked                           */
                         RTOS_ERR_SET(*p_err, RTOS_ERR_OS_SCHED_LOCKED);
                         OS_TRACE_FLAG_PEND_FAILED(p_grp);
                         OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
                         return (0u);
                     }
                 }

                 OS_FlagBlock(p_grp,
                              flags,
                              opt,
                              timeout);
                 CPU_CRITICAL_EXIT();
             }
             break;
#endif

        default:
             CPU_CRITICAL_EXIT();
             OS_TRACE_FLAG_PEND_FAILED(p_grp);
             OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_INVALID_ARG);
             RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_ARG, 0u);
    }

    OS_TRACE_FLAG_PEND_BLOCK(p_grp);

    OSSched();                                                  /* Find next HPT ready to run                           */

    CPU_CRITICAL_ENTER();
    switch (OSTCBCurPtr->PendStatus) {
        case OS_STATUS_PEND_OK:                                 /* We got the event flags                               */
#if (OS_CFG_TS_EN == DEF_ENABLED)
             if (p_ts != DEF_NULL) {
                *p_ts = OSTCBCurPtr->TS;
             }
#endif
             OS_TRACE_FLAG_PEND(p_grp);
             RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
             break;

        case OS_STATUS_PEND_ABORT:                              /* Indicate that we aborted                             */
#if (OS_CFG_TS_EN == DEF_ENABLED)
             if (p_ts != DEF_NULL) {
                *p_ts = OSTCBCurPtr->TS;
             }
#endif
             CPU_CRITICAL_EXIT();
             OS_TRACE_FLAG_PEND_FAILED(p_grp);
             RTOS_ERR_SET(*p_err, RTOS_ERR_ABORT);
             break;

        case OS_STATUS_PEND_TIMEOUT:                            /* Indicate that we didn't get semaphore within timeout */
             if (p_ts != DEF_NULL) {
                *p_ts = 0u;
             }
             CPU_CRITICAL_EXIT();
             OS_TRACE_FLAG_PEND_FAILED(p_grp);
             RTOS_ERR_SET(*p_err, RTOS_ERR_TIMEOUT);
             break;

        case OS_STATUS_PEND_DEL:                                /* Indicate that object pended on has been deleted      */
#if (OS_CFG_TS_EN == DEF_ENABLED)
             if (p_ts != DEF_NULL) {
                *p_ts = OSTCBCurPtr->TS;
             }
#endif
             CPU_CRITICAL_EXIT();
             OS_TRACE_FLAG_PEND_FAILED(p_grp);
             RTOS_ERR_SET(*p_err, RTOS_ERR_OS_OBJ_DEL);
             break;

        default:
             CPU_CRITICAL_EXIT();
             OS_TRACE_FLAG_PEND_FAILED(p_grp);
             RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, 0u);
    }
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        OS_TRACE_FLAG_PEND_FAILED(p_grp);
        OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
        return (0u);
    }

    flags_rdy = OSTCBCurPtr->FlagsRdy;
    if (consume == DEF_TRUE) {                                  /* See if we need to consume the flags                  */
        switch (mode) {
            case OS_OPT_PEND_FLAG_SET_ALL:
            case OS_OPT_PEND_FLAG_SET_ANY:                      /* Clear ONLY the flags we got                          */
                 p_grp->Flags &= ~flags_rdy;
                 break;

#if (OS_CFG_FLAG_MODE_CLR_EN == DEF_ENABLED)
            case OS_OPT_PEND_FLAG_CLR_ALL:
            case OS_OPT_PEND_FLAG_CLR_ANY:                      /* Set   ONLY the flags we got                          */
                 p_grp->Flags |=  flags_rdy;
                 break;
#endif
            default:
                 CPU_CRITICAL_EXIT();
                 OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_INVALID_ARG);
                 RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_ARG, 0u);
        }
    }
    CPU_CRITICAL_EXIT();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        /* Event(s) must have occurred                          */
    OS_TRACE_FLAG_PEND_EXIT(RTOS_ERR_CODE_GET(*p_err));
    return (flags_rdy);
}


/**
*********************************************************************************************************
*                                               OSFlagPendAbort()
*
* @brief    This function aborts & prepares any tasks currently waiting on an event flag group.
*           Rather than posting to the event flag group with OSFlagPost(), you should use this
*           function to fault-abort the wait on the event flag group.
*
* @param    p_grp   Pointer to the event flag group.
*
* @param    opt     Determines the type of ABORT performed:
*                       - OS_OPT_PEND_ABORT_1     ABORT wait for a single task (HPT) waiting on
*                                                 the event flag.
*                       - OS_OPT_PEND_ABORT_ALL   ABORT wait for ALL tasks that are  waiting on
*                                                 the event flag.
*                       - OS_OPT_POST_NO_SCHED    Do not call the scheduler.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s)
*                   from this function:
*                       - RTOS_ERR_NONE
*                       - RTOS_ERR_NONE_WAITING
*
* @return   == 0    If no tasks were waiting on the event flag group, or upon error.
*           >  0    If one or more tasks waiting on the event flag group are now ready and
*                   informed.
*********************************************************************************************************
*/

OS_OBJ_QTY  OSFlagPendAbort (OS_FLAG_GRP  *p_grp,
                             OS_OPT        opt,
                             RTOS_ERR     *p_err)
{
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    CPU_TS         ts;
    OS_OBJ_QTY     nbr_tasks;
    CPU_SR_ALLOC();


    OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

                                                                /* Not allowed to call from an ISR                      */
    OS_ASSERT_DBG_ERR_SET((OSIntNestingCtr == 0u), *p_err, RTOS_ERR_ISR, 0u);

                                                                /* Make sure kernel is running.                         */
    OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_RUNNING), *p_err, RTOS_ERR_NOT_READY, 0u);

                                                                /* Validate 'p_grp'                                     */
    OS_ASSERT_DBG_ERR_SET((p_grp != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

                                                                /* Validate 'opt'                                       */
    OS_ASSERT_DBG_ERR_SET(((opt ==  OS_OPT_PEND_ABORT_1)                           ||
                           (opt ==  OS_OPT_PEND_ABORT_ALL)                         ||
                           (opt == (OS_OPT_PEND_ABORT_1   | OS_OPT_POST_NO_SCHED)) ||
                           (opt == (OS_OPT_PEND_ABORT_ALL | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

                                                                /* Validate event group object                          */
    OS_ASSERT_DBG_ERR_SET((p_grp->Type == OS_OBJ_TYPE_FLAG), *p_err, RTOS_ERR_INVALID_TYPE, 0u);


    CPU_CRITICAL_ENTER();
    p_pend_list = &p_grp->PendList;
    if (p_pend_list->HeadPtr == DEF_NULL) {                     /* Any task waiting on flag group?                      */
        CPU_CRITICAL_EXIT();                                    /* No                                                   */
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE_WAITING);
        return (0u);
    }

    nbr_tasks = 0u;
#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts        = OS_TS_GET();                                    /* Get local time stamp so all tasks get the same time  */
#else
    ts        = 0u;
#endif

    while (p_pend_list->HeadPtr != DEF_NULL) {
        p_tcb = p_pend_list->HeadPtr;
        OS_PendAbort(p_tcb,
                     ts,
                     OS_STATUS_PEND_ABORT);
        nbr_tasks++;
        if (opt != OS_OPT_PEND_ABORT_ALL) {                     /* Pend abort all tasks waiting?                        */
            break;                                              /* No                                                   */
        }
    }
    CPU_CRITICAL_EXIT();

    if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
        OSSched();                                              /* Run the scheduler                                    */
    }

    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (nbr_tasks);
}


/**
*********************************************************************************************************
*                                           OSFlagPendGetFlagsRdy()
*
* @brief    This function is called to obtain the flags that caused the task to be ready.
*           In other words, this function allows you to reveal "Who done it!"
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s)
*                   from this function:
*                       - RTOS_ERR_NONE
*
* @return   The flags that caused the task to be ready.
*********************************************************************************************************
*/

OS_FLAGS  OSFlagPendGetFlagsRdy (RTOS_ERR  *p_err)
{
    OS_FLAGS  flags;
    CPU_SR_ALLOC();


    OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

                                                                /* Make sure kernel is running.                         */
    OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_RUNNING), *p_err, RTOS_ERR_NOT_READY, 0u);

                                                                /* Not allowed to call from an ISR                      */
    OS_ASSERT_DBG_ERR_SET((OSIntNestingCtr == 0u), *p_err, RTOS_ERR_ISR, 0u);

    CPU_CRITICAL_ENTER();
    flags = OSTCBCurPtr->FlagsRdy;
    CPU_CRITICAL_EXIT();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
    return (flags);
}


/**
*********************************************************************************************************
*                                               OSFlagPost()
*
* @brief    This function is called to set or clear some bits in an event flag group. The bits to set
*           or clear are specified by a 'bit mask'.
*
* @param    p_grp   Pointer to the event flag group.
*
* @param    flags   If 'opt' (see below) is OS_OPT_POST_FLAG_SET, each bit set in 'flags' will be
*                   SET to the corresponding bit in the event flag group. For example, to set bits
*                   0, 4, and 5, you would set 'flags' to:
*                   @n
*                   0x31    (note, bit 0 is least significant bit)
*                   @n
*                   If 'opt' (see below) is OS_OPT_POST_FLAG_CLR, each bit set in 'flags'
*                   will CLEAR the corresponding bit in the event flag group. For example, to clear
*                   bits 0, 4, and 5, you would specify 'flags' as:
*                   @n
*                   0x31    (note, bit 0 is least significant bit)
*                   @n
*
* @param    opt     Indicates whether the flags will be Set or Cleared :
*                       - OS_OPT_POST_FLAG_SET    Set.
*                       - OS_OPT_POST_FLAG_CLR    Cleared.
*                   You can also 'ADD' OS_OPT_POST_NO_SCHED to prevent the scheduler from being
*                   called.
*
* @param    p_err   Pointer to the variable that will receive one of the following error code(s)
*                   from this function:
*                       - RTOS_ERR_NONE
*
* @return   The new value of the event flags bits that are still set.
*          
* @note     (1) The execution time of this function depends on the number of tasks waiting on the
*               event flag group.
*********************************************************************************************************
*/

OS_FLAGS  OSFlagPost (OS_FLAG_GRP  *p_grp,
                      OS_FLAGS      flags,
                      OS_OPT        opt,
                      RTOS_ERR     *p_err)
{

    OS_FLAGS       flags_cur;
    OS_FLAGS       flags_rdy;
    OS_OPT         mode;
    OS_PEND_LIST  *p_pend_list;
    OS_TCB        *p_tcb;
    OS_TCB        *p_tcb_next;
    CPU_TS         ts;
    CPU_SR_ALLOC();


    OS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0u);

    OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt);

                                                                /* Make sure kernel is running.                         */
    OS_ASSERT_DBG_ERR_SET((OSRunning == OS_STATE_OS_RUNNING), *p_err, RTOS_ERR_NOT_READY, 0u);

                                                                /* Validate 'p_grp'                                     */
    OS_ASSERT_DBG_ERR_SET((p_grp != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0u);

                                                                /* Validate 'opt'                                       */
    OS_ASSERT_DBG_ERR_SET(((opt ==  OS_OPT_POST_FLAG_SET)                         ||
                           (opt ==  OS_OPT_POST_FLAG_CLR)                         ||
                           (opt == (OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED)) ||
                           (opt == (OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED))), *p_err, RTOS_ERR_INVALID_ARG, 0u);

                                                                /* Validate event group object                          */
    OS_ASSERT_DBG_ERR_SET((p_grp->Type == OS_OBJ_TYPE_FLAG), *p_err, RTOS_ERR_INVALID_TYPE, 0u);

#if (OS_CFG_TS_EN == DEF_ENABLED)
    ts = OS_TS_GET();                                           /* Get timestamp                                        */
#else
    ts = 0u;
#endif

    OS_TRACE_FLAG_POST(p_grp);

    CPU_CRITICAL_ENTER();
    switch (opt) {
        case OS_OPT_POST_FLAG_SET:
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED:
             p_grp->Flags |=  flags;                            /* Set   the flags specified in the group               */
             break;

        case OS_OPT_POST_FLAG_CLR:
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED:
             p_grp->Flags &= ~flags;                            /* Clear the flags specified in the group               */
             break;

        default:
             CPU_CRITICAL_EXIT();                               /* INVALID option, checked before, should never happen  */
             OS_TRACE_FLAG_POST_EXIT(RTOS_ERR_INVALID_ARG);
             OS_TRACE_FLAG_POST_FAILED(p_grp);
             RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_ARG, 0u);
    }
#if (OS_CFG_TS_EN == DEF_ENABLED)
    p_grp->TS   = ts;
#endif
    p_pend_list = &p_grp->PendList;
    if (p_pend_list->HeadPtr == DEF_NULL) {                     /* Any task waiting on event flag group?                */
        CPU_CRITICAL_EXIT();                                    /* No                                                   */
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        OS_TRACE_FLAG_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));
        return (p_grp->Flags);
    }

    p_tcb = p_pend_list->HeadPtr;
    while (p_tcb != DEF_NULL) {                                 /* Go through all tasks waiting on event flag(s)        */
        p_tcb_next = p_tcb->PendNextPtr;
        mode       = p_tcb->FlagsOpt & OS_OPT_PEND_FLAG_MASK;
        switch (mode) {
            case OS_OPT_PEND_FLAG_SET_ALL:                      /* See if all req. flags are set for current node       */
                 flags_rdy = (p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy == p_tcb->FlagsPend) {
                     OS_FlagTaskRdy(p_tcb,                      /* Make task RTR, event(s) Rx'd                         */
                                    flags_rdy,
                                    ts);
                 }
                 break;

            case OS_OPT_PEND_FLAG_SET_ANY:                      /* See if any flag set                                  */
                 flags_rdy = (p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy != 0u) {
                     OS_FlagTaskRdy(p_tcb,                      /* Make task RTR, event(s) Rx'd                         */
                                    flags_rdy,
                                    ts);
                 }
                 break;

#if (OS_CFG_FLAG_MODE_CLR_EN == DEF_ENABLED)
            case OS_OPT_PEND_FLAG_CLR_ALL:                      /* See if all req. flags are set for current node       */
                 flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy == p_tcb->FlagsPend) {
                     OS_FlagTaskRdy(p_tcb,                      /* Make task RTR, event(s) Rx'd                         */
                                    flags_rdy,
                                    ts);
                 }
                 break;

            case OS_OPT_PEND_FLAG_CLR_ANY:                      /* See if any flag set                                  */
                 flags_rdy = (OS_FLAGS)(~p_grp->Flags & p_tcb->FlagsPend);
                 if (flags_rdy != 0u) {
                     OS_FlagTaskRdy(p_tcb,                      /* Make task RTR, event(s) Rx'd                         */
                                    flags_rdy,
                                    ts);
                 }
                 break;
#endif
            default:
                 CPU_CRITICAL_EXIT();                           /* Checked before, should never happen                  */
                 OS_TRACE_FLAG_POST_EXIT(RTOS_ERR_INVALID_ARG);
                 OS_TRACE_FLAG_POST_FAILED(p_grp);
                 RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_INVALID_ARG, 0u);
        }
                                                                /* Point to next task waiting for event flag(s)         */
        p_tcb = p_tcb_next;
    }
    CPU_CRITICAL_EXIT();

    if ((opt & OS_OPT_POST_NO_SCHED) == 0u) {
        OSSched();
    }

    CPU_CRITICAL_ENTER();
    flags_cur = p_grp->Flags;
    CPU_CRITICAL_EXIT();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

    OS_TRACE_FLAG_POST_EXIT(RTOS_ERR_CODE_GET(*p_err));

    return (flags_cur);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           INTERNAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               OS_FlagBlock()
*
* @brief    This function is internal to the Kernel and is used to put a task to sleep until the
*           desired event flag bit(s) are set.
*
* @param    p_grp       Pointer to the event flag group.
*
* @param    flags       Bit pattern indicating which bit(s) (i.e. flags) you wish to check.
*                       The bits you want are specified by setting the corresponding bits in
*                       'flags'. e.g. if your application wants to wait for bits 0 and 1 then
*                       'flags' would contain 0x03.
*
* @param    opt         Specifies whether you want ALL bits to be set/cleared or ANY of the bits
*                       to be set/cleared.
*                       You can specify the following argument:
*                           - OS_OPT_PEND_FLAG_CLR_ALL    Check ALL bits in 'mask' to be clear. (0)
*                           - OS_OPT_PEND_FLAG_CLR_ANY    Check ANY bit  in 'mask' to be clear. (0)
*                           - OS_OPT_PEND_FLAG_SET_ALL    Check ALL bits in 'mask' to be set.   (1)
*                           - OS_OPT_PEND_FLAG_SET_ANY    Check ANY bit  in 'mask' to be set.   (1)
*
* @param    timeout     The desired amount of time that the task will wait for the event flag
*                       bit(s) to be set.
*          
* @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
*********************************************************************************************************
*/

void  OS_FlagBlock (OS_FLAG_GRP  *p_grp,
                    OS_FLAGS      flags,
                    OS_OPT        opt,
                    OS_TICK       timeout)
{
    OSTCBCurPtr->FlagsPend = flags;                             /* Save the flags that we need to wait for              */
    OSTCBCurPtr->FlagsOpt  = opt;                               /* Save the type of wait we are doing                   */
    OSTCBCurPtr->FlagsRdy  = 0u;

    OS_Pend((OS_PEND_OBJ *)((void *)p_grp),
             OS_TASK_PEND_ON_FLAG,
             timeout);
}


/**
*********************************************************************************************************
*                                               OS_FlagClr()
*
* @brief    This function is called by OSFlagDel() to clear the contents of an event flag group.
*
* @param    p_grp   Pointer to the event flag group to clear.
*          
* @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
*********************************************************************************************************
*/

void  OS_FlagClr (OS_FLAG_GRP  *p_grp)
{
    OS_PEND_LIST  *p_pend_list;


#if (OS_OBJ_TYPE_REQ == DEF_ENABLED)
    p_grp->Type             = OS_OBJ_TYPE_NONE;
#endif
#if (OS_CFG_DBG_EN == DEF_ENABLED)
    p_grp->NamePtr          = (CPU_CHAR *)((void *)"?FLAG");    /* Unknown name                                         */
#endif
    p_grp->Flags            =  0u;
    p_pend_list             = &p_grp->PendList;
    OS_PendListInit(p_pend_list);
}


/**
*********************************************************************************************************
*                                           OS_FlagDbgListAdd()
*
* @brief    Add an event flag group from the event flag debug list.
*
* @param    p_grp   Pointer to the event flag group to add.
*          
* @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
*********************************************************************************************************
*/

#if (OS_CFG_DBG_EN == DEF_ENABLED)
void  OS_FlagDbgListAdd (OS_FLAG_GRP  *p_grp)
{
    p_grp->DbgNamePtr                = (CPU_CHAR *)((void *)" ");
    p_grp->DbgPrevPtr                = DEF_NULL;
    if (OSFlagDbgListPtr == DEF_NULL) {
        p_grp->DbgNextPtr            = DEF_NULL;
    } else {
        p_grp->DbgNextPtr            = OSFlagDbgListPtr;
        OSFlagDbgListPtr->DbgPrevPtr = p_grp;
    }
    OSFlagDbgListPtr                 = p_grp;
}


/**
*********************************************************************************************************
*                                           OS_FlagDbgListRemove()
*
* @brief    Remove an event flag group from the event flag debug list.
*
* @param    p_grp   Pointer to the event flag group to remove.
*          
* @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
*********************************************************************************************************
*/

void  OS_FlagDbgListRemove (OS_FLAG_GRP  *p_grp)
{
    OS_FLAG_GRP  *p_grp_next;
    OS_FLAG_GRP  *p_grp_prev;


    p_grp_prev = p_grp->DbgPrevPtr;
    p_grp_next = p_grp->DbgNextPtr;

    if (p_grp_prev == DEF_NULL) {
        OSFlagDbgListPtr = p_grp_next;
        if (p_grp_next != DEF_NULL) {
            p_grp_next->DbgPrevPtr = DEF_NULL;
        }
        p_grp->DbgNextPtr = DEF_NULL;

    } else if (p_grp_next == DEF_NULL) {
        p_grp_prev->DbgNextPtr = DEF_NULL;
        p_grp->DbgPrevPtr      = DEF_NULL;

    } else {
        p_grp_prev->DbgNextPtr =  p_grp_next;
        p_grp_next->DbgPrevPtr =  p_grp_prev;
        p_grp->DbgNextPtr      = DEF_NULL;
        p_grp->DbgPrevPtr      = DEF_NULL;
    }
}
#endif


/**
*********************************************************************************************************
*                                               OS_FlagTaskRdy()
*
* @brief    This function is internal to the Kernel and is used to make a task ready-to-run because
*           the desired event flag bits have been set.
*
* @param    p_tcb       Pointer to the OS_TCB of the task to remove.
*
* @param    flags_rdy   Contains the bit pattern of the event flags that cause the task to become
*                       ready-to-run.
*
* @param    ts          Timestamp associated with the post.
*          
* @note     (1) This function is INTERNAL to the Kernel and your application MUST NOT call it.
*********************************************************************************************************
*/

void   OS_FlagTaskRdy (OS_TCB    *p_tcb,
                       OS_FLAGS   flags_rdy,
                       CPU_TS     ts)
{
#if (OS_CFG_TS_EN == DEF_DISABLED)
    (void)ts;                                                   /* Prevent compiler warning for not using 'ts'          */
#endif

    p_tcb->FlagsRdy   = flags_rdy;
    p_tcb->PendStatus = OS_STATUS_PEND_OK;                      /* Clear pend status                                    */
    p_tcb->PendOn     = OS_TASK_PEND_ON_NOTHING;                /* Indicate no longer pending                           */
#if (OS_CFG_TS_EN == DEF_ENABLED)
    p_tcb->TS         = ts;
#endif
    switch (p_tcb->TaskState) {
        case OS_TASK_STATE_PEND:
        case OS_TASK_STATE_PEND_TIMEOUT:
#if (OS_CFG_TASK_TICK_EN == DEF_ENABLED)
             if (p_tcb->TaskState == OS_TASK_STATE_PEND_TIMEOUT) {
                 OS_TickListRemove(p_tcb);                      /* Remove from tick list                                */
             }
#endif
             OS_RdyListInsert(p_tcb);                           /* Insert the task in the ready list                    */
             p_tcb->TaskState = OS_TASK_STATE_RDY;
             break;

        case OS_TASK_STATE_PEND_SUSPENDED:
        case OS_TASK_STATE_PEND_TIMEOUT_SUSPENDED:
             p_tcb->TaskState = OS_TASK_STATE_SUSPENDED;
             break;

        case OS_TASK_STATE_RDY:
        case OS_TASK_STATE_DLY:
        case OS_TASK_STATE_DLY_SUSPENDED:
        case OS_TASK_STATE_SUSPENDED:
             break;

        case OS_TASK_STATE_DEL:
        default:
             RTOS_CRITICAL_FAIL_EXEC(RTOS_ERR_OS, ;);
    }
    OS_PendListRemove(p_tcb);
}
#endif


/*
*********************************************************************************************************
*********************************************************************************************************
*                                   DEPENDENCIES & AVAIL CHECK(S) END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif  /* (defined(RTOS_MODULE_KERNEL_AVAIL)) */
