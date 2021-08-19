// BatchSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BatchSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBatchSetDlg dialog


CBatchSetDlg::CBatchSetDlg(BOOL bEditFlag, CWnd* pParent /*=NULL*/) :
	CClrDialog(CBatchSetDlg::IDD, pParent),
	m_bEditFlag(bEditFlag)
{
	//{{AFX_DATA_INIT(CBatchSetDlg)
	m_nIndex = -1;
	m_nSimilar = 90;
	//}}AFX_DATA_INIT
}


void CBatchSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CClrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchSetDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_RADIO_MTOS, m_cRadioMToS);
	DDX_Control(pDX, IDC_RADIO_STOM, m_cRadioSToM);
	DDX_Control(pDX, IDC_RADIO_SELDUPLICATE, m_cRadioSelDuplicate);
	DDX_Radio(pDX, IDC_RADIO_STOM, m_nIndex);
	DDX_Text(pDX, IDC_EDIT_SIMILAR, m_nSimilar);
	DDX_Control(pDX, IDC_EDIT_SIMILAR, m_cEditSimilar);
	DDX_Control(pDX, IDC_RADIO_CTOJ, m_cRadioCToJ);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchSetDlg, CClrDialog)
	//{{AFX_MSG_MAP(CBatchSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchSetDlg message handlers

BOOL CBatchSetDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	m_cRadioSToM.EnableWindow(m_bEditFlag);
	m_cRadioMToS.EnableWindow(m_bEditFlag);
	m_cRadioCToJ.EnableWindow(m_bEditFlag);

	m_cEditSimilar.SetLimitText(3);
	SetDlgItemInt(IDC_EDIT_SIMILAR, 99, FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CBatchSetDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wCode = HIWORD(wParam);
	WORD wID = LOWORD(wParam);
	
	if (EN_KILLFOCUS == wCode)
	{
		return CheckInput(wID);
	}
	
	return CClrDialog::OnCommand(wParam, lParam);
}

BOOL CBatchSetDlg::CheckInput(UINT nID /* = 0 */)
{
	if (nID)
	{
		int nNum = GetDlgItemInt(nID);
		
		switch(nID)
		{
		case IDC_EDIT_SIMILAR:
			nNum = max(80, min(nNum, 100));
			break;
			
		default:
			return !0;
		}
		
		SetDlgItemInt(nID, nNum);
	}

	return 0;
}
