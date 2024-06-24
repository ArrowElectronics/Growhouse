/***************************************************************************//**
 * @file sl_malloc.c
 * @brief This is a simple wrapper for the stdlib memory management functions
 *   like malloc, calloc, realloc and free in order to make them
 *   thread safe.
 *******************************************************************************
 * # License
 * <b>Copyright 2017 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Silicon Labs will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#include "sl_malloc.h"
#include "em_core.h"
#include <stdlib.h>

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib malloc in a critical section.
 *
 * @param[in] size
 *   number of bytes to allocate.
 *
 * @return
 *   Either a pointer to the allocated space or a null pointer.
 ******************************************************************************/
void * sl_malloc(size_t size)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  void * ptr = malloc(size);
  CORE_EXIT_CRITICAL();
  return ptr;
}

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib calloc in a critical section.
 *
 * @param[in] nmemb
 *   number of element to allocate space for.
 *
 * @param[in] size
 *   size of each element.
 *
 * @return
 *   Either a pointer to the allocated space or a null pointer.
 ******************************************************************************/
void * sl_calloc(size_t nmemb, size_t size)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  void * ptr = calloc(nmemb, size);
  CORE_EXIT_CRITICAL();
  return ptr;
}

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib realloc in a critical section.
 *
 * @param[in] ptr
 *   ptr is a pointer to a memory that has previously been allocated. If ptr
 *   is a null pointer then this function behaves like sl_malloc.
 *
 * @param[in] size
 *   number of bytes to allocate.
 *
 * @return
 *   Either a pointer to the allocated space or a null pointer.
 ******************************************************************************/
void * sl_realloc(void * ptr, size_t size)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  void * p = realloc(ptr, size);
  CORE_EXIT_CRITICAL();
  return p;
}

/***************************************************************************//**
 * @brief
 *   Wrap a call to stdlib free in a critical section.
 *
 * @param[in] ptr
 *   ptr is a pointer to a memory that has previously been allocated.
 ******************************************************************************/
void sl_free(void * ptr)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  free(ptr);
  CORE_EXIT_CRITICAL();
}
