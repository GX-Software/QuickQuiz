// XPGroupBox.cpp : implementation file
//

#include "stdafx.h"
#include "QGroupBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CQGroupBox, CButton);

/////////////////////////////////////////////////////////////////////////////
// CQGroupBox

CQGroupBox::CQGroupBox() :
	m_nType(XPGB_FRAME), m_dwAlignment(SS_LEFT)
{
    m_strTitle = _T("");
	
	m_clrBorder = ::GetSysColor(COLOR_3DSHADOW);
	m_clrClientBackground = ::GetSysColor(COLOR_BTNFACE);

	m_clrTitleText = ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrTitleBackground = ::GetSysColor(COLOR_BTNFACE);
}

CQGroupBox::~CQGroupBox()
{
}


BEGIN_MESSAGE_MAP(CQGroupBox, CButton)
	//{{AFX_MSG_MAP(CQGroupBox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQGroupBox message handlers

void CQGroupBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
   	CRect	rectClient;
	GetClientRect(rectClient);
	
	// Defalte Rect
	rectClient.DeflateRect(1,1);
	
	// Get Text Rect 
	CSize sizeText;
	CRect rectText, rectFrame;
	CRect rectTitle, rectContent;
	
	CFont *pOldFont = dc.SelectObject(&m_font);
  		
	// get Text if need
	if (m_strTitle.IsEmpty())
	{ 
        GetWindowText(m_strTitle);
		if (!m_strTitle.IsEmpty())
			m_strTitle = _T(" ") + m_strTitle + _T(" ");
	}
	
	if ( ! m_strTitle.IsEmpty() )
	{
		sizeText = dc.GetTextExtent(m_strTitle);
	}
	else
	{
		sizeText.cx = 0;
		sizeText.cy = 0;
	}
	
	if (m_nType == XPGB_FRAME) // Frame style
	{
		// Calculate Text Rect 
		switch(m_dwAlignment)
		{
		case SS_LEFT:	
			rectText.top = rectClient.top;
			rectText.left = rectClient.left + 10;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx;
			break;
		case SS_CENTER:	
			rectText.top = rectClient.top;
			rectText.left = rectClient.left + (rectClient.Width() - sizeText.cx) / 2 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx;
			break;
		case SS_RIGHT:
			rectText.top = rectClient.top;
			rectText.right = rectClient.right -10 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.left = rectText.right - sizeText.cx;
			break;
		}
		
		//  Calculate Frame rect
		rectFrame.left = rectClient.left;
		rectFrame.top = rectClient.top + sizeText.cy/2;
		
		rectFrame.right = rectClient.right;
		rectFrame.bottom = rectFrame.top + rectClient.Height() - sizeText.cy/2; 
		
		// Draw Frame border
		CPen penFrame;
		CBrush brushBKFrame(m_clrTitleBackground);
		
		penFrame.CreatePen(PS_SOLID, 1, m_clrBorder);
		
		CPen* pOldPen = dc.SelectObject(&penFrame);
		CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		
		dc.RoundRect(rectFrame, CPoint(6, 6)); 
		
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush); 
		
		dc.IntersectClipRect(rectText);
		dc.FillSolidRect(rectText, m_clrTitleBackground);
	}
	else  // Windows Style
	{
		// Calculate Title size
		rectTitle.top = rectClient.top;
		rectTitle.left = rectClient.left ;
		
		rectTitle.right = rectClient.right;
		rectTitle.bottom = rectClient.top + sizeText.cy + 4;
		
		// Draw Title round rect
		CPen penFrame;
		CBrush brushBKTitle(m_clrTitleBackground);
		CBrush brushBKContent(m_clrClientBackground);
		
		penFrame.CreatePen(PS_SOLID, 1, m_clrBorder);
		
		CPen* pOldPen = dc.SelectObject(&penFrame);
		CBrush* pOldBrush = dc.SelectObject(&brushBKTitle);
		
		dc.RoundRect(rectClient, CPoint(6, 6)); 
		
		dc.SelectObject(pOldBrush); 
		
		// Draw content area
		rectContent.left = rectClient.left;
		rectContent.top = rectClient.top + sizeText.cy + 4;
		
		rectContent.right = rectClient.right;
		rectContent.bottom = rectContent.top + rectClient.Height() - sizeText.cy - 4; 
		
		pOldBrush = dc.SelectObject(&brushBKContent); 
		
		dc.Rectangle(rectContent);  
		
		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush); 
		
		
		// Calculate Text Rect 
		switch(m_dwAlignment)
		{
		case SS_LEFT:	
			rectText.top = rectTitle.top + 2;
			rectText.left = rectTitle.left + 2 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx ;
			break;
		case SS_CENTER:	
			rectText.top = rectTitle.top + 2;
			rectText.left = rectTitle.left + (rectTitle.Width() - sizeText.cx) / 2 ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.right = rectText.left + sizeText.cx ;
			break;
		case SS_RIGHT:
			rectText.top = rectTitle.top + 2;
			rectText.right = rectClient.right - 2  ;
			
			rectText.bottom = rectText.top + sizeText.cy;
			rectText.left = rectText.right - sizeText.cx;
			break;
		}
	}
    
	COLORREF clrOldText = dc.SetTextColor(m_clrTitleText);
	UINT nMode = dc.SetBkMode(TRANSPARENT);
	
	// 使背景和对话框风格保持一致
	FillRect(dc.m_hDC, rectText,
		(HBRUSH)GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc.m_hDC, (LPARAM)m_hWnd));
	dc.DrawText(m_strTitle, &rectText, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP );
	
	// restore DC
	dc.SetBkMode(nMode);
    dc.SetTextColor(clrOldText);
    dc.SelectObject(pOldFont);
}

BOOL CQGroupBox::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	dwStyle |= BS_ICON;
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

BOOL CQGroupBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= BS_ICON;
	return CButton::PreCreateWindow(cs);
}

void CQGroupBox::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
	
	//modified the style to avoid text overlap when press tab 
	ModifyStyle(0, BS_ICON);
	
	// Get Defalut Font 
	GetObject(g_hNormalFont, sizeof(LOGFONT), &m_lf);
	
	ReconstructFont();
}

void CQGroupBox::ReconstructFont()
{
	m_font.DeleteObject();
	BOOL bCreated = m_font.CreateFontIndirect(&m_lf);
	
	ASSERT(bCreated);
}

void CQGroupBox::UpdateSurface()
{
	CRect (rc);
	GetWindowRect(rc);
	RedrawWindow();
	
	GetParent()->ScreenToClient(rc);
	GetParent()->InvalidateRect(rc, TRUE);
	GetParent()->UpdateWindow();
}

CQGroupBox& CQGroupBox::SetQGroupStyle(QGroupBoxStyle eStyle) 
{
	m_nType = eStyle;
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetFont(LOGFONT lf)
{
	CopyMemory(&m_lf, &lf, sizeof(m_lf));
	ReconstructFont();
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetFontBold(BOOL bBold)
{
	m_lf.lfWeight = bBold ? FW_BOLD : FW_NORMAL;
	ReconstructFont();
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetFontName(const CString& strFont, BYTE byCharSet)
{
	m_lf.lfCharSet = byCharSet;
	
	_tcscpy(m_lf.lfFaceName,strFont);
	ReconstructFont();
	UpdateSurface();
	
	return *this;
}

CQGroupBox& CQGroupBox::SetFontUnderline(BOOL bSet)
{
	m_lf.lfUnderline = bSet;
	ReconstructFont();
	UpdateSurface();
	
	return *this;
}

CQGroupBox& CQGroupBox::SetFontItalic(BOOL bSet)
{
	m_lf.lfItalic = bSet;
	ReconstructFont();
	UpdateSurface();
	
	return *this;	
}

CQGroupBox& CQGroupBox::SetFontSize(int nSize)
{
	CFont cf;
	LOGFONT lf;
	
	cf.CreatePointFont(nSize * 10, m_lf.lfFaceName);
	cf.GetLogFont(&lf);
	
	m_lf.lfHeight = lf.lfHeight;
	m_lf.lfWidth  = lf.lfWidth;
	
	ReconstructFont();
	UpdateSurface();
	
	return *this;
}

CQGroupBox& CQGroupBox::SetBorderColor(COLORREF clrBorder)
{
	m_clrBorder = clrBorder;
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetCatptionTextColor(COLORREF clrText ) 
{
	m_clrTitleText = clrText;
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetBackgroundColor(COLORREF clrBKClient)
{
	m_clrTitleBackground = clrBKClient;
	m_clrClientBackground = clrBKClient;
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetBackgroundColor(COLORREF clrBKTilte,  COLORREF clrBKClient)
{
	m_clrTitleBackground = clrBKTilte;
	m_clrClientBackground = clrBKClient;
	UpdateSurface();
	return *this;
}

CQGroupBox& CQGroupBox::SetText(LPCTSTR lpszText)
{
	if(IsWindow(this->GetSafeHwnd())) 
	{
		m_strTitle = lpszText;
		m_strTitle = _T(" ") + m_strTitle + _T(" ");
		UpdateSurface();
	}
	
	return *this;
}

CQGroupBox& CQGroupBox::SetAlignment(DWORD dwType)
{
	switch(dwType)
	{
	case SS_LEFT:	
		m_dwAlignment =  SS_LEFT;
		break;
		
	case SS_CENTER:	
		m_dwAlignment = SS_CENTER;
		break;
		
	case SS_RIGHT:
		m_dwAlignment = SS_RIGHT;
		break;
		
	default:
		ASSERT(false);
	}
	UpdateSurface();
	
	return *this;
}
