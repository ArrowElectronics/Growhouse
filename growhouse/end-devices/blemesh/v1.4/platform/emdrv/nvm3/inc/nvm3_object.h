/***************************************************************************//**
 * @file
 * @brief NVM3 object definition
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef NVM3_OBJECT_H
#define NVM3_OBJECT_H

#include "nvm3_hal.h"
#include "nvm3_config.h"
#include "nvm3.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define NVM3_OBJ_SMALL_MAX_SIZE          120U                              // Small object is limited to 120 bytes
#define NVM3_OBJ_PTR_INVALID             ((nvm3_ObjPtr_t)(-1L))
#define NVM3_OBJ_HDR_PTR_INVALID         ((nvm3_ObjHdrPtr_t)(-1L))
#define NVM3_OBJ_HEADER_SIZE_LARGE       (sizeof(nvm3_ObjHdrLarge_t))
#define NVM3_OBJ_HEADER_SIZE_WLARGE      (sizeof(nvm3_ObjHdrLarge_t) / sizeof(uint32_t))
#define NVM3_OBJ_HEADER_SIZE_SMALL       (sizeof(nvm3_ObjHdrSmall_t))
#define NVM3_OBJ_HEADER_SIZE_WSMALL      (sizeof(nvm3_ObjHdrSmall_t) / sizeof(uint32_t))
#define NVM3_OBJ_HEADER_SIZE_COUNTER     (NVM3_OBJ_HEADER_SIZE_LARGE)

typedef struct nvm3_ObjHeaderSmall {
  uint32_t oh1;
} nvm3_ObjHdrSmall_t;

typedef struct nvm3_ObjHeaderLarge {
  uint32_t oh1;
  uint32_t oh2;
} nvm3_ObjHdrLarge_t;

typedef nvm3_ObjHdrSmall_t *nvm3_ObjHdrSmallPtr_t;
typedef nvm3_ObjHdrLarge_t *nvm3_ObjHdrLargePtr_t;

typedef enum {
  objTypeDataLarge = 0,
  objTypeCounterLarge = 1,
  objTypeCounterSmall = 2,
  objTypeDeleted = 3,
  objTypeRes_1 = 4,
  objTypeRes_2 = 5,
  objTypeRes_3 = 6,
  objTypeDataSmall = 7,
} nvm3_ObjType_t;

typedef enum {
  objGroupUnknown,
  objGroupData,
  objGroupCounter,
  objGroupDeleted,
} nvm3_ObjGroup_t;

typedef enum {
  fragTypeNone = 0,
  fragTypeFirst = 1,
  fragTypeNext = 2,
  fragTypeLast = 3,
} nvm3_ObjFragType_t;

typedef nvm3_Obj_t *nvm3_ObjPtr_t;

void nvm3_objInit(nvm3_ObjPtr_t obj, nvm3_ObjPtr_t objAdr);
size_t nvm3_objHdrInit(nvm3_ObjHdrLargePtr_t oh, nvm3_ObjectKey_t key, nvm3_ObjType_t objType, size_t len, bool isLarge, nvm3_ObjFragType_t fragTyp);
size_t nvm3_objHdrLen(bool isLarge);
bool nvm3_objHdrValidateSmall(nvm3_ObjHdrSmallPtr_t objHdrSmall);
bool nvm3_objHdrValidateLarge(nvm3_ObjHdrLargePtr_t objHdrLarge);
bool nvm3_objHdrGetErased(nvm3_ObjHdrSmallPtr_t objHdrSmall);
nvm3_ObjFragType_t nvm3_objHdrGetFragTyp(nvm3_ObjHdrSmallPtr_t objHdrSmall);
nvm3_ObjectKey_t nvm3_objHdrGetKey(nvm3_ObjHdrSmallPtr_t objHdrSmall);
bool nvm3_objHdrGetHdrIsLarge(nvm3_ObjHdrSmallPtr_t objHdrSmall);
size_t nvm3_objHdrGetHdrLen(nvm3_ObjHdrSmallPtr_t objHdrSmall);
size_t nvm3_objHdrGetDatLen(nvm3_ObjHdrLargePtr_t objHdrLarge);
nvm3_ObjType_t nvm3_objHdrGetType(nvm3_ObjHdrSmallPtr_t objHdrSmall);
nvm3_ObjType_t nvm3_objGroupToType(nvm3_ObjGroup_t objGroup, bool hdrIsLarge);
nvm3_ObjGroup_t nvm3_objTypeToGroup(nvm3_ObjType_t objType);
/// @endcond

#ifdef __cplusplus
}
#endif

#endif /* NVM3_OBJECT_H */
