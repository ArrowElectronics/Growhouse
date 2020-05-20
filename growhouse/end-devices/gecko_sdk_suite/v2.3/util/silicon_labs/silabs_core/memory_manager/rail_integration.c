/***************************************************************************//**
 * @file rail_integration.c
 * @brief Simple code to link this memory manager with a RAIL application by
 *        implementing the appropriate callbacks.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include <stdint.h>
#include "rail.h"
#include "buffer_pool_allocator.h"

#ifdef  RAIL_STATE_UINT32_BUFFER_SIZE // New symbol created in RAIL 2.0
#error  "rail_integration.c is only compatible with RAIL prior to 2.0"
#endif//RAIL_STATE_UINT32_BUFFER_SIZE

/// Rely on the pool allocator's allocate function to get memory
void *RAILCb_AllocateMemory(uint32_t size)
{
  return memoryAllocate(size);
}

/// Use the pool allocator's free function to return the memory to the pool
void RAILCb_FreeMemory(void *ptr)
{
  memoryFree(ptr);
}

/// Get the memory pointer for this handle and offset into it as requested
void *RAILCb_BeginWriteMemory(void *handle,
                              uint32_t offset,
                              uint32_t *available)
{
  void *memoryPtr = memoryPtrFromHandle(handle);
  if (memoryPtr == NULL) {
    *available = 0;
    return NULL;
  }
  return (void *)(((uint8_t *)memoryPtr) + offset);
}

/// We don't need to track the completion of a memory write so do nothing
void RAILCb_EndWriteMemory(void *handle, uint32_t offset, uint32_t size)
{
}
