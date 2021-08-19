#ifndef _ANSI_C_EX_H_
#define _ANSI_C_EX_H_

wchar_t* __cdecl wcsrchr_e(const wchar_t *string, const wchar_t *end, int c);
wchar_t* __cdecl wcscpyn(wchar_t *strDest, const wchar_t *strSource, int iMaxLength);
wchar_t* __cdecl wcsstri(const wchar_t *string, const wchar_t *search);

char* __cdecl strrchr_e(const char *string, const char *end, int c);
char* __cdecl strstri(const char *string, const char *search);
char* __cdecl strcpyn(char *strDest, const char *strSource, int iMaxLength);

#ifdef _UNICODE

#define _tcsrchr_e		wcsrchr_e
#define _tcsstri		wcsstri
#define _tcscpyn		wcscpyn

#else // #ifndef _UNICODE

#define _tcsrchr_e		strrchr_e
#define _tcsstri		strstri
#define _tcscpyn		strcpyn

#endif // #ifdef _UNICODE

#endif // #ifndef _ANSI_C_EX_H_