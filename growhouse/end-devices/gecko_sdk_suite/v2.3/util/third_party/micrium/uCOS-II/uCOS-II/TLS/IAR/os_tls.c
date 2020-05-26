/*
************************************************************************************************************************
*                                                      uC/OS-II
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2016; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                        THREAD LOCAL STORAGE (TLS) MANAGEMENT
*                                                 IAR IMPLEMENTATION
*
* File    : OS_TLS.C
* By      : JJL
* Version : V2.92.12
*
* LICENSING TERMS:
* ---------------
*           uC/OS-II is provided in source form for FREE short-term evaluation, for educational use or
*           for peaceful research.  If you plan or intend to use uC/OS-II in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-II for its use in your
*           application/product.   We provide ALL the source code for your convenience and to help you
*           experience uC/OS-II.  The fact that the source is provided does NOT mean that you can use
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#define  MICRIUM_SOURCE
#include <ucos_ii.h>
#include <yvals.h>
#include <stdint.h>
#include <stdio.h>


#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  char  *os_tls__c = "$Id: $";
#endif

#if defined(OS_TLS_TBL_SIZE) && (OS_TLS_TBL_SIZE > 0u)
#if OS_TASK_CREATE_EXT_EN > 0u

/*
************************************************************************************************************************
*                                                     LOCAL DEFINES
************************************************************************************************************************
*/


#if     (_DLIB_FILE_DESCRIPTOR > 0) && (_FILE_OP_LOCKS > 0)
#define  OS_TLS_LOCK_MAX              ((_MAX_LOCK) + (FOPEN_MAX))	      /* _MAX_LOCK and _FOPEN_MAX defined by IAR  */
#else
#define  OS_TLS_LOCK_MAX               (_MAX_LOCK)
#endif

/*
************************************************************************************************************************
*                                                   LOCAL DATA TYPES
************************************************************************************************************************
*/

typedef  struct  os_tls_lock   OS_TLS_LOCK;


struct  os_tls_lock {
    OS_EVENT     *Sem;                                                    /* OS semaphore object.                     */
    OS_TLS_LOCK  *NextPtr;                                                /* Pointer to the next object in the pool.  */
};

/*
************************************************************************************************************************
*                                                   LOCAL VARIABLES
************************************************************************************************************************
*/

static  INT8U               OS_TLS_NextAvailID;                           /* Next available TLS ID                    */

static  OS_TLS_ID           OS_TLS_LibID;                                 /* ID used to store library space pointer   */

static  OS_TLS_LOCK         OS_TLS_LockPoolTbl[OS_TLS_LOCK_MAX];
static  OS_TLS_LOCK        *OS_TLS_LockPoolListPtr = (OS_TLS_LOCK *)0;    /* Pointer to head of 'OS_TLS_LOCK' list    */

/*
************************************************************************************************************************
*                                                    LOCAL FUNCTIONS
************************************************************************************************************************
*/

static  void   OS_TLS_LockCreate  (void   **p_lock);
static  void   OS_TLS_LockDel     (void    *p_lock);
static  void   OS_TLS_LockAcquire (void    *p_lock);
static  void   OS_TLS_LockRelease (void    *p_lock);

/*
************************************************************************************************************************
*                                          ALLOCATE THE NEXT AVAILABLE TLS ID
*
* Description: This function is called to obtain the ID of the next free TLS (Task Local Storage) register 'id'
*
* Arguments  : perr       is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE               if the call was successful
*                            OS_ERR_TLS_NO_MORE_AVAIL  if you are attempting to assign more TLS than you declared
*                                                           available through OS_TLS_TBL_SIZE.
*
* Returns    : The next available TLS 'id' or OS_TLS_TBL_SIZE if an error is detected.
*
* Caller(s)  : OS_TLS_Init()
************************************************************************************************************************
*/

OS_TLS_ID  OS_TLS_GetID (INT8U  *perr)
{
    OS_TLS_ID  id;
#if OS_CRITICAL_METHOD == 3u                                      /* Allocate storage for CPU status register         */
    OS_CPU_SR  cpu_sr = 0u;
#endif



#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_TLS_ID)OS_TLS_TBL_SIZE);
    }
#endif

    OS_ENTER_CRITICAL();
    if (OS_TLS_NextAvailID >= OS_TLS_TBL_SIZE) {                  /* See if we exceeded the number of IDs available   */
       *perr = OS_ERR_TLS_NO_MORE_AVAIL;                          /* Yes, cannot allocate more TLS                    */
        OS_EXIT_CRITICAL();
        return ((OS_TLS_ID)OS_TLS_TBL_SIZE);
    }

    id   = OS_TLS_NextAvailID;	  								  /* Assign the next available ID                     */
    OS_TLS_NextAvailID++;										  /* Increment available ID for next request          */
    OS_EXIT_CRITICAL();
   *perr = OS_ERR_NONE;
    return (id);
}

/*
************************************************************************************************************************
*                                        GET THE CURRENT VALUE OF A TLS REGISTER
*
* Description: This function is called to obtain the current value of a TLS register
*
* Arguments  : ptcb     is a pointer to the OS_TCB of the task you want to read the TLS register from.  If 'ptcb' is
*                        a NULL pointer then you will get the TLS register of the current task.
*
*              id        is the 'id' of the desired TLS register.  Note that the 'id' must be less than
*                        'OS_TLS_NextAvailID'
*
*              perr     is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_OS_NOT_RUNNING  if the kernel has not started yet
*                            OS_ERR_TLS_ID_INVALID  if the 'id' is greater or equal to OS_TLS_NextAvailID
*                            OS_ERR_TLS_NOT_EN      if the task was created by specifying that TLS support was not
*                                                     needed for the task
*
* Returns    : The current value of the task's TLS register or 0 if an error is detected.
*
* Note(s)    : 1) ptcb->OSTCBOpt contains options passed to OSTaskCreate().  One of these options (OS_TASK_OPT_NO_TLS)
*                 is used to specify that the user doesn't want TLS support for the task being created.  In other words,
*                 by default, TLS support is enabled if OS_TLS_TBL_SIZE is defined and > 0 so the user must
*                 specifically indicate that he/she doesn't want TLS supported for a task.
************************************************************************************************************************
*/

OS_TLS  OS_TLS_GetValue (OS_TCB     *ptcb,
                         OS_TLS_ID   id,
                         INT8U      *perr)
{
    OS_TLS     value;
#if OS_CRITICAL_METHOD == 3u                                      /* Allocate storage for CPU status register         */
    OS_CPU_SR  cpu_sr = 0u;
#endif



#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return ((OS_TLS)0);
    }
#endif


#if OS_ARG_CHK_EN > 0u
    if (id >= OS_TLS_NextAvailID) {                               /* Caller must specify an ID that's been assigned   */
       *perr = OS_ERR_TLS_ID_INVALID;
        return ((OS_TLS)0);
    }
#endif

    OS_ENTER_CRITICAL();
    if (ptcb == (OS_TCB *)0) {                                    /* Does caller want to use current task's TCB?      */
        ptcb = OSTCBCur;                                          /* Yes                                              */
        if (OSTCBCur == (OS_TCB *)0) {                            /* Is the kernel running?                           */
            OS_EXIT_CRITICAL();                                   /* No, then caller cannot specify NULL              */
           *perr = OS_ERR_OS_NOT_RUNNING;
            return ((OS_TLS)0);
        }
    }
    if ((ptcb->OSTCBOpt & OS_TASK_OPT_NO_TLS) == OS_TASK_OPT_NONE) { /* See if TLS is available for this task         */
        value = ptcb->OSTCBTLSTbl[id];                               /* Yes                                           */
        OS_EXIT_CRITICAL();
       *perr  = OS_ERR_NONE;
        return ((OS_TLS)value);
    } else {
        OS_EXIT_CRITICAL();                                          /* No                                            */
       *perr  = OS_ERR_TLS_NOT_EN;
        return ((OS_TLS)0);
    }
}

/*
************************************************************************************************************************
*                                          DEFINE TLS DESTRUCTOR FUNCTION
*
* Description: This function is called by the user to assign a 'destructor' function to a specific TLS.  When a task is
*              deleted, all the destructors are called for all the task's TLS for which there is a destructor function
*              defined.  In other when a task is deleted, all the non-NULL functions present in OS_TLS_DestructPtrTbl[]
*              will be called.
*
* Arguments  : id          is the ID of the TLS destructor to set
*
*              pdestruct   is a pointer to a function that is associated with a specific TLS register and is called when
*                          a task is deleted.  The prototype of such functions is:
*
*                            void  MyDestructFunction (OS_TCB     *ptcb,
*                                                      OS_TLS_ID   id,
*                                                      OS_TLS      value);
*
*                          you can specify a NULL pointer if you don't want to have a fucntion associated with a TLS
*                          register.  A NULL pointer (i.e. no function associated with a TLS register) is the default
*                          value placed in OS_TLS_DestructPtrTbl[].
*
*              perr       is a pointer to an error return code.  The possible values are:
*
*                            OS_ERR_NONE             The call was successful.
*                            OS_ERR_TLS_ID_INVALID   You you specified an invalid TLS ID
*
* Returns    : none
*
* Note       : none
************************************************************************************************************************
*/

void  OS_TLS_SetDestruct (OS_TLS_ID            id,
                          OS_TLS_DESTRUCT_PTR  pdestruct,
                          INT8U               *perr)
{
   *perr = OS_ERR_NONE;
}

/*
************************************************************************************************************************
*                                       SET THE CURRENT VALUE OF A TASK TLS REGISTER
*
* Description: This function is called to change the current value of a task TLS register.
*
* Arguments  : ptcb      is a pointer to the OS_TCB of the task you want to set the task's TLS register for.  If 'ptcb'
*                        is a NULL pointer then you will change the TLS register of the current task.
*
*              id        is the 'id' of the desired task TLS register.  Note that the 'id' must be less than
*                        'OS_TLS_NextAvailID'
*
*              value     is the desired value for the task TLS register.
*
*              perr      is a pointer to a variable that will hold an error code related to this call.
*
*                            OS_ERR_NONE            if the call was successful
*                            OS_ERR_OS_NOT_RUNNING  if the kernel has not started yet
*                            OS_ERR_TLS_ID_INVALID  if you specified an invalid TLS ID
*                            OS_ERR_TLS_NOT_EN      if the task was created by specifying that TLS support was not
*                                                     needed for the task
*
* Returns    : none
*
* Note(s)    : 1) ptcb->OSTCBOpt contains options passed to OSTaskCreate().  One of these options (OS_TASK_OPT_NO_TLS)
*                 is used to specify that the user doesn't want TLS support for the task being created.  In other words,
*                 by default, TLS support is enabled if OS_TLS_TBL_SIZE is defined and > 0 so the user must
*                 specifically indicate that he/she doesn't want TLS supported for a task.
************************************************************************************************************************
*/

void  OS_TLS_SetValue (OS_TCB     *ptcb,
                       OS_TLS_ID   id,
                       OS_TLS      value,
                       INT8U      *perr)
{
#if OS_CRITICAL_METHOD == 3u                                      /* Allocate storage for CPU status register         */
    OS_CPU_SR  cpu_sr = 0u;
#endif



#ifdef OS_SAFETY_CRITICAL
    if (perr == (INT8U *)0) {
        OS_SAFETY_CRITICAL_EXCEPTION();
        return;
    }
#endif

#if OS_ARG_CHK_EN > 0u
    if (id >= OS_TLS_NextAvailID) {                               /* Caller must specify an ID that's been assigned   */
       *perr = OS_ERR_TLS_ID_INVALID;
        return;
    }
#endif

    OS_ENTER_CRITICAL();                                          /* Does caller want to use current task's TCB?      */
    if (ptcb == (OS_TCB *)0) {                                    /* Yes                                              */
        ptcb = OSTCBCur;                                          /* Is the kernel running?                           */
        if (OSTCBCur == (OS_TCB *)0) {                            /* No, then caller cannot specify NULL              */
            OS_EXIT_CRITICAL();
           *perr = OS_ERR_OS_NOT_RUNNING;
            return;
        }
    }
    if ((ptcb->OSTCBOpt & OS_TASK_OPT_NO_TLS) == OS_TASK_OPT_NONE) { /* See if TLS is available for this task         */
        ptcb->OSTCBTLSTbl[id] = value;                               /* Yes                                           */
        OS_EXIT_CRITICAL();
       *perr                  = OS_ERR_NONE;
    } else {
        OS_EXIT_CRITICAL();                                          /* No                                            */
       *perr                  = OS_ERR_TLS_NOT_EN;
    }
}

/*
************************************************************************************************************************
************************************************************************************************************************
*                                             uC/OS-III INTERNAL FUNCTIONS
*                                         DO NOT CALL FROM THE APPLICATION CODE
************************************************************************************************************************
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                       INITIALIZE THE TASK LOCAL STORAGE SERVICES
*
* Description: This function is called by uC/OS-III to initialize the TLS id allocator.
*
*              This function also initializes an array containing function pointers.  There is one function associated
*                  to each task TLS register and all the functions (assuming non-NULL) will be called when the task is
*                  deleted.
*
* Arguments  : none
*
* Returns    : none
*
* Caller(s)  : OSInit()
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TLS_Init (INT8U *perr)
{
    INT16U        ix;
    OS_TLS_LOCK  *p_lock;
#if OS_CRITICAL_METHOD == 3u                                       /* Allocate storage for CPU status register        */
    OS_CPU_SR     cpu_sr = 0u;
#endif



    OS_TLS_NextAvailID = 0u;
    OS_TLS_LibID       = OS_TLS_GetID(perr);

    OS_ENTER_CRITICAL();
                                                                   /* Create the link list of OS_TLS_LOCK objects.    */
    for (ix = 0u; ix < (OS_TLS_LOCK_MAX - 1u); ix++) {
        p_lock             = &OS_TLS_LockPoolTbl[ix];
        p_lock->NextPtr    = &OS_TLS_LockPoolTbl[ix + 1u];
    }

    p_lock                 = &OS_TLS_LockPoolTbl[OS_TLS_LOCK_MAX - 1u];
    p_lock->NextPtr        = (OS_TLS_LOCK *)0;                     /* Last node points to 'NULL'                      */

    OS_TLS_LockPoolListPtr = &OS_TLS_LockPoolTbl[0];               /* Initialize the list head pointer.               */

    OS_EXIT_CRITICAL();
}

/*
************************************************************************************************************************
*                                                  TASK CREATE HOOK
*
* Description: This function is called by OSTaskCreate()
*
* Arguments  : ptcb     is a pointer to the OS_TCB of the task being created.
*
* Returns    : none
*
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
*
*              2) OSTaskCreate() clears all entries in ptcb->OSTCBTLSTbl[] before calling OS_TLS_TaskCreate() so no need
*                 to this here.
************************************************************************************************************************
*/

void  OS_TLS_TaskCreate (OS_TCB  *ptcb)
{
    OS_TLS  p_tls;


    if ((ptcb->OSTCBOpt & OS_TASK_OPT_NO_TLS) == OS_TASK_OPT_NONE) { /* See if TLS is available for this task         */
	                                                                 /* Get TLS segment from the HEAP.                */
        p_tls                           = (OS_TLS)__iar_dlib_perthread_allocate();
        __iar_dlib_perthread_initialize(p_tls);                      /* Initialize the TLS segment.                   */
        ptcb->OSTCBTLSTbl[OS_TLS_LibID] = p_tls;                     /* Set the TLS segment pointer in the task.      */
    }
}

/*
************************************************************************************************************************
*                                                  TASK DELETE HOOK
*
* Description: This function is called by OSTaskDel()
*
* Arguments  : ptcb     is a pointer to the OS_TCB of the task being deleted.
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
************************************************************************************************************************
*/

void  OS_TLS_TaskDel (OS_TCB  *ptcb)
{
    OS_TLS  p_tls;


    if ((ptcb->OSTCBOpt & OS_TASK_OPT_NO_TLS) == OS_TASK_OPT_NONE) { /* See if TLS is available for this task         */
        p_tls                           = ptcb->OSTCBTLSTbl[OS_TLS_LibID];
        __iar_dlib_perthread_destroy();
        __iar_dlib_perthread_deallocate((void *)p_tls);              /* De-allocate the TLS segment.                  */
        ptcb->OSTCBTLSTbl[OS_TLS_LibID] = (OS_TLS)0;                 /* Remove the TLS segment pointer from the task. */
    }
}

/*
************************************************************************************************************************
*                                                  TASK SWITCH HOOK
*
* Description: This function is called by OSSched() and OSIntExit() just prior to calling the context switch code
*
* Arguments  : none
*
* Returns    : none
*
* Note(s)    : 1) This function is INTERNAL to uC/OS-III and your application should not call it.
*
*              2) It's assumed that OSTCBCur points to the task being switched out and OSTCBHighRdy points to the
*                 task being switched in.
************************************************************************************************************************
*/

void  OS_TLS_TaskSw (void)
{
    if ((OSTCBHighRdy->OSTCBOpt & OS_TASK_OPT_NO_TLS) == OS_TASK_OPT_NONE) { /* See if TLS is available for this task */
    }
}

/*
************************************************************************************************************************
************************************************************************************************************************
*                                          uC/OS-III MUTEX IMPLEMENTATION
************************************************************************************************************************
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                                     OS LOCK CREATE
*
* Description : Allocate  a new 'OS_TLS_LOCK' object from the free pool and create a 'OS_SEM' in the kernel.
*
* Argument(s) : p_lock  Pointer to the DLIB lock handler.
*
* Return(s)   : None.
*
* Caller(s)   : __iar_system_Mtxinit()
*               __iar_file_Mtxinit()
*
* Note(s)     : None.
************************************************************************************************************************
*/

static  void  OS_TLS_LockCreate (void  **p_lock)
{
    OS_TLS_LOCK   *p_tls_lock;
    OS_EVENT      *psem;
#if OS_CRITICAL_METHOD == 3u                                      /* Allocate storage for CPU status register         */
    OS_CPU_SR      cpu_sr = 0u;
#endif


    if (p_lock == (void **)0) {
        return;
    }

    if (OS_TLS_LockPoolListPtr == (OS_TLS_LOCK *)0) {             /* If 'OS_TLS_LOCK' object pool is empty?           */
        *p_lock = (void *)0;                                      /*   return a 'NULL' pointer.                       */
        return;
    }

    p_tls_lock = OS_TLS_LockPoolListPtr;                          /* Get the first object in the list.                */

    psem       = OSSemCreate(1);                                  /* Create the semaphore in the kernel.              */

    if (psem == (OS_EVENT *)0) {                                  /* If the semaphore create funtion failed ...       */
        *p_lock = (void *)0;                                      /* ... return a 'NULL' pointer.                     */
         return;
    }

    p_tls_lock->Sem        = psem;

    OS_ENTER_CRITICAL();
    OS_TLS_LockPoolListPtr = p_tls_lock->NextPtr;                 /* Move HEAD pointer to the next object in the list.*/
    OS_EXIT_CRITICAL();

    *p_lock                = (void *)p_tls_lock;                  /* Return the new 'OS_TLS_LOCK' object pointer.     */
}


/*
************************************************************************************************************************
*                                                    OS LOCK DELETE
*
* Description : Delete a 'OS_MUTEX' from the kernel and return the allocated 'OS_TLS_LOCK'
*               to the free pool.
*
* Argument(s) : p_lock  DLIB lock handler.
*
* Return(s)   : Return the current task TLS pointer.
*
* Caller(s)   : __iar_system_Mtxdst()
*               __iar_file_Mtxdst()
*
* Note(s)     : None.
************************************************************************************************************************
*/

static  void  OS_TLS_LockDel (void  *p_lock)
{
    OS_TLS_LOCK   *p_tls_lock;
    INT8U         err;
#if OS_CRITICAL_METHOD == 3u                                      /* Allocate storage for CPU status register         */
    OS_CPU_SR     cpu_sr = 0u;
#endif


    if (p_lock == (void *)0) {
        return;
    }

    p_tls_lock = (OS_TLS_LOCK *)p_lock;

    (void)OSSemDel(p_tls_lock->Sem,
                   OS_DEL_ALWAYS,
                   &err);

    OS_ENTER_CRITICAL();
                                                                  /* Return the OS_TLS_LOCK in front of the list      */
    if (OS_TLS_LockPoolListPtr == (OS_TLS_LOCK *)0) {
        p_tls_lock->NextPtr = (OS_TLS_LOCK *)0;
    } else {
        p_tls_lock->NextPtr = OS_TLS_LockPoolListPtr;
    }
    OS_TLS_LockPoolListPtr  = p_tls_lock;

    OS_EXIT_CRITICAL();
}


/*
************************************************************************************************************************
*                                                      OS LOCK PEND
*
* Description : Wait indefinitely until the lock become available
*
* Arguments   : p_lock  DLIB lock handler.
*
* Return(s)   : Return the current task TLS pointer.
*
* Caller(s)   : __iar_system_Mtxlock()
*               __iar_file_Mtxlock()

* Note(s)     : None.
************************************************************************************************************************
*/

static  void  OS_TLS_LockAcquire (void  *p_lock)
{
    OS_TLS_LOCK  *p_tls_lock;
    INT8U        err;


    if ((p_lock    == (void *)0          ) ||                     /* Return if the lock handler is 'NULL' or the ...  */
        (OSRunning != OS_TRUE)) {                                 /* ... kernel is not running.                       */
        return;
    }

    p_tls_lock = (OS_TLS_LOCK *)p_lock;
    OSSemPend(p_tls_lock->Sem,
              0u,
              &err);
}


/*
************************************************************************************************************************
*                                                      OS LOCK POST
*
* Description : Signal the lock.
*
* Argument(s) : p_lock  DLIB lock handler.
*
* Return(s)   : Return the current task TLS pointer.
*
* Caller(s)   : __iar_system_Mtxunlock()
*               __iar_file_Mtxunlock()
*
* Note(s)     : None.
************************************************************************************************************************
*/

static  void  OS_TLS_LockRelease (void  *p_lock)
{
    OS_TLS_LOCK  *p_tls_lock;


    if ((p_lock    == (void *)0          ) ||                     /* Return if the lock handler is 'NULL' or the ...  */
        (OSRunning != OS_TRUE)) {                                 /* ... kernel is not running.                       */
        return;
    }

    p_tls_lock = (OS_TLS_LOCK *)p_lock;
    (void)OSSemPost(p_tls_lock->Sem);
}

_STD_BEGIN
/*
************************************************************************************************************************
************************************************************************************************************************
*                                              IAR FUNCTIONS IMPLEMENTATIONS
************************************************************************************************************************
************************************************************************************************************************
*/

/*
************************************************************************************************************************
*                                               SYSTEM LOCK INITIALIZATION
*
* Description: Initialize a system lock.
*
* Arguments  : p_lock    Pointer to the lock info object pointer.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void  __iar_system_Mtxinit (__iar_Rmtx  *p_lock)
{
    OS_TLS_LockCreate((void **)p_lock);
}


/*
************************************************************************************************************************
*                                                SYSTEM LOCK DELETE
*
* Description: Delete a system lock.
*
* Arguments  : p_lock    Pointer to the lock info object.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void __iar_system_Mtxdst(__iar_Rmtx  *p_lock)
{
    OS_TLS_LockDel((void *)*p_lock);
}


/*
************************************************************************************************************************
*                                               SYSTEM LOCK PEND
*
* Description: Pend on a system lock.
*
* Arguments  : p_lock    Pointer to the lock info object.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void __iar_system_Mtxlock(__iar_Rmtx  *p_lock)
{
    OS_TLS_LockAcquire((void *)*p_lock);
}


/*
************************************************************************************************************************
*                                                SYSTEM LOCK POST
*
* Description: Signal a system lock.
*
* Arguments  : p_lock    Pointer to the lock info object.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void __iar_system_Mtxunlock(__iar_Rmtx  *p_lock)
{
    OS_TLS_LockRelease((void *)*p_lock);
}

/*
************************************************************************************************************************
*                                               FILE LOCK INITIALIZATION
*
* Description: Initialize a file lock.
*
* Arguments  : p_lock    Pointer to the lock info object pointer.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void  __iar_file_Mtxinit (__iar_Rmtx  *p_lock)
{
    OS_TLS_LockCreate((void **)p_lock);
}


/*
************************************************************************************************************************
*                                                FILE LOCK DELETE
*
* Description: Delete a system lock.
*
* Arguments  : p_lock    Pointer to the lock info object.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void __iar_file_Mtxdst(__iar_Rmtx  *p_lock)
{
    OS_TLS_LockDel((void *)*p_lock);
}


/*
************************************************************************************************************************
*                                                 FILE LOCK PEND
*
* Description: Pend on a file lock.
*
* Arguments  : p_lock    Pointer to the lock info object.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void __iar_file_Mtxlock(__iar_Rmtx  *p_lock)
{
    OS_TLS_LockAcquire((void *)*p_lock);
}


/*
************************************************************************************************************************
*                                                 FILE LOCK POST
*
* Description: Signal in a file lock.
*
* Arguments  : p_lock    Pointer to the lock info object.
*
* Note(s)    : none.
************************************************************************************************************************
*/

void __iar_file_Mtxunlock(__iar_Rmtx  *p_lock)
{
    OS_TLS_LockRelease((void *)*p_lock);
}



/*
************************************************************************************************************************
*                                             GET CURRENT TLS POINTER
*
* Description : Get the current TLS pointer.
*
* Argument(s) : symbp    Pointer to the symbol in the segment.
*
* Return(s)   : Returns symbol address in the current TLS segment.
*
* Note(s)     : None.
************************************************************************************************************************
*/

void  _DLIB_TLS_MEMORY  *__iar_dlib_perthread_access (void  _DLIB_TLS_MEMORY  *symbp)
{
    void  _DLIB_TLS_MEMORY  *p_tls;
    uintptr_t                tls_start;
    uintptr_t                tls_offset;



    if (OSRunning != OS_TRUE) {                                   /* If the kernel is not running yet?                */
        p_tls  = (OS_TLS)__segment_begin("__DLIB_PERTHREAD");     /* ... return the pointer to the main TLS segment.  */
    } else {
        p_tls  = (void *)OSTCBCur->OSTCBTLSTbl[OS_TLS_LibID];     /* Get the pointer to the TLS for the task          */
    }

    tls_start  = (uintptr_t)(p_tls);
    tls_offset = (uintptr_t)(__IAR_DLIB_PERTHREAD_SYMBOL_OFFSET(symbp));
    p_tls      = (void  _DLIB_TLS_MEMORY  *)(tls_start + tls_offset);

    return (p_tls);
}

_STD_END
#endif
#endif
