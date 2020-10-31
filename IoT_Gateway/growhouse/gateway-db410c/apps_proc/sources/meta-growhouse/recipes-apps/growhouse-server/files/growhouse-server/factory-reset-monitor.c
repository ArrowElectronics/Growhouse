/**
 * @file
 * Factory reset button monitor file
 *  Created on: 28-Nov-2018
 *      Author: einfochips
 * Copyright(C) 2019 eInfochips All rights reserved.
 *
 */

#include "factory-reset-monitor.h"

Button factory_reset = { .Button_status = BUTTON_IDLE , .Button_Hold_timeout = false};

/**
 * Get current time in seconds
 *
 * @param None
 * @return Seconds
 */
long current_time()
{
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec; //seconds
}

/**
 * Grab and immediately ungrab the device.
 *
 * @param fd The file descriptor to the device.
 * @return 0 if the grab was successful, or 1 otherwise.
 */
static int test_grab(int fd, int grab_flag)
{
    int rc;

    rc = ioctl(fd, EVIOCGRAB, (void*)1);

    if (rc == 0 && !grab_flag)
        ioctl(fd, EVIOCGRAB, (void*)0);

    return rc;
}

/**
 *
 * Function to monitor button events
 *
 */
static int monitor_events(int fd, struct mosquitto *mosq)
{
    struct input_event ev[64];
    int i, rd;
    bool event_notify = false;
    fd_set rdfs;

    FD_ZERO(&rdfs);
    FD_SET(fd, &rdfs);

    while (!stop)
    {
        select(fd + 1, &rdfs, NULL, NULL, NULL);
        if (stop)
            break;
        rd = read(fd, ev, sizeof(ev));

        if (rd < (int) sizeof(struct input_event)) {
            printf("expected %d bytes, got %d\n", (int) sizeof(struct input_event), rd);
            perror("\nerror reading");
            return 1;
        }

        for (i = 0; i < rd / sizeof(struct input_event); i++) {
            unsigned int code, value;
            unsigned long timeout;
            code = ev[i].code;
            value= ev[i].value;
            if(code == FACTORY_RESET_BUTTON)
            {
                if(value == factory_reset.Button_status) {
                    timeout = current_time();
                    if( (timeout - factory_reset.Button_Hold_start_time) > TIMEOUT_SEC ) {
                        factory_reset.Button_Hold_timeout = true;
                    }
                    break;
                }
                switch(value) {
                    case BUTTON_RELEASED:
                        event_notify = false;
                        factory_reset.Button_status = BUTTON_RELEASED;
                        factory_reset.Button_Hold_timeout = false;
                        factory_reset.Button_Hold_start_time = 0;
                        break;
                    case BUTTON_PRESS:
                        factory_reset.Button_status = BUTTON_PRESS;
                        break;
                    case BUTTON_HOLD:
                        factory_reset.Button_status = BUTTON_HOLD;
                        factory_reset.Button_Hold_start_time = current_time();
                        break;
                }
            }
        }

        if(factory_reset.Button_Hold_timeout && !event_notify)
        {
            event_notify = true;
            factory_reset.Button_Hold_start_time = 0;
            factory_button_longPress_detected(mosq);
        }
    }
    return 0;
}

/**
 *
 * Function to start monitor of button events
 *
 */
int monitor_button(const char *device, int grab_flag, struct mosquitto *mosq)
{
    int fd;
    char *filename = NULL;

    filename = strdup(device);

    if (!filename)
        return EXIT_FAILURE;

    if ((fd = open(filename, O_RDONLY)) < 0) {
        if (errno == EACCES && getuid() != 0)
            fprintf(stderr, "You do not have access to %s. Try "
                    "running as root instead.\n",
                    filename);
        goto error;
    }

    if (!isatty(fileno(stdout)))
        setbuf(stdout, NULL);

    logButtonInfo("Button Monitoring started");

    if (test_grab(fd, grab_flag))
    {
        printf("***********************************************\n");
        printf("  This device is grabbed by another process.\n");
        printf("  No events are available to this applicaton while the\n"
                "  other grab is active.\n");
        printf("***********************************************\n");
    }

    free(filename);

    return monitor_events(fd, mosq);

error:
    free(filename);
    return EXIT_FAILURE;
}

/**
 *
 * Function to find event number for gpio events
 *
 */
int find_event_num(char * name)
{
    int i = 0 , ret = 0 , fd = -1;
    char filename[MAX_STR_BUF_SIZE];
    char value[MAX_STR_BUF_SIZE];
    int bytes = 0;
    int found_index = -1;

    CLEAR(filename);
    CLEAR(value);
    /*
     * Iterate over all /sys/class/input/eventX/device/name,
     * get name of each device and match with "gpio_keys".
     *
     * If not matched, then module probe is failed due to some reason and
     * device is not present.
     */
    for(i = 0; ; i++) {
        /* prepare file name */
        sprintf(filename, "/sys/class/input/event%d/device/name", i);

        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            sprintf(value, "couldn't open %s", filename);
            perror(value);
            ret = fd;
            break;
        }

        ret = read(fd, value, MAX_STR_BUF_SIZE);
        if (ret < 0) {
            sprintf(value, "couldn't read from %s", filename);
            perror(value);
        }

        value[ret - 1] = '\0';

        close(fd);

        /* compare with device name */
        if ( !strcmp(value, name) ) {
            /* match found */
            found_index = i;
            break;
        }
    }

    return found_index;
}

/**
 *
 * Thread function
 *
 */
void* monitorFactoryReset(void *args)
{
    logButtonInfo("Monitor Factory reset thread started\n");

    int ret = 0;
    char device[MAX_STR_BUF_SIZE];

    struct mosquitto *mosq = (struct mosquitto *)args;

    ret = find_event_num("gpio_keys");
    if(ret < 0)
    {
        logButtonInfo("Error : gpio_keys event not found : %d.\n",ret);
        return NULL;
    }

    sprintf(device, "/dev/input/event%d", ret);

    ret = monitor_button(device, grab_flag, mosq);
    if(ret < 0)
    {
        logButtonInfo("Error : Button monitor failed : %d.\n",ret);

    }

    return NULL;
}
