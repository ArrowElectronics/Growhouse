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
#ifndef MOD_TABLE_IMP_H
#define MOD_TABLE_IMP_H
#include "remote.h"
#include "uthash.h"
#include "verify.h"
#include "AEEstd.h"
#include "qtest_stdlib.h"

/**
 * structure for the mod table
 *
 * you need to define a rw_mutex type and its read/write lock/unlock api's
 * which are under the RW_MUTEX namespace.
 *
 * this library defines 2 functions for opening modules, open_static and
 * open_dynamic.  Both return a handle that should be closed via close.
 *
 * you can also register a const handle, an invoke function for a known handle
 * value.  since handle keys are allocated, you should pick handle values that are
 * not going to be returned by malloc (0, or odd).
 */
struct static_mod_table
{
	RW_MUTEX_T mut;
	struct static_mod *staticMods;
	struct const_mod *constMods;
	boolean bInit;
};

struct open_mod_table
{
	RW_MUTEX_T mut;
	struct open_mod *openMods;
	struct static_mod_table *smt;
};


typedef int (*invoke_fn)(uint32, remote_arg *);
struct static_mod
{
	invoke_fn         invoke_func_ptr;
	UT_hash_handle    hh;
	char              name[1];
};

struct const_mod
{
	invoke_fn         invoke_func_ptr;
	uint32            key;
	UT_hash_handle    hh;
	char              name[1];
};


struct open_mod
{
	void             *dlhandle;
	invoke_fn         invoke_func_ptr;
	uint32            key;
	UT_hash_handle    hh;
	char              name[1];
};

static int static_mod_table_ctor_imp(struct static_mod_table *me)
{
	if (me->bInit == 0)
	{
		RW_MUTEX_CTOR(me->mut);
		me->staticMods = 0;
		me->bInit = 1;
	}
	return 0;
}

static void static_mod_table_dtor_imp(struct static_mod_table *me)
{
	struct static_mod *sm, *stmp;
	struct const_mod *dm, *ftmp;
	if (me->bInit != 0)
	{
		if (me->staticMods || me->constMods)
		{
			RW_MUTEX_LOCK_WRITE(me->mut);
			HASH_ITER(hh, me->staticMods, sm, stmp)
			{
				HASH_DEL(me->staticMods, sm);
				FREE(sm);
			}
			HASH_ITER(hh, me->constMods, dm, ftmp)
			{
				HASH_DEL(me->constMods, dm);
				FREE(dm);
			}
			RW_MUTEX_UNLOCK_WRITE(me->mut);
		}
		RW_MUTEX_DTOR(me->mut);
		me->staticMods = 0;
		me->bInit = 0;
	}
}

static int open_mod_table_ctor_imp(void *ctx, void *data)
{
	struct open_mod_table *me = (struct open_mod_table *)data;
	RW_MUTEX_CTOR(me->mut);
	me->openMods = 0;
	me->smt = (struct static_mod_table *) ctx;
	return 0;
}

static void open_mod_table_dtor_imp(void *data)
{
	struct open_mod_table *me = (struct open_mod_table *)data;
	struct open_mod *dm, *ftmp;
	if (me->openMods)
	{
		RW_MUTEX_LOCK_WRITE(me->mut);
		HASH_ITER(hh, me->openMods, dm, ftmp)
		{
			HASH_DEL(me->openMods, dm);
			if (dm->dlhandle)
			{
				dlw_Close(dm->dlhandle);
			}
			FREE(dm);
		}
		RW_MUTEX_UNLOCK_WRITE(me->mut);
	}
	RW_MUTEX_DTOR(me->mut);
	me->openMods = 0;
}

static int open_mod_table_open_static(struct open_mod_table *me, const char *in_name, remote_handle *handle)
{
	int nErr = 0;
	struct static_mod *sm = 0;
	struct open_mod *dm = 0, *dmOld = 0;
	int len = std_strlen(in_name);
	RW_MUTEX_LOCK_READ(me->mut);
	HASH_FIND_STR(me->smt->staticMods, in_name, sm);
	RW_MUTEX_UNLOCK_READ(me->mut);
	VERIFY(0 != sm);
	VERIFY(0 != (dm = ((struct open_mod *)CALLOC(1, sizeof(struct open_mod) + len + 1))));
	std_strlcpy(dm->name, sm->name, len + 1);
	dm->invoke_func_ptr = sm->invoke_func_ptr;
	dm->key = (uint32)(uintptr_t)dm;

	RW_MUTEX_LOCK_WRITE(me->mut);
	do
	{
		HASH_FIND_INT(me->openMods, &dm->key, dmOld);
		if (dmOld)
		{
			dm->key++;
		}
	}
	while (dmOld);
	HASH_ADD_INT(me->openMods, key, dm);
	RW_MUTEX_UNLOCK_WRITE(me->mut);

	*handle = dm->key;
bail:
	return nErr;
}


static int static_mod_table_register_static_imp(struct static_mod_table *me, const char *in_name, int(*pfn)(uint32 sc, remote_arg *pra))
{
	int nErr = 0;
	struct static_mod *sm = 0;
	int len = std_strlen(in_name) + 1;
	VERIFY(0 != (sm = ((struct static_mod *)CALLOC(1, sizeof(struct static_mod) + len))));
	std_strlcpy(sm->name, in_name, len);
	sm->invoke_func_ptr = pfn;
	RW_MUTEX_LOCK_WRITE(me->mut);
	HASH_ADD_STR(me->staticMods, name, sm);
	RW_MUTEX_UNLOCK_WRITE(me->mut);
bail:
	if (nErr)
	{
		FREEIF(sm);
	}
	return nErr;
}

static int static_mod_table_register_const_handle_imp(struct static_mod_table *me, remote_handle handle, const char *in_name, int(*pfn)(uint32 sc, remote_arg *pra))
{
	int nErr = 0;
	int len = std_strlen(in_name) + 1;
	struct const_mod *dm = 0, *dmOld;
	VERIFY(0 != (dm = ((struct const_mod *)CALLOC(1, sizeof(struct open_mod) + len))));
	dm->key = (uint32)handle;
	dm->invoke_func_ptr = pfn;
	std_strlcpy(dm->name, in_name, len);

	RW_MUTEX_LOCK_WRITE(me->mut);
	HASH_FIND_INT(me->constMods, &handle, dmOld);
	if (dmOld == 0)
	{
		HASH_ADD_INT(me->constMods, key, dm);
	}
	RW_MUTEX_UNLOCK_WRITE(me->mut);
	nErr = dmOld != 0 ? -1 : nErr;

bail:
	if (nErr)
	{
		FREEIF(dm);
	}
	return nErr;
}

#define  FILE_PREFIX       "file://"
#define  FILE_PREFIX_LEN   7
static int open_mod_table_open_dynamic(struct open_mod_table *me, const char *in_name, remote_handle *handle, char *dlStr, int dlerrorLen, int *pdlErr)
{
	int nErr = 0, dlErr = 0;
	struct open_mod *dm = 0, *dmOld;
	int i, len, snlen;
	int name_len = std_strlen(in_name);
	char  *pSoName =  0;
	char  *pTestName =  0;
	char  *pQchar =  0;

	len = name_len + STD_MAX(sizeof("_skel_invoke"), sizeof("lib_skel.so"));
	VERIFY(0 != (dm = ((struct open_mod *)CALLOC(1, sizeof(struct open_mod) + len))));

	pSoName = std_strstr(in_name, FILE_PREFIX);
	if (pSoName == in_name)
	{
		pSoName += FILE_PREFIX_LEN;
		std_strlcpy(dm->name, pSoName, name_len);
		pQchar = std_strchr(dm->name, '?');
		VERIFY(NULL != pQchar);
		*pQchar = '\0';
		pTestName = pQchar + 1;
		VERIFY(NULL != pTestName);
		snlen = std_strlen(dm->name);
	}
	else
	{
		snlen = std_snprintf(dm->name, len, "lib%s_skel.so", in_name);
	}

	VERIFY(len > snlen);

	dm->dlhandle = dlw_Open(dm->name, DLW_RTLD_NOW);
	if (0 != (nErr = (dlErr = dm->dlhandle == 0 ? -5 : 0)))
	{
		goto bail;
	}

	if (pTestName)
	{
		snlen = std_snprintf(dm->name, len, "%s_skel_invoke", pTestName);
	}
	else
	{
		snlen = std_snprintf(dm->name, len, "%s_skel_invoke", in_name);
	}
	VERIFY(len > snlen);

	dm->invoke_func_ptr = (invoke_fn) dlw_Sym(dm->dlhandle, dm->name);
	VERIFY(0 == (dlErr = dm->invoke_func_ptr == 0 ? -1 : 0));

	dm->key = (uint32)(uintptr_t)dm;
	RW_MUTEX_LOCK_WRITE(me->mut);
	do
	{
		HASH_FIND_INT(me->openMods, &dm->key, dmOld);
		if (dmOld)
		{
			dm->key++;
		}
	}
	while (dmOld);
	RW_MUTEX_LOCK_WRITE(me->smt->mut);
	HASH_FIND_INT(me->smt->constMods, &dm->key, dmOld);
	RW_MUTEX_UNLOCK_WRITE(me->smt->mut);
	if (dmOld == 0)
	{
		HASH_ADD_INT(me->openMods, key, dm);
	}
	RW_MUTEX_UNLOCK_WRITE(me->mut);
	nErr = dmOld != 0 ? -1 : nErr;
	if (nErr == 0)
	{
		*handle = dm->key;
		std_strlcpy(dm->name, in_name, name_len);
	}
bail:
	if (nErr)
	{
		if (dlErr)
		{
			const char *dlerr = dlw_Error();
			if (dlerr != 0)
			{
				std_strlcpy(dlStr, dlerr, dlerrorLen);
			}
			nErr = 0;
		}
		if (pdlErr)
		{
			*pdlErr = dlErr;
		}
		if (dm && dm->dlhandle)
		{
			dlw_Close(dm->dlhandle);
		}
		FREEIF(dm);
	}
	return nErr;
}

static int open_mod_table_open_imp(struct open_mod_table *me, const char *in_name, remote_handle *handle, char *dlerr, int dlerrorLen, int *pdlErr)
{
	int nErr = 0, dlErr = 0;
	if (pdlErr)
	{
		*pdlErr = 0;
	}

	VERIFY(0 == open_mod_table_open_dynamic(me, in_name, handle, dlerr, dlerrorLen, &dlErr));
	if (dlErr != 0)
	{
		if (0 != open_mod_table_open_static(me, in_name, handle))
		{
			if (pdlErr)
			{
				*pdlErr = dlErr;
			}
		}
	}
bail:
	return nErr;
}
static int open_mod_table_close_imp(struct open_mod_table *me, remote_handle handle, char *errStr, int errStrLen, int *pdlErr)
{
	int nErr = 0;
	struct open_mod *dm = 0;
	int dlErr = 0;
	// First ensure that the handle is valid
	RW_MUTEX_LOCK_READ(me->mut);
	HASH_FIND_INT(me->openMods, &handle, dm);
	RW_MUTEX_UNLOCK_READ(me->mut);

	VERIFY(dm != NULL);

	if (dm)
	{
		// Try to close the library
		RW_MUTEX_LOCK_WRITE(me->mut);
		HASH_DEL(me->openMods, dm);
		RW_MUTEX_UNLOCK_WRITE(me->mut);
		if (dm->dlhandle)
		{
			dlErr = dlw_Close(dm->dlhandle);
		}
		FREE(dm);
	}

bail:
	if (dlErr)
	{
		const char *error = dlw_Error();
		nErr = dlErr;
		if (error != 0)
		{
			std_strlcpy(errStr, error, errStrLen);
		}
	}
	if (pdlErr)
	{
		*pdlErr = dlErr;
	}
	return nErr;
}

static invoke_fn open_mod_table_handle_to_invoke(struct open_mod_table *me, remote_handle handle)
{
	struct open_mod *om = 0;
	struct const_mod *cm = 0;
	invoke_fn pfn = 0;
	RW_MUTEX_LOCK_READ(me->mut);
	HASH_FIND_INT(me->openMods, &handle, om);
	if (0 != om)
	{
		pfn = om->invoke_func_ptr;
	}
	RW_MUTEX_UNLOCK_READ(me->mut);
	if (pfn == 0)
	{
		RW_MUTEX_LOCK_READ(me->smt->mut);
		HASH_FIND_INT(me->smt->constMods, &handle, cm);
		if (0 != cm)
		{
			pfn = cm->invoke_func_ptr;
		}
		RW_MUTEX_UNLOCK_READ(me->smt->mut);
	}
	return pfn;
}

static int open_mod_table_handle_invoke(struct open_mod_table *me, remote_handle handle, uint32 sc, remote_arg *pra)
{
	int nErr = 0;
	invoke_fn invoke_func_ptr;
	VERIFY(0 != (invoke_func_ptr = open_mod_table_handle_to_invoke(me, handle)));
	VERIFY(0 == (nErr = invoke_func_ptr(sc, pra)));
bail:
	return nErr;
}

struct mod_table
{
	struct static_mod_table smt;
	struct open_mod_table omt;
};

static __inline int mod_table_ctor_imp(struct mod_table *mt)
{
	int nErr = 0;
	VERIFY(0 == static_mod_table_ctor_imp(&mt->smt));
	VERIFY(0 == open_mod_table_ctor_imp(&mt->smt, &mt->omt));
bail:
	return nErr;
}

static __inline void mod_table_dtor_imp(struct mod_table *mt)
{
	open_mod_table_dtor_imp(&mt->omt);
	static_mod_table_dtor_imp(&mt->smt);
}

static __inline int mod_table_open_dynamic(struct mod_table *me, const char *in_name, remote_handle *handle, char *dlStr, int dlerrorLen, int *pdlErr)
{
	return open_mod_table_open_dynamic(&me->omt, in_name, handle, dlStr, dlerrorLen, pdlErr);
}

static __inline int mod_table_close_imp(struct mod_table *me, remote_handle handle, char *errStr, int errStrLen, int *pdlErr)
{
	return open_mod_table_close_imp(&me->omt, handle, errStr, errStrLen, pdlErr);
}

static __inline int mod_table_open_static(struct mod_table *me, const char *in_name, remote_handle *handle)
{
	return open_mod_table_open_static(&me->omt, in_name, handle);
}

static __inline int mod_table_register_static_imp(struct mod_table *me, const char *in_name, int(*pfn)(uint32 sc, remote_arg *pra))
{
	return static_mod_table_register_static_imp(&me->smt, in_name, pfn);
}

static __inline int mod_table_handle_invoke(struct mod_table *me, remote_handle handle, uint32 sc, remote_arg *pra)
{
	return open_mod_table_handle_invoke(&me->omt, handle, sc, pra);
}

static __inline int mod_table_register_const_handle_imp(struct mod_table *me, remote_handle handle, const char *in_name, int(*pfn)(uint32 sc, remote_arg *pra))
{
	return static_mod_table_register_const_handle_imp(&me->smt, handle, in_name, pfn);
}

static __inline int mod_table_open_imp(struct mod_table *me, const char *in_name, remote_handle *handle, char *dlerr, int dlerrorLen, int *pdlErr)
{
	return open_mod_table_open_imp(&me->omt, in_name, handle, dlerr, dlerrorLen, pdlErr);
}
#endif // MOD_TABLE_IMP_H
