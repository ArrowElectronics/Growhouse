/*
 * Copyright (c) 2012-2013, 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#if !defined __WLAN_NV_TYPES_H
#define __WLAN_NV_TYPES_H

#include <linux/string.h>
#include <stdint.h>

typedef enum
{
   /// Request succeeded!
   VOS_STATUS_SUCCESS,

   /// Request failed because system resources (other than memory) to
   /// fulfill request are not available.
   VOS_STATUS_E_RESOURCES,

   /// Request failed because not enough memory is available to
   /// fulfill the request.
   VOS_STATUS_E_NOMEM,

   /// Request could not be fulfilled at this time.  Try again later.
   VOS_STATUS_E_AGAIN,

   /// Request failed because there of an invalid request.  This is
   /// typically the result of invalid parameters on the request.
   VOS_STATUS_E_INVAL,

   /// Request failed because handling the request would cause a
   /// system fault.  This error is typically returned when an
   /// invalid pointer to memory is detected.
   VOS_STATUS_E_FAULT,

   /// Request refused becayse a request is already in progress and
   /// another cannot be handled currently.
   VOS_STATUS_E_ALREADY,

   /// Request failed because the message (type) is bad, invalid, or
   /// not properly formatted.
   VOS_STATUS_E_BADMSG,

   /// Request failed because device or resource is busy.
   VOS_STATUS_E_BUSY,

   /// Request did not complete because it was canceled.
   VOS_STATUS_E_CANCELED,

   /// Request did not complete because it was aborted.
   VOS_STATUS_E_ABORTED,

   /// Request failed because the request is valid, though not supported
   /// by the entity processing the request.
   VOS_STATUS_E_NOSUPPORT,

   /// Operation is not permitted.
   VOS_STATUS_E_PERM,

   /// Request failed because of an empty condition
   VOS_STATUS_E_EMPTY,

   /// Existance failure.  Operation could not be completed because
   /// something exists or does not exist.
   VOS_STATUS_E_EXISTS,

   /// Operation timed out
   VOS_STATUS_E_TIMEOUT,

   /// Request failed for some unknown reason.  Note don't use this
   /// status unless nothing else applies
   VOS_STATUS_E_FAILURE

} VOS_STATUS;

/// Macro to determine if a VOS_STATUS type is success.  All callers
/// wanting to interpret VOS_STATUS should use this macro to check
/// for success to protect against the VOS_STATUS definitions
/// changing.
///
/// Use like this...
///
///  if ( VOS_STATUS_SUCCESS( vosStatus ) ) ...
///
#define VOS_IS_STATUS_SUCCESS( status ) ( VOS_STATUS_SUCCESS == ( status ) )

typedef uint8_t tANI_U8;
typedef uint16_t tANI_U16;
typedef uint32_t tANI_U32;
typedef int8_t tANI_S8;
typedef int16_t tANI_S16;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef int8_t int8;
typedef int16_t int16;

typedef char tANI_BOOL;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define nul '\0'

#endif
