/*
* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
#ifndef VERIFY_PRINT_ERROR
#define VERIFY_PRINT_ERROR
#endif

#include "verify.h"

#include "remote.h"
#include "shared.h"
#include "fastrpc_internal.h"
#include "fastrpc_apps_user.h"
#include "adsp_current_process.h"
#include "remotectl.h"
#include "rpcmem.h"
#include "AEEstd.h"
#include "AEEQList.h"
#include "apps_std.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#ifndef _WIN32
#include <pthread.h>
#include <sys/inotify.h>
#include <sys/eventfd.h>
#include <poll.h>
#endif

#ifndef INT_MAX
#define INT_MAX (int)(-1)
#endif

#ifndef FASTRPC_DEVICE
#define FASTRPC_DEVICE "/dev/adsprpc-smd"
#endif

#define EVENT_SIZE          ( sizeof (struct inotify_event) )
#define EVENT_BUF_LEN       ( 1024 * ( EVENT_SIZE + 16 ) )

struct mem_to_fd
{
	QNode qn;
	void *buf;
	int size;
	int fd;
	uintptr_t vaddr;
};

struct mem_to_fd_list
{
	QList ql;
	pthread_mutex_t mut;
};

struct log_config_watcher_params
{
	int fd;
	int event_fd; /* Duplicate fd to quit the poll */
	_cstring1_t *paths;
	int *wd;
	uint32 numPaths;
	pthread_t thread;
	unsigned char stopThread;
	int asidToWatch;
	char *fileToWatch;
	char *asidFileToWatch;
	char *pidFileToWatch;
};

static struct mem_to_fd_list fdlist;
static struct log_config_watcher_params log_config_watcher = {0};
extern const char *__progname;
static int gdev = -1;
static int glist = -1;
static struct fastrpc_ioctl_init ginit = {0};
static pthread_key_t tlsKey = 0;

int listener_init(void);
void apps_mem_init(void);
void apps_mem_deinit(void);
void apps_std_init(void);
void apps_std_deinit(void);

void remote_register_buf(void *buf, int size, int fd)
{
	struct mem_to_fd *tofd = 0, *freefd = 0;
	int nErr = 0;
	if (fd != -1)
	{
		VERIFY(0 != (tofd = malloc(sizeof(*tofd))));
		QNode_CtorZ(&tofd->qn);
		tofd->buf = buf;
		tofd->size = size;
		tofd->fd = fd;
		tofd->vaddr = 0;
		pthread_mutex_lock(&fdlist.mut);
		QList_AppendNode(&fdlist.ql, &tofd->qn);
		pthread_mutex_unlock(&fdlist.mut);
		if (gdev != -1)
		{
			(void)remote_mmap64(fd, 0, (uintptr_t)buf, size, &tofd->vaddr);
		}
	}
	else
	{
		QNode *pn, *pnn;
		pthread_mutex_lock(&fdlist.mut);
		QLIST_NEXTSAFE_FOR_ALL(&fdlist.ql, pn, pnn)
		{
			tofd = STD_RECOVER_REC(struct mem_to_fd, qn, pn);
			if (tofd->buf == buf && tofd->size == size)
			{
				QNode_DequeueZ(&tofd->qn);
				freefd = tofd;
				break;
			}
		}
		pthread_mutex_unlock(&fdlist.mut);
		if (freefd)
		{
			if (freefd->vaddr)
			{
				(void)remote_munmap64(freefd->vaddr, freefd->size);
			}
			free(freefd);
		}
	}
bail:
	return;
}

static int fdlist_fd_to_buf(void *buf)
{
	QNode *pn;
	int fd = -1;
	pthread_mutex_lock(&fdlist.mut);
	QLIST_FOR_ALL(&fdlist.ql, pn)
	{
		if (fd != -1)
		{
			break;
		}
		else
		{
			struct mem_to_fd *tofd = STD_RECOVER_REC(struct mem_to_fd, qn, pn);
			if (STD_BETWEEN(buf, tofd->buf, (unsigned long)tofd->buf + tofd->size))
			{
				fd = tofd->fd;
			}
		}
	}
	pthread_mutex_unlock(&fdlist.mut);
	return fd;
}

static void exit_thread(void *value)
{
	(void)value;
	VERIFY_IPRINTF("exiting thread");
	(void)adsp_current_process_thread_exit();
}

static void create_key(void)
{
	(void)pthread_key_create(&tlsKey, exit_thread);
}

static void attach_dev(void)
{
	int nErr = 0;
	int dev = gdev;
	if (dev == -1)
	{
		dev = open(FASTRPC_DEVICE, O_NONBLOCK);
		VERIFY(dev >= 0);
	}
	ginit.flags = FASTRPC_INIT_ATTACH;
	VERIFY((0 == ioctl(dev, FASTRPC_IOCTL_INIT, (unsigned long)&ginit)) || errno == ENOTTY);
	gdev = dev;
bail:
	if (nErr && (dev >= 0))
	{
		close(dev);
		gdev = -1;
	}
}

static void free_init_mem(void)
{
	if (ginit.flags == FASTRPC_INIT_CREATE)
	{
		if (ginit.file)
		{
			rpcmem_free(ginit.file);
			ginit.file = 0;
		}
		if (ginit.mem)
		{
			rpcmem_free(ginit.mem);
			ginit.mem = 0;
		}
	}
}

static void create_dev(void)
{
	int nErr = 0;
	apps_std_FILE fh = -1;
	uint64 len;
	int dev = -1;
	if (gdev == -1)
	{
		int readlen = 0, eof;

		dev = open(FASTRPC_DEVICE, O_NONBLOCK);
		VERIFY(dev >= 0);
		VERIFY(!apps_std_fopen_with_env("ADSP_LIBRARY_PATH", ";", "fastrpc_shell_0", "r", &fh));
		VERIFY(!apps_std_flen(fh, &len));
		VERIFY(len < INT_MAX);
		ginit.flags = FASTRPC_INIT_CREATE;
		ginit.file = rpcmem_alloc(0, RPCMEM_HEAP_DEFAULT, (int)len);
		VERIFY(ginit.file);
		ginit.filelen = (int)len;
		VERIFY(!apps_std_fread(fh, ginit.file, len, &readlen, &eof));
		VERIFY(ginit.filelen == readlen);
		ginit.filefd = rpcmem_to_fd((void *)ginit.file);
		ginit.memlen = STD_MAX(1024 * 1024 * 2, (int)len * 4);
		ginit.mem = rpcmem_alloc(0, RPCMEM_HEAP_DEFAULT, ginit.memlen);
		VERIFY(ginit.mem);
		ginit.memfd = rpcmem_to_fd(ginit.mem);
		VERIFY(0 == ioctl(dev, FASTRPC_IOCTL_INIT, (unsigned long)&ginit));
		gdev = dev;
		dev = -1;
	}
bail:
	if (nErr)
	{
		free_init_mem();
	}
	if (fh > -1)
	{
		if (dev >= 0)
		{
			close(dev);
		}
		apps_std_fclose(fh);
	}
	else
	{
		gdev = dev;
		attach_dev();
	}
}

static void create_listener(void)
{
	glist = listener_init();
}

static int open_dev(int attach)
{
	if (gdev == -1)
	{
		static pthread_once_t once_dev = PTHREAD_ONCE_INIT;
		static pthread_once_t once_listener = PTHREAD_ONCE_INIT;
		static pthread_once_t once_ck = PTHREAD_ONCE_INIT;
		int rv = 0;
		if (attach)
		{
			rv = pthread_once(&once_dev, attach_dev);
		}
		else
		{
			rv = pthread_once(&once_dev, create_dev);
		}
		if (rv != 0 || gdev == -1)
		{
			VERIFY_EPRINTF("%s device initialization error: %s\n", FASTRPC_DEVICE, strerror(errno));
			return gdev;
		}
		rv = pthread_once(&once_listener, create_listener);
		if (rv != 0 || glist != 0)
		{
			VERIFY_EPRINTF("listener unsupported: %d", glist);
			return gdev;
		}
		rv = pthread_once(&once_ck, create_key);
		if (rv != 0 || tlsKey == 0)
		{
			VERIFY_EPRINTF("failed to create tls key");
			return gdev;
		}

	}
	return gdev;
}

static int parseLogConfig(unsigned short mask, char *filenames)
{
	_cstring1_t *filesToLog = NULL;
	int filesToLogLen = 0;
	char *tempFiles = NULL;
	int nErr = 0;
	char *saveptr = NULL;
	char *path = NULL;
	char delim[] = {','};
	int maxPathLen = 0;
	int i = 0;

	VERIFY(filenames != NULL);

	VERIFY(NULL != (tempFiles = malloc(sizeof(char) * (std_strlen(filenames) + 1))));
	std_strlcpy(tempFiles, filenames, std_strlen(filenames) + 1);

	/* Get the number of folders and max size needed */
	path = strtok_r(tempFiles, delim, &saveptr);
	while (path != NULL)
	{
		maxPathLen = STD_MAX(maxPathLen, std_strlen(path)) + 1;
		filesToLogLen++;
		path = strtok_r(NULL, delim, &saveptr);
	}

	VERIFY_IPRINTF("%s: #files: %d max_len: %d\n", log_config_watcher.fileToWatch, filesToLogLen, maxPathLen);

	/* Allocate memory */
	VERIFY(NULL != (filesToLog = malloc(sizeof(_cstring1_t) * filesToLogLen)));
	for (i = 0; i < filesToLogLen; ++i)
	{
		VERIFY(NULL != (filesToLog[i].data = malloc(sizeof(char) * maxPathLen)));
		filesToLog[i].dataLen = maxPathLen;
	}

	/* Get the number of folders and max size needed */
	std_strlcpy(tempFiles, filenames, std_strlen(filenames) + 1);
	i = 0;
	path = strtok_r(tempFiles, delim, &saveptr);
	while (path != NULL)
	{
		VERIFY((filesToLog != NULL) && (filesToLog[i].data != NULL) &&
		       filesToLog[i].dataLen >= strlen(path));
		std_strlcpy(filesToLog[i].data, path, filesToLog[i].dataLen);
		VERIFY_IPRINTF("%s: %s\n", log_config_watcher.fileToWatch, filesToLog[i].data);
		path = strtok_r(NULL, delim, &saveptr);
		i++;
	}

	VERIFY(0 == adsp_current_process_set_logging_params(mask, filesToLog, filesToLogLen));

bail:
	if (filesToLog)
	{
		for (i = 0; i < filesToLogLen; ++i)
		{
			if (filesToLog[i].data != NULL)
			{
				free(filesToLog[i].data);
				filesToLog[i].data = NULL;
			}
		}
		free(filesToLog);
		filesToLog = NULL;
	}

	if (tempFiles)
	{
		free(tempFiles);
		tempFiles = NULL;
	}
	return nErr;
}

/* Read log config given the filename */
static int readLogConfigFromPath(const char *base, const char *file)
{
	int nErr = 0;
	apps_std_FILE fp = -1;
	uint64 len;
	byte *buf = NULL;
	int readlen = 0, eof;
	unsigned short mask = 0;
	char *path = NULL;
	char *filenames = NULL;
	boolean fileExists = FALSE;

	len = std_snprintf(0, 0, "%s/%s", base, file) + 1;
	VERIFY(NULL != (path =  malloc(sizeof(char) * len)));
	VERIFY(len >= std_snprintf(path, len, "%s/%s", base, file));

	VERIFY(0 == apps_std_fileExists(path, &fileExists));
	if (fileExists == FALSE)
	{
		VERIFY_IPRINTF("%s: Couldn't find file: %s\n", log_config_watcher.fileToWatch, path);
		nErr = -1;
		goto bail;
	}

	VERIFY(0 == apps_std_fopen(path, "r", &fp));
	VERIFY(0 == apps_std_flen(fp, &len));

	VERIFY(len < INT_MAX);

	VERIFY(NULL != (buf = malloc(sizeof(byte) * len)));
	VERIFY(NULL != (filenames = malloc(sizeof(byte) * len)));
	VERIFY(!apps_std_fread(fp, buf, len, &readlen, &eof));
	VERIFY(len == readlen);

	VERIFY_IPRINTF("%s: Config file %s contents: %s\n", log_config_watcher.fileToWatch, path, buf);

	len = sscanf((const char *)buf, "0x%hx %s", &mask, filenames);
	switch (len)
	{
		case 1:
			VERIFY_IPRINTF("%s: Setting log mask:0x%x", log_config_watcher.fileToWatch, mask);
			VERIFY(0 == adsp_current_process_set_logging_params(mask, NULL, 0));
			break;
		case 2:
			VERIFY(0 == parseLogConfig(mask, filenames));
			VERIFY_IPRINTF("%s: Setting log mask:0x%x, filename:%s", log_config_watcher.fileToWatch, mask, filenames);
			break;
		default:
			VERIFY_EPRINTF("%s: No valid data found in config file %s", log_config_watcher.fileToWatch, path);
			nErr = -1;
			goto bail;
	}

bail:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}

	if (filenames != NULL)
	{
		free(filenames);
		filenames = NULL;
	}

	if (fp != -1)
	{
		apps_std_fclose(fp);
	}

	if (path != NULL)
	{
		free(path);
		path = NULL;
	}


	return nErr;
}

/* Read log config given the watch descriptor */
static int readLogConfigFromEvent(struct inotify_event *event)
{
	int i = 0;

	/* Ensure we are looking at the right file */
	for (i = 0; i < log_config_watcher.numPaths; ++i)
	{
		if (log_config_watcher.wd[i] == event->wd)
		{
			if (std_strcmp(log_config_watcher.fileToWatch, event->name) == 0)
			{
				return readLogConfigFromPath(log_config_watcher.paths[i].data, log_config_watcher.fileToWatch);
			}
			else if (std_strcmp(log_config_watcher.asidFileToWatch, event->name) == 0)
			{
				return readLogConfigFromPath(log_config_watcher.paths[i].data, log_config_watcher.asidFileToWatch);
			}
			else if (std_strcmp(log_config_watcher.pidFileToWatch, event->name) == 0)
			{
				return readLogConfigFromPath(log_config_watcher.paths[i].data, log_config_watcher.pidFileToWatch);
			}
		}
	}
	VERIFY_IPRINTF("%s: Watch descriptor %d not valid for current process", log_config_watcher.fileToWatch, event->wd);
	return 0;
}

/* Read log config given the watch descriptor */
static int resetLogConfigFromEvent(struct inotify_event *event)
{
	int i = 0;

	/* Ensure we are looking at the right file */
	for (i = 0; i < log_config_watcher.numPaths; ++i)
	{
		if (log_config_watcher.wd[i] == event->wd)
		{
			if ((std_strcmp(log_config_watcher.fileToWatch, event->name) == 0) ||
			        (std_strcmp(log_config_watcher.asidFileToWatch, event->name) == 0) ||
			        (std_strcmp(log_config_watcher.pidFileToWatch, event->name) == 0))
			{
				return adsp_current_process_set_logging_params(0, NULL, 0);
			}
		}
	}
	VERIFY_IPRINTF("%s: Watch descriptor %d not valid for current process", log_config_watcher.fileToWatch, event->wd);
	return 0;
}

static void *file_watcher_thread(void *arg)
{
	int ret = 0;
	int length = 0;
	int nErr = 0;
	int i = 0;
	char buffer[EVENT_BUF_LEN];
	struct pollfd pfd[] =
	{
		{log_config_watcher.fd,     POLLIN, 0},
		{log_config_watcher.event_fd, POLLIN, 0}
	};
	const char *fileExtension = ".farf";
	int len = 0;

	/* Check for the presence of the <process_name>.farf file at bootup */
	for (i = 0; i < log_config_watcher.numPaths; ++i)
	{
		if (0 == readLogConfigFromPath(log_config_watcher.paths[i].data, log_config_watcher.fileToWatch))
		{
			VERIFY_IPRINTF("%s: Log config File %s found.\n", log_config_watcher.fileToWatch, log_config_watcher.paths[i].data);
		}
	}

	while (log_config_watcher.stopThread == 0)
	{
		/* Block forever */
		ret = poll(pfd, 2, -1);
		if (ret < 0)
		{
			VERIFY_EPRINTF("%s: Error polling for file change. Runtime FARF will not work for this process. errno=%d !", log_config_watcher.fileToWatch, errno);
			break;
		}
		else if (pfd[1].revents & POLLIN)     /* Check for exit */
		{
			VERIFY_EPRINTF("Received exit.\n");
			break;
		}
		else
		{
			length = read(log_config_watcher.fd, buffer, EVENT_BUF_LEN);
			i = 0;
			while (i < length)
			{
				struct inotify_event *event = (struct inotify_event *) &buffer[ i ];
				if (event->len)
				{
					/* Get the asiD for the current process.
					 Do it once only */
					if (log_config_watcher.asidToWatch == -1)
					{
						VERIFY(0 == adsp_current_process_getASID((unsigned int *)&log_config_watcher.asidToWatch));
						len = strlen(fileExtension) + strlen(__TOSTR__(INT_MAX));
						VERIFY(NULL != (log_config_watcher.asidFileToWatch = malloc(sizeof(char) * len)));
						snprintf(log_config_watcher.asidFileToWatch, len, "%d%s", log_config_watcher.asidToWatch, fileExtension);
						VERIFY_IPRINTF("%s: Watching ASID file %s\n", log_config_watcher.fileToWatch, log_config_watcher.asidFileToWatch);
					}

					VERIFY_IPRINTF("%s: %s %d.\n", log_config_watcher.fileToWatch, event->name, event->mask);
					if ((event->mask & IN_CREATE) || (event->mask & IN_MODIFY))
					{
						VERIFY_IPRINTF("%s: File %s created.\n", log_config_watcher.fileToWatch, event->name);
						if (0 != readLogConfigFromEvent(event))
						{
							VERIFY_EPRINTF("%s: Error reading config file %s", log_config_watcher.fileToWatch, log_config_watcher.paths[i].data);
						}
					}
					else if (event->mask & IN_DELETE)
					{
						VERIFY_IPRINTF("%s: File %s deleted.\n", log_config_watcher.fileToWatch, event->name);
						if (0 != resetLogConfigFromEvent(event))
						{
							VERIFY_EPRINTF("%s: Error resetting FARF runtime log config", log_config_watcher.fileToWatch);
						}
					}
				}

				i += EVENT_SIZE + event->len;
			}
		}
	}
bail:
	if (nErr != 0)
	{
		VERIFY_EPRINTF("%s: Error in file watcher thread. Runtime FARF will not work for this process !", log_config_watcher.fileToWatch);
	}
	return NULL;
}

void deinitFileWatcher()
{
	int i = 0;
	uint64 stop = 10;

	/* Destroy the file watcher thread */
	log_config_watcher.stopThread = 1;
	write(log_config_watcher.event_fd, &stop, sizeof(uint64));

	pthread_join(log_config_watcher.thread, NULL);

	if (log_config_watcher.fileToWatch)
	{
		free(log_config_watcher.fileToWatch);
	}

	if (log_config_watcher.asidFileToWatch)
	{
		free(log_config_watcher.asidFileToWatch);
	}

	if (log_config_watcher.pidFileToWatch)
	{
		free(log_config_watcher.pidFileToWatch);
	}

	if (log_config_watcher.wd)
	{
		for (i = 0; i < log_config_watcher.numPaths; ++i)
		{
			if (log_config_watcher.wd[i] != 0)
			{
				inotify_rm_watch(log_config_watcher.fd, log_config_watcher.wd[i]);
			}
		}
		free(log_config_watcher.wd);
		log_config_watcher.wd = NULL;
	}

	if (log_config_watcher.paths)
	{
		for (i = 0; i < log_config_watcher.numPaths; ++i)
		{
			if (log_config_watcher.paths[i].data)
			{
				free(log_config_watcher.paths[i].data);
				log_config_watcher.paths[i].data = NULL;
			}
		}
		free(log_config_watcher.paths);
		log_config_watcher.paths = NULL;
	}

	if (log_config_watcher.fd != 0)
	{
		close(log_config_watcher.fd);
		log_config_watcher.fd = 0;
	}

	log_config_watcher.numPaths = 0;
}

int initFileWatcher()
{
	int nErr = 0;
	const char *fileExtension = ".farf";
	uint32 len = 0;
	uint16 maxPathLen = 0;
	int i = 0;
	char *name = NULL;

	log_config_watcher.asidToWatch = 0;
	VERIFY(NULL != (name = std_basename(__progname)));

	len = strlen(name) + strlen(fileExtension) + 1;
	VERIFY(NULL != (log_config_watcher.fileToWatch = malloc(sizeof(char) * len)));
	snprintf(log_config_watcher.fileToWatch, len, "%s%s", name, fileExtension);

	len = strlen(fileExtension) + strlen(__TOSTR__(INT_MAX));
	VERIFY(NULL != (log_config_watcher.pidFileToWatch = malloc(sizeof(char) * len)));
	snprintf(log_config_watcher.pidFileToWatch, len, "%d%s", getpid(), fileExtension);

	VERIFY_IPRINTF("%s: Watching PID file: %s\n", log_config_watcher.fileToWatch, log_config_watcher.pidFileToWatch);

	log_config_watcher.fd = inotify_init();
	if (log_config_watcher.fd < 0)
	{
		nErr = -1;
		VERIFY_EPRINTF("Error in inotify_init. errno = %d\n", errno);
		goto bail;
	}

	/* Duplicate the fd, so we can use it to quit polling */
	log_config_watcher.event_fd = eventfd(0, 0);
	if (log_config_watcher.event_fd < 0)
	{
		nErr = -1;
		VERIFY_EPRINTF("Error in dup. errno = %d\n", errno);
		goto bail;
	}
	VERIFY_IPRINTF("fd = %d dupfd=%d\n", log_config_watcher.fd, log_config_watcher.event_fd);

	/* Get the required size */
	apps_std_get_search_paths_with_env("ADSP_LIBRARY_PATH", ";", NULL, 0,
	                                   &log_config_watcher.numPaths, &maxPathLen);

	maxPathLen += + 1;

	/* Allocate memory */
	VERIFY(NULL != (log_config_watcher.paths
	                = malloc(sizeof(_cstring1_t) * log_config_watcher.numPaths)));
	VERIFY(NULL != (log_config_watcher.wd
	                = malloc(sizeof(int) * log_config_watcher.numPaths)));

	for (i = 0; i < log_config_watcher.numPaths; ++i)
	{
		VERIFY(NULL != (log_config_watcher.paths[i].data
		                = malloc(sizeof(char) * maxPathLen)));
		log_config_watcher.paths[i].dataLen = maxPathLen;
	}

	/* Get the paths */
	VERIFY(0 == apps_std_get_search_paths_with_env("ADSP_LIBRARY_PATH", ";",
	        log_config_watcher.paths, log_config_watcher.numPaths, &len, &maxPathLen));

	maxPathLen += 1;

	VERIFY_IPRINTF("%s: Watching folders:\n", log_config_watcher.fileToWatch);
	for (i = 0; i < log_config_watcher.numPaths; ++i)
	{
		/* Watch for creation, deletion and modification of files in path */
		VERIFY_IPRINTF("%s: %s\n", log_config_watcher.fileToWatch, log_config_watcher.paths[i].data);
		VERIFY(0 < (log_config_watcher.wd[i] = inotify_add_watch(log_config_watcher.fd,
		                                       log_config_watcher.paths[i].data,  IN_CREATE | IN_DELETE)));
	}

	/* Create a thread to watch for file changes */
	log_config_watcher.asidToWatch = -1;
	log_config_watcher.stopThread = 0;
	pthread_create(&log_config_watcher.thread, NULL, file_watcher_thread, NULL);
bail:
	if (nErr != 0)
	{
		VERIFY_EPRINTF("%s: Failed to register with inotify. Runtime FARF will not work for the process %s!", log_config_watcher.fileToWatch, name);
		deinitFileWatcher();
	}

	return nErr;
}

static int fastrpc_apps_user_init()
{
	pthread_mutex_init(&fdlist.mut, 0);
	QList_Ctor(&fdlist.ql);
	rpcmem_init();
	apps_mem_init();
	apps_std_init();
	return 0;
}

int listener_deinit();
static int fastrpc_apps_user_deinit()
{
	int olddev = gdev;


	VERIFY_EPRINTF("exit:");
	if (olddev != -1)
	{
		adsp_current_process_exit();
		listener_deinit();
		gdev = -1;
		VERIFY_EPRINTF("exit: closing %s, rpc errors are expected.\n", FASTRPC_DEVICE);
		close(olddev);
	}
	if (tlsKey)
	{
		pthread_key_delete(tlsKey);
		tlsKey = 0;
	}

	apps_mem_deinit();
	apps_std_deinit();
	free_init_mem();
	rpcmem_deinit();
	pthread_mutex_destroy(&fdlist.mut);
	return 0;
}

SHARED_OBJECT_API_ENTRY(fastrpc_apps_user_init, fastrpc_apps_user_deinit);

int remote_handle_invoke(remote_handle handle, uint32_t sc, remote_arg *pra)
{
	struct fastrpc_ioctl_invoke_fd invoke;
	int fds[32], *pfds = 0, bufs, i, req, nErr = 0;
	int dev = open_dev(0);
	VERIFY(dev != -1);
	invoke.inv.handle = handle;
	invoke.inv.sc = sc;
	invoke.inv.pra = pra;

	bufs = REMOTE_SCALARS_LENGTH(sc);
	if (bufs > STD_ARRAY_SIZE(fds))
	{
		VERIFY(0 != (pfds = malloc(bufs * sizeof(*pfds))));
	}
	else
	{
		pfds = fds;
	}
	invoke.fds = pfds;
	for (i = 0; i < bufs; i++)
	{
		if (pra[i].buf.nLen)
		{
			pfds[i] = fdlist_fd_to_buf(pra[i].buf.pv);
		}
		else
		{
			pfds[i] = -1;
		}
	}
	req = FASTRPC_IOCTL_INVOKE;
	for (i = 0; i < bufs; i++)
	{
		if (pfds[i] != -1)
		{
			req = FASTRPC_IOCTL_INVOKE_FD;
			break;
		}
	}
	if (0 == pthread_getspecific(tlsKey))
	{
		pthread_setspecific(tlsKey, (void *)1);
	}
	nErr = ioctl(dev, req, (unsigned long)&invoke);
bail:
	if (pfds && (pfds != fds))
	{
		free(pfds);
	}
	return nErr;
}

int listener_geteventfd(int *fd);
int remote_handle_open(const char *name, remote_handle *ph)
{
	char dlerrstr[255];
	int dlerr = 0, nErr = 0;
	if (!std_strcmp(name, ITRANSPORT_PREFIX "geteventfd"))
	{
		return listener_geteventfd((int *)ph);
	}
	if (!std_strcmp(name, ITRANSPORT_PREFIX "attachguestos"))
	{
		int dev = open_dev(1);
		return dev == -1;
	}
	VERIFY(0 == (nErr = remotectl_open(name, (int *)ph, dlerrstr, sizeof(dlerrstr), &dlerr)));

	VERIFY(0 == (nErr = dlerr));
bail:
	if (dlerr != 0)
	{
		VERIFY_EPRINTF("adsp dlopen error: %s %s", name, dlerrstr);
	}
	return nErr;
}

int remote_handle_close(remote_handle h)
{
	char dlerrstr[255];
	int dlerr = 0, nErr = 0;
	VERIFY(0 == (nErr = remotectl_close(h, dlerrstr, sizeof(dlerrstr), &dlerr)));
	VERIFY(0 == (nErr = dlerr));
bail:
	return nErr;
}

int remote_mmap64(int fd, uint32_t flags, uintptr_t vaddrin, int64_t size, uintptr_t *vaddrout)
{
	int nErr = 0;
	struct fastrpc_ioctl_mmap mmap;
	int dev = open_dev(0);
	if (dev == -1)
	{
		return -1;
	}
	mmap.fd  = fd;
	mmap.flags  = flags;
	mmap.vaddrin = vaddrin;
	mmap.size = size;
	VERIFY(0 == (nErr = ioctl(dev, FASTRPC_IOCTL_MMAP, (unsigned long)&mmap)));
	*vaddrout = mmap.vaddrout;
bail:
	return nErr;
}

int remote_mmap(int fd, uint32_t flags, uint32_t vaddrin, int size, uint32_t *vaddrout)
{
	return remote_mmap64(fd, flags, (uintptr_t)vaddrin, (int64_t)size, (uintptr_t *)vaddrout);
}

int remote_munmap64(uintptr_t vaddrout, int64_t size)
{
	struct fastrpc_ioctl_munmap munmap;
	int dev = open_dev(0);
	if (dev == -1)
	{
		return -1;
	}
	munmap.vaddrout = vaddrout;
	munmap.size = size;
	return ioctl(dev, FASTRPC_IOCTL_MUNMAP, (unsigned long)&munmap);
}

int remote_munmap(uint32_t vaddrout, int size)
{
	return remote_munmap64((uintptr_t)vaddrout, (int64_t)size);
}

int remote_set_mode(uint32_t mode)
{
	int dev = open_dev(0);
	if (dev == -1)
	{
		return -1;
	}
	return ioctl(dev, FASTRPC_IOCTL_SETMODE, mode);
}
