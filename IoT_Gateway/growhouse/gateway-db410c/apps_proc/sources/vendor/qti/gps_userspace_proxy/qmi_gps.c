/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
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

#include <errno.h>
#include <string.h>
#include "qmi_gps.h"

struct gps_loc_start_req *gps_loc_start_req_alloc(unsigned txn)
{
	return (struct gps_loc_start_req*)qmi_tlv_init(txn, 34, 0);
}

struct gps_loc_start_req *gps_loc_start_req_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_start_req*)qmi_tlv_decode(buf, len, txn, 0);
}

void *gps_loc_start_req_encode(struct gps_loc_start_req *loc_start_req, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_start_req, len);
}

void gps_loc_start_req_free(struct gps_loc_start_req *loc_start_req)
{
	qmi_tlv_free((struct qmi_tlv*)loc_start_req);
}

int gps_loc_start_req_set_session_id(struct gps_loc_start_req *loc_start_req, uint8_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 1, &val, sizeof(uint8_t));
}

int gps_loc_start_req_get_session_id(struct gps_loc_start_req *loc_start_req, uint8_t *val)
{
	uint8_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 1, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint8_t))
		return -EINVAL;

	*val = *(uint8_t*)ptr;
	return 0;
}

int gps_loc_start_req_set_fix_recurrence(struct gps_loc_start_req *loc_start_req, uint32_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 16, &val, sizeof(uint32_t));
}

int gps_loc_start_req_get_fix_recurrence(struct gps_loc_start_req *loc_start_req, uint32_t *val)
{
	uint32_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 16, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint32_t))
		return -EINVAL;

	*val = *(uint32_t*)ptr;
	return 0;
}

int gps_loc_start_req_set_horizontal_accuracy_level(struct gps_loc_start_req *loc_start_req, uint32_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 17, &val, sizeof(uint32_t));
}

int gps_loc_start_req_get_horizontal_accuracy_level(struct gps_loc_start_req *loc_start_req, uint32_t *val)
{
	uint32_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 17, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint32_t))
		return -EINVAL;

	*val = *(uint32_t*)ptr;
	return 0;
}

int gps_loc_start_req_set_intermediate_report_state(struct gps_loc_start_req *loc_start_req, uint32_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 18, &val, sizeof(uint32_t));
}

int gps_loc_start_req_get_intermediate_report_state(struct gps_loc_start_req *loc_start_req, uint32_t *val)
{
	uint32_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 18, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint32_t))
		return -EINVAL;

	*val = *(uint32_t*)ptr;
	return 0;
}

int gps_loc_start_req_set_min_interval(struct gps_loc_start_req *loc_start_req, uint32_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 19, &val, sizeof(uint32_t));
}

int gps_loc_start_req_get_min_interval(struct gps_loc_start_req *loc_start_req, uint32_t *val)
{
	uint32_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 19, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint32_t))
		return -EINVAL;

	*val = *(uint32_t*)ptr;
	return 0;
}

int gps_loc_start_req_set_application_id(struct gps_loc_start_req *loc_start_req, struct gps_loc_application_id *val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 20, val, sizeof(struct gps_loc_application_id));
}

struct gps_loc_application_id *gps_loc_start_req_get_application_id(struct gps_loc_start_req *loc_start_req)
{
	size_t len;
	void *ptr;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 20, &len);
	if (!ptr)
		return NULL;

	if (len != sizeof(struct gps_loc_application_id))
		return NULL;

	return ptr;
}

int gps_loc_start_req_set_config_altitude_assumed(struct gps_loc_start_req *loc_start_req, uint32_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 21, &val, sizeof(uint32_t));
}

int gps_loc_start_req_get_config_altitude_assumed(struct gps_loc_start_req *loc_start_req, uint32_t *val)
{
	uint32_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 21, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint32_t))
		return -EINVAL;

	*val = *(uint32_t*)ptr;
	return 0;
}

int gps_loc_start_req_set_min_intermediate_position_report_interval(struct gps_loc_start_req *loc_start_req, uint32_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_req, 22, &val, sizeof(uint32_t));
}

int gps_loc_start_req_get_min_intermediate_position_report_interval(struct gps_loc_start_req *loc_start_req, uint32_t *val)
{
	uint32_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_req, 22, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint32_t))
		return -EINVAL;

	*val = *(uint32_t*)ptr;
	return 0;
}

struct gps_loc_start_resp *gps_loc_start_resp_alloc(unsigned txn)
{
	return (struct gps_loc_start_resp*)qmi_tlv_init(txn, 34, 2);
}

struct gps_loc_start_resp *gps_loc_start_resp_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_start_resp*)qmi_tlv_decode(buf, len, txn, 2);
}

void *gps_loc_start_resp_encode(struct gps_loc_start_resp *loc_start_resp, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_start_resp, len);
}

void gps_loc_start_resp_free(struct gps_loc_start_resp *loc_start_resp)
{
	qmi_tlv_free((struct qmi_tlv*)loc_start_resp);
}

int gps_loc_start_resp_set_result(struct gps_loc_start_resp *loc_start_resp, struct gps_qmi_result *val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_start_resp, 2, val, sizeof(struct gps_qmi_result));
}

struct gps_qmi_result *gps_loc_start_resp_get_result(struct gps_loc_start_resp *loc_start_resp)
{
	size_t len;
	void *ptr;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_start_resp, 2, &len);
	if (!ptr)
		return NULL;

	if (len != sizeof(struct gps_qmi_result))
		return NULL;

	return ptr;
}

struct gps_loc_stop_req *gps_loc_stop_req_alloc(unsigned txn)
{
	return (struct gps_loc_stop_req*)qmi_tlv_init(txn, 35, 0);
}

struct gps_loc_stop_req *gps_loc_stop_req_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_stop_req*)qmi_tlv_decode(buf, len, txn, 0);
}

void *gps_loc_stop_req_encode(struct gps_loc_stop_req *loc_stop_req, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_stop_req, len);
}

void gps_loc_stop_req_free(struct gps_loc_stop_req *loc_stop_req)
{
	qmi_tlv_free((struct qmi_tlv*)loc_stop_req);
}

int gps_loc_stop_req_set_session_id(struct gps_loc_stop_req *loc_stop_req, uint8_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_stop_req, 1, &val, sizeof(uint8_t));
}

int gps_loc_stop_req_get_session_id(struct gps_loc_stop_req *loc_stop_req, uint8_t *val)
{
	uint8_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_stop_req, 1, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint8_t))
		return -EINVAL;

	*val = *(uint8_t*)ptr;
	return 0;
}

struct gps_loc_stop_resp *gps_loc_stop_resp_alloc(unsigned txn)
{
	return (struct gps_loc_stop_resp*)qmi_tlv_init(txn, 35, 2);
}

struct gps_loc_stop_resp *gps_loc_stop_resp_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_stop_resp*)qmi_tlv_decode(buf, len, txn, 2);
}

void *gps_loc_stop_resp_encode(struct gps_loc_stop_resp *loc_stop_resp, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_stop_resp, len);
}

void gps_loc_stop_resp_free(struct gps_loc_stop_resp *loc_stop_resp)
{
	qmi_tlv_free((struct qmi_tlv*)loc_stop_resp);
}

int gps_loc_stop_resp_set_result(struct gps_loc_stop_resp *loc_stop_resp, struct gps_qmi_result *val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_stop_resp, 2, val, sizeof(struct gps_qmi_result));
}

struct gps_qmi_result *gps_loc_stop_resp_get_result(struct gps_loc_stop_resp *loc_stop_resp)
{
	size_t len;
	void *ptr;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_stop_resp, 2, &len);
	if (!ptr)
		return NULL;

	if (len != sizeof(struct gps_qmi_result))
		return NULL;

	return ptr;
}

struct gps_loc_reg_events_req *gps_loc_reg_events_req_alloc(unsigned txn)
{
	return (struct gps_loc_reg_events_req*)qmi_tlv_init(txn, 33, 0);
}

struct gps_loc_reg_events_req *gps_loc_reg_events_req_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_reg_events_req*)qmi_tlv_decode(buf, len, txn, 0);
}

void *gps_loc_reg_events_req_encode(struct gps_loc_reg_events_req *loc_reg_events_req, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_reg_events_req, len);
}

void gps_loc_reg_events_req_free(struct gps_loc_reg_events_req *loc_reg_events_req)
{
	qmi_tlv_free((struct qmi_tlv*)loc_reg_events_req);
}

int gps_loc_reg_events_req_set_event_reg_mask(struct gps_loc_reg_events_req *loc_reg_events_req, uint64_t val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_reg_events_req, 1, &val, sizeof(uint64_t));
}

int gps_loc_reg_events_req_get_event_reg_mask(struct gps_loc_reg_events_req *loc_reg_events_req, uint64_t *val)
{
	uint64_t *ptr;
	size_t len;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_reg_events_req, 1, &len);
	if (!ptr)
		return -ENOENT;

	if (len != sizeof(uint64_t))
		return -EINVAL;

	*val = *(uint64_t*)ptr;
	return 0;
}

struct gps_loc_reg_events_resp *gps_loc_reg_events_resp_alloc(unsigned txn)
{
	return (struct gps_loc_reg_events_resp*)qmi_tlv_init(txn, 33, 2);
}

struct gps_loc_reg_events_resp *gps_loc_reg_events_resp_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_reg_events_resp*)qmi_tlv_decode(buf, len, txn, 2);
}

void *gps_loc_reg_events_resp_encode(struct gps_loc_reg_events_resp *loc_reg_events_resp, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_reg_events_resp, len);
}

void gps_loc_reg_events_resp_free(struct gps_loc_reg_events_resp *loc_reg_events_resp)
{
	qmi_tlv_free((struct qmi_tlv*)loc_reg_events_resp);
}

int gps_loc_reg_events_resp_set_result(struct gps_loc_reg_events_resp *loc_reg_events_resp, struct gps_qmi_result *val)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_reg_events_resp, 2, val, sizeof(struct gps_qmi_result));
}

struct gps_qmi_result *gps_loc_reg_events_resp_get_result(struct gps_loc_reg_events_resp *loc_reg_events_resp)
{
	size_t len;
	void *ptr;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_reg_events_resp, 2, &len);
	if (!ptr)
		return NULL;

	if (len != sizeof(struct gps_qmi_result))
		return NULL;

	return ptr;
}

struct gps_loc_event_nmea_ind *gps_loc_event_nmea_ind_alloc(unsigned txn)
{
	return (struct gps_loc_event_nmea_ind*)qmi_tlv_init(txn, 38, 4);
}

struct gps_loc_event_nmea_ind *gps_loc_event_nmea_ind_parse(void *buf, size_t len, unsigned *txn)
{
	return (struct gps_loc_event_nmea_ind*)qmi_tlv_decode(buf, len, txn, 4);
}

void *gps_loc_event_nmea_ind_encode(struct gps_loc_event_nmea_ind *loc_event_nmea_ind, size_t *len)
{
	return qmi_tlv_encode((struct qmi_tlv*)loc_event_nmea_ind, len);
}

void gps_loc_event_nmea_ind_free(struct gps_loc_event_nmea_ind *loc_event_nmea_ind)
{
	qmi_tlv_free((struct qmi_tlv*)loc_event_nmea_ind);
}

int gps_loc_event_nmea_ind_set_nmea(struct gps_loc_event_nmea_ind *loc_event_nmea_ind, char *buf, size_t len)
{
	return qmi_tlv_set((struct qmi_tlv*)loc_event_nmea_ind, 1, buf, len);
}

int gps_loc_event_nmea_ind_get_nmea(struct gps_loc_event_nmea_ind *loc_event_nmea_ind, char *buf, size_t buflen)
{
	size_t len;
	char *ptr;

	ptr = qmi_tlv_get((struct qmi_tlv*)loc_event_nmea_ind, 1, &len);
	if (!ptr)
		return -ENOENT;

	if (len >= buflen)
		return -ENOMEM;

	memcpy(buf, ptr, len);
	buf[len] = '\0';
	return len;
}

