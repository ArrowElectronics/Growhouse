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
#include <sys/time.h>
#include <time.h>
#include "qdiag.h"

extern struct diag_if * g_dif;
uint8_t buf[APPS_BUF_SIZE] = { 0 };

/*~~~~~~~~~~~~~~~~~~ USB ~~~~~~~~~~~~~~~~~~~~~~*/

#include "usb.h"
static int diag_usb_connect(const char* usbname, const char* serial, int *incoming, int *outgoing, const char* gadget, const char* udc)
{
	int ret;
	int fd;

	ret = usb_init_device(usbname, serial, incoming, outgoing, gadget, udc);
	if (ret < 0)
		return -errno;
	fd = ret;
	diag_dbg(DIAG_DBG_TRANSPORT, "Connected to %s %s\n", USB_DEVNAME, USB_SERIAL);

	return fd;
}

/*~~~~~~~~~~~~~~~~~~ UART ~~~~~~~~~~~~~~~~~~~~~~*/

#include "termios.h"
#define DEFAULT_BAUDRATE B115200 
static unsigned int check_baudrate(unsigned int baudrate) {
	switch (baudrate)
	{
	case 9600:
		return B9600;
	case 19200:
		return B19200;
	case 38400:
		return B38400;
	case 115200:
		return B115200;
	default:
		warn("Illegal baud rate %u! Defaulting to %u", baudrate, DEFAULT_BAUDRATE);
		return DEFAULT_BAUDRATE;
	}
}

static int diag_uart_connect(const char *uartname, unsigned int baudrate)
{
	int ret;
	int fd;
	struct termios options, options_save;

	baudrate = check_baudrate(baudrate);

	ret = open(uartname, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (ret < 0)
		return -errno;
	fd = ret;
	ret = tcflush(fd, TCIOFLUSH);
	if (ret < 0)
		return -errno;
	ret = fcntl(fd, F_SETFL, 0);
	if (ret < 0)
		return -errno;
	ret = ioctl(fd, TCGETS, &options_save);
	if (ret < 0)
		return -errno;
	options = options_save;
	options.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	options.c_cc[VMIN]     = 4;   /* blocking read until 4 chars received */
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= (CS8 | CLOCAL | CREAD);
	options.c_iflag = 0;
	options.c_oflag = 0;
	options.c_lflag = 0;
	options.c_cflag = (options.c_cflag & ~CBAUD) | (baudrate & CBAUD);
	ret = ioctl(fd, TCSETS, &options); // TODO: need to call ioctl(ret, TCSETS, &options_save) to revert to original state
	if (ret < 0)
		return -errno;

	diag_dbg(DIAG_DBG_TRANSPORT, "Connected to %s:%d\n", uartname, baudrate);

	return fd;
}

/*~~~~~~~~~~~~~~~~~~ SOCKET ~~~~~~~~~~~~~~~~~~~~~~*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int diag_sock_connect(const char *hostname, unsigned short port)
{
	struct sockaddr_in addr={0};
	int ret;
	int fd;

	diag_dbg(DIAG_DBG_TRANSPORT, "START\n");
	fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
	if (fd < 0)
		return -errno;

	addr.sin_family = AF_INET;
	inet_aton(hostname, &addr.sin_addr);
	addr.sin_port = htons(port);

	ret = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0)
		return -errno;

	ret = fcntl(fd, F_SETFL, O_NONBLOCK);
	if (ret < 0)
		return -errno;

	diag_info("Connected to %s:%d\n", hostname, port);

	return fd;
}

static struct mbuf * create_packet(uint8_t *data, size_t len)
{
	void *ptr;
	struct mbuf *packet = mbuf_alloc(len);
	if (packet != NULL)
	{
		ptr = mbuf_put(packet, len);
		memcpy(ptr, data, len);
	}
	return packet;
}

int diag_transport_receive_packet(int fd, void* data)
{
	struct mbuf* packet;
	uint8_t *ptr;
	uint8_t *msg;
	size_t msglen;
	size_t len;
	ssize_t n;
	int ret = 0;
	FILE *lf;
	uint8_t loc[APPS_BUF_SIZE] = { 0 };
	struct diag_client *dc = (struct diag_client *)data;
	static ssize_t off = 0;
	static ssize_t head = 0;

	n = read(dc->in_fd, loc, sizeof(loc));
	if ((n < 0) && (errno != EAGAIN)) {
		diag_err("Failed to read with returned value %d with error %d", (int)n, errno);
		return -errno;
	} else if (((n < 0) && (errno == EAGAIN)) || ((n == 0) && (off == 0))) { // in O_NONBLOCK mode OR simply nothing is available
		diag_warn("Read nothing!!!\n");
		return ret;
	} else if (n > sizeof(buf))
	{
		diag_warn("n=%ld > sizeof(buf)=%lu", n, sizeof(buf));
		return 0;
	} else if (off+n >= sizeof(buf))
	{
		off = 0;
		head = 0;
	}
	memcpy(&buf[off],loc,n);
	off +=n;
	ptr = &buf[head];
	len = off - head;

	diag_dbg_dump(DIAG_DBG_TRANSPORT_DUMP, "Packet (LOC): ", loc, n);

	while (len > 0) {
		msg = hdlc_decode_one(&ptr, &len, &msglen);
		if (!msg )
			break;
		if (((int)msglen)<=0){
			off = 0;
			head = 0;
			break;
		}

		head=(off-len);
		if (g_dif->conf->log_enabled) {
			fd = g_dif->conf->transport_config.logger->out_fd;
			lf = fdopen(fd, "a");
			if (lf) {
				diag_log_packet(lf, "< ", msg, msglen, 16);
				fflush(lf);
				fclose(lf);
			}
		}

		diag_dbg_dump(DIAG_DBG_TRANSPORT_DUMP, "Packet (MSG): ", msg, msglen);
		packet = create_packet(msg, msglen);
		if (packet == NULL)
			return -1;
		g_dif->router_handle_incoming_func(packet);
		diag_dbg_free(DIAG_DBG_TRANSPORT, packet, "\n");
	}
	return 0;
}

int diag_transport_dispatch_packet(void *data)
{
	struct mbuf* mbuf = (struct mbuf*)data;
	int fd = g_dif->conf->transport_config.client->out_fd;
	FILE *lf;
	write(fd, mbuf->data, mbuf->size);
	if (g_dif->conf->log_enabled) {
		fd = g_dif->conf->transport_config.logger->out_fd;
		lf = fdopen(fd, "a");
		if (lf) {
			diag_log_packet(lf, "> ", mbuf->data, mbuf->size, 16);
			fflush(lf);
			fclose(lf);
		}
	}
	diag_dbg_dump(DIAG_DBG_TRANSPORT_DUMP, "diag_transport_dispatch_packet Packet: ", mbuf->data, mbuf->size);
	
	return 0;
}

//int diag_transport_init(struct diag_transport_config *tconf)
int diag_transport_init(struct diag_if *dif)
{
	int ret;
	if (dif == NULL)
		return -1;
	struct diag_transport_config *tconf=&dif->conf->transport_config;
	struct diag_client *qxdm, *logger;
	diag_dbg(DIAG_DBG_TRANSPORT, "START\n");
	
	struct diag_config *dconf = dif->conf;
	diag_dbg(DIAG_DBG_TRANSPORT, "g_dif->transport_init_func=0x%p\n", (void *)g_dif->transport_init_func);
	qxdm = (struct diag_client *)malloc(sizeof(struct diag_client));
	memset(qxdm, 0, sizeof(struct diag_client));

	diag_dbg(DIAG_DBG_TRANSPORT, "Operation mode=%d\n", tconf->op_mode);
	switch(tconf->op_mode) {
	case DIAG_TRANS_USB:
		diag_dbg(DIAG_DBG_TRANSPORT, "USB device=%s\n", tconf->usb_config.device);
		ret = diag_usb_connect(tconf->usb_config.device, tconf->usb_config.serial, &qxdm->in_fd, &qxdm->out_fd, tconf->usb_config.gadget, tconf->usb_config.udc);
		diag_dbg(DIAG_DBG_TRANSPORT, "Returned file descriptor = %d\n", ret);
		if (ret < 0)
			err(1, "failed to connect to qxdm");
		break;
	case DIAG_TRANS_UART:
		diag_dbg(DIAG_DBG_TRANSPORT, "UART device=%s baudrate=%u\n", tconf->uart_config.device, tconf->uart_config.baud);
		ret = diag_uart_connect(tconf->uart_config.device, tconf->uart_config.baud); //"/dev/ttySMS1", 115200
		diag_dbg(DIAG_DBG_TRANSPORT, "Returned file descriptor = %d\n", ret);
		if (ret < 0)
			err(1, "failed to connect to qxdm");
		qxdm->in_fd = qxdm->out_fd = ret;
		break;
	case DIAG_TRANS_SOCKET:
	default:
		diag_dbg(DIAG_DBG_TRANSPORT, "SOCKET address=%s port=%u\n", tconf->socket_config.address, tconf->socket_config.port);
		ret = diag_sock_connect(tconf->socket_config.address, tconf->socket_config.port); //"10.18.166.176", 2500
		diag_dbg(DIAG_DBG_TRANSPORT, "Returned file descriptor = %d\n", ret);
		if (ret < 0)
			err(1, "failed to connect to qxdm");
		qxdm->in_fd = qxdm->out_fd = ret;
		break;
	}
	qxdm->name = strdup("QXDM");
	diag_dbg(DIAG_DBG_TRANSPORT, "qxdm->in_fd=%d, qxdm->out_fd=%d, qxdm->name=%s\n", qxdm->in_fd, qxdm->out_fd, qxdm->name);
	g_dif->conf->transport_config.client = qxdm;
	watch_add_readfd(qxdm->in_fd, diag_transport_receive_packet, qxdm);
	if (dconf->log_enabled) {
		FILE *lf;
		logger = (struct diag_client *)malloc(sizeof(struct diag_client));
		memset(logger, 0, sizeof(struct diag_client));
		logger->name = tconf->log_fname;
		ret = open(logger->name, O_CREAT | O_TRUNC | O_APPEND | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
		if (ret < 0)
			err(1, "failed to open log file named %s", logger->name);
		logger->out_fd = ret;
		lf = fdopen(logger->out_fd, "a");
		if (lf) {
			struct timeval tv;
			if (gettimeofday(&tv, NULL) != -1) {
				fprintf(lf, "DIAG session logging started %s in mode %u\n", asctime( localtime(&tv.tv_sec) ), g_dif->conf->transport_config.op_mode);
				fflush(lf);
				fclose(lf);
			}
		}
		g_dif->conf->transport_config.logger = logger;
	}
	diag_dbg(DIAG_DBG_TRANSPORT, "END\n");

	return 0;
}
