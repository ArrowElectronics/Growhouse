/*
 * watchdog_daemon.c
 * watchdog daemon app
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/watchdog.h>
#include <sys/ioctl.h>
#include <syslog.h>

#define WATCHDOG_DEV "/dev/watchdog"
#define MAGIC 'V'
#define STOP_WATCHDOG_AND_EXIT 0

/*
 * main routine
 */
int main(int argc, char* argv[])
{
	int fd = -1;
	int biteTime = 10;
	int petTime = 5;
	int option = -1;

	while ((option = getopt(argc, argv, ":t:T:")) != -1)
	{
		switch(option)
		{
			case 't':
				petTime = atoi(optarg);
				break;
			case 'T':
				biteTime = atoi(optarg);
				break;
			default:
				break;
		}
	}

	daemon(0, 0);

	setlogmask (LOG_UPTO (LOG_NOTICE));
	openlog ("WatchdogDaemon", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

	syslog (LOG_NOTICE, "Watchdog daemon started\n");
	/* Open */
	fd = open(WATCHDOG_DEV, O_WRONLY);
	if (fd < 0)
	{
		syslog (LOG_ERR, "Cannot open watchdog device\n");
		return -1;
	}

	/* Set timeout */
	if(ioctl(fd, WDIOC_SETTIMEOUT, &biteTime))
	{
		syslog (LOG_ERR, "ioctl to set timeout failed\n");
	}

	ioctl(fd, WDIOC_KEEPALIVE, 0);

	/* Ping */
	while(1)
	{
		sleep((petTime));
		if (ioctl(fd, WDIOC_KEEPALIVE, 0))
			syslog (LOG_ERR, "Cannot pet watchdog!\n");
	}

	closelog();
	close(fd);
}

/*
 * End watchdog_daemon.c
 */
