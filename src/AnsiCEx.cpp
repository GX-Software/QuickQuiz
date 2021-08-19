#include "StdAfx.h"

wchar_t* __cdecl wcscpyn(wchar_t *strDest, const wchar_t *strSource, int iMaxLength)
{
	if (!strDest || !strSource || !iMaxLength)
	{
		return NULL;
	}
	
	wchar_t *ret = wcsncpy(strDest, strSource, iMaxLength);
	strDest[iMaxLength - 1] = 0;
	
	return ret;
}

#ifdef _UNICODE
wchar_t* __cdecl wcsrchr_e(const wchar_t *string, const wchar_t *end, int c)
{
	register wchar_t *p = NULL;

	if (!string || !string[0])
	{
		return NULL;
	}

	if (end)
	{
		p = (wchar_t*)end;
	}
	else
	{
		p = (wchar_t*)string + wcslen(string) - 1;
	}

	while(p != string)
	{
		if (*p == c)
		{
			return p;
		}
		p--;
	}

	return NULL;
}

wchar_t* __cdecl wcsstri(const wchar_t *string, const wchar_t *search)
{
	wchar_t *p = (wchar_t*)string;
	wchar_t *s1, *s2;
	
	if (!search || !*search)
	{
		return p;
	}
	
	while(*p)
	{
		s1 = p;
		s2 = (wchar_t*)search;
		
		while (*s1 && *s2 && (towlower(*s1) == towlower(*s2)))
		{
			s1++, s2++;
		}
		
		if (!*s2)
		{
			return p;
		}
		
		p++;
	}
	
	return NULL;
}

#else // #ifndef _UNICODE
char* __cdecl strrchr_e(const char *string, const char *end, int c)
{
	register char *p = NULL;

	if (!string || !string[0])
	{
		return NULL;
	}

	if (end)
	{
		p = (char*)end;
	}
	else
	{
		p = (char*)string + strlen(string) - 1;
	}

	while(p != string)
	{
		if (*p == c)
		{
			return p;
		}
		p--;
	}

	return NULL;
}

char* __cdecl strstri(const char *string, const char *search)
{
	char *p = (char*)string;
	char *s1, *s2;
	
	if (!search || !*search)
	{
		return p;
	}
	
	while(*p)
	{
		s1 = p;
		s2 = (char*)search;
		
		while (*s1 && *s2 && (tolower(*s1) == tolower(*s2)))
		{
			s1++, s2++;
		}
		
		if (!*s2)
		{
			return p;
		}
		
		p++;
	}
	
	return NULL;
}

char* __cdecl strcpyn(char *strDest, const char *strSource, int iMaxLength)
{
	if (!strDest || !strSource || !iMaxLength)
	{
		return NULL;
	}

	char *ret = _tcsncpy(strDest, strSource, iMaxLength);
	strDest[iMaxLength - 1] = 0;
	
	return ret;
}

#endif // #ifdef _UNICODE