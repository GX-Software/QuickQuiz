// ChooseTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "ChooseTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseTypeDlg dialog


CChooseTypeDlg::CChooseTypeDlg(PTYPEPARA pPara, CPaper *pPaper,
							   CWnd* pParent /*=NULL*/) :
	CClrDialog(CChooseTypeDlg::IDD, pParent),
	m_pPara(pPara), m_pPaper(pPaper)
{
	//{{AFX_DATA_INIT(CChooseTypeDlg)
	m_bRandom = FALSE;
	//}}AFX_DATA_INIT
}


void CChooseTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseTypeDlg)
	DDX_Control(pDX, IDC_LIST_TYPECHOOSE, m_cList);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDC_CHECK_RANDOM, m_cCheckRandom);
	DDX_Check(pDX, IDC_CHECK_RANDOM, m_bRandom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChooseTypeDlg, CClrDialog)
	//{{AFX_MSG_MAP(CChooseTypeDlg)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TYPECHOOSE, OnClickType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChooseTypeDlg message handlers

void CChooseTypeDlg::OnOK() 
{
	ASSERT(m_pPara);

	UpdateData();

	m_pPara->dwTypeMask = 0;
	
	int i;
	for (i = m_cList.GetItemCount() - 1; i >= 0; i--)
	{
		if (m_cList.GetCheck(i))
		{
			m_pPara->dwTypeMask |= m_cList.GetItemData(i);
		}
	}

	m_pPara->bRandom = m_bRandom;
	
	CDialog::OnOK();
}

BOOL CChooseTypeDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();

	m_cList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	if(CLRTYPE_DEFAULT != g_nColorType)
	{
		m_cList.SetBkColor(AfxGetQColor(QCOLOR_FACE));
		m_cList.SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		m_cList.SetTextBkColor(AfxGetQColor(QCOLOR_FACE));
	}
	
	ASSERT(m_pPara && m_pPaper);

	InitShow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChooseTypeDlg::InitShow()
{
	int nType = TYPE_SCHOISE, nIndex = 0;
	DWORD dwMask = TYPEMASK_SCHOISE;
	CList *pList = NULL;
	CString string;

	string.LoadString(IDS_TYPEDLG_TYPE);
	m_cList.InsertColumn(0, string, LVCFMT_LEFT, 80);
	
	string.LoadString(IDS_TYPEDLG_COUNT);
	m_cList.InsertColumn(1, string, LVCFMT_LEFT, 80);

	m_cList.DeleteAllItems();

	while(1)
	{
		pList = m_pPaper->GetTypeList(nType);
		if (!pList)
		{
			break;
		}
		else if (pList->GetItemCount())
		{
			m_cList.InsertItem(LVIF_TEXT | LVIF_PARAM, nIndex,
				pList->GetDescription(), NULL, NULL, NULL, dwMask);

			string.Format(_T("%d"), pList->GetItemCount());
			m_cList.SetItemText(nIndex, 1, string);

			if (m_pPara->dwTypeMask & dwMask)
			{
				m_cList.SetCheck(nIndex);
			}

			nIndex++;
		}

		dwMask <<= 1;
		nType = AfxGetNextType(nType);
	}

	m_bRandom = m_pPara->bRandom;
	UpdateData(FALSE);
}

void CChooseTypeDlg::OnClickType(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMLISTVIEW pListInfo = (LPNMLISTVIEW)pNMHDR;
	if (pListInfo->iItem >= 0)
	{
		CPoint pt;
		GetCursorPos(&pt);
		m_cList.ScreenToClient(&pt);
		UINT uFlags;
		m_cList.HitTest(pt, &uFlags);

		if (!(LVHT_ONITEMSTATEICON & uFlags))
		{
			BOOL b = m_cList.GetCheck(pListInfo->iItem);
			m_cList.SetCheck(pListInfo->iItem, !b);
		}
	}
	
	*pResult = 0;
}
