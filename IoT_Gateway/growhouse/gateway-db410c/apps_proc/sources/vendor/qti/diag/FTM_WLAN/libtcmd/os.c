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
#include  "os.h"

int tcmd_set_timer(struct tcmd_cfg *cfg)
{
	struct itimerspec exp_time;
	int err;

	A_DBG("setting timer\n");
	bzero(&exp_time, sizeof(exp_time));
	exp_time.it_value.tv_sec = TCMD_TIMEOUT;
	err = timer_settime(cfg->timer, 0, &exp_time, NULL);
	cfg->timeout = false;
	if (err < 0)
		return errno;
	return 0;
}

int tcmd_reset_timer(struct tcmd_cfg *cfg)
{
	struct itimerspec curr_time;
	int err;

	err = timer_gettime(cfg->timer, &curr_time);
	if (err < 0)
		return errno;

	if (!curr_time.it_value.tv_sec && !curr_time.it_value.tv_nsec)
		return -ETIMEDOUT;

	A_DBG("resetting timer\n");
	bzero(&curr_time, sizeof(curr_time));
	err = timer_settime(cfg->timer, 0, &curr_time, NULL);
	if (err < 0)
		return errno;
	return 0;
}
