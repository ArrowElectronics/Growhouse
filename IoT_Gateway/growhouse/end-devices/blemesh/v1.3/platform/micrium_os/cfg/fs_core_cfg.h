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
*                                       FILE SYSTEM CONFIGURATION
*
*                                      CONFIGURATION TEMPLATE FILE
*
* File : fs_core_cfg.h
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _FS_CORE_CFG_H_
#define  _FS_CORE_CFG_H_


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
*                                      FILE SYSTEM CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  FS_CORE_CFG_FAT_EN                                 DEF_ENABLED

#define  FS_CORE_CFG_POSIX_EN                               DEF_ENABLED

#define  FS_CORE_CFG_DIR_EN                                 DEF_ENABLED

#define  FS_CORE_CFG_FILE_BUF_EN                            DEF_ENABLED

#define  FS_CORE_CFG_FILE_LOCK_EN                           DEF_ENABLED

#define  FS_CORE_CFG_PARTITION_EN                           DEF_ENABLED

#define  FS_CORE_CFG_TASK_WORKING_DIR_EN                    DEF_ENABLED

#define  FS_CORE_CFG_UTF8_EN                                DEF_ENABLED

#define  FS_CORE_CFG_THREAD_SAFETY_EN                       DEF_ENABLED

#define  FS_CORE_CFG_ORDERED_WR_EN                          DEF_ENABLED

#define  FS_CORE_CFG_FILE_COPY_EN                           DEF_ENABLED

#define  FS_CORE_CFG_RD_ONLY_EN                             DEF_DISABLED

#define  FS_CORE_CFG_POSIX_PUTCHAR                          putchar

#define  FS_CORE_CFG_MAX_VOL_NAME_LEN                       20u


/*
*********************************************************************************************************
*********************************************************************************************************
*                                   FILE SYSTEM DEBUG CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  FS_CORE_CFG_DBG_MEM_CLR_EN                         DEF_DISABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                             FILE SYSTEM COUNTER MANAGEMENT CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  FS_CORE_CFG_CTR_STAT_EN                            DEF_DISABLED

#define  FS_CORE_CFG_CTR_ERR_EN                             DEF_DISABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                    FILE SYSTEM FAT CONFIGURATION
*********************************************************************************************************
*********************************************************************************************************
*/

#define  FS_FAT_CFG_LFN_EN                                  DEF_ENABLED

#define  FS_FAT_CFG_FAT12_EN                                DEF_ENABLED

#define  FS_FAT_CFG_FAT16_EN                                DEF_ENABLED

#define  FS_FAT_CFG_FAT32_EN                                DEF_ENABLED

#define  FS_FAT_CFG_JOURNAL_EN                              DEF_ENABLED


/*
*********************************************************************************************************
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of fs_core_cfg.h module include.                 */
