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
#include "qdiag.h"
#include "diag_cntl.h"
#include "masks.h"
#include "hdlc.h"

extern struct diag_if * g_dif;
struct list_head apps_cmds=LIST_INIT(apps_cmds);
struct list_head common_cmds=LIST_INIT(common_cmds);

void queue_push(struct list_head *queue, uint8_t *msg, size_t msglen)
{
	struct mbuf *mbuf;
	void *ptr = NULL;
	struct list_head* item = NULL;

	diag_dbg(DIAG_DBG_ROUTER,"START\n");
	mbuf = mbuf_alloc(msglen);
	if (mbuf != NULL) {
		ptr = mbuf_put(mbuf, msglen);
		memcpy(ptr, msg, msglen);
	} else {
		err(-ENOMEM, "NO MORE MEM");
	}

	item = &mbuf->node;
	list_add(queue, item);
	diag_dbg(DIAG_DBG_ROUTER,"END\n");
}

static int send_packet(uint8_t *msg, size_t msglen, int encode)
{
	struct mbuf *mbuf;
	uint8_t *outbuf = msg;
	size_t outlen = msglen;
	void *ptr;
	int ret =0;
	if (encode) {
		outbuf = hdlc_encode(msg, msglen, &outlen);
		if (!outbuf)
			err(1, "failed to allocate hdlc destination buffer");
	}
	mbuf = mbuf_alloc(outlen);
	if (mbuf == NULL) return -1;
	ptr = mbuf_put(mbuf, outlen);
	memcpy(ptr, outbuf, outlen);
	if (encode) 
	{
		diag_dbg_free(DIAG_DBG_ROUTER, outbuf, "\n");
	}
	//diag_dbg_dump(DIAG_DBG_ROUTER_DUMP,"sending: ",msg,msglen);
	ret = g_dif->transport_send_func(mbuf);
	
	diag_dbg_free(DIAG_DBG_ROUTER, mbuf, "\n");
	return ret;
}

int diag_router_handle_outgoing_packet_cmd(int fd, void *data)
{
	struct peripheral *peripheral = data;
	uint8_t buf[APPS_BUF_SIZE];
	ssize_t n;

	n = read(fd, buf, sizeof(buf));
	//diag_dbg(DIAG_DBG_ROUTER,"read %d bytes from fd %d\n",n,fd);
	if (n < 0) {
		if (errno != EAGAIN) {
			warn("failed to read from data channel");
			peripheral_close(peripheral);
			return -1;
		}
		return -1;
	}
	
	diag_dbg_dump(DIAG_DBG_ROUTER_DUMP, "************************* ",buf,n);
	return 0;
}
	
	
int diag_router_handle_outgoing_packet(int fd, void *data)
{
	struct peripheral *peripheral = data;
	uint8_t buf[APPS_BUF_SIZE];
	uint8_t *ptr;
	size_t len;
	ssize_t n;


	n = read(fd, buf, sizeof(buf));
	//diag_dbg(DIAG_DBG_ROUTER,"read %d bytes from fd %d\n",n,fd);
	if (n < 0) {
		if (errno != EAGAIN) {
			warn("failed to read from data channel");
			peripheral_close(peripheral);
			return -1;
		}
		return -1;
	}

	ptr = buf;
	len = n;
	
	send_packet(ptr, len, ((peripheral->features & DIAG_FEATURE_APPS_HDLC_ENCODE) == 0));


	return 0;
}

static int diag_router_handle_Diag_version(struct diag_cmd *dc, void *data)
{		
	struct Diag_version_Resp_pkt
	{
		uint8_t CMD_CODE;
		uint8_t Ver;
	}Diag_version_Resp_pkt;
	struct Diag_version_Resp_pkt pkt={0};

	pkt.CMD_CODE=0x1c;
	pkt.Ver =2;
	
	
	send_packet((uint8_t *)&pkt, sizeof(Diag_version_Resp_pkt), 1);
	
	
	return 0;
}

struct Diag_Log_Cmd_Header
{
	uint8_t CMD_CODE;
	uint8_t res1;
	uint8_t res2;
	uint8_t res3;
	uint32_t OPERATION;
}__packed;


struct Diag_Log_Cmd_Mask {
	uint32_t equip_id;
	uint32_t num_items;
	uint8_t mask[];
}__packed;

struct Diag_Msg_Cmd_Header
{
	uint8_t CMD_CODE;
	uint8_t OPERATION;
}__packed;

struct Diag_Event_Cmd_Header
{
	uint8_t CMD_CODE;
	uint8_t STATUS;
	uint16_t RESERVED;
}__packed;

#define DIAG_CMD_OP_LOG_DISABLE		0
#define DIAG_CMD_OP_GET_LOG_RANGE	1
#define DIAG_CMD_OP_SET_LOG_MASK	3
#define DIAG_CMD_OP_GET_LOG_MASK	4

#define DIAG_CMD_OP_GET_SSID_RANGE	1
#define DIAG_CMD_OP_GET_BUILD_MASK	2
#define DIAG_CMD_OP_GET_MSG_MASK	3
#define DIAG_CMD_OP_SET_MSG_MASK	4
#define DIAG_CMD_OP_SET_ALL_MSG_MASK	5


#define DIAG_CMD_STATUS_SUCCESS	0
#define DIAG_CMD_STATUS_INVALID_EQUIPID	1

static int diag_router_handle_Logging_Configuration_Response(struct diag_cmd *dc, void *data)
{
	struct mbuf* mbuf = (struct mbuf*) data;
	struct Diag_Log_Cmd_Header *request_header = (struct Diag_Log_Cmd_Header *)mbuf->data;

	if (request_header->CMD_CODE != 0x73)
		err(errno, "Wrong command!");

	switch (request_header->OPERATION) {
	case DIAG_CMD_OP_LOG_DISABLE: {
		struct Get_Log_Disable_Response_pkt
		{
			struct Diag_Log_Cmd_Header header;
			uint32_t STATUS;
		} __packed pkt;
		memcpy(&pkt, request_header, sizeof(*request_header));
		diag_cmd_disable_log();
		pkt.STATUS = 0;

		struct list_head *item;
		list_for_each(item, peripherals) {
			struct peripheral *perf = container_of(item, struct peripheral, node);
			diag_cntl_send_log_mask(perf, 0); // equip_id is ignored
		}

		send_packet((uint8_t *)&pkt, sizeof(pkt), 1);
		break;
	}
	case DIAG_CMD_OP_GET_LOG_RANGE: {
		struct Get_Log_Range_Response_pkt
		{
			struct Diag_Log_Cmd_Header header;
			uint32_t STATUS;
			uint32_t RANGES[MAX_EQUIP_ID];
		} __packed pkt;
		memcpy(&pkt, request_header, sizeof(*request_header));
		diag_cmd_get_log_range(pkt.RANGES, MAX_EQUIP_ID);
		pkt.STATUS = 0;

		send_packet((uint8_t *)&pkt, sizeof(pkt), 1);

		break;
	}
	case DIAG_CMD_OP_SET_LOG_MASK: {
		struct Diag_Log_Cmd_Mask *mask_to_set = (struct Diag_Log_Cmd_Mask*)(mbuf->data + sizeof(struct Diag_Log_Cmd_Header));
		struct Set_Log_Response_pkt
		{
			struct Diag_Log_Cmd_Header header;
			uint32_t STATUS;
			struct Diag_Log_Cmd_Mask MASK_STRUCTURE;
		} __packed *pkt;
		uint32_t pkt_size = sizeof(*pkt);
		uint32_t mask_size = mbuf->offset - sizeof(struct Diag_Log_Cmd_Header);
		pkt_size += mask_size;
		pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
		memcpy(pkt, request_header, sizeof(*request_header));
		diag_dbg(DIAG_DBG_ROUTER, "Request: equip_id=%u num_items=%u\n", mask_to_set->equip_id, mask_to_set->num_items);
		diag_cmd_set_log_mask(mask_to_set->equip_id, &mask_to_set->num_items, mask_to_set->mask, &mask_size);
		memcpy(&pkt->MASK_STRUCTURE, mask_to_set, mask_size); // num_items might have been capped!!!
		pkt->STATUS =0;

		struct list_head *item;
		list_for_each(item, peripherals) {
			struct peripheral *perf = container_of(item, struct peripheral, node);
			diag_cntl_send_log_mask(perf, pkt->MASK_STRUCTURE.equip_id);
		}

		send_packet((uint8_t *)pkt, pkt_size, 1);
		diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");

		break;
	}
	case DIAG_CMD_OP_GET_LOG_MASK: {
		uint32_t *equip_id = (uint32_t *)(mbuf->data + sizeof(struct Diag_Log_Cmd_Header));
		struct Get_Log_Response_pkt
		{
			struct Diag_Log_Cmd_Header header;
			uint32_t STATUS;
			struct Diag_Log_Cmd_Mask MASK_STRUCTURE;
		} __packed *pkt;
		uint32_t num_items = 0;
		uint8_t *mask;
		uint32_t mask_size = 0;
		uint32_t pkt_size = sizeof(*pkt);
		if (diag_cmd_get_log_mask(*equip_id, &num_items, &mask, &mask_size) == 0) {
			pkt_size += mask_size;
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			memcpy(pkt, request_header, sizeof(*request_header));
			pkt->MASK_STRUCTURE.equip_id = *equip_id;
			pkt->MASK_STRUCTURE.num_items = num_items;
			if (mask != NULL) {
				memcpy(&pkt->MASK_STRUCTURE.mask, mask, mask_size);
				diag_dbg_free(DIAG_DBG_MEMORY, mask, "\n");
			}
			pkt->STATUS = 0;
		} else {
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			memcpy(pkt, request_header, sizeof(*request_header));
			pkt->MASK_STRUCTURE.equip_id = *equip_id;
			pkt->MASK_STRUCTURE.num_items = num_items;
			pkt->STATUS = MSG_STATUS_FAIL;
		}

		struct list_head *item;
		list_for_each(item, peripherals) {
			struct peripheral *perf = container_of(item, struct peripheral, node);
			diag_cntl_send_log_mask(perf, pkt->MASK_STRUCTURE.equip_id);
		}

		send_packet((uint8_t *)pkt, pkt_size, 1);
		diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");

		break;
	}
	default:
		diag_err("Unrecognized operation %d!!!", request_header->OPERATION);
	}


	return 0;
}

static int diag_router_handle_Extended_Message_Configuration_Response(struct diag_cmd *dc, void *data)
{		
	struct mbuf* mbuf = (struct mbuf*) data;
	struct Diag_Msg_Cmd_Header *request_header = (struct Diag_Msg_Cmd_Header *)mbuf->data;

	if (request_header->CMD_CODE != 0x7D)
		err(errno, "Wrong command!");

	switch (request_header->OPERATION) {
	case DIAG_CMD_OP_GET_SSID_RANGE: {
		struct Get_SSID_Response_pkt
		{
			struct Diag_Msg_Cmd_Header header;
			uint8_t STATUS;
			uint8_t RESERVED;
			uint32_t RANGE_CNT;
			struct diag_ssid_range_t RANGES[];
		} __packed *pkt;
		uint32_t pkt_size = sizeof(*pkt);
		uint32_t count = 0;
		struct diag_ssid_range_t *ranges = NULL;
		uint32_t ranges_size = 0;
		diag_cmd_get_ssid_range(&count, &ranges);
		ranges_size = count * sizeof(*ranges);
		pkt_size += ranges_size;
		pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
		memcpy(pkt, request_header, sizeof(*request_header));
		pkt->RANGE_CNT = count;
		if (ranges != NULL) {
			memcpy(pkt->RANGES, ranges, ranges_size);
			diag_dbg_free(DIAG_DBG_MEMORY, ranges, "\n");
		}
		pkt->STATUS = MSG_STATUS_SUCCESS;

		send_packet((uint8_t *)pkt, pkt_size, 1);
		diag_dbg_free(DIAG_DBG_MEMORY, pkt, "\n");

		break;
	}
	case DIAG_CMD_OP_GET_BUILD_MASK: {
		struct diag_ssid_range_t *range = (struct diag_ssid_range_t *)(mbuf->data + sizeof(struct Diag_Msg_Cmd_Header));
		struct Get_Build_Mask_Response_pkt
		{
			struct Diag_Msg_Cmd_Header header;
			struct diag_ssid_range_t range;
			uint8_t STATUS;
			uint8_t RESERVED;
			uint32_t BLD_MASKS[];
		} __packed *pkt;
		uint32_t pkt_size = sizeof(*pkt);
		uint32_t *masks = NULL;
		uint32_t masks_size = 0;

		if (diag_cmd_get_build_mask(range, &masks) == 0) {
			masks_size = (range->ssid_last - range->ssid_first + 1) * sizeof(*masks);
			pkt_size += masks_size;
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			memcpy(pkt, request_header, sizeof(*request_header));
			memcpy(&pkt->range, range, sizeof(*range));
			if (masks != NULL) {
				memcpy(pkt->BLD_MASKS, masks, masks_size);
				diag_dbg_free(DIAG_DBG_MEMORY, masks, "\n");
			}
			pkt->STATUS = MSG_STATUS_SUCCESS;
		} else {
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			memcpy(pkt, request_header, sizeof(*request_header));
			pkt->STATUS = MSG_STATUS_FAIL;
		}

		send_packet((uint8_t *)pkt, pkt_size, 1);
		diag_dbg_free(DIAG_DBG_MEMORY, pkt, "\n");

		break;
	}
	case DIAG_CMD_OP_GET_MSG_MASK: {
		struct diag_ssid_range_t *range = (struct diag_ssid_range_t *)(mbuf->data + sizeof(struct Diag_Msg_Cmd_Header));
		struct Get_Msg_Mask_Response_pkt
		{
			struct Diag_Msg_Cmd_Header header;
			struct diag_ssid_range_t range;
			uint8_t STATUS;
			uint8_t RSVD;
			uint32_t RT_MASKS[];
		} __packed *pkt;
		uint32_t pkt_size = sizeof(*pkt);
		uint32_t *masks = NULL;
		uint32_t masks_size = 0;
		if (diag_cmd_get_msg_mask(range, &masks) == 0) {

			masks_size = (range->ssid_last - range->ssid_first + 1) * sizeof(*masks);
			pkt_size += masks_size;
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			memcpy(pkt, request_header, sizeof(*request_header));
			memcpy(&pkt->range, range, sizeof(*range));
			if (masks != NULL) {
				memcpy(pkt->RT_MASKS, masks, masks_size);
				diag_dbg_free(DIAG_DBG_MEMORY, masks, "\n");
			}
			pkt->STATUS = MSG_STATUS_SUCCESS;
		} else {
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			memcpy(pkt, request_header, sizeof(*request_header));
			pkt->STATUS = MSG_STATUS_FAIL;
		}

		send_packet((uint8_t *)pkt, pkt_size, 1);
		diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");

		break;
	}
	case DIAG_CMD_OP_SET_MSG_MASK: {
		struct Set_Msg_Mask
		{
			struct Diag_Msg_Cmd_Header header;
			struct diag_ssid_range_t range;
			uint16_t RSVD;
			uint32_t masks[];
		} __packed *req = (struct Set_Msg_Mask *)mbuf->data;
		struct Set_Msg_Mask_Response_pkt
		{
			struct Diag_Msg_Cmd_Header header;
			struct diag_ssid_range_t RANGE;
			uint8_t STATUS;
			uint8_t RSVD;
			uint32_t RT_MASKS[];
		} __packed *pkt;
		uint32_t pkt_size = sizeof(*pkt);
		uint32_t masks_size = 0;
		if (diag_cmd_set_msg_mask(&req->range, req->masks) == 0) {
			masks_size = (req->range.ssid_last - req->range.ssid_first + 1) * sizeof(uint32_t);
			pkt_size += masks_size;
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			pkt->header = req->header;
			pkt->RANGE = req->range;
			pkt->RSVD = req->RSVD;
			if (req->masks != NULL) {
				memcpy(pkt->RT_MASKS, req->masks, masks_size);
			}
			pkt->STATUS = MSG_STATUS_SUCCESS;

			struct list_head *item;
			list_for_each(item, peripherals) {
				struct peripheral *perf = container_of(item, struct peripheral, node);
				diag_cntl_send_msg_mask(perf, &pkt->RANGE);
			}
		} else {
			pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
			pkt->header = req->header;
			pkt->RANGE = req->range;
			pkt->RSVD = req->RSVD;
			pkt->STATUS = MSG_STATUS_FAIL;
		}

		send_packet((uint8_t *)pkt, pkt_size, 1);
		diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");

		break;
	}
	case DIAG_CMD_OP_SET_ALL_MSG_MASK: {
		struct Set_Msg_Mask
		{
			struct Diag_Msg_Cmd_Header header;
			uint8_t RSVD;
			uint32_t mask;
		} __packed *req = (struct Set_Msg_Mask *)mbuf->data;
		struct Set_Msg_Mask_Response_pkt
		{
			struct Diag_Msg_Cmd_Header header;
			uint8_t STATUS;
			uint8_t RSVD;
			uint32_t RT_MASK;
		} __packed pkt;
		diag_cmd_set_all_msg_mask(req->mask);
		pkt.header = req->header;
		pkt.RSVD = req->RSVD;
		pkt.RT_MASK = req->mask;
		pkt.STATUS = MSG_STATUS_SUCCESS;

		struct list_head *item;
		list_for_each(item, peripherals) {
			struct peripheral *perf = container_of(item, struct peripheral, node);
			diag_cntl_send_msg_mask(perf, NULL); // range is ignored
		}

		send_packet((uint8_t *)&pkt, sizeof(pkt), 1);
		break;
	}
	default:
		diag_err("Unrecognized operation %d!!!", request_header->OPERATION);
	}

	return 0;
}

static int diag_router_handle_Event_Get_Mask_Response(struct diag_cmd *dc, void *data)
{
	struct mbuf* mbuf = (struct mbuf*) data;
	struct Diag_Event_Cmd_Header *request_header = (struct Diag_Event_Cmd_Header *)mbuf->data;
	struct Get_Event_Mask_Response_pkt
	{
		struct Diag_Event_Cmd_Header header;
		uint16_t NUM_BITS;
		uint8_t MASK[];
	} __packed *pkt;
	uint32_t pkt_size = sizeof(*pkt);
	uint16_t mask_size = 0;
	uint8_t *mask = NULL;

	if (request_header->CMD_CODE != 0x81)
		err(errno, "Wrong command!");

	if (diag_cmd_get_event_mask(event_max_num_bits, &mask) == 0) {
		mask_size = EVENT_COUNT_TO_BYTES(event_max_num_bits);
		pkt_size += mask_size;
		pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
		pkt->header = *request_header;
		pkt->NUM_BITS = event_max_num_bits;
		if (mask != NULL) {
			memcpy(&pkt->MASK, mask, mask_size);
			diag_dbg_free(DIAG_DBG_MEMORY, mask, "\n");
		}
		pkt->header.STATUS = EVENT_STATUS_SUCCESS;
	} else {
		pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
		pkt->header = *request_header;
		pkt->NUM_BITS = 0;
		pkt->header.STATUS = EVENT_STATUS_FAIL;
	}

	send_packet((uint8_t *)pkt, pkt_size, 1);
	diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");

	return 0;
}

static int diag_router_handle_Event_Set_Mask_Response(struct diag_cmd *dc, void *data)
{
	struct mbuf* mbuf = (struct mbuf*) data;
	struct Set_Event_Mask_Response_pkt
	{
		struct Diag_Event_Cmd_Header header;
		uint16_t NUM_BITS;
		uint8_t MASK[];
	} __packed *pkt, *req = (struct Set_Event_Mask_Response_pkt *)mbuf->data;
	uint32_t pkt_size = sizeof(*pkt);
	uint16_t mask_size = 0;

	if (req->header.CMD_CODE != 0x82)
		err(errno, "Wrong command!");

	if (diag_cmd_update_event_mask(req->NUM_BITS, req->MASK) == 0) {
		mask_size = EVENT_COUNT_TO_BYTES(req->NUM_BITS);
		pkt_size += mask_size;
		pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
		pkt->header = req->header;
		pkt->NUM_BITS = req->NUM_BITS;
		memcpy(pkt->MASK, req->MASK, mask_size);
		pkt->header.STATUS = EVENT_STATUS_SUCCESS;

		struct list_head *item;
		list_for_each(item, peripherals) {
			struct peripheral *perf = container_of(item, struct peripheral, node);
			diag_cntl_send_event_mask(perf);
		}
	} else {
		pkt = diag_dbg_alloc(DIAG_DBG_ROUTER, true, pkt_size, "\n");
		pkt->header = req->header;
		pkt->NUM_BITS = 0;
		pkt->header.STATUS = EVENT_STATUS_FAIL;
	}

	send_packet((uint8_t *)pkt, pkt_size, 1);
	diag_dbg_free(DIAG_DBG_ROUTER, pkt, "\n");

	return 0;
}

static int diag_router_handle_Event_Report_Control_Response(struct diag_cmd *dc, void *data)
{
	struct mbuf* mbuf = (struct mbuf*) data;
	struct Toggle_Events_Request_pkt
	{
		uint8_t CMD_CODE;
		uint8_t OPERATION_SWITCH;
	} __packed *req = (struct Toggle_Events_Request_pkt *)mbuf->data;
	struct Toggle_Events_Response_pkt
	{
		uint8_t CMD_CODE;
		uint16_t LENGTH;
	} __packed pkt;

	if (req->CMD_CODE != 0x60)
		err(errno, "Wrong command!");

	switch (req->OPERATION_SWITCH)
	{
	case DIAG_CTRL_MASK_ALL_DISABLED:
	case DIAG_CTRL_MASK_ALL_ENABLED:
		diag_cmd_toggle_events(req->OPERATION_SWITCH);
		pkt.CMD_CODE = req->CMD_CODE;
		pkt.LENGTH = 0;

		struct list_head *item;
		list_for_each(item, peripherals) {
			struct peripheral *perf = container_of(item, struct peripheral, node);
			diag_cntl_send_event_mask(perf);
		}

		send_packet((uint8_t *)&pkt, sizeof(pkt), 1);
		return 0;
	default:
		return 1;
	}
}

static int diag_router_handle_Extended_Build_ID_Request(struct diag_cmd *dc, void *data)
{		
	struct Extended_Build_ID_Resp_pkt
	{
		uint8_t CMD_CODE;
		uint8_t Ver;
		uint8_t res0;
		uint32_t msm_rev;
		uint16_t res1;
		uint32_t Mobile_Model_Number;
		char strings[10];
	} __packed pkt;
	size_t pkt_size = sizeof(pkt) - sizeof(pkt.strings);
	size_t strings_size = 0;
	char Mobile_Software_Revision[] = "OE";
	char Mobile_Model_String[] = "DB410C";

	pkt.CMD_CODE=0x7c;
	pkt.Ver=2;
	pkt.msm_rev=2;
	pkt.Mobile_Model_Number=0;
	strcpy(pkt.strings + strings_size, Mobile_Software_Revision);
	strings_size += strlen(Mobile_Software_Revision) + 1;
	strcpy(pkt.strings + strings_size, Mobile_Model_String);
	strings_size += strlen(Mobile_Model_String) + 1;
	pkt_size += strings_size;
	
	send_packet((uint8_t *)&pkt, pkt_size, 1);
	
	return 0;
}

static int diag_router_handle_rsp(struct diag_cmd *dc, void *data)
{
	size_t len;
	uint8_t *buf;
	size_t i = 0;

	if (dc == NULL) return -1;
	if (data == NULL) return -1;

	len = 16;
	buf = diag_dbg_alloc(DIAG_DBG_ROUTER, true, len, "\n");
	if (!buf)
		err(1, "failed to allocate error buffer");
	buf[i++] = 0x4b;
	buf[i++] = 0x32;
	buf[i++] = 0x03;
	memset(buf + i, 0, len - i);

	send_packet(buf, len, 1);
	diag_dbg_free(DIAG_DBG_ROUTER, buf, "\n");

	return 0;
}

int diag_router_peripheral_cmd_cb(struct diag_cmd *dc, void *data)
{
	struct mbuf *mbuf = (struct mbuf*)data;
	struct peripheral *peripheral=NULL;
	uint8_t *ptr;
	size_t len;
	size_t n;

	if (dc == NULL) return -1;
	diag_dbg(DIAG_DBG_ROUTER,"handled by %s\n", dc->peripheral->name);
	peripheral = dc->peripheral;

	if (data == NULL) return -1;
	
	ptr = (uint8_t *)mbuf->data;
	len = mbuf->offset;
	
	/* FWD the msg to the peripheral cmd queue*/
	//queue_push(&peripheral->cmdq, ptr, len);
	if (peripheral->fd[peripheral_ch_type_cmd] == -1) return -1;

	n = write(peripheral->fd[peripheral_ch_type_cmd], ptr, len);
	diag_dbg(DIAG_DBG_ROUTER, "forwarded to fd=%d %ld out of %ld\n", peripheral->fd[peripheral_ch_type_cmd], n, len);
	
	while (diag_router_handle_outgoing_packet(peripheral->fd[peripheral_ch_type_data], (void *)peripheral)) {}
	diag_dbg(DIAG_DBG_ROUTER, "read response on fd=%d\n", peripheral->fd[peripheral_ch_type_data]);

	//diag_dbg_dump(DIAG_DBG_ROUTER, "fwd : ", ptr, len);
	return 0;
	
}

static int diag_cmd_dispatch(struct mbuf *mbuf)
{
	struct list_head *item;
	struct diag_cmd *dc;
	unsigned int key;
	int handled = 0;
	uint8_t *ptr;
	
	if (mbuf == NULL)
	{
		diag_err("got an empty packet");
		return -1;
	}
	ptr = (uint8_t *)mbuf->data;
	
	if (ptr[0] == DIAG_CMD_SUBSYS_DISPATCH || ptr[0] == DIAG_CMD_SUBSYS_DISPATCH_V2) {
		key = ptr[0] << 24 | ptr[1] << 16 | ptr[3] << 8 | ptr[2];
		diag_dbg_dump(DIAG_DBG_ROUTER_DUMP, "subsys cmdid = ", ptr, 4);
	} else {
		key = 0xff << 24 | 0xff << 16 | ptr[0];
		diag_dbg_dump(DIAG_DBG_ROUTER_DUMP, "cmdid = ", ptr, 1);
	}
    
	
	if (key != 0x4b320003)
		diag_dbg(DIAG_DBG_ROUTER, "search for 0x%x \n", key);
	/* first check for common cmds that are handled by the apps router */
	list_for_each(item, &common_cmds) 
	{
		dc = container_of(item, struct diag_cmd, node);
		if (key < dc->first || key > dc->last){			
			continue;
		}

		if (!dc->cb || dc->cb(dc, mbuf)) {
			return -1;
		}
		
		handled++;
	}
	if (!handled)
	{		
		list_for_each(item, &g_dif->diag_cmds) 
		{
			dc = container_of(item, struct diag_cmd, node);
			if (key < dc->first || key > dc->last){			
				continue;
			}
			if (key !=0x4b320003)
				diag_dbg(DIAG_DBG_ROUTER,"match 0x%x by %s\n",key, dc->peripheral->name);
			if (!dc->cb || dc->cb(dc, mbuf)) {
				return -1;
			}
			
			handled++;
		}
		if (!handled)
		{
			list_for_each(item, &apps_cmds) 
			{
				dc = container_of(item, struct diag_cmd, node);
				if (key < dc->first || key > dc->last){			
					continue;
				}

				if (!dc->cb || dc->cb(dc, mbuf)) {
					return -1;
				}
				
				handled++;
			}
		}
	}		

	return handled ? 0 : -ENOENT;
}

static void diag_rsp_bad_command(struct mbuf *mbuf)
{
	uint8_t *buf;
	uint8_t *msg;
	size_t len;
	msg = (uint8_t *)mbuf->data;
	len = mbuf->offset;
	buf = diag_dbg_alloc(DIAG_DBG_ROUTER, true, len + 1, "\n");
	if (!buf)
		err(errno, "failed to allocate error buffer");

	buf[0] = 0x13;
	memcpy(buf + 1, msg, len);

	send_packet(buf, len + 1, 1);

	diag_dbg_free(DIAG_DBG_ROUTER, buf, "\n");
}

int diag_router_handle_incoming_packet(void *data)
{
	struct mbuf *mbuf = (struct mbuf*)data;
	int ret =0;
	ret = diag_cmd_dispatch(mbuf);	
	if (ret<0) 
	{
		diag_rsp_bad_command(mbuf);
	}
	return 0;
	
}

static int diag_cmds_init()
{
	struct diag_cmd *dc;

	/**********Extended_Build_ID_Request*****************/	
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff007c;
	dc->last = 0xffff007c;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Extended_Build_ID_Request;
	list_add(&apps_cmds, &dc->node);
	
	/*********Diag_version******************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff001c;
	dc->last = 0xffff001c;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Diag_version;
	list_add(&apps_cmds, &dc->node);
	
	/*********keep alive******************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0x4b320003;
	dc->last = 0x4b320003;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_rsp;
	list_add(&apps_cmds, &dc->node);
	
	/************Logging_Configuration_Response***************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff0073;
	dc->last = 0xffff0073;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Logging_Configuration_Response;
	list_add(&common_cmds, &dc->node);

	/************Extended_Message_Configuration_Response***************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff007d;
	dc->last = 0xffff007d;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Extended_Message_Configuration_Response;
	list_add(&common_cmds, &dc->node);

	/************Event_Get_Mask_Response***************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff0081;
	dc->last = 0xffff0081;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Event_Get_Mask_Response;
	list_add(&common_cmds, &dc->node);

	/************Event_Set_Mask_Response***************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff0082;
	dc->last = 0xffff0082;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Event_Set_Mask_Response;
	list_add(&common_cmds, &dc->node);

	/************Event_Report_Control_Response***************/
	dc = malloc(sizeof(*dc));
	if (!dc)
		err(1, "malloc failed");
	memset(dc, 0, sizeof(*dc));
	dc->first = 0xffff0060;
	dc->last = 0xffff0060;
	dc->peripheral = NULL;
	dc->cb=&diag_router_handle_Event_Report_Control_Response;
	list_add(&common_cmds, &dc->node);

	return 0;
}

int diag_router_init(struct diag_if *dif)
{
	struct diag_config *diag_conf=NULL;
	struct diag_router_config *rconf=NULL;
	if(dif==NULL)
		return -1;

	diag_conf = dif->conf;
	rconf=&diag_conf->router_config;
	LIST_DYNAMIC_INIT(rconf->peripherals_list);

	/* Init the peripherals according to the configuration */
	peripheral_init(&rconf->peripherals_list, diag_conf->peripherals_names);

	/* Init the masks */
	diag_masks_init();

	/* Register the cmd's that need to be handled by the router (in case no other peripheral does)*/
	diag_cmds_init();
	
	return 0;
}

int diag_router_quit(void)
{
	struct list_head *item;	
	struct diag_cmd *dc;

	diag_masks_exit();
	peripheral_exit();
	list_for_each(item, &apps_cmds) 
	{
		dc = container_of(item, struct diag_cmd, node);
		free(dc);
	}
	list_for_each(item, &g_dif->diag_cmds) 
	{
		dc = container_of(item, struct diag_cmd, node);
		free(dc);
	}
	watch_quit();	
	return 0;
}
