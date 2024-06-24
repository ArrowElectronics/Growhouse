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
#include "verify.h"
#include "listener.h"
#include "remote.h"
#include "rpcmem.h"
#include "adsp_listener.h"
#include "listener_buf.h"
#include "shared.h"
#include "uthash.h"
#include "AEEstd.h"

#define LOGL(format, ...) VERIFY_PRINT_INFO(format, ##__VA_ARGS__)
#include "mod_table_imp.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/eventfd.h>

#ifndef VERIFY_PRINT_ERROR
#define VERIFY_PRINT_ERROR
#endif

struct listener
{
	pthread_t thread;
	struct mod_table mt;
	int eventfd;
};

static struct listener gMe = {0};

#define MAX_BUFS 250
struct invoke_bufs
{
	adsp_listener_buffer outbufs[MAX_BUFS];
	adsp_listener_buffer inbufs[MAX_BUFS];
	int inbufLenReqs[MAX_BUFS];
	int outbufLenReqs[MAX_BUFS];
	remote_arg args[2 * MAX_BUFS];
};


__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_remotectl_open)(const char *name, uint32 *handle, char *dlStr, int dlerrorLen, int *dlErr) __QAIC_IMPL_ATTRIBUTE
{
	struct listener *me = &gMe;
	return mod_table_open_imp(&me->mt, name, handle, dlStr, dlerrorLen, dlErr);
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_remotectl_close)(uint32 handle, char *errStr, int errStrLen, int *dlErr) __QAIC_IMPL_ATTRIBUTE
{
	struct listener *me = &gMe;
	mod_table_close_imp(&me->mt, handle, errStr, errStrLen, dlErr);
	return 0;
}

#define RPC_FREEIF(heapid, buf) \
do {\
   if(heapid == -1)  {\
      FREEIF(buf);\
   } else {\
      if(buf) {\
         rpcmem_free(buf);\
         buf = 0;\
      }\
   }\
} while (0)

static __inline void *rpcmem_realloc(int heapid, uint32 flags, void *buf, int oldsize, int size)
{
	if (heapid == -1)
	{
		return REALLOC(buf, size);
	}
	else
	{
		void *bufnew = rpcmem_alloc(heapid, flags, size);
		if (buf && bufnew)
		{
			memmove(bufnew, buf, oldsize);
			rpcmem_free(buf);
		}
		return bufnew;
	}
}


extern int adsp_current_process_exit(void);
static void *listener(void *arg)
{
	struct listener *me = (struct listener *)arg;
	int numOutBufs = 0;
	int nErr = 0;
	adsp_listener_invoke_ctx ctx = 0;
	struct invoke_bufs *bufs = 0;
	boolean bNeedMore;
	int result = -1;
	adsp_listener_remote_handle handle;
	uint32 sc, ii, inBufsAllocated = 0;
	const char *eheap = getenv("ADSP_LISTENER_HEAP_ID");
	int heapid = eheap == 0 ? -1 : atoi(eheap);
	const char *eflags = getenv("ADSP_LISTENER_HEAP_FLAGS");
	uint32 flags = eflags == 0 ? 0 : atoi(eflags);
	eventfd_t event = 0xff;

	VERIFY_EPRINTF("listener using ion heap: %d\n", heapid);

	VERIFY(0 != (bufs = rpcmem_realloc(heapid, flags, 0, 0, sizeof(*bufs))));
	memset(bufs, 0, sizeof(*bufs));

	do
	{
invoke:
		bNeedMore = FALSE;
		sc = 0xffffffff;
		if (result != 0)
		{
			numOutBufs = 0;
		}
		nErr = __QAIC_HEADER(adsp_listener_next_invoke)(
		           ctx, result, bufs->outbufs, numOutBufs, &ctx,
		           &handle, &sc, bufs->inbufs, inBufsAllocated,
		           bufs->inbufLenReqs, MAX_BUFS, bufs->outbufLenReqs, MAX_BUFS);
		if (nErr)
		{
			VERIFY_EPRINTF("listener protocol failure %d\n", nErr);
			VERIFY(0 == (nErr = __QAIC_HEADER(adsp_listener_next_invoke)(
			                        ctx, nErr, 0, 0, &ctx,
			                        &handle, &sc, bufs->inbufs, inBufsAllocated,
			                        bufs->inbufLenReqs, MAX_BUFS, bufs->outbufLenReqs, MAX_BUFS)));
		}

		if (MAX_BUFS < REMOTE_SCALARS_INBUFS(sc) || MAX_BUFS < REMOTE_SCALARS_OUTBUFS(sc))
		{
			result = - 8;
			goto invoke;
		}
		for (ii = 0; ii < REMOTE_SCALARS_INBUFS(sc); ++ii)
		{
			if (bufs->inbufs[ii].dataLen < bufs->inbufLenReqs[ii])
			{
				if (0 != bufs->inbufLenReqs[ii])
				{
					bufs->inbufs[ii].data = rpcmem_realloc(heapid, flags, bufs->inbufs[ii].data,  bufs->inbufs[ii].dataLen, bufs->inbufLenReqs[ii]);
					if (0 == bufs->inbufs[ii].data)
					{
						bufs->inbufs[ii].dataLen = 0;
						result = -8;
						goto invoke;
					}
				}
				bufs->inbufs[ii].dataLen = bufs->inbufLenReqs[ii];
				inBufsAllocated = STD_MAX(inBufsAllocated, ii + 1);
				bNeedMore = TRUE;
			}
			bufs->args[ii].buf.pv = bufs->inbufs[ii].data;
			bufs->args[ii].buf.nLen = bufs->inbufLenReqs[ii];
		}
		for (ii = 0; ii < REMOTE_SCALARS_OUTBUFS(sc); ++ii)
		{
			if (bufs->outbufs[ii].dataLen < bufs->outbufLenReqs[ii])
			{
				if (0 !=  bufs->outbufLenReqs[ii])
				{
					bufs->outbufs[ii].data = rpcmem_realloc(heapid, flags, bufs->outbufs[ii].data, bufs->outbufs[ii].dataLen, bufs->outbufLenReqs[ii]);
					if (0 == bufs->outbufs[ii].data)
					{
						result = -8;
						goto invoke;
					}
				}
				bufs->outbufs[ii].dataLen = bufs->outbufLenReqs[ii];
			}
			bufs->args[ii + REMOTE_SCALARS_INBUFS(sc)].buf.pv = bufs->outbufs[ii].data;
			bufs->args[ii + REMOTE_SCALARS_INBUFS(sc)].buf.nLen = bufs->outbufLenReqs[ii];
		}
		numOutBufs = REMOTE_SCALARS_OUTBUFS(sc);
		if (bNeedMore)
		{
			assert(inBufsAllocated >= REMOTE_SCALARS_INBUFS(sc));
			if (0 != (result = __QAIC_HEADER(adsp_listener_invoke_get_in_bufs)(ctx, bufs->inbufs,
			                   REMOTE_SCALARS_INBUFS(sc))))
			{
				VERIFY_EPRINTF("adsp_listener_invoke_get_in_bufs failed  %d\n", result);
				goto invoke;
			}
		}

		result = mod_table_handle_invoke(&me->mt, handle, sc, bufs->args);
	}
	while (1);
bail:
	for (ii = 0; ii < MAX_BUFS && bufs; ++ii)
	{
		RPC_FREEIF(heapid, bufs->outbufs[ii].data);
		RPC_FREEIF(heapid, bufs->inbufs[ii].data);
	}
	RPC_FREEIF(heapid, bufs);
	if (nErr)
	{
		VERIFY_EPRINTF("listener thread exiting with code %d\n", nErr);
		if (0 != adsp_current_process_exit())
		{
			VERIFY_EPRINTF("listener thread failed to cleanly shutdown.  This is ok durring process exit.\n");
		}
	}
	eventfd_write(me->eventfd, event);
	return (void *)(uintptr_t)nErr;
}

static int listener_start_thread(struct listener *me)
{
	return pthread_create(&me->thread, 0, listener, (void *)me);
}

static void *listener2(void *arg)
{
	struct listener *me = (struct listener *)arg;
	int nErr = 0;
	adsp_listener_invoke_ctx ctx = 0;
	uint8 *outBufs = 0;
	int outBufsLen = 0, outBufsCapacity = 0;
	uint8 *inBufs = 0;
	int inBufsLen = 0, inBufsLenReq = 0;
	int result = -1;
	adsp_listener_remote_handle handle = -1;
	uint32 sc = 0;
	const char *eheap = getenv("ADSP_LISTENER_HEAP_ID");
	int heapid = eheap == 0 ? -1 : atoi(eheap);
	const char *eflags = getenv("ADSP_LISTENER_HEAP_FLAGS");
	uint32 flags = eflags == 0 ? 0 : atoi(eflags);
	remote_arg args[512];
	struct sbuf buf;
	eventfd_t event = 0xff;

	memset(args, 0, sizeof(args));
	VERIFY_IPRINTF("listener using ion heap: %d\n", heapid);

	do
	{
invoke:
		sc = 0xffffffff;
		if (result != 0)
		{
			outBufsLen = 0;
		}
		nErr = __QAIC_HEADER(adsp_listener_next2)(
		           ctx, result, outBufs, outBufsLen,
		           &ctx, &handle, &sc, inBufs, inBufsLen, &inBufsLenReq);
		if (nErr)
		{
			VERIFY_EPRINTF("listener protocol failure %d\n", nErr);
			VERIFY(0 == (nErr = __QAIC_HEADER(adsp_listener_next2)(
			                        ctx, nErr, 0, 0,
			                        &ctx, &handle, &sc, inBufs, inBufsLen,
			                        &inBufsLenReq)));
		}
		if (inBufsLenReq > inBufsLen)
		{
			void *buf;
			int req;
			int oldLen = inBufsLen;
			int size = _SBUF_ALIGN(inBufsLenReq, 8);
			if (0 == (buf = rpcmem_realloc(heapid, flags, inBufs, inBufsLen, size)))
			{
				result = -10;
				VERIFY_EPRINTF("rpcmem_realloc failed");
				goto invoke;
			}
			inBufs = buf;
			inBufsLen = size;
			if (0 != (result = __QAIC_HEADER(adsp_listener_get_in_bufs2)(ctx, oldLen,
			                   inBufs + oldLen,
			                   inBufsLen - oldLen, &req)))
			{
				VERIFY_EPRINTF("adsp_listener_invoke_get_in_bufs2 failed  %d", result);
				goto invoke;
			}
			if (req > inBufsLen)
			{
				result = -13;
				VERIFY_EPRINTF("adsp_listener_invoke_get_in_bufs2 failed  %d", result);
				goto invoke;
			}
		}
		if (REMOTE_SCALARS_INHANDLES(sc) + REMOTE_SCALARS_OUTHANDLES(sc) > 0)
		{
			result = -9;
			goto invoke;
		}

		sbuf_init(&buf, 0, inBufs, inBufsLen);
		unpack_in_bufs(&buf, args,  REMOTE_SCALARS_INBUFS(sc));
		unpack_out_lens(&buf, args + REMOTE_SCALARS_INBUFS(sc), REMOTE_SCALARS_OUTBUFS(sc));

		sbuf_init(&buf, 0, 0, 0);
		pack_out_bufs(&buf, args + REMOTE_SCALARS_INBUFS(sc), REMOTE_SCALARS_OUTBUFS(sc));
		outBufsLen = sbuf_needed(&buf);

		if (outBufsLen > outBufsCapacity)
		{
			void *buf;
			int size = _SBUF_ALIGN(outBufsLen, 8);
			if (0 == (buf = rpcmem_realloc(heapid, flags, outBufs, outBufsCapacity, size)))
			{
				result = -11;
				VERIFY_EPRINTF("listener rpcmem_realloc failed");
				goto invoke;
			}
			outBufs = buf;
			outBufsLen = size;
			outBufsCapacity = size;
		}
		sbuf_init(&buf, 0, outBufs, outBufsLen);
		pack_out_bufs(&buf, args + REMOTE_SCALARS_INBUFS(sc), REMOTE_SCALARS_OUTBUFS(sc));

		result = mod_table_handle_invoke(&me->mt, handle, sc, args);
	}
	while (1);
bail:
	RPC_FREEIF(heapid, outBufs);
	RPC_FREEIF(heapid, inBufs);
	if (nErr)
	{
		VERIFY_EPRINTF("listener thread exiting with code %d\n", nErr);
		if (0 != adsp_current_process_exit())
		{
			VERIFY_EPRINTF("listener thread failed to cleanly shutdown.  This is ok durring process exit.\n");
		}
	}
	eventfd_write(me->eventfd, event);
	return (void *)(uintptr_t)nErr;
}
static int listener_start_thread2(struct listener *me)
{
	return pthread_create(&me->thread, 0, listener2, (void *)me);
}

extern int apps_console_skel_invoke(uint32 _sc, remote_arg *_pra);
extern int apps_remotectl_skel_invoke(uint32 _sc, remote_arg *_pra);
extern int apps_std_skel_invoke(uint32 _sc, remote_arg *_pra);
extern int apps_mem_skel_invoke(uint32 _sc, remote_arg *_pra);

#include "adsp_listener_stub.c"

int listener_init(void)
{
	int nErr = 0;
	struct listener *me = &gMe;
	VERIFY(0 == mod_table_ctor_imp(&me->mt));
	VERIFY(0 == mod_table_register_const_handle_imp(&me->mt, 0, "apps_remotectl", apps_remotectl_skel_invoke));
	VERIFY(0 == mod_table_register_static_imp(&me->mt, "apps_std", apps_std_skel_invoke));
	VERIFY(0 == mod_table_register_static_imp(&me->mt, "apps_mem", apps_mem_skel_invoke));
	VERIFY(-1 != (me->eventfd = eventfd(0, 0)));
	nErr = __QAIC_HEADER(adsp_listener_init2)();
	if (AEE_EUNSUPPORTED == nErr)
	{
		VERIFY_EPRINTF("listener2 initialization error falling back to listener1 %d\n", nErr);
		VERIFY(0 == (nErr = __QAIC_HEADER(adsp_listener_init)()));
		VERIFY(0 == listener_start_thread(me));
	}
	else if (0 == nErr)
	{
		VERIFY_IPRINTF("listener2 initialized\n");
		VERIFY(0 == listener_start_thread2(me));
	}
bail:
	if (nErr)
	{
		VERIFY_EPRINTF("listener initialization error %d\n", nErr);
	}
	return nErr;
}

void listener_deinit(void)
{
	struct listener *me = &gMe;
	VERIFY_IPRINTF("listener joining to exit");
	pthread_join(me->thread, 0);
	VERIFY_IPRINTF("listener joined");
	mod_table_dtor_imp(&me->mt);
	close(me->eventfd);
	me->eventfd = -1;
}

int listener_geteventfd(int *fd)
{
	struct listener *me = &gMe;
	int nErr = 0;
	VERIFY(-1 != me->eventfd);
	*fd = me->eventfd;
bail:
	return nErr;
}

