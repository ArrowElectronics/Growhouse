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
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include "apps_mem.h"
#include "remote64.h"
#include "rpcmem.h"
#include "verify.h"
#include "rpcmem.h"
#include "AEEQList.h"
#include "AEEstd.h"

#define ADSP_MMAP_HEAP_ADDR 4

static QList memlst;
static pthread_mutex_t memmt;

struct mem_info
{
	QNode qn;
	uint64 vapps;
	uint64 vadsp;
	int32 size;
};

/* These should be called in some static constructor of the .so that
uses rpcmem.

I moved them into fastrpc_apps_user.c because there is no gurantee in
the order of when constructors are called. */

void apps_mem_init(void)
{
	QList_Ctor(&memlst);
	pthread_mutex_init(&memmt, 0);
}

void apps_mem_deinit(void)
{
	QNode *pn;
	while ((pn = QList_PopZ(&memlst)) != NULL)
	{
		struct mem_info *mfree = STD_RECOVER_REC(struct mem_info, qn, pn);
		rpcmem_free((void *)(uintptr_t)mfree->vapps);
		free(mfree);
	}
	pthread_mutex_destroy(&memmt);
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_mem_request_map64)(int heapid, uint32 lflags, uint32 rflags, uint64 vin, int64 len, uint64 *vapps, uint64 *vadsp) __QAIC_IMPL_ATTRIBUTE
{
	struct mem_info *minfo = 0;
	int nErr = 0;
	void *buf = 0;
	uintptr_t pbuf;
	int fd;
	(void)vin;
	VERIFY(0 != (minfo = malloc(sizeof(*minfo))));
	QNode_CtorZ(&minfo->qn);
	*vadsp = 0;
	if (rflags == ADSP_MMAP_HEAP_ADDR)
	{
		VERIFY(0 == remote_mmap64(-1, rflags, 0, len, (uintptr_t *)vadsp));
		*vapps = 0;
		minfo->vapps = 0;
	}
	else
	{
		lflags |= RPCMEM_HEAP_NOREG;
		VERIFY(0 != (buf = rpcmem_alloc(heapid, lflags, len)));
		fd = rpcmem_to_fd(buf);
		VERIFY(fd > 0);
		VERIFY(0 == remote_mmap64(fd, rflags, (uintptr_t)buf, len, (uintptr_t *)vadsp));
		pbuf = (uintptr_t)buf;
		*vapps = (uint64)pbuf;
		minfo->vapps = *vapps;
	}
	minfo->vadsp = *vadsp;
	minfo->size = len;
	pthread_mutex_lock(&memmt);
	QList_AppendNode(&memlst, &minfo->qn);
	pthread_mutex_unlock(&memmt);
bail:
	if (nErr)
	{
		if (buf)
		{
			rpcmem_free(buf);
		}
		if (minfo)
		{
			free(minfo);
		}
	}
	return nErr;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_mem_request_map)(int heapid, uint32 lflags, uint32 rflags, uint32 vin, int32 len, uint32 *vapps, uint32 *vadsp) __QAIC_IMPL_ATTRIBUTE
{
	uint64 vin1, vapps1, vadsp1;
	int64 len1;
	int nErr;
	vin1 = (uint64)vin;
	len1 = (int64)len;
	nErr = apps_mem_request_map64(heapid, lflags, rflags, vin1, len1, &vapps1, &vadsp1);
	*vapps = (uint32)vapps1;
	*vadsp = (uint32)vadsp1;
	return nErr;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_mem_request_unmap64)(uint64 vadsp, int64 len) __QAIC_IMPL_ATTRIBUTE
{
	int nErr = 0;
	struct mem_info * minfo, *mfree = 0;
	QNode * pn, *pnn;
	VERIFY(0 == remote_munmap64((uintptr_t)vadsp, len));
	pthread_mutex_lock(&memmt);
	QLIST_NEXTSAFE_FOR_ALL(&memlst, pn, pnn)
	{
		minfo = STD_RECOVER_REC(struct mem_info, qn, pn);
		if (minfo->vadsp == vadsp)
		{
			mfree = minfo;
			QNode_Dequeue(&minfo->qn);
			break;
		}
	}
	pthread_mutex_unlock(&memmt);
	VERIFY(mfree);
	rpcmem_free((void *)(uintptr_t)mfree->vapps);
	free(mfree);
bail:
	return nErr;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_mem_request_unmap)(uint32 vadsp, int32 len) __QAIC_IMPL_ATTRIBUTE
{
	uint64 vadsp1 = (uint64)vadsp;
	int64 len1 = (int64)len;
	return apps_mem_request_unmap64(vadsp1, len1);
}

