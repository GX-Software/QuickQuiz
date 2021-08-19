// TMessageBox.h: interface for the CTMessageBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMESSAGEBOX_H__AC2090E6_92B4_4C09_BDE7_FACF6CD71268__INCLUDED_)
#define AFX_TMESSAGEBOX_H__AC2090E6_92B4_4C09_BDE7_FACF6CD71268__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClrDialog.h"

#define MESSAGE_MAXLEN			255

#define MB_CANCELTRYCONTINUE	0x00000006L
#define IDTRYAGAIN				10
#define IDCONTINUE				11

class CTMessageBox  
{
public:
	CTMessageBox();
	virtual ~CTMessageBox();

	int SuperMessageBox(HWND hWnd,
						UINT nFormatID,
						UINT nCaptionID,
						UINT uType,
						UINT uDefaultType,
						UINT dwSecond, ...);
	int SuperMessageBox(HWND hWnd,
						LPCTSTR lpText,
						UINT nCaptionID,
						UINT uType,
						UINT uDefaultType,
						UINT dwSecond);
	
protected:
	int SuperMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, UINT uDefaultType, UINT dwSecond);

	static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI BoxProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI BtnProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void CALLBACK TimerProc(HWND hwnd,UINT uMsg,UINT idEvent, DWORD dwTime);

	static int HandleResult(int nResult, LPARAM hCtrl);
	
	static TCHAR m_strMessage[MESSAGE_MAXLEN + 1];
	static TCHAR m_strCaption[MESSAGE_MAXLEN + 1];
	static UINT m_uType;
	
	static HHOOK m_hWndHook;
	static HWND m_hWnd;
	static HWND m_hTxtWnd;
	
	static int m_nTime;
	static UINT m_uDefault;
};

#endif // !defined(AFX_TMESSAGEBOX_H__AC2090E6_92B4_4C09_BDE7_FACF6CD71268__INCLUDED_)
