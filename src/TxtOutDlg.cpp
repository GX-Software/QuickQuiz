// TxtOutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "TxtOutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR textout_TxtEx = _T("txt");
static LPCTSTR textout_HTMLEx = _T("html");

static LPCTSTR lpszTxtOutDlg = _T("Text Out");
static LPCTSTR lpszSel = _T("sel");
static LPCTSTR lpszAswType = _T("Asw Type");
static LPCTSTR lpszWithRes = _T("With Res");
static LPCTSTR lpszAnsiFont = _T("ANSI Font");
static LPCTSTR lpszMainFont = _T("Main Font");
static LPCTSTR lpszPrefaceFont = _T("Preface Font");

/////////////////////////////////////////////////////////////////////////////
// CTxtOutDlg dialog


CTxtOutDlg::CTxtOutDlg(CPaper *pPaper, CWnd* pParent /*=NULL*/) :
	CClrDialog(CTxtOutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTxtOutDlg)
	m_nSel = 0;
	m_sFileFullPath = _T("");
	m_nAswStyle = 0;
	m_bWithRes = FALSE;
	//}}AFX_DATA_INIT
	m_pPaper = pPaper;
}


void CTxtOutDlg::DoDataExchange(CDataExchange* pDX)
{
	CClrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTxtOutDlg)
	DDX_Control(pDX, IDC_CHECK_WITHRES, m_cCheckRes);
	DDX_Control(pDX, IDC_RADIO_WITHQ, m_cRadioWithQ);
	DDX_Control(pDX, IDC_RADIO_ONLYASW, m_cRadioOnly);
	DDX_Control(pDX, IDC_COMBO_PREFACEFONT, m_cComboPre);
	DDX_Control(pDX, IDC_COMBO_MAINFONT, m_cComboMain);
	DDX_Control(pDX, IDC_COMBO_ANSIFONT, m_cComboAnsi);
	DDX_Control(pDX, IDC_EDIT_FILEPATH, m_cEditPath);
	DDX_Control(pDX, IDC_BUTTON_FILEPATH, m_cBtnPath);
	DDX_Control(pDX, IDC_LIST_TXTOUT, m_cList);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_LBIndex(pDX, IDC_LIST_TXTOUT, m_nSel);
	DDX_Text(pDX, IDC_EDIT_FILEPATH, m_sFileFullPath);
	DDX_Radio(pDX, IDC_RADIO_ONLYASW, m_nAswStyle);
	DDX_Check(pDX, IDC_CHECK_WITHRES, m_bWithRes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTxtOutDlg, CClrDialog)
	//{{AFX_MSG_MAP(CTxtOutDlg)
	ON_LBN_DBLCLK(IDC_LIST_TXTOUT, OnDblClkList)
	ON_BN_CLICKED(IDC_BUTTON_FILEPATH, OnFilePath)
	ON_LBN_SELCHANGE(IDC_LIST_TXTOUT, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTxtOutDlg message handlers

BOOL CTxtOutDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	CString str;
	UINT load[] = {IDS_TXTOUTDLG_TXTONLY, IDS_TXTOUTDLG_HTMLPLUS, IDS_TXTOUTDLG_HTMLONLY};
	int i;
	for (i = 0; i < _countof(load); ++i)
	{
		str.LoadString(load[i]);
		m_cList.AddString(str);
	}

	FillFileInfo(m_pPaper->GetFilePath());
	m_sFileTitle = m_pPaper->GetDescription();

	m_nAswStyle = AfxGetApp()->GetProfileInt(lpszTxtOutDlg, lpszAswType, 0);
	m_bWithRes = AfxGetApp()->GetProfileInt(lpszTxtOutDlg, lpszWithRes, FALSE);
	UpdateData(FALSE);

	i = AfxGetApp()->GetProfileInt(lpszTxtOutDlg, lpszSel, 0);
	m_cList.SetCurSel(i);
	m_cList.SetFocus();
	OnSelChange();

	FillFontList();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTxtOutDlg::OnOK() 
{
	UpdateData();

	m_cComboAnsi.GetWindowText(m_sAnsiFont);
	m_cComboMain.GetWindowText(m_sMainFont);
	m_cComboPre.GetWindowText(m_sPreFont);

	SaveSettings();
	
	CClrDialog::OnOK();
}

void CTxtOutDlg::OnDblClkList() 
{
	POINT pt = {0};
	BOOL bOut;

	GetCursorPos(&pt);
	m_cList.ScreenToClient(&pt);

	CPoint cp(pt);
	m_cList.ItemFromPoint(cp, bOut);
	if (bOut)
	{
		return;
	}

	UpdateData();
	OnOK();
}

void CTxtOutDlg::OnSelChange() 
{
	CString str;
	int nSel = m_cList.GetCurSel();

	str = (m_sFilePath + m_sFileTitle + _T('.'));
	switch(nSel)
	{
	case PAPERTYPE_TEXTONLY:
		str += textout_TxtEx;
		m_cComboAnsi.EnableWindow(FALSE);
		m_cComboMain.EnableWindow(FALSE);
		m_cComboPre.EnableWindow(FALSE);
		break;

	case PAPERTYPE_HTMLONLY:
	case PAPERTYPE_HTMLPLUS:
		str += textout_HTMLEx;
		m_cComboAnsi.EnableWindow(TRUE);
		m_cComboMain.EnableWindow(TRUE);
		m_cComboPre.EnableWindow(TRUE);
		break;
	}

	m_cEditPath.SetWindowText(str);
}

void CTxtOutDlg::OnFilePath() 
{
	OPENFILENAME ofn;
	TCHAR strFilePath[MAX_PATH] = {0};
	UINT uID;
	LPCTSTR pExtension;
	
	CString strTitle;
	strTitle.LoadString(IDS_TXTOUTDLG_SAVEPATH);
	
	switch(m_cList.GetCurSel())
	{
	case PAPERTYPE_TEXTONLY:
		uID = IDS_TXTOUTDLG_TXTFILTER;
		pExtension = textout_TxtEx;
		break;

	case PAPERTYPE_HTMLONLY:
	case PAPERTYPE_HTMLPLUS:
		uID = IDS_TXTOUTDLG_HTMLFILTER;
		pExtension = textout_HTMLEx;
		break;
	}
	LPTSTR strFilter = CTextManager::LoadFileFilter(uID);
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_TXTOUTDLG_SAVEFAIL);
		return;
	}
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = GetSafeHwnd();
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = strFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = strFilePath;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = m_sFilePath;
	ofn.lpstrTitle        = strTitle;
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = pExtension;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;

	// 获取了文件路径
	if (GetSaveFileName(&ofn))
	{
		m_cEditPath.SetWindowText(strFilePath);
	}
	free(strFilter);

	FillFileInfo(strFilePath);
}

void CTxtOutDlg::FillFileInfo(LPCTSTR pPath)
{
	int i;
	for (i = _tcslen(pPath) - 1; i >= 0; --i)
	{
		if (pPath[i] == _T('\\'))
		{
			break;
		}
	}
	LPTSTR pSave = m_sFilePath.GetBuffer(i + 2);
	_tcscpyn(pSave, pPath, i + 2);
	m_sFilePath.ReleaseBuffer();
	
	pSave = m_sFileTitle.GetBuffer(_tcslen(pPath) - i + 2);
	_tcscpyn(pSave, pPath + i + 1, _tcslen(pPath) - i + 2);
	for (i = _tcslen(pSave); i >= 0; --i)
	{
		if (pSave[i] == _T('.'))
		{
			break;
		}
	}
	if (i >= 0)
	{
		pSave[i] = 0;
	}
	m_sFileTitle.ReleaseBuffer();
}

void CTxtOutDlg::FillFontList()
{
	HDC hDC = ::GetDC(GetSafeHwnd());
	LOGFONT lf = {0};
	LOGFONT deflf = {0};
	int n;
	CApp *pApp = (CApp*)AfxGetApp();

	GetObject(g_hNormalFont, sizeof(LOGFONT), &deflf);

	lf.lfCharSet = ANSI_CHARSET;
	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontFamExProc, (DWORD)&m_cComboAnsi, 0);
	m_sAnsiFont = pApp->GetProfileString(lpszTxtOutDlg, lpszAnsiFont, _T("Times New Roman"));
	n = m_cComboAnsi.SelectString(-1, m_sAnsiFont);
	if (CB_ERR == n)
	{
		m_cComboAnsi.SetCurSel(0);
	}

	lf.lfCharSet = DEFAULT_CHARSET;
	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontFamExProc, (DWORD)&m_cComboMain, 0);
	m_sMainFont = pApp->GetProfileString(lpszTxtOutDlg, lpszMainFont, deflf.lfFaceName);
	n = m_cComboMain.SelectString(-1, m_sMainFont);
	if (CB_ERR == n)
	{
		m_cComboMain.SetCurSel(0);
	}

	EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumFontFamExProc, (DWORD)&m_cComboPre, 0);
	m_sPreFont = pApp->GetProfileString(lpszTxtOutDlg, lpszPrefaceFont, deflf.lfFaceName);
	n = m_cComboPre.SelectString(-1, m_sPreFont);
	if (CB_ERR == n)
	{
		m_cComboPre.SetCurSel(0);
	}
}

int CALLBACK CTxtOutDlg::EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD FontType, LPARAM lParam)
{
	CQComboBox *cBox = (CQComboBox*)lParam;

	// 不保存隐藏字体
	if (_tcsnicmp(_T("@"), lpelfe->elfLogFont.lfFaceName, 1) &&
		CB_ERR == cBox->FindStringExact(-1, lpelfe->elfLogFont.lfFaceName))
	{
		cBox->AddString(lpelfe->elfLogFont.lfFaceName);
	}

	return TRUE;
}

void CTxtOutDlg::SaveSettings()
{
	CApp *pApp = (CApp*)AfxGetApp();
	int n = m_cList.GetCurSel();

	UpdateData();

	pApp->WriteProfileInt(lpszTxtOutDlg, lpszSel, n);
	pApp->WriteProfileInt(lpszTxtOutDlg, lpszAswType, m_nAswStyle);
	pApp->WriteProfileInt(lpszTxtOutDlg, lpszWithRes, m_bWithRes);

	pApp->WriteProfileString(lpszTxtOutDlg, lpszAnsiFont, m_sAnsiFont);
	pApp->WriteProfileString(lpszTxtOutDlg, lpszMainFont, m_sMainFont);
	pApp->WriteProfileString(lpszTxtOutDlg, lpszPrefaceFont, m_sPreFont);
}
