// ViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "ViewDlg.h"

#include "Blank.h"
#include "Text.h"

#include "ListFrame.h"
#include "ShowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewDlg dialog


CViewDlg::CViewDlg(CPaper *pPaper, CWnd* pParent /*=NULL*/) :
	CClrDialog(CViewDlg::IDD, pParent),
	m_pPaper(pPaper), m_nCurShow(0),
	m_nCurStep(0), m_nMaxStep(0),
	m_pListWnd(NULL)
{
	//{{AFX_DATA_INIT(CViewDlg)
	//}}AFX_DATA_INIT
}

CViewDlg::~CViewDlg()
{
}

void CViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewDlg)
	DDX_Control(pDX, IDC_COMBO_FONTSIZE, m_cComboFont);
	DDX_Control(pDX, IDC_CHECK_HOLESHOW, m_cCheckAll);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDCANCEL, m_cBtnCancel);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_cBtnSave);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_OPTION, m_cBtnOption);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_CUSTOM_ITEM, m_cShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewDlg, CClrDialog)
	//{{AFX_MSG_MAP(CViewDlg)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnButtonOption)
	ON_CBN_SELCHANGE(IDC_COMBO_FONTSIZE, OnSelChangeFontSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewDlg message handlers

int CViewDlg::DoModal() 
{
	m_cShow.RegisterClass(AfxGetInstanceHandle());

	int nRet = CClrDialog::DoModal();

	if (m_pListWnd && IsWindow(m_pListWnd->GetSafeHwnd()))
	{
		m_pListWnd->DestroyWindow();
		m_pListWnd = NULL;
	}

	return nRet;
}

BOOL CViewDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();

	ModifyStyleEx(0, WS_EX_APPWINDOW);
	
	// 搞一个随机队列
	srand(time(NULL) + _getpid());

	ZeroMemory(&m_Para, sizeof(m_Para));
	m_Para.dwTypeMask = 0xFFFFFFFFL;

	// 填充题目列表
	m_pPaper->FillTempList(&m_Para, &m_List);

	CRect rDlg;
	GetWindowRect(&rDlg);
	AfxCalcDialogPos(&rDlg, FRAMEPOSTYPE_ALIGN);
	SetWindowPos(NULL, rDlg.left, rDlg.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// 创建题目列表
	if (m_pListWnd && IsWindow(m_pListWnd->GetSafeHwnd()))
	{
		m_pListWnd->DestroyWindow();
		m_pListWnd = NULL;
	}
	m_pListWnd = new CListFrame(this);
	if (m_pListWnd)
	{
		GetWindowRect(rDlg);
		AfxCalcFrameWndPos(&rDlg, FRAMEPOSTYPE_ALIGN);
		m_pListWnd->CreateFrame(NULL, rDlg, AfxGetMainWnd(), 0);
		
		if (IsWindow(m_pListWnd->GetSafeHwnd()))
		{
			m_pListWnd->SetList(&m_List, LISTFRM_TYPE_VIEW, 0);
			m_pListWnd->ShowWindow(SW_SHOW);
		}
	}

	m_nCurShow = 0;
	m_nCurStep = 0;
	m_nMaxStep = GetMaxStep();

	m_cShow.SetShowType(SHOWWNDTYPE_VIEW);
	m_cComboFont.SetCurSel(g_nShowFont);
	OnSelChangeFontSize();

	m_cShow.SetList(&m_List);
	SetShow();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CViewDlg::SetCurSel(int nIndex)
{
	if (nIndex >= m_List.GetItemCount())
	{
		nIndex = 0;
	}
	
	m_nCurShow = nIndex;
	m_nCurStep = 0;
	m_nMaxStep = GetMaxStep();

	SetShow();
}

int CViewDlg::GetMaxStep()
{
	CItem *pItem = m_List.GetItem(m_nCurShow);

	switch(pItem->GetType())
	{
	case TYPE_SCHOISE:
	case TYPE_MCHOISE:
	case TYPE_JUDGE:
		return 1;
		break;

	case TYPE_BLANK:
		return ((CBlank*)pItem)->GetBlankCount();
		break;

	case TYPE_TEXT:
		return ((CText*)pItem)->GetAnswerParagraphs();
		break;

	default:
		ASSERT(FALSE);
	}

	return 0;
}

void CViewDlg::FindNext()
{
	++m_nCurShow;
	if (m_nCurShow >= m_List.GetItemCount())
	{
		if (m_Para.bRandom)
		{
			m_List.Random(0, 0, time(NULL) + _getpid());
		}
		m_nCurShow = 0;
	}
}

void CViewDlg::SetShow()
{
	CItem *pParent;
	CString string;
	_stdstring stdstring;
	TPARAS Para = {0};
	CItem *pItem = m_List.GetItem(m_nCurShow);

	// 显示哪个题目
	Para.dwPara1 = m_nCurShow;

	// 是否为题目组
	pParent = pItem->GetParent();
	ASSERT(pParent);
	if (pParent->GetType() == TYPE_GROUP)
	{
		Para.dwPara3 = (DWORD)pParent;
	}

	// 显示答案
	if (!m_nCurStep)
	{
		m_cShow.ShowItem(m_nCurShow, 0, m_nCurStep);
	}
	else
	{
		DWORD dwStyle = SHOWSTYLE_WITHANSWER;
		if (m_nCurStep == m_nMaxStep)
		{
			dwStyle |= SHOWSTYLE_WITHRESOLVE;
		}
		m_cShow.ShowItem(m_nCurShow, dwStyle, m_nCurStep);
	}

	GetDlgItem(IDC_BUTTON_PRE)->EnableWindow(pItem != m_List.GetItem(0));

	// 显示题目类型
	SetDlgItemText(IDC_STATIC_TYPE, pItem->GetTypeDesc());

	if (pItem->IsStored())
	{
		string.LoadString(IDS_QVIEW_BUTTONUNSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}
	else
	{
		string.LoadString(IDS_VIEWDLG_BUTTONSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}

	ASSERT(IsWindow(m_pListWnd->GetSafeHwnd()));
	m_pListWnd->SetCurSel(m_nCurShow);
}

void CViewDlg::OnButtonNext() 
{
	if (m_cCheckAll.GetCheck())
	{
		if (m_nCurStep == m_nMaxStep)
		{
			FindNext();
			m_nCurStep = 0;
			m_nMaxStep = GetMaxStep();
		}
		else
		{
			m_nCurStep = m_nMaxStep;
		}
	}
	else
	{
		m_nCurStep++;
		if (m_nCurStep > 1)
		{
			CItem *pItem = m_List.GetItem(m_nCurShow);
			// 填空题需要一个空一个空显示
			if (pItem->GetType() == TYPE_BLANK)
			{
				CBlank *pBlank = (CBlank*)pItem;
				if (m_nCurStep <= pBlank->GetBlankCount())
				{
					SetShow();
					return;
				}
			}
			// 简答题需要一段一段显示
			else if (pItem->GetType() == TYPE_TEXT)
			{
				CText *pText = (CText*)pItem;
				if (m_nCurStep <= pText->GetAnswerParagraphs())
				{
					SetShow();
					return;
				}
			}
			
			FindNext();
			m_nCurStep = 0;
			m_nMaxStep = GetMaxStep();
		}
	}

	SetShow();
}

void CViewDlg::OnButtonPre() 
{
	ASSERT(m_nCurShow);

	--m_nCurShow;
	m_nCurStep = 0;
	m_nMaxStep = GetMaxStep();
	SetShow();
}

void CViewDlg::OnButtonSave() 
{
	CString string;
	CItem *pItem = m_List.GetItem(m_nCurShow);

	if (pItem->IsStored())
	{
		pItem->SetStore(FALSE);

		string.LoadString(IDS_VIEWDLG_BUTTONSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}
	else
	{
		pItem->SetStore(TRUE);

		string.LoadString(IDS_QVIEW_BUTTONUNSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}

	m_pListWnd->RefreshList();
}

void CViewDlg::OnButtonOption() 
{
	TYPEPARA Save;
	CItem *pItemStore = m_List.GetItem(m_nCurShow);
	memcpy(&Save, &m_Para, sizeof(TYPEPARA));

	CChooseTypeDlg Dlg(&m_Para, m_pPaper);
	if (IDOK != Dlg.DoModal())
	{
		return;
	}

	// 若勾选了随机看题，则重新随机
	// 否则，若选项未变化，直接返回
	if (!m_Para.bRandom && !memcmp(&Save, &m_Para, sizeof(TYPEPARA)))
	{
		return;
	}

	if (!m_Para.dwTypeMask)
	{
		CTMessageBox Box;
		Box.SuperMessageBox(m_hWnd, IDS_VIEWDLG_FINDFAIL, IDS_MSGCAPTION_INFOR,
			MB_OK | MB_ICONINFORMATION, 0, 0);
		memcpy(&m_Para, &Save, sizeof(TYPEPARA));

		return;
	}

	// 如果有了修改
	m_pPaper->FillTempList(&m_Para, &m_List);

	// 如果没有找到，GetIndex返回-1，可以从开头开始
	// 如果找到了，就从当前显示的下一个开始
	m_nCurShow = m_List.GetIndex(pItemStore);
	if (m_nCurShow < 0)
	{
		m_nCurShow = 0;
	}
	m_pListWnd->UpdateList(m_nCurShow);
	
	SetShow();
}

void CViewDlg::OnSelChangeFontSize() 
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
	m_cShow.SetFontSize(lFontSize, TRUE);
	m_cShow.SetFocus();
}
