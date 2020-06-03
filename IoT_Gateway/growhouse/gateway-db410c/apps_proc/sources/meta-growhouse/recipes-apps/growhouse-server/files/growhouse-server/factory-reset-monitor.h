#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

#include <stdio.h>
#include <pthread.h>

#define _GNU_SOURCE /* for asprintf */
#include <stdio.h>
#include <stdint.h>

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <linux/version.h>
#include <linux/input.h>

#include <time.h>
#include <syslog.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <mosquitto.h>

#define TIMEOUT_SEC 		3
#define MAX_STR_BUF_SIZE 	255
#define CLEAR(x) memset(x, 0, sizeof(x))

#define FACTORY_RESET_BUTTON	0x100 // BTN_0 KeyCode

#define logButtonInfo(fmt, args...) syslog(LOG_INFO, "%s: " fmt, __func__, ##args)

/*
 * Button states enum
 */
enum {
        BUTTON_RELEASED,
        BUTTON_PRESS,
        BUTTON_HOLD,
        BUTTON_IDLE,
};

typedef struct Button {
        int Button_status;
        bool Button_Hold_timeout;
        unsigned long Button_Hold_start_time;
}Button;

static int grab_flag = 0;
static volatile sig_atomic_t stop = 0;


// Thread function declaration
void* monitorFactoryReset(void *);
void factory_button_longPress_detected(struct mosquitto * mosq);

#endif
