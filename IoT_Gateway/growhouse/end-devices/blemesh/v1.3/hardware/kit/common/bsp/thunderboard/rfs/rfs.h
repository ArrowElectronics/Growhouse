/***************************************************************************//**
 * @file rfs.h
 * @brief ROM File System Drive
 * @version 5.5.0
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef RFS_H
#define RFS_H

#include <stdint.h>

/***************************************************************************//**
 * @cond DOXYGEN_INCLUDE_RFS
 * @addtogroup RFS
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    RFS file handle structure
 ******************************************************************************/
typedef struct __RFS_FileHandle {
  uint16_t fileIndex;         /**< Index of the file within the file system */
  uint32_t currentIndex;      /**< Current index position within the file   */
} RFS_FileHandle;

#define RFS_MAX_FILE_NAME_SIZE 256  /**< Maximum length of the file name string  */

#define RFS_SEEK_SET 0x80000000     /**< The beginnig of the file                */
#define RFS_SEEK_CUR 0x80000001     /**< The current index position of the file  */
#define RFS_SEEK_END 0x80000002     /**< The end of the file                     */

uint32_t RFS_getFileCount(void);
uint8_t *RFS_getFileNameByIndex(uint32_t index);
int32_t RFS_getFileLengthByIndex(uint32_t index);
int16_t RFS_getFileIndex(uint8_t name[]);

int32_t RFS_fileOpen(RFS_FileHandle *fileHandle, uint8_t name[]);
int32_t RFS_fileSeek(RFS_FileHandle *fileHandle, int32_t offset, uint32_t whence);
int32_t RFS_fileRead(uint8_t *buf, uint32_t size, uint32_t nmemb, RFS_FileHandle *fileHandle);
int32_t RFS_getFileLength(RFS_FileHandle *fileHandle);
uint8_t *RFS_getFileName(RFS_FileHandle *fileHandle);
uint8_t *RFS_fileGetRawData(RFS_FileHandle *fileHandle);

/** @} */
/** @endcond {DOXYGEN_INCLUDE_RFS} */

#endif // RFS_H
