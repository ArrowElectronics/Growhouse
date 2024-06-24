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

#include "list.h"
#include "qdiag.h"
#include "qdiag_so_client.h"
#include <dlfcn.h>

struct diag_if * g_dif=NULL;

static int create_so_client(char *so)
{
	struct qdiag_table *tbl;
	struct qdiag_table_entry *entry;
	char *error;;
	struct client_so{
		char *name;
		void *handle;
		int fd;
	};
	void (*diag_get_table)(struct qdiag_table **tbl_ptr);
	int (*diag_set_pipe)(int fd);
	int (*diag_set_debug_level)(int level);
	int pipefd[2];
	struct diag_cmd *dc;
	struct client_so *client=(struct client_so *)malloc(sizeof(struct client_so));
	struct peripheral *peripheral;
	
	if ( client == NULL) 
	{
		diag_err("Failed to allocate client so");
		return -1;
	}
	client->handle = dlopen(so, RTLD_LAZY);
    if (!client->handle) {
        diag_err("%s\n", dlerror());
        exit(0);
    }
	
    dlerror();    /* Clear any existing error */
	
	/* create a virtual peripheral for the so */
	peripheral = malloc(sizeof(*peripheral));
	memset(peripheral, 0, sizeof(*peripheral));
	peripheral->name = strdup(so);
	peripheral->features = DIAG_FEATURE_APPS_HDLC_ENCODE;
	
	*(void **) (&diag_get_table) = dlsym(client->handle, "qdiag_get_table");
	*(void **) (&diag_set_pipe) = dlsym(client->handle, "qdiag_set_pipe");
	*(void **) (&diag_set_debug_level) = dlsym(client->handle, "qdiag_set_debug_level");
	
	if ((error = dlerror()) != NULL)  {
        diag_err("%s\n", error);
		return -1;        
    }
	
	(*diag_get_table)(&tbl);
	
	for(int i=0;i<tbl->hdr.num_of_entries;i++)
	{
		entry=&tbl->table[i];
		dc = malloc(sizeof(*dc));
		if (!dc)
			err(1, "malloc failed");
		memset(dc, 0, sizeof(*dc));

		dc->first = entry->first_cmd;
		dc->last =  entry->last_cmd;
		dc->peripheral = peripheral;
		dc->cb = (diag_cmd_cb)entry->cb;
		list_add(&g_dif->diag_cmds, &dc->node);	
		diag_dbg(DIAG_DBG_CONFIG,"imported cmd = 0x%X  cb = 0x%p\n", dc->first, dc->cb);
	}
	
	pipe(pipefd);
	(*diag_set_pipe)(pipefd[1]);
	watch_add_readfd(pipefd[0], diag_router_handle_outgoing_packet, peripheral);
	(*diag_set_debug_level)(diag_dbg_mask & DIAG_DBG_PLUGIN);
	return 0;
}
	
static int diag_config_fill(struct diag_config *conf,
			       const char *buf, char *pos, int line)
{
	struct peripheral_name *temp;

	diag_dbg(DIAG_DBG_CONFIG, "START\n");

	if (strcmp(buf, "remoteproc_data") == 0) {
		temp=(struct peripheral_name *)malloc(sizeof(struct peripheral_name));
		if (temp == NULL)
		{
			return -1;
		}
		temp->name = strdup(pos);
		list_add(&conf->peripherals_names[peripheral_ch_type_data], &temp->node);
		
	diag_dbg(DIAG_DBG_CONFIG, "ADDED %s\n",temp->name );
	} else if (strcmp(buf, "remoteproc_ctrl") == 0) {
		temp = (struct peripheral_name *)malloc(sizeof(struct peripheral_name));
		if (temp == NULL)
		{
			return -1;
		}
		temp->name = strdup(pos);
		list_add(&conf->peripherals_names[peripheral_ch_type_ctrl], &temp->node);
		
	diag_dbg(DIAG_DBG_CONFIG, "ADDED %s\n",temp->name );
	} else if (strcmp(buf, "remoteproc_cmd") == 0) {
		temp=(struct peripheral_name *)malloc(sizeof(struct peripheral_name));
		if (temp == NULL)
		{
			return -1;
		}
		temp->name = strdup(pos);
		list_add(&conf->peripherals_names[peripheral_ch_type_cmd], &temp->node);
		
	diag_dbg(DIAG_DBG_CONFIG, "ADDED %s\n",temp->name );
	} else if (strcmp(buf, "remoteproc_client_so") == 0) {
		create_so_client(pos);
	}else if (strcmp(buf, "socket_address") == 0) {
		conf->transport_config.socket_config.address = strdup(pos);
	} else if (strcmp(buf, "socket_port") == 0) {
		conf->transport_config.socket_config.port = atoi(pos);
	} else if (strcmp(buf, "uart_device") == 0) {
		conf->transport_config.uart_config.device = strdup(pos);
	} else if (strcmp(buf, "uart_baud_rate") == 0) {
		conf->transport_config.uart_config.baud = atoi(pos);
	} else if (strcmp(buf, "usb_device") == 0) {
		conf->transport_config.usb_config.device = strdup(pos);
	} else if (strcmp(buf, "usb_serial") == 0) {
		conf->transport_config.usb_config.serial = strdup(pos);
	} else if (strcmp(buf, "usb_gadget") == 0) {
		conf->transport_config.usb_config.gadget = strdup(pos);
	} else if (strcmp(buf, "usb_udc") == 0) {
		conf->transport_config.usb_config.udc = strdup(pos);
	} else if (strcmp(buf, "transport_op_mode") == 0) {
		conf->transport_config.op_mode = atoi(pos);
	} else if (strcmp(buf, "log_file_name") == 0) {
		conf->transport_config.log_fname = strdup(pos);
	}
	diag_dbg(DIAG_DBG_CONFIG, "END\n");
	return 0;
	
}		   
			   
void diag_set_defaults_peripherals(struct peripheral * peripherals)
{
	return;
	
}
#define DIAG_CHAN_NUM 4
 int diag_config_defaults(struct diag_config *conf)
{
	for(int i=peripheral_ch_type_data ;i< MAX_NUM_OF_CH; i++)
	{
		LIST_DYNAMIC_INIT(conf->peripherals_names[i])
	}

	/* Config filename */
	conf->conf_fname = strdup("./diag.config");

	/* Log enabled */
	conf->log_enabled = 0;

	/* Router configuration */
	conf->router_config.num_peripherals = 0;
	
	/* Transport Configuration */
	conf->transport_config.log_fname = strdup("./diag.log");

	/* UART configuration */
	conf->transport_config.uart_config.device=strdup("/dev/ttyMSM1");
	conf->transport_config.uart_config.baud=115200;
	
	/* SOCKET configuration */
	conf->transport_config.socket_config.address=strdup("10.18.166.176");
	conf->transport_config.socket_config.port=2500;
	
	/* USB configuration */
	conf->transport_config.usb_config.device = strdup("/dev/gadget/qdiag");
	conf->transport_config.usb_config.serial = NULL;
	conf->transport_config.usb_config.gadget = strdup("/sys/kernel/config/usb_gadget/g1");
	conf->transport_config.usb_config.udc = strdup("ci_hdrc.0");

	/* Operation Mode */
	conf->transport_config.op_mode = DIAG_TRANS_SOCKET;

	//diag_set_defaults_peripherals(conf->peripherals);
	
	return 0;
}


/**
 * hostapd_config_read - Read and parse a configuration file
 * @fname: Configuration file name (including path, if needed)
 * Returns: Allocated configuration data structure
 */
int diag_config_read(struct diag_config *conf, const char *fname)
{
	FILE *f;
	char buf[4096], *pos;
	int line = 0;
	int errors = 0;
	int ret = 0;

	diag_dbg(DIAG_DBG_CONFIG,"START reading file %s\n", fname);
	f = fopen(fname, "r");
	if (f == NULL) {
		diag_err( "Could not open configuration file '%s' "
			   "for reading.", fname);
		return -1;
	}

	while (fgets(buf, sizeof(buf), f)) {
		line++;
		diag_dbg(DIAG_DBG_CONFIG,"Line #%d: %s\n", line, buf);

		if (buf[0] == '#')
			continue;
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;

		pos = strchr(buf, '=');
		if (pos == NULL) {
			diag_warn( "Line %d: invalid line '%s'",
				   line, buf);
			errors++;
			continue;
		}
		*pos = '\0';
		pos++;
		diag_dbg(DIAG_DBG_CONFIG, "Parsed Name=%s Value=%s\n", buf, pos);
		errors += diag_config_fill(conf, buf, pos, line);
	}

	fclose(f);
	diag_dbg(DIAG_DBG_CONFIG, "END\n");
	return ret;
}

int diag_quit()
{
	int ret = 0;
	if(g_dif == NULL) return -1;
	if(g_dif->router_quit)
	{
		ret = g_dif->router_quit();
		if (ret) {
			diag_err("Error");
			return -1;
		}
	}	
	return ret;
}
int diag_init(struct diag_if * dif, const char * conf_fname, int log_enabled)
{
	
	struct diag_config *conf;
	int ret=0;	
	if (dif == NULL) 
		return -1;
	g_dif=dif;
	diag_dbg(DIAG_DBG_CONFIG,"START\n");
	/* Allocate configuration */
	conf = (struct diag_config *)malloc(sizeof(struct diag_config));
	if (conf == NULL) {
		diag_err("Failed to allocate memory for "
				 "configuration data.");
		free(conf);
		return -1;
	}
	
	diag_dbg(DIAG_DBG_CONFIG,"Attach configuration\n");
	/* Attach configuration to IF */
	dif->conf = conf;	
    
	LIST_DYNAMIC_INIT(dif->diag_cmds);
	
	diag_dbg(DIAG_DBG_CONFIG,"Set Defaults\n");
	/* set defaults */
	ret = diag_config_defaults(conf);
	if (ret) {
		diag_err("Could not set defaults.");
		return -1;
	}		
	if (conf_fname)
		conf->conf_fname = conf_fname;
	diag_dbg(DIAG_DBG_CONFIG,"Opening file %s\n", dif->conf->conf_fname);
	/* set values based on configuration */
	ret = diag_config_read(conf, dif->conf->conf_fname);
	if (ret) {
		diag_err("Could not open configuration file for reading using defaults.");
	}

	conf->log_enabled = log_enabled;
	
	diag_dbg(DIAG_DBG_CONFIG,"Init router layer\n");
	/* Init the router layer */
	if (dif->router_init_func)
	{
		ret = dif->router_init_func(dif);
		if (ret) {
			diag_err("Failed to initialise router layer");
			return -1;
		}
	}
	
	diag_dbg(DIAG_DBG_CONFIG,"Init transport layer\n");
	/* Init the transport layer */
	if (dif->transport_init_func)
	{
		ret = dif->transport_init_func(dif);
		if (ret) {
			diag_err("Failed to initialise transport layer");
			return -1;
		}
	}
	diag_dbg(DIAG_DBG_CONFIG,"END\n");
	return 0;
}
