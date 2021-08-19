// QComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "QComboBox.h"

#include "MemBmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQComboBox

CQComboBox::CQComboBox()
{
}

CQComboBox::~CQComboBox()
{
}


BEGIN_MESSAGE_MAP(CQComboBox, CComboBox)
	//{{AFX_MSG_MAP(CQComboBox)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQComboBox message handlers

void CQComboBox::OnPaint() 
{
	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		CComboBox::OnPaint();
	}
	else
	{
		CPaintDC paintDC(this);

		CRect rcClient; 
		GetWindowRect(rcClient);
		ScreenToClient(rcClient);

		CMemBmp Mem;
		HDC dc = Mem.CreateBmp(paintDC.GetSafeHdc(), rcClient, TRUE, AfxGetQColor(QCOLOR_FACE));

		HPEN hOldPen = (HPEN)SelectObject(dc, CreatePen(PS_SOLID, 1, AfxGetQColor(QCOLOR_BORDER)));
		HBRUSH hOldBrush = (HBRUSH)SelectObject(dc, AfxGetQBrush());

		// 绘制边框
		RoundRect(dc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom, 4, 4);

		// 绘制阴影
		BOOL bDroppedState = GetDroppedState();
		CRect rcShadow(rcClient);
		rcShadow.DeflateRect(1, 1, 1, 1);
		if ((bDroppedState && CLRTYPE_DARK == g_nColorType) ||
			(!bDroppedState && CLRTYPE_GREEN == g_nColorType))
		{
			rcShadow.top += rcShadow.Height() / 2;
		}
		else
		{
			rcShadow.bottom -= rcShadow.Height() / 2;
		}
		
		HBRUSH hShadowBrush = CreateSolidBrush(AfxGetQColor(QCOLOR_SHADOW));
		FillRect(dc, rcShadow, hShadowBrush);
		DeleteObject(hShadowBrush);

		// 如果已经下拉，则添加一个内框
		if (bDroppedState)
		{
			rcShadow = rcClient;
			rcShadow.DeflateRect(1, 1, 1, 1);

			::DeleteObject(::SelectObject(dc, CreatePen(PS_SOLID, 1, AfxGetQColor(QCOLOR_HIGHLIGHT))));
			::SelectObject(dc, GetStockObject(NULL_BRUSH));
			Rectangle(dc, rcShadow.left, rcShadow.top, rcShadow.right, rcShadow.bottom);
		}

		BOOL bEnable = ::IsWindowEnabled(m_hWnd);
		int nTextColor = bEnable ? QCOLOR_TEXT : QCOLOR_TEXTDISABLE;

		// 画小箭头
		DeleteObject(SelectObject(dc, CreatePen(PS_SOLID, 1, AfxGetQColor(nTextColor))));
		SelectObject(dc, CreateSolidBrush(AfxGetQColor(nTextColor)));

		POINT pt[3] = {0};
		GetPoints(rcClient, pt);
		Polygon(dc, pt, 3);

		DeleteObject(SelectObject(dc, hOldPen));
		DeleteObject(SelectObject(dc, hOldBrush));

		HFONT hFont = (HFONT)SendMessage(WM_GETFONT, 0, 0);
		if (!hFont)
		{
			hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		}
		HFONT hOldFont = (HFONT)SelectObject(dc, hFont);

		COLORREF clrOld = SetTextColor(dc, AfxGetQColor(nTextColor));
		int nOldMode = SetBkMode(dc, TRANSPARENT);
		
		CString strText;
		CRect rcText = rcClient;
		GetWindowText(strText);

		rcText.DeflateRect(5, 1, 1, 1);
		rcText.right -= rcText.Height();
		DrawText(dc, strText, -1, rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);

		SelectObject(dc, hOldFont);
		SetTextColor(dc, clrOld);
		SetBkMode(dc, nOldMode);

		Mem.Draw(paintDC.GetSafeHdc(), rcClient.left, rcClient.top);
	}
}

void CQComboBox::GetPoints(LPRECT rc, LPPOINT pt)
{
	CRect rcBtn(rc);
	rcBtn.left = rcBtn.right - rcBtn.Height();
	
	pt[0].x = rcBtn.left + (rcBtn.Width() - 7) / 2;
	pt[0].y = rcBtn.top + (rcBtn.Height() - 4) / 2;
	
	pt[1].x = pt[0].x + 6;
	pt[1].y = pt[0].y;
	
	pt[2].x = pt[0].x + 3;
	pt[2].y = pt[0].y + 3;
}

BOOL CQComboBox::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
