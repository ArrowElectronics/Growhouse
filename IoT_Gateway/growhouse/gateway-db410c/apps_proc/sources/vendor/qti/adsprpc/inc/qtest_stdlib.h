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
#ifndef QTEST_STDLIB_H
#define QTEST_STDLIB_H

#include <assert.h>

#define FREEIF(pv) \
   do {\
      if(pv) { \
         void* tmp = (void*)pv;\
         pv = 0;\
         FREE(tmp);\
      } \
   } while(0)

#ifndef QASSERT
#define QASSERT(st) assert(st)
#endif

#ifndef QTEST
//default implementation for stdlib
#include <stdlib.h>


#define IF_QTEST(vv) (void)0

#ifndef QASSERT
#define QASSERT(st) (void)0
#endif

#ifndef MALLOC
#define MALLOC malloc
#endif

#ifndef CALLOC
#define CALLOC calloc
#endif

#ifndef FREE
#define FREE free
#endif

#ifndef REALLOC
#define REALLOC realloc
#endif

#define qtest_set_failure_mask(mask) (void)mask
#define qtest_get_failure_mask(mask) (void)mask
#define qtest_set_pass_count(cnt)    (void)cnt
#define qtest_done()                 (void)0
#define qtest_test_failure()         0
#define qtest_atexit(pfn,ctx)        (void)pfn; (void)ctx

#else // QTEST

#include "AEEStdDef.h"

#define IF_QTEST(vv) do {\
   vv \
} while (0)

//causes alloc to fail when mask & 0x1 is true
//each test shifts the mask to the right
void qtest_set_failure_mask(uint32 mask);
void qtest_get_failure_mask(uint32 *mask);

//causes alloc to fail when count == 0
//each test decrements the count
void qtest_set_pass_count(int count);

//returns 0 if succeeds and shifts the mask
//usefull for generating controlled failures in functions
int qtest_test_failure(void);

void qtest_atexit(void (*pfnAtExit)(void *pCtx), void *pvCxt);

void qtest_done(void);

void *qtest_malloc(const char *name, int sz);

void *qtest_calloc(const char *name, int cnt, int sz);

void *qtest_realloc(const char *name, void *ptr, int sz);

void qtest_free(const char *name, void *rv);


#define MALLOC(sz)         qtest_malloc(__FILE_LINE__, sz)
#define CALLOC(cnt, sz)    qtest_calloc(__FILE_LINE__, cnt, sz)
#define REALLOC(ptr, sz)   qtest_realloc(__FILE_LINE__, ptr, sz)
#define FREE(ptr)          qtest_free(__FILE_LINE__, ptr)

#endif //QTEST
#endif //QTEST_STDLIB_H
