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
#ifndef FASTRPC_INTERNAL_H
#define FASTRPC_INTERNAL_H

#include "remote64.h"
#include "verify.h"
#include "AEEstd.h"

#define FASTRPC_IOCTL_INVOKE        _IOWR('R', 1, struct fastrpc_ioctl_invoke)
#define FASTRPC_IOCTL_MMAP          _IOWR('R', 2, struct fastrpc_ioctl_mmap)
#define FASTRPC_IOCTL_MUNMAP        _IOWR('R', 3, struct fastrpc_ioctl_munmap)
#define FASTRPC_IOCTL_INVOKE_FD     _IOWR('R', 4, struct fastrpc_ioctl_invoke_fd)
#define FASTRPC_IOCTL_SETMODE       _IOWR('R', 5, uint32)
#define FASTRPC_IOCTL_INIT          _IOWR('R', 6, struct fastrpc_ioctl_init)


#define DEVICE_NAME "adsprpc-smd"

#if !(defined __qdsp6__) && !(defined __hexagon__)
static __inline uint32 Q6_R_cl0_R(uint32 num)
{
	int ii;
	for (ii = 31; ii >= 0; --ii)
	{
		if (num & (1 << ii))
		{
			return 31 - ii;
		}
	}
	return 0;
}
#else
#include "hexagon_protos.h"
#include <types.h>
#endif

struct fastrpc_ioctl_invoke
{
	remote_handle handle;
	uint32_t sc;
	remote_arg *pra;
};

struct fastrpc_ioctl_invoke_fd
{
	struct fastrpc_ioctl_invoke inv;
	int *fds;
};

#define FASTRPC_INIT_ATTACH      0
#define FASTRPC_INIT_CREATE      1

struct fastrpc_ioctl_init
{
	uint32_t flags;
	byte *file;
	int filelen;
	int filefd;
	void *mem;
	int memlen;
	int memfd;
};

struct fastrpc_ioctl_munmap
{
	uintptr_t vaddrout; /* optional virtual address, if non zero, dsp will use vaaddrin */
	ssize_t size;       /* size */
};

struct fastrpc_ioctl_mmap
{
	int fd;             /* ion handle */
	uint32 flags;       /* flags to map with */
	uintptr_t vaddrin;  /* virtual address */
	ssize_t size;       /* size */
	uintptr_t vaddrout; /* dsps virtual address */
};

#define FASTRPC_SMD_GUID "fastrpcsmd-apps-dsp"

struct smq_null_invoke32
{
	uint32_t ctx;         //! invoke caller context
	remote_handle handle; //! handle to invoke
	uint32_t sc;          //! scalars structure describing the rest of the data
};

struct smq_null_invoke
{
	uint64_t ctx;         //! invoke caller context
	remote_handle handle; //! handle to invoke
	uint32_t sc;          //! scalars structure describing the rest of the data
};

typedef uint32_t smq_invoke_buf_phy_addr;

struct smq_phy_page
{
	uint64_t addr; //! physical address
	int64_t size;  //! size
};

struct smq_phy_page32
{
	uint32_t addr; //! physical address
	uint32_t size; //! size
};

struct smq_invoke_buf
{
	int num;
	int pgidx;
};

struct smq_invoke32
{
	struct smq_null_invoke32 header;
	struct smq_phy_page32 page;   //! remote arg and list of pages address
};

struct smq_invoke
{
	struct smq_null_invoke header;
	struct smq_phy_page page;     //! remote arg and list of pages address
};

struct smq_msg32
{
	uint32_t pid;
	uint32_t tid;
	struct smq_invoke32 invoke;
};

struct smq_msg
{
	uint32_t pid;
	uint32_t tid;
	struct smq_invoke invoke;
};

struct smq_msg_u
{
	union
	{
		struct smq_msg32 msg32;
		struct smq_msg msg64;
	} msg;
	int size;
};

struct smq_invoke_rsp32
{
	uint32_t ctx;                 //! invoke caller context
	int nRetVal;                  //! invoke return value
};

struct smq_invoke_rsp
{
	uint64_t ctx;                 //! invoke caller context
	int nRetVal;                  //! invoke return value
};

struct smq_invoke_rsp_u
{
	union
	{
		struct smq_invoke_rsp32 rsp32;
		struct smq_invoke_rsp rsp64;
	} rsp;
	int size;
};

static __inline void to_smq_msg(uint32 mode, struct smq_msg_u *msg, struct smq_msg *msg64)
{
	if (0 == mode)
	{
		msg64->pid = msg->msg.msg32.pid;
		msg64->tid = msg->msg.msg32.tid;
		msg64->invoke.header.ctx = msg->msg.msg32.invoke.header.ctx;
		msg64->invoke.header.handle = msg->msg.msg32.invoke.header.handle;
		msg64->invoke.header.sc = msg->msg.msg32.invoke.header.sc;
		msg64->invoke.page.addr = msg->msg.msg32.invoke.page.addr;
		msg64->invoke.page.size = msg->msg.msg32.invoke.page.size;
	}
	else
	{
		std_memmove(msg64, &msg->msg.msg64, sizeof(*msg64));
	}
}

static __inline void to_smq_invoke_rsp(uint32 mode, uint64 ctx, int nRetVal, struct smq_invoke_rsp_u *rsp)
{
	if (0 == mode)
	{
		rsp->rsp.rsp32.ctx = (uint32)ctx;
		rsp->rsp.rsp32.nRetVal = nRetVal;
		rsp->size = sizeof(rsp->rsp.rsp32);
	}
	else
	{
		rsp->rsp.rsp64.ctx = ctx;
		rsp->rsp.rsp64.nRetVal = nRetVal;
		rsp->size = sizeof(rsp->rsp.rsp64);
	}
}

static __inline struct smq_invoke_buf *to_smq_invoke_buf_start(uint32 mode, void *virt, uint32 sc)
{
	struct smq_invoke_buf *buf;
	int len = REMOTE_SCALARS_LENGTH(sc);
	if (0 == mode)
	{
		remote_arg *pra = (remote_arg *)virt;
		buf = (struct smq_invoke_buf *)(&pra[len]);
	}
	else
	{
		remote_arg64 *pra = (remote_arg64 *)virt;
		buf = (struct smq_invoke_buf *)(&pra[len]);
	}
	return buf;
}

static __inline struct smq_invoke_buf *smq_invoke_buf_start(uint32 mode, void *pv, uint32 sc)
{
	int len = REMOTE_SCALARS_LENGTH(sc);
	if (0 == mode)
	{
		remote_arg *pra = (remote_arg *)pv;
		return (struct smq_invoke_buf *)(&pra[len]);
	}
	else
	{
		remote_arg64 *pra = (remote_arg64 *)pv;
		return (struct smq_invoke_buf *)(&pra[len]);
	}
}

static __inline struct smq_phy_page *smq_phy_page_start(uint32 sc, struct smq_invoke_buf *buf)
{
	int nTotal =  REMOTE_SCALARS_INBUFS(sc) + REMOTE_SCALARS_OUTBUFS(sc);
	return (struct smq_phy_page *)(&buf[nTotal]);
}

//! size of the out of band data
static __inline int smq_data_size(uint32 mode, uint32 sc, int nPages)
{
	struct smq_invoke_buf *buf = smq_invoke_buf_start(mode, 0, sc);
	struct smq_phy_page *page = smq_phy_page_start(sc, buf);
	return (int)(uintptr_t)(&(page[nPages]));
}

static __inline void to_smq_data(uint32 mode, uint32 sc, int nPages, void *pv, remote_arg64 *rpra)
{
	if (0 == mode)
	{
		struct smq_phy_page *page;
		struct smq_phy_page32 *page32;
		remote_arg *pra = (remote_arg *)pv;
		int ii, len;
		len = REMOTE_SCALARS_LENGTH(sc);
		for (ii = 0; ii < len; ++ii)
		{
			rpra[ii].buf.pv = (uint64)(uintptr_t)pra[ii].buf.pv;
			rpra[ii].buf.nLen = pra[ii].buf.nLen;
		}
		len = REMOTE_SCALARS_INBUFS(sc) + REMOTE_SCALARS_OUTBUFS(sc);
		std_memmove(&rpra[ii], &pra[ii], len * sizeof(struct smq_invoke_buf));
		page = (struct smq_phy_page *)((struct smq_invoke_buf *)&rpra[ii] + len);
		page32 = (struct smq_phy_page32 *)((struct smq_invoke_buf *)&pra[ii] + len);
		for (ii = 0; ii < nPages; ++ii)
		{
			page[ii].addr = page32[ii].addr;
			page[ii].size = page32[ii].size;
		}
	}
	else
	{
		std_memmove(rpra, pv, smq_data_size(mode, sc, nPages));
	}
}

#endif // FASTRPC_INTERNAL_H
