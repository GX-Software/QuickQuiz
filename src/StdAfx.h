// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__72E9D769_79BB_4252_9D29_C0E3C7D34DFC__INCLUDED_)
#define AFX_STDAFX_H__72E9D769_79BB_4252_9D29_C0E3C7D34DFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define FUNCCALL			__stdcall

#define IDC_HAND            MAKEINTRESOURCE(32649)

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxadv.h>			// CSharedFiles类所用
#include <..\src\afximpl.h>	// afxData定义
#include <Shlwapi.h>		// 字符串处理API
#include <process.h>		// 获取进程ID
#include <iostream>			// 使用std::string类

#include "define.h"
#include "QError.h"
#include "resource.h"

#include "TextManager.h"
#include "TextSimilarity.h"
#include "FixedSharedFile.h"

#include "Item.h"
#include "List.h"
#include "ItemPtr.h"
#include "TempList.h"
#include "ItemGroup.h"
#include "Paper.h"

#include "TMessageBox.h"
#include "MD5.h"
#include "AnsiCEx.h"

// 色彩
#include "ClrCommon.h"

extern TCHAR g_szPropName[];
extern int g_nOSVersion;

// 剪贴板格式
extern UINT CF_ITEM;
#ifdef _UNICODE
#define CF_TCHAR	CF_UNICODETEXT
#else
#define CF_TCHAR	CF_TEXT
#endif

// 查找消息
extern UINT WM_FINDREPLACE;

extern HFONT g_hNormalFont;
extern HFONT g_hBigFont;
extern HFONT g_hBigBoldFont;

enum ShowFont
{
	SW_SMALLFONT = 0,
	SW_NORMALFONT,
	SW_LARGEFONT,
	SW_GIANTFONT
};
extern int g_nShowFont;

extern BOOL g_bReset;

// 全局函数
int FUNCCALL AfxTypeToIndex(int nType);
int FUNCCALL AfxGetNextType(int nType);

long FUNCCALL AfxGetStrShowWidth(LPCTSTR str);

LPTSTR FUNCCALL AfxGetClipText(HWND hWnd);
BOOL FUNCCALL AfxPasteItem(CList *pList, HWND hWndNewOwner);

#define FRAME_DEFAULTWIDTH		400
#define FRAMEPOSTYPE_ALIGN		0
#define FRAMEPOSTYPE_AVOID		1
void FUNCCALL AfxCalcDialogPos(CRect *pRect, int nPosType);
void FUNCCALL AfxCalcFrameWndPos(CRect *pRect, int nPosType);

BOOL FUNCCALL AfxGetOSVersion();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__72E9D769_79BB_4252_9D29_C0E3C7D34DFC__INCLUDED_)
