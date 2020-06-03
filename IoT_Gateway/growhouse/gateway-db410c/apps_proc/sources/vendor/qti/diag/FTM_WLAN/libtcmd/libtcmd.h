/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

#ifndef _LIBTCMD_H_
#define _LIBTCMD_H_

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

#ifdef DEBUG
#define A_ERR(ret, args...)	{		\
	fprintf(stderr, "Error: ret=%d ", ret); \
	fprintf(stderr, ##args);	\
	}
#define A_DBG(args...) fprintf(stderr, args);
#else
#define A_ERR(ret, args...)	while (0) {}
#define A_DBG(args...) while (0) {}
#endif

#define TCMD_TIMEOUT 10	/* s */

struct tcmd_cfg {
	char iface[100];
	uint16_t cmd;
	void (*rx_cb)(void *buf, int len);
	struct nl_sock *nl_handle;
	int nl_id;
	struct sigevent sev;
	timer_t timer;
	bool timeout;
} tcmd_cfg;

/* WLAN API */
#include "nl80211_drv.h"

/* send tcmd type in cmd and payload in buffer buf of length len.
 * resp == true if a response by the FW is required.
 * Returns: 0 on success, -ETIMEOUT on timeout
 */
int tcmd_tx(uint16_t cmd, void *buf, int len, bool resp);

/* Initialize tcmd transport layer on given iface. Call given rx_cb on tcmd
 * response and cmd */
int tcmd_tx_init(char *iface, void (*rx_cb)(void *buf, int len));
#endif /* _LIBTCMD_H_ */
