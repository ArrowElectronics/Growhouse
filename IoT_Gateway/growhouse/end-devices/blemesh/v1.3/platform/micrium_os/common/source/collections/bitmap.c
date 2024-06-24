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
*                                               BITMAPS
*
* File : bitmap.c
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                               INCLUDE FILES
*********************************************************************************************************
*********************************************************************************************************
*/

#include  <common/source/collections/bitmap_priv.h>
#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_def.h>


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           GLOBAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/**
*********************************************************************************************************
*                                               Bitmap_BitIsSet()
*
* @brief    Determines if specified bit of bitmap is set.
*
* @param    p_bitmap    Pointer to bitmap.
*
* @param    offset_bit  Offset of bit in bitmap to test.
*
* @return   DEF_YES, if bit is set;
*           DEF_NO , otherwise.
*********************************************************************************************************
*/

CPU_BOOLEAN  Bitmap_BitIsSet (CPU_INT08U  *p_bitmap,
                              CPU_SIZE_T   offset_bit)
{
    CPU_SIZE_T   offset_octet;
    CPU_DATA     offset_bit_in_octet;
    CPU_INT08U   bit_mask;
    CPU_BOOLEAN  bit_set;


    offset_octet        = offset_bit >> DEF_OCTET_TO_BIT_SHIFT;
    offset_bit_in_octet = offset_bit &  DEF_OCTET_TO_BIT_MASK;

    bit_mask = DEF_BIT(offset_bit_in_octet);
    bit_set  = DEF_BIT_IS_SET(p_bitmap[offset_octet], bit_mask);

    return (bit_set);
}


/**
*********************************************************************************************************
*                                               Bitmap_BitSet()
*
* @brief    Set specified bit in bitmap.
*
* @param    p_bitmap    Pointer to bitmap.
*
* @param    offset_bit  Offset of bit in bitmap to test.
*********************************************************************************************************
*/

void  Bitmap_BitSet (CPU_INT08U  *p_bitmap,
                     CPU_SIZE_T   offset_bit)
{
    CPU_SIZE_T  offset_octet;
    CPU_DATA    offset_bit_in_octet;
    CPU_INT08U  bit_mask;


    offset_octet        = offset_bit >> DEF_OCTET_TO_BIT_SHIFT;
    offset_bit_in_octet = offset_bit &  DEF_OCTET_TO_BIT_MASK;

    bit_mask = DEF_BIT(offset_bit_in_octet);
    DEF_BIT_SET(p_bitmap[offset_octet], bit_mask);
}


/**
*********************************************************************************************************
*                                               Bitmap_BitClr()
*
* @brief    Clear specified bit in bitmap.
*
* @param    p_bitmap    Pointer to bitmap.
*
* @param    offset_bit  Offset of bit in bitmap to test.
*********************************************************************************************************
*/

void  Bitmap_BitClr (CPU_INT08U  *p_bitmap,
                     CPU_SIZE_T   offset_bit)
{
    CPU_SIZE_T  offset_octet;
    CPU_DATA    offset_bit_in_octet;
    CPU_INT08U  bit_mask;


    offset_octet        = offset_bit >> DEF_OCTET_TO_BIT_SHIFT;
    offset_bit_in_octet = offset_bit &  DEF_OCTET_TO_BIT_MASK;

    bit_mask = DEF_BIT(offset_bit_in_octet);
    DEF_BIT_CLR_08(p_bitmap[offset_octet], bit_mask);
}
