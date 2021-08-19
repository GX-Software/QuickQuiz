// QuickQuiz.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "QuickQuiz.h"

#include "MainFrm.h"
#include "Doc.h"
#include "LeftView.h"

#include "ClrDialog.h"
#include <Shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

AFX_STATIC_DATA const TCHAR _afxFileSection[] = _T("Recent File List");
AFX_STATIC_DATA const TCHAR _afxFileEntry[] = _T("File%d");
AFX_STATIC_DATA const TCHAR _afxPreviewSection[] = _T("Settings");
AFX_STATIC_DATA const TCHAR _afxPreviewEntry[] = _T("PreviewPages");

static LPCTSTR lpszRootItem		= _T("GX Software");
static LPCTSTR lpszRegisterVer	= _T("RegisterVer");

/////////////////////////////////////////////////////////////////////////////
// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_CLEARMRU, OnClearMRU)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApp construction

CApp::CApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CApp object

CApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CApp initialization

BOOL CApp::InitInstance()
{
	if (!CheckSimple())
	{
		return FALSE;
	}

	if (!AfxGetOSVersion())
	{
		MessageBox(NULL, _T("The application can not run on current OS!"), _T("Error!"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(lpszRootItem);

	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDoc),
		RUNTIME_CLASS(CMain),       // main SDI frame window
		RUNTIME_CLASS(CLView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 生成字体
	if (g_hBigFont)
	{
		DeleteObject(g_hBigFont);
	}
	if (g_hBigBoldFont)
	{
		DeleteObject(g_hBigBoldFont);
	}

	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);

	LOGFONT lf;
	memcpy(&lf, &ncm.lfMessageFont, sizeof(LOGFONT));
	g_hNormalFont = CreateFontIndirect(&lf);
	if (!g_hNormalFont)
	{
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		g_hNormalFont = CreateFontIndirect(&lf);
	}
	ASSERT(g_hNormalFont);

	lf.lfHeight = -15;
	g_hBigFont = CreateFontIndirect(&lf);
	ASSERT(g_hBigFont);
	
	lf.lfWeight = FW_BOLD;
	g_hBigBoldFont = CreateFontIndirect(&lf);
	ASSERT(g_hBigBoldFont);

	// 生成画刷
	if (g_hGreenBrush)
	{
		DeleteObject(g_hGreenBrush);
	}
	if (g_hDarkBrush)
	{
		DeleteObject(g_hDarkBrush);
	}

	AfxInitGlobalBrush();

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

int CApp::ExitInstance() 
{
	if (g_hNormalFont)
	{
		DeleteObject(g_hNormalFont);
	}
	if (g_hBigFont)
	{
		DeleteObject(g_hBigFont);
	}
	if (g_hBigBoldFont)
	{
		DeleteObject(g_hBigBoldFont);
	}

	AfxDestroyGlobalBrush();
	
	int nRet = CWinApp::ExitInstance();

	if (g_bReset)
	{
		ResetRegisterKey();
	}

	return nRet;
}

void CApp::OnUpdateMRUFile(CCmdUI *pCmdUI)
{
	ASSERT_VALID(this);
	if (m_pRecentFileList == NULL) // no MRU files
		pCmdUI->Enable(FALSE);
	else
		m_pRecentFileList->UpdateMenu(pCmdUI);
}

BOOL CApp::OpenRecentFile(CDoc *pDoc, UINT nID)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);
	
	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);
	
	if (pDoc->OnOpenDocument((*m_pRecentFileList)[nIndex]) == NULL)
		m_pRecentFileList->Remove(nIndex);
	
	return TRUE;
}

BOOL CApp::IsAnyRecentFile()
{
	int i, nSize;

	nSize = m_pRecentFileList->GetSize();
	for (i = 0; i < nSize; i++)
	{
		if (m_pRecentFileList->m_arrNames->GetLength())
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CApp::LoadStdProfileSettings(UINT nMaxMRU)
{
	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList == NULL);
	
	if (nMaxMRU != 0)
	{
		// create file MRU since nMaxMRU not zero
		m_pRecentFileList = new CFixedRecentFileList(0, _afxFileSection, _afxFileEntry,
			nMaxMRU);
		m_pRecentFileList->ReadList();
	}
	// 0 by default means not set
	m_nNumPreviewPages = GetProfileInt(_afxPreviewSection, _afxPreviewEntry, 0);
}

void CApp::OnClearMRU() 
{
	CTMessageBox Box;
	if (IDYES != Box.SuperMessageBox(AfxGetMainWnd()->m_hWnd, IDS_DOC_CLEARMRU, IDS_MSGCAPTION_ASK,
		MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	((CFixedRecentFileList*)m_pRecentFileList)->ClearList();
}

BOOL CApp::CheckSimple()
{
	// 创建命名信标对象。
	HANDLE hSem = CreateSemaphore(NULL, 1, 1, g_szPropName);
	if(hSem) // 信标对象创建成功。
	{
		// 信标对象已经存在，则程序已有一个实例在运行。
		if(ERROR_ALREADY_EXISTS == GetLastError())
		{                 
			CloseHandle(hSem);      //关闭信号量句柄。
			
			// 获取桌面窗口的一个子窗口。
			HWND hWndPrev=::GetWindow(::GetDesktopWindow(), GW_CHILD);
			
			while(::IsWindow(hWndPrev))
			{
				// 判断窗口是否有我们预先设置的标记，如有，则是我们寻找的窗口，并将它激活。
				if(::GetProp(hWndPrev, g_szPropName))
				{
					// 如果主窗口已最小化，则恢复其大小。
					if (::IsIconic(hWndPrev))    
					{
						::ShowWindow(hWndPrev,SW_RESTORE);
					}
					
					//将应用程序的主窗口激活。
					::SetForegroundWindow(hWndPrev);
					return FALSE;
				}
				//继续寻找下一个窗口。
				hWndPrev = ::GetWindow(hWndPrev, GW_HWNDNEXT);
			}
			
			AfxMessageBox(_T("Can not find the window of exist instance!"));
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox(_T("Can not build the semaphore!"));
		return FALSE;
	}

	return TRUE;
}

void CApp::ResetRegisterKey()
{
	ASSERT(m_pszRegistryKey != NULL);
	
	HKEY hSoftKey = NULL;
	HKEY hCompanyKey = NULL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("software"), 0, KEY_WRITE|KEY_READ,
		&hSoftKey) == ERROR_SUCCESS)
	{
		DWORD dw;
		RegCreateKeyEx(hSoftKey, m_pszRegistryKey, 0, REG_NONE,
			REG_OPTION_NON_VOLATILE, KEY_WRITE|KEY_READ, NULL,
			&hCompanyKey, &dw);
	}
	if (hSoftKey != NULL)
	{
		RegCloseKey(hSoftKey);
	}

	SHDeleteKey(hCompanyKey, m_pszProfileName);
	RegCloseKey(hCompanyKey);
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CClrDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CQButton	m_cBtnOK;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CClrDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CClrDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	CString string;
#ifdef _UNICODE
	string.Format(IDS_ABOUTDLG_CODE, _T("Unicode"));
#else
	string.Format(IDS_ABOUTDLG_CODE, _T("MBCS"));
#endif

#ifdef _DEBUG
	string += _T("(Debug)");
#endif
	SetDlgItemText(IDC_STATIC_TITLE, string);

	string.Format(IDS_ABOUTDLG_DATE, _T(__DATE__), _T(__TIME__));
	SetDlgItemText(IDC_STATIC_INFO, string);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

