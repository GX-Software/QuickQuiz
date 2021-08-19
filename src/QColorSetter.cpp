// QColorSetter.cpp : implementation file
//

#include "stdafx.h"
#include "QColorSetter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQColorSetter

CQColorSetter::CQColorSetter() :
	m_bReadOnly(FALSE)
{
}

CQColorSetter::~CQColorSetter()
{
}


BEGIN_MESSAGE_MAP(CQColorSetter, CWnd)
	//{{AFX_MSG_MAP(CQColorSetter)
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQColorSetter message handlers

void CQColorSetter::PreSubclassWindow() 
{
	GetClientRect(m_rcClient);
	ModifyStyle(NULL, SS_NOTIFY);
	
	CWnd::PreSubclassWindow();
}

BOOL CQColorSetter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_bReadOnly)
	{
		return 0;
	}
	else
	{
		::SetCursor(LoadCursor(NULL, IDC_HAND));
		return 1;
	}
}

void CQColorSetter::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	HPEN pen = ::CreatePen(PS_SOLID, 1, 0);

	dc.SelectObject(pen);
	dc.SelectObject(GetStockObject(NULL_BRUSH));

	dc.FillSolidRect(0, 0, m_rcClient.Width(), m_rcClient.Height(), m_clr);
	dc.Rectangle(m_rcClient);

	DeleteObject(pen);
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CQColorSetter::SetColor(COLORREF clr)
{
	m_clr = clr;
	Invalidate(TRUE);
}