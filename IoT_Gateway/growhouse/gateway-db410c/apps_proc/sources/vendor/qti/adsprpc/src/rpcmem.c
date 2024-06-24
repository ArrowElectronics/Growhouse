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
#include "rpcmem.h"
#include "verify.h"
#include "AEEQList.h"
#include "AEEstd.h"

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define PAGE_SIZE 4096
#define PAGE_MASK ~((uintptr_t)PAGE_SIZE - 1)

static QList rpclst;
static pthread_mutex_t rpcmt;

struct rpc_info
{
	QNode qn;
	void *buf;
	void *aligned_buf;
	int size;
};

void rpcmem_init()
{
	QList_Ctor(&rpclst);
	pthread_mutex_init(&rpcmt, 0);
}

void rpcmem_deinit()
{
	pthread_mutex_destroy(&rpcmt);
}

int rpcmem_to_fd(void *po)
{
	return 1;
}

void *rpcmem_alloc(int heapid, uint32 flags, int size)
{
	struct rpc_info *rinfo;
	int nErr = 0;
	(void)heapid;
	(void)flags;

	VERIFY(0 != (rinfo = calloc(1, sizeof(*rinfo))));
	VERIFY(0 != (rinfo->buf = malloc(size + PAGE_SIZE)));
	rinfo->aligned_buf = (void *)(((uintptr_t)rinfo->buf + PAGE_SIZE) & PAGE_MASK);
	rinfo->size = size;
	pthread_mutex_lock(&rpcmt);
	QList_AppendNode(&rpclst, &rinfo->qn);
	pthread_mutex_unlock(&rpcmt);
	return rinfo->aligned_buf;
bail:
	if (nErr)
	{
		if (rinfo)
		{
			if (rinfo->buf)
			{
				free(rinfo->buf);
			}
			free(rinfo);
		}
	}
	return 0;
}

void rpcmem_free(void *po)
{
	struct rpc_info *rinfo, *rfree = 0;
	QNode *pn, *pnn;

	pthread_mutex_lock(&rpcmt);
	QLIST_NEXTSAFE_FOR_ALL(&rpclst, pn, pnn)
	{
		rinfo = STD_RECOVER_REC(struct rpc_info, qn, pn);
		if (rinfo->aligned_buf == po)
		{
			rfree = rinfo;
			QNode_Dequeue(&rinfo->qn);
			break;
		}
	}
	pthread_mutex_unlock(&rpcmt);
	if (rfree)
	{
		free(rfree->buf);
		free(rfree);
	}
}
