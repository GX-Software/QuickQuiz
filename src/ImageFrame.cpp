// ImageFrame.cpp : implementation file
//

#include "stdafx.h"
#include "ImageFrame.h"
#include "ImageManager.h"
#include "DataList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPCTSTR classname = _T("QQImageFrame");

#define IMAGEFRM_NOIMAGEWIDTH		300
#define IMAGEFRM_NOIMAGEHEIGHT		300

enum DrawType {
	DRAWTYPE_COVER = 0
};

/////////////////////////////////////////////////////////////////////////////
// CImageFrame

CImageFrame::CImageFrame() :
	m_bShowBar(TRUE),
	m_pImageList(NULL), m_nCurShow(0),
	m_bModify(FALSE)
{
}

CImageFrame::~CImageFrame()
{
}

BEGIN_MESSAGE_MAP(CImageFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CImageFrame)
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_IMAGEBARNOTIFY, ImageBarNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageFrame message handlers

BOOL CImageFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// 创建一个没有图标的框架类
	WNDCLASS wndcls;
	ZeroMemory(&wndcls, sizeof(WNDCLASS));
	
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = NULL;
	wndcls.hCursor = afxData.hcurArrow;
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = classname;
	
	VERIFY(AfxRegisterClass(&wndcls));
	
	cs.lpszClass = classname;
	cs.dwExStyle |= WS_EX_DLGMODALFRAME;
	return TRUE;
}

BOOL CImageFrame::CreateFrame(LPCTSTR lpszWindowName,
							  CWnd* pParentWnd /* = NULL */,
							  const RECT& rect /* = rectDefault */,
							  DWORD dwExStyle /* = 0 */)
{
	// 禁止缩放
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;

	return CreateEx(dwExStyle, classname, lpszWindowName, dwStyle, rect, pParentWnd, NULL);
}

int CImageFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_cBar.CreateBar(this, IDD_IMAGESET_BAR))
	{
		return -1;
	}
	
	return 0;
}

void CImageFrame::OnClose() 
{
	Save();
	ShowWindow(SW_HIDE);
}

void CImageFrame::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rc;
	GetClientRect(&rc);
	dc.FillSolidRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, AfxGetQColor(QCOLOR_EDITFACE));
	
	int x = (rc.right - rc.left - m_Bmp.GetWidth()) / 2;
	int y = rc.bottom - rc.top - m_Bmp.GetHeight();
	if (m_bShowBar)
	{
		y -= m_cBar.GetDefaultHeight();
	}
	y /= 2;
	m_Bmp.Draw(dc.GetSafeHdc(), x, y);
}

BOOL CImageFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (m_cBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CImageFrame::ShowCover(CMemBmp *pBmp)
{
	m_Bmp.CopyBmp(pBmp);
	
	m_bShowBar = FALSE;
	m_cBar.ShowWindow(SW_HIDE);
	
	RECT rc = {0, 0, m_Bmp.GetWidth(), m_Bmp.GetHeight()};
	::AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
	SetWindowPos(NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
	
	Invalidate(FALSE);
}

void CImageFrame::SetShow()
{
	ASSERT(IsWindow(m_cBar.GetSafeHwnd()) && m_bShowBar);

	if (!m_pImageList)
	{
		m_cBar.SetStatus(0, 0, 0, NULL);
		DrawInitial(IDS_IMGFRM_NOIMAGE);
		return;
	}

	PITEMIMAGE pImage = (PITEMIMAGE)m_pImageList->GetData(m_nCurShow);
	if (!pImage)
	{
		m_cBar.SetStatus(0, 0, m_pImageList->GetLimitCount(), NULL);
		DrawInitial(IDS_IMGFRM_NOIMAGE);
		return;
	}

	m_cBar.SetStatus(m_pImageList->GetDataCount(), m_nCurShow, m_pImageList->GetLimitCount(), pImage->szImageTitle);

	DrawImage(pImage);
}

void CImageFrame::SetImageList(CDataList *pList, int nInitShow)
{
	ASSERT(IsWindow(m_cBar.GetSafeHwnd()));

	if (m_pImageList)
	{
		Save();
	}

	m_pImageList = pList;
	m_nCurShow = nInitShow;
	m_bModify = FALSE;

	if (m_pImageList && m_pImageList->GetDataCount())
	{
		m_bShowBar = TRUE;
		m_cBar.ShowWindow(SW_SHOW);
	}
	
	SetShow();
}

BOOL CImageFrame::IsModified()
{
	return (m_cBar.IsModified() || m_bModify);
}

LRESULT CImageFrame::ImageBarNotify(WPARAM wParam, LPARAM lParam)
{
	if (lParam)
	{
		CItem::SetItemImageTitle(m_pImageList, m_nCurShow, (LPCTSTR)lParam);
		m_bModify = TRUE;
	}

	int nLoad = -1;
	switch(wParam)
	{
	case IMAGEBARNOTIFY_PRE:
		--m_nCurShow;
		break;

	case IMAGEBARNOTIFY_NEXT:
		++m_nCurShow;
		break;

	case IMAGEBARNOTIFY_NEW:
		nLoad = LoadImage(TRUE);
		break;

	case IMAGEBARNOTIFY_SET:
		nLoad = LoadImage(FALSE);
		break;

	case IMAGEBARNOTIFY_DELETE:
		nLoad = DeleteImage();
		break;

	default:
		ASSERT(FALSE);
	}

	if (nLoad > 0)
	{
		m_bModify = TRUE;
	}
	
	SetShow();
	return 0;
}

void CImageFrame::DrawImage(PITEMIMAGE pImage)
{
	HDC hDC = ::GetDC(GetSafeHwnd());
	if (!ShowImage(hDC, &m_Bmp, pImage->nImageType, pImage->pImageData, pImage->ulImageSize))
	{
		::ReleaseDC(GetSafeHwnd(), hDC);
		SetRedraw(TRUE);
		DrawInitial(IDS_IMGFRM_INVALIDIMG);
		return;
	}

	::ReleaseDC(GetSafeHwnd(), hDC);

	RECT rc = {0, 0, m_Bmp.GetWidth(), m_Bmp.GetHeight()};
	rc.bottom += m_cBar.GetDefaultHeight();
	rc.right = max(rc.right, m_cBar.GetDefaultWidth());

	::AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
	SetWindowPos(NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
	
	Invalidate(FALSE);
}

void CImageFrame::DrawInitial(UINT uID)
{
	RECT rc = {0, 0, IMAGEFRM_NOIMAGEWIDTH, IMAGEFRM_NOIMAGEHEIGHT};
	if (m_bShowBar)
	{
		rc.bottom += m_cBar.GetDefaultHeight();
		rc.right = max(rc.right, m_cBar.GetDefaultWidth());
	}
	::AdjustWindowRectEx(&rc, GetStyle(), FALSE, GetExStyle());
	SetWindowPos(NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE);
	
	LOGFONT font;
	HDC hDC = ::GetDC(GetSafeHwnd());
	HDC hBmpDC = m_Bmp.CreateBmp(hDC, IMAGEFRM_NOIMAGEWIDTH, IMAGEFRM_NOIMAGEHEIGHT, TRUE, AfxGetQColor(QCOLOR_EDITFACE));
	if (!hBmpDC)
	{
		::ReleaseDC(GetSafeHwnd(), hDC);
		return;
	}
	::ReleaseDC(GetSafeHwnd(), hDC);
	
	COLORREF oldClr = ::SetTextColor(hBmpDC, AfxGetQColor(QCOLOR_TEXT));
	int oldMode = ::SetBkMode(hBmpDC, TRANSPARENT);
	HFONT hFont, hOldFont;
	UINT uOldAlign;
	
	if (!GetObject(g_hNormalFont, sizeof(LOGFONT), &font))
	{
		return;
	}
	hFont = CreateFontIndirect(&font);
	if (!hFont)
	{
		return;
	}
	
	hOldFont = (HFONT)::SelectObject(hBmpDC, hFont);
	uOldAlign = ::SetTextAlign(hBmpDC, TA_CENTER);
	
	CString string;
	string.LoadString(uID);
	
	int nFit = 0;
	SIZE size;
	if (!GetTextExtentExPoint(hBmpDC, string, string.GetLength(), IMAGEFRM_NOIMAGEWIDTH, &nFit, NULL, &size))
	{
		return;
	}
	TextOut(hBmpDC, IMAGEFRM_NOIMAGEWIDTH / 2, (IMAGEFRM_NOIMAGEHEIGHT - size.cy) / 2, string, nFit);
	
	::SetTextAlign(hBmpDC, uOldAlign);
	::SetTextColor(hBmpDC, oldClr);
	::SetBkMode(hBmpDC, oldMode);
	::DeleteObject(::SelectObject(hBmpDC, hOldFont));
	
	Invalidate(FALSE);
}

BOOL CImageFrame::LoadImage(BOOL bAdd)
{
	OPENFILENAME ofn;
	TCHAR tmpFilePath[MAX_PATH] = {0};
	
	CString strTitle;
	strTitle.LoadString(IDS_IMGFRM_LOADTITLE);
	
	LPTSTR strFilter = CTextManager::LoadFileFilter(IDS_IMGFRM_LOADFILTER);
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_IMGFRM_READFAIL);
		return FALSE;
	}
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = GetSafeHwnd();
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = strFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = tmpFilePath;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = strTitle;
	ofn.Flags             = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = NULL;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
	
	if (!GetOpenFileName(&ofn))
	{
		free(strFilter);
		return FALSE;
	}
	free(strFilter);

	FILE *fp = _tfopen(tmpFilePath, _T("rb"));
	if (!fp)
	{
		return FALSE;
	}
	
	fseek(fp, 0, SEEK_END);
	long lDataLen = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	PBYTE pImage = (PBYTE)malloc(lDataLen);
	if (!pImage)
	{
		return FALSE;
	}
	if (!fread(pImage, lDataLen, 1, fp))
	{
		free(pImage);
		return FALSE;
	}

	int nImageType;
	if (!_tcsicmp(tmpFilePath + ofn.nFileExtension, _T("jpg")) ||
		!_tcsicmp(tmpFilePath + ofn.nFileExtension, _T("jpeg")))
	{
		nImageType = ITEMIMAGETYPE_JPEG;
	}
	else if (!_tcsicmp(tmpFilePath + ofn.nFileExtension, _T("png")))
	{
		nImageType = ITEMIMAGETYPE_PNG;
	}
	else
	{
		nImageType = ITEMIMAGETYPE_DIB;
	}

	SIZE sz = {0};
	if (!ReadImageSize(nImageType, pImage, lDataLen, &sz))
	{
		free(pImage);
		return FALSE;
	}
	if (sz.cx > ITEMIMAGESIZE_MAXX ||
		sz.cy > ITEMIMAGESIZE_MAXY)
	{
		free(pImage);

		CTMessageBox Box;
		Box.SuperMessageBox(GetSafeHwnd(), IDS_IMGFRM_TOOBIG, IDS_MSGCAPTION_WARNING, MB_OK | MB_ICONWARNING, 0, 0,
			ITEMIMAGESIZE_MAXX, ITEMIMAGESIZE_MAXY);
		return FALSE;
	}	

	ASSERT(m_pImageList);
	if (bAdd)
	{
		if (!CItem::SetItemImage(m_pImageList, -1, nImageType, pImage, lDataLen))
		{
			free(pImage);
			return FALSE;
		}
		m_nCurShow = m_pImageList->GetDataCount() - 1;
	}
	else
	{
		m_pImageList->DeleteData(m_nCurShow);
		if (!CItem::SetItemImage(m_pImageList, m_nCurShow, nImageType, pImage, lDataLen))
		{
			free(pImage);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CImageFrame::DeleteImage()
{
	CTMessageBox Box;
	if (IDYES != Box.SuperMessageBox(GetSafeHwnd(), IDS_IMGFRM_DELASK, IDS_MSGCAPTION_ASK, MB_YESNO | MB_ICONQUESTION, 0, 0))
	{
		return FALSE;
	}

	ASSERT(m_pImageList);
	m_pImageList->DeleteData(m_nCurShow);
	if (m_nCurShow >= m_pImageList->GetDataCount())
	{
		--m_nCurShow;
	}
	return TRUE;
}

void CImageFrame::Save()
{
	if (!m_pImageList)
	{
		return;
	}

	CString string;
	m_cBar.GetImageTitle(string);
	CItem::SetItemImageTitle(m_pImageList, m_nCurShow, string);
}
