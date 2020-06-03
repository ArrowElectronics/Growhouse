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

#include "libtcmd.h"
#include "os.h"

int cb_ret;

/* shared with wcn36xx */
enum wcn36xx_testmode_attr {
	__WCN36XX_TM_ATTR_INVALID	= 0,
	WCN36XX_TM_ATTR_CMD	= 1,
	WCN36XX_TM_ATTR_DATA	= 2,

	/* keep last */
	__WCN36XX_TM_ATTR_AFTER_LAST,
	WCN36XX_TM_ATTR_MAX	= __WCN36XX_TM_ATTR_AFTER_LAST - 1
};

/* All wcn36xx testmode interface commands specified in
 * WCN36XX_TM_ATTR_CMD
 */
enum wcn36xx_tm_cmd {
	/* Returns the supported wcn36xx testmode interface version in
	 * WCN36XX_TM_ATTR_VERSION. Always guaranteed to work. User space
	 * uses this to verify it's using the correct version of the
	 * testmode interface
	 */
	WCN36XX_TM_CMD_GET_VERSION = 0,

	/* The netdev interface must be down at the
	 * time.
	 */
	WCN36XX_TM_CMD_START = 1,

	/* Puts the driver back into OFF state.
	 */
	WCN36XX_TM_CMD_STOP = 2,

	/* The command used to transmit a PTT command to the firmware.
	 * Command payload is provided in WCN36XX_TM_ATTR_DATA.
	 */
	WCN36XX_TM_CMD_PTT = 3,
};

int nl80211_set_cmd(uint16_t *driv_cmd, uint16_t ftm_cmd_type)
{
	switch (ftm_cmd_type) {
	case WCN36XX_TM_CMD_GET_VERSION:
	case WCN36XX_TM_CMD_START:
	case WCN36XX_TM_CMD_STOP:
	case WCN36XX_TM_CMD_PTT:
		*driv_cmd = ftm_cmd_type;
		break;
	default:
		fprintf(stderr, "nl80211: unknown cmd!");
		return -1;
	}
	return 0;
}

/* tcmd rx_cb wrapper to "unpack" the nl80211 msg and call the "real" cb */
int nl80211_rx_cb(struct nl_msg *msg, void *arg)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *td[WCN36XX_TM_ATTR_MAX + 1];
	void *buf;
	int len;

	A_DBG("nl80211: cb wrapper called\n");
	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_TESTDATA] || !tb[NL80211_ATTR_WIPHY]) {
		printf("no data!\n");
		return NL_SKIP;
	}

	nla_parse(td, WCN36XX_TM_ATTR_MAX, nla_data(tb[NL80211_ATTR_TESTDATA]),
		  nla_len(tb[NL80211_ATTR_TESTDATA]), NULL);

	if (!td[WCN36XX_TM_ATTR_DATA]) {
		printf("no data in reply\n");
		return NL_SKIP;
	}

	buf = nla_data(td[WCN36XX_TM_ATTR_DATA]);
	len = nla_len(td[WCN36XX_TM_ATTR_DATA]);

	A_DBG("nl80211: resp received, calling custom cb\n");
	tcmd_cfg.rx_cb(buf, len);
	/* trip waiting thread */
	tcmd_cfg.timeout = true;

	return NL_SKIP;
}

/********************************/
int nl80211_init(struct tcmd_cfg *cfg)
{
	int ret;

	A_DBG("%s started\n", __FUNCTION__);

	if(cfg->nl_handle)
		nl_socket_free(cfg->nl_handle);

	cfg->nl_handle = nl_socket_alloc();
	if (cfg->nl_handle == NULL) {
		A_DBG("Failed to allocate netlink socket.\n");
		return -ENOMEM;
	}

	if (genl_connect(cfg->nl_handle)) {
		A_DBG("Failed to connect to generic netlink.\n");
		ret = -ENOLINK;
		goto out_handle_destroy;
	}

	cfg->nl_id = genl_ctrl_resolve(cfg->nl_handle, "nl80211");
	if (cfg->nl_id < 0) {
		A_DBG("nl80211 not found.\n");
		ret = -ENOENT;
		goto out_handle_destroy;
	}

	ret = genl_ctrl_resolve_grp(cfg->nl_handle, "nl80211", "testmode");
	if (ret >= 0) {
		ret = nl_socket_add_membership(cfg->nl_handle, ret);
		if (ret) {
			A_DBG("failed to join \"testmode\" group!\n");
			ret = -ENOENT;
			goto out_handle_destroy;
		}
	} else {
		A_DBG("Could not resolve group!!!\n");
		ret = -ENOENT;
		goto out_handle_destroy;
	}

	/*
	 * Enable peek mode so drivers can send large amounts
	 * of data in blobs without problems.
	 */
	nl_socket_enable_msg_peek(cfg->nl_handle);

	/*
	 * disable sequence checking to handle events.
	 */
	nl_socket_disable_seq_check(cfg->nl_handle);

	/* so we can handle timeouts properly */
	nl_socket_set_nonblocking(cfg->nl_handle);

	return 0;

 out_handle_destroy:
 	nl_socket_free(cfg->nl_handle);
	return ret;
}

int nl80211_tcmd_tx(struct tcmd_cfg *cfg, void *buf, int len)
{
	struct nl_msg *msg;
	struct nlattr *nest;
	int devidx, err = 0;

	/* CHANGE HERE: you may need to allocate larger messages! */
	msg = nlmsg_alloc();
	if (!msg) {
		A_DBG("failed to allocate netlink message\n");
		return 2;
	}

	genlmsg_put(msg, 0, 0, cfg->nl_id, 0,
		    0, NL80211_CMD_TESTMODE, 0);

	devidx = if_nametoindex(cfg->iface);
	if (devidx) {
		NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);
	} else {
		A_DBG("Device not found\n");
		err = -ENOENT;
		goto out_free_msg;
	}

	nest = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
	if (!nest) {
		A_DBG("failed to nest\n");
		err = -1;
		goto out_free_msg;
	}

	NLA_PUT_U16(msg, WCN36XX_TM_ATTR_CMD, cfg->cmd);
	NLA_PUT(msg, WCN36XX_TM_ATTR_DATA, len, buf);

	nla_nest_end(msg, nest);

	A_DBG("nl80211: sending message\n");
	nl_send_auto_complete(cfg->nl_handle, msg);
	A_DBG("nl80211: message sent\n");

	nl_socket_modify_cb(cfg->nl_handle, NL_CB_VALID, NL_CB_CUSTOM, nl80211_rx_cb, NULL);
	A_DBG("nl80211: rx callback set to %d\n", (void *)nl80211_rx_cb);

 out_free_msg:
	nlmsg_free(msg);
	A_DBG("nl80211: sent message is freed\n");
	return err;

 nla_put_failure:
	A_DBG("building message failed\n");
	return 2;
}

int nl80211_tcmd_rx(struct tcmd_cfg *cfg)
{
	struct nl_cb *cb;
	int err = 0;

	cb = nl_socket_get_cb(cfg->nl_handle);
	if (!cb) {
		fprintf(stderr, "failed to allocate netlink callbacks\n");
		err = 2;
		goto out;
	}

	err = tcmd_set_timer(cfg);
	if (err)
		goto out;

	A_DBG("nl80211: waiting for response\n");
	while (!cfg->timeout)
		nl_recvmsgs(cfg->nl_handle, cb);

	return tcmd_reset_timer(cfg);
out:
	return err;
}
