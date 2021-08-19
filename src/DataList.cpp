// DataList.cpp: implementation of the CDataList class and CData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

class CData
{
public:
	CData(PVOID pData, _dataclear clear = NULL) :
		m_pPrev(NULL), m_pNext(NULL), m_pParent(NULL),
		m_pData(pData), m_funcClear(clear) {}

	virtual ~CData()
	{
		ClearData();
	}
	
	inline void SetPrev(CData *pPrev) { m_pPrev = pPrev; }
	inline CData* GetPrev() { return (m_pPrev); }
	
	inline void SetNext(CData *pNext) { m_pNext = pNext; }
	inline CData* GetNext() { return (m_pNext); }
	
	inline CDataList* GetParent() { return (m_pParent); }
	inline void SetParent(CDataList *pParent) { m_pParent = pParent; }
	
	inline PVOID GetData() { return (m_pData); }
	void SetData(PVOID pData, _dataclear clear = NULL)
	{
		if (m_pData)
		{
			ClearData();
		}
		m_pData = pData;
		m_funcClear = clear;
	}
	void ClearData()
	{
		if (m_funcClear)
		{
			m_funcClear(m_pData);
			m_pData = NULL;
			m_funcClear = NULL;
		}
	}
	
private:
	PVOID m_pData;
	_dataclear m_funcClear;
	
	CDataList *m_pParent;
	CData *m_pPrev;
	CData *m_pNext;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataList::CDataList(size_t ulLimit /* = 0 */) :
	m_ulLimitCount(ulLimit), m_pHead(NULL), m_pTail(NULL),
	m_ulDataCount(0),
	m_nLastItem(-1), m_pLastItem(NULL)
{
}

CDataList::~CDataList()
{
	ClearData();
}

/*
功能：	向列表中添加数据对象
返回值：添加成功则返回TRUE，否则返回FALSE
备注：	当nIndex为-1时，添加到队列尾，否则，添加到nIndex指定位置
*/
BOOL CDataList::AddData(int nIndex, PVOID pData, _dataclear funcClear /* = NULL */)
{
	if (!pData || (m_ulLimitCount && m_ulDataCount >= m_ulLimitCount))
	{
		return FALSE;
	}

	CData *pAdd = new CData(pData, funcClear);
	if (!pAdd)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	
	return AddData(pAdd, nIndex);
}

/*
功能：	设置列表中某项的值
返回值：设置成功则返回TRUE，否则返回FALSE
备注：	当nIndex为-1、或大于实际列表大小时，在队列尾添加
		否则，在指定位置设置数值，并将清理之前的值
*/
BOOL CDataList::SetData(int nIndex, PVOID pData, _dataclear funcClear /* = NULL */)
{
	if (m_ulLimitCount && m_ulDataCount >= m_ulLimitCount)
	{
		return FALSE;
	}

	if (nIndex >= m_ulDataCount || nIndex < 0)
	{
		return AddData(-1, pData, funcClear);
	}
	else
	{
		CData *p = GetDataItem(nIndex);
		p->SetData(pData, funcClear);
		return TRUE;
	}
}

BOOL CDataList::DeleteData(int nIndex)
{
	CData *pData = RemoveData(nIndex);
	if (!pData)
	{
		return FALSE;
	}
	
	delete pData;
	return TRUE;
}

BOOL CDataList::MoveData(CDataList *pList, int nSrcIndex, int nDstIndex)
{
	CData *pData = pList->RemoveData(nSrcIndex);
	if (!pData)
	{
		return FALSE;
	}
	
	AddData(pData, nDstIndex);
	return TRUE;
}

PVOID CDataList::GetData(int nIndex)
{
	CData *pData = GetDataItem(nIndex);
	if (pData)
	{
		return pData->GetData();
	}
	return NULL;
}

void CDataList::ClearData()
{
	while(m_ulDataCount)
	{
		DeleteData(0);
	}
}

BOOL CDataList::AddData(CData *pData, int nIndex)
{
	if (!pData || (m_ulLimitCount && m_ulDataCount >= m_ulLimitCount))
	{
		return FALSE;
	}
	
	CData *pIn = GetDataItem(nIndex);
	if (!pIn)
	{
		if (!m_pHead)
		{
			ASSERT(!m_pTail);
			m_pHead = m_pTail = pData;
			
			pData->SetNext(NULL);
			pData->SetPrev(NULL);
		}
		else
		{
			ASSERT(m_pTail);
			m_pTail->SetNext(pData);
			
			pData->SetPrev(m_pTail);
			pData->SetNext(NULL);
			
			m_pTail = pData;
		}
	}
	else
	{
		pData->SetPrev(pIn->GetPrev());
		if (pIn->GetPrev())
		{
			pIn->GetPrev()->SetNext(pData);
		}
		else
		{
			m_pHead = pData;
		}
		
		pIn->SetPrev(pData);
		pData->SetNext(pIn);
	}
	
	pData->SetParent(this);
	++m_ulDataCount;

	m_nLastItem = -1;
	m_pLastItem = NULL;
	return TRUE;
}

CData* CDataList::RemoveData(int nIndex)
{
	CData *pData = GetDataItem(nIndex);
	if (!pData || !m_pHead || pData->GetParent() != this)
	{
		return NULL;
	}
	
	CData *pPrev = pData->GetPrev();
	CData *pNext = pData->GetNext();
	
	if (pPrev)
	{
		pPrev->SetNext(pNext);
	}
	else
	{
		m_pHead = pNext;
	}
	
	if (pNext)
	{
		pNext->SetPrev(pPrev);
	}
	else
	{
		m_pTail = pPrev;
	}
	
	--m_ulDataCount;

	m_nLastItem = -1;
	m_pLastItem = NULL;
	return pData;
}

CData* CDataList::GetDataItem(int nIndex)
{
	if (nIndex >= m_ulDataCount || nIndex < 0)
	{
		return NULL;
	}
	
	int nStart = m_nLastItem;
	CData *pStart = m_pLastItem;
	if (!pStart)
	{
		pStart = m_pHead;
		nStart = 0;
	}

	if (nStart <= nIndex)
	{
		int i;
		for (i = nStart + 1; i <= nIndex; ++i)
		{
			pStart = pStart->GetNext();
		}
	}
	else
	{
		int i;
		for (i = nStart - 1; i >= nIndex; --i)
		{
			pStart = pStart->GetPrev();
		}
	}
	
	m_nLastItem = nIndex;
	m_pLastItem = pStart;
	return pStart;
}
