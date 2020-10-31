/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Common
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
*                                               MAP INTERFACE
*
* File : map_priv.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _MAP_PRIV_H_
#define  _MAP_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>

#include  <common/source/collections/slist_priv.h>

#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  SLIST_MEMBER  *MAP_INSTANCE;

typedef  struct  map_item {
    SLIST_MEMBER   ListNode;
    CPU_CHAR      *Key;
    void          *Value;
} MAP_ITEM;


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/

void       MapInit        (MAP_INSTANCE  *p_map_instance);

void       MapItemAdd     (MAP_INSTANCE  *p_map_instance,
                           MAP_ITEM      *p_map_item,
                           RTOS_ERR      *p_err);

void       MapItemRemove  (MAP_INSTANCE  *p_map_instance,
                           MAP_ITEM      *p_map_item,
                           RTOS_ERR      *p_err);

void       MapKeyRemove   (MAP_INSTANCE  *p_map_instance,
                           CPU_CHAR      *key,
                           RTOS_ERR      *p_err);

MAP_ITEM  *MapKeyItemGet  (MAP_INSTANCE  *p_map_instance,
                           CPU_CHAR      *key,
                           RTOS_ERR      *p_err);

void      *MapKeyValueGet (MAP_INSTANCE  *p_map_instance,
                           CPU_CHAR      *key,
                           RTOS_ERR      *p_err);


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of map module include.                           */
