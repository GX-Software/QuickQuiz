// ClrDialog.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "ClrDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClrDialog dialog


CClrDialog::CClrDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CClrDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CClrDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClrDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CClrDialog, CDialog)
	//{{AFX_MSG_MAP(CClrDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrDialog message handlers

HBRUSH CClrDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	// 首先交给子窗口处理
	LRESULT lResult;
	if (pWnd->SendChildNotifyLastMsg(&lResult))
	{
		return (HBRUSH)lResult;
	}

	switch(g_nColorType)
	{
	case CLRTYPE_DEFAULT:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		pDC->SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		pDC->SetBkColor(AfxGetQColor(QCOLOR_FACE));
		return AfxGetQBrush();

	default:
		ASSERT(FALSE);
	}

	return NULL;
}

BOOL CClrDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SendMessage(WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);
	SendMessageToDescendants(WM_SETFONT, (WPARAM)g_hNormalFont, MAKELPARAM(FALSE, 0), TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
