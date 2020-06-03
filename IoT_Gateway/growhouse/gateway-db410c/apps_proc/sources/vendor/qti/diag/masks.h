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
#ifndef MASKS_H_
#define MASKS_H_

#include "qdiag.h"


#define MSG_MASKS_TYPE		0x00000001
#define LOG_MASKS_TYPE		0x00000002
#define EVENT_MASKS_TYPE	0x00000004

#define MSG_MASK_TBL_CNT		25
#define APPS_EVENT_LAST_ID		0x0B3F

#define MAX_SSID_PER_RANGE	200

#define __packed __attribute__ ((packed))

extern uint16_t event_max_num_bits;
struct diag_log_mask_t {
	uint8_t equip_id;
	uint32_t num_items;
	uint32_t num_items_tools;
	uint32_t range;
	uint32_t range_tools;
	uint8_t *ptr;
}__packed;

struct diag_ssid_range_t {
	uint16_t ssid_first;
	uint16_t ssid_last;
}__packed;

struct diag_msg_mask_t {
	uint32_t ssid_first;
	uint32_t ssid_last;
	uint32_t ssid_last_tools;
	uint32_t range;
	uint32_t range_tools;
	uint32_t *ptr;
}__packed;

struct diag_mask_info {
	uint8_t *ptr;
	int mask_len;
	uint8_t status;
}__packed;

#define MSG_MASK_SIZE	(MSG_MASK_TBL_CNT * sizeof(struct diag_msg_mask_t))
#define LOG_MASK_SIZE	(MAX_EQUIP_ID * sizeof(struct diag_log_mask_t))

struct diag_mask_info msg_mask;
struct diag_mask_info msg_bt_mask;
struct diag_mask_info log_mask;
struct diag_mask_info event_mask;

int diag_masks_init(void);
void diag_masks_exit(void);

inline uint8_t diag_get_log_mask_status() { return log_mask.status; }
void diag_cmd_disable_log();
void diag_cmd_get_log_range(uint32_t *ranges, uint32_t count);
int diag_cmd_set_log_mask(uint8_t equip_id, uint32_t *num_items, uint8_t *mask, uint32_t *mask_size);
int diag_cmd_get_log_mask(uint32_t equip_id, uint32_t *num_items, uint8_t ** mask, uint32_t *mask_size);

inline uint8_t diag_get_build_mask_status() { return msg_bt_mask.status; }
void diag_cmd_get_ssid_range(uint32_t *count, struct diag_ssid_range_t **ranges);
int diag_cmd_get_build_mask(struct diag_ssid_range_t *range, uint32_t **mask);

inline uint8_t diag_get_msg_mask_status() { return msg_mask.status; }
int diag_cmd_get_msg_mask(struct diag_ssid_range_t *range, uint32_t **mask);
int diag_cmd_set_msg_mask(struct diag_ssid_range_t *range, uint32_t *mask);
void diag_cmd_set_all_msg_mask(uint32_t mask);

inline uint8_t diag_get_event_mask_status() { return event_mask.status; }
int diag_cmd_get_event_mask(uint16_t num_bits, uint8_t ** mask);
int diag_cmd_update_event_mask(uint16_t num_bits, uint8_t *mask);
void diag_cmd_toggle_events(uint8_t operation);

#endif /* MASKS_H_ */
