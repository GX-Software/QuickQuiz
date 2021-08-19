// DataList.h: interface for the CDataList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATALIST_H__598A1532_B9F2_4FCA_9FF1_DA2A3497C0B5__INCLUDED_)
#define AFX_DATALIST_H__598A1532_B9F2_4FCA_9FF1_DA2A3497C0B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int (FUNCCALL *_dataclear)(PVOID p);

class CData;
class CDataList  
{
public:
	CDataList(size_t ulLimit = 0);
	virtual ~CDataList();

	BOOL AddData(int nIndex, PVOID pData, _dataclear funcClear = NULL);
	BOOL SetData(int nIndex, PVOID pData, _dataclear funcClear = NULL);

	/* 将数据销毁 */
	BOOL DeleteData(int nIndex);
	/*
	将数据从pList中移除，并添加到本列表中
	当nDstIndex为-1时，添加到末尾
	*/
	BOOL MoveData(CDataList *pList, int nSrcIndex, int nDstIndex);
	void ClearData();

	PVOID GetData(int nIndex);
	inline size_t GetDataCount() { return m_ulDataCount; }
	inline size_t GetLimitCount() { return m_ulLimitCount; }

protected:
	CData* GetDataItem(int nIndex);
	BOOL AddData(CData *pData, int nIndex);

	/* 将数据移除，并返回 */
	CData* RemoveData(int nIndex);

protected:
	CData *m_pHead;
	CData *m_pTail;
	size_t m_ulDataCount;
	size_t m_ulLimitCount; // 当此值为0时，不限制数量

private:
	int m_nLastItem;
	CData *m_pLastItem;
};

#endif // !defined(AFX_DATALIST_H__598A1532_B9F2_4FCA_9FF1_DA2A3497C0B5__INCLUDED_)
