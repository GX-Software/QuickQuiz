// JpegLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JpegCoverDlg.h"
#include "ImageManager.h"
#include "ImageFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR jpeg_lpszImageExt = _T("jpeg");

enum {
	RADIO_NOCOVER = 0,
	RADIO_DEFALTCOVER,
	RADIO_CUSTOMCOVER,
};

/////////////////////////////////////////////////////////////////////////////
// CJpegCoverDlg dialog

CJpegCoverDlg::CJpegCoverDlg(CPaper *pPaper, CWnd* pParent /* = NULL */) :
	CClrDialog(CJpegCoverDlg::IDD, pParent),
	m_pPaper(pPaper), m_clrBk(JPEGCOVER_DEFAULTBKCOLOR), m_clrTxt(JPEGCOVER_DEFAULTTXTCOLOR),
	m_nWidth(JPEGCOVER_DEFAULTWIDTH), m_nHeight(JPEGCOVER_DEFAULTHEIGHT), m_nQuatlity(JPEGCOVER_DEFAULTQUALITY),
	m_bModify(FALSE),
	m_pImgFrame(NULL)
{
	//{{AFX_DATA_INIT(CJpegCoverDlg)
	m_nRadioType = 0;
	//}}AFX_DATA_INIT
	ZeroMemory(&m_inf, sizeof(m_inf));
}

CJpegCoverDlg::~CJpegCoverDlg()
{
	if (m_inf.strTitle)
	{
		free(m_inf.strTitle);
		m_inf.strTitle = NULL;
	}
	if (m_inf.strJpegPath)
	{
		free(m_inf.strJpegPath);
		m_inf.strJpegPath = NULL;
	}
}

void CJpegCoverDlg::DoDataExchange(CDataExchange* pDX)
{
	CClrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJpegCoverDlg)
	DDX_Control(pDX, IDC_BUTTON_VIEW, m_cBtnView);
	DDX_Control(pDX, IDC_STATIC_TXTCOLOR, m_cTxtColor);
	DDX_Control(pDX, IDC_STATIC_BKCOLOR, m_cBkColor);
	DDX_Control(pDX, IDC_RADIO_NO, m_cRadioNo);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDCANCEL, m_cBtnCancel);
	DDX_Control(pDX, IDC_RADIO_DEFAULT, m_cRadioDefault);
	DDX_Control(pDX, IDC_RADIO_USER, m_cRadioUser);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_cEditWidth);
	DDX_Control(pDX, IDC_EDIT_TITLE, m_cEditTitle);
	DDX_Control(pDX, IDC_EDIT_QUALITY, m_cEditQuality);
	DDX_Control(pDX, IDC_EDIT_JPEGCHOOSE, m_cEditJpegChoose);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_cEditHeight);
	DDX_Control(pDX, IDC_BUTTON_OPEN, m_cBtnOpen);
	DDX_Radio(pDX, IDC_RADIO_NO, m_nRadioType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJpegCoverDlg, CClrDialog)
	//{{AFX_MSG_MAP(CJpegCoverDlg)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnView)
	ON_BN_CLICKED(IDC_STATIC_BKCOLOR, OnSetBkColor)
	ON_BN_CLICKED(IDC_STATIC_TXTCOLOR, OnSetTxtColor)
	ON_BN_CLICKED(IDC_RADIO_NO, OnRadio)
	ON_BN_CLICKED(IDC_RADIO_USER, OnRadio)
	ON_BN_CLICKED(IDC_RADIO_DEFAULT, OnRadio)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, OnChangeEdit)
	ON_EN_CHANGE(IDC_EDIT_TITLE, OnChangeEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJpegCoverDlg message handlers

void CJpegCoverDlg::OnOK() 
{
	UpdateData();
	CTMessageBox Box;

	m_cEditTitle.GetWindowText(m_strTitle);
	if (PAPER_COVERTYPE_DEFAULT == m_nRadioType && !m_strTitle.GetLength())
	{
		Box.SuperMessageBox(GetSafeHwnd(), IDS_JPEGCOVERDLG_NOTITLE, IDS_MSGCAPTION_WARNING, MB_OK | MB_ICONWARNING, 0, 0);
		return;
	}

	m_cEditJpegChoose.GetWindowText(m_strJpegPath);
	if (PAPER_COVERTYPE_CUSTOM == m_nRadioType && !m_strJpegPath.GetLength())
	{
		Box.SuperMessageBox(GetSafeHwnd(), IDS_JPEGCOVERDLG_NOPATH, IDS_MSGCAPTION_WARNING, MB_OK | MB_ICONWARNING, 0, 0);
		return;
	}

	m_nWidth = GetDlgItemInt(IDC_EDIT_WIDTH);
	m_nHeight = GetDlgItemInt(IDC_EDIT_HEIGHT);
	m_nQuatlity = GetDlgItemInt(IDC_EDIT_QUALITY);
	
	CClrDialog::OnOK();
}

int CJpegCoverDlg::DoModal() 
{
	if (m_pImgFrame && IsWindow(m_pImgFrame->GetSafeHwnd()))
	{
		m_pImgFrame->DestroyWindow();
	}

	m_pImgFrame = new CImageFrame();
	if (!m_pImgFrame)
	{
		return IDCANCEL;
	}
	
	int nRet = CClrDialog::DoModal();

	if (m_pImgFrame && IsWindow(m_pImgFrame->GetSafeHwnd()))
	{
		m_pImgFrame->DestroyWindow();
	}
	m_pImgFrame = NULL;
	return nRet;
}

BOOL CJpegCoverDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	ASSERT(m_pPaper);
	LPCPAPERCOVERINFO i = m_pPaper->GetCoverInfo();

	m_nRadioType = i->nType;
	UpdateData(FALSE);
	OnRadio();

	m_cEditTitle.SetLimitText(MAX_PAPERNAME_LEN);
	m_cEditTitle.SetWindowText(i->strTitle);

	m_cEditWidth.SetLimitText(4);
	SetDlgItemInt(IDC_EDIT_WIDTH, i->nWidth);

	m_cEditHeight.SetLimitText(4);
	SetDlgItemInt(IDC_EDIT_HEIGHT, i->nHeight);

	m_cEditQuality.SetLimitText(3);
	SetDlgItemInt(IDC_EDIT_QUALITY, i->nQuality);

	m_clrBk = i->clrBk;
	m_cBkColor.SetColor(i->clrBk);
	m_clrTxt = i->clrText;
	m_cTxtColor.SetColor(i->clrText);

	m_cRadioNo.EnableWindow(m_pPaper->CanEdit());
	m_cRadioDefault.EnableWindow(m_pPaper->CanEdit());
	m_cRadioUser.EnableWindow(m_pPaper->CanEdit());
	m_cEditWidth.SetReadOnly(!m_pPaper->CanEdit());
	m_cEditHeight.SetReadOnly(!m_pPaper->CanEdit());
	m_cEditTitle.SetReadOnly(!m_pPaper->CanEdit());
	m_cBtnOpen.EnableWindow(m_pPaper->CanEdit());
	m_cBtnOK.EnableWindow(m_pPaper->CanEdit());
	m_cBkColor.SetReadOnly(!m_pPaper->CanEdit());
	m_cTxtColor.SetReadOnly(!m_pPaper->CanEdit());
	m_cEditQuality.SetReadOnly(!m_pPaper->CanEdit());

	m_bModify = FALSE;

	ASSERT(m_pImgFrame);

	CString string;
	string.LoadString(IDS_IMGFRM_COVERVIEW);
	m_pImgFrame->CreateFrame(string, this);
	if (IsWindow(m_pImgFrame->GetSafeHwnd()))
	{
		m_pImgFrame->ShowWindow(SW_HIDE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJpegCoverDlg::OnOpen() 
{
	OPENFILENAME ofn;
	TCHAR strFilePath[MAX_PATH] = {0};
	
	CString strTitle;
	strTitle.LoadString(IDS_JPEGCOVERDLG_OPENTITLE);
	
	LPTSTR strFilter = CTextManager::LoadFileFilter(IDS_JPEGCOVERDLG_OPENFILTER);
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_JPEGCOVERDLG_OPENFAIL);
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
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = strTitle;
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = jpeg_lpszImageExt;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;

	// 获取了文件路径
	if (GetOpenFileName(&ofn))
	{
		m_cEditJpegChoose.SetWindowText(strFilePath);
	}

	free(strFilter);

	m_nRadioType = PAPER_COVERTYPE_CUSTOM;
	UpdateData(FALSE);
	m_cBtnView.EnableWindow(TRUE);
	m_bModify = TRUE;
}

void CJpegCoverDlg::OnView() 
{
	UpdateData();

	CString str;
	PAPERCOVERINFO info = {0};
	info.nType = m_nRadioType;

	if (PAPER_COVERTYPE_DEFAULT == info.nType)
	{
		m_cEditTitle.GetWindowText(str);
		
		info.nWidth = GetDlgItemInt(IDC_EDIT_WIDTH);
		info.nHeight = GetDlgItemInt(IDC_EDIT_HEIGHT);
		info.strTitle = str.GetBuffer(str.GetLength());
		info.clrBk = m_clrBk;
		info.clrText = m_clrTxt;
		info.nQuality = GetDlgItemInt(IDC_EDIT_QUALITY);
		
		HDC hDC = ::GetDC(GetSafeHwnd());
		ShowDefaultCover(hDC, &m_Bmp, &info);
		::ReleaseDC(GetSafeHwnd(), hDC);
	}
	else if (PAPER_COVERTYPE_CUSTOM == info.nType)
	{
		PBYTE pJpeg;
		size_t ulJpegSize;
		BOOL bNeedFree;

		// 先检查当前打开的内容
		CString strPath;
		m_cEditJpegChoose.GetWindowText(strPath);
		if (strPath.GetLength())
		{
			FILE *fp = _tfopen(strPath, _T("rb"));
			if (!fp)
			{
				return;
			}

			fseek(fp, 0, SEEK_END);
			long lLen = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			PBYTE ptr = (PBYTE)malloc(lLen);
			if (!ptr)
			{
				return;
			}
			if (!fread(ptr, lLen, 1, fp))
			{
				free(ptr);
				return;
			}

			pJpeg = ptr;
			ulJpegSize = lLen;
			bNeedFree = TRUE;
		}
		else
		{
			LPCPAPERCOVERINFO i = m_pPaper->GetCoverInfo();
			if (i->lpData && i->ulDataSize)
			{
				pJpeg = i->lpData;
				ulJpegSize = i->ulDataSize;
			}
			else
			{
				return;
			}
			bNeedFree = FALSE;
		}

		HDC hDC = ::GetDC(GetSafeHwnd());
		ShowImage(hDC, &m_Bmp, ITEMIMAGETYPE_JPEG, pJpeg, ulJpegSize);
		::ReleaseDC(GetSafeHwnd(), hDC);
		if (bNeedFree)
		{
			free(pJpeg);
		}
	}
	else
	{
		return;
	}

	if (m_pImgFrame && IsWindow(m_pImgFrame->GetSafeHwnd()))
	{
		m_pImgFrame->ShowCover(&m_Bmp);
		m_pImgFrame->ShowWindow(SW_SHOW);
	}
}

void CJpegCoverDlg::OnSetBkColor() 
{
	if (!m_pPaper->CanEdit())
	{
		return;
	}

	CHOOSECOLOR clr;
	COLORREF acrCustClr[16];
	
	clr.lStructSize		= sizeof(CHOOSECOLOR);
	clr.hwndOwner		= GetSafeHwnd();
	clr.rgbResult		= m_clrBk;
	clr.lpCustColors	= acrCustClr;
	clr.Flags			= CC_RGBINIT;
	clr.lCustData		= 0;
	clr.lpfnHook		= NULL;
	clr.lpTemplateName	= NULL;
	
	if (ChooseColor(&clr))
	{
		m_clrBk = clr.rgbResult;
		m_cBkColor.SetColor(m_clrBk);
	}
}

void CJpegCoverDlg::OnSetTxtColor() 
{
	if (!m_pPaper->CanEdit())
	{
		return;
	}

	CHOOSECOLOR clr;
	COLORREF acrCustClr[16];
	
	clr.lStructSize		= sizeof(CHOOSECOLOR);
	clr.hwndOwner		= GetSafeHwnd();
	clr.rgbResult		= m_clrTxt;
	clr.lpCustColors	= acrCustClr;
	clr.Flags			= CC_RGBINIT;
	clr.lCustData		= 0;
	clr.lpfnHook		= NULL;
	clr.lpTemplateName	= NULL;
	
	if (ChooseColor(&clr))
	{
		m_clrTxt = clr.rgbResult;
		m_cTxtColor.SetColor(m_clrTxt);
	}
}

LPPAPERCOVERINFO CJpegCoverDlg::GetCoverInfo()
{
	CPaper::ClearCoverInfo(&m_inf);

	m_inf.nType = m_nRadioType;
	m_inf.nWidth = m_nWidth;
	m_inf.nHeight = m_nHeight;
	m_inf.clrBk = m_clrBk;
	m_inf.clrText = m_clrTxt;
	m_inf.nQuality = m_nQuatlity;
	m_inf.strTitle = _tcsdup(m_strTitle);
	m_inf.strJpegPath = _tcsdup(m_strJpegPath);
	m_inf.lpData = NULL;
	m_inf.ulDataSize = 0;

	return &m_inf;
}

void CJpegCoverDlg::OnRadio() 
{
	int n = m_nRadioType;

	UpdateData();
	if (n != m_nRadioType)
	{
		m_bModify = TRUE;
	}

	switch(m_nRadioType)
	{
	case PAPER_COVERTYPE_CUSTOM:
	case PAPER_COVERTYPE_DEFAULT:
		m_cBtnView.EnableWindow(TRUE);
		break;

	default:
		m_cBtnView.EnableWindow(FALSE);
	}
}

void CJpegCoverDlg::OnChangeEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CClrDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_bModify = TRUE;
}
