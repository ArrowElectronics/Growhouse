/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __QDIAG_H__
#define __QDIAG_H__

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "hdlc.h"
#include "list.h"
#include "mbuf.h"
#include "diag_config.h"
#include "peripheral.h"
#include "util.h"
#include "watch.h"


extern unsigned int diag_dbg_mask;
extern struct diag_if * g_dif;

enum diag_debug_mask {
	DIAG_DBG_NONE				= 0x00000000,
	DIAG_DBG_CONFIG				= 0x00000001,
	DIAG_DBG_PERIPHERAL		    = 0x00000002,
	DIAG_DBG_TRANSPORT			= 0x00000004,
	DIAG_DBG_WATCH				= 0x00000008,
	DIAG_DBG_ROUTER				= 0x00000010,
	DIAG_DBG_CTRL				= 0x00000020,
	DIAG_DBG_MAIN				= 0x00000040,
	DIAG_DBG_MEMORY				= 0x00000080,
	DIAG_DBG_TRANSPORT_DUMP		= 0x00010000,
	DIAG_DBG_CONFIG_DUMP		= 0x00020000,
	DIAG_DBG_PERIPHERAL_DUMP	= 0x00040000,
	DIAG_DBG_ROUTER_DUMP		= 0x00080000,
	DIAG_DBG_PLUGIN				= 0x10000000,
	DIAG_DBG_ANY				= 0xffffffff,
};

#define pr_fmt(fmt) "QDIAG: " fmt

#define diag_err(fmt, arg...)				\
	printf(pr_fmt("ERROR " fmt "\n"), ##arg)

#define diag_warn(fmt, arg...)				\
	printf(pr_fmt("WARNING " fmt "\n"), ##arg)

#define diag_info(fmt, arg...)		\
	printf(pr_fmt("INFO " fmt "\n"), ##arg)

#define diag_dbg(mask, fmt, arg...) do {			\
	if (diag_dbg_mask & mask)					\
		printf(pr_fmt("%s@%s#%u: " fmt), __FILE__, __FUNCTION__, __LINE__, ##arg);	\
} while (0)

#define print_hex(prefix_str, buf, len, group) { \
	int i=0; printf(prefix_str); for(i=0; i<len; i++) { printf("%02X ", (buf)[i]); if ((i % group) == (group - 1) || (i == (len - 1))) printf("\n"); } \
}

#define diag_dbg_dump(mask, prefix_str, buf, len) do {	if ((((buf)[0]==0x4b)&&((buf)[1]==0x32))||(((buf)[0]==0x4b)&&((buf)[1]==0x12)&&((buf)[2]==0x39)&&((buf)[3]==0x00))) break; \
	if (diag_dbg_mask & mask)					\
		print_hex(pr_fmt(prefix_str),	\
			       (buf), len, 16);		\
} while (0)

#define diag_log_packet(f, prefix_str, buf, len, group) { \
	int i=0; fprintf(f, prefix_str); for(i=0; i<len; i++) { fprintf(f, "%02X ", (buf)[i]); if ((i % group) == (group - 1) || (i == (len - 1))) fprintf(f, "\n"); } \
}

#if defined USE_DIAG_MALLOC
#define diag_dbg_alloc(mask, a, s, fmt, arg...) \
	diag_malloc((a), (s)); \
	do {			\
		if (diag_dbg_mask & (mask | DIAG_DBG_MEMORY))					\
			printf(pr_fmt("%s@%s#%u: %lu bytes allocated %s" fmt), __FILE__, __FUNCTION__, __LINE__, (size_t)((a) ? QUAD_ALIGNED((s)) : (s)), ((a) ? "QUAD_ALIGNED" : ""), ##arg); \
	} while (0)
#define diag_dbg_free(mask, p, fmt, arg...) \
	do {			\
		size_t s = 0;\
		if (diag_dbg_mask & (mask | DIAG_DBG_MEMORY))					\
			printf(pr_fmt("%s@%s#%u: freeing at %p" fmt), __FILE__, __FUNCTION__, __LINE__, (void *)(p), ##arg); \
		s = (size_t)diag_free((p)); \
		if (diag_dbg_mask & (mask | DIAG_DBG_MEMORY))					\
			printf(pr_fmt("%s@%s#%u: %lu bytes freed at %p" fmt), __FILE__, __FUNCTION__, __LINE__, s, (void *)(p), ##arg); \
	} while (0)
#else
#define diag_dbg_alloc(mask, a, s, fmt, arg...) \
	malloc((a) ? QUAD_ALIGNED((s)) : (s)); 
#define diag_dbg_free(mask, p, fmt, arg...) \
	do {			\
		free(p); \
	} while (0)
#endif

#define BIT(x) (1 << (x))

#define APPS_BUF_SIZE	16384

/*
 * Feature Mask Definitions: Feature mask is used to specify Diag features
 * supported by the Apps processor
 *
 * F_DIAG_FEATURE_MASK_SUPPORT - Denotes we support sending and receiving
 *                               feature masks
 * F_DIAG_LOG_ON_DEMAND_APPS - Apps responds to Log on Demand request
 * F_DIAG_REQ_RSP_SUPPORT - Apps supported dedicated request response Channel
 * F_DIAG_APPS_HDLC_ENCODE - HDLC encoding is done on the forward channel
 * F_DIAG_STM - Denotes Apps supports Diag over STM
 */
#define DIAG_FEATURE_FEATURE_MASK_SUPPORT			BIT(0)
#define DIAG_FEATURE_DIAG_MASTER_SETS_COMMON_MASK	BIT(1)
#define DIAG_FEATURE_LOG_ON_DEMAND_APPS				BIT(2)
#define DIAG_FEATURE_DIAG_VERSION_RSP_ON_MASTER		BIT(3)
#define DIAG_FEATURE_REQ_RSP_SUPPORT				BIT(4)
#define DIAG_FEATURE_DIAG_PRESET_MASKS				BIT(5)
#define DIAG_FEATURE_APPS_HDLC_ENCODE				BIT(6)
#define DIAG_FEATURE_STM							BIT(9)
#define DIAG_FEATURE_PERIPHERAL_BUFFERING			BIT(10)
#define DIAG_FEATURE_MASK_CENTRALIZATION			BIT(11)
#define DIAG_FEATURE_SOCKETS_ENABLED				BIT(13)

#define DIAG_CMD_SUBSYS_DISPATCH		75
#define DIAG_CMD_SUBSYS_DISPATCH_V2		128

void queue_push(struct list_head *queue, uint8_t *msg, size_t msglen);

int diag_transport_init(struct diag_if * dif);
int diag_transport_receive_packet(int fd, void *data);
int diag_transport_dispatch_packet(void *data);

int diag_router_init(struct diag_if * dif);
int diag_router_handle_incoming_packet(void *data);
int diag_router_handle_outgoing_packet(int fd, void *data);
int diag_router_handle_outgoing_packet_cmd(int fd, void *data);
int diag_router_quit(void);
int diag_init(struct diag_if * dif, const char *conf_fname, int log_enabled);
int diag_quit(void);

int diag_router_peripheral_cmd_cb(struct diag_cmd *dc, void *data);
#endif
