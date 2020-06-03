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

#include "string.h"
#include "libtcmd.h"
#include "os.h"

int tcmd_tx(uint16_t cmd, void *buf, int len, bool resp)
{
	int ret = 0;

	ret = nl80211_set_cmd(&tcmd_cfg.cmd, cmd);
	if (ret)
		goto err_out;
	ret = nl80211_tcmd_tx(&tcmd_cfg, buf, len);
	if (ret)
		goto err_out;
	if (resp) {
		ret = nl80211_tcmd_rx(&tcmd_cfg);
		if (ret)
			goto err_out;
	}

	return ret;
err_out:
	A_DBG("tcmd_tx failed: %d\n", ret);
	return ret;
}

static void tcmd_expire(union sigval sig)
{
	/* tcmd expired, do something */
	A_DBG("timer expired\n");
	tcmd_cfg.timeout = true;
}

int tcmd_tx_init(char *iface, void (*rx_cb)(void *buf, int len))
{
	int err;

	strncpy(tcmd_cfg.iface, iface, sizeof(tcmd_cfg.iface));
	tcmd_cfg.rx_cb = rx_cb;
	tcmd_cfg.sev.sigev_notify = SIGEV_THREAD;
	tcmd_cfg.sev.sigev_notify_function = tcmd_expire;
	timer_create(CLOCK_REALTIME, &tcmd_cfg.sev, &tcmd_cfg.timer);

	err = nl80211_init(&tcmd_cfg);
	if (err) {
		A_DBG("couldn't init nl80211!: %s nl_handle=%p\n", strerror(-err), (void *)(tcmd_cfg.nl_handle));
		return err;
	}

	return 0;
}

