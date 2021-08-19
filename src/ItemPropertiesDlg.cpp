// ViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "PropertiesDlg.h"

#include "ShowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemPropertiesDlg dialog


CItemPropertiesDlg::CItemPropertiesDlg(CTempList *pList, int nInitShow /* = 0 */, CWnd* pParent /* = NULL */) :
	CClrDialog(CItemPropertiesDlg::IDD, pParent),
	m_pList(pList), m_nCurShow(nInitShow)
{
	//{{AFX_DATA_INIT(CItemPropertiesDlg)
	//}}AFX_DATA_INIT
}

CItemPropertiesDlg::~CItemPropertiesDlg()
{
}

void CItemPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CItemPropertiesDlg)
	DDX_Control(pDX, IDC_COMBO_FONTSIZE, m_cComboFont);
	DDX_Control(pDX, IDC_BUTTON_STORE, m_cBtnStore);
	DDX_Control(pDX, IDCANCEL, m_cBtnCancel);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_CUSTOM_ITEM, m_cShow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CItemPropertiesDlg, CClrDialog)
	//{{AFX_MSG_MAP(CItemPropertiesDlg)
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_STORE, OnStore)
	ON_CBN_SELCHANGE(IDC_COMBO_FONTSIZE, OnSelChangeFontSize)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ITEMSHOWWNDNOTIFY, OnItemShowWndNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemPropertiesDlg message handlers

int CItemPropertiesDlg::DoModal() 
{
	m_cShow.RegisterClass(AfxGetInstanceHandle());

	return CClrDialog::DoModal();
}

BOOL CItemPropertiesDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();

	m_cShow.SetShowType(SHOWWNDTYPE_PROPERTY);
	m_cComboFont.SetCurSel(g_nShowFont);
	OnSelChangeFontSize();

	m_cShow.SetList(m_pList);
	SetShow();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemPropertiesDlg::SetShow()
{
	GetDlgItem(IDC_BUTTON_PRE)->EnableWindow(m_pList->GetItemCount() > 1);
	GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(m_pList->GetItemCount() > 1);

	CItem *pItem = m_pList->GetItem(m_nCurShow)->GetItemFrom();
	SetDlgItemText(IDC_STATIC_QUESTIONTYPE, pItem->GetTypeDesc());

	m_cShow.ShowItem(m_nCurShow, SHOWSTYLE_WITHANSWER | SHOWSTYLE_WITHRESOLVE | SHOWSTYLE_WITHSTORE);
	RefreshStore();
}

void CItemPropertiesDlg::OnButtonPre() 
{
	--m_nCurShow;
	if (m_nCurShow < 0)
	{
		m_nCurShow = m_pList->GetItemCount() - 1;
	}

	m_cShow.SetFocus();
	SetShow();
}

void CItemPropertiesDlg::OnButtonNext() 
{
	++m_nCurShow;
	if (m_nCurShow >= m_pList->GetItemCount())
	{
		m_nCurShow = 0;
	}

	m_cShow.SetFocus();
	SetShow();
}

void CItemPropertiesDlg::OnSetFocus(CWnd* pOldWnd) 
{
	m_cShow.SetFocus();
}

void CItemPropertiesDlg::OnStore() 
{
	CItem *pItem = m_pList->GetItem(m_nCurShow);

	if (pItem->GetType() == TYPE_GROUP)
	{
		pItem = m_cShow.GetSelection();
	}
	pItem->SetStore(!pItem->IsStored());

	m_cShow.UpdateShowInfo();
	m_cShow.SetFocus();
}

void CItemPropertiesDlg::RefreshStore()
{
	CString string;
	CItem *pItem = m_pList->GetItem(m_nCurShow)->GetItemFrom();
	if (pItem->GetType() == TYPE_GROUP)
	{
		pItem = m_cShow.GetSelection();
	}
	
	if (pItem->IsStored())
	{
		string.LoadString(IDS_QVIEW_BUTTONUNSTORE);
	}
	else
	{
		string.LoadString(IDS_QVIEW_BUTTONSTORE);
	}
	m_cBtnStore.SetWindowText(string);
}

LRESULT CItemPropertiesDlg::OnItemShowWndNotify(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case ISN_SELECTITEM:
		RefreshStore();
		break;
	}
	
	return 0;
}

void CItemPropertiesDlg::OnSelChangeFontSize() 
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
