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
*                                         KERNEL CONFIGURATION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : os_cfg.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _OS_CFG_H_
#define  _OS_CFG_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/include/lib_def.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            MISCELLANEOUS
*
* Note(s) : (1) Configure OS_CFG_APP_HOOKS_EN to enable or disable Application-specific Hooks.
*
*           (2) Configure OS_CFG_DBG_EN to enable or disable debug helper code and variables.
*
*           (3) Configure OS_CFG_DYN_TICK_EN to enable or disable the Dynamic Tick feature.
*
*           (4) Configure OS_CFG_TS_EN to enable or disable Timestamping capabilities.
*
*           (5) Configure OS_CFG_PRIO_MAX to set the maximum number of Task Priorities (see OS_PRIO data
*               type).
*
*           (6) Configure OS_CFG_SCHED_LOCK_TIME_MEAS_EN to enable or disable the Scheduler Lock time
*               measurement code.
*
*           (7) Configure OS_CFG_SCHED_ROUND_ROBIN_EN to enable or disable the Round-Robin Scheduler.
*
*           (8) Configure OS_CFG_STK_SIZE_MIN to set the minimum allowable Task Stack size (in CPU_STK
*               elements).
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_APP_HOOKS_EN                                DEF_DISABLED

#define  OS_CFG_DBG_EN                                      DEF_ENABLED

#define  OS_CFG_DYN_TICK_EN                                 DEF_DISABLED

#define  OS_CFG_TS_EN                                       DEF_ENABLED

#define  OS_CFG_PRIO_MAX                                    64u

#define  OS_CFG_SCHED_LOCK_TIME_MEAS_EN                     DEF_DISABLED

#define  OS_CFG_SCHED_ROUND_ROBIN_EN                        DEF_ENABLED

#define  OS_CFG_STK_SIZE_MIN                                64u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             EVENT FLAGS
*
* Note(s) : (1) Configure OS_CFG_FLAG_EN to enable or disable the Event Flags synchronization construct.
*
*           (2) Configure OS_CFG_FLAG_MODE_CLR_EN to enable or disable the active-low mode of Event
*               Flags.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_FLAG_EN                                     DEF_ENABLED

#define  OS_CFG_FLAG_MODE_CLR_EN                            DEF_DISABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                     MUTUAL EXCLUSION SEMAPHORES
*
* Note(s) : (1) Configure OS_CFG_MUTEX_EN to enable or disable the Mutual Exclusion (Mutex)
*               synchronization construct.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_MUTEX_EN                                    DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                            MESSAGE QUEUES
*
* Note(s) : (1) Configure OS_CFG_Q_EN to enable or disable the Message Queue construct.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_Q_EN                                        DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              SEMAPHORES
*
* Note(s) : (1) Configure OS_CFG_SEM_EN to enable or disable the Semaphore synchronization construct.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_SEM_EN                                      DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MONITORS
*
* Note(s) : (1) Configure OS_CFG_MON_EN to enable or disable the Monitor (condition variable)
*               synchronization construct.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_MON_EN                                      DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           TASK MANAGEMENT
*
* Note(s) : (1) Configure OS_CFG_STAT_TASK_EN to enable or disable the Statistics gathering Task.
*
*           (2) Configure OS_CFG_STAT_TASK_STK_CHK_EN to enable or disable the stack overflow detection
*               of the Statistics Task.
*
*           (3) Configure OS_CFG_TASK_IDLE_EN to enable or disable the Idle Task.
*
*           (4) Configure OS_CFG_TASK_PROFILE_EN to enable or disable Task profiling instrumentation.
*
*           (5) Configure OS_CFG_TASK_Q_EN to enable or disable built-in Task Message Queues.
*
*           (6) Configure OS_CFG_TASK_REG_TBL_SIZE to set the number of Task Registers.
*
*           (7) Configure OS_CFG_TASK_STK_REDZONE_EN to enable or disable the Redzone Stack Protection.
*
*           (8) Configure OS_CFG_TASK_STK_REDZONE_DEPTH to set the depth of the Redzone Stack
*               Protection.
*
*           (9) Configure OS_CFG_TASK_TICK_EN to enable or disable the Tick Task.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_STAT_TASK_EN                                DEF_ENABLED

#define  OS_CFG_STAT_TASK_STK_CHK_EN                        DEF_ENABLED

#define  OS_CFG_TASK_IDLE_EN                                DEF_ENABLED

#define  OS_CFG_TASK_PROFILE_EN                             DEF_ENABLED

#define  OS_CFG_TASK_Q_EN                                   DEF_ENABLED

#define  OS_CFG_TASK_REG_TBL_SIZE                           3u

#define  OS_CFG_TASK_STK_REDZONE_EN                         DEF_ENABLED

#define  OS_CFG_TASK_STK_REDZONE_DEPTH                      8u

#define  OS_CFG_TASK_TICK_EN                                DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                    TASK LOCAL STORAGE MANAGEMENT
*
* Note(s) : (1) Configure OS_CFG_TLS_TBL_SIZE to set the number of Task Local Storage registers.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_TLS_TBL_SIZE                                0u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           TIMER MANAGEMENT
*
* Note(s) : (1) Configure OS_CFG_TMR_EN to enable or disable Software Timers.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_TMR_EN                                      DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           Trace Recorders
*
* Note(s) : (1) Configure OS_CFG_TRACE_EN to enable or disable the kernel events trace recorder.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_TRACE_EN                                    DEF_DISABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                              DEPRECATED
*
* Note(s) : (1) The following configurations are DEPRECATED and will be removed in a future version of
*               this product.
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                          MEMORY MANAGEMENT
*
* Note(s) : (1) Configure OS_CFG_MEM_EN to enable or disable the built-in Memory Manager.
*********************************************************************************************************
*********************************************************************************************************
*/

#define  OS_CFG_MEM_EN                                      DEF_DISABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of os_cfg.h module include.                      */
