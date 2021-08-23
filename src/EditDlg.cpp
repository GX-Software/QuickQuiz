// AddQuestionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "EditDlg.h"

#include "EditFrame.h"
#include "ListFrame.h"
#include "ImageFrame.h"
#include "AllBuildDlg.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog

CEditDlg::CEditDlg(int nInitShow /* = 0 */, CWnd* pParent /* = NULL */) :
	CClrDialog(CEditDlg::IDD, pParent),
	m_pList(NULL), m_nCurShow(-1), m_nLastShow(-1),
	m_nInitShow(nInitShow), m_pAddList(NULL),
	m_pSChoiseDlg(NULL), m_pMChoiseDlg(NULL), m_pJudgeDlg(NULL), m_pBlankDlg(NULL), m_pTextDlg(NULL), m_nType(TYPE_DEFAULT),
	m_bIsNew(TRUE), m_bEdited(FALSE), m_bModify(FALSE),
	m_pEditWnd(NULL), m_pListWnd(NULL),
	m_pImageWnd(NULL), m_DescImageList(IMAGE_MAXCOUNT), m_ResvImageList(IMAGE_MAXCOUNT)
{
	//{{AFX_DATA_INIT(CEditDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEditDlg::~CEditDlg()
{
	if (m_pList && m_bIsNew)
	{
		delete m_pList;
	}
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDlg)
	DDX_Control(pDX, IDC_BUTTON_SETRESIMAGE, m_cBtnResImage);
	DDX_Control(pDX, IDC_BUTTON_SETDESCIMAGE, m_cBtnDescImage);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_CHECK_TOPMOST, m_cCheckTopMost);
	DDX_Control(pDX, IDC_CHECK_QLIST, m_cCheckQList);
	DDX_Control(pDX, IDC_BUTTON_TEXT, m_cBtnText);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_cBtnNew);
	DDX_Control(pDX, IDC_BUTTON_DESC, m_cBtnDesc);
	DDX_Control(pDX, IDC_BUTTON_RESOLVE, m_cBtnResolve);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_cBtnDel);
	DDX_Control(pDX, IDC_BUTTON_ALLBUILD, m_cBtnAllBuild);
	DDX_Control(pDX, IDC_EDIT_DESC, m_cEditDesc);
	DDX_Control(pDX, IDC_EDIT_RESOLVE, m_cEditResolve);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDC_STATIC_GROUP2, m_cGroup2);
	DDX_Control(pDX, IDC_TAB_TYPE, m_cTab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDlg, CClrDialog)
	//{{AFX_MSG_MAP(CEditDlg)
	ON_EN_CHANGE(IDC_EDIT_DESC, OnChange)
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_ALLBUILD, OnButtonAllBuild)
	ON_BN_CLICKED(IDC_BUTTON_TEXT, OnButtonText)
	ON_BN_CLICKED(IDC_BUTTON_DESC, OnButtonDesc)
	ON_BN_CLICKED(IDC_BUTTON_RESOLVE, OnButtonResolve)
	ON_BN_CLICKED(IDC_CHECK_TOPMOST, OnCheckTopMost)
	ON_BN_CLICKED(IDC_CHECK_QLIST, OnCheckQList)
	ON_BN_CLICKED(IDC_BUTTON_SETDESCIMAGE, OnSetDescImage)
	ON_EN_CHANGE(IDC_EDIT_RESOLVE, OnChange)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_TYPE, OnChange)
	ON_BN_CLICKED(IDC_BUTTON_SETRESIMAGE, OnSetResImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDlg message handlers

void CEditDlg::OnOK() 
{
	if (!SaveCurrent())
	{
		return;
	}

	int nIndex = m_pList->GetFirstInvalidItem();
	if (nIndex >= 0)
	{
		CTMessageBox Box;
		if (IDYES == Box.SuperMessageBox(m_hWnd, IDS_EDITDLG_NOANSWER, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
			m_nCurShow = nIndex;
			SetShow();
			return;
		}

		// 如果是编辑，则不修改没有答案的题目
		m_pList->ClearInvalidItems();
	}

	CDialog::OnOK();
}

void CEditDlg::OnCancel() 
{
	if (CheckModify() || m_bEdited)
	{
		CTMessageBox Box;
		if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_EDITDLG_NOSAVE, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
		{
			return;
		}
	}
	
	CDialog::OnCancel();
}

void CEditDlg::SetEditPara(CList *pList, CList *pAddList, int nType)
{
	if (m_pList && m_bIsNew)
	{
		delete m_pList;
	}

	m_pList = pList;
	m_bIsNew = pList ? FALSE : TRUE;

	ASSERT(pAddList);
	m_pAddList = pAddList;

	m_bEdited = FALSE;
	m_nType = nType;

	if (IsWindow(m_pListWnd->GetSafeHwnd()))
	{
		m_pListWnd->SetList(m_pList, LISTFRM_TYPE_EDIT, 0);
	}
}

int CEditDlg::DoModal() 
{
	// 新建题目的时候
	if (!m_pList)
	{
		m_pList = new CList;
		if (!m_pList)
		{
			return IDCANCEL;
		}
	}

	int nType = CItem::GetType(m_nType);
	if (CItem::IsDefaultType(m_nType) || nType == TYPE_SCHOISE)
	{
		// 单选题
		if (m_pSChoiseDlg)
		{
			delete m_pSChoiseDlg;
			m_pSChoiseDlg = NULL;
		}
		m_pSChoiseDlg = new CChildSChoiseDlg;
		if (!m_pSChoiseDlg)
		{
			return IDCANCEL;
		}
	}
	else if (m_pSChoiseDlg)
	{
		delete m_pSChoiseDlg;
		m_pSChoiseDlg = NULL;
	}

	if (CItem::IsDefaultType(m_nType) || nType == TYPE_MCHOISE)
	{
		// 多选题
		if (m_pMChoiseDlg)
		{
			delete m_pMChoiseDlg;
			m_pMChoiseDlg = NULL;
		}
		m_pMChoiseDlg = new CChildMChoiseDlg;
		if (!m_pMChoiseDlg)
		{
			return IDCANCEL;
		}
	}
	else if (m_pMChoiseDlg)
	{
		delete m_pMChoiseDlg;
		m_pMChoiseDlg = NULL;
	}

	if (CItem::IsDefaultType(m_nType) || nType == TYPE_JUDGE)
	{
		// 判断题
		if (m_pJudgeDlg)
		{
			delete m_pJudgeDlg;
			m_pJudgeDlg = NULL;
		}
		m_pJudgeDlg = new CChildJudgeDlg;
		if (!m_pJudgeDlg)
		{
			return IDCANCEL;
		}
	}
	else if (m_pJudgeDlg)
	{
		delete m_pJudgeDlg;
		m_pJudgeDlg = NULL;
	}

	if (CItem::IsDefaultType(m_nType) || nType == TYPE_BLANK)
	{
		// 填空题
		if (m_pBlankDlg)
		{
			delete m_pBlankDlg;
			m_pBlankDlg = NULL;
		}
		m_pBlankDlg = new CChildBlankDlg;
		if (!m_pBlankDlg)
		{
			return IDCANCEL;
		}
	}
	else if (m_pBlankDlg)
	{
		delete m_pBlankDlg;
		m_pBlankDlg = NULL;
	}

	if (CItem::IsDefaultType(m_nType) || nType == TYPE_TEXT)
	{
		// 简答题
		if (m_pTextDlg)
		{
			delete m_pTextDlg;
			m_pTextDlg = NULL;
		}
		m_pTextDlg = new CChildTextDlg;
		if (!m_pTextDlg)
		{
			return IDCANCEL;
		}
	}
	else if (m_pTextDlg)
	{
		delete m_pTextDlg;
		m_pTextDlg = NULL;
	}

	m_pSChoiseDlg->SetMChoisePtr(m_pMChoiseDlg);
	m_pMChoiseDlg->SetSChoisePtr(m_pSChoiseDlg);

	if (!CItem::IsDefaultType(m_nType))
	{
		m_pList->SetDescription(m_pAddList->GetDescription());
	}

	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->DestroyWindow();
	}
	m_pEditWnd = new CEditFrame(EDITFRAMETYPE_NEWOREDIT);
	if (!m_pEditWnd)
	{
		return IDCANCEL;
	}

	if (m_pListWnd && IsWindow(m_pListWnd->GetSafeHwnd()))
	{
		m_pListWnd->DestroyWindow();
	}
	m_pListWnd = new CListFrame(this);
	if (!m_pListWnd)
	{
		return IDCANCEL;
	}

	if (m_pImageWnd && IsWindow(m_pImageWnd->GetSafeHwnd()))
	{
		m_pImageWnd->DestroyWindow();
	}
	m_pImageWnd = new CImageFrame();
	if (!m_pImageWnd)
	{
		return IDCANCEL;
	}
	
	int nRet = CClrDialog::DoModal();

	if (m_pSChoiseDlg)
	{
		delete m_pSChoiseDlg;
		m_pSChoiseDlg = NULL;
	}
	if (m_pMChoiseDlg)
	{
		delete m_pMChoiseDlg;
		m_pMChoiseDlg = NULL;
	}
	if (m_pJudgeDlg)
	{
		delete m_pJudgeDlg;
		m_pJudgeDlg = NULL;
	}
	if (m_pBlankDlg)
	{
		delete m_pBlankDlg;
		m_pBlankDlg = NULL;
	}
	if (m_pTextDlg)
	{
		delete m_pTextDlg;
		m_pTextDlg = NULL;
	}

	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->CheckSaveable(FALSE);
		m_pEditWnd->DestroyWindow();
		m_pEditWnd = NULL;
	}
	if (m_pListWnd && IsWindow(m_pListWnd->GetSafeHwnd()))
	{
		m_pListWnd->DestroyWindow();
		m_pListWnd = NULL;
	}
	if (m_pImageWnd && IsWindow(m_pImageWnd->GetSafeHwnd()))
	{
		m_pImageWnd->DestroyWindow();
		m_pImageWnd = NULL;
	}

	return nRet;
}

BOOL CEditDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();

	ModifyStyleEx(0, WS_EX_APPWINDOW);
	m_cEditDesc.SetLimitText(0);
	
	CString string;
	if (CItem::IsDefaultType(m_nType))
	{
		string.LoadString(IDS_ITEMTYPE_SCH);
		m_cTab.AddPage(IDD_CHILD_SCHOISE_DLG, m_pSChoiseDlg, string);
		
		string.LoadString(IDS_ITEMTYPE_MCH);
		m_cTab.AddPage(IDD_CHILD_MCHOISE_DLG, m_pMChoiseDlg, string);
		
		string.LoadString(IDS_ITEMTYPE_JDG);
		m_cTab.AddPage(IDD_CHILD_JUDGE_DLG, m_pJudgeDlg, string);
		
		string.LoadString(IDS_ITEMTYPE_BLK);
		m_cTab.AddPage(IDD_CHILD_BLANK_DLG, m_pBlankDlg, string);
		m_pBlankDlg->SetDescEdit(&m_cEditDesc, &m_cBtnDesc);

		string.LoadString(IDS_ITEMTYPE_TXT);
		m_cTab.AddPage(IDD_CHILD_TEXT_DLG, m_pTextDlg, string);
	}
	else
	{
		ASSERT(!m_pList->IsPaper());

		int nType = CItem::GetType(m_nType);
		UINT uTemplate = nType - TYPE_SCHOISE + IDD_CHILD_SCHOISE_DLG;

		CTabPage *pPage = NULL;
		switch(nType)
		{
		case TYPE_SCHOISE:
			pPage = m_pSChoiseDlg;
			break;

		case TYPE_MCHOISE:
			pPage = m_pMChoiseDlg;
			break;

		case TYPE_JUDGE:
			pPage = m_pJudgeDlg;
			break;

		case TYPE_BLANK:
			pPage = m_pBlankDlg;
			m_pBlankDlg->SetDescEdit(&m_cEditDesc, &m_cBtnDesc);
			break;

		case TYPE_TEXT:
			pPage = m_pTextDlg;
			break;

		default:
			ASSERT(FALSE);
		}
		m_cTab.AddPage(uTemplate, pPage, m_pList->GetDescription());
	}

	// 类型可以任意选择的情况
	m_cTab.SetChangeable(TYPE_DEFAULT == m_nType || TYPE_GROUP == m_nType);

	// 重新定位
	CRect rDlg;
	GetWindowRect(rDlg);
	AfxCalcDialogPos(&rDlg, FRAMEPOSTYPE_ALIGN);
	SetWindowPos(NULL, rDlg.left, rDlg.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	
	GetWindowRect(rDlg);
	AfxCalcFrameWndPos(&rDlg, FRAMEPOSTYPE_ALIGN);

	m_pListWnd->CreateFrame(NULL, rDlg, AfxGetMainWnd(), 0);
	ASSERT(IsWindow(m_pListWnd->GetSafeHwnd()));
	m_pListWnd->SetList(m_pList, LISTFRM_TYPE_EDIT, 0);
	m_pListWnd->SetCurSel(m_nInitShow);
	m_pListWnd->ShowWindow(SW_SHOW);
	((CButton*)GetDlgItem(IDC_CHECK_QLIST))->SetCheck(TRUE);

	string.LoadString(IDS_IMGFRM_ITEMIMAGE);
	m_pImageWnd->CreateFrame(string, AfxGetMainWnd(), rDlg, 0);
	ASSERT(IsWindow(m_pImageWnd->GetSafeHwnd()));

	rDlg.OffsetRect(0, 100);
	m_pEditWnd->CreateFrame(NULL, rDlg, AfxGetMainWnd(), 0);
	ASSERT(m_pEditWnd->GetSafeHwnd());
	m_pEditWnd->ShowWindow(SW_HIDE);
	
	// 是新建或编辑
	if(m_bIsNew)
	{
		m_nCurShow = -1;
		string.LoadString(IDS_EDITDLG_TITLENEW);
	}
	else
	{
		m_nCurShow = m_nInitShow;
		string.LoadString(IDS_EDITDLG_TITLEEDIT);
	}
	SetWindowText(string);

	m_bEdited = FALSE;
	m_bModify = FALSE;

	SetShow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditDlg::SetShow()
{
	// 当新建题目时，也应允许向前滚动
	GetDlgItem(IDC_BUTTON_PRE)->EnableWindow(m_pList->GetItemCount() && m_nCurShow != 0);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(m_nCurShow >= 0 && m_nCurShow < m_pList->GetItemCount() - 1);
	GetDlgItem(IDC_BUTTON_NEW)->EnableWindow(m_pAddList->GetItemCount() < MAX_QUESTION_NUM);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(m_pList->GetItemCount());
	GetDlgItem(IDC_BUTTON_ALLBUILD)->EnableWindow(m_pAddList->GetItemCount() < MAX_QUESTION_NUM);

	CString string;

	// 新建状态，当前未生成CItem子类对象
	if (m_nCurShow < 0)
	{
		SetDlgItemText(IDC_EDIT_DESC, CItem::s_strNull);
		SetDlgItemText(IDC_EDIT_RESOLVE, CItem::s_strNull);

		string.Format(IDS_EDITDLG_INFONEW, m_pList->GetItemCount());
		SetDlgItemText(IDC_STATIC_EDITINFO, string);

		if (IsWindow(m_pListWnd->GetSafeHwnd()))
		{
			m_pListWnd->UpdateList();
		}
	}
	else
	{
		CItem *pCurShow = m_pList->GetItem(m_nCurShow);

		SetDlgItemText(IDC_EDIT_DESC, pCurShow->GetDescription());
		SetDlgItemText(IDC_EDIT_RESOLVE, pCurShow->GetResolve());

		string.Format(IDS_EDITDLG_INFOREVIEW, m_pList->GetItemCount(), m_nCurShow + 1);
		SetDlgItemText(IDC_STATIC_EDITINFO, string);

		if (IsWindow(m_pListWnd->GetSafeHwnd()))
		{
			m_pListWnd->UpdateList(m_nCurShow);
		}

		m_cTab.SwitchPage(AfxTypeToIndex(pCurShow->GetRawType()));
	}
	ShowPage();
	
	UpdateData(FALSE);
	m_bModify = FALSE;

	GetDlgItem(IDC_EDIT_DESC)->SetFocus();
}

void CEditDlg::ShowPage()
{
	int i;
	int nIndex = m_cTab.GetCurSel();
	CTabPage *pPage = NULL;

	for (i = 0; i < m_cTab.GetPageCount(); i++)
	{
		pPage = m_cTab.GetPage(i);
		if (i == nIndex)
		{
			if (m_nCurShow < 0)
			{
				pPage->SetShow(NULL);
			}
			else
			{
				pPage->SetShow(m_pList->GetItem(m_nCurShow));
			}
		}
		else
		{
			pPage->SetShow(NULL);
		}
	}
}

void CEditDlg::OnChange()
{
	m_bModify = TRUE;
}

BOOL CEditDlg::CheckModify()
{
	// 编辑内容发生了变化
	if (m_bModify || m_pImageWnd->IsModified())
	{
		return TRUE;
	}

	// 检查题目类型是否发生了变化
	if (m_nCurShow >= 0)
	{
		CItem *pCurShow = m_pList->GetItem(m_nCurShow);
		if (pCurShow->GetRawType() != Index2Type(m_cTab.GetCurSel()))
		{
			ASSERT(pCurShow->IsDefaultType());
			return TRUE;
		}
	}
	
	return m_cTab.GetPage(m_cTab.GetCurSel())->IsModify();
}

CChoise* CEditDlg::MakeChoise(BOOL bIsSingle)
{
	CChoise *pChoise = new CChoise(bIsSingle);
	if (!pChoise)
	{
		return NULL;
	}
	
	SaveChoise(pChoise, bIsSingle ? TYPE_SCHOISE : TYPE_MCHOISE);
	return pChoise;
}

CJudge* CEditDlg::MakeJudge()
{
	CJudge *pJudge = new CJudge;
	if (!pJudge)
	{
		return NULL;
	}
	
	SaveJudge(pJudge);
	return pJudge;
}

CBlank* CEditDlg::MakeBlank()
{
	CBlank *pBlank = new CBlank;
	if (!pBlank)
	{
		return NULL;
	}

	SaveBlank(pBlank);
	return pBlank;
}

CText* CEditDlg::MakeText()
{
	CText *pText = new CText;
	if (!pText)
	{
		return NULL;
	}

	SaveText(pText);
	return pText;
}

void CEditDlg::SaveCommon(CItem *pItem)
{
	CString string;

	GetDlgItemText(IDC_EDIT_DESC, string);
	pItem->SetDescription(string);
	
	GetDlgItemText(IDC_EDIT_RESOLVE, string);
	pItem->SetResolve(string);
}

void CEditDlg::SaveChoise(CItem *pChoise, int nType)
{
	ASSERT(TYPE_SCHOISE == nType || TYPE_MCHOISE == nType);

	if(pChoise->GetType() != nType)
	{
		CChoise *pTemp = new CChoise(TYPE_SCHOISE == nType);
		if (!pTemp)
		{
			return;
		}
		pTemp->SetStore(pChoise->IsStored());

		pTemp->CopyItem(pChoise);
		m_pList->ReplaceItem(pChoise, pTemp);
		delete pChoise;

		pChoise = pTemp;
		pChoise->ClearFromPtr(); // 存疑
	}
	
	SaveCommon(pChoise);
	if (TYPE_SCHOISE == nType)
	{
		m_pSChoiseDlg->GetInfo((CChoise*)pChoise);
	}
	else
	{
		m_pMChoiseDlg->GetInfo((CChoise*)pChoise);
	}
}

void CEditDlg::SaveJudge(CItem *pJudge)
{
	if (pJudge->GetType() != TYPE_JUDGE)
	{
		CJudge *pTemp = new CJudge;
		if (!pTemp)
		{
			return;
		}
		pTemp->SetStore(pJudge->IsStored());

		pTemp->CopyItem(pJudge);
		m_pList->ReplaceItem(pJudge, pTemp);
		delete pJudge;

		pJudge = pTemp;
		pJudge->ClearFromPtr();
	}
	
	SaveCommon(pJudge);
	m_pJudgeDlg->GetInfo((CJudge*)pJudge);
}

void CEditDlg::SaveBlank(CItem *pBlank)
{
	if (pBlank->GetType() != TYPE_BLANK)
	{
		CBlank *pTemp = new CBlank;
		if (!pTemp)
		{
			return;
		}
		pTemp->SetStore(pBlank->IsStored());
		
		m_pList->ReplaceItem(pBlank, pTemp);
		delete pBlank;

		pBlank = pTemp;
		pBlank->ClearFromPtr();
	}
	
	SaveCommon(pBlank);
	m_pBlankDlg->GetInfo((CBlank*)pBlank);
}

void CEditDlg::SaveText(CItem *pText)
{
	if (pText->GetType() != TYPE_TEXT)
	{
		CText *pTemp = new CText;
		if (!pTemp)
		{
			return;
		}
		pTemp->SetStore(pText->IsStored());
		
		m_pList->ReplaceItem(pText, pTemp);
		delete pText;

		pText = pTemp;
		pText->ClearFromPtr();
	}

	SaveCommon(pText);
	m_pTextDlg->GetInfo((CText*)pText);
}

int CEditDlg::CheckSaveable()
{
	CString str;
	GetDlgItemText(IDC_EDIT_DESC, str);

	// 在题目组中，允许没有题干
	if (!CTextManager::CheckValidCharactor(str) && m_nType != TYPE_GROUP)
	{
		CTMessageBox Box;
		switch(Box.SuperMessageBox(m_hWnd, IDS_EDITDLG_NODESC, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
		{
		case IDYES:
			// 丢失本题，继续
			return SAVEABLE_CONTINUE;

		case IDNO:
			// 留在本题，编辑
			return SAVEABLE_DENY;

		default:
			ASSERT(FALSE);
			return SAVEABLE_DENY;
		}
	}

	return m_cTab.GetPage(m_cTab.GetCurSel())->CheckSaveable();
}

/*
功能：	保存当前的题目信息
返回值：若允许后续步骤，则返回TRUE
备注：
*/
BOOL CEditDlg::SaveCurrent()
{
	// 在批量删除时会用到这个
	if (m_nCurShow >= 0 && !m_pList->GetItem(m_nCurShow))
	{
		m_bModify = FALSE;
		return TRUE;
	}

	// 通知图像窗口保存内容（一般为保存图片标题）
	m_pImageWnd->Save();

	if (!CheckModify())
	{
		return TRUE;
	}

	// 检查题目格式是否正确
	switch(CheckSaveable())
	{
	case SAVEABLE_OK:
		break;

	case SAVEABLE_CONTINUE:
		m_bModify = FALSE;
		return TRUE;
		
	case SAVEABLE_DENY:
		GetDlgItem(IDC_EDIT_DESC)->SetFocus();
		return FALSE;

	default:
		ASSERT(FALSE);
	}
	
	m_bEdited = TRUE;
	UpdateData();
	
	// 如果正在创建新的题目
	if(m_nCurShow < 0)
	{
		CItem *pItem = NULL;
		switch(CItem::GetType(Index2Type(m_cTab.GetCurSel())))
		{
		case TYPE_SCHOISE:
			pItem = MakeChoise(TRUE);
			break;

		case TYPE_MCHOISE:
			pItem = MakeChoise(FALSE);
			break;

		case TYPE_JUDGE:
			pItem = MakeJudge();
			break;

		case TYPE_BLANK:
			pItem = MakeBlank();
			break;

		case TYPE_TEXT:
			pItem = MakeText();
			break;

		default:
			ASSERT(FALSE);
		}
		pItem->SaveImage(ITEMIMAGEFLAG_DESC, &m_DescImageList);
		pItem->SaveImage(ITEMIMAGEFLAG_RESOLVE, &m_ResvImageList);
		ASSERT(!m_DescImageList.GetDataCount() && !m_ResvImageList.GetDataCount());

		m_pList->AddItem(pItem);
		m_nCurShow = m_pList->GetItemCount() - 1;
	}
	else
	{
		CItem *pCurShow = m_pList->GetItem(m_nCurShow);
		switch(CItem::GetType(Index2Type(m_cTab.GetCurSel())))
		{
		case TYPE_SCHOISE:
			SaveChoise(pCurShow, TYPE_SCHOISE);
			break;
			
		case TYPE_MCHOISE:
			SaveChoise(pCurShow, TYPE_MCHOISE);
			break;
			
		case TYPE_JUDGE:
			SaveJudge(pCurShow);
			break;
			
		case TYPE_BLANK:
			SaveBlank(pCurShow);
			break;
			
		case TYPE_TEXT:
			SaveText(pCurShow);
			break;
			
		default:
			ASSERT(FALSE);
		}
	}
	
	m_bModify = FALSE;
	return TRUE;
}

void CEditDlg::OnButtonPre() 
{
	if (!SaveCurrent())
	{
		return;
	}
	
	if (m_nCurShow < 0)
	{
		m_nCurShow = m_pList->GetItemCount() - 1;
	}
	else
	{
		--m_nCurShow;
	}
	SetShow();
}

void CEditDlg::OnButtonNext() 
{
	if (!SaveCurrent())
	{
		return;
	}
	
	++m_nCurShow;
	SetShow();
}

void CEditDlg::OnButtonNew() 
{
	// 如果当前内容修改过
	if (!SaveCurrent())
	{
		return;
	}
	
	if (m_nCurShow < 0)
	{
		m_nLastShow = m_pList->GetItemCount() - 1;
	}
	else
	{
		m_nLastShow = m_nCurShow;
	}
	m_nCurShow = -1;
	SetShow();
}

void CEditDlg::OnButtonDel() 
{
	Delete(DELTYPE_BUTTON);
}

void CEditDlg::OnButtonAllBuild() 
{
	CTMessageBox Box;

	// 如果当前内容修改过
	if (!SaveCurrent())
	{
		return;
	}

	LPTSTR ptr = GetEditFrmSel();
	if (!ptr)
	{
		Box.SuperMessageBox(m_hWnd, IDS_EDITDLG_CANNOTALLBUILD, IDS_MSGCAPTION_ERROR,
			MB_OK | MB_ICONERROR, 0, 0);
		return;
	}

	CAllBuildDlg Dlg(ptr, m_nType, m_pAddList);
	if (IDOK != Dlg.DoModal())
	{
		free(ptr);
		return;
	}

	CList *pList = Dlg.GetList();
	ASSERT(pList);

	if (!pList->GetItemCount())
	{
		Box.SuperMessageBox(m_hWnd, IDS_EDITDLG_ALLBUILDNULL, IDS_MSGCAPTION_INFOR,
			MB_OK | MB_ICONINFORMATION, 0, 0);
		free(ptr);
		return;
	}

	CString string;
	if (pList->GetItemCount() == Dlg.GetMakeAnswerCount())
	{
		string.Format(IDS_EDITDLG_ALLBUILDALLASW, pList->GetItemCount());
	}
	else
	{
		if (!Dlg.GetMakeAnswerCount())
		{
			string.Format(IDS_EDITDLG_ALLBUILDNOASW, pList->GetItemCount());
		}
		else
		{
			string.Format(IDS_EDITDLG_ALLBUILDPARTASW, pList->GetItemCount(), Dlg.GetMakeAnswerCount());
		}
	}

	// 如果没有全部解析完
	if (Dlg.GetStopPos())
	{
		CString add;
		add.LoadString(IDS_EDITDLG_ALLBUILDNOTALL);
		string += add;

		if (m_pEditWnd->IsWindowVisible())
		{
			add.LoadString(IDS_EDITDLG_ALLBUILDSHOWSEL);
			string += add;
			
			int nStart, nEnd;
			m_pEditWnd->GetSel(nStart, nEnd);
			m_pEditWnd->SetSel(nStart + Dlg.GetStopPos(), nEnd);
		}
	}

	free(ptr);
	Box.SuperMessageBox(m_hWnd, string, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);

	m_nCurShow = m_pList->GetItemCount() - (pList->GetItemCount() ? 0 : 1);
	m_pList->MoveFromList(pList);
	SetShow();

	m_bEdited = TRUE;
	m_bModify = FALSE;
}

void CEditDlg::OnButtonText() 
{
	ASSERT(m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()));
	m_pEditWnd->ShowWindow(SW_SHOW);
	m_pEditWnd->SetFocus();
}

void CEditDlg::OnButtonDesc() 
{
	LPTSTR ptr = GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(IDC_EDIT_DESC, ptr);
		free(ptr);
		m_bModify = TRUE;
	}
}

void CEditDlg::OnButtonResolve() 
{
	LPTSTR ptr = GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(IDC_EDIT_RESOLVE, ptr);
		free(ptr);
		m_bModify = TRUE;
	}
}

int CEditDlg::Index2Type(int nIndex)
{
	if (CItem::IsDefaultType(m_nType))
	{
		switch(nIndex)
		{
		case ITEMINDEX_SCHOISE:
			return TYPE_SCHOISE;
			
		case ITEMINDEX_MCHOISE:
			return TYPE_MCHOISE;
			
		case ITEMINDEX_JUDGE:
			return TYPE_JUDGE;
			
		case ITEMINDEX_BLANK:
			return TYPE_BLANK;
			
		case ITEMINDEX_TEXT:
			return TYPE_TEXT;
			
		default:
			ASSERT(FALSE);
			return TYPE_DEFAULT;
		}
	}
	else
	{
		ASSERT(nIndex == ITEMINDEX_CUSTOM);
		return m_nType;
	}
}

LPTSTR CEditDlg::GetEditFrmSel()
{
	LPTSTR ptr = NULL;
	if (m_pEditWnd->IsWindowVisible())
	{
		ptr = m_pEditWnd->GetSelText();
		if (ptr)
		{
			return ptr;
		}
	}
	
	ptr = AfxGetClipText(GetSafeHwnd());
	if (ptr)
	{
		return ptr;
	}
	
	// 复制不成功
	MessageBeep(0);
	return NULL;
}

void CEditDlg::OnCheckTopMost() 
{
	if (((CButton*)GetDlgItem(IDC_CHECK_TOPMOST))->GetCheck())
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

BOOL CEditDlg::SetCurSel(int nIndex)
{
	ASSERT(nIndex < m_pList->GetItemCount());
	if (nIndex < 0)
	{
		nIndex = 0;
	}
	
	if (IsWindow(GetSafeHwnd()))
	{
		if (!SaveCurrent())
		{
			return FALSE;
		}

		if (m_pList->GetItemCount())
		{
			m_nCurShow = nIndex;
		}
		else
		{
			m_nCurShow = -1;
		}
		SetShow();
	}
	else
	{
		m_nInitShow = nIndex;
	}

	return TRUE;
}

void CEditDlg::Delete(int nDelType, int nIndex /* = -1 */)
{
	switch (nDelType)
	{
	case DELTYPE_BUTTON:
		{
			CTMessageBox Box;
			if (IDOK != Box.SuperMessageBox(m_hWnd, IDS_EDITDLG_DELASK, IDS_MSGCAPTION_ASK,
				MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
			{
				return;
			}

			if (m_nCurShow < 0)
			{
				m_nCurShow = m_pList->GetItemCount() - 1;
			}
			else
			{
				CItem *pItem = m_pList->GetItem(m_nCurShow);
				m_pList->RemoveItem(pItem, TRUE);
				if (m_nCurShow >= m_pList->GetItemCount())
				{
					m_nCurShow = m_pList->GetItemCount() - 1;
				}
				m_bEdited = TRUE;
			}
			SetShow();
		}
		break;

	case DELTYPE_MARK:
		{
			ASSERT(nIndex >= 0);
			CItem *pItem = m_pList->GetItem(nIndex);
			pItem->SetItemState(ITEMMARK_DELETE, ITEMMARK_DELETE);
		}
		break;

	case DELTYPE_DO:
		{
			int i;
			for (i = 0; i < m_pList->GetItemCount();)
			{
				CItem *pItem = m_pList->GetItem(i);
				if (pItem->GetItemState(ITEMMARK_DELETE))
				{
					m_pList->RemoveItem(pItem, TRUE);
				}
				else
				{
					++i;
				}
			}
		}
		break;
	}
}

void CEditDlg::OnCheckQList()
{
	ASSERT(IsWindow(m_pListWnd->GetSafeHwnd()));
	
	if (((CButton*)GetDlgItem(IDC_CHECK_QLIST))->GetCheck())
	{
		m_pListWnd->ShowWindow(SW_SHOW);
		m_pListWnd->SetWndPos(m_hWnd);
	}
	else
	{
		m_pListWnd->ShowWindow(SW_HIDE);
	}
}

void CEditDlg::OnSetDescImage()
{
	if (m_nCurShow >= 0)
	{
		CItem *pCurShow = m_pList->GetItem(m_nCurShow);
		m_pImageWnd->SetImageList(pCurShow->GetImageList(ITEMIMAGEFLAG_DESC), 0);
	}
	else
	{
		m_pImageWnd->SetImageList(&m_DescImageList, 0);
	}
	m_pImageWnd->ShowWindow(SW_SHOW);
}

void CEditDlg::OnSetResImage() 
{
	if (m_nCurShow >= 0)
	{
		CItem *pCurShow = m_pList->GetItem(m_nCurShow);
		m_pImageWnd->SetImageList(pCurShow->GetImageList(ITEMIMAGEFLAG_RESOLVE), 0);
	}
	else
	{
		m_pImageWnd->SetImageList(&m_ResvImageList, 0);
	}
	m_pImageWnd->ShowWindow(SW_SHOW);
}
