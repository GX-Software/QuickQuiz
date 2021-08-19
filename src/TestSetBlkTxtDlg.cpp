// TestSetBlkTxtDlg.cpp : implementation file
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
// CTestSetBlkTxtDlg dialog


CTestSetBlkTxtDlg::CTestSetBlkTxtDlg(CWnd* pParent /*=NULL*/) :
	CTabPage(CTestSetBlkTxtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestSetBlkTxtDlg)
	m_bBlkAllRight = FALSE;
	m_bOnlyBlk = FALSE;
	m_bRandomBlk = FALSE;
	m_bTxtAllRight = FALSE;
	//}}AFX_DATA_INIT
}


void CTestSetBlkTxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSetBlkTxtDlg)
	DDX_Control(pDX, IDC_CHECK_TXTALLRIGHT, m_cCheckTxtAllRight);
	DDX_Control(pDX, IDC_CHECK_RANDOMBLK, m_cCheckRandomBlk);
	DDX_Control(pDX, IDC_CHECK_ONLYBLK, m_cCheckOnlyBlk);
	DDX_Control(pDX, IDC_CHECK_BLKALLRIGHT, m_cCheckBlkAllRight);
	DDX_Check(pDX, IDC_CHECK_BLKALLRIGHT, m_bBlkAllRight);
	DDX_Check(pDX, IDC_CHECK_ONLYBLK, m_bOnlyBlk);
	DDX_Check(pDX, IDC_CHECK_RANDOMBLK, m_bRandomBlk);
	DDX_Check(pDX, IDC_CHECK_TXTALLRIGHT, m_bTxtAllRight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestSetBlkTxtDlg, CTabPage)
	//{{AFX_MSG_MAP(CTestSetBlkTxtDlg)
	ON_BN_CLICKED(IDC_CHECK_ONLYBLK, OnCheckOnlyBlk)
	ON_BN_CLICKED(IDC_CHECK_RANDOMBLK, OnCheckRandomBlk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestSetBlkTxtDlg message handlers

void CTestSetBlkTxtDlg::OnCheckOnlyBlk() 
{
	((CButton*)GetDlgItem(IDC_CHECK_RANDOMBLK))->SetCheck(FALSE);
}

void CTestSetBlkTxtDlg::OnCheckRandomBlk() 
{
	((CButton*)GetDlgItem(IDC_CHECK_ONLYBLK))->SetCheck(FALSE);
}

BOOL CTestSetBlkTxtDlg::OnKillActive(BOOL bIsDestroy)
{
	UpdateData();

	return CTabPage::OnKillActive(bIsDestroy);
}

void CTestSetBlkTxtDlg::InitTestPara(CTestPara *pCPara)
{
	if (!pCPara)
	{
		return;
	}
	PTESTPARA pPara = pCPara->GetTestPara();

	m_bBlkAllRight = (pPara->dwBlkOptions & BLKOPTION_ALLRIGHT) ? TRUE : FALSE;
	m_bOnlyBlk = (pPara->dwBlkOptions & BLKOPTION_ONLYBLK) ? TRUE : FALSE;
	m_bRandomBlk = (pPara->dwBlkOptions & BLKOPTION_RANDOMBLK) ? TRUE : FALSE;
	m_bTxtAllRight = (pPara->dwBlkOptions & TXTOPTION_ALLRIGHT) ? TRUE : FALSE;
}

void CTestSetBlkTxtDlg::FillTestPara(CTestPara *pCPara)
{
	if (!pCPara)
	{
		return;
	}
	PTESTPARA pPara = pCPara->GetTestPara();
	
	pPara->dwBlkOptions = pPara->dwTxtOptions = 0;
	pPara->dwBlkOptions |= (m_bBlkAllRight ? BLKOPTION_ALLRIGHT : 0);
	pPara->dwBlkOptions |= (m_bOnlyBlk ? BLKOPTION_ONLYBLK: 0);
	pPara->dwBlkOptions |= (m_bRandomBlk ? BLKOPTION_RANDOMBLK: 0);
	pPara->dwTxtOptions |= (m_bTxtAllRight ? TXTOPTION_ALLRIGHT: 0);
}
