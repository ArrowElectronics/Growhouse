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

#define _GNU_SOURCE /* posix pseudo tty */

#include <sys/mman.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <libqrtr.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "gps_proxy.h"
#include "qmi_gps.h"
#include "util.h"

#define QMI_LOC_SERVICE			0x10
#define QMI_LOC_VERSION			0x02
#define QMI_LOC_INSTANCE		0x00

#define QMI_LOC_EVENT_MASK_NMEA_V02 	((uint64_t)0x00000004ull)
#define MAX_BUF_SZ 			512

#define MAX_ATTEMPTS_CONNECT_TO_QRTR 5
#define QRTR_CONNECTION_ATTEMPTS_INTERVAL 10
#define MAX_SLAVE_DEV_NAME_SIZE 216
#define MAX_COMMAND_SIZE 32
#define CHECK_IF_ACTIVE_TIME 1

#define GPS_DEVICE_NAME "/dev/ttyGPS"

#define STOP_SIG SIGUSR1
#define START_SIG SIGUSR2

#define RD_WR_ALL_PERM 0666

typedef enum {
	NEED_SEND = 1,
	NEED_RECV
} op_mask_t;

struct qmi_packet {
	uint8_t flags;
	uint16_t txn_id;
	uint16_t msg_id;
	uint16_t msg_len;
	uint8_t data[];
} __attribute__((__packed__));

struct sockaddr_qrtr {
	unsigned short sq_family;
	uint32_t sq_node;
	uint32_t sq_port;
};

static unsigned g_txn = 1;
static uint8_t g_session_id = 0;
static uint32_t g_loc_node = -1;
static uint32_t g_loc_port = -1;
static bool g_dbgprintf_enabled;
static bool g_is_active = true;

static void dbgprintf(const char *fmt, ...)
{
	va_list ap;

	if (!g_dbgprintf_enabled)
		return;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

static int qrtr_send_recv(int sock, unsigned node, unsigned port, void *ptr, size_t len, char *buf,
		 	size_t buf_len, unsigned *in_node, unsigned *in_port,
			struct qmi_packet **qmi, int op_mask)
{
	int ret = 0;
	if (NEED_SEND & op_mask) {
		ret = qrtr_sendto(sock, node, port, ptr, len);
		if (ret < 0) {
			fprintf(stderr, "[GPS] failed to send request: %s\n", strerror(-ret));
			ret = -EINVAL;
		}
	}

	if (NEED_RECV & op_mask) {
		/* wait for response */
		ret = qrtr_recvfrom(sock, buf, buf_len, in_node, in_port);
		if (ret < 0) {
			fprintf(stderr, "[GPS] failed to receive response: %s\n", strerror(-ret));
			ret = -EINVAL;
		}
		
		*qmi = (struct qmi_packet*)buf;
		if ((*qmi)->msg_len != ret - sizeof(struct qmi_packet)) {
			fprintf(stderr, "[GPS] Invalid length of incoming qmi request\n");
			ret = -EINVAL;
		}
	}

	return 0;
}

static int check_result(struct gps_qmi_result *result)
{
	if (!result) {
		fprintf(stderr, "[GPS] Can't get response result from message\n");
		return -EINVAL;
	}

	if (result->result != QMI_GPS_RESULT_SUCCESS) {
		fprintf(stderr, "[GPS] Stop Response error: %d, %d\n",
			(int)result->result, (int)result->error);
		return -EINVAL;
	}

	++g_txn;

	return 0;
}

static int gps_send_start(int sock, unsigned node, unsigned port)
{
	int ret = 0;
	struct gps_qmi_result *result;
	struct gps_loc_start_req *req;
	struct gps_loc_start_resp *resp;
	size_t len;
	void *ptr;
	unsigned in_node, in_port;
	char buf[MAX_BUF_SZ];
	struct qmi_packet *qmi;
	unsigned txn;

	dbgprintf("[GPS] send LOC_START to (%d:%d)\n", node, port);

	req = gps_loc_start_req_alloc(g_txn);
	gps_loc_start_req_set_session_id(req, g_session_id);
	ptr = gps_loc_start_req_encode(req, &len);
	if (!ptr)
		goto free_req;

	ret = qrtr_send_recv(sock, node, port, ptr, len, buf, sizeof(buf), 
				&in_node, &in_port, &qmi, NEED_SEND | NEED_RECV);
	if (!ret)
		goto free_req;

	/* check response status */
	if (QMI_GPS_LOC_START == qmi->msg_id) {
		resp = gps_loc_start_resp_parse(qmi, qmi->msg_len, &txn);
		if ((!resp) || (txn != g_txn)) {
			fprintf(stderr, "[GPS] Invalid response message\n");
			ret = -EINVAL;
			goto free_req;
		}

		result = gps_loc_start_resp_get_result(resp);
		ret = check_result(result);
		if (ret)
			goto free_req;
	}

	else {
		fprintf(stderr, "[GPS] Unknown message: id=%d size=%d\n", qmi->msg_id, qmi->msg_len);
		ret = -EINVAL;
	}

free_req:
	gps_loc_start_req_free(req);

	return ret;
}

static int gps_send_stop(int sock, unsigned node, unsigned port)
{
	int ret = 0;
	struct gps_qmi_result *result;
	struct gps_loc_stop_req *req;
	struct gps_loc_stop_resp *resp;
	void *ptr;
	unsigned in_node, in_port;
	char buf[MAX_BUF_SZ];
	struct qmi_packet *qmi;
	unsigned txn;
	int stop = 0;
	size_t len;

	dbgprintf("[GPS] send LOC_STOP to (%d:%d)\n", node, port);

	req = gps_loc_stop_req_alloc(g_txn);
	gps_loc_stop_req_set_session_id(req, g_session_id);
	ptr = gps_loc_stop_req_encode(req, &len);
	if (!ptr)
		goto free_req;

	ret = qrtr_send_recv(sock, node, port, ptr, len, buf, sizeof(buf),
				&in_node, &in_port, &qmi, NEED_SEND);
	if (!ret)
		goto free_req;

	while (!stop) {
		ret = qrtr_send_recv(sock, node, port, ptr, len, buf, sizeof(buf),
					&in_node, &in_port, &qmi, NEED_RECV);
		if (!ret)
			goto free_req;

		/* check response status */
		switch (qmi->msg_id) {
		case QMI_GPS_LOC_STOP:
			resp = gps_loc_stop_resp_parse(qmi, qmi->msg_len, &txn);
			if ((!resp) || (txn != g_txn)) {
				fprintf(stderr, "[GPS] Invalid response message\n");
				ret = -EINVAL;
				goto free_req;
			}

			result = gps_loc_stop_resp_get_result(resp);
			ret = check_result(result);
			if (ret)
				goto free_req;
			stop = 1;

			break;
		case QMI_GPS_LOC_NMEA_IND:
			break;

		default:
			fprintf(stderr, "[GPS] Unknown message: id=%d size=%d\n", qmi->msg_id, qmi->msg_len);
			ret = -EINVAL;
			stop = 1;
			break;
		}
	}

free_req:
	gps_loc_stop_req_free(req);

	return ret;
}

static int gps_send_register_nmea_event(int sock, unsigned node, unsigned port)
{
	int ret = 0;
	struct gps_qmi_result *result;
	struct gps_loc_reg_events_req *req;
	struct gps_loc_reg_events_resp *resp;
	size_t len;
	void *ptr;
	unsigned in_node, in_port;
	char buf[MAX_BUF_SZ];
	struct qmi_packet *qmi;
	unsigned txn;

	dbgprintf("[GPS] send LOC_REG_EVENT to (%d:%d)\n", node, port);

	req = gps_loc_reg_events_req_alloc(g_txn);
	gps_loc_reg_events_req_set_event_reg_mask(req, QMI_LOC_EVENT_MASK_NMEA_V02);
	ptr = gps_loc_reg_events_req_encode(req, &len);
	if (!ptr)
		goto free_req;

	ret = qrtr_send_recv(sock, node, port, ptr, len, buf, sizeof(buf),
				&in_node, &in_port, &qmi, NEED_SEND | NEED_RECV);
	if (!ret)
		goto free_req;

	/* check response status */
	if (QMI_GPS_LOC_REG_EVENTS == qmi->msg_id) {
		resp = gps_loc_reg_events_resp_parse(qmi, qmi->msg_len, &txn);
		if ((!resp) || (txn != g_txn)) {
			fprintf(stderr, "[GPS] Invalid response message\n");
			ret = -EINVAL;
			goto free_req;
		}

		result = gps_loc_reg_events_resp_get_result(resp);
		ret = check_result(result);
		if (ret)
			goto free_req;
	}

	else {
		fprintf(stderr, "[GPS] Unknown message: id=%d size=%d\n", qmi->msg_id, qmi->msg_len);
		ret = -EINVAL;
	}

free_req:
	gps_loc_reg_events_req_free(req);

	return ret;
}

static void loc_service_cb(void *udata, uint32_t service, uint32_t instance, uint32_t node, uint32_t port)
{
	dbgprintf("[GPS] New LOC service found at (%u:%u)\n", node, port);
	g_loc_node = node;
	g_loc_port = port;	
}

static int handle_gps_ind(int sock,int dev)
{
	struct sockaddr_qrtr sq;
	socklen_t sl;
	char buf[MAX_BUF_SZ];
	int ret, len;
	unsigned txn;
	struct gps_loc_event_nmea_ind *req;
	struct gps_proxy_data nmea_data;

	sl = sizeof(sq);
	len = ret = recvfrom(sock, buf, sizeof(buf), 0, (void *)&sq, &sl);
	if (ret < 0) {
		perror("[GPS] recvfrom failed");
		return ret;
	}

	req = gps_loc_event_nmea_ind_parse(buf, len, &txn);
	if (req) {
		if (0 < gps_loc_event_nmea_ind_get_nmea(req, nmea_data.nmea_string, sizeof(nmea_data.nmea_string))) {
			nmea_data.nmea_length = strnlen(nmea_data.nmea_string, sizeof(nmea_data.nmea_string)-1) +1;
			ret = write(dev, nmea_data.nmea_string, strlen(nmea_data.nmea_string) + 1);
			if (ret < 0) {
				perror("[GPS] can't post nmea string");
			}

		}
	}

	dbgprintf("[GPS] packet; from: %d:%d\n", sq.sq_node, sq.sq_port);
	if (g_dbgprintf_enabled)
		print_hex_dump("[GPS <-]", buf, len);

	return (ret < 0) ? ret : 0;
}

static int pty_master_init(char* pty_slave_name)
{
	int fd, fail_val = -1;

	fd = posix_openpt(O_RDWR | O_NOCTTY);
	if (fd < 0) {
		perror("Failed to open pty master");
		return fail_val;
	}

	if (grantpt(fd)) {
		perror("Failed to grant pty");
		return fail_val;
	}

	if (unlockpt(fd)) {
		perror("Failed to unlock pty");
		return fail_val;
	}

	if (ptsname_r(fd, pty_slave_name, MAX_SLAVE_DEV_NAME_SIZE)) {
		perror("Failed to get slave device name");
		return fail_val;
	}

	if (chmod(pty_slave_name, RD_WR_ALL_PERM))
		perror("Failed to change slave device permission");

	if (unlink(GPS_DEVICE_NAME) && ENOENT != errno) {
		perror("Failed to remove yhr old GPS device");
		return fail_val;
	}

	if (symlink(pty_slave_name, GPS_DEVICE_NAME)) {
		perror("Failed to rename slave device");
		return fail_val;
	}

	return fd;
}

static int qrtr_init(void)
{
	int ret, gps_fd, fail_val = -1;
	int attempt_to_connect_to_qrtr = MAX_ATTEMPTS_CONNECT_TO_QRTR;

	gps_fd = qrtr_open(0);
	if (gps_fd < 0) {
		perror("failed to create qrtr socket");
		return fail_val;
	}

	/* perform several atempts to connect to modem */
	while (attempt_to_connect_to_qrtr--) {
		ret = qrtr_lookup(gps_fd, QMI_LOC_SERVICE, QMI_LOC_VERSION,
					QMI_LOC_INSTANCE, 0xFFFFFFFF, loc_service_cb, NULL);

		if (ret < 0 || g_loc_node == -1 || g_loc_port == -1) {
			fprintf(stderr, "failed to lookup LOC service\n");

			if (attempt_to_connect_to_qrtr)
				sleep(QRTR_CONNECTION_ATTEMPTS_INTERVAL);
		}
		else
			break;
	}

	if (attempt_to_connect_to_qrtr < 0) {
		fprintf(stderr, "failed to lookup LOC service for %d times with %d seconds interval\n", MAX_ATTEMPTS_CONNECT_TO_QRTR, QRTR_CONNECTION_ATTEMPTS_INTERVAL);
		return fail_val;
	}

	else {
		fprintf(stdout, "GPS is activated\n");
		fflush(stdout);
	}

	return gps_fd;
}

static int sig_handler_init(void)
{
	int sig_fd, fail_val = -1;
	sigset_t mask;

	sigemptyset(&mask);
	sigaddset(&mask, STOP_SIG);
	sigaddset(&mask, START_SIG);

	if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		perror("sigprocmask failed");
		return fail_val;
	}

	sig_fd = signalfd(-1, &mask, 0);
	if (sig_fd < 0) {
		perror("signalfd failed");
		return fail_val;
	}

	return sig_fd;
}

static int gps_start(int gps_fd)
{
	int ret, fail_val = -1;

	ret = gps_send_register_nmea_event(gps_fd,g_loc_node,g_loc_port);
	if (ret < 0) {
		fprintf(stderr, "failed to send LOC_REG_EVENTS\n");
		return fail_val;
	}

	ret = gps_send_start(gps_fd,g_loc_node,g_loc_port);
	if (ret < 0) {
		fprintf(stderr, "failed to send LOC_START\n");
		return fail_val;
	}

	return 0;
}

static int gps_stop(int gps_fd)
{
	int ret, fail_val = -1;

	ret = gps_send_stop(gps_fd, g_loc_node, g_loc_port);
	if (ret < 0) {
		fprintf(stderr, "failed to send LOC_STOP\n");
		return fail_val;
	}

	return 0;
}

static void nmea_disable(void)
{
	g_is_active = false;
}

static void nmea_enable(void)
{
	g_is_active = true;
}

static int check_if_signal(int socket, int signal)
{
	struct signalfd_siginfo sig_info;
	ssize_t res;
	int fail_val = 0;

	res = read(socket, &sig_info, sizeof(sig_info));

	if (res < 0 || res != sizeof(sig_info)) {
		perror("sig_fd read failed");
		return fail_val;
	}

	return sig_info.ssi_signo == signal;
}

static int check_if_stop(int socket)
{
	return check_if_signal(socket, STOP_SIG);
}

static int check_if_start(int socket)
{
	return check_if_signal(socket, START_SIG);
}

static void wait_for_start(int socket)
{
	while (true) {
		if (check_if_start(socket)) {
			nmea_enable();
			break;
		}

	}
}

static void transit_gps_data(int source_fd, int destination_fd, int sig_fd)
{
	fd_set rfds;
	int nfds;
	int ret;

	nfds = MAX(source_fd, sig_fd) + 1;

	while (g_is_active) {
		FD_ZERO(&rfds);
		FD_SET(source_fd, &rfds);
		FD_SET(sig_fd, &rfds);

		ret = select(nfds, &rfds, NULL, NULL, NULL);
		if (ret < 0) {
			perror("select failed");
			break;
		} else if (ret == 0) {
			continue;
		}

		if (FD_ISSET(sig_fd, &rfds) && check_if_stop(sig_fd)) {
			nmea_disable();
			break;
		}

		if (FD_ISSET(source_fd, &rfds))
			handle_gps_ind(source_fd, destination_fd);
	}
}

int main(int argc, char **argv)
{
	int gps_fd, pty_m_fd, sig_fd;
	int ret;
	char pty_slave_name[MAX_SLAVE_DEV_NAME_SIZE];

	if (argc == 2 && strcmp(argv[1], "-v") == 0)
		g_dbgprintf_enabled = true;

	sig_fd = sig_handler_init();
	if (sig_fd < 0)
		goto clean_signal;

	pty_m_fd = pty_master_init(pty_slave_name);
	if (pty_m_fd < 0)
		goto clean_file;

	gps_fd = qrtr_init();
	if (gps_fd < 0)
		goto clean;

	while (true) {
		wait_for_start(sig_fd);
			
		ret = gps_start(gps_fd);
		if (ret < 0)
			goto clean;

		transit_gps_data(gps_fd, pty_m_fd, sig_fd);

		ret = gps_stop(gps_fd);
		if (ret < 0)
			goto clean;
	}
clean:
	qrtr_close(gps_fd);
clean_file:
	close(pty_m_fd);
clean_signal:
	close(sig_fd);
	return EXIT_SUCCESS;
}
