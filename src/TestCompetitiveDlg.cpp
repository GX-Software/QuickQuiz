// TestCompetitiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "TestCompetitiveDlg.h"

#include <windowsx.h>
#include "EditFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define STEP_INIT		0		// ����ǰʲô������ʾ
#define STEP_WAIT		1		// �ȴ�����������
#define STEP_END		2		// ���������ʾ��ʱ
#define STEP_SHOW		3		// ��ʾ������Ŀ���

/////////////////////////////////////////////////////////////////////////////
// CTestCompetitiveDlg dialog

CTestCompetitiveDlg::CTestCompetitiveDlg(CPaper *pPaper, CWnd* pParent /*=NULL*/) :
	CTabPage(CTestCompetitiveDlg::IDD, pParent),
	m_rcButton(0, 0, 0, 0), m_nStep(STEP_INIT),
	m_pPaper(pPaper), m_pCurShow(NULL),
	m_nCurShow(0), m_nNewest(0),
	m_nInter(200), m_uChar(0),
	m_pEditWnd(NULL)
{
	//{{AFX_DATA_INIT(CTestCompetitiveDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTestCompetitiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestCompetitiveDlg)
	DDX_Control(pDX, IDC_STATIC_GROUP3, m_cGroup3);
	DDX_Control(pDX, IDC_STATIC_GROUP2, m_cGroup2);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	DDX_Control(pDX, IDC_EDIT_INTER, m_cEditInter);
	DDX_Control(pDX, IDC_CHECK_SHOWTYPE, m_cCheckShowType);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_cBtnSave);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_OPTION, m_cBtnOption);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_cBtnNew);
	DDX_Control(pDX, IDC_STATIC_SHOW, m_cShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestCompetitiveDlg, CTabPage)
	//{{AFX_MSG_MAP(CTestCompetitiveDlg)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnButtonOption)
	//}}AFX_MSG_MAP
	ON_NOTIFY(WWN_SHOWEND, IDC_STATIC_SHOW, OnShowEnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestCompetitiveDlg message handlers

void CTestCompetitiveDlg::OnCancel()
{
	CDialog::OnCancel();
}

BOOL CTestCompetitiveDlg::OnInitDialog() 
{
	CTabPage::OnInitDialog();

	ModifyStyleEx(0, WS_EX_APPWINDOW);

	GetDlgItem(IDC_STATIC_BUTTON)->GetWindowRect(m_rcButton);
	ScreenToClient(m_rcButton);
	
	// Ĭ��0.2��һ����
	SetDlgItemText(IDC_EDIT_INTER, _T("0.2"));
	((CEdit*)GetDlgItem(IDC_EDIT_INTER))->SetLimitText(4);

	GetDlgItem(IDC_STATIC_SHOW)->SendMessage(WM_SETFONT, (WPARAM)g_hBigFont, TRUE);
	GetDlgItem(IDC_EDIT_DESC)->SendMessage(WM_SETFONT, (WPARAM)g_hBigFont, TRUE);
	GetDlgItem(IDC_EDIT_ANSWER)->SendMessage(WM_SETFONT, (WPARAM)g_hBigFont, TRUE);
	GetDlgItem(IDC_STATIC_BUTTON)->SendMessage(WM_SETFONT, (WPARAM)g_hBigFont, TRUE);

	ZeroMemory(&m_Para, sizeof(m_Para));
	m_Para.dwTypeMask = 0xFFFFFFFFL;

	// �����Ŀ�б�
	m_pPaper->FillTempList(&m_Para, &m_List);
	
	// ��ʼ��
	m_nCurShow = 0;
	InitShow();

	// ������Ŀ����ʾ��
	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->DestroyWindow();
		m_pEditWnd = NULL;
	}
	m_pEditWnd = new CEditFrame(EDITFRAMETYPE_SHOWDESC);
	if (m_pEditWnd)
	{
		CRect rc;
		GetWindowRect(rc);
		AfxCalcFrameWndPos(&rc, FRAMEPOSTYPE_AVOID);
		
		m_pEditWnd->CreateFrame(NULL, rc, this);
		m_pEditWnd->ShowWindow(SW_HIDE);
	}

	SetShow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CItem* CTestCompetitiveDlg::FindNext()
{
	m_nCurShow++;
	if (m_nCurShow >= m_List.GetItemCount())
	{
		m_List.Random(0, 0, time(NULL) + _getpid());
		m_nCurShow = 0;
	}
	
	return m_List.GetItem(m_nCurShow);
}

void CTestCompetitiveDlg::InitShow()
{
	m_uChar = 0;

	GetDlgItem(IDC_STATIC_SHOW)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DESC)->ShowWindow(SW_HIDE);
	SetDlgItemText(IDC_EDIT_ANSWER, _T(""));

	if (m_nCurShow >= m_List.GetItemCount())
	{
		if (m_Para.bRandom)
		{
			m_List.Random(0, 0, NULL);
		}
		m_nCurShow = 0;
	}
	m_pCurShow = m_List.GetItem(m_nCurShow);
	
	_stdstring string;
	m_pCurShow->MakeText(string, MKTXT_OPTIONNEWLINE | MKTXT_OPTIONLEN, 35);
	m_string = string.c_str();
	m_cShow.SetText(m_string);

	m_nStep = STEP_INIT;
	if (m_nCurShow > m_nNewest)
	{
		m_nNewest = m_nCurShow;
	}
}

void CTestCompetitiveDlg::SetShow()
{
	m_cCheckShowType.EnableWindow(m_nStep != STEP_WAIT);
	m_cBtnPre.EnableWindow(m_nCurShow && m_nStep != STEP_WAIT);
	m_cBtnNext.EnableWindow(m_nStep != STEP_WAIT);
	m_cBtnNew.EnableWindow(m_nStep != STEP_WAIT);
	m_cBtnSave.EnableWindow(m_nStep == STEP_SHOW);
	m_cBtnOption.EnableWindow(m_nStep != STEP_WAIT);
	m_cEditInter.EnableWindow(m_nStep != STEP_WAIT);

	// ��ʾ��Ŀ����
	if (m_cCheckShowType.GetCheck() || (STEP_SHOW == m_nStep))
	{
		SetDlgItemText(IDC_STATIC_TYPE, m_pCurShow->GetTypeDesc());
	}
	else
	{
		SetDlgItemText(IDC_STATIC_TYPE, _T("???"));
	}

	CString string;
	switch(m_nStep)
	{
	case STEP_INIT:
		string.LoadString(IDS_TESTCOMP_START);
		SetDlgItemText(IDC_STATIC_BUTTON, string);
		break;

	case STEP_WAIT:
		string.LoadString(IDS_TESTCOMP_STOP);
		SetDlgItemText(IDC_STATIC_BUTTON, string);
		break;

	case STEP_END:
		if (m_uChar)
		{
			if (m_uChar >= '0' && m_uChar <= '9')
			{
				string.Format(IDS_TESTCOMP_CHECKDIF, m_uChar, m_Time.GetDur());
			}
			else
			{
				string.Format(IDS_TESTCOMP_CHECKDIF, m_uChar & (~0x20ul), m_Time.GetDur());
			}
		}
		else
		{
			string.Format(IDS_TESTCOMP_CHECK, m_Time.GetDur());
		}
		SetDlgItemText(IDC_STATIC_BUTTON, string);
		break;

	case STEP_SHOW:
		string.LoadString(IDS_TESTCOMP_CONTINUE);
		SetDlgItemText(IDC_STATIC_BUTTON, string);
		break;

	default:
		ASSERT(FALSE);
	}

	if (m_pCurShow->IsStored())
	{
		string.LoadString(IDS_QVIEW_BUTTONUNSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}
	else
	{
		string.LoadString(IDS_VIEWDLG_BUTTONSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}

	// ��ʾ��Ŀ������
	CItem *pFrom = m_pCurShow->GetItemFrom();
	pFrom = pFrom->GetParent();
	if (pFrom->GetType() == TYPE_GROUP)
	{
		m_pEditWnd->SetContent(pFrom->GetDescription());
		m_pEditWnd->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pEditWnd->ShowWindow(SW_HIDE);
	}

	// û�а취�İ취���öԻ����ܹ���ȡWM_CHAR��Ϣ
	GetDlgItem(IDC_EDIT_ANSWER)->SetFocus();
}

void CTestCompetitiveDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_rcButton.PtInRect(point))
	{
		switch(m_nStep)
		{
		case STEP_INIT:
			OnStepInit();
			break;

		case STEP_WAIT:
			OnStepWait(0);
			break;

		case STEP_END:
			OnStepEnd();
			break;

		case STEP_SHOW:
			OnStepShow();
			break;
		}

		SetShow();
	}
	
	CTabPage::OnLButtonDown(nFlags, point);
}

BOOL CTestCompetitiveDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_CHAR == pMsg->message)
	{
		if (pMsg->hwnd != m_cEditInter.m_hWnd)
		{
			UINT uChar = pMsg->wParam;
			// �����������֡���ĸ���ո�ʱ����������
			if ((uChar >= '0' && uChar <= '9') ||
				(uChar >= 'A' && uChar <= 'Z') ||
				(uChar >= 'a' && uChar <= 'z') ||
				uChar == ' ')
			{
				switch(m_nStep)
				{
					// ��������
				case STEP_INIT:
					OnStepInit();
					break;
					
					// ֹͣ������ʾ������
				case STEP_WAIT:
					if (uChar != ' ')
					{
						OnStepWait(uChar);
					}
					else
					{
						OnStepWait(0);
					}
					break;
					
				case STEP_END:
					OnStepEnd();
					break;
					
				case STEP_SHOW:
					OnStepShow();
					break;
					
				default:
					return CTabPage::PreTranslateMessage(pMsg);
				}
				
				SetShow();
				return TRUE;
			}
		}
	}
	else if (WM_LBUTTONDBLCLK == pMsg->message)
	{
		CPoint pt(GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam));
		OnLButtonDown((UINT)pMsg->wParam, pt); 
	}

	return CTabPage::PreTranslateMessage(pMsg);
}

void CTestCompetitiveDlg::OnStepInit()
{
	CheckInput();

	// û�а취�İ취���öԻ����ܹ���ȡWM_CHAR��Ϣ
	GetDlgItem(IDC_EDIT_ANSWER)->SetFocus();

	m_nStep = STEP_WAIT;
	m_cShow.Show(m_nInter);
	m_Time.Start();
}

void CTestCompetitiveDlg::OnStepWait(UINT nChar)
{
	m_nStep = STEP_END;
	m_uChar = nChar;

	m_Time.End();
	m_cShow.Stop();
}

void CTestCompetitiveDlg::OnStepEnd()
{
	m_nStep = STEP_SHOW;

	SetDlgItemText(IDC_EDIT_DESC, m_string);
	
	_stdstring string;
	m_pCurShow->MakeAnswer(string, MKASW_WITHOPTION | MKASW_WITHINDEX, -1);
	m_string = string.c_str();
	SetDlgItemText(IDC_EDIT_ANSWER, m_string);
	
	GetDlgItem(IDC_EDIT_DESC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_SHOW)->ShowWindow(SW_HIDE);
}

void CTestCompetitiveDlg::OnStepShow()
{
	OnButtonNext();

	// û�а취�İ취���öԻ����ܹ���ȡWM_CHAR��Ϣ
	GetDlgItem(IDC_EDIT_ANSWER)->SetFocus();
}

BOOL CTestCompetitiveDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wCode = HIWORD(wParam);
	WORD wID = LOWORD(wParam);
	
	if (EN_KILLFOCUS == wCode && IDC_EDIT_INTER == wID)
	{
		return CheckInput();
	}
	
	return CTabPage::OnCommand(wParam, lParam);
}

BOOL CTestCompetitiveDlg::CheckInput()
{
	TCHAR str[5] = {0};
	GetDlgItemText(IDC_EDIT_INTER, str, 5);

	double lfInter = 0;
	_stscanf(str, _T("%f"), &lfInter);

	if (lfInter < 0.1f)
	{
		lfInter = 0.1f;
	}
	else if (lfInter > 2.0f)
	{
		lfInter = 2.0f;
	}

	_stprintf(str, _T("%.3G"), lfInter);
	SetDlgItemText(IDC_EDIT_INTER, str);

	lfInter *= 1000;
	m_nInter = (int)lfInter;
	
	return 0;
}

/*
���ܣ�	��Ӧ������ʾ������ʾ��������Ϣ
����ֵ����
��ע��	��ʱ�����굫��������ʱ����������
*/
void CTestCompetitiveDlg::OnShowEnd(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_Time.End();
	m_nStep = STEP_END;

	SetShow();
	*pResult = 0;
}

void CTestCompetitiveDlg::OnButtonPre() 
{
	--m_nCurShow;
	InitShow();

	SetShow();
}

void CTestCompetitiveDlg::OnButtonNext() 
{
	++m_nCurShow;
	InitShow();

	SetShow();
}

void CTestCompetitiveDlg::OnButtonNew() 
{
	// �ҵ����µ�λ��
	m_nCurShow = m_nNewest + 1;
	InitShow();

	SetShow();
}

void CTestCompetitiveDlg::OnButtonSave() 
{
	CString string;
	if (m_pCurShow->IsStored())
	{
		m_pCurShow->SetStore(FALSE);

		string.LoadString(IDS_VIEWDLG_BUTTONSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}
	else
	{
		m_pCurShow->SetStore(TRUE);

		string.LoadString(IDS_QVIEW_BUTTONUNSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}
}

void CTestCompetitiveDlg::OnButtonOption() 
{
	TYPEPARA Save;
	memcpy(&Save, &m_Para, sizeof(TYPEPARA));
	
	CChooseTypeDlg Dlg(&m_Para, m_pPaper);
	if (IDOK != Dlg.DoModal())
	{
		return;
	}
	
	// ����ѡ��������⣬���������
	// ������ѡ��δ�仯��ֱ�ӷ���
	if (!m_Para.bRandom && !memcmp(&Save, &m_Para, sizeof(TYPEPARA)))
	{
		return;
	}
	
	if (!m_Para.dwTypeMask)
	{
		CTMessageBox Box;
		Box.SuperMessageBox(m_hWnd, IDS_VIEWDLG_FINDFAIL, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);
		memcpy(&m_Para, &Save, sizeof(TYPEPARA));
		
		return;
	}
	
	// ��������޸�
	m_pPaper->FillTempList(&m_Para, &m_List);
	
	// ���û���ҵ���GetIndex����-1�����Դӿ�ͷ��ʼ
	// ����ҵ��ˣ��ʹӵ�ǰ��ʾ����һ����ʼ
	m_nCurShow = m_List.GetIndex(m_pCurShow);
	if (m_nCurShow < 0)
	{
		m_nCurShow = 0;
	}
	m_nNewest = 0;

	InitShow();
	SetShow();
}
