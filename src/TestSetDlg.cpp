// TestSetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "TestSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR lpszTestDlg = _T("Test Dialog");
static LPCTSTR lpszTestPara = _T("Test Parameter");

/////////////////////////////////////////////////////////////////////////////
// CTestSetDlg

CTestSetDlg::CTestSetDlg(UINT nIDCaption, CList *pPaperList, CPaper *pCurSelPaper, CWnd* pParent /*=NULL*/) :
	CClrDialog(CTestSetDlg::IDD, pParent),
	m_CommDlg(pPaperList, pCurSelPaper),
	m_nIDCaption(nIDCaption)
{
}

CTestSetDlg::~CTestSetDlg()
{
}

void CTestSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSetDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDCANCEL, m_cBtnCancel);
	DDX_Control(pDX, IDC_TAB, m_cTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestSetDlg, CClrDialog)
	//{{AFX_MSG_MAP(CTestSetDlg)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK, OnOK)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestSetDlg message handlers

void CTestSetDlg::OnOK()
{
	if (m_CommDlg.CheckCanExit())
	{
		m_ChsDlg.UpdateData();

		CDialog::OnOK();
	}
}

int CTestSetDlg::DoModal() 
{
	m_CommDlg.SetChsDlg(&m_ChsDlg);
	LoadSetPara();
	
	int nRet = CDialog::DoModal();
	if (IDOK == nRet)
	{
		m_CommDlg.FillTestPara(&m_TestPara);
		m_ChsDlg.FillTestPara(&m_TestPara);
		m_BlkTxtDlg.FillTestPara(&m_TestPara);

		SaveSetPara();
	}

	return nRet;
}

BOOL CTestSetDlg::OnInitDialog()
{
	CClrDialog::OnInitDialog();

	CString string;
	string.LoadString(IDS_TEST_TESTSET);
	SetWindowText(string);

	string.LoadString(IDS_TESTSET_COMMSET);
	m_cTab.AddPage(IDD_TESTSET_COMMDLG, &m_CommDlg, string);

	string.LoadString(IDS_TESTSET_CHSSET);
	m_cTab.AddPage(IDD_TESTSET_CHSDLG, &m_ChsDlg, string);

	string.LoadString(IDS_TESTSET_BLKTXTSET);
	m_cTab.AddPage(IDD_TESTSET_BLKTXTDLG, &m_BlkTxtDlg, string);

	return FALSE;
}

void CTestSetDlg::LoadSetPara()
{
	if (m_TestPara.LoadTestPara())
	{
		m_CommDlg.InitTestPara(&m_TestPara);
		m_ChsDlg.InitTestPara(&m_TestPara);
		m_BlkTxtDlg.InitTestPara(&m_TestPara);
	}
}

void CTestSetDlg::SaveSetPara()
{
	m_TestPara.SaveTestPara();
}