// stdafx.cpp : source file that includes just the standard includes
//	QuickQuiz.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _DEBUG
#pragma comment(lib, "zlibd.lib")
#pragma comment(lib, "jpegd.lib")
#pragma comment(lib, "libpngd.lib")
#else
#pragma comment(lib, "zlib.lib")
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "libpng.lib")
#endif

#pragma comment(lib, "Shlwapi.lib")

TCHAR g_szPropName[] = _T("GXQuickQuiz");

int g_nOSVersion = OS_WINNTNEW;

// 剪贴板格式
UINT CF_ITEM = RegisterClipboardFormat (_T("QuickItem"));

// 查找消息
UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

// 字体
HFONT g_hNormalFont = NULL;
HFONT g_hBigFont = NULL;
HFONT g_hBigBoldFont = NULL;
int g_nShowFont = SW_NORMALFONT;

// 重置软件
BOOL g_bReset = FALSE;

// 在确认是默认类型的情况下，可以获取题目类型序列
// 其余时刻，仅用于确认编辑窗口内显示的页面序号
int FUNCCALL AfxTypeToIndex(int nType)
{
	if (TYPE_DEFAULT == nType)
	{
		return 0;
	}
	else if (nType <= TYPE_LASTDEFAULT)
	{
		return nType - TYPE_SCHOISE;
	}
	else
	{
		return ITEMINDEX_CUSTOM;
	}
}

int FUNCCALL AfxGetNextType(int nType)
{
	if (nType < TYPE_LASTDEFAULT)
	{
		nType++;
	}
	else
	{
		nType += TYPE_CUSTOMRIGHT;
	}

	return nType;
}

long FUNCCALL AfxGetStrShowWidth(LPCTSTR str)
{
#ifdef _UNICODE
	int i, nRet = 0;
	for (i = 0; str[i]; i++)
	{
		if (str[i] >= 0x4E00 && str[i] <= 0x9EA5)
		{
			nRet++;
		}
		nRet++;
	}

	return nRet;
#else
	return lstrlen(str);
#endif
}

LPTSTR FUNCCALL AfxGetClipText(HWND hWnd)
{
	if (!IsWindow(hWnd))
	{
		return NULL;
	}
	if (IsClipboardFormatAvailable(CF_TCHAR))
	{
		HGLOBAL hGlobal;
		LPTSTR pGlobal;
		LPTSTR pText = NULL;
		
		::OpenClipboard(hWnd);
		
		if (hGlobal = GetClipboardData(CF_TCHAR))
		{
			pGlobal = (LPTSTR)GlobalLock(hGlobal);
			
			pText = (LPTSTR)malloc(GlobalSize(hGlobal));
			if (pText)
			{
				lstrcpy(pText, pGlobal);
			}
		}
		
		::CloseClipboard();
		return pText;
	}
	return NULL;
}

BOOL FUNCCALL AfxPasteItem(CList *pList, HWND hWndNewOwner)
{
	BOOL b = TRUE;
	pList->ClearAllItems(TRUE);
	
	if (::OpenClipboard(hWndNewOwner))
	{
		if (::IsClipboardFormatAvailable(CF_ITEM))
		{
			CFixedSharedFile sf;
			
			sf.SetHandle(GetClipboardData(CF_ITEM));
			
			// 复制入内容
			b = pList->ClipPaste(sf);
			
			sf.Detach();
		}

		::CloseClipboard();
	}

	return b;
}

void FUNCCALL AfxCalcDialogPos(CRect *pRect, int nPosType)
{
	if (FRAMEPOSTYPE_ALIGN != nPosType)
	{
		return;
	}

	int cx = GetSystemMetrics(SM_CXSCREEN), cy = GetSystemMetrics(SM_CYSCREEN);
	CRect rc;

	rc.top = (cy - pRect->Height()) / 2;
	if (rc.top < 0)
	{
		rc.top = 0;
	}
	
	rc.left = cx - pRect->Width() - FRAME_DEFAULTWIDTH;
	if (rc.left < 0)
	{
		rc.left = 0;
	}
	rc.left /= 2;

	*pRect = rc;
}

void FUNCCALL AfxCalcFrameWndPos(CRect *pRect, int nPosType)
{
	int cx = GetSystemMetrics(SM_CXSCREEN), cy = GetSystemMetrics(SM_CYSCREEN);
	CRect rc;

	if (FRAMEPOSTYPE_ALIGN == nPosType)
	{
		rc.left = pRect->right + 3;
		rc.top = pRect->top;
		rc.right = rc.left + FRAME_DEFAULTWIDTH;
		rc.bottom = pRect->bottom;
	}
	else if (FRAMEPOSTYPE_AVOID == nPosType)
	{
		if (pRect->left >= FRAME_DEFAULTWIDTH + 3)
		{
			rc.right = pRect->left - 3;
			rc.left = pRect->left - FRAME_DEFAULTWIDTH - 3;
		}
		else
		{
			rc.left = pRect->right + 3;
			rc.right = pRect->right + FRAME_DEFAULTWIDTH + 3;
		}

		rc.top = pRect->top + pRect->Height() / 2;
		rc.bottom = pRect->bottom;
	}

	*pRect = rc;
}

BOOL FUNCCALL AfxGetOSVersion()
{
	OSVERSIONINFO osver = {0};
    osver.dwOSVersionInfoSize = sizeof(osver);
    GetVersionEx((OSVERSIONINFO*)&osver);
    
	switch(osver.dwMajorVersion)
	{
	case 1:
	case 2:
	case 3:
		return FALSE;

	case 4:
		if (osver.dwMinorVersion > 0)
		{
			g_nOSVersion = OS_WIN98;
		}
		else if (VER_PLATFORM_WIN32_WINDOWS == osver.dwPlatformId)
		{
			g_nOSVersion = OS_WIN95;
		}
		else
		{
			g_nOSVersion = OS_WINNT4;
		}
		break;

	default:
		g_nOSVersion = OS_WINNTNEW;
		break;
	}

	return TRUE;
}
