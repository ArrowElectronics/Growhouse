/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/functionfs.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "usb.h"
#include "qdiag.h"

#define MAX_PACKET_SIZE_FS    64
#define MAX_PACKET_SIZE_HS    512
#define MAX_PACKET_SIZE_SS    1024

#define cpu_to_le16(x)  (x)
#define cpu_to_le32(x)  (x)

struct usb_handle
{
    pthread_cond_t notify, ready;
    pthread_mutex_t lock;

    int (*write)(struct usb_handle *h, const void *data, int len);
    int (*read)(struct usb_handle *h, void *data, int len);
    void (*kick)(struct usb_handle *h);

    // FunctionFS
    int control;
    int bulk_out; /* "out" from the host's perspective => source for qdiagd */
    int bulk_in;  /* "in" from the host's perspective => sink for qdiagd */
};

struct func_desc {
    struct usb_interface_descriptor intf;
    struct usb_endpoint_descriptor_no_audio source;
    struct usb_endpoint_descriptor_no_audio sink;
} __attribute__((packed));

struct ss_func_desc {
    struct usb_interface_descriptor intf;
    struct usb_endpoint_descriptor_no_audio source;
    struct usb_ss_ep_comp_descriptor source_comp;
    struct usb_endpoint_descriptor_no_audio sink;
    struct usb_ss_ep_comp_descriptor sink_comp;
} __attribute__((packed));

struct desc_v1 {
    struct usb_functionfs_descs_head_v1 {
        __le32 magic;
        __le32 length;
        __le32 fs_count;
        __le32 hs_count;
    } __attribute__((packed)) header;
    struct func_desc fs_descs, hs_descs;
} __attribute__((packed));

struct desc_v2 {
    struct usb_functionfs_descs_head_v2 header;
    // The rest of the structure depends on the flags in the header.
    __le32 fs_count;
    __le32 hs_count;
    __le32 ss_count;
    struct func_desc fs_descs, hs_descs;
    struct ss_func_desc ss_descs;
} __attribute__((packed));

struct func_desc fs_descriptors = {
    .intf = {
        .bLength = sizeof(fs_descriptors.intf),
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
        .bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
        .bInterfaceProtocol = 0xff,
        .iInterface = 1, /* first string from the provided table */
    },
    .source = {
        .bLength = sizeof(fs_descriptors.source),
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 1 | USB_DIR_OUT,
        .bmAttributes = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize = MAX_PACKET_SIZE_FS,
    },
    .sink = {
        .bLength = sizeof(fs_descriptors.sink),
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 2 | USB_DIR_IN,
        .bmAttributes = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize = MAX_PACKET_SIZE_FS,
    },
};

struct func_desc hs_descriptors = {
    .intf = {
        .bLength = sizeof(hs_descriptors.intf),
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
        .bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
        .bInterfaceProtocol = 0xff,
        .iInterface = 1, /* first string from the provided table */
    },
    .source = {
        .bLength = sizeof(hs_descriptors.source),
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 1 | USB_DIR_OUT,
        .bmAttributes = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize = MAX_PACKET_SIZE_HS,
    },
    .sink = {
        .bLength = sizeof(hs_descriptors.sink),
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 2 | USB_DIR_IN,
        .bmAttributes = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize = MAX_PACKET_SIZE_HS,
    },
};

static struct ss_func_desc ss_descriptors = {
    .intf = {
        .bLength = sizeof(ss_descriptors.intf),
        .bDescriptorType = USB_DT_INTERFACE,
        .bInterfaceNumber = 0,
        .bAlternateSetting = 0,
        .bNumEndpoints = 2,
        .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
        .bInterfaceSubClass = USB_SUBCLASS_VENDOR_SPEC,
        .bInterfaceProtocol = 0xff,
        .iInterface = 1, /* first string from the provided table */
    },
    .source = {
        .bLength = sizeof(ss_descriptors.source),
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 1 | USB_DIR_OUT,
        .bmAttributes = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize = MAX_PACKET_SIZE_SS,
    },
    .source_comp = {
        .bLength = sizeof(ss_descriptors.source_comp),
        .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
    },
    .sink = {
        .bLength = sizeof(ss_descriptors.sink),
        .bDescriptorType = USB_DT_ENDPOINT,
        .bEndpointAddress = 2 | USB_DIR_IN,
        .bmAttributes = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize = MAX_PACKET_SIZE_SS,
    },
    .sink_comp = {
        .bLength = sizeof(ss_descriptors.sink_comp),
        .bDescriptorType = USB_DT_SS_ENDPOINT_COMP,
    },
};

#define STR_INTERFACE_ "Qdiag Interface"

static const struct {
    struct usb_functionfs_strings_head header;
    struct {
        __le16 code;
        const char str1[sizeof(STR_INTERFACE_)];
    } __attribute__((packed)) lang0;
} __attribute__((packed)) strings = {
    .header = {
        .magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
        .length = cpu_to_le32(sizeof(strings)),
        .str_count = cpu_to_le32(1),
        .lang_count = cpu_to_le32(1),
    },
    .lang0 = {
        cpu_to_le16(0x0409), /* en-us */
        STR_INTERFACE_,
    },
};

struct usb_handle* g_h = NULL;
const char * USB_DEVNAME = "/dev/gadget/qdiag";
char USB_SERIAL[64] = "0123456789";
char * USB_FFS_EP0;
char * USB_FFS_OUT;
char * USB_FFS_IN;
const char* UDCNAME = "ci_hdrc.0";
const char* GADGETPATH = "/sys/kernel/config/usb_gadget/g1";

static void init_functionfs(struct usb_handle *h)
{
    ssize_t ret;
    struct desc_v1 v1_descriptor;
    struct desc_v2 v2_descriptor;

    v2_descriptor.header.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2);
    v2_descriptor.header.length = cpu_to_le32(sizeof(v2_descriptor));
    v2_descriptor.header.flags = FUNCTIONFS_HAS_FS_DESC | FUNCTIONFS_HAS_HS_DESC |
                                 FUNCTIONFS_HAS_SS_DESC;
    v2_descriptor.fs_count = 3;
    v2_descriptor.hs_count = 3;
    v2_descriptor.ss_count = 5;
    v2_descriptor.fs_descs = fs_descriptors;
    v2_descriptor.hs_descs = hs_descriptors;
    v2_descriptor.ss_descs = ss_descriptors;

    diag_dbg(DIAG_DBG_TRANSPORT, "OPENING %s\n", USB_FFS_EP0);
    h->control = open(USB_FFS_EP0, O_RDWR);
    if (h->control < 0) {
        diag_dbg(DIAG_DBG_TRANSPORT, "[ %s: cannot open control endpoint: errno=%d]\n", USB_FFS_EP0, errno);
        goto err;
    }
    diag_info("Control ep %s -> fd=%d", USB_FFS_EP0, h->control);

    ret = write(h->control, &v2_descriptor, sizeof(v2_descriptor));
    if (ret < 0) {
        v1_descriptor.header.magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC);
        v1_descriptor.header.length = cpu_to_le32(sizeof(v1_descriptor));
        v1_descriptor.header.fs_count = 3;
        v1_descriptor.header.hs_count = 3;
        v1_descriptor.fs_descs = fs_descriptors;
        v1_descriptor.hs_descs = hs_descriptors;
        diag_dbg(DIAG_DBG_TRANSPORT, "[ %s: Switching to V1_descriptor format errno=%d ]\n", USB_FFS_EP0, errno);
        ret = write(h->control, &v1_descriptor, sizeof(v1_descriptor));
        if (ret < 0) {
            diag_dbg(DIAG_DBG_TRANSPORT, "[ %s: write descriptors failed: errno=%d ]\n", USB_FFS_EP0, errno);
            goto err;
        }
    }

    ret = write(h->control, &strings, sizeof(strings));
    if (ret < 0) {
        diag_dbg(DIAG_DBG_TRANSPORT, "[ %s: writing strings failed: errno=%d]\n", USB_FFS_EP0, errno);
        goto err;
    }

    USB_FFS_OUT = malloc(strlen(USB_DEVNAME)+strlen(USB_FFS_OUT_NAME));
    strcpy(USB_FFS_OUT, USB_DEVNAME);
    strcat(USB_FFS_OUT, "/");
    strcat(USB_FFS_OUT, USB_FFS_OUT_NAME);
    diag_dbg(DIAG_DBG_TRANSPORT, "OPENING %s\n", USB_FFS_OUT);
    h->bulk_out = open(USB_FFS_OUT, O_RDWR);
    if (h->bulk_out < 0) {
        diag_dbg(DIAG_DBG_TRANSPORT, "[ %s: cannot open bulk-out ep: errno=%d ]\n", USB_FFS_OUT, errno);
        goto err;
    }
    diag_info("Incoming ep %s -> fd=%d", USB_FFS_OUT, h->bulk_out);

    USB_FFS_IN = malloc(strlen(USB_DEVNAME)+strlen(USB_FFS_IN_NAME));
    strcpy(USB_FFS_IN, USB_DEVNAME);
    strcat(USB_FFS_IN, "/");
    strcat(USB_FFS_IN, USB_FFS_IN_NAME);
    diag_dbg(DIAG_DBG_TRANSPORT, "OPENING %s\n", USB_FFS_IN);
    h->bulk_in = open(USB_FFS_IN, O_RDWR);
    if (h->bulk_in < 0) {
        diag_dbg(DIAG_DBG_TRANSPORT, "[ %s: cannot open bulk-in ep: errno=%d ]\n", USB_FFS_IN, errno);
        goto err;
    }
    diag_info("Outgoing ep %s -> fd=%d", USB_FFS_IN, h->bulk_in);

    return;

err:
    if (h->bulk_in > 0) {
        close(h->bulk_in);
        h->bulk_in = -1;
    }
    if (h->bulk_out > 0) {
        close(h->bulk_out);
        h->bulk_out = -1;
    }
    if (h->control > 0) {
        close(h->control);
        h->control = -1;
    }
    return;
}

static void *usb_ffs_open_thread(void *x)
{
    struct usb_handle *usb = (struct usb_handle *)x;

    while (true) {
        // wait until the USB device needs opening
        pthread_mutex_lock(&usb->lock);
        while (usb->control != -1)
            pthread_cond_wait(&usb->notify, &usb->lock);
        pthread_mutex_unlock(&usb->lock);

        while (true) {
            init_functionfs(usb);

            if (usb->control >= 0)
                break;

            sleep(1000);
        }

        pthread_cond_signal(&usb->ready);

        diag_dbg(DIAG_DBG_TRANSPORT, "[ usb_thread - we are READY! ]\n");
    }

    // never gets here
    return 0;
}

static int bulk_write(int bulk_in, const uint8_t* buf, size_t length)
{
    size_t count = 0;
    int ret;

    do {
        ret = write(bulk_in, buf + count, length - count);
        if (ret < 0) {
            if (errno != EINTR)
                return ret;
        } else {
            count += ret;
        }
    } while (count < length);

    diag_dbg(DIAG_DBG_TRANSPORT, "[ bulk_write done fd=%d ]\n", bulk_in);
    return count;
}

static int usb_ffs_write(struct usb_handle* h, const void* data, int len)
{
    diag_dbg(DIAG_DBG_TRANSPORT, "about to write (fd=%d, len=%d)\n", h->bulk_in, len);
    int n = bulk_write(h->bulk_in, (const uint8_t*)(data), len);
    if (n != len) {
        diag_dbg(DIAG_DBG_TRANSPORT, "ERROR: fd = %d, n = %d: %s\n", h->bulk_in, n, strerror(errno));
        return -1;
    }
    diag_dbg(DIAG_DBG_TRANSPORT, "[ done fd=%d ]\n", h->bulk_in);
    return 0;
}

static int bulk_read(int bulk_out, uint8_t* buf, size_t length)
{
    size_t count = 0;
    int ret;

    do {
        ret = read(bulk_out, buf + count, length - count);
        if (ret < 0) {
            if (errno != EINTR) {
                diag_dbg(DIAG_DBG_TRANSPORT, "[ bulk_read failed fd=%d length=%zu count=%zu ]\n",
                                           bulk_out, length, count);
                return ret;
            }
        } else {
            count += ret;
        }
    } while (count < length);

    return count;
}

static int usb_ffs_read(struct usb_handle* h, void* data, int len)
{
    diag_dbg(DIAG_DBG_TRANSPORT, "about to read (fd=%d, len=%d)\n", h->bulk_out, len);
    int n = bulk_read(h->bulk_out, (uint8_t*)(data), len);
    if (n != len) {
        diag_dbg(DIAG_DBG_TRANSPORT, "ERROR: fd = %d, n = %d: %s\n", h->bulk_out, n, strerror(errno));
        return -1;
    }
    diag_dbg(DIAG_DBG_TRANSPORT, "[ done fd=%d ]\n", h->bulk_out);
    return 0;
}

static void usb_ffs_kick(struct usb_handle *h)
{
    int err;

    err = ioctl(h->bulk_in, FUNCTIONFS_CLEAR_HALT);
    if (err < 0)
        diag_dbg(DIAG_DBG_TRANSPORT, "[ kick: source (fd=%d) clear halt failed (%d) ]", h->bulk_in, errno);

    err = ioctl(h->bulk_out, FUNCTIONFS_CLEAR_HALT);
    if (err < 0)
        diag_dbg(DIAG_DBG_TRANSPORT, "[ kick: sink (fd=%d) clear halt failed (%d) ]", h->bulk_out, errno);

    pthread_mutex_lock(&h->lock);
    close(h->control);
    close(h->bulk_out);
    close(h->bulk_in);
    h->control = h->bulk_out = h->bulk_in = -1;

    // notify usb_ffs_open_thread that we are disconnected
    pthread_cond_signal(&h->notify);
    pthread_mutex_unlock(&h->lock);
}

static void usb_ffs_init()
{
    diag_dbg(DIAG_DBG_TRANSPORT, "[ usb_init - using FunctionFS ]\n");

    struct usb_handle* h = (struct usb_handle*)(calloc(1, sizeof(struct usb_handle)));
    if (h == NULL) diag_err("couldn't allocate usb_handle");

    h->write = usb_ffs_write;
    h->read = usb_ffs_read;
    h->kick = usb_ffs_kick;
    h->control = -1;
    h->bulk_out = -1;
    h->bulk_in = -1;

    pthread_cond_init(&h->notify, 0);
    pthread_cond_init(&h->ready, 0);
    pthread_mutex_init(&h->lock, 0);

    diag_dbg(DIAG_DBG_TRANSPORT, "[ usb_init - starting thread ]\n");
    pthread_t tid;
    pthread_attr_t   attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid, &attr, usb_ffs_open_thread, h)){
        diag_err("[ cannot create usb thread ]\n");
    } else {
        g_h = h;
    }
}

void usb_init()
{
    USB_FFS_EP0 = malloc(strlen(USB_DEVNAME)+strlen(USB_FFS_EP0_NAME));
    strcpy(USB_FFS_EP0, USB_DEVNAME);
    strcat(USB_FFS_EP0, "/");
    strcat(USB_FFS_EP0, USB_FFS_EP0_NAME);

    if (access(USB_FFS_EP0, F_OK) == 0) {
        diag_info("ep0 %s is ready", USB_FFS_EP0);
        usb_ffs_init();
    } else
        diag_err("ep0 %s not ready!!!", USB_FFS_EP0);
}

void usb_cleanup()
{
}

int usb_write(struct usb_handle *h, const void *data, int len)
{
    return h->write(h, data, len);
}

int usb_read(struct usb_handle *h, void *data, int len)
{
    return h->read(h, data, len);
}

int usb_close(struct usb_handle *h)
{
    return 0;
}

void usb_kick(struct usb_handle *h)
{
    h->kick(h);
}

static void enable_udc(bool enable)
{
    char* UDCGADGETPATH;
    char UDCATTRIB[5] = "/UDC";
    int f;

    UDCGADGETPATH = malloc(strlen(GADGETPATH) + strlen(UDCATTRIB) + 1);
    strcpy(UDCGADGETPATH, GADGETPATH);
    strcat(UDCGADGETPATH, UDCATTRIB);
    f = open(UDCGADGETPATH, O_WRONLY);
    if (f) {
        write(f, UDCNAME, strlen(UDCNAME));
    }
    close(f);
    diag_dbg(DIAG_DBG_TRANSPORT, "Wrote \"%s\" to %s\n", UDCNAME, UDCGADGETPATH);
}

int usb_init_device(const char* device, const char* serial, int *incoming, int *outgoing, const char* gadget, const char* udc)
{
    if (device && device[0]) {
        USB_DEVNAME = device;
    }
    if (serial && serial[0]) {
        strncpy(USB_SERIAL, serial, sizeof(USB_SERIAL) - 1);
    }
    if (gadget && gadget[0]) {
        GADGETPATH = gadget;
    }
    if (udc && udc[0]) {
        UDCNAME = udc;
    }
    usb_init();
    if (g_h != NULL) {
        diag_dbg(DIAG_DBG_TRANSPORT, "FSS inited waiting for all endpoints to be opened\n");
        pthread_mutex_lock(&g_h->lock);
        while (g_h->control == -1 || g_h->bulk_out == -1 || g_h->bulk_in == -1) {
            diag_dbg(DIAG_DBG_TRANSPORT, "Got a USB handle with fds Control=%d, OUT=%d, IN=%d\n", g_h->control, g_h->bulk_out, g_h->bulk_in);
            pthread_cond_wait(&g_h->ready, &g_h->lock);
        }
        pthread_mutex_unlock(&g_h->lock);
        enable_udc(true);
        diag_info("Got a USB handle with fds Control=%d, OUT=%d, IN=%d", g_h->control, g_h->bulk_out, g_h->bulk_in);
        *incoming = g_h->bulk_out;
        *outgoing = g_h->bulk_in;
        return g_h->control;
    }
    return -1;
}
