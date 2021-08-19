// PaperPropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "PropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaperPropertiesDlg dialog


CPaperPropertiesDlg::CPaperPropertiesDlg(CWnd* pParent /*=NULL*/) :
	CClrDialog(CPaperPropertiesDlg::IDD, pParent),
	m_pPaper(NULL), m_bShowStoreList(FALSE)
{
	//{{AFX_DATA_INIT(CPaperPropertiesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPaperPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaperPropertiesDlg)
	DDX_Control(pDX, IDC_STATIC_INFO, m_cGroup);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaperPropertiesDlg, CClrDialog)
	//{{AFX_MSG_MAP(CPaperPropertiesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaperPropertiesDlg message handlers

BOOL CPaperPropertiesDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	if (m_pPaper)
	{
		SetDlgItemText(IDC_EDIT_PAPERNAME, m_pPaper->GetDescription());

		CString string1, string2;
		LPCTSTR ptr = m_pPaper->GetFilePath();
		if (ptr)
		{
			SetDlgItemText(IDC_EDIT_PAPERPATH, ptr);
		}
		else
		{
			string1.LoadString(IDS_PAPER_NOPATH);
			SetDlgItemText(IDC_EDIT_PAPERPATH, string1);
		}

		string1.LoadString(IDS_ITEM_YES);
		string2.LoadString(IDS_ITEM_NO);
		SetDlgItemText(IDC_EDIT_CANEDIT, m_pPaper->CanEdit() ? string2 : string1);

		switch(m_pPaper->GetDefSaveStyle())
		{
		case PAPER_FILETYPE_BIN:
		case PAPER_FILETYPE_JPEGBIN:
			string1.LoadString(IDS_PAPER_BIN);
			break;

		case PAPER_FILETYPE_OLDBIN:
			string1.LoadString(IDS_PAPER_OLDBIN);
			break;

		case PAPER_FILETYPE_XML:
		case PAPER_FILETYPE_JPEGXML:
			string1.LoadString(IDS_PAPER_XML);
			break;

		default:
			string1.LoadString(IDS_PAPER_UNKNOWN);
		}
		SetDlgItemText(IDC_EDIT_FILETYPE, string1);

		CString str;
		if (m_bShowStoreList)
		{
			m_pPaper->GetSaveList()->GetListInfo(str);

			string1.LoadString(IDS_PROPS_STORE);
			SetDlgItemText(IDC_STATIC_INFO, string1);
		}
		else
		{
			m_pPaper->GetPaperInfo(str);
		}
		SetDlgItemText(IDC_EDIT_PAPERINFO, str);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
