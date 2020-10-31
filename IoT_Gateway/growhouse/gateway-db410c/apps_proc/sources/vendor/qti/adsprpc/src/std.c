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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <libgen.h>
#include "AEEstd.h"
#include "version.h"


int  std_getversion(char *pcDst, int nDestSize)
{
	return std_strlcpy(pcDst, VERSION_STRING, nDestSize);
}


char std_tolower(char c)
{
	return tolower(c);
}

char std_toupper(char c)
{
	return toupper(c);
}


static __inline int x_casecmp(unsigned char c1, unsigned char c2)
{
	int diff = c1 - c2;
	if (c1 >= 'A' && c1 <= 'Z')
	{
		diff += 32;
	}
	if (c2 >= 'A' && c2 <= 'Z')
	{
		diff -= 32;
	}
	return diff;
}


int std_strlen(const char *sStart)
{
	return strlen(sStart);
}

int std_strncmp(const char *s1, const char *s2, int n)
{
	return strncmp(s1, s2, n);
}

int std_strcmp(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}

int std_strnicmp(const char *s1, const char *s2, int n)
{
	return strncasecmp(s1, s2, n);
}

int std_stricmp(const char *s1, const char *s2)
{
	return std_strnicmp(s1, s2, MAX_INT32);
}

int std_strlcpy(char *pcDst, const char *cpszSrc, int nDestSize)
{
	int nLen = std_strlen(cpszSrc);

	if (0 < nDestSize)
	{
		int n;

		n = STD_MIN(nLen, nDestSize - 1);
		(void)std_memmove(pcDst, cpszSrc, n);

		pcDst[n] = 0;
	}

	return nLen;
}

int std_strlcat(char *pcDst, const char *cpszSrc, int nDestSize)
{
	int nLen = 0;

	while ((nLen < nDestSize) && (0 != pcDst[nLen]))
	{
		++nLen;
	}

	return nLen + std_strlcpy(pcDst + nLen, cpszSrc, nDestSize - nLen);
}

char *std_strstr(const char *cpszHaystack, const char *cpszNeedle)
{
	return strstr(cpszHaystack, cpszNeedle);
}


int std_memcmp(const void *p1, const void *p2, int length)
{
	return memcmp(p1, p2, length);
}

int std_wstrlen(const AECHAR *s)
{
	return wcslen((const wchar_t *)s);
}


int std_wstrlcpy(AECHAR *pwcDst, const AECHAR *cpwszSrc, int nDestSize)
{
	int nLen = std_wstrlen(cpwszSrc);

	if (0 < nDestSize)
	{
		int n;

		n = STD_MIN(nLen, nDestSize - 1);
		/* call memmove, in case n is larger than 1G */
		(void)memmove(pwcDst, cpwszSrc, ((size_t)n)*sizeof(AECHAR));

		pwcDst[n] = 0;
	}

	return nLen;
}

int std_wstrlcat(AECHAR *pwcDst, const AECHAR *cpwszSrc, int nDestSize)
{
	int nLen = 0;

	while ((nLen < nDestSize) && (0 != pwcDst[nLen]))
	{
		++nLen;
	}

	return nLen + std_wstrlcpy(pwcDst + nLen, cpwszSrc, nDestSize - nLen);
}

char *std_strchrend(const char *cpsz, char c)
{
	while (*cpsz && *cpsz != c)
	{
		++cpsz;
	}
	return (char *)cpsz;
}

char *std_strchr(const char *cpszSrch, int c)
{
	return strchr(cpszSrch, c);
}

void *std_memstr(const char *cpHaystack, const char *cpszNeedle,
                 int nHaystackLen)
{
	int nLen = 0;

	/* Handle empty needle string as a special case */
	if ('\0' == *cpszNeedle)
	{
		return (char *)cpHaystack;
	}

	/* Find the first character of the needle string in the haystack string */
	while (nLen < nHaystackLen)
	{
		if (cpHaystack[nLen] == *cpszNeedle)
		{
			/* check if the rest of the string matches */
			const char *cpNeedle = cpszNeedle;
			int nRetIndex = nLen;
			do
			{
				if ('\0' == *++cpNeedle)
				{
					/* Found a match */
					return (void *)(cpHaystack + nRetIndex);
				}
				nLen++;
			}
			while (cpHaystack[nLen] == *cpNeedle);
		}
		else
		{
			nLen++;
		}
	}

	return 0;
}

void *std_memchrend(const void *p, int c, int nLen)
{
	const char *cpc = (const char *)p + nLen;
	int i = -nLen;

	if (nLen > 0)
	{
		do
		{
			if (cpc[i] == c)
			{
				break;
			}
		}
		while (++i);
	}
	return (void *)(cpc + i);
}

void *std_memchr(const void *s, int c, int n)
{
	return memchr(s, c, n);
}

void *std_memrchr(const void *p, int c, int nLen)
{
	const char *cpc = (const char *)p - 1;

	if (nLen > 0)
	{
		do
		{
			if (cpc[nLen] == c)
			{
				return (void *)(cpc + nLen);
			}
		}
		while (--nLen);
	}

	return 0;
}


char *std_strrchr(const char *cpsz, int c)
{
	return std_memrchr(cpsz, c, std_strlen(cpsz) + 1);
}


void *std_memrchrbegin(const void *p, int c, int n)
{
	void *pOut = std_memrchr(p, c, n);

	return (pOut ? pOut : (void *)p);
}


/* WARNING:  nLen must be >0 */

static char *x_scanbytes(const char *pcBuf, const char *cpszChars,
                         int nLen, unsigned char cStop, boolean bTestEqual)
{
	int n;
	unsigned a[8];

	/* Initialize bit mask based on the input flag that specifies whether
	 we are looking for a character that matches "any" or "none"
	 of the characters in the search string */

#define ENTRY(c)   a[((c)&7)]   /* c's bit lives here */
#define SHIFT(c)   ((c)>>3)     /* c's bit is shifted by this much */

	if (bTestEqual)
	{
		std_memset(a, 0, STD_SIZEOF(a));
		do
		{
			ENTRY(cStop) |= (0x80000000U >> SHIFT(cStop));
			cStop = (unsigned char) * cpszChars++;
		}
		while (cStop);
	}
	else
	{
		std_memset(a, 0xFF, STD_SIZEOF(a));

		while (0 != (cStop = (unsigned char) * cpszChars++))
		{
			ENTRY(cStop) ^= (0x80000000U >> SHIFT(cStop));
		}
	}

	/* Search buffer */

	pcBuf += nLen;
	n = -nLen;
	do
	{
		unsigned char uc = (unsigned char)pcBuf[n];
		/* testing for negative after shift is quicker than comparison */
		if ((int)(ENTRY(uc) << SHIFT(uc)) < 0)
		{
			break;
		}
	}
	while (++n);

	return (char *)(pcBuf + n);
}


void *std_memchrsend(const void *pBuf, const char *cpszChars, int nLen)
{
	if (nLen <= 0)
	{
		return (void *)pBuf;
	}
	if ('\0' == *cpszChars)
	{
		return (char *)pBuf + nLen;
	}

	return x_scanbytes((const char *)pBuf, cpszChars + 1, nLen,
	                   (unsigned char) * cpszChars, TRUE);
}


char *std_strchrsend(const char *cpszSrch, const char *cpszChars)
{
	return x_scanbytes(cpszSrch, cpszChars, MAX_INT32, '\0', TRUE);
}


char *std_strchrs(const char *cpszSrch, const char *cpszChars)
{
	const char *pc = std_strchrsend(cpszSrch, cpszChars);

	return (*pc ? (char *)pc : 0);
}


char *std_striends(const char *cpsz, const char *cpszSuffix)
{
	int nOffset = std_strlen(cpsz) - std_strlen(cpszSuffix);

	if ((0 <= nOffset) &&
	        (0 == std_stricmp(cpsz + nOffset, cpszSuffix)))
	{

		return (char *)(cpsz + nOffset);
	}

	return 0;
}


char *std_strends(const char *cpsz, const char *cpszSuffix)
{
	int nOffset = std_strlen(cpsz) - std_strlen(cpszSuffix);

	if ((0 <= nOffset) &&
	        (0 == std_strcmp(cpsz + nOffset, cpszSuffix)))
	{

		return (char *)(cpsz + nOffset);
	}

	return 0;
}

char *std_strbegins(const char *cpsz, const char *cpszPrefix)
{
	for (;;)
	{
		if ('\0' == *cpszPrefix)
		{
			return (char *)cpsz;
		}

		if (*cpszPrefix != *cpsz)
		{
			return 0;
		}

		++cpszPrefix;
		++cpsz;
	}
	/* not reached */
}

char *std_stribegins(const char *cpsz, const char *cpszPrefix)
{
	for (;;)
	{
		if ('\0' == *cpszPrefix)
		{
			return (char *)cpsz;
		}

		if (x_casecmp((unsigned char)*cpszPrefix, (unsigned char)*cpsz))
		{
			return 0;
		}

		++cpszPrefix;
		++cpsz;
	}
	/* not reached */
}

int std_strcspn(const char *cpszSrch, const char *cpszChars)
{
	const char *pc = x_scanbytes(cpszSrch, cpszChars, MAX_INT32, '\0', TRUE);

	return (pc - cpszSrch);
}

int std_strspn(const char *cpszSrch, const char *cpszChars)
{
	const char *pc = x_scanbytes(cpszSrch, cpszChars, MAX_INT32, '\0', FALSE);

	return (pc - cpszSrch);
}

int std_wstrncmp(const AECHAR *s1, const AECHAR *s2, int nLen)
{
	return wcsncmp((const wchar_t *)s1, (const wchar_t *)s2, nLen);
}

int std_wstrcmp(const AECHAR *s1, const AECHAR *s2)
{
	return std_wstrncmp(s1, s2, MAX_INT32);
}

AECHAR *std_wstrchr(const AECHAR *cpwszText, AECHAR ch)
{
	return (AECHAR *)(wcschr((const wchar_t *)cpwszText, ch));
}

AECHAR *std_wstrrchr(const AECHAR *cpwszText, AECHAR ch)
{
	const AECHAR *p = 0;

	do
	{
		if (*cpwszText == ch)
		{
			p = cpwszText;
		}
	}
	while (*cpwszText++ != (AECHAR)0);

	return (AECHAR *)p;
}

