// ItemGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "ItemGroupDlg.h"

#include "EditFrame.h"
#include "ImageFrame.h"

#include "EditDlg.h"
#include "PropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SUMMARYLENGTH		60

/////////////////////////////////////////////////////////////////////////////
// CItemGroupDlg dialog

CItemGroupDlg::CItemGroupDlg(CList *pList, CPaper *pPaper, CWnd* pParent /*=NULL*/) :
	CClrDialog(CItemGroupDlg::IDD, pParent),
	m_pList(pList), m_pPaper(pPaper),
	m_nCurShow(0), m_bIsNew(pList ? FALSE : TRUE),
	m_bModify(FALSE), m_bEdited(FALSE),
	m_pEditWnd(NULL), m_pImageWnd(NULL), m_ImageList(IMAGE_MAXCOUNT)
{
	//{{AFX_DATA_INIT(CItemGroupDlg)
	//}}AFX_DATA_INIT
}

CItemGroupDlg::~CItemGroupDlg()
{
	if (m_bIsNew && m_pList)
	{
		delete m_pList;
		m_pList = NULL;
	}
}

void CItemGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemGroupDlg)
	DDX_Control(pDX, IDC_EDIT_QNUMBER, m_cEditNumber);
	DDX_Control(pDX, IDC_BUTTON_SETRESIMAGE, m_cBtnResImage);
	DDX_Control(pDX, IDC_BUTTON_SETDESCIMAGE, m_cBtnDescImage);
	DDX_Control(pDX, IDC_BUTTON_COPY, m_cBtnCopy);
	DDX_Control(pDX, IDC_STATIC_GROUP3, m_cGroup3);
	DDX_Control(pDX, IDC_BUTTON_TEXT, m_cBtnText);
	DDX_Control(pDX, IDC_BUTTON_RESOLVE, m_cBtnResolve);
	DDX_Control(pDX, IDC_BUTTON_DESC, m_cBtnDesc);
	DDX_Control(pDX, IDC_EDIT_RESOLVE, m_cEditResolve);
	DDX_Control(pDX, IDC_BUTTON_DELG, m_cBtnDelG);
	DDX_Control(pDX, IDC_BUTTON_NEWG, m_cBtnNewG);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_PASTE, m_cBtnPaste);
	DDX_Control(pDX, IDC_BUTTON_OK, m_cBtnOk);
	DDX_Control(pDX, IDC_STATIC_GROUP2, m_cGroup2);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDC_LIST_GROUP, m_cList);
	DDX_Control(pDX, IDC_EDIT_ITEMGRPNAME, m_cEditItemGrpName);
	DDX_Control(pDX, IDC_EDIT_DESC, m_cEditDesc);
	DDX_Control(pDX, IDC_BUTTON_UP, m_cBtnUp);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_cBtnNew);
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_cBtnEdit);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_cBtnDown);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_cBtnDel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemGroupDlg, CClrDialog)
	//{{AFX_MSG_MAP(CItemGroupDlg)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_GROUP, OnGetDispInfo)
	ON_BN_CLICKED(IDC_BUTTON_DESC, OnButtonDesc)
	ON_BN_CLICKED(IDC_BUTTON_RESOLVE, OnButtonResolve)
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_NEWG, OnButtonNewG)
	ON_BN_CLICKED(IDC_BUTTON_DELG, OnButtonDelG)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, OnButtonPaste)
	ON_BN_CLICKED(IDC_BUTTON_TEXT, OnButtonText)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnButtonOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUP, OnDblClkListGroup)
	ON_EN_CHANGE(IDC_EDIT_ITEMGRPNAME, OnEditChange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GROUP, OnListGroupChanged)
	ON_BN_CLICKED(IDC_BUTTON_COPY, OnButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_SETDESCIMAGE, OnSetDescImage)
	ON_EN_CHANGE(IDC_EDIT_DESC, OnEditChange)
	ON_EN_CHANGE(IDC_EDIT_RESOLVE, OnEditChange)
	ON_EN_CHANGE(IDC_EDIT_QNUMBER, OnEditChange)
	ON_BN_CLICKED(IDC_BUTTON_SETRESIMAGE, OnSetResImage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemGroupDlg message handlers

int CItemGroupDlg::DoModal() 
{
	if (m_bIsNew)
	{
		ASSERT(!m_pList);
		m_pList = new CList(TYPE_GROUP);
		if (!m_pList)
		{
			return IDCANCEL;
		}

		CItemGroup *pGroup = new CItemGroup;
		if (!pGroup)
		{
			return IDCANCEL;
		}
		m_pList->AddItem(pGroup);
	}
	else if (!m_pList->GetItemCount())
	{
		return IDCANCEL;
	}
	m_nCurShow = 0;

	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->DestroyWindow();
	}
	m_pEditWnd = new CEditFrame(EDITFRAMETYPE_NEWOREDIT);
	if (!m_pEditWnd)
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

	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->DestroyWindow();
		m_pEditWnd = NULL;
	}
	if (m_pImageWnd && IsWindow(m_pImageWnd->GetSafeHwnd()))
	{
		m_pImageWnd->DestroyWindow();
		m_pImageWnd = NULL;
	}

	if (!m_pList->GetItemCount())
	{
		return IDCANCEL;
	}
	else
	{
		return nRet;
	}
}

void CItemGroupDlg::OnCancel()
{
	if (m_bModify)
	{
		CTMessageBox Box;
		if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_GROUPDLG_NOSAVE, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
		{
			return;
		}
	}

	CClrDialog::OnCancel();
}

BOOL CItemGroupDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();

	ModifyStyleEx(0, WS_EX_APPWINDOW);

	m_cImageList.Create(IDB_QVIEWICON, 16, 1, RGB (255, 0, 255));
	m_cImageList.SetBkColor(CLR_NONE);
	m_cList.SetImageList(&m_cImageList, LVSIL_SMALL);

	m_cList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	if(CLRTYPE_DEFAULT != g_nColorType)
	{
		m_cList.SetBkColor(AfxGetQColor(QCOLOR_FACE));
		m_cList.SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		m_cList.SetTextBkColor(AfxGetQColor(QCOLOR_FACE));
	}

	m_cEditItemGrpName.SetLimitText(MAX_CUSTOMTYPENAME_LEN);

	CString string;
	string.LoadString(IDS_ITEMGRPDLG_IDX);
	m_cList.InsertColumn(0, string, LVCFMT_LEFT, 40);
	
	string.LoadString(IDS_ITEMGRPDLG_TYPE);
	m_cList.InsertColumn(1, string, LVCFMT_LEFT, 80);
	
	string.LoadString(IDS_ITEMGRPDLG_DESC);
	m_cList.InsertColumn(2, string, LVCFMT_LEFT, 200);

	CRect rDlg;
	GetWindowRect(rDlg);
	AfxCalcDialogPos(&rDlg, FRAMEPOSTYPE_ALIGN);
	SetWindowPos(NULL, rDlg.left, rDlg.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	GetWindowRect(rDlg);
	AfxCalcFrameWndPos(&rDlg, FRAMEPOSTYPE_ALIGN);
	m_pEditWnd->CreateFrame(NULL, rDlg, AfxGetMainWnd(), 0);
	ASSERT(m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()));
	m_pEditWnd->ShowWindow(SW_HIDE);
	
	ASSERT(m_pImageWnd);
	string.LoadString(IDS_IMGFRM_ITEMIMAGE);
	m_pImageWnd->CreateFrame(string, AfxGetMainWnd(), rDlg, 0);
	ASSERT(IsWindow(m_pImageWnd->GetSafeHwnd()));

	SetShow();
	m_bModify = FALSE;
	
	if (m_bIsNew)
	{
		return TRUE;
	}
	else
	{
		m_cEditDesc.SetFocus();
		m_cEditDesc.SetSel(0, -1);
		return FALSE;
	}
}

void CItemGroupDlg::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;

	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	CItem *pItem = pGroup->GetItem(plvItem->iItem);
	ASSERT(pItem);

	if (plvItem->mask & LVIF_TEXT)
	{
		switch(plvItem->iSubItem)
		{
		case 0:
			_sntprintf(plvItem->pszText, plvItem->cchTextMax, _T("%d"), plvItem->iItem + 1);
			break;

		case 1:
			_tcsncpy(plvItem->pszText, pItem->GetTypeDesc(), plvItem->cchTextMax);
			break;

		case 2:
			{
				int nLen = pItem->GetDescriptionSummary(SUMMARYLENGTH);

				_stdstring str;
				pItem->MakeText(str, MKTXT_OPTIONSINALINE, 0);
				
				lstrcpyn(plvItem->pszText, str.c_str(), min(nLen + 1, plvItem->cchTextMax));
			}
			break;
		}
	}
	if (plvItem->mask & LVIF_IMAGE)
	{
		if (pItem->IsStored())
		{
			plvItem->iImage = 0;
		}
	}
	
	*pResult = 0;
}

void CItemGroupDlg::SetShow()
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	ASSERT(pGroup);

	if (pGroup->GetTypeDesc())
	{
		m_cEditItemGrpName.SetWindowText(pGroup->GetTypeDesc());
	}
	m_cEditDesc.SetWindowText(pGroup->GetDescription());
	m_cEditResolve.SetWindowText(pGroup->CItem::GetResolve());

	CString string;
	string.Format(IDS_GROUPDLG_INFOVIEW, m_pList->GetItemCount(), m_nCurShow + 1);
	SetDlgItemText(IDC_STATIC_EDITINFO, string);

	m_cBtnPre.EnableWindow(m_nCurShow);
	m_cBtnNext.EnableWindow(m_nCurShow < m_pList->GetItemCount() - 1);

	m_cList.SetItemCountEx(pGroup->GetItemCount(), LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
	int nSel = m_cList.GetNextItem(-1, LVNI_SELECTED);
	int nCount = m_cList.GetSelectedCount();

	m_cBtnUp.EnableWindow(nSel > 0 && nCount == 1);
	m_cBtnDown.EnableWindow(nCount == 1 && nSel < pGroup->GetItemCount() - 1);
	m_cBtnNew.EnableWindow(pGroup->GetItemCount() < MAX_ITEMGROUP_NUM);
	m_cBtnEdit.EnableWindow(pGroup->GetItemCount());
	m_cBtnDel.EnableWindow(nSel >= 0);
	m_cBtnPaste.EnableWindow(pGroup->GetItemCount() < MAX_ITEMGROUP_NUM);
	m_cBtnCopy.EnableWindow(nSel >= 0);

	SetDlgItemInt(IDC_EDIT_QNUMBER, pGroup->GetQNumber());
	m_cEditNumber.SetModify(FALSE);
}

BOOL CItemGroupDlg::SaveCurrent()
{
	// 通知图像窗口保存内容（一般为保存图片标题）
	m_pImageWnd->Save();

	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	if (m_bModify || m_pImageWnd->IsModified())
	{
		CString str;
		
		// 允许不写题目类型
		GetDlgItemText(IDC_EDIT_ITEMGRPNAME, str);
		pGroup->SetTypeDesc(str);
		
		// 允许不写描述
		GetDlgItemText(IDC_EDIT_DESC, str);
		pGroup->SetDescription(str);

		// 允许不写描述
		GetDlgItemText(IDC_EDIT_RESOLVE, str);
		pGroup->SetResolve(str);

		int n;
		CList *pList;

		n = GetDlgItemInt(IDC_EDIT_QNUMBER);
		pList = m_pPaper->CheckQNumberValid(n, pGroup);
		if (pList)
		{
			CTMessageBox Box;
			Box.SuperMessageBox(GetSafeHwnd(), IDS_TYPEPROPT_DUPL, IDS_MSGCAPTION_ERROR,
				MB_OK | MB_ICONERROR, 0, 0, pList->GetTypeDesc());
			m_cEditNumber.SetFocus();
			m_cEditNumber.SetSel(0, -1);

			return FALSE;
		}
		else
		{
			pGroup->SetQNumber(n);
		}

		m_bEdited = TRUE;
	}

	m_bModify = FALSE;
	return TRUE;
}

LPTSTR CItemGroupDlg::GetEditFrmSel()
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
	
	// 其余情况，就检查剪贴板
	ptr = AfxGetClipText(GetSafeHwnd());
	if (ptr)
	{
		return ptr;
	}
	
	// 复制不成功
	MessageBeep(0);
	return NULL;
}

void CItemGroupDlg::OnButtonDesc() 
{
	LPTSTR ptr = GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(IDC_EDIT_DESC, ptr);
		free(ptr);
		m_bModify = TRUE;
	}
}

void CItemGroupDlg::OnButtonResolve() 
{
	LPTSTR ptr = GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(IDC_EDIT_RESOLVE, ptr);
		free(ptr);
		m_bModify = TRUE;
	}
}

void CItemGroupDlg::OnButtonPre() 
{
	if (!SaveCurrent())
	{
		return;
	}

	--m_nCurShow;

	SetShow();
	m_cList.Invalidate(TRUE);
}

void CItemGroupDlg::OnButtonNext() 
{
	if (!SaveCurrent())
	{
		return;
	}
	
	++m_nCurShow;

	SetShow();
	m_cList.Invalidate(TRUE);
}

void CItemGroupDlg::OnButtonNewG() 
{
	if (!SaveCurrent())
	{
		return;
	}

	CItemGroup *pGroup = new CItemGroup;
	if (!pGroup)
	{
		return;
	}

	m_pList->AddItem(pGroup);
	m_nCurShow = m_pList->GetItemCount() - 1;
	SetShow();
}

void CItemGroupDlg::OnButtonDelG() 
{
	CTMessageBox Box;
	if (IDOK != Box.SuperMessageBox(m_hWnd, IDS_GROUPDLG_DELGASK, IDS_MSGCAPTION_ASK,
		MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	CItem *pCurShow = m_pList->GetItem(m_nCurShow);
	m_pList->RemoveItem(pCurShow, TRUE);

	if (!m_pList->GetItemCount())
	{
		pCurShow = new CItemGroup;
		m_pList->AddItem(pCurShow);
	}
	if (m_nCurShow >= m_pList->GetItemCount())
	{
		m_nCurShow = m_pList->GetItemCount() - 1;
	}

	SetShow();
}

void CItemGroupDlg::OnButtonUp() 
{
	if (m_cList.GetSelectedCount() > 1)
	{
		return;
	}

	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);

	int nSel = m_cList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel <= 0)
	{
		return;
	}

	pGroup->SwapItems(nSel, nSel - 1);
	m_cList.SetItemState(nSel - 1, LVNI_SELECTED, LVNI_SELECTED);
	m_cList.Invalidate();
	m_bModify = TRUE;
}

void CItemGroupDlg::OnButtonDown() 
{
	if (m_cList.GetSelectedCount() > 1)
	{
		return;
	}

	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);

	int nSel = m_cList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0 || nSel >= pGroup->GetItemCount())
	{
		return;
	}
	
	pGroup->SwapItems(nSel, nSel + 1);
	m_cList.SetItemState(nSel + 1, LVNI_SELECTED, LVNI_SELECTED);
	m_cList.Invalidate();
	m_bModify = TRUE;
}

void CItemGroupDlg::OnButtonNew() 
{
	if (!SaveCurrent())
	{
		return;
	}
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	ShowWindow(SW_HIDE);

	CEditDlg Dlg;
	Dlg.SetEditPara(NULL, pGroup, TYPE_GROUP);

	if (IDOK != Dlg.DoModal())
	{
		ShowWindow(SW_SHOW);
		return;
	}
	ShowWindow(SW_SHOW);

	pGroup->MoveFromList(Dlg.GetList());
	pGroup->ClearSubItemFrom();

	m_bModify = TRUE;
	SetShow();
}

void CItemGroupDlg::OnButtonEdit() 
{
	if (!SaveCurrent())
	{
		return;
	}
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);

	int nSel = m_cList.GetNextItem(-1, LVNI_SELECTED);
	if (nSel < 0)
	{
		nSel = 0;
	}
	CEditDlg Dlg(nSel);
	Dlg.SetEditPara(pGroup, pGroup, TYPE_GROUP);

	ShowWindow(SW_HIDE);
	if (IDOK != Dlg.DoModal())
	{
		ShowWindow(SW_SHOW);
		return;
	}
	ShowWindow(SW_SHOW);

	pGroup->ClearSubItemFrom();
	
	m_bModify = TRUE;
	SetShow();
}

void CItemGroupDlg::OnButtonDel() 
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);

	if (!SaveCurrent())
	{
		return;
	}

	CTMessageBox Box;
	if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_GROUPDLG_DELASK, IDS_MSGCAPTION_ASK,
		MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	m_cList.SetRedraw(FALSE);

	CTempList List;
	int nSel = m_cList.GetNextItem(-1, LVNI_SELECTED);
	while(nSel >= 0)
	{
		List.AddItem(pGroup->GetItem(nSel));
		nSel = m_cList.GetNextItem(nSel, LVNI_SELECTED);
	}
	
	int i;
	for (i = 0; i < List.GetItemCount(); ++i)
	{
		CItem *pItem = List.GetItem(i);
		pGroup->RemoveItem(pItem, TRUE);
	}

	m_cList.SetRedraw(TRUE);
	m_cList.Invalidate(TRUE);

	m_bModify = TRUE;
	SetShow();
}

void CItemGroupDlg::OnButtonPaste() 
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);

	if (!SaveCurrent())
	{
		return;
	}
	CList List;
	if (!AfxPasteItem(&List, m_hWnd) ||
		!List.GetItemCount())
	{
		return;
	}

	List.SetCustomIndex(0);
	
	int i;
	for (i = 0; i < List.GetItemCount();)
	{
		CItem *pItem = List.GetItem(i);
		if (pItem->GetType() != TYPE_GROUP)
		{
			if (!pGroup->AddItem(pItem))
			{
				break;
			}
		}
		else
		{
			++i;
		}
	}

	m_bModify = TRUE;
	SetShow();
}

void CItemGroupDlg::OnButtonCopy() 
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);

	CTempList List;
	int i;
	for (i = 0; i < pGroup->GetItemCount(); ++i)
	{
		CItem *pItem = pGroup->GetItem(i);
		// 仅复制有描述的题目
		if (_tcslen(pItem->GetDescription()))
		{
			List.AddItem(pItem);
		}
	}
	if (!List.GetItemCount())
	{
		return;
	}

	CFixedSharedFile sf;
	List.ClipCopy(sf);
	
	// 将题目文字写入剪贴板中
	CString strMain;
	_stdstring strTemp;
	for(i = 0; i < List.GetItemCount(); ++i)
	{
		CItem *pItem = List.GetItem(i);
		pItem->MakeText(strTemp,
			MKTXT_OPTIONNEWLINE | MKTXT_ENDLINE | MKTXT_JUDGEBRACKET, 0);
		strMain += strTemp.c_str();
		strMain += CTextManager::s_szWinReturn;
	}
	
	if (!strMain.GetLength())
	{
		return;
	}
	
	HGLOBAL hGlobal;
	PTSTR pGlobal;
	hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (strMain.GetLength() + 1) * sizeof (TCHAR));
	pGlobal = (PTSTR)GlobalLock(hGlobal);
	
	lstrcpy(pGlobal, strMain);
	GlobalUnlock(hGlobal);
	
	if (::OpenClipboard(NULL))
	{
		EmptyClipboard();
		
		SetClipboardData(CF_ITEM, sf.Detach());
		SetClipboardData(CF_TCHAR, hGlobal);
		
		CloseClipboard();
	}
}

void CItemGroupDlg::OnButtonText() 
{
	ASSERT(m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()));

	m_pEditWnd->ShowWindow(SW_SHOW);
	m_pEditWnd->SetFocus();
}

void CItemGroupDlg::OnButtonOk() 
{
	if (!SaveCurrent())
	{
		return;
	}
	
	int nIndex = m_pList->GetFirstInvalidItem();
	if (nIndex >= 0)
	{
		CTMessageBox Box;
		if (IDYES != Box.SuperMessageBox(GetSafeHwnd(), IDS_GROUPDLG_NOITEM,
			IDS_MSGCAPTION_ASK, MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
			m_nCurShow = nIndex;
			SetShow();
			return;
		}
		
		m_pList->ClearInvalidItems();
	}

	CDialog::OnOK();
}

void CItemGroupDlg::OnDblClkListGroup(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	int nSel = m_cList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nSel >= 0)
	{
		CTempList List;
		List.AddFromList(pGroup);

		CItemPropertiesDlg Dlg(&List, nSel);
		Dlg.DoModal();
	}
	
	*pResult = 0;
}

void CItemGroupDlg::OnEditChange() 
{
	m_bModify = TRUE;
}

void CItemGroupDlg::OnListGroupChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	SetShow();
	
	*pResult = 0;
}

void CItemGroupDlg::OnSetDescImage() 
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	m_pImageWnd->SetImageList(pGroup->GetImageList(ITEMIMAGEFLAG_DESC), 0);
	m_pImageWnd->ShowWindow(SW_SHOW);
}

void CItemGroupDlg::OnSetResImage() 
{
	CItemGroup *pGroup = (CItemGroup*)m_pList->GetItem(m_nCurShow);
	m_pImageWnd->SetImageList(pGroup->GetImageList(ITEMIMAGEFLAG_RESOLVE), 0);
	m_pImageWnd->ShowWindow(SW_SHOW);
}
