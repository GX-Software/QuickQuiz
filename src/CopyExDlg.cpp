// CopyExDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "CopyExDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopyExDlg dialog


CCopyExDlg::CCopyExDlg(CWnd* pParent /*=NULL*/)
	: CClrDialog(CCopyExDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCopyExDlg)
	m_nSel = 0;
	//}}AFX_DATA_INIT
}


void CCopyExDlg::DoDataExchange(CDataExchange* pDX)
{
	CClrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCopyExDlg)
	DDX_Control(pDX, IDC_RADIO_WITHASW, m_cRadioAsw);
	DDX_Control(pDX, IDC_RADIO_ONLYQ, m_cRadioQ);
	DDX_Control(pDX, IDC_RADIO_ALL, m_cRadioAll);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Radio(pDX, IDC_RADIO_ONLYQ, m_nSel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopyExDlg, CClrDialog)
	//{{AFX_MSG_MAP(CCopyExDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCopyExDlg message handlers
