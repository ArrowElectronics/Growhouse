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

#ifndef __QDIAG_CONFIG_H__
#define __QDIAG_CONFIG_H__

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

#define MAX_PERIPH_NUM 16

struct __attribute__((__packed__)) diag_client {
	struct list_head node;
	const char *name;
	int in_fd;
	int out_fd;
};

struct diag_cmd;
typedef int (*diag_cmd_cb)(struct diag_cmd *, void *);

struct __attribute__((__packed__)) diag_cmd {
	struct list_head node;
	unsigned int first;
	unsigned int last;
	diag_cmd_cb cb;
	struct peripheral *peripheral;
};

struct __attribute__((__packed__)) diag_uart_config{
	const char *device;
	unsigned int baud;
};

struct __attribute__((__packed__)) diag_usb_config{
	const char *device;
	const char *serial;
	const char *gadget;
	const char *udc;
};

struct __attribute__((__packed__)) diag_socket_config{
	const char *address;
	unsigned short port;
};

struct __attribute__((__packed__)) diag_transport_config{
	struct diag_uart_config uart_config;
	struct diag_socket_config socket_config;
	struct diag_usb_config usb_config;
    enum diag_trans_mode {
     DIAG_TRANS_UART = 0,
     DIAG_TRANS_SOCKET = 1,
	 DIAG_TRANS_USB = 2,
     DIAG_TRANS_LAST
	} op_mode;
	struct diag_client *client;
	struct diag_client *logger;
	const char *log_fname;
};

struct __attribute__((__packed__)) diag_router_config{
	struct list_head peripherals_list;
	int num_peripherals;
	struct list_head timers;
	struct list_head read_watches;
	struct list_head write_watches;
	struct list_head quit_watches;
	struct list_head devnodes;
};

#define DIAG_MAX_PERIPH_NAME 128

struct __attribute__((__packed__)) peripheral_name{
	struct list_head node;
	const char *name;
};

struct __attribute__((__packed__)) diag_config{
	/* Configuration file path */
	const char *conf_fname;

	/* Logging enabled flag */
	int log_enabled;

	struct diag_router_config router_config;
	struct diag_transport_config transport_config;
	struct list_head peripherals_names[3];
};

struct __attribute__((__packed__)) diag_if{
	// Global diag configuration */
	struct diag_config *conf;

	/* Transport module functions pointers and queues*/
	//int (*transport_init_func)(struct diag_transport_config *);
	int (*transport_init_func)(struct diag_if *);
	int (*transport_send_func)(void *data);
	int (*transport_receive_func)(int fd, void *data);
	struct list_head diag_cmds; /*TODO need to be init */
	
	/* Router functions pointers */
	//int (*router_init_func)(struct diag_router_config *);
	int (*router_init_func)(struct diag_if * dif);
	int (*router_handle_incoming_func)(void *data);
	int (*router_handle_outgoing_func)(int fd, void *data);
	int (*router_quit)(void);
};

#endif
