// ItemGroup.cpp: implementation of the CItemGroup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemGroup.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemGroup::CItemGroup() :
	m_nStartIndex(1)
{
	m_nType = TYPE_GROUP;
}

CItemGroup::~CItemGroup()
{

}

LPCTSTR CItemGroup::GetTypeDesc()
{
	if (m_szTypeDesc[0])
	{
		return m_szTypeDesc;
	}
	else
	{
		return CItem::GetTypeDesc();
	}
}

void CItemGroup::SetTypeDesc(LPCTSTR szTypeDesc)
{
	_tcscpyn(m_szTypeDesc, szTypeDesc, TYPE_BUFFSIZE);
}

int CItemGroup::GetTypeCount(int nType, BOOL bStore)
{
	int nCount = 0;
	CItem *pItem = m_pHead;

	while(pItem)
	{
		if (pItem->GetType() == nType)
		{
			if (!bStore || (bStore && pItem->IsStored()))
			{
				nCount++;
			}
		}

		pItem = pItem->GetNext();
	}

	return nCount;
}

void CItemGroup::ChangeAsPara(CTestPara *pCPara, int nExPara /* = 0 */)
{
	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->ChangeAsPara(pCPara, nExPara);
		pItem = pItem->GetNext();
	}
}

void CItemGroup::SetStore(BOOL bStore)
{
	CItem *pItem = m_pHead;

	while(pItem)
	{
		pItem->SetStore(bStore);
		pItem = pItem->GetNext();
	}
}

BOOL CItemGroup::IsStored()
{
	CItem *pItem = m_pHead;
	
	while(pItem)
	{
		if (pItem->IsStored())
		{
			return TRUE;
		}

		pItem = pItem->GetNext();
	}

	return FALSE;
}

BOOL CItemGroup::LoadStore(FILE *fp)
{
	CItem *pItem = m_pHead;

	while(pItem)
	{
		if (!pItem->LoadStore(fp))
		{
			return FALSE;
		}

		pItem = pItem->GetNext();
	}

	return TRUE;
}

BOOL CItemGroup::SaveStore(FILE *fp)
{
	CItem *pItem = m_pHead;
	
	while(pItem)
	{
		if (!pItem->SaveStore(fp))
		{
			return FALSE;
		}
		
		pItem = pItem->GetNext();
	}
	
	return TRUE;
}

// 题目组存储格式
// 12Bytes：题目类型
// ?Bytes：通用存储块
// 2Byte：题目组中包含题目数
// ?Byte：所包含题目
EBOOL CItemGroup::LoadFile(FILE *fp, DWORD dwVersion)
{
	int nTemp, nType;
	CItem *pItem = NULL;

	// 读取类型
	LPTSTR ptr = CTextManager::LoadFileString(fp, 0, 6);
	if (!ptr)
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}
	SetTypeDesc(ptr);
	free(ptr);

	// 通用存储块
	switch (CItem::LoadFile(fp, dwVersion))
	{
	case EOF:
	case FALSE:
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

	// 题目数
	nTemp = 0;
	if (!fread(&nTemp, sizeof(short), 1, fp))
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}
	if (nTemp > MAX_ITEMGROUP_NUM)
	{
		return Q_WARNING(ERROR_FILE_INCORRECT);
	}

	while(nTemp--)
	{
		// 读取题目类型
		if (!fread(&nType, sizeof(int), 1, fp))
		{
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		
		nType = nType % TYPE_CUSTOMRIGHT;
		switch(nType)
		{
		case TYPE_DEFAULT:
			continue;
			
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
			pItem = new CChoise(TYPE_SCHOISE == nType);
			break;
			
		case TYPE_JUDGE:
			pItem = new CJudge;
			break;
			
		case TYPE_BLANK:
			pItem = new CBlank;
			break;
			
		case TYPE_TEXT:
			pItem = new CText;
			break;
			
		default:
			return FALSE;
		}
		if (!pItem)
		{
			return Q_WARNING(ERROR_ALLOC_FAIL);
		}
		
		switch(pItem->LoadFile(fp, dwVersion))
		{
		case FALSE:
		case EOF:
			delete pItem;
			return Q_WARNING(ERROR_INNER);
			
		case TRUE:
			break;
			
		default:
			ASSERT(FALSE);
		}
		
		AddItem(pItem);
		
		// 计算一下CRC码
		pItem->CalcCRC();
	}

	return Q_RET(TRUE);
}

// 题目组存储格式
// 12Bytes：题目类型
// ?Bytes：通用存储块
// 2Byte：题目组中包含题目数
// ?Byte：所包含题目
EBOOL CItemGroup::SaveFile(FILE *fp)
{
	WCHAR wTypeDesc[6] = {0};
#ifdef _UNICODE
	wcscpyn(wTypeDesc, m_szTypeDesc, 6);
#else
	LPWSTR ptrW = (LPWSTR)CTextManager::SwitchSave(m_szTypeDesc, CS_UTF16, NULL);
	if (!ptrW)
	{
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}
	wcscpyn(wTypeDesc, ptrW, 6);
	free(ptrW);
#endif
	if (!fwrite(wTypeDesc, sizeof(wTypeDesc), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	if (!CItem::SaveFile(fp))
	{
		return Q_ERROR(ERROR_INNER);
	}

	if (!fwrite(&m_nItemCount, sizeof(short), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	CItem *pItem = m_pHead;
	int i;
	while(pItem)
	{
		i = pItem->GetRawType();
		if (!fwrite(&i, sizeof(int), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}

		if (!pItem->SaveFile(fp))
		{
			return Q_ERROR(ERROR_INNER);
		}

		pItem = pItem->GetNext();
	}

	return Q_TRUE;
}

RETURN_E TiXmlElement* CItemGroup::WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible)
{
	LPSTR lpString = NULL;
	TiXmlText *pText = NULL;

	// 根节点
	TiXmlElement *pGroup = new TiXmlElement(bMaxCompatible ? xml_lpszNode_Group : xml_lpszNode_Question);
	if (!pGroup)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pParent->LinkEndChild(pGroup);

	if (!bMaxCompatible)
	{
		pGroup->SetAttribute(xml_lpszAttribute_Type, XMLGetTypeAttribute(GetType()));
		pGroup->SetAttribute(xml_lpszAttribute_Custom, m_nType / TYPE_CUSTOMRIGHT);
	}
	if (m_nQNumber)
	{
		pGroup->SetAttribute(xml_lpszAttribute_QNum, m_nQNumber);
	}

	TiXmlElement *pNode;
	// 类型描述，默认保存，不保存也可
	pNode = new TiXmlElement(xml_lpszNode_Type);
	if (!pNode)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pGroup->LinkEndChild(pNode);
	lpString = CTextManager::SwitchSave(m_szTypeDesc, nEncode, NULL);
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
	pNode->LinkEndChild(pText);

	// 题干描述，默认保存，不保存也可
	pNode = new TiXmlElement(xml_lpszNode_Description);
	if (!pNode)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pGroup->LinkEndChild(pNode);
	lpString = CTextManager::SwitchSave(GetDescription(), nEncode, NULL);
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
	pNode->LinkEndChild(pText);

	// 题干解析（可能为空）
	if (lstrlen(CItem::GetResolve()))
	{
		pNode = new TiXmlElement(xml_lpszNode_Resolve);
		if (!pNode)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pGroup->LinkEndChild(pNode);
		lpString = CTextManager::SwitchSave(CItem::GetResolve(), nEncode, NULL);
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
		pNode->LinkEndChild(pText);
	}

	// 保存图片
	if (!WriteItemImage(pGroup, nEncode, ITEMIMAGEFLAG_DESC) ||
		!WriteItemImage(pGroup, nEncode, ITEMIMAGEFLAG_RESOLVE))
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}

	// 子题目
	CItem *pItem = m_pHead;
	while(pItem)
	{
		if (!pItem->WriteXML(pGroup, nEncode, FALSE))
		{
			return Q_ERROR_RET(ERROR_INNER, NULL);
		}

		pItem = pItem->GetNext();
	}

	return Q_TRUE_RET(pGroup);
}

EBOOL CItemGroup::ReadXML(TiXmlElement *pGrp, int nEncode, BOOL bGroup)
{
	LPTSTR lpString = NULL;
	int nLoad;

	if (!pGrp->Attribute(xml_lpszAttribute_QNum, &nLoad))
	{
		nLoad = 0;
	}
	m_nQNumber = nLoad;

	// 类型描述（允许不存储）
	TiXmlElement *pNode = pGrp->FirstChildElement(xml_lpszNode_Type);
	if (pNode && pNode->GetText())
	{
		lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
		if (!lpString)
		{
			return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
		}
		SetTypeDesc(lpString);
		free(lpString);
	}

	if (!CItem::ReadXML(pGrp, nEncode, TRUE))
	{
		return Q_WARNING(ERROR_INNER);
	}

	// 子题目
	CItem *pItem = NULL;
	pNode = pGrp->FirstChildElement(xml_lpszNode_Question);
	while(pNode)
	{
		switch(XMLGetTypeIndex(pNode->Attribute(xml_lpszAttribute_Type)))
		{
		case TYPE_SCHOISE:
			pItem = new CChoise(TRUE);
			break;
			
		case TYPE_MCHOISE:
			pItem = new CChoise(FALSE);
			break;
			
		case TYPE_JUDGE:
			pItem = new CJudge;
			break;
			
		case TYPE_BLANK:
			pItem = new CBlank;
			break;
			
		case TYPE_TEXT:
			pItem = new CText;
			break;
			
		default:
			pItem = NULL;
		}
		
		if (pItem)
		{
			if (!pItem->ReadXML(pNode, nEncode, TRUE) || !AddItem(pItem))
			{
				delete pItem;
			}
			else
			{
				pItem->CalcCRC();
			}
		}
		
		pNode = pNode->NextSiblingElement(xml_lpszNode_Question);
	}

	return Q_RET(GetItemCount() ? TRUE : FALSE);
}

BOOL CItemGroup::AddItem(CItem *pItem)
{
	if (m_nItemCount > MAX_ITEMGROUP_NUM)
	{
		// 这里就不清理可能加不进去的对象了
		// 因为列表在销毁时一定会销毁所有对象
		// 不论列表的销毁方式如何
		return FALSE;
	}

	if (!pItem)
	{
		return FALSE;
	}
	pItem->SetCustomIndex(0);
	return CList::AddItem(pItem);
}

void CItemGroup::ClearSubItemFrom()
{
	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->ClearFromPtr();
		pItem = pItem->GetNext();
	}
}

void CItemGroup::ClearAllInfo()
{
	CItem::ClearAllInfo();

	ClearAllItems(TRUE);
}

void CItemGroup::ClearUserInfo()
{
	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->ClearUserInfo();
		pItem = pItem->GetNext();
	}
}

CItem* CItemGroup::Alloc()
{
	CItemGroup *pGroup = new CItemGroup;
	if (!pGroup)
	{
		return NULL;
	}

	if (!pGroup->CopyItem(this))
	{
		delete pGroup;
		return NULL;
	}

	return pGroup;
}

BOOL CItemGroup::CopyItem(CItem *pItem)
{
	if (pItem->GetType() != TYPE_GROUP)
	{
		return FALSE;
	}
	CItemGroup *pGroup = (CItemGroup*)pItem;

	ClearAllInfo();
	if (!CItem::CopyItem(pItem))
	{
		return FALSE;
	}
	// 这里题号需要被复制，用于题目的编辑
	m_nQNumber = pGroup->m_nQNumber;

	CItem *pCopy = pGroup->GetHead();
	CItem *pNew = NULL;
	while(pCopy)
	{
		pNew = pCopy->Alloc();
		if (!pNew)
		{
			return FALSE;
		}
		AddItem(pNew);

		pCopy = pCopy->GetNext();
	}

	return TRUE;
}

void CItemGroup::ClipCopy(CFixedSharedFile &sf)
{
	CItem::ClipCopy(sf);

	// 复制类型名称
	int nLen = lstrlen(m_szTypeDesc);
	sf.Write(&nLen, sizeof(nLen));
	sf.Write(m_szTypeDesc, nLen * sizeof(TCHAR));

	CList::ClipCopy(sf);
}

BOOL CItemGroup::ClipPaste(CFixedSharedFile &sf)
{
	if (!CItem::ClipPaste(sf))
	{
		return FALSE;
	}

	int nLen = 0;
	CString string;
	sf.Read(&nLen, sizeof(nLen));
	sf.Read(string.GetBufferSetLength(nLen), nLen * sizeof(TCHAR));
	SetTypeDesc(string); // 不检查是否成功了
	
	return CList::ClipPaste(sf);
}

void CItemGroup::MakeText(_stdstring &str, UINT nStyle, int nPara)
{
	_stdstring stdStrTemp;
	CString strTemp;
	TCHAR szIndex[8] = {0};
	CItem *pItem = m_pHead;
	PTESTPARA pPara = NULL;

	int nIndex = m_nStartIndex;
	BOOL bFromBegin = (nIndex == 1);

	if (lstrlen(GetDescription()))
	{
		str = m_strDescription;
		str += CTextManager::s_szWinReturn;
	}

	BOOL bEndLine = (nStyle & MKTXT_ENDLINE);
	nStyle &= ~MKTXT_ENDLINE;
	nStyle |= MKTXT_OPTIONNEWLINE |
			  MKTXT_OPTIONLEN |
			  MKTXT_JUDGEBRACKET;

	while(pItem)
	{
		if (bFromBegin)
		{
			_sntprintf(szIndex, _countof(szIndex), _T("(%d) "), nIndex++);
		}
		else
		{
			_sntprintf(szIndex, _countof(szIndex), _T("%d. "), nIndex++);
		}
		str += szIndex;

		pItem->MakeText(stdStrTemp, nStyle, 50);
		str += stdStrTemp;

		pItem = pItem->GetNext();
		if (pItem)
		{
			str += CTextManager::s_szWinReturn;
		}
	}

	if (bEndLine)
	{
		str +=CTextManager::s_szWinReturn;
	}
}

void CItemGroup::MakeAnswer(_stdstring &str, UINT nStyle, int nPara /* = 0 */)
{
	TCHAR szIndex[8] = {0};
	_stdstring stdStrTemp;
	CItem *pItem = m_pHead;

	int nIndex;
	BOOL bFromBegin;
	if (nPara <= 0)
	{
		nIndex = 1;
		bFromBegin = TRUE;
	}
	else
	{
		nIndex = nPara;
		bFromBegin = FALSE;
	}

	if (nStyle & MKASW_ASWINQUESTION)
	{
		str = m_strDescription;
		str += CTextManager::s_szWinReturn;

		if (MKASW_WITHRESOLVE & nStyle)
		{
			str += GetResolve();
			str += CTextManager::s_szWinReturn;
		}
	}
	else
	{
		str.erase();
	}

	int nCount = 0;
	BOOL bRet = FALSE;
	while(pItem)
	{
		if (bFromBegin)
		{
			_sntprintf(szIndex, _countof(szIndex), _T("(%d) "), nIndex++);
		}
		else
		{
			_sntprintf(szIndex, _countof(szIndex), _T("%d. "), nIndex++);
		}
		str += szIndex;
		nCount++;

		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
			pItem->MakeAnswer(stdStrTemp,
				(nStyle & MKASW_ASWINQUESTION) |
				((nStyle & MKASW_ASWINQUESTION) ? MKASW_ENDLINE : 0) |
				MKTXT_OPTIONNEWLINE | MKTXT_OPTIONSINALINE);
			break;
			
		case TYPE_BLANK:
		case TYPE_TEXT:
			pItem->MakeAnswer(stdStrTemp,
				(nStyle & MKASW_ASWINQUESTION) |
				((nStyle & MKASW_ASWINQUESTION) ? MKASW_ENDLINE : 0), -1);
			break;
			
		default:
			ASSERT(FALSE);
		}
		str += stdStrTemp;

		if (nStyle & MKASW_ASWINQUESTION)
		{
			bRet = TRUE;
		}
		else
		{
			if (!(nCount % 5) || pItem->GetType() == TYPE_TEXT)
			{
				str += CTextManager::s_szWinReturn;
				bRet = TRUE;
			}
			else
			{
				bRet = FALSE;
			}
		}

		pItem = pItem->GetNext();
	}

	if (nStyle & MKASW_ENDLINE && !bRet)
	{
		str += CTextManager::s_szWinReturn;
	}
}

BOOL CItemGroup::CheckItemValid()
{
	if (!m_nItemCount)
	{
		return FALSE;
	}

	CItem *pItem = m_pHead;
	while(pItem)
	{
		if (!pItem->CheckItemValid())
		{
			return FALSE;
		}
		pItem = pItem->GetNext();
	}

	return TRUE;
}

int CItemGroup::GetFullMark(CTestPara *pCPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();
	int i = 0;

	CItem *pItem = m_pHead;
	while(pItem)
	{
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
			i += pPara->nSChPoints;
			break;
			
		case TYPE_MCHOISE:
			i += pPara->nMChPoints;
			break;
			
		case TYPE_JUDGE:
			i += pPara->nJdgPoints;
			break;
			
		case TYPE_BLANK:
			i += pPara->nBlkPoints * ((CBlank*)pItem)->GetBlankCount();
			break;
			
		case TYPE_TEXT:
			i += pPara->nTxtPoints;
			break;
			
		default:
			ASSERT(FALSE);
		}
		pItem = pItem->GetNext();
	}

	return i;
}

int CItemGroup::GetAtomCount()
{
	int nCount = 0;
	CItem *pItem = m_pHead;
	while(pItem)
	{
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
		case TYPE_TEXT:
			++nCount;
			break;

		case TYPE_BLANK:
			nCount += ((CBlank*)pItem)->GetBlankCount();
			break;
			
		default:
			ASSERT(FALSE);
		}
		pItem = pItem->GetNext();
	}
	
	return nCount;
}

int CItemGroup::Score(CTestPara *pCPara)
{
	int nRet = 0;
	CItem *pItem = m_pHead;
	while(pItem)
	{
		nRet += pItem->Score(pCPara);
		pItem = pItem->GetNext();
	}

	return nRet;
}

BOOL CItemGroup::PointSubItem(CTempList *pList)
{
	CItem *pItem = GetHead();
	while(pItem)
	{
		if (!pList->AddItem(pItem))
		{
			return FALSE;
		}
		pItem = pItem->GetNext();
	}

	return TRUE;
}

void CItemGroup::MakeTextOut(_stdstring &str,
							 int nListIndex,
							 int nItemIndex,
							 PTPARAS pImagePara,
							 BOOL bWithAnswer,
							 BOOL bWithRes)
{
	TCHAR sz[128] = {0};
	TCHAR format[128] = {0};
	if (GetItemCount() < 2)
	{
		LoadString(AfxGetInstanceHandle(), IDS_TEST_GROUPCAUTIONS, format, 128);
		_sntprintf(sz, 128, format, nItemIndex);
	}
	else
	{
		LoadString(AfxGetInstanceHandle(), IDS_TEST_GROUPCAUTIONM, format, 128);
		_sntprintf(sz, 128, format, nItemIndex, nItemIndex + GetItemCount() - 1);
	}
	str += sz;
	str += CTextManager::s_szWinReturn;
	str += GetDescription();
	str += CTextManager::s_szWinReturn;
	
	MakeTextImage(str, nListIndex, pImagePara, !bWithAnswer);
	str += CTextManager::s_szWinReturn;

	int i;
	for(i = 0; i < GetItemCount(); ++i)
	{
		CItem *pItem = GetItem(i);
		if (!pItem)
		{
			break;
		}
		pItem->MakeTextOut(str, nListIndex, nItemIndex, pImagePara,
			bWithAnswer, bWithRes);
		str += CTextManager::s_szWinReturn;
		++nItemIndex;
	}
}

void CItemGroup::MakeHTML(FILE *fp,
						  _stdstring &str,
						  int nListIndex,
						  int nItemIndex,
						  PTPARAS pImagePara,
						  BOOL bWithAnswer,
						  BOOL bWithRes)
{
	TCHAR sz[128] = {0};
	TCHAR format[128] = {0};
	if (GetItemCount() < 2)
	{
		LoadString(AfxGetInstanceHandle(), IDS_TEST_GROUPCAUTIONS, format, 128);
		_sntprintf(sz, 128, format, nItemIndex);
	}
	else
	{
		LoadString(AfxGetInstanceHandle(), IDS_TEST_GROUPCAUTIONM, format, 128);
		_sntprintf(sz, 128, format, nItemIndex, nItemIndex + GetItemCount() - 1);
	}
	str += _T("<div class=\"txt\" style=\"font-weight:bold;\">");
	str += sz;
	str += _T("</div>\n");

	_stdstring ss;
	ss = GetDescription();
	CItemGroup::ReplaceHTMLEndLine(ss);
	str += ss;

	if (m_DescImageList.GetDataCount())
	{
		MakeHTMLImage(fp, str, nListIndex, pImagePara, !bWithAnswer);
	}
	str += _T("</div>\n");

	int i;
	for (i = 0; i < GetItemCount(); ++i)
	{
		CItem *pItem = GetItem(i);
		if (!pItem)
		{
			break;
		}
		pItem->MakeHTML(fp, str, nListIndex, nItemIndex, pImagePara,
			bWithAnswer, bWithRes);
		++nItemIndex;
	}
}

PVOID CItemGroup::GetUserAnswer(int nItem, int nIndex)
{
	CItem *pItem = GetItem(nItem);
	if (!pItem)
	{
		return NULL;
	}
	
	switch(pItem->GetType())
	{
	case TYPE_SCHOISE:
	case TYPE_MCHOISE:
		return (PVOID)((CChoise*)pItem)->GetUserAnswer(nIndex);
		
	case TYPE_JUDGE:
		return (PVOID)((CJudge*)pItem)->GetUserAnswer();
		
	case TYPE_BLANK:
		return (PVOID)((CBlank*)pItem)->GetUserAnswer(nIndex);
		break;
		
	case TYPE_TEXT:
		return (PVOID)((CText*)pItem)->GetUserAnswer();
		
	default:
		ASSERT(FALSE);
		return NULL;
	}
}

int CItemGroup::CheckAnswer(int nItem, int *nRightCount /* = NULL */)
{
	CItem *pItem = GetItem(nItem);
	if (!pItem)
	{
		return WRONG;
	}

	return pItem->CheckAnswer(nRightCount);
}

BOOL CItemGroup::IsAnswered(int nItem)
{
	CItem *pItem = GetItem(nItem);
	if (!pItem)
	{
		return FALSE;
	}

	return pItem->IsAnswered();
}

LPCTSTR CItemGroup::GetResolve()
{
	if (lstrlen(CItem::GetResolve()))
	{
		m_strGResolve = CItem::GetResolve();
		m_strGResolve += CTextManager::s_szWinReturn;
	}
	else
	{
		m_strGResolve.erase();
	}
	
	TCHAR szIndex[8] = {0};
	int nIndex = 1;
	CItem *pItem = m_pHead;
	while(pItem)
	{
		if (lstrlen(pItem->GetResolve()))
		{
			_sntprintf(szIndex, _countof(szIndex), _T("(%d) "), nIndex++);
			m_strGResolve += szIndex;
			m_strGResolve += pItem->GetResolve();
			if (pItem->GetNext())
			{
				m_strGResolve += CTextManager::s_szWinReturn;
			}
		}

		pItem = pItem->GetNext();
	}

	return m_strGResolve.c_str();
}

BOOL CItemGroup::Match(LPCTSTR strMatch, BOOL bMatchCase)
{
	if (CItem::Match(strMatch, bMatchCase))
	{
		return TRUE;
	}

	CItem *pItem = m_pHead;
	while(pItem)
	{
		if (pItem->Match(strMatch, bMatchCase))
		{
			return TRUE;
		}
		pItem = pItem->GetNext();
	}

	return FALSE;
}

void CItemGroup::CalcCRC()
{
	CItem::CalcCRC();

	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->CalcCRC();
		m_wCRC += pItem->GetCRC();

		pItem = pItem->GetNext();
	}
}

void CItemGroup::ReplaceHTMLEndLine(_stdstring &str)
{
	str.insert(0, _T("<div class=\"txt\">"));
	while(1)
	{
		int i = str.find(_T('\n'));
		if (i < 0)
		{
			break;
		}
		str.replace(i, 1, _T("</div><div class=\"txt\">"));
	}
	while(1)
	{
		int i = str.find(_T('\r'));
		if (i < 0)
		{
			break;
		}
		str.erase(i, 1);
	}
	str.append(_T("</div>\n"));
}