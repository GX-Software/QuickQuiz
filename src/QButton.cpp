// QButton.cpp : implementation file
//

#include "stdafx.h"
#include "QButton.h"

#include "MemBmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQButton

CQButton::CQButton() :
	m_bDown(FALSE)
{
}

CQButton::~CQButton()
{
}


BEGIN_MESSAGE_MAP(CQButton, CButton)
	//{{AFX_MSG_MAP(CQButton)
	ON_WM_GETDLGCODE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQButton message handlers

UINT CQButton::OnGetDlgCode() 
{
	DWORD nStyle = GetStyle() & 0xFL;
	if (nStyle == BS_RADIOBUTTON ||
		nStyle == BS_AUTORADIOBUTTON)
	{
		return DLGC_RADIOBUTTON;
	}
	else
	{
		return CButton::OnGetDlgCode();
	}
}

void CQButton::OnPaint() 
{
	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		CButton::OnPaint();
	}
	else
	{
		CPaintDC paintDC(this);
		DrawButton(m_hWnd, paintDC.GetSafeHdc(), m_bDown);
	}
}

BOOL CQButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void FUNCCALL CQButton::DrawButton(HWND hWnd, HDC hWndDC, BOOL bDown)
{
	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		return;
	}

	CRect rcClient;
	::GetClientRect(hWnd, rcClient);

	CMemBmp Mem;
	HDC hDC = Mem.CreateBmp(hWndDC, rcClient, TRUE, AfxGetQColor(QCOLOR_FACE));
	
	BOOL bEnable = ::IsWindowEnabled(hWnd);
	COLORREF crOldColor = 0;
	if (bEnable)
	{
		crOldColor = ::SetTextColor(hDC, AfxGetQColor(QCOLOR_TEXT));
	}
	else
	{
		crOldColor = ::SetTextColor(hDC, AfxGetQColor(QCOLOR_TEXTDISABLE));
	}
	int nOldMode = ::SetBkMode(hDC, TRANSPARENT);

	HFONT hFont = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);
	if (!hFont)
	{
		hFont = g_hNormalFont;
	}
	HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
	
	DWORD nStyle = GetWindowLong(hWnd, GWL_STYLE);
	BOOL bPushLike = nStyle & BS_PUSHLIKE;
	nStyle &= 0xFL;
	if ((nStyle == BS_RADIOBUTTON || nStyle == BS_AUTORADIOBUTTON ||
		 nStyle == BS_CHECKBOX || nStyle == BS_AUTOCHECKBOX) && !bPushLike)
	{
		CRect rcButton(rcClient);
		UINT uState = bEnable ? 0 : DFCS_INACTIVE;
		
		if (nStyle == BS_RADIOBUTTON || nStyle == BS_AUTORADIOBUTTON)
		{
			rcButton.bottom = rcButton.top + 12;
			rcButton.right = rcButton.left + 12;
			
			uState |= DFCS_BUTTONRADIO;
			if (::SendMessage(hWnd, BM_GETCHECK, 0, 0))
			{
				uState |= DFCS_CHECKED;
			}
		}
		else
		{
			rcButton.bottom = rcButton.top + 13;
			rcButton.right = rcButton.left + 13;
			
			uState |= DFCS_BUTTONCHECK;
			if (::SendMessage(hWnd, BM_GETCHECK, 0, 0))
			{
				uState |= DFCS_CHECKED;
			}
		}
		DrawFrameControl(hDC, rcButton, DFC_BUTTON, uState);
		
		TCHAR strText[64] = {0};
		::GetWindowText(hWnd, strText, _countof(strText));
		::TextOut(hDC, rcButton.right + 4, rcButton.top, strText, lstrlen(strText));
	}
	else
	{
		// 程序中就认为只有Button了
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, AfxGetQBrush());
		HPEN hOldPen = (HPEN)::SelectObject(hDC, CreatePen(PS_SOLID, 1, AfxGetQColor(QCOLOR_BORDER)));
		
		RoundRect(hDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 4, 4);
		
		CRect rcShadow(rcClient);
		rcShadow.DeflateRect(1, 1, 1, 1);
		if ((bDown && CLRTYPE_DARK == g_nColorType) ||
			(!bDown && CLRTYPE_GREEN == g_nColorType))
		{
			rcShadow.top += rcShadow.Height() / 2;
		}
		else
		{
			rcShadow.bottom -= rcShadow.Height() / 2;
		}
		
		HBRUSH hShadowBrush = CreateSolidBrush(AfxGetQColor(QCOLOR_SHADOW));
		FillRect(hDC, rcShadow, hShadowBrush);
		DeleteObject(hShadowBrush);

		::DeleteObject(::SelectObject(hDC, CreatePen(PS_SOLID, 1, AfxGetQColor(QCOLOR_HIGHLIGHT))));
		::SelectObject(hDC, GetStockObject(NULL_BRUSH));

		if (nStyle == BS_DEFPUSHBUTTON ||
			(bPushLike && ::SendMessage(hWnd, BM_GETCHECK, 0, 0)))
		{
			rcShadow = rcClient;
			rcShadow.DeflateRect(1, 1, 1, 1);

			Rectangle(hDC, rcShadow.left, rcShadow.top, rcShadow.right, rcShadow.bottom);

			rcShadow.DeflateRect(1, 1, 1, 1);
			Rectangle(hDC, rcShadow.left, rcShadow.top, rcShadow.right, rcShadow.bottom);
		}
		if (::GetFocus() == hWnd)
		{
			rcShadow = rcClient;
			rcShadow.DeflateRect(2, 2, 2, 2);

			Rectangle(hDC, rcShadow.left, rcShadow.top, rcShadow.right, rcShadow.bottom);
			DrawFocusRect(hDC, rcShadow);
		}
		
		UINT ulBtnStyle = (UINT)GetWindowLong(hWnd, GWL_STYLE) & 0xff;
		if (ulBtnStyle & BS_ICON)
		{
			HICON hIcon = (HICON)::SendMessage(hWnd, BM_GETIMAGE, IMAGE_ICON, 0L);
			ICONINFO ic;
			::GetIconInfo(hIcon, &ic);
			::DrawIconEx(hDC,
				(rcClient.Width() - ic.xHotspot * 2) / 2,
				(rcClient.Height() - ic.yHotspot * 2) / 2,
				hIcon, ic.xHotspot * 2, ic.yHotspot * 2, 0, NULL, DI_NORMAL);
		}
		else
		{
			TCHAR strText[64] = {0};
			::GetWindowText(hWnd, strText, _countof(strText));
			DrawText(hDC, strText, -1, rcClient, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
		
		::SelectObject(hDC, hOldBrush);
		::DeleteObject(::SelectObject(hDC, hOldPen));
	}
	
	::SelectObject(hDC, hOldFont);
	::SetBkMode(hDC, nOldMode);
	::SetTextColor(hDC, crOldColor);

	Mem.Draw(hWndDC, rcClient.left, rcClient.top);
}

void CQButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (CLRTYPE_DARK != g_nColorType)
	{
		CButton::OnMouseMove(nFlags, point);
	}
}

void CQButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	HDC dc = ::GetDC(m_hWnd);
	DrawButton(m_hWnd, dc, m_bDown);
	::ReleaseDC(m_hWnd, dc);
}

LRESULT CQButton::OnSetText(WPARAM wParam, LPARAM lParam)
{
	Default();

	HDC dc = ::GetDC(m_hWnd);
	DrawButton(m_hWnd, dc, m_bDown);
	::ReleaseDC(m_hWnd, dc);

	return 0;
}

BOOL CQButton::PreTranslateMessage(MSG* pMsg) 
{
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
		m_bDown = TRUE;
		break;

	case WM_LBUTTONDBLCLK:
		m_bDown = TRUE;
		break;

	case WM_LBUTTONUP:
		m_bDown = FALSE;
		break;
	}
	
	return CButton::PreTranslateMessage(pMsg);
}

