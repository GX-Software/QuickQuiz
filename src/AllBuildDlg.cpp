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
		// 选择题、判断题、填空题所有功能生效
		GetDlgItem(IDC_CHECK_DETECTASWINQ)->EnableWindow(TRUE);
		break;

	case ABSEL_BLANK:
		// 填空题必须检测答案
		if (!m_bDetectAswInQ && !m_bDetectAswAfterQ)
		{
			m_bDetectAswInQ = TRUE;
		}
		break;

	case ABSEL_TEXT:
		// 简答题不能选择在题目中寻找答案
		m_bDetectAswInQ = FALSE;
		GetDlgItem(IDC_CHECK_DETECTASWINQ)->EnableWindow(FALSE);
		break;

	default:
		ASSERT(FALSE);
	}
	UpdateData(FALSE);
}

/*
功能：	获得批量生成的题目类型
返回值：返回列表序号
备注：
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

	// 不论何种编码格式，均按UNICODE解析
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
		// 创建一个对象
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
		// 批量生成的题目，解析全部设置为空字符
		pItem->SetResolve(_T(""));

		// 记录本题开始解析的位置
		m_nStopPos = i;

		// 解析题干
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

		// 如果去掉开头的题号
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
		
		// 根据不同题目类型解析后部分
		// 目前仅有选择题需要处理
		switch(pItem->GetType())
		{
		// 选择题解析选项
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
		
		// 在题目中寻找答案（不需要移动i）
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
		// 在题目的下一行寻找答案（若成功，则将移动i）
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
			// 下一行解析不出答案时，仍然继续
			if (GetLines(nGetType, m_pWText + i, FALSE, &uStart, &uLen))
			{
				switch(pItem->GetType())
				{
				case TYPE_BLANK:
					// 额外做这一步的原因是给填空题创建括号
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
功能：	跳过开头的回车和空格
返回值：跳过的数量
备注：
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
功能：	从字符串中获取若干行，并记录起始位置与长度
返回值：取得的行数
备注：	取得的行数仅包括含有有效文字的行数（即连续的换行不计算）
		bMarkLastLine指的是去掉最后一行，当按照规则仅能取得一行时，此参数无效
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

	// 排除开头的空白与换行符，从正式字符开始
	*uStart = i;

	bOut = FALSE;
	// 选取指定行数的文字
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
			// 只取一行
			case GETLINES_SINGLE:
				bOut = TRUE;
				continue;

			// 寻找选项开头
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

			// 寻找标号开头
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

	// 排除掉最后一行
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
功能：	检查下一行开头是否为题目标号
返回值：是题目标号则返回TRUE，不是则返回FALSE
备注：	当是题目标号时，nJump返回带有题目标号长度的跳过长度
		当不是题目标号时，nJump返回不带题目标号长度的跳过长度
*/
BOOL CAllBuildDlg::CheckNumIndex(LPCWSTR pWText, int *nJump)
{
	int i = JumpSpaces(pWText);
	BOOL bFindIndex = FALSE, bIndexEnd = FALSE;
	int nFirstValid = -1;

	// 跳过开头的空白
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
				(pWText[i] >= _L('０') && pWText[i] <= _L('９')))
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
功能：	检查选择题的选项号
返回值：如果是选项号，返回TRUE，否则返回FALSE
备注：	nJump用来返回解析字串开头的换行与空格长度
		仅当不是选项号时，nJump才会赋值，否则nJump不做改动
*/
BOOL CAllBuildDlg::CheckOptionTitle(LPCWSTR pWText, int *nJump)
{
	int i = JumpSpaces(pWText);
	int nRetCount = 0;
	BOOL bFindTitle = FALSE;

	// 跳过开头的空白
	ASSERT(nJump);
	*nJump = i;

	while(pWText[i])
	{
		switch(pWText[i])
		{
		CASE_SPACE:
		CASE_RETURN:
			// 遇到空白字符直接跳过
			break;
			
		CASE_LEFTBRACE:
			// 选项号后不允许有左括号，不然视为内容
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
				(pWText[i] >= _L('Ａ') && pWText[i] <= _L('Ｈ')) ||
				(pWText[i] >= _L('ａ') && pWText[i] <= _L('ｈ')))
			{
				bFindTitle = TRUE;
			}
			else if (pWText[i] >= _L('1') && pWText[i] <= _L('8') ||
					(pWText[i] >= _L('１') && pWText[i] <= _L('８')))
			{
				// 在批量生成中，如果以题号区分，则不认为数码为选项开始
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
功能：	检查字符串中括号内的内容，并作为答案保存
返回值：返回检测到答案的数量
备注：	本函数若检测成功，将修改字符串内容（但不会增长）
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
			else if (pWText[i] >= _L('Ａ') && pWText[i] <= _L('Ｈ'))
			{
				*dwAnswer |= (0x80 >> (pWText[i] - _L('Ａ')));
				++nCount;
			}
			else if (pWText[i] >= _L('ａ') && pWText[i] <= _L('ｈ'))
			{
				*dwAnswer |= (0x80 >> (pWText[i] - _L('ａ')));
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
			if (pWText[i] == _L('对') || pWText[i] == _L('√') ||
				pWText[i] == _L('V') || pWText[i] == _L('v'))
			{
				btStore = TRUE;
			}
			else if (pWText[i] == _L('错') || pWText[i] == _L('×') ||
					 pWText[i] == _L('X') || pWText[i] == _L('x'))
			{
				btStore = FALSE;
			}
			else if (pWText[i] == _L('正'))
			{
				if (pWText[i + 1] == _L('确'))
				{
					i++;
					btStore = TRUE;
				}
				else
				{
					return FALSE;
				}
			}
			else if (pWText[i] == _L('误'))
			{
				if (pWText[i - 1] == _L('错'))
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

	// 清理掉选项的痕迹
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

	// 首先将括号找到
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
				// 识别后续可能有的右括号
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

	// 如果找不到括号，就从题目末尾开始找
	// 要求答案与题目之间至少有一个空格隔开
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
	// 首先检测内容中的冒号或空格
	// 要求答案写在冒号后或空格后
	while(pWText[i] && !bOut)
	{
		switch(pWText[i])
		{
		case _L(':'):
		case _L('：'):
		CASE_SPACE:
			bOut = TRUE;
			break;
			
		default:
			break;
		}
		++i;
	}
	
	DWORD dwAnswer = 0;
	// 找到了冒号或空格
	if (bOut)
	{
		if (CheckQuote(pItem->GetType(), pWText + i, uLen - i + 1, &dwAnswer))
		{
			pItem->SetU32Answer(dwAnswer);
			return TRUE;
		}
		// 如果在冒号后无法解析，也不立即返回
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
	// 从后往前进行查找，不破坏序号
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

		// 首先检测内容中的冒号
		// 要求答案写在冒号后或空格后
		BOOL bFindText = FALSE;
		while(pWText[i] && !bOut)
		{
			switch(pWText[i])
			{
			case _L(':'):
			case _L('：'):
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

		// 这样设计是为了减少代码量
		// 首先检查冒号后的内容是否足够，足够则不再检测
		// 如果冒号后内容不够，则从头开始
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
					// 各答案之间用以下符号进行分隔
				case 0:
				CASE_SPACE:
				case _L(','):
				case _L('.'):
				case _L(';'):
				case _L('，'):
				case _L('。'):
				case _L('、'):
				case _L('；'):
					if (nStart >= 0)
					{
						// 设定对应的空白
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
