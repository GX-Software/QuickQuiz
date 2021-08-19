// TabPage.cpp: implementation of the CTabPage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TabPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTabPage::CTabPage(UINT nIDTemplate, CWnd* pParentWnd /* = NULL */) :
	CClrDialog(nIDTemplate, pParentWnd),
	m_bModify(FALSE)
{
	
}

CTabPage::~CTabPage()
{

}

BEGIN_MESSAGE_MAP(CTabPage, CClrDialog)
	//{{AFX_MSG_MAP(CTabPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTabPage::Create(UINT nIDTemplate, CWnd* pParentWnd /* = NULL */)
{
	if (!CDialog::Create(nIDTemplate, pParentWnd))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTabPage::OnInitDialog()
{
	CClrDialog::OnInitDialog();

	EnableDialogTexture();

	return TRUE;
}

typedef HRESULT (WINAPI *PFNETDT)(HWND, DWORD);
#define ETDT_USETABTEXTURE              0x00000004
LRESULT CTabPage::EnableDialogTexture()
{
	PFNETDT pfn;
	HMODULE hModule;
	hModule = LoadLibrary(TEXT("uxtheme.dll"));
	if (hModule != NULL)
	{
		pfn = (PFNETDT)GetProcAddress(hModule, "EnableThemeDialogTexture");
		FreeLibrary(hModule);
		if (pfn != NULL)
		{
			return (*pfn)(GetSafeHwnd(), ETDT_USETABTEXTURE);
		}
	}
	return E_FAIL;
}
