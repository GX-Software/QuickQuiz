// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "PasswordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/) :
	CClrDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	//}}AFX_DATA_INIT
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Control(pDX, IDC_EDIT_CHECKPSWD, m_cEditCheckPswd);
	DDX_Control(pDX, IDC_EDIT_NEWPSWD, m_cEditNewPswd);
	DDX_Control(pDX, IDC_EDIT_OLDPSWD, m_cEditOldPswd);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CClrDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers

void CPasswordDlg::OnOK() 
{
	if (CheckInput())
	{
		CDialog::OnOK();
	}
	else
	{
		if (m_uOldPswd[0])
		{
			GetDlgItem(IDC_EDIT_OLDPSWD)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_OLDPSWD))->SetSel(0, -1);
		}
		else
		{
			GetDlgItem(IDC_EDIT_NEWPSWD)->SetFocus();
			((CEdit*)GetDlgItem(IDC_EDIT_NEWPSWD))->SetSel(0, -1);
		}

		GetDlgItem(IDC_EDIT_CHECKPSWD)->SetWindowText(_T(""));
	}
}

BOOL CPasswordDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	((CEdit*)GetDlgItem(IDC_EDIT_OLDPSWD))->SetLimitText(MAX_PASSWORD_LEN);
	((CEdit*)GetDlgItem(IDC_EDIT_NEWPSWD))->SetLimitText(MAX_PASSWORD_LEN);
	((CEdit*)GetDlgItem(IDC_EDIT_CHECKPSWD))->SetLimitText(MAX_PASSWORD_LEN);

	if (m_bIsLocked)
	{
		GetDlgItem(IDC_EDIT_OLDPSWD)->SetFocus();
	}
	else
	{
		GetDlgItem(IDC_EDIT_OLDPSWD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_NEWPSWD)->SetFocus();
	}
	
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPasswordDlg::SetPara(BOOL bIsLocked, LPBYTE pOldPswd)
{
	m_bIsLocked = bIsLocked;

	if (!m_bIsLocked)
	{
		ZeroMemory(m_uOldPswd, sizeof(m_uOldPswd));
	}
	else
	{
		memcpy(m_uOldPswd, pOldPswd, sizeof(m_uOldPswd));
	}
	
	ZeroMemory(m_uNewPswd, sizeof(m_uNewPswd));
}

BOOL CPasswordDlg::CheckInput()
{
	UCHAR uKey[16] = {0};
	CTMessageBox Box;
	TCHAR string[MAX_PASSWORD_LEN + 1] = {0}, string1[MAX_PASSWORD_LEN + 1] = {0};
#ifdef _UNICODE
	char ptrC[MAX_PASSWORD_LEN + 1] = {0};
#endif

	// 需要检查旧密码时
	if (m_uOldPswd[0])
	{
		GetDlgItemText(IDC_EDIT_OLDPSWD, string, MAX_PASSWORD_LEN + 1);
		
#ifdef _UNICODE
		if (!WideCharToMultiByte(CP_ACP, 0, string, -1, ptrC, 17, NULL, FALSE))
		{
			return FALSE;
		}
		
		MakeMD5((PBYTE)ptrC, strlen(ptrC), uKey);
#else
		MakeMD5((PBYTE)string, strlen(string), uKey);
#endif
		
		if (memcmp(m_uOldPswd, uKey, sizeof(m_uOldPswd)))
		{
			Box.SuperMessageBox(m_hWnd, IDS_PSWDDLG_OLDPSWDF, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);
			return FALSE;
		}
	}

	GetDlgItemText(IDC_EDIT_NEWPSWD, string, MAX_PASSWORD_LEN + 1);
	GetDlgItemText(IDC_EDIT_CHECKPSWD, string1, MAX_PASSWORD_LEN + 1);
	if (!string[0])
	{
		switch(Box.SuperMessageBox(m_hWnd, IDS_PSWDDLG_DELPSWD, IDS_MSGCAPTION_ASK,
			MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
		{
		case IDOK:
			ZeroMemory(m_uNewPswd, sizeof(m_uNewPswd));
			return TRUE;

		case IDCANCEL:
			return FALSE;
		}
	}
	if (lstrlen(string) < 6)
	{
		Box.SuperMessageBox(m_hWnd, IDS_PSWDDLG_TOOSHORT, IDS_MSGCAPTION_ASK, MB_OK | MB_ICONINFORMATION, 0, 0);
		return FALSE;
	}
	
	if (_tcscmp(string, string1))
	{
		Box.SuperMessageBox(m_hWnd, IDS_PSWDDLG_NEWPSWDIN, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);
		return FALSE;
	}

#ifdef _UNICODE
	if (!WideCharToMultiByte(CP_ACP, 0, string, -1, ptrC, MAX_PASSWORD_LEN + 1, NULL, FALSE))
	{
		return FALSE;
	}
	
	MakeMD5((PBYTE)ptrC, strlen(ptrC), uKey);
	strcpy(m_sPswd, ptrC);
#else
	MakeMD5((PBYTE)string, strlen(string), uKey);
	strcpy(m_sPswd, string);
#endif

	memcpy(m_uNewPswd, uKey, sizeof(m_uNewPswd));
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CUnlockDlg dialog

CUnlockDlg::CUnlockDlg(LPBYTE pPswd, CWnd* pParent /*=NULL*/) :
	CClrDialog(CUnlockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUnlockDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	memcpy(m_uTrue, pPswd, sizeof(m_uTrue));
}


void CUnlockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUnlockDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_cEditPswd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnlockDlg, CClrDialog)
	//{{AFX_MSG_MAP(CUnlockDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnlockDlg message handlers

void CUnlockDlg::OnOK() 
{
	if (!CheckInput())
	{
		CTMessageBox Box;
		Box.SuperMessageBox(m_hWnd, IDS_PSWDDLG_WRONG, IDS_MSGCAPTION_ERROR, MB_OK | MB_ICONERROR, 0, 0);
		
		m_cEditPswd.SetWindowText(_T(""));
		m_cEditPswd.SetFocus();
		return;
	}
	
	CClrDialog::OnOK();
}

BOOL CUnlockDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	m_cEditPswd.SetLimitText(MAX_PASSWORD_LEN);
	m_cEditPswd.SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUnlockDlg::CheckInput()
{
	BYTE uKey[16] = {0};
	TCHAR string[MAX_PASSWORD_LEN + 1] = {0};
	
	GetDlgItemText(IDC_EDIT_PASSWORD, string, MAX_PASSWORD_LEN + 1);
	if (!string[0])
	{
		return FALSE;
	}
	
#ifdef _UNICODE
	char ptrC[MAX_PASSWORD_LEN + 1] = {0};
	if (!WideCharToMultiByte(CP_ACP, 0, string, -1, ptrC, MAX_PASSWORD_LEN + 1, NULL, FALSE))
	{
		return FALSE;
	}
	
	MakeMD5((PBYTE)ptrC, strlen(ptrC), uKey);
	strcpy(m_sPswd, ptrC);
#else
	MakeMD5((PBYTE)string, strlen(string), uKey);
	strcpy(m_sPswd, string);
#endif
	
	return memcmp(m_uTrue, uKey, sizeof(m_uTrue)) ? FALSE : TRUE;
}
