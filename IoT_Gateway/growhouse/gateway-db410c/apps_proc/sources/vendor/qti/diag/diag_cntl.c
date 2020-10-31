/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Copyright (c) 2016, Linaro Ltd.
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
#include <err.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "qdiag.h"
#include "peripheral.h"
#include "util.h"
#include "masks.h"
#include "diag_cntl.h"

#define DIAG_CTRL_MSG_DTR               2
#define DIAG_CTRL_MSG_DIAGMODE          3
#define DIAG_CTRL_MSG_DIAGDATA          4
#define DIAG_CTRL_MSG_FEATURE           8
#define DIAG_CTRL_MSG_EQUIP_LOG_MASK    9
#define DIAG_CTRL_MSG_EVENT_MASK_V2     10
#define DIAG_CTRL_MSG_F3_MASK_V2        11
#define DIAG_CTRL_MSG_NUM_PRESETS       12
#define DIAG_CTRL_MSG_SET_PRESET_ID     13
#define DIAG_CTRL_MSG_LOG_MASK_WITH_PRESET_ID   14
#define DIAG_CTRL_MSG_EVENT_MASK_WITH_PRESET_ID 15
#define DIAG_CTRL_MSG_F3_MASK_WITH_PRESET_ID    16
#define DIAG_CTRL_MSG_CONFIG_PERIPHERAL_TX_MODE 17
#define DIAG_CTRL_MSG_PERIPHERAL_BUF_DRAIN_IMM  18
#define DIAG_CTRL_MSG_CONFIG_PERIPHERAL_WMQ_VAL 19
#define DIAG_CTRL_MSG_DCI_CONNECTION_STATUS     20
#define DIAG_CTRL_MSG_LAST_EVENT_REPORT         22
#define DIAG_CTRL_MSG_LOG_RANGE_REPORT          23
#define DIAG_CTRL_MSG_SSID_RANGE_REPORT         24
#define DIAG_CTRL_MSG_BUILD_MASK_REPORT         25
#define DIAG_CTRL_MSG_DEREG             27
#define DIAG_CTRL_MSG_DCI_HANDSHAKE_PKT         29
#define DIAG_CTRL_MSG_PD_STATUS                 30
#define DIAG_CTRL_MSG_TIME_SYNC_PKT             31


extern struct diag_if *g_dif;

struct diag_cntl_hdr {
	uint32_t cmd;
	uint32_t len;
};

struct cmd_range {
	uint16_t first;
	uint16_t last;
	uint32_t data;
};

#define DIAG_CNTL_CMD_REGISTER	1
struct diag_cntl_cmd_reg {
	struct diag_cntl_hdr hdr;
	uint32_t version;
	uint16_t cmd;
	uint16_t subsys;
	uint16_t count_entries;
	uint16_t port;
	struct cmd_range ranges[];
} __packed;
#define to_cmd_reg(h) container_of(h, struct diag_cntl_cmd_reg, hdr)

#define DIAG_CNTL_CMD_FEATURE_MASK 8
struct diag_cntl_cmd_feature {
	struct diag_cntl_hdr hdr;
	uint32_t mask_len;
	uint8_t mask[];
} __packed;
#define to_cmd_feature(h) container_of(h, struct diag_cntl_cmd_feature, hdr)

#define DIAG_CNTL_CMD_LOG_MASK 9
struct diag_cntl_cmd_log_mask {
	struct diag_cntl_hdr hdr;
	uint8_t stream_id;
	uint8_t status;
	uint8_t equip_id;
	uint32_t last_item;
	uint32_t log_mask_size;
	uint8_t equip_log_mask[];
} __packed;

#define DIAG_CNTL_CMD_MSG_MASK 11
struct diag_cntl_cmd_msg_mask {
	struct diag_cntl_hdr hdr;
	uint8_t stream_id;
	uint8_t status;
	uint8_t msg_mode;
	struct diag_ssid_range_t range;
	uint32_t msg_mask_len;
	uint8_t range_msg_mask[];
} __packed;

#define DIAG_CNTL_CMD_EVENT_MASK 10
struct diag_cntl_cmd_event_mask {
	struct diag_cntl_hdr hdr;
	uint8_t stream_id;
	uint8_t status;
	uint8_t event_config;
	uint32_t event_mask_len;
	uint8_t event_mask[];
} __packed;

#define DIAG_CNTL_CMD_NUM_PRESETS 12
struct diag_cntl_num_presets {
	struct diag_cntl_hdr hdr;
	uint8_t num;
};

static int diag_cntl_register(struct peripheral *peripheral,
			      struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_reg *pkt = to_cmd_reg(hdr);
	struct diag_cmd *dc;
	unsigned int subsys;
	unsigned int cmd;
	unsigned int first;
	unsigned int last;
	int i;

	for (i = 0; i < pkt->count_entries; i++) {
		cmd = pkt->cmd;
		subsys = pkt->subsys;
		//diag_dbg(DIAG_DBG_CTRL, "cmd = 0x%X: \n", pkt->cmd);
		//diag_dbg(DIAG_DBG_CTRL, "subsys = 0x%X: \n", pkt->subsys);
		if (cmd == 0xff && subsys != 0xff)
			cmd = DIAG_CMD_SUBSYS_DISPATCH;

		first = cmd << 24 | subsys << 16 | pkt->ranges[i].first;
		last = cmd << 24 | subsys << 16 | pkt->ranges[i].last;

		 diag_dbg(DIAG_DBG_CTRL,"[%s] register 0x%x - 0x%x\n",
			  peripheral->name, first, last);

		dc = malloc(sizeof(*dc));
		if (!dc)
			err(1, "malloc failed");
		memset(dc, 0, sizeof(*dc));

		dc->first = first;
		dc->last = last;
		dc->peripheral = peripheral;
		dc->cb=&diag_router_peripheral_cmd_cb;
		list_add(&g_dif->diag_cmds, &dc->node);
	}

	return 0;
}

static int diag_cntl_feature_mask(struct peripheral *peripheral,
				  struct diag_cntl_hdr *hdr, size_t len)
{
	struct diag_cntl_cmd_feature *pkt = to_cmd_feature(hdr);
	uint32_t mask = 0;
	int i;

	for (i = 0; i < pkt->mask_len && i < sizeof(mask); i++)
		mask |= pkt->mask[i] << (8 * i);

	printf("[%s] mask:", peripheral->name);

	if (mask & DIAG_FEATURE_FEATURE_MASK_SUPPORT)
		printf(" FEATURE_MASK_SUPPORT");
	if (mask & DIAG_FEATURE_DIAG_MASTER_SETS_COMMON_MASK)
		printf(" DIAG_MASTER_SETS_COMMON_MASK");
	if (mask & DIAG_FEATURE_LOG_ON_DEMAND_APPS)
		printf(" LOG_ON_DEMAND");
	if (mask & DIAG_FEATURE_DIAG_VERSION_RSP_ON_MASTER)
		printf(" DIAG_VERSION_RSP_ON_MASTER");
	if (mask & DIAG_FEATURE_REQ_RSP_SUPPORT)
		printf(" REQ_RSP");
	if (mask & DIAG_FEATURE_APPS_HDLC_ENCODE)
		printf(" APPS_HDLC_ENCODE");
	if (mask & DIAG_FEATURE_STM)
		printf(" STM");
	if (mask & DIAG_FEATURE_PERIPHERAL_BUFFERING)
		printf(" PERIPHERAL-BUFFERING");
	if (mask & DIAG_FEATURE_MASK_CENTRALIZATION)
		printf(" MASK-CENTERALIZATION");
	if (mask & DIAG_FEATURE_SOCKETS_ENABLED)
		printf(" SOCKETS");

	printf(" (0x%x)\n", mask);

	peripheral->features = mask;
	diag_cntl_send_feature_mask(peripheral);

	return 0;
}

void diag_cntl_send_log_mask(struct peripheral *peripheral, uint32_t equip_id)
{
	struct diag_cntl_cmd_log_mask *pkt;
	size_t len = sizeof(*pkt);
	uint32_t num_items = 0;
	uint8_t *mask = NULL;
	uint32_t mask_size = 0;
	uint8_t status = diag_get_log_mask_status();

	if (&peripheral->cntlq == NULL) {
		diag_warn("Peripheral %s has no control queue. Skipping!\n", peripheral->name);
		return;
	}

	if (status == DIAG_CTRL_MASK_VALID) {
		diag_cmd_get_log_mask(equip_id, &num_items, &mask, &mask_size);
	} else {
		equip_id = 0;
	}
	len += mask_size;
	pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, len, "\n");
	pkt->hdr.cmd = DIAG_CNTL_CMD_LOG_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->stream_id = 1;
	pkt->status = status;
	pkt->equip_id = equip_id;
	pkt->last_item = num_items;
	pkt->log_mask_size = mask_size;
	if (mask != NULL) {
		memcpy(pkt->equip_log_mask, mask, mask_size);
		diag_dbg_free(DIAG_DBG_MEMORY, mask, "\n");
	}

	if (peripheral->cntlq.prev == NULL) {
		diag_dbg(DIAG_DBG_CTRL, "Peripheral %s control queue not initialised! Initialising\n", peripheral->name);
		LIST_DYNAMIC_INIT(peripheral->cntlq);
	}
	queue_push(&peripheral->cntlq, (uint8_t *)pkt, len);
	diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");
}

void diag_cntl_send_msg_mask(struct peripheral *peripheral, struct diag_ssid_range_t *range)
{
	struct diag_cntl_cmd_msg_mask *pkt;
	size_t len = sizeof(*pkt);
	uint32_t num_items = 0;
	uint32_t *mask = NULL;
	uint32_t mask_size = 0;
	struct diag_ssid_range_t DUMMY_RANGE = { 0, 0 };
	uint8_t status = diag_get_msg_mask_status();

	if (&peripheral->cntlq == NULL) {
		diag_warn("Peripheral %s has no control queue. Skipping!\n", peripheral->name);
		return;
	}

	if (status == DIAG_CTRL_MASK_VALID) {
		diag_cmd_get_msg_mask(range, &mask);
		num_items = range->ssid_last - range->ssid_first + 1;
	} else if (status == DIAG_CTRL_MASK_ALL_DISABLED) {
		range = &DUMMY_RANGE;
		num_items = 0;
	} else if (status == DIAG_CTRL_MASK_ALL_ENABLED) {
		diag_cmd_get_msg_mask(range, &mask);
		num_items = 1;
	}
	mask_size = num_items * sizeof(*mask);
	len += mask_size;
	pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, len, "\n");
	pkt->hdr.cmd = DIAG_CNTL_CMD_MSG_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->stream_id = 1;
	pkt->status = status;
	pkt->msg_mode = 0;
	pkt->range = *range;
	pkt->msg_mask_len = num_items;
	if (mask != NULL) {
		memcpy(pkt->range_msg_mask, mask, mask_size);
		diag_dbg_free(DIAG_DBG_MEMORY, mask, "\n");
	}

	if (peripheral->cntlq.prev == NULL) {
		diag_dbg(DIAG_DBG_CTRL, "Peripheral %s control queue not initialised! Initialising\n", peripheral->name);
		LIST_DYNAMIC_INIT(peripheral->cntlq);
	}
	queue_push(&peripheral->cntlq, (uint8_t *)pkt, len);
	diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");
}

void diag_cntl_send_event_mask(struct peripheral *peripheral)
{
	struct diag_cntl_cmd_event_mask *pkt;
	size_t len = sizeof(*pkt);
	uint8_t *mask = NULL;
	uint16_t mask_size = 0;
	uint8_t status = diag_get_event_mask_status();
	uint8_t event_config = (status == DIAG_CTRL_MASK_ALL_ENABLED || status == DIAG_CTRL_MASK_VALID) ? 0x1 : 0x0;

	if (&peripheral->cntlq == NULL) {
		diag_warn("Peripheral %s has no control queue. Skipping!\n", peripheral->name);
		return;
	}

	if (status == DIAG_CTRL_MASK_VALID) {
		if (diag_cmd_get_event_mask(event_max_num_bits , &mask) == 0) {
			mask_size = EVENT_COUNT_TO_BYTES(event_max_num_bits );
		}
	}
	len += mask_size;
	pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, len, "\n");
	pkt->hdr.cmd = DIAG_CNTL_CMD_EVENT_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->stream_id = 1;
	pkt->status = status;
	pkt->event_config = event_config;
	pkt->event_mask_len = mask_size;
	if (mask != NULL) {
		memcpy(pkt->event_mask, mask, mask_size);
		diag_dbg_free(DIAG_DBG_MEMORY, mask, "\n");
	}

	if (peripheral->cntlq.prev == NULL) {
		diag_dbg(DIAG_DBG_CTRL, "Peripheral %s control queue not initialised! Initialising\n", peripheral->name);
		LIST_DYNAMIC_INIT(peripheral->cntlq);
	}
	queue_push(&peripheral->cntlq, (uint8_t *)pkt, len);
	diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");
}

void diag_cntl_send_feature_mask(struct peripheral *peripheral)
{
	struct diag_cntl_cmd_feature *pkt;
	size_t len = sizeof(*pkt) + 2;
	uint32_t mask = 0;

	if (&peripheral->cntlq == NULL) {
		diag_warn("Peripheral %s has no control queue. Skipping!\n", peripheral->name);
		return;
	}

	mask = DIAG_FEATURE_FEATURE_MASK_SUPPORT | 
	       DIAG_FEATURE_DIAG_MASTER_SETS_COMMON_MASK | 
	       DIAG_FEATURE_APPS_HDLC_ENCODE ;

	pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, 1, len, "\n");
	pkt->hdr.cmd = DIAG_CNTL_CMD_FEATURE_MASK;
	pkt->hdr.len = len - sizeof(struct diag_cntl_hdr);
	pkt->mask_len = 2;
	pkt->mask[0] = (mask >> 8) & 0xff;
	pkt->mask[1] = (mask >> 0) & 0xff;

	if (peripheral->cntlq.prev == NULL) {
		diag_dbg(DIAG_DBG_CTRL, "Peripheral %s control queue not initialised! Initialising\n", peripheral->name);
		LIST_DYNAMIC_INIT(peripheral->cntlq);
	}
	queue_push(&peripheral->cntlq, (uint8_t *)pkt, len);
	diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");
}

int diag_cntl_recv(int fd, void *data)
{
	struct peripheral *peripheral = data;
	struct diag_cntl_hdr *hdr;
	uint8_t buf[APPS_BUF_SIZE];
	size_t offset = 0;
	ssize_t n;

	n = read(fd, buf, sizeof(buf));
	if (n < 0) {
		warn("failed to read from cntl channel");
		peripheral_close(peripheral);
		return 0;
	}
		diag_dbg_dump(DIAG_DBG_CTRL, "CNTL read : ", buf, n);

	for (;;) {
		if (offset + sizeof(struct diag_cntl_hdr) > n)
			break;

		hdr = (struct diag_cntl_hdr *)(buf + offset);
		if (offset + sizeof(struct diag_cntl_hdr) + hdr->len > n) {
			warnx("truncated diag cntl command");
			break;
		}

		switch (hdr->cmd) {
		case DIAG_CNTL_CMD_REGISTER:
			diag_cntl_register(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_FEATURE_MASK:
			diag_cntl_feature_mask(peripheral, hdr, n);
			break;
		case DIAG_CNTL_CMD_NUM_PRESETS:
			break;
		default:
			warnx("[%s] unsupported control packet: %d",
			      peripheral->name, hdr->cmd);
			break;
		}
		diag_dbg(DIAG_DBG_CTRL, "CNTL\n");

		offset += sizeof(struct diag_cntl_hdr) + hdr->len;
	}

	return 0;
}

void diag_cntl_close(struct peripheral *peripheral)
{
	struct list_head *item;
	struct list_head *next;
	struct diag_cmd *dc;
/* TODO init diag_cmds to interface */
	list_for_each_safe(item, next, &g_dif->diag_cmds) {
		dc = container_of(item, struct diag_cmd, node);
		if (dc->peripheral == peripheral)
			list_del(&dc->node);
	}

}
