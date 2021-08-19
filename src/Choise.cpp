// Choise.cpp : implementation file
//

#include "stdafx.h"
#include "Choise.h"
#include "Judge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXOPTIONSINALINE		4

/////////////////////////////////////////////////////////////////////////////
// CChoise

CChoise::CChoise(BOOL bIsSingle) :
	m_nOptionCount(0), m_nAnswerCount(0),
	m_btUserAnswer(0)
{
	m_nType = bIsSingle ? TYPE_SCHOISE : TYPE_MCHOISE;

	ZeroMemory(m_Option, sizeof(m_Option));
}

CChoise::~CChoise()
{
	ClearAllInfo();
}

void CChoise::SetType(int nType)
{
	switch(nType)
	{
	case TYPE_SCHOISE:
	case TYPE_MCHOISE:
		m_nType = nType;
		break;

	default:
		ASSERT(FALSE);
	}
}

CItem* CChoise::MakeJudge()
{
	if (m_nAnswerCount > 1)
	{
		return NULL;
	}

	int i, bAns = JUDGEANS_UNKNOWN;
	for (i = 0; i < m_nOptionCount; i++)
	{
		if (_tcsstr(m_Option[i].option, _T("正确")) ||
			_tcsstri(m_Option[i].option, _T("对")) ||
			_tcsstri(m_Option[i].option, _T("T")) ||
			_tcsstri(m_Option[i].option, _T("Y")) ||
			_tcsstri(m_Option[i].option, _T("V")) ||
			_tcsstri(m_Option[i].option, _T("TRUE")) ||
			_tcsstri(m_Option[i].option, _T("YES")) ||
			_tcsstri(m_Option[i].option, _T("RIGHT")) ||
			_tcsstr(m_Option[i].option, _T("√")))
		{
			if (m_Option[i].answer)
			{
				bAns = TRUE;
			}
		}
		else if (_tcsstr(m_Option[i].option, _T("错误")) ||
				 _tcsstr(m_Option[i].option, _T("错")) ||
				 _tcsstri(m_Option[i].option, _T("F")) ||
				 _tcsstri(m_Option[i].option, _T("N")) ||
				 _tcsstri(m_Option[i].option, _T("X")) ||
				 _tcsstri(m_Option[i].option, _T("FALSE")) ||
				 _tcsstri(m_Option[i].option, _T("NO")) ||
				 _tcsstri(m_Option[i].option, _T("WRONG")) ||
				 _tcsstr(m_Option[i].option, _T("×")))
		{
			if (m_Option[i].answer)
			{
				bAns = FALSE;
			}
		}
		else
		{
			return NULL;
		}
	}

	ASSERT(bAns != JUDGEANS_UNKNOWN);
	CJudge *pItem = new CJudge;
	if (!pItem->SetDescription(GetDescription()))
	{
		delete pItem;
		return NULL;
	}
	pItem->SetAnswer(bAns);
	if (!pItem->SetResolve(GetResolve()))
	{
		delete pItem;
		return NULL;
	}
	return pItem;
}

LPCTSTR CChoise::GetTypeDesc()
{
	LPCTSTR pReturn = CItem::GetTypeDesc();
	if (pReturn)
	{
		return pReturn;
	}
	
	if (TYPE_SCHOISE == GetType())
	{
		LoadString(AfxGetInstanceHandle(), IDS_ITEMTYPE_SCH, m_szTypeDesc, TYPE_BUFFSIZE);
	}
	else
	{
		LoadString(AfxGetInstanceHandle(), IDS_ITEMTYPE_MCH, m_szTypeDesc, TYPE_BUFFSIZE);
	}
	
	return m_szTypeDesc;
}

BOOL CChoise::AddOption(LPCTSTR szOption, BOOL bIsTrue)
{
	if (m_nOptionCount >= 8)
	{
		return TRUE;
	}
	if (!CTextManager::CheckValidCharactor(szOption))
	{
		return TRUE;
	}
	
	m_nOptionCount++;
	
	return ModifyOption(m_nOptionCount-1, szOption, bIsTrue);
}

BOOL CChoise::DeleteOption(int nIndex)
{
	if (nIndex >= m_nOptionCount)
	{
		return FALSE;
	}
	
	int i;
	for (i = nIndex; i < m_nOptionCount-1; i++)
	{
		m_Option[i] = m_Option[i + 1];
	}
	
	m_nOptionCount--;
	
	return TRUE;
}

BOOL CChoise::AutoOptions(LPCWSTR pText, int nStrLen)
{
	// 在本函数中，不检测可能出现的题目号，认为在nStrLen范围内都是同一个题目
	// 当nStrLen为0时，检测范围为pText整个字符串，仍认为是同一个题目

	enum {STATE_SPACE = 1,	// 选项前的空白
		  STATE_INDEX,		// 选项号
		  STATE_INDEXSEP,	// 选项号后的空白
		  STATE_TEXT		// 选项内容
	};
	if (!pText)
	{
		return FALSE;
	}

	ClearAllOptions();

	int i = 0, IdxTextBeg = 0, IdxTextEnd = 0;
	int nState = STATE_SPACE;
	BOOL bFindIndexSep = FALSE;
	BOOL bSpaceReturn = FALSE;
	BOOL bOut = FALSE;

	int nWBuffSize = MAX_OPTION_LEN + 1;
	WCHAR strWOption[MAX_OPTION_LEN + 1] = {0};

	while(pText[i] && !bOut)
	{
		if (nStrLen && i >= nStrLen)
		{
			break;
		}

		if (STATE_SPACE == nState ||
			STATE_INDEXSEP == nState)
		{
			switch(pText[i])
			{
				// 空白
			CASE_SPACE:
				// 找到选项号后的空白，意味着新的选项开始了
				if (STATE_INDEXSEP == nState)
				{
					bFindIndexSep = TRUE;
				}
				break;

			CASE_RETURN:
				if (STATE_INDEXSEP == nState)
				{
					nState = STATE_SPACE;
					// 只有在未找到选项号后标点时，才会把当前标点并入前一个选项中
					if (!bFindIndexSep)
					{
						IdxTextEnd = i;
					}
				}
				bSpaceReturn = TRUE;
				break;

				// 仅视为选项号后的一部分
			CASE_POINT:
			CASE_RIGHTBRACE:
				if (STATE_SPACE == nState)
				{
					nState++;
				}
				else // STATE_INDEXSEP == nState
				{
					bFindIndexSep = TRUE;
				}
				break;

			CASE_LEFTBRACE:
				// 如果此时选项已经找到，则认为选项正文开始
				if (STATE_INDEXSEP == nState)
				{
					nState++;
				}
				// 如果选项未找到，则仅视为选项间的一部分
				break;

			default:
				nState++;
				break;
			}

			// 必须在选项号后有空格或反括号等字符，否则不认为是新选项的开始
			// 如果确信是题目的开始，则记录题目开始的序号
			if (STATE_TEXT == nState && bFindIndexSep)
			{
				// 保存上一个选项
				if (IdxTextBeg < IdxTextEnd && bFindIndexSep)
				{
					int nStart, nLen;
					if (!CTextManager::CheckSpaces(pText + IdxTextBeg, min(nWBuffSize, IdxTextEnd - IdxTextBeg), &nStart, &nLen))
					{
						return FALSE;
					}
					wcscpyn(strWOption, pText + IdxTextBeg + nStart, nLen + 1);
#ifdef _UNICODE
					if (!AddOption(strWOption, FALSE))
#else
					if (!AddWOption(strWOption, FALSE))
#endif
					{
						return FALSE;
					}
				}
				
				// 下一个选项内容开始
				IdxTextBeg = i;
				
				bFindIndexSep = FALSE;
				continue;
			}
			// 当选项与选项间有反括号时，按选项号检测
			else if (STATE_INDEX == nState)
			{
				continue;
			}
		}
		else if (STATE_INDEX == nState)
		{
			if ((pText[i] >= _L('A') && pText[i] <= _L('H')) ||
				(pText[i] >= _L('a') && pText[i] <= _L('h')) ||
				(pText[i] >= _L('Ａ') && pText[i] <= _L('Ｈ')) ||
				(pText[i] >= _L('ａ') && pText[i] <= _L('ｈ')))
			{
				nState = STATE_INDEXSEP;
			}
			else if (pText[i] >= _L('1') && pText[i] <= _L('8') ||
					(pText[i] >= _L('１') && pText[i] <= _L('８')))
			{
				// 如果找到了回车，则考虑是选项号
				if (bSpaceReturn)
				{
					nState = STATE_INDEXSEP;
				}
				// 否则认为是前一个选项
				else
				{
					nState = STATE_TEXT;
				}
			}
			// 如果出现了其它字符，就并入前一个选项
			else
			{
				nState = STATE_TEXT;
				continue;
			}
		}
		// 题目正文
		else // STATE_TEXT == nState
		{
			switch(pText[i])
			{
				// 题目正文中出现以下字符，则需要判断是否当前选项结束了
			CASE_SPACE:
			CASE_RETURN:
			case _L(';'):
			case _L('；'):
			case _L('。'):
				nState = STATE_SPACE;
				IdxTextEnd = i;
				break;

				// 发现括号了，就视为选项的开始
				// 在选项中判断是否为真的选项
			CASE_LEFTBRACE:
				nState = STATE_SPACE;
				IdxTextEnd = i;
				continue;
				
			default:
				break;
			}

			bSpaceReturn = FALSE;
		}

		i++;
	}

	// 保存最后一个选项
	if (STATE_TEXT == nState)
	{
		int nStart, nLen;
		if (!CTextManager::CheckSpaces(pText + IdxTextBeg, min(nWBuffSize, i - IdxTextBeg + 1), &nStart, &nLen))
		{
			return FALSE;
		}
		wcscpyn(strWOption, pText + IdxTextBeg + nStart, nLen + 1);
	}
	else
	{
		int nStart, nLen;
		if (!CTextManager::CheckSpaces(pText + IdxTextBeg, min(nWBuffSize, IdxTextEnd - IdxTextBeg + 1), &nStart, &nLen))
		{
			return FALSE;
		}
		wcscpyn(strWOption, pText + IdxTextBeg + nStart, nLen + 1);
	}

#ifdef _UNICODE
	AddOption(strWOption, FALSE);
#else
	AddWOption(strWOption, FALSE);
#endif

	if (!m_nOptionCount)
	{
		return FALSE;
	}

	return TRUE;
}

void CChoise::ChangeAsPara(CTestPara *pCPara, int nExPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();

	if (!IsDefaultType())
	{
		if (!(pPara->dwChsOptions & CHSOPTION_CUSTOMASWELL))
		{
			return;
		}
	}

	if (pPara->dwChsOptions & CHSOPTION_RANDOM)
	{
		RandomOptions(nExPara);
	}
}

BOOL CChoise::GetAnswer(int nIndex)
{
	if (nIndex >= m_nOptionCount || !m_Option[nIndex].option)
	{
		return FALSE;
	}
	
	return m_Option[nIndex].answer;
}

LPCTSTR CChoise::GetOption(int nIndex)
{
	if (nIndex >= m_nOptionCount || !m_Option[nIndex].option)
	{
		return FALSE;
	}
	
	return m_Option[nIndex].option;
}

// 选择题存储格式
// ?Bytes：通用存储块
// 4Bytes：读取需要的最大缓冲区字节数
// 1Byte：选项数
// 1Byte：某选项答案
// 2Bytes：某选项长度（字节计），包括末尾0
// ?Bytes：选项
EBOOL CChoise::LoadFile(FILE *fp, DWORD dwVersion)
{
	BYTE btTemp = 0;
	int nMax = 0, nTemp = 0, j = 0;
	BOOL bAnswer = 0;
	LPWSTR ptr = NULL;

	if(!CItem::LoadFile(fp, dwVersion))
	{
		return Q_WARNING(ERROR_INNER);
	}
	
	// 最大缓冲字节数
	if (!fread(&nMax, sizeof(int), 1, fp))
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}
	ptr = (LPWSTR)malloc(nMax);
	if (!ptr)
	{
		return Q_WARNING(ERROR_ALLOC_FAIL);
	}

#ifndef _UNICODE
	// 全部题库按UNICODE存储
	LPTSTR ptrC = (LPTSTR)malloc(nMax);
	if (!ptrC)
	{
		free(ptr);
		return Q_WARNING(ERROR_ALLOC_FAIL);
	}
#endif
	
	// 选项数
	if (!fread(&btTemp, sizeof(BYTE), 1, fp))
	{
		free(ptr);
#ifndef _UNICODE
		free(ptrC);
#endif
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}
	
	ClearAllOptions();
	// 读各选项
	while(j++ < btTemp)
	{
		// 选项是否正确
		if (!fread(&bAnswer, sizeof(BYTE), 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}

		// 选项长度
		nTemp = 0;
		if (!fread(&nTemp, sizeof(short), 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		if (nTemp > nMax)
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCORRECT);
		}

		// 选项内容
		if (!fread(ptr, nTemp, 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		
		// 添加选项
#ifdef _UNICODE
		if (!AddOption(ptr, bAnswer))
		{
			free(ptr);
			return Q_WARNING(ERROR_INNER);
		}
#else
		if (!WideCharToMultiByte(CP_ACP, 0, ptr, -1, ptrC, nTemp, NULL, FALSE))
		{
			free(ptr);
			free(ptrC);
			return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
		}
		if (!AddOption(ptrC, bAnswer))
		{
			free(ptr);
			free(ptrC);
			return Q_WARNING(ERROR_INNER);
		}
#endif
	}

	free(ptr);
#ifndef _UNICODE
	free(ptrC);
#endif
	return Q_RET(TRUE);
	
}

// 选择题存储格式
// ?Bytes：通用存储块
// 4Bytes：读取需要的最大缓冲区字节数
// 1Byte：选项数
// 1Byte：某选项答案
// 2Bytes：某选项长度（字节计），包括末尾0
// ?Bytes：选项
EBOOL CChoise::SaveFile(FILE *fp)
{
	int i, nMaxLen, nTemp;

	if (!CItem::SaveFile(fp))
	{
		return Q_ERROR(ERROR_INNER);
	}

#ifdef _UNICODE
	// 找出读取需要的最大缓冲区大小
	nMaxLen = 0;
	for(i = 0; i < m_nOptionCount; i++)
	{
		nTemp = (_tcslen(m_Option[i].option) + 1) * sizeof(TCHAR);
		nMaxLen = max(nMaxLen, nTemp);
	}
	if (!fwrite(&nMaxLen, sizeof(int), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 选项数
	if (!fwrite(&m_nOptionCount, sizeof(BYTE), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	
	for (i = 0; i < m_nOptionCount; i++)
	{
		// 选项答案
		if (!fwrite(&m_Option[i].answer, sizeof(BYTE), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		
		// 选项长度
		nTemp = (_tcslen(m_Option[i].option) + 1) * sizeof(TCHAR);
		if (!fwrite(&nTemp, sizeof(short), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		
		// 选项内容
		if (!fwrite(m_Option[i].option, nTemp, 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}
#else
	nMaxLen = 0;
	for(i = 0; i < m_nOptionCount; i++)
	{
		nTemp = MultiByteToWideChar(CP_ACP, 0, m_Option[i].option, -1, NULL, 0);
		nMaxLen = max(nMaxLen, nTemp);
	}
	nTemp = nMaxLen * sizeof(WCHAR);
	if (!fwrite(&nTemp, sizeof(int), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	LPWSTR ptrW = (LPWSTR)malloc(nTemp);
	if (!ptrW)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	
	// 选项数
	if (!fwrite(&m_nOptionCount, sizeof(BYTE), 1, fp))
	{
		free(ptrW);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	for (i = 0; i < m_nOptionCount; i++)
	{
		// 选项答案
		if (!fwrite(&m_Option[i].answer, sizeof(BYTE), 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		
		// 选项长度
		nTemp = MultiByteToWideChar(CP_ACP, 0, m_Option[i].option, -1, ptrW, nMaxLen) * sizeof(WCHAR);
		if (!fwrite(&nTemp, sizeof(short), 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		
		// 选项内容
		if (!fwrite(ptrW, nTemp, 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}

	free(ptrW);
#endif

	return Q_TRUE;
}

RETURN_E TiXmlElement* CChoise::WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible)
{
	pParent = CItem::WriteXML(pParent, nEncode, bMaxCompatible);
	if (!pParent)
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}
	
	TiXmlText *pText = NULL;
	TiXmlElement *pOption = NULL;
	LPSTR lpString = NULL;
	
	int i;
	for (i = 0; i < m_nOptionCount; i++)
	{
		pOption = new TiXmlElement(xml_lpszNode_Option);
		if (!pOption)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pParent->LinkEndChild(pOption);
		pOption->SetAttribute(xml_lpszAttribute_Answer, m_Option[i].answer ? xml_lpszCommon_True : xml_lpszCommon_False);
		
		lpString = CTextManager::SwitchSave(m_Option[i].option, nEncode, NULL);
		if (!lpString)
		{
			return Q_ERROR_RET(ERROR_TEXT_TRANSCODING_FAIL, NULL);
		}
		pText = new TiXmlText(lpString);
		free(lpString);
		if (!pText)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pOption->LinkEndChild(pText);
	}
	
	return Q_TRUE_RET(pParent);
}

EBOOL CChoise::ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup)
{
	if (!CItem::ReadXML(pQstn, nEncode, bGroup))
	{
		return Q_WARNING(ERROR_INNER);
	}
	
	LPTSTR lpString;
	LPCSTR lpAsw;
	BOOL bAsw = FALSE;
	TiXmlElement *pNode = pQstn->FirstChildElement(xml_lpszNode_Option);
	while(pNode)
	{
		lpAsw = pNode->Attribute(xml_lpszAttribute_Answer);
		if (!stricmp(lpAsw, xml_lpszCommon_True))
		{
			bAsw = TRUE;
		}
		else if (!stricmp(lpAsw, xml_lpszCommon_False))
		{
			bAsw = FALSE;
		}
		else
		{
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		
		lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
		if (!lpString)
		{
			break;
		}
		CTextManager::RemoveReturn(lpString, 0);
		
		if (!AddOption(lpString, bAsw))
		{
			Q_WARNING(ERROR_FILE_INCOMPLETE);
			free(lpString);
			break;
		}
		free(lpString);
		
		pNode = pNode->NextSiblingElement(xml_lpszNode_Option);
	}
	
	return Q_RET(m_nOptionCount > 1);
}

void CChoise::ClearAllOptions()
{
	int i;
	for(i = _countof(m_Option) - 1; i >= 0; i--)
	{
		if (m_Option[i].option)
		{
			free(m_Option[i].option);
			m_Option[i].option = NULL;
			m_Option[i].answer = FALSE;
		}
	}
	
	m_nOptionCount = 0;
	m_nAnswerCount = 0;
}

void CChoise::ClearAllInfo()
{
	CItem::ClearAllInfo();
	
	ClearAllOptions();
}

void CChoise::ClearUserInfo()
{
	CItem::ClearUserInfo();
	m_btUserAnswer = 0;
}

CItem* CChoise::Alloc()
{
	CChoise *pChoise = new CChoise(TYPE_SCHOISE == GetType());
	if (!pChoise)
	{
		return NULL;
	}
	
	pChoise->CopyItem(this);
	return (CItem*)pChoise;
}

BOOL CChoise::CopyItem(CItem *pItem)
{
	ClearAllInfo();
	
	if (!CItem::CopyItem(pItem))
	{
		return FALSE;
	}
	
	// 只有在题目类型相符的时候才会复制选项
	if (GetType() == pItem->GetType())
	{
		int i;
		CChoise *pChoise = (CChoise*)pItem;
		
		for (i = 0; i < pChoise->m_nOptionCount; i++)
		{
			if (!AddOption(pChoise->m_Option[i].option, pChoise->m_Option[i].answer))
			{
				return FALSE;
			}
		}
	}
	
	return TRUE;
}

BOOL CChoise::CopyChoiseIgnoreType(CItem *pItem)
{
#ifdef _DEBUG
	if (pItem->GetType() == TYPE_MCHOISE && GetType() == TYPE_SCHOISE)
	{
		ASSERT(((CChoise*)pItem)->m_nAnswerCount == 1);
	}
#endif

	int nSave = m_nType;
	m_nType = pItem->GetType();
	
	BOOL bRet = CopyItem(pItem);
	
	m_nType = nSave;
	return bRet;
}

void CChoise::ClipCopy(CFixedSharedFile &sf)
{
	CItem::ClipCopy(sf);
	
	int nLen, i;
	
	sf.Write(&m_nOptionCount, sizeof(m_nOptionCount));
	
	for (i = 0; i < m_nOptionCount; i++)
	{
		nLen = _tcslen(m_Option[i].option);
		
		sf.Write(&nLen, sizeof(nLen));
		sf.Write(m_Option[i].option, nLen * sizeof(TCHAR));
		sf.Write(&m_Option[i].answer, sizeof(m_Option[i].answer));
	}
}

BOOL CChoise::ClipPaste(CFixedSharedFile &sf)
{
	if (!CItem::ClipPaste(sf))
	{
		return FALSE;
	}
	
	int i, j;
	CString str;
	
	sf.Read(&i, sizeof(i));
	
	ClearAllOptions();
	while(i--)
	{
		sf.Read(&j, sizeof(j));
		sf.Read(str.GetBufferSetLength(j), j * sizeof(TCHAR));
		sf.Read(&j, sizeof(j));
		
		if (!AddOption(str, j))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

void CChoise::MakeText(_stdstring &str, UINT nStyle, int nPara)
{
	str.erase();
	
	str += GetDescription();
	
	if (nStyle & MKTXT_OPTIONNEWLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
	else
	{
		str += _T("  ");
	}
	
	MakeOptionText(str, nStyle, nPara);
	
	if (nStyle & MKTXT_ENDLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CChoise::MakeAnswer(_stdstring &str, UINT nStyle, int nPara /* = 0 */)
{
	int i;
	
	str.erase();

	if (nStyle & MKASW_ASWINQUESTION)
	{
		int nStart, nEnd;
		_stdstring str1;
		MakeAnswer(str1, MKASW_ADDSPACE, 0);

		if(FindQuotePos(nStart, nEnd))
		{
			str = GetDescription();

			if (nStart == nEnd)
			{
				str.insert(nStart, str1);
			}
			else
			{
				str.replace(nStart, nEnd - nStart, str1);
			}
		}
		else
		{
			str = GetDescription();
			str += _T("(");
			str += str1;
			str += _T(")");
		}

		if (nStyle & MKTXT_OPTIONNEWLINE)
		{
			str += CTextManager::s_szWinReturn;
		}
		
		MakeOptionText(str, nStyle, nPara);

		if (MKASW_WITHRESOLVE & nStyle)
		{
			str += CTextManager::s_szWinReturn;
			str += GetResolve();
		}
	}
	else
	{
		if(nStyle & MKASW_ADDSPACE)
		{
			str += _T("  ");
		}

		for (i = 0; i < m_nOptionCount; i++)
		{
			if (m_Option[i].answer)
			{
				str += (TCHAR)(_T('A') + i);
				
				if (nStyle & MKASW_WITHOPTION)
				{
					str += _T(". ");
					str += m_Option[i].option;
					str += CTextManager::s_szWinReturn;
				}
			}
		}

		if(nStyle & MKASW_ADDSPACE)
		{
			str += _T("  ");
		}
	}

	if(nStyle & MKASW_ENDLINE && !(nStyle & MKASW_WITHOPTION))
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CChoise::MakeUserAnswer(_stdstring &str)
{
	int i;
	BYTE btMask = 0x80;
	
	str.erase();
	
	for (i = 0; i < 8; btMask >>= 1, i++)
	{
		if (m_btUserAnswer & btMask)
		{
			str += (TCHAR)(_T('A') + i);
		}
	}
	
	if (!str.length())
	{
		TCHAR szBuf[16] = {0};
		LoadString(AfxGetInstanceHandle(), IDS_QLIST_UNASW, szBuf, _countof(szBuf));
		str = szBuf;
	}
}

BOOL CChoise::CheckItemValid()
{
	return (m_nOptionCount && m_nAnswerCount) ? TRUE : FALSE;
}

void CChoise::MakeTextOut(_stdstring &str,
						  int nListIndex,
						  int nItemIndex,
						  PTPARAS pImagePara,
						  BOOL bWithAnswer,
						  BOOL bWithRes)
{
	if (bWithAnswer)
	{
		_stdstring asw;
		int nStart, nEnd;
		TCHAR sz[64] = {0};

		_sntprintf(sz, 64, _T("%d. "), nItemIndex);
		str += sz;

		MakeAnswer(asw, MKASW_ADDSPACE, 0);
		if (FindQuotePos(nStart, nEnd))
		{
			_stdstring desc = GetDescription();
			if (nStart == nEnd)
			{
				desc.insert(nStart, asw);
			}
			else
			{
				desc.replace(nStart, nEnd - nStart, asw);
			}
			str += desc;
		}
		else
		{
			str += GetDescription();
			str += _T("(");
			str += asw;
			str += _T(")");
		}
		str += CTextManager::s_szWinReturn;
		MakeTextImage(str, nItemIndex, pImagePara, FALSE);
	}
	else
	{
		CItem::MakeTextOut(str, nListIndex, nItemIndex, pImagePara, bWithAnswer, bWithRes);
	}

	MakeOptionText(str, MKTXT_OPTIONLEN, 50);
	str += CTextManager::s_szWinReturn;

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_TEXTONLY);
	}
}

void CChoise::MakeHTML(FILE *fp,
					   _stdstring &str,
					   int nListIndex,
					   int nItemIndex,
					   PTPARAS pImagePara,
					   BOOL bWithAnswer,
					   BOOL bWithRes)
{
	CItem::MakeHTML(fp, str, nListIndex, nItemIndex, pImagePara, bWithAnswer, bWithRes);

	int i;
	int ColWidth[MAXOPTIONSINALINE] = {0};
	int nOptionsInALine = CheckOptionWidth(50, ColWidth);
	LPCTSTR pDesc = GetDescription();

	str += _T("<ol class=\"choise");

	// 如果题干为空，则选项和题号在同一行
	if (!pDesc || !_tcslen(pDesc))
	{
		str += _T("in\">\n");
	}
	else
	{
		str += _T("\">\n");
	}

	for (i = 0; i < GetOptionCount(); ++i)
	{
		str += _T("<li class=\"choise\"");
		if (nOptionsInALine > 2)
		{
			if (bWithAnswer && m_Option[i].answer)
			{
				str += _T(" style=\"text-decoration:underline;font-weight:bold;\">");
			}
			else
			{
				str += _T(">");
			}
		}
		else
		{
			str += _T(" style=\"clear:both;padding:0;");
			if (bWithAnswer && m_Option[i].answer)
			{
				str += _T("text-decoration:underline;font-weight:bold;\">");
			}
			else
			{
				str += _T("\">");
			}
		}
		str += m_Option[i].option;
		str += _T("</li>\n");
	}

	str += _T("</ol>\n");

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_HTMLONLY);
	}
}

void CChoise::SetUserAnswer(int nIndex)
{
	BYTE btSet = (0x80 >> nIndex);
	
	if (nIndex >= m_nOptionCount)
	{
		return;
	}

	if (TYPE_SCHOISE == GetType())
	{
		// 单选题选中了就禁止取消
		if (m_btUserAnswer & btSet)
		{
			return;
		}
		m_btUserAnswer = btSet;
	}
	else
	{
		if (m_btUserAnswer & btSet)
		{
			m_btUserAnswer &= ~btSet;
		}
		else
		{
			m_btUserAnswer |= btSet;
		}
	}
}

BOOL CChoise::GetUserAnswer(int nIndex)
{
	BYTE Mask = 0x80 >> nIndex;
	return (m_btUserAnswer & Mask);
}

int CChoise::Score(CTestPara *pCPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();
	BYTE answer = MakeByteAnswer();
	
	if (!m_btUserAnswer)
	{
		m_nAnswerCheck = WRONG;
	}
	else if (answer == m_btUserAnswer)
	{
		m_nAnswerCheck = ALLRIGHT;
	}
	else
	{
		BYTE btWrong = ~answer;
		if (m_btUserAnswer & btWrong)
		{
			m_nAnswerCheck = WRONG;
		}
		else
		{
			m_nAnswerCheck = HALFRIGHT;
		}
	}

	if (MARKTYPE_AVERAGE == pPara->nMarkType)
	{
		switch(m_nAnswerCheck)
		{
		case ALLRIGHT:
			return 100;

		case HALFRIGHT:
			if (pPara->dwChsOptions & CHSOPTION_MCHHALF)
			{
				return 50;
			}

		default:
			return 0;
		}
	}
	else
	{
		switch(m_nAnswerCheck)
		{
		case ALLRIGHT:
			return (2 *
				((GetType() == TYPE_SCHOISE) ? pPara->nSChPoints : pPara->nMChPoints));

		case HALFRIGHT:
			if (pPara->dwChsOptions & CHSOPTION_MCHHALF)
			{
				return pPara->nMChPoints;
			}
			
		default:
			return 0;
		}
	}
}

BOOL CChoise::Match(LPCTSTR strMatch, BOOL bMatchCase)
{
	if (CItem::Match(strMatch, bMatchCase))
	{
		return TRUE;
	}
	
	int i;
	for (i = 0; i < m_nOptionCount; i++)
	{
		if (bMatchCase)
		{
			if (_tcsstr(m_Option[i].option, strMatch))
			{
				return TRUE;
			}
		}
		else
		{
			if (_tcsstri(m_Option[i].option, strMatch))
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

void CChoise::SetU32Answer(DWORD dwAnswer)
{
	BYTE btMark = 0x80;
	int i;
	
	m_nAnswerCount = 0;
	for (i = 0; i < m_nOptionCount; i++)
	{
		if (dwAnswer & (btMark >> i))
		{
			if (GetType() == TYPE_SCHOISE && m_nAnswerCount > 0)
			{
				m_Option[i].answer = FALSE;
			}
			else
			{
				m_Option[i].answer = TRUE;
				m_nAnswerCount++;
			}
		}
		else
		{
			m_Option[i].answer = FALSE;
		}
	}
}

void CChoise::CalcCRC()
{
	CItem::CalcCRC();

	int nLen, i, j;
	PBYTE ptr;

#ifndef _UNICODE
	int nBuffSize = 0;
	for (i = 0; i < m_nOptionCount; i++)
	{
		j = _tcslen(m_Option[i].option) + 1;
		nBuffSize = max(nBuffSize, j);
	}
	nBuffSize *= sizeof(WCHAR);
	LPWSTR str = (LPWSTR)malloc(nBuffSize);
	if (!str)
	{
		return;
	}
#endif

	for (i = 0; i < m_nOptionCount; i++)
	{
#ifdef _UNICODE
		ptr = (PBYTE)m_Option[i].option;
		nLen = _tcslen(m_Option[i].option) * sizeof(TCHAR);
#else
		if (!MultiByteToWideChar(CP_ACP, 0, m_Option[i].option, -1, str, nBuffSize))
		{
			free(str);
			return;
		}
		ptr = (PBYTE)str;
		nLen = wcslen(str) * sizeof(WCHAR);
#endif

		for (j = 0; j < nLen; j++)
		{
			m_wCRC += ptr[j];
		}
	}

#ifndef _UNICODE
	free(str);
#endif
}

BOOL CChoise::IsSameItem(CItem *pItem, int nSimilar)
{
	if (!CItem::IsSameItem(pItem, nSimilar))
	{
		return FALSE;
	}

	CChoise *pCom = (CChoise*)pItem;
	if (pCom->m_nOptionCount != m_nOptionCount || pCom->m_nAnswerCount != m_nAnswerCount)
	{
		return FALSE;
	}

	int i;
	for (i = 0; i < m_nOptionCount; i++)
	{
		if (nSimilar > 99)
		{
			if (_tcscmp(m_Option[i].option, pCom->m_Option[i].option))
			{
				return FALSE;
			}
		}
		else
		{
			CTextSimilarity t;
			t.Compare(m_Option[i].option, pCom->m_Option[i].option);
			if ((int)(t.GetInfo() * 100.f) < nSimilar)
			{
				return FALSE;
			}
		}
		if (m_Option[i].answer != pCom->m_Option[i].answer)
		{
			return FALSE;
		}
	}

	return TRUE;
}

#ifndef _UNICODE
BOOL CChoise::AddWOption(LPCWSTR szOption, BOOL bIsTrue)
{
	TCHAR strOption[(MAX_OPTION_LEN + 1) * sizeof(WCHAR)] = {0};

	if (!WideCharToMultiByte(CP_ACP, 0, szOption, -1, strOption, (MAX_OPTION_LEN + 1) * sizeof(WCHAR), 0, 0))
	{
		return FALSE;
	}

	return AddOption(strOption, bIsTrue);
}
#endif

void CChoise::RandomOptions(DWORD seed /* = NULL */)
{
	if (!seed)
	{
		srand(time(NULL) + _getpid());
	}
	
	int i, j;
	CHOISEOPTION temp;
	
	for (i = 0; i < m_nOptionCount; i++)
	{
		j = rand() % m_nOptionCount;
		temp = m_Option[i];
		m_Option[i] = m_Option[j];
		m_Option[j] = temp;
	}
}

BOOL CChoise::ModifyOption(int nIndex, LPCTSTR szOption, BOOL bIsTrue)
{
	// 禁止自定位置添加选项
	if (nIndex >= m_nOptionCount)
	{
		return FALSE;
	}

	// 当某选项为空时，视为删除选项
	if (!szOption)
	{
		return DeleteOption(nIndex);
	}

	if (m_Option[nIndex].option)
	{
		free(m_Option[nIndex].option);
		m_Option[nIndex].option = NULL;
	}

	m_Option[nIndex].option = _tcsdup(szOption);
	if (!m_Option[nIndex].option)
	{
		return FALSE;
	}
	CTextManager::RemoveReturn(m_Option[nIndex].option, 0);

	if (m_Option[nIndex].answer != bIsTrue)
	{
		bIsTrue ? ++m_nAnswerCount : --m_nAnswerCount;
	}

	// 如果是单选题，在新增答案时只能添加一个
	if (GetType() == TYPE_SCHOISE && m_nAnswerCount > 1 && bIsTrue)
	{
		m_nAnswerCount = 1;
		return TRUE;
	}
	
	m_Option[nIndex].answer = bIsTrue;
	return TRUE;
}

BYTE CChoise::MakeByteAnswer()
{
	int i;
	BYTE ret = 0;

	for (i = 0; i < 8; i++)
	{
		if (!m_Option[i].option)
		{
			break;
		}

		if (m_Option[i].answer)
		{
			ret |= (0x80 >> i);
		}
	}

	return ret;
}

int CChoise::CheckOptionWidth(int nWidth, int *ColWidth)
{
	ASSERT(ColWidth);
	ZeroMemory(ColWidth, sizeof(int) * MAXOPTIONSINALINE);

	int i;
	int nLen[8] = {0};
	for (i = 0; i < m_nOptionCount; i++)
	{
		// 加3是加的选项号长度
		nLen[i] = AfxGetStrShowWidth(m_Option[i].option) + 3;
		ColWidth[i % MAXOPTIONSINALINE] = max(ColWidth[i % MAXOPTIONSINALINE], nLen[i]);
	}

	// 这里把所有选项的长度都获取了
	// 先检查四个四个是否有可能
	if ((ColWidth[0] + ColWidth[1] + ColWidth[2] + ColWidth[3] + 3 * 3) <= nWidth)
	{
		ColWidth[0] += 3;
		ColWidth[1] += 3;
		ColWidth[2] += 3;

		return 4;
	}

	// 四个四个不可能，就检查两个两个
	ColWidth[0] = max(ColWidth[0], ColWidth[2]);
	ColWidth[1] = max(ColWidth[1], ColWidth[3]);
	ColWidth[2] = ColWidth[3] = 0;

	if ((ColWidth[0] + ColWidth[1] + 3) <= nWidth)
	{
		ColWidth[0] += 3;

		return 2;
	}

	ColWidth[0] = ColWidth[1] = 0;
	return 1;
}

void CChoise::MakeOptionText(_stdstring &str, UINT nStyle, int nPara)
{
	int i;
	int ColWidth[MAXOPTIONSINALINE] = {0};
	int nOptionsInALine = 1;

	if (nStyle & MKTXT_OPTIONLEN)
	{
		nOptionsInALine = CheckOptionWidth(nPara, ColWidth);
	}
	
	// 检查最长的选项长度
	int nMax = 0, nSize = 0;;
	if (nOptionsInALine > 1)
	{
		for (i = 0; i < MAXOPTIONSINALINE; i++)
		{
			nMax = max(nMax, ColWidth[i]);
		}
	}
	else
	{
		nMax = _tcslen(m_Option[0].option);
		for (i = 1; i < m_nOptionCount; i++)
		{
			nSize = _tcslen(m_Option[i].option);
			nMax = max(nSize, nMax);
		}
	}
	nSize = nMax + 5;
	nMax = nSize * sizeof(TCHAR);
	
	LPTSTR tmp = (LPTSTR)malloc(nMax);
	if (!tmp)
	{
		return;
	}
	LPTSTR tmp1 = (LPTSTR)malloc(nMax);
	if (!tmp1)
	{
		return;
	}
	TCHAR Fmt[24] = {0};
	
	BOOL bIsReturn = TRUE;
	for (i = 0; i < m_nOptionCount; i++)
	{
		bIsReturn = !((i + 1) % nOptionsInALine);
		if (nOptionsInALine > 1 && !bIsReturn && i < m_nOptionCount - 1)
		{
			_sntprintf(Fmt, _countof(Fmt), _T("%%-%ds"), ColWidth[i % nOptionsInALine]);
		}
		else
		{
			_tcsncpy(Fmt, _T("%s"), _countof(Fmt));
		}
		
		_sntprintf(tmp, nSize, _T("(%c) %s"), (TCHAR)(_T('A') + i), m_Option[i].option);
		_sntprintf(tmp1, nSize, Fmt, tmp);
		
		str += tmp1;
		if (nStyle & MKTXT_OPTIONSINALINE)
		{
			str += _T("  ");
		}
		else if (bIsReturn && i < m_nOptionCount - 1)
		{
			str += CTextManager::s_szWinReturn;
		}
	}
	
	free(tmp);
	free(tmp1);
}

BOOL CChoise::FindQuotePos(int &nStart, int &nEnd)
{
	if (!m_strDescription)
	{
		return FALSE;
	}

	int i = 0;
	nStart = -1;
	nEnd = 0;

	// 首先将括号找到
	while(m_strDescription[i])
	{
		switch(m_strDescription[i])
		{
		case _T(' '):
		case _T('\t'):
			break;

		case _T('\r'):
		case _T('\n'):
			nStart = -1;
			return FALSE;

		case _T('('):
		case _T('['):
		case _T('{'):
		case _T('<'):
			if (nStart < 0)
			{
				nStart = i + 1;
			}
			break;

		case _T(')'):
		case _T(']'):
		case _T('}'):
		case _T('>'):
			if (nStart >= 0)
			{
				nEnd = i;
				return TRUE;
			}
			break;

#ifdef _UNICODE
		case _T('　'):
			break;

		case _T('〕'):
		case _T('〉'):
		case _T('」'):
		case _T('』'):
		case _T('〗'):
		case _T('】'):
		case _T('）'):
		case _T('］'):
		case _T('｝'):
			if (nStart >= 0)
			{
				nEnd = i;
				return TRUE;
			}
			break;

		case _T('〔'):
		case _T('〈'):
		case _T('「'):
		case _T('『'):
		case _T('〖'):
		case _T('【'):
		case _T('（'):
		case _T('［'):
		case _T('｛'):
			if (nStart < 0)
			{
				nStart = i + 1;
			}
			break;
#else
		case (TCHAR)0xA1:
			if (m_strDescription[i + 1] == (TCHAR)0xA1) // 全角空格
			{
				i++;
			}
			else if (m_strDescription[i + 1] == (TCHAR)0xB3 || // 〕
					 m_strDescription[i + 1] == (TCHAR)0xB5 || // 〉
					 m_strDescription[i + 1] == (TCHAR)0xB9 || // 」
					 m_strDescription[i + 1] == (TCHAR)0xBB || // 』
					 m_strDescription[i + 1] == (TCHAR)0xBD || // 〗
					 m_strDescription[i + 1] == (TCHAR)0xBF)   // 】
			{
				if (nStart >= 0)
				{
					nEnd = i;
					return TRUE;
				}
				i++;
			}
			// 仅视为题目号前的一部分
			else if (m_strDescription[i + 1] == (TCHAR)0xB2 || // 〔
					 m_strDescription[i + 1] == (TCHAR)0xB4 || // 〈
					 m_strDescription[i + 1] == (TCHAR)0xB8 || // 「
					 m_strDescription[i + 1] == (TCHAR)0xBA || // 『
					 m_strDescription[i + 1] == (TCHAR)0xBC || // 〖
					 m_strDescription[i + 1] == (TCHAR)0xBE)   // 【
			{
				i++;
				if (nStart < 0)
				{
					nStart = i + 1;
				}
			}
			else
			{
				if (nStart >= 0)
				{
					nStart = -1;
				}
				i++;
			}
			break;

		case (TCHAR)0xA3:
			// 仅视为题目号后的一部分
			if (m_strDescription[i + 1] == (TCHAR)0xA9 || // ）
				m_strDescription[i + 1] == (TCHAR)0xDD || // ］
				m_strDescription[i + 1] == (TCHAR)0xFD)   // ｝
			{
				if (nStart >= 0)
				{
					nEnd = i;
					return TRUE;
				}
				i++;
			}
			// 仅视为题目号前的一部分
			else if (m_strDescription[i + 1] == (TCHAR)0xA8 || // （
					 m_strDescription[i + 1] == (TCHAR)0xDB || // ［
					 m_strDescription[i + 1] == (TCHAR)0xFB)   // ｛
			{
				i++;
				if (nStart < 0)
				{
					nStart = i + 1;
				}
			}
			else
			{
				if (nStart >= 0)
				{
					nStart = -1;
				}
				i++;
			}
			break;
#endif

		default:
#ifndef _UNICODE
			// 发现是个多字节字，则多前进一个
			if (m_strDescription[i] & 0x80)
			{
				i++;
			}
#endif
			if (nStart >= 0)
			{
				nStart = -1;
			}
			break;
		}

		i++;
	}

	nStart = -1;
	return FALSE;
}
