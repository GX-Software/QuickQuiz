// TestSetCommDlg.cpp : implementation file
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
// CTestSetCommDlg dialog


CTestSetCommDlg::CTestSetCommDlg(CList *pPaperList, CPaper *pCurSelPaper, CWnd* pParent /*=NULL*/) :
	CTabPage(CTestSetCommDlg::IDD, pParent),
	m_pPaperList(pPaperList), m_pCurSelPaper(pCurSelPaper),
	m_pChsDlg(NULL),
	m_bCanExit(TRUE)
{
	//{{AFX_DATA_INIT(CTestSetCommDlg)
	m_nSChCount = 0;
	m_nSChPoint = 1;
	m_nMChCount = 0;
	m_nMChPoint = 1;
	m_nJdgCount = 0;
	m_nJdgPoint = 1;
	m_nBlkCount = 0;
	m_nBlkPoint = 1;
	m_nTxtCount = 0;
	m_nTxtPoint = 1;
	m_nTimeHour = 0;
	m_nTimeMinute = 30;
	m_nFullMark = 100;
	m_nMarkType = MARKTYPE_AVERAGE;
	m_nCustCount = 0;
	m_nCustPoint = 1;
	m_nGrpCount = 0;
	m_bUseStore = FALSE;
	m_bQNumOrder = FALSE;
	//}}AFX_DATA_INIT

	ZeroMemory(m_nItemCount, sizeof(m_nItemCount));
	ZeroMemory(m_sCustInfo, sizeof(m_sCustInfo));
}


void CTestSetCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestSetCommDlg)
	DDX_Control(pDX, IDC_CHECK_QNUMORDER, m_cCheckQNumOrder);
	DDX_Control(pDX, IDC_EDIT_GRPCOUNT, m_cEditGrpCount);
	DDX_Control(pDX, IDC_EDIT_CUSTPOINT, m_cEditCustPoint);
	DDX_Control(pDX, IDC_EDIT_CUSTCOUNT, m_cEditCustCount);
	DDX_Control(pDX, IDC_COMBO_CUSTOMTYPE2, m_cComboType2);
	DDX_Control(pDX, IDC_COMBO_CUSTOMTYPE1, m_cComboType1);
	DDX_Control(pDX, IDC_RADIO_EQU, m_cRadioEQU);
	DDX_Control(pDX, IDC_RADIO_DIF, m_cRadioDIF);
	DDX_Control(pDX, IDC_EDIT_MINUTE, m_cEditMinute);
	DDX_Control(pDX, IDC_EDIT_HOUR, m_cEditHour);
	DDX_Control(pDX, IDC_EDIT_FULLMARK, m_cEditFullMark);
	DDX_Control(pDX, IDC_EDIT_SCHCOUNT, m_cEditSChCount);
	DDX_Control(pDX, IDC_EDIT_MCHCOUNT, m_cEditMChCount);
	DDX_Control(pDX, IDC_EDIT_JDGCOUNT, m_cEditJdgCount);
	DDX_Control(pDX, IDC_EDIT_BLKCOUNT, m_cEditBlkCount);
	DDX_Control(pDX, IDC_EDIT_TXTCOUNT, m_cEditTxtCount);
	DDX_Control(pDX, IDC_EDIT_SCHPOINT, m_cEditSChPoint);
	DDX_Control(pDX, IDC_EDIT_MCHPOINT, m_cEditMChPoint);
	DDX_Control(pDX, IDC_EDIT_JDGPOINT, m_cEditJdgPoint);
	DDX_Control(pDX, IDC_EDIT_BLKPOINT, m_cEditBlkPoint);
	DDX_Control(pDX, IDC_EDIT_TXTPOINT, m_cEditTxtPoint);
	DDX_Control(pDX, IDC_CHECK_USESTORE, m_cCheckUserStore);
	DDX_Control(pDX, IDC_COMBO_PAPERPATH, m_PaperCombo);
	DDX_Text(pDX, IDC_EDIT_SCHCOUNT, m_nSChCount);
	DDX_Text(pDX, IDC_EDIT_SCHPOINT, m_nSChPoint);
	DDX_Text(pDX, IDC_EDIT_MCHCOUNT, m_nMChCount);
	DDX_Text(pDX, IDC_EDIT_MCHPOINT, m_nMChPoint);
	DDX_Text(pDX, IDC_EDIT_JDGCOUNT, m_nJdgCount);
	DDX_Text(pDX, IDC_EDIT_JDGPOINT, m_nJdgPoint);
	DDX_Text(pDX, IDC_EDIT_BLKCOUNT, m_nBlkCount);
	DDX_Text(pDX, IDC_EDIT_BLKPOINT, m_nBlkPoint);
	DDX_Text(pDX, IDC_EDIT_TXTCOUNT, m_nTxtCount);
	DDX_Text(pDX, IDC_EDIT_TXTPOINT, m_nTxtPoint);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_nTimeHour);
	DDX_Text(pDX, IDC_EDIT_MINUTE, m_nTimeMinute);
	DDX_Text(pDX, IDC_EDIT_FULLMARK, m_nFullMark);
	DDX_Radio(pDX, IDC_RADIO_EQU, m_nMarkType);
	DDX_Text(pDX, IDC_EDIT_CUSTCOUNT, m_nCustCount);
	DDX_Text(pDX, IDC_EDIT_CUSTPOINT, m_nCustPoint);
	DDX_Text(pDX, IDC_EDIT_GRPCOUNT, m_nGrpCount);
	DDX_Check(pDX, IDC_CHECK_USESTORE, m_bUseStore);
	DDX_Check(pDX, IDC_CHECK_QNUMORDER, m_bQNumOrder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestSetCommDlg, CTabPage)
	//{{AFX_MSG_MAP(CTestSetCommDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PAPERPATH, OnComboSelChange)
	ON_BN_CLICKED(IDC_CHECK_USESTORE, OnCheckUseStore)
	ON_BN_CLICKED(IDC_RADIO_EQU, OnRadioEqu)
	ON_BN_CLICKED(IDC_RADIO_DIF, OnRadioDif)
	ON_CBN_SELCHANGE(IDC_COMBO_CUSTOMTYPE1, OnSelChangeCountCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_CUSTOMTYPE2, OnSelChangePointCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestSetCommDlg message handlers

BOOL CTestSetCommDlg::OnInitDialog() 
{
	CTabPage::OnInitDialog();
	
	FillPaperList();
	FillTypeList();

	ASSERT(m_pCurSelPaper);
	if (!m_pCurSelPaper->GetSaveList()->GetItemCount())
	{
		m_bUseStore = FALSE;
	}
	UpdateData(FALSE);

	if (m_nMarkType == MARKTYPE_AVERAGE)
	{
		OnRadioEqu();
	}
	else
	{
		OnRadioDif();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTestSetCommDlg::FillPaperList()
{
	CString string;
	TCHAR strFilePath[MAX_PATH + 5] = {0};
	int i;
	for (i = 0; i < m_pPaperList->GetItemCount(); ++i)
	{
		CPaper *pPaper = (CPaper*)m_pPaperList->GetItem(i);
		if (!pPaper)
		{
			break;
		}

		pPaper->GetShortFilePath(strFilePath, MAX_PATH + 5);
		string.Format(_T("%s(%s)"), pPaper->GetDescription(), strFilePath);
		m_PaperCombo.AddString(string);
	}

	m_PaperCombo.SetCurSel(m_pPaperList->GetIndex(m_pCurSelPaper));
}

void CTestSetCommDlg::FillTypeList()
{
	ASSERT(m_pCurSelPaper);

	int nType = TYPE_CUSTOMRIGHT;
	CList *pList = m_pCurSelPaper->GetTypeList(nType);

	m_cComboType1.ResetContent();
	m_cComboType2.ResetContent();
	m_cComboType1.EnableWindow((BOOL)pList);
	m_cComboType2.EnableWindow((BOOL)pList);

	while(pList)
	{
		m_cComboType1.AddString(pList->GetDescription());
		m_cComboType2.AddString(pList->GetDescription());

		nType += TYPE_CUSTOMRIGHT;
		pList = m_pCurSelPaper->GetTypeList(nType);
	}

	m_cComboType1.SetCurSel(0);
	m_cComboType2.SetCurSel(0);
}

void CTestSetCommDlg::InitTestPara(CTestPara *pCPara)
{
	if (!pCPara)
	{
		return;
	}
	PTESTPARA pPara = pCPara->GetTestPara();

	m_bUseStore = pPara->bUseStore;
	m_bQNumOrder = pPara->bQNumOrder;

	m_nSChCount = pPara->nSChCount;
	m_nMChCount = pPara->nMChCount;
	m_nJdgCount = pPara->nJdgCount;
	m_nBlkCount = pPara->nBlkCount;
	m_nTxtCount = pPara->nTxtCount;
	m_nGrpCount = pPara->nGrpCount;
	m_nCustCount = pPara->nCustCount[0];

	m_nTimeHour = pPara->nTime / 60;
	m_nTimeMinute = pPara->nTime % 60;

	m_nFullMark = pPara->nFullMark;
	m_nMarkType = pPara->nMarkType;

	m_nSChPoint = pPara->nSChPoints;
	m_nMChPoint = pPara->nMChPoints;
	m_nJdgPoint = pPara->nJdgPoints;
	m_nBlkPoint = pPara->nBlkPoints;
	m_nTxtPoint = pPara->nTxtPoints;

	int i;
	for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
	{
		m_sCustInfo[i].dwPara1 = pPara->nCustCount[i];
		m_sCustInfo[i].dwPara2 = pPara->nCustPoints[i];
	}
}

void CTestSetCommDlg::FillTestPara(CTestPara *pCPara)
{
	if (!pCPara)
	{
		return;
	}
	PTESTPARA pPara = pCPara->GetTestPara();

	pPara->pPaper = m_pCurSelPaper;
	pPara->bUseStore = m_bUseStore;
	pPara->bQNumOrder = m_bQNumOrder;

	pPara->nSChCount = m_nSChCount;
	pPara->nMChCount = m_nMChCount;
	pPara->nJdgCount = m_nJdgCount;
	pPara->nBlkCount = m_nBlkCount;
	pPara->nTxtCount = m_nTxtCount;
	pPara->nGrpCount = m_nGrpCount;

	pPara->nTime = m_nTimeHour * 60 + m_nTimeMinute;
	pPara->nTime = min(pPara->nTime, TIME_QUIZMAXMINUTE);

	pPara->nFullMark = m_nFullMark;
	pPara->nMarkType = m_nMarkType;

	pPara->nSChPoints = m_nSChPoint;
	pPara->nMChPoints = m_nMChPoint;
	pPara->nJdgPoints = m_nJdgPoint;
	pPara->nBlkPoints = m_nBlkPoint;
	pPara->nTxtPoints = m_nTxtPoint;

	int i;
	for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
	{
		pPara->nCustCount[i] = m_sCustInfo[i].dwPara1;
		pPara->nCustPoints[i] = m_sCustInfo[i].dwPara2;
	}
}

void CTestSetCommDlg::OnSetActive()
{
	ShowItemCount();
}

void CTestSetCommDlg::OnComboSelChange() 
{
	m_pCurSelPaper = (CPaper*)m_pPaperList->GetItem(m_PaperCombo.GetCurSel());
	ASSERT(m_pCurSelPaper);

	FillTypeList();
	ShowItemCount();
}

void CTestSetCommDlg::OnCheckUseStore() 
{
	UpdateData();

	ShowItemCount();
}

void CTestSetCommDlg::ShowItemCount()
{
	CString string;
	int i = 0;

	ZeroMemory(m_nItemCount, sizeof(m_nItemCount));

	// 单选
	if (m_pChsDlg && m_pChsDlg->m_bMChAsSCh)
	{
		if (m_bUseStore)
		{
			m_nItemCount[ITEMINDEX_SCHOISE] = m_pCurSelPaper->GetSaveList()->GetSingleChoiseCount();
		}
		else
		{
			m_nItemCount[ITEMINDEX_SCHOISE] = m_pCurSelPaper->GetSingleChoiseCount();
		}
	}
	else
	{
		if (m_bUseStore)
		{
			m_nItemCount[ITEMINDEX_SCHOISE] = m_pCurSelPaper->GetSaveList()->GetSChoiseCount();
		}
		else
		{
			m_nItemCount[ITEMINDEX_SCHOISE] = m_pCurSelPaper->GetItemCount(TYPE_SCHOISE);
		}
	}

	// 多选
	if (m_pChsDlg && m_pChsDlg->m_bMChNoSingle)
	{
		if (m_bUseStore)
		{
			m_nItemCount[ITEMINDEX_MCHOISE] = m_pCurSelPaper->GetSaveList()->GetMultiChoiseCount();
		}
		else
		{
			m_nItemCount[ITEMINDEX_MCHOISE] = m_pCurSelPaper->GetMultiChoiseCount();
		}
	}
	else if (m_pChsDlg && m_pChsDlg->m_bSChAsMCh)
	{
		if (m_bUseStore)
		{
			m_nItemCount[ITEMINDEX_MCHOISE] = m_pCurSelPaper->GetSaveList()->GetSChoiseCount() +
											  m_pCurSelPaper->GetSaveList()->GetMChoiseCount();
		}
		else
		{
			m_nItemCount[ITEMINDEX_MCHOISE] = m_pCurSelPaper->GetItemCount(TYPE_SCHOISE) +
											  m_pCurSelPaper->GetItemCount(TYPE_MCHOISE);
		}
	}
	else
	{
		if (m_bUseStore)
		{
			m_nItemCount[ITEMINDEX_MCHOISE] = m_pCurSelPaper->GetSaveList()->GetMChoiseCount();
		}
		else
		{
			m_nItemCount[ITEMINDEX_MCHOISE] = m_pCurSelPaper->GetItemCount(TYPE_MCHOISE);
		}
	}

	if (m_bUseStore)
	{
		// 判断
		m_nItemCount[ITEMINDEX_JUDGE] = m_pCurSelPaper->GetSaveList()->GetJudgeCount();

		// 填空
		m_nItemCount[ITEMINDEX_BLANK] = m_pCurSelPaper->GetSaveList()->GetBlankCount();

		// 简答
		m_nItemCount[ITEMINDEX_TEXT] = m_pCurSelPaper->GetSaveList()->GetTextCount();

		// 自定义
		int i, nType = TYPE_CUSTOMRIGHT;
		for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
		{
			m_sCustInfo[i].dwPara3 =
				m_pCurSelPaper->GetSaveList()->GetCustomCount(nType);
			nType += TYPE_CUSTOMRIGHT;
		}
	}
	else
	{
		// 判断
		m_nItemCount[ITEMINDEX_JUDGE] = m_pCurSelPaper->GetItemCount(TYPE_JUDGE);

		// 填空
		m_nItemCount[ITEMINDEX_BLANK] = m_pCurSelPaper->GetItemCount(TYPE_BLANK);

		// 简答
		m_nItemCount[ITEMINDEX_TEXT] = m_pCurSelPaper->GetItemCount(TYPE_TEXT);

		// 自定义
		int i, nType = TYPE_CUSTOMRIGHT;
		for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
		{
			m_sCustInfo[i].dwPara3 =
				m_pCurSelPaper->GetItemCount(nType);
			nType += TYPE_CUSTOMRIGHT;
		}
	}
	m_nItemCount[ITEMINDEX_GROUP] = m_pCurSelPaper->GetItemCount(TYPE_GROUP);

	for (i = ITEMINDEX_SCHOISE; i < TYPE_SINGLEDEFAULTCOUNT; i++)
	{
		string.Format(_T("(%d)"), m_nItemCount[i]);
		SetDlgItemText(IDC_STATIC_SCHCOUNT + i, string);
	}
	string.Format(_T("(%d)"), m_nItemCount[ITEMINDEX_GROUP]);
	SetDlgItemText(IDC_STATIC_GRPCOUNT, string);

	// 检查各题目的数目是否合适
	CheckInput();

	// 显示自定义题目数量
	ShowCustomCount();

	// 是否可以选错题
	if (!m_pCurSelPaper->GetSaveList()->GetItemCount())
	{
		m_bUseStore = FALSE;
		GetDlgItem(IDC_CHECK_USESTORE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_USESTORE)->EnableWindow(TRUE);
	}
}

void CTestSetCommDlg::ShowCustomCount()
{
	int nType = m_cComboType1.GetCurSel();
	
	if (nType >= 0)
	{
		CString string;
		string.Format(_T("(%d)"), m_sCustInfo[nType].dwPara3);
		SetDlgItemText(IDC_STATIC_CUSTCOUNT, string);

		SetDlgItemInt(IDC_EDIT_CUSTCOUNT, m_sCustInfo[nType].dwPara1);
		m_cEditCustCount.EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_STATIC_CUSTCOUNT, _T(""));
		SetDlgItemInt(IDC_EDIT_CUSTCOUNT, 0);
		m_cEditCustCount.EnableWindow(FALSE);
	}
}

BOOL CTestSetCommDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wCode = HIWORD(wParam);
	WORD wID = LOWORD(wParam);

	if (EN_KILLFOCUS == wCode)
	{
		return CheckInput(wID);
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CTestSetCommDlg::CheckInput(UINT nID /* = 0 */)
{
	int nNum, i, nCustSel;

	if (nID)
	{
		nNum = GetDlgItemInt(nID);
		
		switch(nID)
		{
		case IDC_EDIT_SCHCOUNT:
		case IDC_EDIT_MCHCOUNT:
		case IDC_EDIT_JDGCOUNT:
		case IDC_EDIT_BLKCOUNT:
		case IDC_EDIT_TXTCOUNT:
		case IDC_EDIT_GRPCOUNT:
			nNum = max(0, min(nNum, m_nItemCount[nID - IDC_EDIT_SCHCOUNT]));
			break;

		case IDC_EDIT_CUSTCOUNT:
			nCustSel = m_cComboType1.GetCurSel();
			if (nCustSel < 0)
			{
				nCustSel = 0;
			}
			i = m_sCustInfo[nCustSel].dwPara3;
			nNum = max(0, min(nNum, i));
			m_sCustInfo[nCustSel].dwPara1 = nNum;
			break;

		case IDC_EDIT_SCHPOINT:
		case IDC_EDIT_MCHPOINT:
		case IDC_EDIT_JDGPOINT:
		case IDC_EDIT_BLKPOINT:
		case IDC_EDIT_TXTPOINT:
			nNum = max(1, nNum);
			break;

		case IDC_EDIT_CUSTPOINT:
			nNum = max(1, nNum);
			nCustSel = m_cComboType2.GetCurSel();
			if (nCustSel < 0)
			{
				nCustSel = 0;
			}
			m_sCustInfo[nCustSel].dwPara2 = nNum;
			break;
			
		case IDC_EDIT_HOUR:
			nNum = max(0, min(nNum, TIME_QUIZMAXHOUR));
			break;
			
		case IDC_EDIT_MINUTE:
			nNum = max(0, min(nNum, TIME_QUIZMAXMINUTE));
			break;
			
		default:
			return !0;
		}
		
		SetDlgItemInt(nID, nNum);
	}
	else
	{
		for (i = IDC_EDIT_SCHCOUNT; i <= IDC_EDIT_GRPCOUNT; i++)
		{
			nNum = GetDlgItemInt(i);
			nNum = max(0, min(nNum, m_nItemCount[i - IDC_EDIT_SCHCOUNT]));
			SetDlgItemInt(i, nNum);
		}

		for (i = IDC_EDIT_SCHPOINT; i < IDC_EDIT_TXTPOINT; i++)
		{
			nNum = GetDlgItemInt(i);
			nNum = max(1, nNum);
			SetDlgItemInt(i, nNum);
		}

		nNum = GetDlgItemInt(IDC_EDIT_CUSTCOUNT);
		nCustSel = m_cComboType1.GetCurSel();
		if (nCustSel < 0)
		{
			nCustSel = 0;
		}
		i = m_sCustInfo[nCustSel].dwPara3;
		nNum = max(0, min(nNum, i));
		SetDlgItemInt(IDC_EDIT_CUSTCOUNT, nNum);
		m_sCustInfo[nCustSel].dwPara1 = nNum;

		for (i = 0; i < m_pCurSelPaper->GetCustomTypeCount(); i++)
		{
			if (i == m_cComboType2.GetCurSel())
			{
				nNum = GetDlgItemInt(IDC_EDIT_CUSTPOINT);
				nNum = max(1, nNum);
				SetDlgItemInt(IDC_EDIT_CUSTPOINT, nNum);
			}
			else
			{
				nNum = max(1, m_sCustInfo[i].dwPara2);
			}

			m_sCustInfo[i].dwPara2 = nNum;
		}

		nNum = GetDlgItemInt(IDC_EDIT_HOUR);
		nNum = max(0, min(nNum, TIME_QUIZMAXHOUR));
		SetDlgItemInt(IDC_EDIT_HOUR, nNum);

		nNum = GetDlgItemInt(IDC_EDIT_MINUTE);
		nNum = max(0, min(nNum, TIME_QUIZMAXMINUTE));
		SetDlgItemInt(IDC_EDIT_MINUTE, nNum);
	}

	return 0;
}

BOOL CTestSetCommDlg::CheckCanExit()
{
	CheckInput();
	UpdateData();

	CTMessageBox Box;

	// 检查时间是否太短
	int nLeastTime = m_nSChCount + m_nMChCount * 2 + m_nJdgCount + m_nBlkCount * 5 + m_nTxtCount * 10;
	int nSetTime = m_nTimeHour * 3600 + m_nTimeMinute * 60;
	if (nSetTime <= 0)
	{
		m_nTimeHour = m_nTimeMinute = 0;
	}
	else if (nSetTime < nLeastTime)
	{
		if (IDNO != Box.SuperMessageBox(m_hWnd, IDS_TESTSET_TIMETOOSHORT, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
			return FALSE;
		}
	}
	else if (nSetTime > TIME_QUIZMAXMINUTE * 60)
	{
		if (IDNO != Box.SuperMessageBox(m_hWnd, IDS_TESTSET_TIMETOOLONG, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
			return FALSE;
		}
	}

	int nSChCount, nMChCount;
	if (m_bUseStore)
	{
		nSChCount = m_pCurSelPaper->GetSaveList()->GetSChoiseCount();
		nMChCount = m_pCurSelPaper->GetSaveList()->GetMChoiseCount();
	}
	else
	{
		nSChCount = m_pCurSelPaper->GetItemCount(TYPE_SCHOISE);
		nMChCount = m_pCurSelPaper->GetItemCount(TYPE_MCHOISE);
	}
	
	if (m_nSChCount <= nSChCount &&
		m_nMChCount <= nMChCount)
	{
		return TRUE;
	}
	
	// 如果多选题作为了单选题，或单选题作为了多选题
	// 为了防止重复题目，则可能会减少多选题的数目
	if (m_pChsDlg->m_bMChAsSCh || m_pChsDlg->m_bSChAsMCh)
	{
		if (m_nSChCount - nSChCount + m_nMChCount > nMChCount)
		{
			if (IDNO != Box.SuperMessageBox(m_hWnd, IDS_TESTSET_CONFLICT, IDS_MSGCAPTION_ASK,
				MB_YESNO | MB_ICONQUESTION, 0, 0))
			{
				return FALSE;
			}
			m_nMChCount = nSChCount + nMChCount - m_nSChCount;
			ASSERT(m_nMChCount >= 0);
		}
	}
	
	return TRUE;
}

void CTestSetCommDlg::OnRadioEqu() 
{
	GetDlgItem(IDC_EDIT_FULLMARK)->EnableWindow(TRUE);

	int i;
	for (i = IDC_EDIT_SCHPOINT; i <= IDC_EDIT_TXTPOINT; i++)
	{
		GetDlgItem(i)->EnableWindow(FALSE);
	}

	GetDlgItem(IDC_EDIT_CUSTPOINT)->EnableWindow(FALSE);
	m_cComboType2.EnableWindow(FALSE);
}

void CTestSetCommDlg::OnRadioDif() 
{
	GetDlgItem(IDC_EDIT_FULLMARK)->EnableWindow(FALSE);
	
	int i;
	for (i = IDC_EDIT_SCHPOINT; i <= IDC_EDIT_TXTPOINT; i++)
	{
		GetDlgItem(i)->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_EDIT_CUSTPOINT)->EnableWindow(TRUE);
	m_cComboType2.EnableWindow(TRUE);
}

void CTestSetCommDlg::OnSelChangeCountCombo() 
{
	ShowCustomCount();

	CheckInput(IDC_EDIT_CUSTCOUNT);
}

void CTestSetCommDlg::OnSelChangePointCombo() 
{
	int nType = m_cComboType2.GetCurSel();
	int nPoint = m_sCustInfo[nType].dwPara2;
	if (!m_sCustInfo[nType].dwPara1)
	{
		nPoint = 0;
		GetDlgItem(IDC_EDIT_CUSTPOINT)->EnableWindow(FALSE);
	}
	else
	{
		nPoint = max(1, nPoint);
		GetDlgItem(IDC_EDIT_CUSTPOINT)->EnableWindow(TRUE);
	}

	SetDlgItemInt(IDC_EDIT_CUSTPOINT, nPoint);
}
