// AllBuildDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AllBuildDlg.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUILDJUDGE_TRUE		(TRUE + 1)
#define BUILDJUDGE_FALSE	(FALSE + 1)

enum {
	ABSEL_SCHOISE = 0,
	ABSEL_MCHOISE,
	ABSEL_JUDGE,
	ABSEL_BLANK,
	ABSEL_TEXT,
	ABSEL_CUSTOM
};

enum {
	GETLINES_SINGLE = 0,
	GETLINES_OPTIONS,
	GETLINES_INDEX,
};

/////////////////////////////////////////////////////////////////////////////
// CAllBuildDlg dialog

CAllBuildDlg::CAllBuildDlg(LPTSTR pText, int nType, CList *pAddList, CWnd* pParent /*=NULL*/) :
	CClrDialog(CAllBuildDlg::IDD, pParent),
	m_pText(pText), m_pWText(NULL), m_strWDescription(NULL), m_nWDesLen(0),
	m_nType(nType), m_pAddList(pAddList),
	m_nMakeAnswerCount(0),
	m_nStopPos(0)
{
	//{{AFX_DATA_INIT(CAllBuildDlg)
	m_bHandleIndex = TRUE;
	m_bAutoSkip = FALSE;
	m_bDetectAswInQ = FALSE;
	m_bDetectAswAfterQ = FALSE;
	m_bNumSep = TRUE;
	//}}AFX_DATA_INIT
}

CAllBuildDlg::~CAllBuildDlg()
{
#ifndef _UNICODE
	if (m_pWText)
	{
		free(m_pWText);
	}
#endif

	if (m_strWDescription)
	{
		free(m_strWDescription);
	}
}

void CAllBuildDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAllBuildDlg)
	DDX_Control(pDX, IDC_EDIT_ALLBUILDTIPS, m_cEditTips);
	DDX_Control(pDX, IDC_CHECK_NUMSEP, m_cBtnNumSep);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cCombo);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup);
	DDX_Control(pDX, IDC_CHECK_HANDLEINDEX, m_cBtnHandleIndex);
	DDX_Control(pDX, IDC_CHECK_DETECTASWAFTERQ, m_cBtnDetectAswAfterQ);
	DDX_Control(pDX, IDC_CHECK_DETECTASWINQ, m_cBtnDetectAswInQ);
	DDX_Control(pDX, IDC_CHECK_AUTOSKIP, m_cBtnAutoSkip);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_cBtnSave);
	DDX_Check(pDX, IDC_CHECK_HANDLEINDEX, m_bHandleIndex);
	DDX_Check(pDX, IDC_CHECK_AUTOSKIP, m_bAutoSkip);
	DDX_Check(pDX, IDC_CHECK_DETECTASWINQ, m_bDetectAswInQ);
	DDX_Check(pDX, IDC_CHECK_DETECTASWAFTERQ, m_bDetectAswAfterQ);
	DDX_Check(pDX, IDC_CHECK_NUMSEP, m_bNumSep);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAllBuildDlg, CClrDialog)
	//{{AFX_MSG_MAP(CAllBuildDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_CHECK_DETECTASWINQ, OnCheckDetectAswInQ)
	ON_BN_CLICKED(IDC_CHECK_DETECTASWAFTERQ, OnCheckDetectAswAfterQ)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllBuildDlg message handlers

BOOL CAllBuildDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	if (!m_nType || TYPE_GROUP == m_nType)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(0);
	}
	else
	{
		if (CItem::IsDefaultType(m_nType))
		{
			int nIndex;
			switch(m_nType)
			{
			case TYPE_SCHOISE:
				nIndex = ABSEL_SCHOISE;
				break;
				
			case TYPE_MCHOISE:
				nIndex = ABSEL_MCHOISE;
				break;
				
			case TYPE_JUDGE:
				nIndex = ABSEL_JUDGE;
				break;

			case TYPE_BLANK:
				nIndex = ABSEL_BLANK;
				break;
				
			case TYPE_TEXT:
				nIndex = ABSEL_TEXT;
				break;
				
			default:
				ASSERT(FALSE);
			}

			((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(nIndex);
		}
		else
		{
			((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->AddString(m_pAddList->GetDescription());
			((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->SetCurSel(ABSEL_CUSTOM);
		}

		GetDlgItem(IDC_COMBO_TYPE)->EnableWindow(FALSE);
		SetOptionsCheckBoxes();
	}

	CString strTips, strTips2;
	strTips.LoadString(IDS_EDITDLG_ALLBUILDTIPS);
	m_cEditTips.SetWindowText(strTips);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAllBuildDlg::OnButtonSave() 
{
	UpdateData();
	m_List.ClearAllItems(TRUE);

	m_nMakeAnswerCount = 0;
	MakeList();

	int i;
	CItem *pItem;
	for (i = 0; i < m_List.GetItemCount(); ++i)
	{
		pItem = m_List.GetItem(i);
		if (pItem->CheckItemValid())
		{
			m_nMakeAnswerCount++;
		}
	}

	CDialog::OnOK();
}

void CAllBuildDlg::OnSelChange() 
{
	SetOptionsCheckBoxes();
}

void CAllBuildDlg::SetOptionsCheckBoxes()
{
	UpdateData();

	switch(GetRealIndex())
	{
	case ABSEL_SCHOISE:
	case ABSEL_MCHOISE:
	case ABSEL_JUDGE:
		// ѡ���⡢�ж��⡢��������й�����Ч
		GetDlgItem(IDC_CHECK_DETECTASWINQ)->EnableWindow(TRUE);
		break;

	case ABSEL_BLANK:
		// �����������
		if (!m_bDetectAswInQ && !m_bDetectAswAfterQ)
		{
			m_bDetectAswInQ = TRUE;
		}
		break;

	case ABSEL_TEXT:
		// ����ⲻ��ѡ������Ŀ��Ѱ�Ҵ�
		m_bDetectAswInQ = FALSE;
		GetDlgItem(IDC_CHECK_DETECTASWINQ)->EnableWindow(FALSE);
		break;

	default:
		ASSERT(FALSE);
	}
	UpdateData(FALSE);
}

/*
���ܣ�	����������ɵ���Ŀ����
����ֵ�������б����
��ע��
*/
int CAllBuildDlg::GetRealIndex()
{
	if (CItem::IsDefaultType(m_nType))
	{
		return ((CComboBox*)GetDlgItem(IDC_COMBO_TYPE))->GetCurSel();
	}

	int nIndex = 0;
	
	switch(CItem::GetType(m_nType))
	{
	case TYPE_SCHOISE:
		nIndex = ABSEL_SCHOISE;
		break;
		
	case TYPE_MCHOISE:
		nIndex = ABSEL_MCHOISE;
		break;
		
	case TYPE_JUDGE:
		nIndex = ABSEL_JUDGE;
		break;
		
	case TYPE_BLANK:
		nIndex = ABSEL_BLANK;
		break;
		
	case TYPE_TEXT:
		nIndex = ABSEL_TEXT;
		break;
		
	default:
		ASSERT(FALSE);
	}
	
	return nIndex;
}

void CAllBuildDlg::MakeList()
{
	if (!m_pText)
	{
		return;
	}

	// ���ۺ��ֱ����ʽ������UNICODE����
#ifdef _UNICODE
	m_pWText = m_pText;
#else
	if (m_pWText)
	{
		free(m_pWText);
		m_pWText = NULL;
	}
	m_pWText = (LPWSTR)CTextManager::SwitchSave(m_pText, CS_UTF16, NULL);
	if (!m_pWText)
	{
		return;
	}
#endif

	size_t i = 0;
	int nGetType = 0;
	BOOL bMarkLastLine = FALSE;
	CItem *pItem = NULL;
	size_t uStart = 0, uLen = 0;
	BYTE btAnswer = 0;
	
	while(m_pWText[i])
	{
		// ����һ������
		switch(GetRealIndex())
		{
		case ABSEL_SCHOISE:
			pItem = new CChoise(TRUE);
			break;

		case ABSEL_MCHOISE:
			pItem = new CChoise(FALSE);
			break;

		case ABSEL_JUDGE:
			pItem = new CJudge;
			break;

		case ABSEL_BLANK:
			pItem = new CBlank;
			break;

		case ABSEL_TEXT:
			pItem = new CText;
			break;

		default:
			ASSERT(FALSE);
		}
		if (!pItem)
		{
			return;
		}
		// �������ɵ���Ŀ������ȫ������Ϊ���ַ�
		pItem->SetResolve(_T(""));

		// ��¼���⿪ʼ������λ��
		m_nStopPos = i;

		// �������
		if (m_bNumSep)
		{
			switch(pItem->GetType())
			{
			case TYPE_SCHOISE:
			case TYPE_MCHOISE:
				nGetType = GETLINES_OPTIONS;
				bMarkLastLine = FALSE;
				break;

			case TYPE_TEXT:
				nGetType = GETLINES_SINGLE;
				bMarkLastLine = FALSE;
				break;

			case TYPE_JUDGE:
			case TYPE_BLANK:
				nGetType = GETLINES_INDEX;
				bMarkLastLine = m_bDetectAswAfterQ;
				break;

			default:
				ASSERT(FALSE);
			}
		}
		else
		{
			nGetType = GETLINES_SINGLE;
			bMarkLastLine = FALSE;
		}
		if (!GetLines(nGetType, m_pWText + i, m_bDetectAswAfterQ, &uStart, &uLen))
		{
			delete pItem;
			if (!uLen)
			{
				break;
			}
			else if (m_bAutoSkip)
			{
				i += (uStart + uLen);
				continue;
			}
			else
			{
				return;
			}
		}
		i += uStart;

		if (m_nWDesLen < (uLen + 1) * (int)sizeof(WCHAR))
		{
			m_nWDesLen = (uLen + 1) * sizeof(WCHAR);
			m_strWDescription = (LPWSTR)realloc(m_strWDescription, m_nWDesLen);
		}

		// ���ȥ����ͷ�����
		if (m_bHandleIndex)
		{
			int n;
			CheckNumIndex(m_pWText + i, &n);
			wcscpyn(m_strWDescription, m_pWText + i + n, uLen + 1 - n);
		}
		else
		{
			wcscpyn(m_strWDescription, m_pWText + i, uLen + 1);
		}
		i += uLen;
		
		// ���ݲ�ͬ��Ŀ���ͽ����󲿷�
		// Ŀǰ����ѡ������Ҫ����
		switch(pItem->GetType())
		{
		// ѡ�������ѡ��
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
			if (m_bNumSep)
			{
				nGetType = GETLINES_INDEX;
				bMarkLastLine = m_bDetectAswAfterQ;
			}
			else
			{
				nGetType = GETLINES_SINGLE;
				bMarkLastLine = FALSE;
			}
			if (GetLines(nGetType, m_pWText + i, bMarkLastLine, &uStart, &uLen))
			{
				BOOL b = ((CChoise*)pItem)->AutoOptions(m_pWText + i + uStart, uLen);
				i += (uStart + uLen);
				if (!b)
				{
					delete pItem;
					if (m_bAutoSkip)
					{
						continue;
					}
					else
					{
						return;
					}
				}
			}
			else
			{
				delete pItem;
				if (m_bAutoSkip)
				{
					continue;
				}
				else
				{
					return;
				}
			}
			break;

		default:
			break;
		}
		
		// ����Ŀ��Ѱ�Ҵ𰸣�����Ҫ�ƶ�i��
		if (m_bDetectAswInQ)
		{
			switch(pItem->GetType())
			{
			case TYPE_BLANK:
				DetectTextAnswerInQ(pItem, m_strWDescription);
				break;

			case TYPE_SCHOISE:
			case TYPE_MCHOISE:
			case TYPE_JUDGE:
				DetectU32AnswerInQ(pItem, m_strWDescription);
				break;

			default:
				ASSERT(FALSE);
			}
		}
		// ����Ŀ����һ��Ѱ�Ҵ𰸣����ɹ������ƶ�i��
		else if (m_bDetectAswAfterQ)
		{
			if (pItem->GetType() == TYPE_TEXT)
			{
				if (m_bNumSep)
				{
					nGetType = GETLINES_INDEX;
				}
				else
				{
					nGetType = GETLINES_SINGLE;
				}
			}
			else
			{
				nGetType = GETLINES_SINGLE;
			}
			// ��һ�н���������ʱ����Ȼ����
			if (GetLines(nGetType, m_pWText + i, FALSE, &uStart, &uLen))
			{
				switch(pItem->GetType())
				{
				case TYPE_BLANK:
					// ��������һ����ԭ���Ǹ�����ⴴ������
					if (!DetectTextAnswerInQ(pItem, m_strWDescription, TRUE))
					{
						delete pItem;
						if (m_bAutoSkip)
						{
							i += (uStart + uLen);
							continue;
						}
						else
						{
							return;
						}
					}
				case TYPE_TEXT:
					DetectTextAnswerAfterQ(pItem, m_pWText + i + uStart, uLen);
					break;
					
				case TYPE_SCHOISE:
				case TYPE_MCHOISE:
				case TYPE_JUDGE:
					DetectU32AnswerAfterQ(pItem, m_pWText + i + uStart, uLen);
					break;
				}

				i += (uStart + uLen);
			}
			else
			{
				delete pItem;
				if (m_bAutoSkip)
				{
					continue;
				}
				else
				{
					return;
				}
			}
		}

		if (pItem->GetType() != TYPE_BLANK)
		{
#ifdef _UNICODE
			if (!pItem->SetDescription(m_strWDescription))
#else
			if (!pItem->SetWDescription(m_strWDescription))
#endif
			{
				delete pItem;
				if (m_bAutoSkip)
				{
					continue;
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			if (!((CBlank*)pItem)->CheckItemValid())
			{
				delete pItem;
				if (m_bAutoSkip)
				{
					continue;
				}
				else
				{
					return;
				}
			}
		}

		m_List.AddItem(pItem);
	}

	m_nStopPos = 0;
}

/*
���ܣ�	������ͷ�Ļس��Ϳո�
����ֵ������������
��ע��
*/
int CAllBuildDlg::JumpSpaces(LPCWSTR pWText)
{
	int i = 0;
	BOOL bOut = FALSE;

	while(pWText[i] && !bOut)
	{
		switch(pWText[i])
		{
		CASE_RETURN:
		CASE_SPACE:
			break;
		
		default:
			bOut = TRUE;
			continue;
		}
		++i;
	}

	return i;
}

/*
���ܣ�	���ַ����л�ȡ�����У�����¼��ʼλ���볤��
����ֵ��ȡ�õ�����
��ע��	ȡ�õ�����������������Ч���ֵ��������������Ļ��в����㣩
		bMarkLastLineָ����ȥ�����һ�У������չ������ȡ��һ��ʱ���˲�����Ч
*/
int CAllBuildDlg::GetLines(int nType, LPCWSTR pWText, BOOL bMarkLastLine, size_t *uStart, size_t *uLen)
{
	if (!pWText)
	{
		return 0;
	}
	ASSERT(uStart && uLen);

	size_t i = JumpSpaces(pWText);
	BOOL bOut = FALSE, bValid = FALSE;
	int n = 0, nRetCount = 0;

	// �ų���ͷ�Ŀհ��뻻�з�������ʽ�ַ���ʼ
	*uStart = i;

	bOut = FALSE;
	// ѡȡָ������������
	while(pWText[i] && !bOut)
	{
		switch(pWText[i])
		{
		CASE_RETURN:
			if (bValid)
			{
				++nRetCount;
				bValid = FALSE;
			}
			switch(nType)
			{
			// ֻȡһ��
			case GETLINES_SINGLE:
				bOut = TRUE;
				continue;

			// Ѱ��ѡ�ͷ
			case GETLINES_OPTIONS:
				if (CheckOptionTitle(pWText + i, &n))
				{
					bOut = TRUE;
				}
				else
				{
					i += n;
				}
				continue;

			// Ѱ�ұ�ſ�ͷ
			case GETLINES_INDEX:
				if (CheckNumIndex(pWText + i, &n))
				{
					bOut = TRUE;
				}
				else
				{
					i += n;
				}
				continue;
			}
			break;

		default:
			bValid = TRUE;
			break;
		}

		++i;
	}
	if (i <= *uStart)
	{
		*uLen = 0;
		return 0;
	}
	if (bValid)
	{
		++nRetCount;
	}

	// �ų������һ��
	if (bMarkLastLine && nRetCount >= 2)
	{
		BOOL bFindRet = FALSE, bValid = FALSE;;
		bOut = FALSE;

		--i;
		while(i > *uStart && !bOut)
		{
			switch(pWText[i])
			{
			CASE_RETURN:
				if (bValid)
				{
					bFindRet = TRUE;
				}
				break;

			CASE_SPACE:
				break;

			default:
				bValid = TRUE;
				if (bFindRet)
				{
					++i;
					--nRetCount;
					bOut = TRUE;
					continue;
				}
				break;
			}

			--i;
		}
	}

	*uLen = (i - *uStart);
	return nRetCount;
}

/*
���ܣ�	�����һ�п�ͷ�Ƿ�Ϊ��Ŀ���
����ֵ������Ŀ����򷵻�TRUE�������򷵻�FALSE
��ע��	������Ŀ���ʱ��nJump���ش�����Ŀ��ų��ȵ���������
		��������Ŀ���ʱ��nJump���ز�����Ŀ��ų��ȵ���������
*/
BOOL CAllBuildDlg::CheckNumIndex(LPCWSTR pWText, int *nJump)
{
	int i = JumpSpaces(pWText);
	BOOL bFindIndex = FALSE, bIndexEnd = FALSE;
	int nFirstValid = -1;

	// ������ͷ�Ŀհ�
	ASSERT(nJump);
	*nJump = i;

	while(pWText[i])
	{
		switch(pWText[i])
		{
		CASE_SPACE:
		CASE_POINT:
		CASE_RIGHTBRACE:
			if (bFindIndex)
			{
				bIndexEnd = TRUE;
			}
			break;

		default:
			if ( pWText[i] >= _L('0') && pWText[i] <= _L('9') ||
				(pWText[i] >= _L('��') && pWText[i] <= _L('��')))
			{
				bFindIndex = TRUE;
			}
			else
			{
				if (bIndexEnd)
				{
					*nJump = i;
					return TRUE;
				}
				else
				{
					return FALSE;
				}
			}
		}

		++i;
	}

	return FALSE;
}

/*
���ܣ�	���ѡ�����ѡ���
����ֵ�������ѡ��ţ�����TRUE�����򷵻�FALSE
��ע��	nJump�������ؽ����ִ���ͷ�Ļ�����ո񳤶�
		��������ѡ���ʱ��nJump�Żḳֵ������nJump�����Ķ�
*/
BOOL CAllBuildDlg::CheckOptionTitle(LPCWSTR pWText, int *nJump)
{
	int i = JumpSpaces(pWText);
	int nRetCount = 0;
	BOOL bFindTitle = FALSE;

	// ������ͷ�Ŀհ�
	ASSERT(nJump);
	*nJump = i;

	while(pWText[i])
	{
		switch(pWText[i])
		{
		CASE_SPACE:
		CASE_RETURN:
			// �����հ��ַ�ֱ������
			break;
			
		CASE_LEFTBRACE:
			// ѡ��ź������������ţ���Ȼ��Ϊ����
			if (bFindTitle)
			{
				return FALSE;
			}
			break;

		CASE_RIGHTBRACE:
		CASE_POINT:
			if (bFindTitle)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
			break;
			
		default:
			if ((pWText[i] >= _L('A') && pWText[i] <= _L('H')) ||
				(pWText[i] >= _L('a') && pWText[i] <= _L('h')) ||
				(pWText[i] >= _L('��') && pWText[i] <= _L('��')) ||
				(pWText[i] >= _L('��') && pWText[i] <= _L('��')))
			{
				bFindTitle = TRUE;
			}
			else if (pWText[i] >= _L('1') && pWText[i] <= _L('8') ||
					(pWText[i] >= _L('��') && pWText[i] <= _L('��')))
			{
				// �����������У������������֣�����Ϊ����Ϊѡ�ʼ
				if (m_bNumSep)
				{
					return FALSE;
				}
				else
				{
					bFindTitle = TRUE;
				}
			}
			else
			{
				return FALSE;
			}
		}

		++i;
	}
	
	return FALSE;
}

/*
���ܣ�	����ַ����������ڵ����ݣ�����Ϊ�𰸱���
����ֵ�����ؼ�⵽�𰸵�����
��ע��	�����������ɹ������޸��ַ������ݣ�������������
*/
int CAllBuildDlg::CheckQuote(int nType, LPWSTR pWText, size_t uLen, DWORD *dwAnswer)
{
	if (!pWText || !uLen)
	{
		return 0;
	}

	int nCount = 0;
	size_t i = 0;
	if (nType == TYPE_SCHOISE || nType == TYPE_MCHOISE)
	{
		*dwAnswer = 0;
		while(pWText[i] && i < uLen)
		{
			if (pWText[i] >= _L('A') && pWText[i] <= _L('H'))
			{
				*dwAnswer |= (0x80 >> (pWText[i] - _L('A')));
				++nCount;
			}
			else if (pWText[i] >= _L('a') && pWText[i] <= _L('h'))
			{
				*dwAnswer |= (0x80 >> (pWText[i] - _L('a')));
				++nCount;
			}
			else if (pWText[i] >= _L('��') && pWText[i] <= _L('��'))
			{
				*dwAnswer |= (0x80 >> (pWText[i] - _L('��')));
				++nCount;
			}
			else if (pWText[i] >= _L('��') && pWText[i] <= _L('��'))
			{
				*dwAnswer |= (0x80 >> (pWText[i] - _L('��')));
				++nCount;
			}
			else
			{
				switch(pWText[i])
				{
				CASE_SPACE:
				CASE_RETURN:
				CASE_LEFTBRACE:
				CASE_RIGHTBRACE:
					break;

				default:
					*dwAnswer = 0;
					return 0;
				}
			}

			++i;
		}

		if (!*dwAnswer)
		{
			return 0;
		}
		else if (nCount > 1 && TYPE_SCHOISE == nType)
		{
			return nCount;
		}
	}
	else if (nType == TYPE_JUDGE)
	{
		BYTE btStore = 0xFF;
		while(pWText[i] && i < uLen)
		{
			if (pWText[i] == _L('��') || pWText[i] == _L('��') ||
				pWText[i] == _L('V') || pWText[i] == _L('v'))
			{
				btStore = TRUE;
			}
			else if (pWText[i] == _L('��') || pWText[i] == _L('��') ||
					 pWText[i] == _L('X') || pWText[i] == _L('x'))
			{
				btStore = FALSE;
			}
			else if (pWText[i] == _L('��'))
			{
				if (pWText[i + 1] == _L('ȷ'))
				{
					i++;
					btStore = TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			else if (pWText[i] == _L('��'))
			{
				if (pWText[i - 1] == _L('��'))
				{
					btStore = FALSE;
				}
				else
				{
					return FALSE;
				}
			}
			else if (pWText[i] == _L('T') || pWText[i] == _L('t'))
			{
				btStore = TRUE;
				if (!wcsnicmp(pWText + i, _L("true"), 4))
				{
					i += 4;
					continue;
				}
			}
			else if (pWText[i] == _L('F') || pWText[i] == _L('f'))
			{
				btStore = FALSE;
				if (!wcsnicmp(pWText + i, _L("false"), 5))
				{
					i += 5;
					continue;
				}
			}
			else if (pWText[i] == _L('Y') || pWText[i] == _L('y'))
			{
				btStore = TRUE;
				if (!wcsnicmp(pWText + i, _L("yes"), 3))
				{
					i += 3;
					continue;
				}
			}
			else if (pWText[i] == _L('N') || pWText[i] == _L('n'))
			{
				btStore = FALSE;
				if (!wcsnicmp(pWText + i, _L("no"), 2))
				{
					i += 2;
					continue;
				}
			}
			else if (pWText[i] == _L('R') || pWText[i] == _L('r'))
			{
				btStore = TRUE;
				if (!wcsnicmp(pWText + i, _L("right"), 5))
				{
					i += 5;
					continue;
				}
			}
			else if (pWText[i] == _L('W') || pWText[i] == _L('w'))
			{
				btStore = FALSE;
				if (!wcsnicmp(pWText + i, _L("wrong"), 5))
				{
					i += 5;
					continue;
				}
			}
			else
			{
				switch(pWText[i])
				{
				CASE_SPACE:
				CASE_RETURN:
				CASE_LEFTBRACE:
				CASE_RIGHTBRACE:
					break;

				default:
					return FALSE;
				}
			}
			
			++i;
		}

		if (btStore > TRUE)
		{
			return 0;
		}
		else
		{
			*dwAnswer = btStore;
			nCount = 1;
		}
	}

	// �����ѡ��ĺۼ�
	for (i = 0; i < uLen; i++)
	{
		pWText[i] = _L(' ');
	}

	return nCount;
}

BOOL CAllBuildDlg::DetectU32AnswerInQ(CItem *pItem, LPWSTR strWDescription)
{
	if (!pItem || !strWDescription)
	{
		return FALSE;
	}

	DWORD dwAnswer = 0;
	int nType = pItem->GetType();
	int nStart = -1;
	int i = 0;

	// ���Ƚ������ҵ�
	while(strWDescription[i])
	{
		switch(strWDescription[i])
		{
		CASE_LEFTBRACE:
			if (nStart < 0)
			{
				nStart = i + 1;
			}
			break;

		CASE_RIGHTBRACE:
			if (nStart >= 0)
			{
				// ʶ����������е�������
				BOOL bOut = FALSE, nLast = i;
				while(!bOut)
				{
					switch(strWDescription[i])
					{
					CASE_RIGHTBRACE:
						nLast = i;
						++i;
						break;

					CASE_SPACE:
					CASE_RETURN:
						++i;
						break;

					default:
						bOut = TRUE;
						i = nLast;
					}
				}

				int n = CheckQuote(nType, strWDescription + nStart, i - nStart, &dwAnswer);
				switch(nType)
				{
				case TYPE_MCHOISE:
				case TYPE_JUDGE:
					pItem->SetU32Answer(dwAnswer);
					return TRUE;

				case TYPE_SCHOISE:
					if (n <= 1)
					{
						pItem->SetU32Answer(dwAnswer);
						return TRUE;
					}
				}

				nStart = -1;
			}
			break;

		default:
			break;
		}

		i++;
	}

	// ����Ҳ������ţ��ʹ���Ŀĩβ��ʼ��
	// Ҫ�������Ŀ֮��������һ���ո����
	BOOL bOut = FALSE;
	BOOL bFindValid = FALSE;
	nStart = wcslen(strWDescription);
	i = nStart - 1;
	while(i > 0 && !bOut)
	{
		switch(strWDescription[i])
		{
		CASE_SPACE:
			if (bFindValid)
			{
				bOut = TRUE;
				++i;
				continue;
			}
			break;

		default:
			bFindValid = TRUE;
			break;
		}

		--i;
	}
	
	int n = CheckQuote(nType, strWDescription + i, nStart - i + 1, &dwAnswer);
	switch(nType)
	{
	case TYPE_MCHOISE:
	case TYPE_JUDGE:
		pItem->SetU32Answer(dwAnswer);
		return TRUE;

	case TYPE_SCHOISE:
		if (n <= 1)
		{
			pItem->SetU32Answer(dwAnswer);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CAllBuildDlg::DetectU32AnswerAfterQ(CItem *pItem, LPWSTR pWText, size_t uLen)
{
	if (!pItem || !pWText || !uLen)
	{
		return FALSE;
	}
	
	size_t i = 0;
	BOOL bOut = FALSE;
	// ���ȼ�������е�ð�Ż�ո�
	// Ҫ���д��ð�ź��ո��
	while(pWText[i] && !bOut)
	{
		switch(pWText[i])
		{
		case _L(':'):
		case _L('��'):
		CASE_SPACE:
			bOut = TRUE;
			break;
			
		default:
			break;
		}
		++i;
	}
	
	DWORD dwAnswer = 0;
	// �ҵ���ð�Ż�ո�
	if (bOut)
	{
		if (CheckQuote(pItem->GetType(), pWText + i, uLen - i + 1, &dwAnswer))
		{
			pItem->SetU32Answer(dwAnswer);
			return TRUE;
		}
		// �����ð�ź��޷�������Ҳ����������
	}
	
	if (!CheckQuote(pItem->GetType(), pWText, uLen, &dwAnswer))
	{
		return FALSE;
	}
	pItem->SetU32Answer(dwAnswer);
	return TRUE;
}

BOOL CAllBuildDlg::DetectTextAnswerInQ(CItem *pItem, LPWSTR strWDescription, BOOL bDefaultBlank /* = FALSE */)
{
	ASSERT(pItem->GetType() == TYPE_BLANK);
	ASSERT(pItem && strWDescription);

	int i;
#ifdef _UNICODE
	if (!pItem->SetDescription(strWDescription))
	{
		return FALSE;
	}
#else
	if (!pItem->SetWDescription(strWDescription))
	{
		return FALSE;
	}
#endif

	int nEnd = -1;
	// �Ӻ���ǰ���в��ң����ƻ����
	for (i = wcslen(strWDescription) - 1; i >= 0; i--)
	{
		switch(strWDescription[i])
		{
		CASE_RIGHTBRACE:
			if (nEnd < 0)
			{
				nEnd = i;
			}
			break;

		CASE_LEFTBRACE:
			if (nEnd >= 0)
			{
				((CBlank*)pItem)->AddBlank(i + 1, nEnd, bDefaultBlank ? _T("N") : _T(""));
				nEnd = -1;
			}
			break;

		default:
			break;
		}
	}

	if (((CBlank*)pItem)->GetBlankCount() < 1)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CAllBuildDlg::DetectTextAnswerAfterQ(CItem *pItem, LPCWSTR pWText, size_t uLen)
{
	if (!pItem || !pWText || !uLen)
	{
		return FALSE;
	}

	size_t i = 0;
	BOOL bOut = FALSE;
	if (pItem->GetType() == TYPE_BLANK)
	{
		CBlank *pBlank = (CBlank*)pItem;
		int nStart = -1;
		size_t uCount = 0;

		// ���ȼ�������е�ð��
		// Ҫ���д��ð�ź��ո��
		BOOL bFindText = FALSE;
		while(pWText[i] && !bOut)
		{
			switch(pWText[i])
			{
			case _L(':'):
			case _L('��'):
			CASE_SPACE:
				bFindText = TRUE;
				break;
				
			default:
				if (bFindText)
				{
					bOut = TRUE;
					continue;
				}
				break;
			}
			++i;
		}

		// ���������Ϊ�˼��ٴ�����
		// ���ȼ��ð�ź�������Ƿ��㹻���㹻���ټ��
		// ���ð�ź����ݲ��������ͷ��ʼ
		int nRow = 2;
		while(nRow > 0)
		{
			if (!bOut)
			{
				--nRow;
				i = 0;
			}

			bOut = FALSE;
			while(!bOut)
			{
				switch(pWText[i])
				{
					// ����֮�������·��Ž��зָ�
				case 0:
				CASE_SPACE:
				case _L(','):
				case _L('.'):
				case _L(';'):
				case _L('��'):
				case _L('��'):
				case _L('��'):
				case _L('��'):
					if (nStart >= 0)
					{
						// �趨��Ӧ�Ŀհ�
#ifdef _UNICODE
						pBlank->EditBlank(uCount, pWText + nStart, i - nStart);
#else
						pBlank->EditBlankW(uCount, pWText + nStart, i - nStart);
#endif
						nStart = -1;
						uCount++;
						if (uCount >= pBlank->GetBlankCount())
						{
							return TRUE;
						}
					}
					break;
					
				default:
					nStart = (nStart < 0) ? i : nStart;
					break;
				}
				
				++i;
			}

			bOut = FALSE;
			--nRow;
		}
	}
	else if (pItem->GetType() == TYPE_TEXT)
	{
#ifdef _UNICODE
		if (!((CText*)pItem)->SetAnswer(pWText, uLen))
		{
			delete pItem;
			return FALSE;
		}
#else
		if (!((CText*)pItem)->SetAnswerW(pWText, uLen))
		{
			delete pItem;
			return FALSE;
		}
#endif
	}
	else
	{
		ASSERT(FALSE);
	}

	return TRUE;
}

void CAllBuildDlg::OnCheckDetectAswInQ() 
{
	if (GetRealIndex() == ABSEL_BLANK)
	{
		if (m_cBtnDetectAswInQ.GetCheck())
		{
			m_cBtnDetectAswAfterQ.SetCheck(FALSE);
		}
		else
		{
			m_cBtnDetectAswAfterQ.SetCheck(TRUE);
		}
	}
	else
	{
		m_cBtnDetectAswAfterQ.SetCheck(FALSE);
	}
}

void CAllBuildDlg::OnCheckDetectAswAfterQ() 
{
	if (GetRealIndex() == ABSEL_BLANK)
	{
		if (m_cBtnDetectAswAfterQ.GetCheck())
		{
			m_cBtnDetectAswInQ.SetCheck(FALSE);
		}
		else
		{
			m_cBtnDetectAswInQ.SetCheck(TRUE);
		}
	}
	else
	{
		m_cBtnDetectAswInQ.SetCheck(FALSE);
	}
}
