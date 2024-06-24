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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "qdiag.h"
#include "diag_cntl.h"
#include "list.h"
#include "peripheral.h"
#include "util.h"
#include "watch.h"

struct list_head *peripherals = NULL;
struct list_head *peripheral_ch_names = NULL;


static void peripheral_open(void *data)
{
	struct peripheral *peripheral = data;	
	int fd=-1;
	
	if (peripheral == NULL) return;
	
	for(int i=peripheral_ch_type_data ;i< MAX_NUM_OF_CH; i++)
	{
		if(peripheral->ch_name[i]) 
		{	
			diag_dbg(DIAG_DBG_PERIPHERAL, "openning %s\n",peripheral->ch_name[i]);
			fd= open(peripheral->ch_name[i], O_RDWR);
			if (fd > 0){			
				peripheral->fd[i]=fd;
				diag_dbg(DIAG_DBG_PERIPHERAL, "fd = %d\n",peripheral->fd[i]);
			}
			else {
				warn("failed to open %s channel",peripheral->ch_name[i]);
			}
		}
	}
	
	watch_add_writeq(peripheral->fd[peripheral_ch_type_ctrl], &peripheral->cntlq);
	watch_add_writeq(peripheral->fd[peripheral_ch_type_data], &peripheral->dataq);
	watch_add_writeq(peripheral->fd[peripheral_ch_type_cmd], &peripheral->cmdq);
	watch_add_readfd(peripheral->fd[peripheral_ch_type_cmd], diag_router_handle_outgoing_packet_cmd, peripheral);
	watch_add_readfd(peripheral->fd[peripheral_ch_type_ctrl], diag_cntl_recv, peripheral);
	watch_add_readfd(peripheral->fd[peripheral_ch_type_data], diag_router_handle_outgoing_packet, peripheral);
	return;
}

static int peripheral_create(const char *name,const char *ch_name, peripheral_ch_type ch_type)
{
	struct peripheral *peripheral;
	struct list_head *item;
	list_for_each(item, peripherals) 
	{
		peripheral = container_of(item, struct peripheral, node);
		if (strcmp(peripheral->name, name) == 0)
		{
			diag_dbg(DIAG_DBG_PERIPHERAL,"Added %s to %s device \n",ch_name, name);
			peripheral->ch_name[ch_type] = strdup(ch_name);
			return 0;
		}
	}

	peripheral = malloc(sizeof(*peripheral));
	memset(peripheral, 0, sizeof(*peripheral));

	peripheral->name = strdup(name);
	peripheral->ch_name[ch_type] = strdup(ch_name);
	for(int i=peripheral_ch_type_data ;i< MAX_NUM_OF_CH; i++)
	{
		peripheral->fd[i] = -1;
	}
	list_add(peripherals, &peripheral->node);

	return 0;
}

void peripheral_close(struct peripheral *peripheral)
{
	diag_cntl_close(peripheral);
	printf("closing each peripheral\n");
	for(int i=peripheral_ch_type_data ;i< MAX_NUM_OF_CH; i++)
	{
		if(peripheral->fd[i] >= 0){
			watch_remove_fd(peripheral->fd[i]);
			close(peripheral->fd[i]);
			printf("closed %s:%s fd=%d \n",peripheral->name,peripheral->ch_name[i],peripheral->fd[i]);
		}
	}

	list_del(&peripheral->node);
	free(peripheral->name);
	free(peripheral);
}

int peripheral_init(struct list_head *list,struct list_head *conf_names )
{
	struct peripheral_name *periph_name;
	struct list_head *item;
	struct peripheral *peripheral;
	char *path;
	char *rproc;
	char *name;
	char *tmp;
	char *p;
	char ch='/';
	
	peripherals = list;
	peripheral_ch_names = conf_names;	
	
	/* Open the device only if it exist in the list */
	for(int i=peripheral_ch_type_data ;i< MAX_NUM_OF_CH; i++)
	{
		list_for_each(item, &peripheral_ch_names[i]) {
			periph_name=container_of(item,struct peripheral_name, node);
			path=strdup(periph_name->name);
			/* expect a path : /dev/rpmsg/proc_name/channel_name*/
			p=strrchr(path,ch);
			
			if (p != NULL)
			{
				tmp=p+1;
				name = strdup(tmp);
				diag_dbg(DIAG_DBG_PERIPHERAL,"name = %s\n",name);
			}
			else 
			{
				err(1,"can't find channel, please supply channel full path with the following pattern /dev/rpmsg/$proc_name/$channel_name");
			}
			*p=0;
			p=strrchr(path,ch);
			if (p != NULL)
			{
				tmp=p+1;
				rproc = strdup(tmp);
				diag_dbg(DIAG_DBG_PERIPHERAL,"rproc = %s\n",rproc);
			}
			else 
			{
				err(1,"can't find channel, please supply channel full path with the following pattern /dev/rpmsg/$proc_name/$channel_name");
			}
			peripheral_create(rproc,periph_name->name,i);
			free(path);
		}
	}

	list_for_each(item, peripherals) {
	   peripheral=container_of(item,struct peripheral, node);
	   diag_dbg(DIAG_DBG_PERIPHERAL,"openning %s to %s %s %s  device \n", peripheral->name,peripheral->ch_name[0],peripheral->ch_name[1],peripheral->ch_name[2]);	  
	   peripheral_open(peripheral);		
	}	
	return 0;
}

int peripheral_exit()
{
	struct list_head *item;
	struct peripheral *peripheral;
	warn("peripheral_exit");
	
	/* Close each device */
	list_for_each(item, peripherals) {
		peripheral=container_of(item,struct peripheral, node);
		peripheral_close(peripheral);
	}
	return 0;
}
