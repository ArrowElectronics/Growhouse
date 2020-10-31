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
#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma warning( disable : 4996 )
#endif

#include "verify.h"
#include "qtest_stdlib.h"
#include "AEEstd.h"
#include "AEEatomic.h"
#include "AEEQList.h"
#include "apps_std.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#ifndef C_ASSERT
#define C_ASSERT(test) \
    switch(0) {\
      case 0:\
      case test:;\
    }
#endif

#define APPS_FD_BASE 100
#define ERRNO (errno == 0 ? -1 : errno)

struct apps_std_info
{
	QNode qn;
	FILE *stream;
	apps_std_FILE fd;
};

static QList apps_std_qlst;
static pthread_mutex_t apps_std_mt;

int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);

void apps_std_init(void)
{
	QList_Ctor(&apps_std_qlst);
	pthread_mutex_init(&apps_std_mt, 0);
}

void apps_std_deinit(void)
{
	pthread_mutex_destroy(&apps_std_mt);
}

static int apps_std_FILE_free(apps_std_FILE fd)
{
	struct apps_std_info *sinfo, *sfree = 0;
	QNode *pn, *pnn;
	int nErr = 0;

	pthread_mutex_lock(&apps_std_mt);
	QLIST_NEXTSAFE_FOR_ALL(&apps_std_qlst, pn, pnn)
	{
		sinfo = STD_RECOVER_REC(struct apps_std_info, qn, pn);
		if (sinfo->fd == fd)
		{
			sfree = sinfo;
			QNode_Dequeue(&sinfo->qn);
			break;
		}
	}
	pthread_mutex_unlock(&apps_std_mt);
	VERIFY(sfree);
	FREE(sfree);

bail:
	return nErr;
}

static int apps_std_FILE_alloc(FILE *stream, apps_std_FILE *fd)
{
	struct apps_std_info *sinfo = 0, *info;
	QNode *pn = 0;
	apps_std_FILE prevfd = APPS_FD_BASE - 1;
	int nErr = 0;

	VERIFY(0 != (sinfo = MALLOC(sizeof(*sinfo))));
	QNode_CtorZ(&sinfo->qn);
	pthread_mutex_lock(&apps_std_mt);
	pn = QList_GetFirst(&apps_std_qlst);
	if (pn)
	{
		info = STD_RECOVER_REC(struct apps_std_info, qn, pn);
		prevfd = info->fd;
		QLIST_FOR_REST(&apps_std_qlst, pn)
		{
			info = STD_RECOVER_REC(struct apps_std_info, qn, pn);
			if (info->fd != prevfd + 1)
			{
				sinfo->fd = prevfd + 1;
				QNode_InsPrev(pn, &sinfo->qn);
				break;
			}
			prevfd = info->fd;
		}
	}
	if (!QNode_IsQueuedZ(&sinfo->qn))
	{
		sinfo->fd = prevfd + 1;
		QList_AppendNode(&apps_std_qlst, &sinfo->qn);
	}
	pthread_mutex_unlock(&apps_std_mt);

	sinfo->stream = stream;
	*fd = sinfo->fd;

bail:
	if (nErr)
	{
		FREEIF(sinfo);
	}
	return nErr;
}

static FILE *apps_std_FILE_get(apps_std_FILE fd)
{
	struct apps_std_info *sinfo = 0;
	QNode *pn, *pnn;
	FILE *stream = 0;

	pthread_mutex_lock(&apps_std_mt);
	QLIST_NEXTSAFE_FOR_ALL(&apps_std_qlst, pn, pnn)
	{
		sinfo = STD_RECOVER_REC(struct apps_std_info, qn, pn);
		if (sinfo->fd == fd)
		{
			stream = sinfo->stream;
			break;
		}
	}
	pthread_mutex_unlock(&apps_std_mt);

	return stream;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fopen)(const char *name, const char *mode, apps_std_FILE *psout) __QAIC_IMPL_ATTRIBUTE
{
	FILE *stream = fopen(name, mode);
	if (stream)
	{
		return apps_std_FILE_alloc(stream, psout);
	}
	VERIFY_IPRINTF("fopen did not find: %s %s\n", name, strerror(ERRNO));
	return ERRNO;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_freopen)(apps_std_FILE sin, const char *name, const char *mode, apps_std_FILE *psout) __QAIC_IMPL_ATTRIBUTE
{
	FILE *stream = freopen(name, mode, apps_std_FILE_get(sin));
	if (stream)
	{
		return apps_std_FILE_alloc(stream, psout);
	}
	return ERRNO;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fflush)(apps_std_FILE sin) __QAIC_IMPL_ATTRIBUTE
{
	if (0 == fflush(apps_std_FILE_get(sin)))
	{
		return 0;
	}
	return ERRNO;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fclose)(apps_std_FILE sin) __QAIC_IMPL_ATTRIBUTE
{
	if (0 == fclose(apps_std_FILE_get(sin)))
	{
		apps_std_FILE_free(sin);
		return 0;
	}
	return ERRNO;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fread)(apps_std_FILE sin, byte *buf, int bufLen, int *bytesRead, int *bEOF) __QAIC_IMPL_ATTRIBUTE
{
	int out = fread(buf, 1, bufLen, apps_std_FILE_get(sin));
	*bEOF = FALSE;
	if (out <= bufLen)
	{
		int err;
		if (0 == out && (0 != (err = ferror(apps_std_FILE_get(sin)))))
		{
			return err;
		}
		*bEOF = feof(apps_std_FILE_get(sin));
	}
	*bytesRead = out;
	return 0;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fwrite)(apps_std_FILE sin, const byte *buf, int bufLen, int *bytesRead, int *bEOF) __QAIC_IMPL_ATTRIBUTE
{
	int out = fwrite(buf, 1, bufLen, apps_std_FILE_get(sin));
	*bEOF = FALSE;
	if (out <= bufLen)
	{
		int err;
		if (0 == out && (0 != (err = ferror(apps_std_FILE_get(sin)))))
		{
			return err;
		}
		*bEOF = feof(apps_std_FILE_get(sin));
	}
	*bytesRead = out;
	return 0;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fgetpos)(apps_std_FILE sin, byte *pos, int posLen, int *posLenReq) __QAIC_IMPL_ATTRIBUTE
{
	fpos_t fpos;
	if (0 == fgetpos(apps_std_FILE_get(sin), &fpos))
	{
		std_memmove(pos, &fpos, STD_MIN((int)sizeof(fpos), posLen));
		*posLenReq = sizeof(fpos);
		return 0;
	}
	return ERRNO;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fsetpos)(apps_std_FILE sin, const byte *pos, int posLen) __QAIC_IMPL_ATTRIBUTE
{
	fpos_t fpos;
	if (sizeof(fpos) != posLen)
	{
		return -1;
	}
	std_memmove(&fpos, pos, sizeof(fpos));
	if (0 == fsetpos(apps_std_FILE_get(sin), &fpos))
	{
		return 0;
	}
	return ERRNO;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_ftell)(apps_std_FILE sin, int *pos) __QAIC_IMPL_ATTRIBUTE
{
	if ((*pos = ftell(apps_std_FILE_get(sin))) >= 0)
	{
		return 0;
	}
	return ERRNO;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fseek)(apps_std_FILE sin, int offset, apps_std_SEEK whence) __QAIC_IMPL_ATTRIBUTE
{
	C_ASSERT(APPS_STD_SEEK_SET == SEEK_SET);
	C_ASSERT(APPS_STD_SEEK_CUR == SEEK_CUR);
	C_ASSERT(APPS_STD_SEEK_END == SEEK_END);
	if (0 == fseek(apps_std_FILE_get(sin), offset, whence))
	{
		return 0;
	}
	return ERRNO;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_rewind)(apps_std_FILE sin) __QAIC_IMPL_ATTRIBUTE
{
	rewind(apps_std_FILE_get(sin));
	return 0;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_feof)(apps_std_FILE sin, int *bEOF) __QAIC_IMPL_ATTRIBUTE
{
	*bEOF = feof(apps_std_FILE_get(sin));
	return 0;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_ferror)(apps_std_FILE sin, int *err) __QAIC_IMPL_ATTRIBUTE
{
	*err = ferror(apps_std_FILE_get(sin));
	return 0;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_clearerr)(apps_std_FILE sin) __QAIC_IMPL_ATTRIBUTE
{
	clearerr(apps_std_FILE_get(sin));
	return 0;
}
__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_flen)(apps_std_FILE sin, uint64 *len) __QAIC_IMPL_ATTRIBUTE
{
	struct stat st_buf;
	int fd = fileno(apps_std_FILE_get(sin));
	C_ASSERT(sizeof(st_buf.st_size) <= sizeof(*len));
	if (fd == -1)
	{
		return ERRNO;
	}
	if (0 != fstat(fd, &st_buf))
	{
		return ERRNO;
	}
	*len = st_buf.st_size;
	return 0;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_print_string)(const char *str) __QAIC_IMPL_ATTRIBUTE
{
	printf("%s", str);
	return 0;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_getenv)(const char *name, char *val, int valLen, int *valLenReq) __QAIC_IMPL_ATTRIBUTE
{
	char *vv = getenv(name);
	if (vv)
	{
		*valLenReq = std_strlen(vv) + 1;
		std_strlcpy(val, vv, valLen);
		return 0;
	}
	VERIFY_IPRINTF("apps_std getenv failed: %s %s\n", name, strerror(ERRNO));
	return ERRNO;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_setenv)(const char *name, const char *val, int override) __QAIC_IMPL_ATTRIBUTE
{
#ifdef _WIN32
	return -1;
#else
	if (0 != setenv(name, val, override))
	{
		return ERRNO;
	}
	return 0;
#endif
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_unsetenv)(const char *name) __QAIC_IMPL_ATTRIBUTE
{
#ifdef _WIN32
	return -1;
#else
	if (0 != unsetenv(name))
	{
		return ERRNO;
	}
	return 0;
#endif
}

#if (defined _WIN32)
static char *ADSP_LIBRARY_PATH = ";c:\\Program Files\\Qualcomm\\RFSA\\aDSP";
#elif (defined __QNX__)
static char *ADSP_LIBRARY_PATH = "/radio/lib/firmware";
#else
static char *ADSP_LIBRARY_PATH = ";/usr/local/lib";
#endif

#define     EMTPY_STR      ""
#define     ENV_LEN_GUESS  256

static int get_dirlist_from_env(const char *envvarname, char **ppDirList)
{
	char     *envList    =  NULL;
	char     *envListBuf =  NULL;
	char     *dirList    =  NULL;
	char     *dirListBuf =  NULL;
	char     *srcStr     =  NULL;
	int      nErr        =  0;
	int      envListLen  =  0;
	int      listLen     =  0;
	int      envLenGuess = STD_MAX(ENV_LEN_GUESS, 1 + std_strlen(ADSP_LIBRARY_PATH));

	VERIFY(NULL != ppDirList);

	VERIFY(envListBuf = (char *)MALLOC(sizeof(char) * envLenGuess));
	envList = envListBuf;
	*envList = '\0';
	if (0 == apps_std_getenv(envvarname, envList, envLenGuess, &envListLen))
	{
		if (envLenGuess < envListLen)
		{
			FREEIF(envListBuf);
			VERIFY(envListBuf = REALLOC(envListBuf, sizeof(char) * envListLen));
			envList = envListBuf;
			VERIFY(0 == apps_std_getenv(envvarname, envList, envListLen, &listLen));
		}
	}
	else if (std_strcmp(envvarname, "ADSP_LIBRARY_PATH") == 0)
	{
		envListLen = listLen = 1 + std_strlcpy(envListBuf, ADSP_LIBRARY_PATH, envLenGuess);
	}

	/* Allocate mem. to copy envvarname.
	If envvarname not set, allocate mem to create an empty string.*/
	if ('\0' != *envList)
	{
		srcStr = envList;
	}
	else
	{
		srcStr = EMTPY_STR;
		envListLen = std_strlen(EMTPY_STR) + 1;
	}
	VERIFY(dirListBuf = (char *)MALLOC(sizeof(char) * envListLen));
	dirList = dirListBuf;
	std_strlcpy(dirList, srcStr, envListLen);
	*ppDirList = dirListBuf;

bail:
	FREEIF(envListBuf);
	return nErr;
}

__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fopen_with_env)(const char *envvarname,
        const char *delim, const char *name, const char *mode,
        apps_std_FILE *psout) __QAIC_IMPL_ATTRIBUTE
{

	int      nErr        =  0;
	char     *dirName    =  NULL;
	char     *pos        =  NULL;
	char     *dirListBuf =  NULL;
	char     *dirList    =  NULL;
	char     *absName    =  NULL;
	uint16   absNameLen  =  0;

	VERIFY(NULL != mode);
	VERIFY(NULL != delim);
	VERIFY(NULL != name);

	VERIFY(0 == get_dirlist_from_env(envvarname, &dirListBuf));
	VERIFY(NULL != (dirList = dirListBuf));

	while (dirList)
	{
		pos = strstr(dirList, delim);
		dirName = dirList;
		if (pos)
		{
			*pos = '\0';
			dirList = pos + std_strlen(delim);
		}
		else
		{
			dirList = 0;
		}

		absNameLen = std_strlen(dirName) + std_strlen(name) + 2;
		VERIFY(absName = (char *)MALLOC(sizeof(char) * absNameLen));
		if ('\0' != *dirName)
		{
			std_strlcpy(absName, dirName, absNameLen);
			std_strlcat(absName, "/", absNameLen);
			std_strlcat(absName, name, absNameLen);
		}
		else
		{
			std_strlcpy(absName, name, absNameLen);
		}

		nErr = apps_std_fopen(absName, mode, psout);
		FREEIF(absName);
		if (0 == nErr)
		{
			goto bail;
		}
	}

bail:
	FREEIF(absName);
	FREEIF(dirListBuf);

	return nErr;
}

__QAIC_HEADER_EXPORT int __QAIC_IMPL(apps_std_get_search_paths_with_env)(
    const char *envvarname, const char *delim, _cstring1_t *paths,
    int pathsLen, uint32 *numPaths, uint16 *maxPathLen) __QAIC_IMPL_ATTRIBUTE
{

	char *path        = NULL;
	int  nErr         = 0;
	char *dirListBuf  = NULL;
	int  i            = 0;
	char *saveptr     = NULL;
	struct stat st;

	VERIFY(NULL != numPaths);
	VERIFY(NULL != delim);
	VERIFY(NULL != maxPathLen);

	VERIFY(0 == get_dirlist_from_env(envvarname, &dirListBuf));

	*numPaths = 0;
	*maxPathLen = 0;

	/* Get the number of folders */
	path = strtok_r(dirListBuf, delim, &saveptr);
	while (path != NULL)
	{
		/* If the path exists, add it to the return */
		if ((stat(path, &st) == 0) && (S_ISDIR(st.st_mode)))
		{
			*maxPathLen = STD_MAX(*maxPathLen, std_strlen(path) + 1);
			if (paths && paths[i].data && paths[i].dataLen >= std_strlen(path))
			{
				std_strlcpy(paths[i].data, path, paths[i].dataLen);
			}
			i++;
		}
		path = strtok_r(NULL, delim, &saveptr);
	}
	*numPaths = i;

bail:
	FREEIF(dirListBuf);
	return nErr;
}


__QAIC_IMPL_EXPORT int __QAIC_IMPL(apps_std_fgets)(apps_std_FILE sin, byte *buf, int bufLen, int *bEOF) __QAIC_IMPL_ATTRIBUTE
{
	char *out = fgets((char *)buf, bufLen, apps_std_FILE_get(sin));
	*bEOF = FALSE;
	if (!out)
	{
		int err;
		if (0 != (err = ferror(apps_std_FILE_get(sin))))
		{
			return err;
		}
		*bEOF = feof(apps_std_FILE_get(sin));
	}
	return 0;
}

__QAIC_HEADER_EXPORT int __QAIC_HEADER(apps_std_fileExists)(const char *path, boolean *exists) __QAIC_HEADER_ATTRIBUTE
{
	int nErr = 0;
	struct stat   buffer;

	VERIFY(path != NULL);
	VERIFY(exists != NULL);

	*exists = (stat(path, &buffer) == 0);

bail:
	return nErr;
}

