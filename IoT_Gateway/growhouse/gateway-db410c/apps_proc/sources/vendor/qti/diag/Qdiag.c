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
#include <sys/ioctl.h>
#include <sys/types.h>
#include  <signal.h>
#include <syslog.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "qdiag.h"
#include "diag_cntl.h"
#include "util.h"
#include "watch.h"
#include "diag_config.h"


#define VERSION_STR "1.1.001"
static void show_version(void)
{
	fprintf(stderr,
		"Qdiag v" VERSION_STR "\n"
		"User space for diag interface,\n");
}
static int switch2silent()
{
	int devnull=0;
	devnull = open("/dev/null", O_RDWR);		
	if (dup2(devnull, STDIN_FILENO) < 0) {
		close(devnull);
		return -1;
	}

	if (dup2(devnull, STDOUT_FILENO) < 0) {
		close(devnull);
		return -1;
	}

	if (dup2(devnull, STDERR_FILENO) < 0) {
		close(devnull);
		return -1;
	}
	return 0;
}
static int go_daemonize(const char * fname)
{
	int main_pid=getpid();
	int pid_son=fork();
	
	if(pid_son < 0) 
	{
		diag_err("Failed to create a process");
		return -1;
	}
	else
	{
		/* close the main process */
		if (getpid()==main_pid) 
		{
			
			show_version();
			diag_info("Working as a daemon with pid %d", pid_son);
			exit(0);
		}
	}
	if (fname != NULL){
		FILE *f = fopen(fname, "w");
		if (f) {
			fprintf(f, "%u\n", getpid());
			fclose(f);
		}
		else 
		{
			diag_info("cant open %s",fname);
			return -1;
		}
	}
	return 0;
}
static void say_Hi()
{
	diag_info("\n\
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\
	!!                Hi from QDiag %8s               !!\n\
	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", VERSION_STR);
}
static void usage(void)
{
	show_version();
	fprintf(stderr,
		"\n"
		"usage: diag [-hdBPKtvVcm] "
		"\\\n"
		"\n"
		"options:\n"
		"   -c   <configuration file>\n"
		"   -h   show this usage\n"
		"   -d   show more debug messages\n"
		"   -B   run daemon in the background\n"
		"   -l   log output to debug file instead of stdout\n"
		"   -V   verbose\n"
		"   -P   PID file\n"
		"   -m   <debug mask in Hex e.g 0xFFffFFff>\n"
		"   -v   show diag version\n");

	exit(1);
}

unsigned int diag_dbg_mask = DIAG_DBG_NONE;
void     INThandler(int sig)
{
	printf("int!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	diag_quit();
	signal(sig, SIG_IGN);
	exit(0);
}

static unsigned int hatoi(char *str)
{
	unsigned int num=0,tmp;
	char ch=0;
	if (str == NULL) return 0;
	if (str[0] == '0' && (str[1]=='x' || str[1] == 'X')) str+=2;
	while(str && *str)
	{
		num *=16;
		tmp = 0;
		ch=*str++;
		if ((ch>='A') && ( ch <= 'F')) tmp = ch - 'A' + 10;
		else if ((ch>='a') && ( ch <= 'f')) tmp = ch - 'a' + 10;
		else tmp = ch - '0';
		num+=tmp;
	}
	return num;
}

int main(int argc, char **argv)
{
	int ret;
	int c;
	int	debug = 0;
	int	verbose = 0;
	int	daemonize = 0;
	const char *conf_fname = NULL;
	int log_enabled = 0;
	const char *pid_file = NULL;
	struct diag_if * interface;
	
	signal(SIGINT, INThandler) ;
	
	for (;;) {
		c = getopt(argc, argv, "P:c:m:hlvdVBP");
		if (c < 0)
			break;
		switch (c) {
		case 'c':
			/* Set Config filename */
			conf_fname = strdup(optarg);
			break;
		case 'd':
			debug++;
			break;
		case 'B':
			daemonize++;
			break;
		case 'P':
			pid_file = strdup(optarg);	
			break;
		case 'V':
			verbose++;
			break;
		case 'l':
			log_enabled = 1;
			break;
		case 'v':
			show_version();
			return 0;
		case 'm':
			diag_dbg_mask=hatoi(optarg);
			break;
		default:
		case 'h':
			usage();
			break;
		}
	}
	

	if (debug)
	{
		diag_dbg_mask = DIAG_DBG_ANY;
	}
	diag_dbg(DIAG_DBG_MAIN, "Debug mask is 0x%08X \n", diag_dbg_mask);
	diag_dbg(DIAG_DBG_MAIN, "Configuration file name to be read is  %s\n", conf_fname);
	diag_dbg(DIAG_DBG_MAIN, "Logging is %s\n", log_enabled ? "ENABLED" : "DISABLED");

	/* Verbose the process if asked */
	if (!verbose || daemonize)
	{
		switch2silent();		
	}
	/* continue as daemon if needed */
	if(daemonize)
	{	
		ret = go_daemonize(pid_file);
	}
	
	interface = (struct diag_if *)malloc(sizeof(struct diag_if));
	if (interface == NULL)
	{
		diag_err("Failed to allocate main interface");
	}
	memset(interface, 0, sizeof(struct diag_if));
	
	

	/* Set functions hooks */
	interface->transport_init_func = &diag_transport_init;
	interface->transport_receive_func = &diag_transport_receive_packet;
	interface->transport_send_func = &diag_transport_dispatch_packet;

	interface->router_init_func = &diag_router_init;
	interface->router_handle_incoming_func = &diag_router_handle_incoming_packet;
	interface->router_handle_outgoing_func = &diag_router_handle_outgoing_packet;
	interface->router_quit = &diag_router_quit;
	
	/* Init diag modules according to the configuration */
	ret = diag_init(interface, conf_fname, log_enabled);
	if(ret)
	{
		return -1;
	}	
	
	say_Hi();
	/* Main loop */
	watch_run();
	
	/* Exit */
	diag_quit();
	diag_dbg(DIAG_DBG_MAIN, "END\n");

	return 0;
}
