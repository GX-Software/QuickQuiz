// BlkTxtCheckDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BlkTxtCheckDlg.h"

#include "Blank.h"
#include "Text.h"
#include "TestMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlkTxtCheckDlg dialog


CBlkTxtCheckDlg::CBlkTxtCheckDlg(CList *pList, CTestPara *pPara, CWnd* pParent /*=NULL*/) :
	CClrDialog(CBlkTxtCheckDlg::IDD, pParent),
	m_pPara(pPara), m_pList(pList), m_nCurShow(0),
	m_pUserSet(NULL)
{
	//{{AFX_DATA_INIT(CBlkTxtCheckDlg)
	//}}AFX_DATA_INIT
}

CBlkTxtCheckDlg::~CBlkTxtCheckDlg()
{
	if (m_pUserSet)
	{
		free(m_pUserSet);
	}
}

void CBlkTxtCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlkTxtCheckDlg)
	DDX_Control(pDX, IDC_COMBO_FONTSIZE, m_cComboFont);
	DDX_Control(pDX, IDCANCEL, m_cBtnCancel);
	DDX_Control(pDX, IDC_STATIC_GROUP2, m_cGroup2);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDC_EDIT_TXTSCORE, m_cEditTxtScore);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_CUSTOM_USER, m_cUser);
	DDX_Control(pDX, IDC_CUSTOM_REAL, m_cReal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlkTxtCheckDlg, CClrDialog)
	//{{AFX_MSG_MAP(CBlkTxtCheckDlg)
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_EN_KILLFOCUS(IDC_EDIT_TXTSCORE, OnKillFocus)
	ON_CBN_SELCHANGE(IDC_COMBO_FONTSIZE, OnSelChangeFontSize)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ITEMSHOWWNDNOTIFY, OnItemShowWndNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlkTxtCheckDlg message handlers

int CBlkTxtCheckDlg::DoModal() 
{
	m_cUser.RegisterClass();
	m_cReal.RegisterClass();
	
	return CClrDialog::DoModal();
}

void CBlkTxtCheckDlg::OnCancel() 
{
	SaveCurrent();

	int i;
	for (i = 0; i < m_TmpList.GetItemCount(); i++)
	{
		if (!m_pUserSet[i])
		{
			break;
		}
	}

	if (i < m_TmpList.GetItemCount())
	{
		CTMessageBox Box;
		if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_CHECKDLG_MISS, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
			return;
		}
	}
	
	CClrDialog::OnCancel();
}

BOOL CBlkTxtCheckDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();

	ModifyStyleEx(0, WS_EX_APPWINDOW);
	
	InitTmpList();

	if (m_pUserSet)
	{
		free(m_pUserSet);
	}
	m_pUserSet = (BOOL*)malloc(m_TmpList.GetItemCount() * sizeof(BOOL));
	ZeroMemory(m_pUserSet, m_TmpList.GetItemCount() * sizeof(BOOL));

	m_cUser.SetShowType(SHOWWNDTYPE_SUBJECTIVEVIEW);
	m_cUser.SetList(&m_TmpList);
	m_cReal.SetShowType(SHOWWNDTYPE_VIEW);
	m_cReal.SetList(&m_TmpList);
	m_cComboFont.SetCurSel(g_nShowFont);

	m_nCurShow = 0;
	SetShow();
	
	return FALSE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBlkTxtCheckDlg::InitTmpList()
{
	ASSERT(m_pList);

	m_TmpList.ClearAllItems(TRUE);

	CTempList t;
	PTESTPARA pPara = m_pPara->GetTestPara();
	int i;

	CTestMaker::MakeAtomList(m_pList, &t);

	for (i = 0; i < t.GetItemCount(); ++i)
	{
		CItem *pItem = t.GetItem(i);
		switch(pItem->GetType())
		{
		case TYPE_BLANK:
			if (!(pPara->dwBlkOptions & BLKOPTION_ALLRIGHT))
			{
				CBlank *pBlank = (CBlank*)pItem;
				m_TmpList.AddItem(pItem);
				pBlank->AutoScore(m_pPara);
			}
			break;
			
		case TYPE_TEXT:
			if (!(pPara->dwTxtOptions & TXTOPTION_ALLRIGHT))
			{
				CText *pText = (CText*)pItem;
				m_TmpList.AddItem(pText);
				pText->AutoScore(m_pPara);
			}
			break;
			
		default:
			break;
		}
	}

	ASSERT(m_TmpList.GetItemCount());
}

void CBlkTxtCheckDlg::SetShow()
{
	CString string;
	_stdstring stdstring;
	HDC hDC = ::GetDC(GetSafeHwnd());
	
	// 当前题目已经经过检查
	m_pUserSet[m_nCurShow] = TRUE;

	CItem *pItem = m_TmpList.GetItem(m_nCurShow);
	
	GetDlgItem(IDC_BUTTON_PRE)->EnableWindow(m_nCurShow);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(m_nCurShow < m_TmpList.GetItemCount() - 1);
	
	// 显示题目类型
	SetDlgItemText(IDC_STATIC_TYPE, pItem->GetTypeDesc());
	
	// 显示题目内容
	m_cUser.ShowItem(m_nCurShow, SHOWSTYLE_WITHUSERANSWER);
	m_cReal.ShowItem(m_nCurShow, SHOWSTYLE_WITHANSWER);
	
	// 相似度
	float fSimilarity = GetSimilarity(pItem);
	string.Format(_T("%3.2f%%"), fSimilarity * 100);
	SetDlgItemText(IDC_STATIC_SIMILAR, string);
	
	PTESTPARA pPara = m_pPara->GetTestPara();
	CEdit *pTxt = (CEdit*)GetDlgItem(IDC_EDIT_TXTSCORE);
	if (pItem->GetType() == TYPE_TEXT)
	{
		CText *pText = (CText*)pItem;
		SetDlgItemInt(IDC_EDIT_TXTSCORE, pText->GetPoint());
		SetDlgItemText(IDC_STATIC_BLKSCORE, _T(""));
		pTxt->EnableWindow(TRUE);
		pTxt->SetFocus();
		pTxt->SetSel(0, -1);

		string.Format(_T("(0-%d)"), (MARKTYPE_AVERAGE == pPara->nMarkType) ?
			100 : pPara->nTxtPoints);
	}
	else
	{
		CBlank *pBlank = (CBlank*)pItem;
		int nCount;

		pTxt->SetWindowText(_T(""));
		pTxt->EnableWindow(FALSE);

		pBlank->CheckAnswer(&nCount);
		SetDlgItemInt(IDC_STATIC_BLKSCORE, nCount);

		string.Empty();
	}

	SetDlgItemText(IDC_STATIC_TXTSCORE, string);
}

void CBlkTxtCheckDlg::SaveCurrent()
{
	CItem *pItem = m_TmpList.GetItem(m_nCurShow);

	if (pItem->GetType() == TYPE_TEXT)
	{
		int i = GetDlgItemInt(IDC_EDIT_TXTSCORE);
		((CText*)pItem)->SetPoint(i);
	}
}

float CBlkTxtCheckDlg::GetSimilarity(CItem *pItem)
{
	switch(pItem->GetType())
	{
	case TYPE_BLANK:
		((CBlank*)pItem)->CheckSimilarity(&m_cSimilarity);
		break;

	case TYPE_TEXT:
		((CText*)pItem)->CheckSimilarity(&m_cSimilarity);
		break;
	}

	return m_cSimilarity.GetInfo();
}

void CBlkTxtCheckDlg::OnButtonPre() 
{
	SaveCurrent();

	m_nCurShow--;
	SetShow();
}

void CBlkTxtCheckDlg::OnButtonNext() 
{
	SaveCurrent();

	m_nCurShow++;
	SetShow();
}

int CBlkTxtCheckDlg::GetMaxPoint()
{
	PTESTPARA pPara = m_pPara->GetTestPara();
	CItem *pItem = m_TmpList.GetItem(m_nCurShow);

	if(pItem->GetType() == TYPE_BLANK)
	{
		return ((CBlank*)pItem)->GetBlankCount();
	}
	else
	{
		return ((MARKTYPE_AVERAGE == pPara->nMarkType) ? 100 : pPara->nTxtPoints);
	}
}

void CBlkTxtCheckDlg::OnKillFocus() 
{
	int nNum = GetDlgItemInt(IDC_EDIT_TXTSCORE);
	int nMax = GetMaxPoint();
	
	nNum = min(nMax, max(nNum, 0));
	SetDlgItemInt(IDC_EDIT_TXTSCORE, nNum);
}

LRESULT CBlkTxtCheckDlg::OnItemShowWndNotify(WPARAM wParam, LPARAM lParam)
{
	int nCount;

	switch(wParam)
	{
	case ISN_BLANKCHECK:
		((CBlank*)lParam)->CheckAnswer(&nCount);
		SetDlgItemInt(IDC_STATIC_BLKSCORE, nCount);
		break;
	}

	return 0;
}

void CBlkTxtCheckDlg::OnSelChangeFontSize() 
{
	long lFontSize;
	
	g_nShowFont = m_cComboFont.GetCurSel();
	switch(g_nShowFont)
	{
	case SW_SMALLFONT:
		lFontSize = ISP_SMALLFONT;
		break;
		
	case SW_NORMALFONT:
	default:
		lFontSize = ISP_NORMALFONT;
		break;
		
	case SW_LARGEFONT:
		lFontSize = ISP_LARGEFONT;
		break;
		
	case SW_GIANTFONT:
		lFontSize = ISP_GIANTFONT;
		break;
	}
	m_cUser.SetFontSize(lFontSize, TRUE);
	m_cReal.SetFontSize(lFontSize, TRUE);
	m_cUser.SetFocus();
}
