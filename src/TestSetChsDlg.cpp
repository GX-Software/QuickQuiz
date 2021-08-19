// TestSetChsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "TestSetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestSetChsDlg dialog


CTestSetChsDlg::CTestSetChsDlg(CWnd* pParent /*=NULL*/)
	: CTabPage(CTestSetChsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestSetChsDlg)
	m_bMChAsSCh = FALSE;
	m_bMChHalf = FALSE;
	m_bMChNoSingle = FALSE;
	m_bRandom = FALSE;
	m_bSChAsMCh = FALSE;
	//}}AFX_DATA_INIT
}


void CTestSetChsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSetChsDlg)
	DDX_Control(pDX, IDC_CHECK_SCHASMCH, m_cCheckSAsM);
	DDX_Control(pDX, IDC_CHECK_RANDOM, m_cCheckRandom);
	DDX_Control(pDX, IDC_CHECK_MCHNOSINGLE, m_cCheckMNoSingle);
	DDX_Control(pDX, IDC_CHECK_MCHHALF, m_cCheckHalf);
	DDX_Control(pDX, IDC_CHECK_MCHASSCH, m_cCheckMAsS);
	DDX_Check(pDX, IDC_CHECK_MCHASSCH, m_bMChAsSCh);
	DDX_Check(pDX, IDC_CHECK_MCHHALF, m_bMChHalf);
	DDX_Check(pDX, IDC_CHECK_MCHNOSINGLE, m_bMChNoSingle);
	DDX_Check(pDX, IDC_CHECK_RANDOM, m_bRandom);
	DDX_Check(pDX, IDC_CHECK_SCHASMCH, m_bSChAsMCh);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestSetChsDlg, CTabPage)
	//{{AFX_MSG_MAP(CTestSetChsDlg)
	ON_BN_CLICKED(IDC_CHECK_MCHNOSINGLE, OnCheckMChNoSingle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestSetChsDlg message handlers

BOOL CTestSetChsDlg::OnKillActive(BOOL bIsDestroy)
{
	UpdateData();

	return CTabPage::OnKillActive(bIsDestroy);
}

void CTestSetChsDlg::InitTestPara(CTestPara *pCPara)
{
	if (!pCPara)
	{
		return;
	}
	PTESTPARA pPara = pCPara->GetTestPara();

	m_bSChAsMCh = (pPara->dwChsOptions & CHSOPTION_SCHASMCH) ? TRUE : FALSE;
	m_bMChAsSCh = (pPara->dwChsOptions & CHSOPTION_MCHASSCH) ? TRUE : FALSE;
	m_bMChHalf = (pPara->dwChsOptions & CHSOPTION_MCHHALF) ? TRUE : FALSE;
	m_bMChNoSingle = (pPara->dwChsOptions & CHSOPTION_MCHNOSINGLE) ? TRUE : FALSE;
	m_bRandom = (pPara->dwChsOptions & CHSOPTION_RANDOM) ? TRUE : FALSE;
}

void CTestSetChsDlg::FillTestPara(CTestPara *pCPara)
{
	if (!pCPara)
	{
		return;
	}
	PTESTPARA pPara = pCPara->GetTestPara();
	
	pPara->dwChsOptions = 0;
	pPara->dwChsOptions |= (m_bSChAsMCh ? CHSOPTION_SCHASMCH : 0);
	pPara->dwChsOptions |= (m_bMChAsSCh ? CHSOPTION_MCHASSCH : 0);
	pPara->dwChsOptions |= (m_bMChHalf ? CHSOPTION_MCHHALF : 0);
	pPara->dwChsOptions |= (m_bMChNoSingle ? CHSOPTION_MCHNOSINGLE : 0);
	pPara->dwChsOptions |= (m_bRandom ? CHSOPTION_RANDOM : 0);
}

void CTestSetChsDlg::OnCheckMChNoSingle() 
{
	UpdateData();

	if (m_bMChNoSingle)
	{
		m_bSChAsMCh = FALSE;
		UpdateData(FALSE);

		GetDlgItem(IDC_CHECK_SCHASMCH)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_SCHASMCH)->EnableWindow(TRUE);
	}
}
