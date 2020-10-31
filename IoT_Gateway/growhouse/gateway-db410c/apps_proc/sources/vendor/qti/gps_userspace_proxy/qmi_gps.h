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

#ifndef __QMI_GPS_H__
#define __QMI_GPS_H__

#include <stdint.h>
#include <stdlib.h>

struct qmi_tlv;

struct qmi_tlv *qmi_tlv_init(unsigned txn, unsigned msg_id, unsigned type);
struct qmi_tlv *qmi_tlv_decode(void *buf, size_t len, unsigned *txn, unsigned type);
void *qmi_tlv_encode(struct qmi_tlv *tlv, size_t *len);
void qmi_tlv_free(struct qmi_tlv *tlv);

void *qmi_tlv_get(struct qmi_tlv *tlv, unsigned id, size_t *len);
void *qmi_tlv_get_array(struct qmi_tlv *tlv, unsigned id, unsigned len_size, size_t *len, size_t *size);
int qmi_tlv_set(struct qmi_tlv *tlv, unsigned id, void *buf, size_t len);
int qmi_tlv_set_array(struct qmi_tlv *tlv, unsigned id, unsigned len_size, void *buf, size_t len, size_t size);

#define QMI_GPS_RESULT_SUCCESS 0
#define QMI_GPS_RESULT_FAILURE 1
#define QMI_GPS_ERR_NONE 0
#define QMI_GPS_ERR_INTERNAL 3
#define QMI_GPS_ERR_MALFORMED_MSG 1
#define QMI_GPS_QMI_ERR_NO_MEMORY 2
#define QMI_GPS_QMI_ERR_INVALID_HANDLE 9
#define QMI_GPS_LOC_START 34
#define QMI_GPS_LOC_STOP 35
#define QMI_GPS_LOC_REG_EVENTS 33
#define QMI_GPS_LOC_NMEA_IND 38
#define QMI_LOC_MAX_APP_ID_NAME_LENGTH_V02 32
#define QMI_LOC_MAX_APP_ID_PROVIDER_LENGTH_V02 24
#define QMI_LOC_MAX_APP_ID_VERSION_LENGTH_V02 8

struct gps_qmi_result {
	uint16_t result;
	uint16_t error;
};

struct gps_loc_application_id {
	char application_provider[QMI_LOC_MAX_APP_ID_PROVIDER_LENGTH_V02+1];
	char application_name[QMI_LOC_MAX_APP_ID_NAME_LENGTH_V02];
	uint8_t application_version_valid;
	char application_version[QMI_LOC_MAX_APP_ID_VERSION_LENGTH_V02];
};

struct gps_loc_start_req;
struct gps_loc_start_resp;
struct gps_loc_stop_req;
struct gps_loc_stop_resp;
struct gps_loc_reg_events_req;
struct gps_loc_reg_events_resp;
struct gps_loc_event_nmea_ind;

/*
 * gps_loc_start_req message
 */
struct gps_loc_start_req *gps_loc_start_req_alloc(unsigned txn);
struct gps_loc_start_req *gps_loc_start_req_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_start_req_encode(struct gps_loc_start_req *loc_start_req, size_t *len);
void gps_loc_start_req_free(struct gps_loc_start_req *loc_start_req);

int gps_loc_start_req_set_session_id(struct gps_loc_start_req *loc_start_req, uint8_t val);
int gps_loc_start_req_get_session_id(struct gps_loc_start_req *loc_start_req, uint8_t *val);

int gps_loc_start_req_set_fix_recurrence(struct gps_loc_start_req *loc_start_req, uint32_t val);
int gps_loc_start_req_get_fix_recurrence(struct gps_loc_start_req *loc_start_req, uint32_t *val);

int gps_loc_start_req_set_horizontal_accuracy_level(struct gps_loc_start_req *loc_start_req, uint32_t val);
int gps_loc_start_req_get_horizontal_accuracy_level(struct gps_loc_start_req *loc_start_req, uint32_t *val);

int gps_loc_start_req_set_intermediate_report_state(struct gps_loc_start_req *loc_start_req, uint32_t val);
int gps_loc_start_req_get_intermediate_report_state(struct gps_loc_start_req *loc_start_req, uint32_t *val);

int gps_loc_start_req_set_min_interval(struct gps_loc_start_req *loc_start_req, uint32_t val);
int gps_loc_start_req_get_min_interval(struct gps_loc_start_req *loc_start_req, uint32_t *val);

int gps_loc_start_req_set_application_id(struct gps_loc_start_req *loc_start_req, struct gps_loc_application_id *val);
struct gps_loc_application_id *gps_loc_start_req_get_application_id(struct gps_loc_start_req *loc_start_req);

int gps_loc_start_req_set_config_altitude_assumed(struct gps_loc_start_req *loc_start_req, uint32_t val);
int gps_loc_start_req_get_config_altitude_assumed(struct gps_loc_start_req *loc_start_req, uint32_t *val);

int gps_loc_start_req_set_min_intermediate_position_report_interval(struct gps_loc_start_req *loc_start_req, uint32_t val);
int gps_loc_start_req_get_min_intermediate_position_report_interval(struct gps_loc_start_req *loc_start_req, uint32_t *val);

/*
 * gps_loc_start_resp message
 */
struct gps_loc_start_resp *gps_loc_start_resp_alloc(unsigned txn);
struct gps_loc_start_resp *gps_loc_start_resp_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_start_resp_encode(struct gps_loc_start_resp *loc_start_resp, size_t *len);
void gps_loc_start_resp_free(struct gps_loc_start_resp *loc_start_resp);

int gps_loc_start_resp_set_result(struct gps_loc_start_resp *loc_start_resp, struct gps_qmi_result *val);
struct gps_qmi_result *gps_loc_start_resp_get_result(struct gps_loc_start_resp *loc_start_resp);

/*
 * gps_loc_stop_req message
 */
struct gps_loc_stop_req *gps_loc_stop_req_alloc(unsigned txn);
struct gps_loc_stop_req *gps_loc_stop_req_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_stop_req_encode(struct gps_loc_stop_req *loc_stop_req, size_t *len);
void gps_loc_stop_req_free(struct gps_loc_stop_req *loc_stop_req);

int gps_loc_stop_req_set_session_id(struct gps_loc_stop_req *loc_stop_req, uint8_t val);
int gps_loc_stop_req_get_session_id(struct gps_loc_stop_req *loc_stop_req, uint8_t *val);

/*
 * gps_loc_stop_resp message
 */
struct gps_loc_stop_resp *gps_loc_stop_resp_alloc(unsigned txn);
struct gps_loc_stop_resp *gps_loc_stop_resp_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_stop_resp_encode(struct gps_loc_stop_resp *loc_stop_resp, size_t *len);
void gps_loc_stop_resp_free(struct gps_loc_stop_resp *loc_stop_resp);

int gps_loc_stop_resp_set_result(struct gps_loc_stop_resp *loc_stop_resp, struct gps_qmi_result *val);
struct gps_qmi_result *gps_loc_stop_resp_get_result(struct gps_loc_stop_resp *loc_stop_resp);

/*
 * gps_loc_reg_events_req message
 */
struct gps_loc_reg_events_req *gps_loc_reg_events_req_alloc(unsigned txn);
struct gps_loc_reg_events_req *gps_loc_reg_events_req_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_reg_events_req_encode(struct gps_loc_reg_events_req *loc_reg_events_req, size_t *len);
void gps_loc_reg_events_req_free(struct gps_loc_reg_events_req *loc_reg_events_req);

int gps_loc_reg_events_req_set_event_reg_mask(struct gps_loc_reg_events_req *loc_reg_events_req, uint64_t val);
int gps_loc_reg_events_req_get_event_reg_mask(struct gps_loc_reg_events_req *loc_reg_events_req, uint64_t *val);

/*
 * gps_loc_reg_events_resp message
 */
struct gps_loc_reg_events_resp *gps_loc_reg_events_resp_alloc(unsigned txn);
struct gps_loc_reg_events_resp *gps_loc_reg_events_resp_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_reg_events_resp_encode(struct gps_loc_reg_events_resp *loc_reg_events_resp, size_t *len);
void gps_loc_reg_events_resp_free(struct gps_loc_reg_events_resp *loc_reg_events_resp);

int gps_loc_reg_events_resp_set_result(struct gps_loc_reg_events_resp *loc_reg_events_resp, struct gps_qmi_result *val);
struct gps_qmi_result *gps_loc_reg_events_resp_get_result(struct gps_loc_reg_events_resp *loc_reg_events_resp);

/*
 * gps_loc_event_nmea_ind message
 */
struct gps_loc_event_nmea_ind *gps_loc_event_nmea_ind_alloc(unsigned txn);
struct gps_loc_event_nmea_ind *gps_loc_event_nmea_ind_parse(void *buf, size_t len, unsigned *txn);
void *gps_loc_event_nmea_ind_encode(struct gps_loc_event_nmea_ind *loc_event_nmea_ind, size_t *len);
void gps_loc_event_nmea_ind_free(struct gps_loc_event_nmea_ind *loc_event_nmea_ind);

int gps_loc_event_nmea_ind_set_nmea(struct gps_loc_event_nmea_ind *loc_event_nmea_ind, char *buf, size_t len);
int gps_loc_event_nmea_ind_get_nmea(struct gps_loc_event_nmea_ind *loc_event_nmea_ind, char *buf, size_t buflen);

#endif
