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
#ifndef LISTENER_BUF_H
#define LISTENER_BUF_H

#include "sbuf.h"
#include "remote.h"
#include "verify.h"

static __inline void pack_in_bufs(struct sbuf *buf, remote_arg *pra, int nBufs)
{
	int ii;
	uint32_t len;
	C_ASSERT(sizeof(len) == 4);
	for (ii = 0; ii < nBufs; ++ii)
	{
		len = (uint32_t)pra[ii].buf.nLen;
		sbuf_write(buf, (uint8 *)&len, 4);
		if (len)
		{
			sbuf_align(buf, 8);
			sbuf_write(buf, pra[ii].buf.pv, len);
		}
	}
}

void pack_out_lens(struct sbuf *buf, remote_arg *pra, int nBufs)
{
	int ii;
	uint32_t len;
	C_ASSERT(sizeof(len) == 4);
	for (ii = 0; ii < nBufs; ++ii)
	{
		len = (uint32_t)pra[ii].buf.nLen;
		sbuf_write(buf, (uint8 *)&len, 4);
	}
}

void unpack_in_bufs(struct sbuf *buf, remote_arg *pra, int nBufs)
{
	int ii;
	uint32_t len;
	C_ASSERT(sizeof(len) == 4);
	for (ii = 0; ii < nBufs; ++ii)
	{
		sbuf_read(buf, (uint8 *)&len, 4);
		pra[ii].buf.nLen = len;
		if (pra[ii].buf.nLen)
		{
			sbuf_align(buf, 8);
			if ((int)pra[ii].buf.nLen <= sbuf_left(buf))
			{
				pra[ii].buf.pv = sbuf_head(buf);
			}
			sbuf_advance(buf, pra[ii].buf.nLen);
		}
	}
}

void unpack_out_lens(struct sbuf *buf, remote_arg *pra, int nBufs)
{
	int ii;
	uint32_t len;
	C_ASSERT(sizeof(len) == 4);
	for (ii = 0; ii < nBufs; ++ii)
	{
		sbuf_read(buf, (uint8 *)&len, 4);
		pra[ii].buf.nLen = len;
	}
}

//map out buffers on the hlos side to the remote_arg array
//dst is the space required for buffers we coun't map from the adsp
void pack_out_bufs(struct sbuf *buf, remote_arg *pra, int nBufs)
{
	int ii;
	uint32_t len;
	C_ASSERT(sizeof(len) == 4);
	for (ii = 0; ii < nBufs; ++ii)
	{
		len = (uint32_t)pra[ii].buf.nLen;
		sbuf_write(buf, (uint8 *)&len, 4);
		if (pra[ii].buf.nLen)
		{
			sbuf_align(buf, 8);
			if ((int)pra[ii].buf.nLen <= sbuf_left(buf))
			{
				pra[ii].buf.pv = sbuf_head(buf);
			}
			sbuf_advance(buf, pra[ii].buf.nLen);
		}
	}
}

//on the aDSP copy the data from buffers we had to copy to the local remote_arg structure
static __inline int unpack_out_bufs(struct sbuf *buf, remote_arg *pra, int nBufs)
{
	int ii, nErr = 0;
	uint32_t len;
	C_ASSERT(sizeof(len) == 4);
	for (ii = 0; ii < nBufs; ++ii)
	{
		sbuf_read(buf, (uint8 *)&len, 4);
		VERIFY(len == pra[ii].buf.nLen);
		if (pra[ii].buf.nLen)
		{
			sbuf_align(buf, 8);
			sbuf_read(buf, pra[ii].buf.pv, pra[ii].buf.nLen);
		}
	}
bail:
	return nErr;
}

#endif
