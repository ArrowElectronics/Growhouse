/*
* Copyright (c) 2017, The Linux Foundation. All rights reserved.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef REMOTE64_H
#define REMOTE64_H

#include "remote.h"

typedef struct
{
	uint64_t pv;
	int64_t nLen;
} remote_buf64;

typedef union
{
	remote_buf64	buf;
	remote_handle h;
} remote_arg64;

#ifndef __QAIC_REMOTE
#define __QAIC_REMOTE(ff) ff
#endif //__QAIC_REMOTE

#ifndef __QAIC_REMOTE_EXPORT
#ifdef _WIN32
#define __QAIC_REMOTE_EXPORT __declspec(dllexport)
#else //_WIN32
#define __QAIC_REMOTE_EXPORT
#endif //_WIN32
#endif //__QAIC_REMOTE_EXPORT

#ifndef __QAIC_REMOTE_ATTRIBUTE
#define __QAIC_REMOTE_ATTRIBUTE
#endif

/* map memory to the remote domain
 *
 * @param fd, fd associated with this memory
 * @param flags, flags to be used for the mapping
 * @param vaddrin, input address
 * @param size, size of buffer
 * @param vaddrout, output address
 * @retval, 0 on success
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_mmap64)(int fd, uint32_t flags, uintptr_t vaddrin, int64_t size, uintptr_t *vaddrout) __QAIC_REMOTE_ATTRIBUTE;

/* unmap memory from the remote domain
 *
 * @param vaddrout, remote address mapped
 * @param size, size to unmap.  Unmapping a range partially may  not be supported.
 * @retval, 0 on success, may fail if memory is still mapped
 */
__QAIC_REMOTE_EXPORT int __QAIC_REMOTE(remote_munmap64)(uintptr_t vaddrout, int64_t size) __QAIC_REMOTE_ATTRIBUTE;

#endif // REMOTE64_H
