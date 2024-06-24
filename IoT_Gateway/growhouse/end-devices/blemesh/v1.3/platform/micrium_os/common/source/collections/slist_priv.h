/*
*********************************************************************************************************
*                                               Micrium OS
*                                                   Common
*
*                               (c) Copyright 2015; Silicon Laboratories Inc.
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
*                                       SINGLY-LINKED LISTS (SList)
*
* File : slist_priv.h
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*********************************************************************************************************
*/

#ifndef  _SLIST_PRIV_H_
#define  _SLIST_PRIV_H_


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*********************************************************************************************************
*/

typedef  struct  slist_member  SLIST_MEMBER;

struct  slist_member {
    SLIST_MEMBER  *p_next;
};


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MACROS
*********************************************************************************************************
*********************************************************************************************************
*/


#define  OFFSET_OF(type, member)                      ((CPU_SIZE_T)&(((type *)0)->member))

#define  SLIST_FOR_EACH(list_head, iterator)          for ((iterator) = (list_head); (iterator) != DEF_NULL; (iterator) = (iterator)->p_next)

#define  SLIST_FOR_EACH_ENTRY(list_head, entry, type, member) for (  (entry) = SLIST_ENTRY(list_head, type, member); \
                                                                   &((entry)->member) != DEF_NULL; \
                                                                     (entry) = SLIST_ENTRY((entry)->member.p_next, type, member))

#define  SLIST_ENTRY                                  CONTAINER_OF


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*********************************************************************************************************
*/


void           SList_Init     (SLIST_MEMBER  **p_head_ptr);

void           SList_Push     (SLIST_MEMBER  **p_head_ptr,
                               SLIST_MEMBER   *p_item);

void           SList_PushBack (SLIST_MEMBER  **p_head_ptr,
                               SLIST_MEMBER   *p_item);

SLIST_MEMBER  *SList_Pop      (SLIST_MEMBER  **p_head_ptr);

void           SList_Add      (SLIST_MEMBER   *p_item,
                               SLIST_MEMBER   *p_pos);

void           SList_Rem      (SLIST_MEMBER  **p_head_ptr,
                               SLIST_MEMBER   *p_item);

void           SList_Sort     (SLIST_MEMBER  **p_head_ptr,
                               CPU_BOOLEAN   (*cmp_fnct)(SLIST_MEMBER  *p_item_l, SLIST_MEMBER  *p_item_r));


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*********************************************************************************************************
*/

#endif                                                          /* End of private slist module include.                 */
