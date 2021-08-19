// TMessageBox.cpp: implementation of the CTMessageBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TMessageBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SS_EDITCONTROL		0x00002000L

TCHAR	CTMessageBox::m_strCaption[256] = {0};
TCHAR	CTMessageBox::m_strMessage[256] = {0};
UINT	CTMessageBox::m_uType = 0;
HHOOK	CTMessageBox::m_hWndHook = NULL;
HWND	CTMessageBox::m_hWnd = NULL;
HWND	CTMessageBox::m_hTxtWnd = NULL;
int		CTMessageBox::m_nTime = 0;
UINT	CTMessageBox::m_uDefault = IDNO;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTMessageBox::CTMessageBox()
{

}

CTMessageBox::~CTMessageBox()
{
	// 每次析构时都清空
	CTMessageBox::m_strCaption[0] = 0;
	CTMessageBox::m_strMessage[0] = 0;
	CTMessageBox::m_uType = 0;
	CTMessageBox::m_hWndHook = NULL;
	CTMessageBox::m_hWnd = NULL;
	CTMessageBox::m_hTxtWnd = NULL;
	CTMessageBox::m_nTime = 0;
	CTMessageBox::m_uDefault = IDNO;
}

void CALLBACK CTMessageBox::TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    if (m_nTime <= 1)
    {
		KillTimer(m_hWnd, 1);
		EndDialog(m_hWnd, m_uDefault);
    }
    else
    {
        m_nTime--;
        TCHAR strShow[300] = {0};

		TCHAR szFormat[32];
		LoadString(AfxGetInstanceHandle(), IDS_MSGINFO_TIME, szFormat, _countof(szFormat));

		_stprintf(strShow, szFormat, m_strMessage, m_nTime);
        SetWindowText(m_hTxtWnd, strShow);
    }
}

LRESULT WINAPI CTMessageBox::BoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CTLCOLORSTATIC:
		{
			HDC dc = (HDC)wParam;
			SetTextColor(dc, AfxGetQColor(QCOLOR_TEXT));
			SetBkMode(dc, TRANSPARENT);
			return (LRESULT)AfxGetQBrush();
		}
		break;
		
	case WM_ERASEBKGND:
		{
			HDC dc = GetDC(hWnd);
			RECT rc;
			GetClientRect(hWnd, &rc);
			FillRect(dc, &rc, AfxGetQBrush());
			ReleaseDC(hWnd, dc);
		}
		return TRUE;
		
	case WM_COMMAND:
		if (BN_CLICKED == HIWORD(wParam))
		{
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					int nID = GetDlgCtrlID(GetFocus());
					if (nID)
					{
						EndDialog(hWnd, nID);
					}
					else
					{
						EndDialog(hWnd, LOWORD(wParam));
					}
				}
				return 0;

			case IDCANCEL:
			case IDABORT:
			case IDRETRY:
			case IDIGNORE:
			case IDYES:
			case IDNO:
			case IDTRYAGAIN:
			case IDCONTINUE:
				{
					int nRet = HandleResult(LOWORD(wParam), lParam);
					if (nRet)
					{
						EndDialog(hWnd, nRet);
					}
				}
				return 0;
			}
		}
		break;

	case WM_CLOSE:
		// EndDialog会处理当前的返回值，这里的IDCANCEL不一定是真实的返回值
		EndDialog(hWnd, IDCANCEL);
		return 0;
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT WINAPI CTMessageBox::BtnProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bClicked = FALSE;
	PAINTSTRUCT ps;
	
	switch(uMsg)
	{
	case WM_PAINT:
		{
			HDC dc = BeginPaint(hWnd, &ps);
			CQButton::DrawButton(hWnd, dc, bClicked);
			EndPaint(hWnd, &ps);
			return 0;
		}
		break;
		
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		if (MK_LBUTTON == wParam)
		{
			bClicked = TRUE;
			SetCapture(hWnd);

			SetFocus(hWnd);

			HDC dc = GetDC(hWnd);
			CQButton::DrawButton(hWnd, dc, bClicked);
			ReleaseDC(hWnd, dc);

			return 0;
		}
		break;
		
	case WM_LBUTTONUP:
		if (bClicked)
		{
			bClicked = FALSE;
			ReleaseCapture();

			HDC dc = GetDC(hWnd);
			CQButton::DrawButton(hWnd, dc, bClicked);
			ReleaseDC(hWnd, dc);

			CRect rc;
			GetClientRect(hWnd, rc);
			CPoint pt(LOWORD(lParam), HIWORD(lParam));
			if (rc.PtInRect(pt))
			{
				SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hWnd), BN_CLICKED), (LONG)hWnd);
			}
		}
		break;

	case WM_KILLFOCUS:
	case WM_SETFOCUS:
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_KEYUP:
		switch(wParam)
		{
		case VK_SPACE:
			SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(GetDlgCtrlID(hWnd), BN_CLICKED), (LONG)GetFocus());
			return 0;
		}
		break;
	}
	
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CTMessageBox::CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(m_hWndHook, nCode, wParam, lParam);
	}

	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		UnhookWindowsHookEx(m_hWndHook);
		return 0;
	}

	switch(nCode)
    {
	case HCBT_CREATEWND:
		{
			HWND hWnd = (HWND)wParam;
			long a = ::GetWindowLong(hWnd, GWL_STYLE);
			// 说明是文本显示（这个宏在VC6.0版本中没有定义，自己定义了一个）
			if (a & SS_EDITCONTROL)
			{
				m_hTxtWnd = hWnd;
				if (m_nTime)
				{
					SetTimer(m_hWnd, 1, 1000, (TIMERPROC)TimerProc);
				}
			}
			// 说明是按钮或图表
			else if (a & WS_CHILD)
			{
				switch(LOWORD(a))
				{
				case SS_ICON:
					break;
					
				default:
					SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~BS_DEFPUSHBUTTON);
					SetWindowLong(hWnd, GWL_WNDPROC, (LONG)BtnProc);
					break;
				}
			}
		}
		break;

	case HCBT_ACTIVATE:
		m_hWnd = (HWND)wParam;
		SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)BoxProc);
		UnhookWindowsHookEx(m_hWndHook);
		break;
    }
	
    return 0;
}

int CTMessageBox::SuperMessageBox(HWND hWnd,
								  LPCTSTR lpText,
								  LPCTSTR lpCaption,
								  UINT uType,
								  UINT uDefaultType,
								  UINT dwSecond)
{
	m_nTime = dwSecond;
	_tcsncpy(m_strMessage, lpText, _countof(m_strMessage));
	m_strMessage[MESSAGE_MAXLEN] = 0;
	
	TCHAR strShow[300] = {0};
	LPCTSTR lpShow = NULL;

	if (m_nTime)
	{
		TCHAR szFormat[32];
		LoadString(AfxGetInstanceHandle(), IDS_MSGINFO_TIME, szFormat, _countof(szFormat));
		
		_stprintf(strShow, szFormat, m_strMessage, m_nTime);
		lpShow = strShow;
	}
	else
	{
		lpShow = lpText;
	}
	
	m_uType = uType;
	m_uDefault = uDefaultType;
	
	m_hWndHook = SetWindowsHookEx(WH_CBT, (HOOKPROC)CBTProc, 0, GetCurrentThreadId());
	int iResult = ::MessageBox(hWnd, lpShow, lpCaption, uType);
	
	KillTimer(m_hWnd, 1);
	
	// 结束后清空
	CTMessageBox::m_strCaption[0] = 0;
	CTMessageBox::m_strMessage[0] = 0;
	CTMessageBox::m_uType = 0;
	CTMessageBox::m_hWndHook = NULL;
	CTMessageBox::m_hWnd = NULL;
	CTMessageBox::m_hTxtWnd = NULL;
	CTMessageBox::m_nTime = 0;
	CTMessageBox::m_uDefault = IDNO;
	
	return iResult;
}

int CTMessageBox::SuperMessageBox(HWND hWnd,
								  UINT nFormatID,
								  UINT nCaptionID,
								  UINT uType,
								  UINT uDefaultType,
								  UINT dwSecond, ...)
{
	TCHAR szFormat[256];
	TCHAR szCaption[256];
	TCHAR szBuffer[256];

	LoadString(AfxGetInstanceHandle(), nFormatID, szFormat, _countof(szFormat));
	LoadString(AfxGetInstanceHandle(), nCaptionID, szCaption, _countof(szCaption));
	
    va_list argList;
	// va_start第二个参数要填写...之前的那个参数
	// 在本函数中即为dwSecond
	va_start(argList, dwSecond);
	_vsntprintf(szBuffer, _countof(szBuffer), szFormat, argList);
    va_end (argList);

	return SuperMessageBox(hWnd, szBuffer, szCaption, uType, uDefaultType, dwSecond);
}

int CTMessageBox::SuperMessageBox(HWND hWnd,
								  LPCTSTR lpText,
								  UINT nCaptionID,
								  UINT uType,
								  UINT uDefaultType,
								  UINT dwSecond)
{
	TCHAR szCaption[256];
	LoadString(AfxGetInstanceHandle(), nCaptionID, szCaption, _countof(szCaption));

	return SuperMessageBox(hWnd, lpText, szCaption, uType, uDefaultType, dwSecond);
}

int CTMessageBox::HandleResult(int nResult, LPARAM hCtrl)
{
	if (hCtrl)
	{
		return nResult;
	}
	else
	{
		ASSERT(IDOK != nResult);

		switch(m_uType & 0xF)
		{
		case MB_OKCANCEL:
		case MB_YESNOCANCEL:
		case MB_RETRYCANCEL:
			return nResult;

		case MB_YESNO:
		case MB_ABORTRETRYIGNORE:
			// 这个return 0是为了防止在某些情况下按ESC键导致返回IDCANCEL
			// 而按钮中没有IDCANCEL这个动作
			return 0;
		}
	}

	return IDOK;
}