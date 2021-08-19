
#ifndef _IMAGE_MANAGER_H_
#define _IMAGE_MANAGER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemBmp.h"

#define JPEGCOVER_DEFAULTWIDTH		300
#define JPEGCOVER_DEFAULTHEIGHT		300
#define JPEGCOVER_DEFAULTBKCOLOR	(RGB(255, 255, 255))
#define JPEGCOVER_DEFAULTTXTCOLOR	(RGB(0, 0, 0))
#define JPEGCOVER_DEFAULTQUALITY	85

#ifdef __cplusplus
extern "C" {
#endif

/*
��info�е���Ϣ����DDBλͼ����������pBmp��
*/
EBOOL FUNCCALL ShowDefaultCover(HDC hDC, CMemBmp *pBmp, LPPAPERCOVERINFO info);

/*
��info�е���Ϣ����Jpeg��ʽ���ݣ�������info��
*/
EBOOL FUNCCALL MakeJpegCover(HDC hDC, LPPAPERCOVERINFO info);

/*
��ȡͼ����Ϣ������DDBλͼ��������pBmp��
*/
BOOL FUNCCALL ShowImage(HDC hDC, CMemBmp *pBmp, int nImageType, PBYTE pImageData, size_t ulImageSize);

/*
��ȡͼ��Ĵ�С��������pImageSize��
*/
BOOL FUNCCALL ReadImageSize(int nImageType, PBYTE pImage, size_t ulImageSize, LPSIZE pImageSize);

/*
�Լ�ʵ�ֵ�͸����ͼ������Windows95
*/
void FUNCCALL TransparentBlt2(HDC hdcDest,      // Ŀ��DC
							  int nXOriginDest,   // Ŀ��Xƫ��
							  int nYOriginDest,   // Ŀ��Yƫ��
							  int nWidthDest,     // Ŀ����
							  int nHeightDest,    // Ŀ��߶�
							  HDC hdcSrc,         // ԴDC
							  int nXOriginSrc,    // ԴX���
							  int nYOriginSrc,    // ԴY���
							  int nWidthSrc,      // Դ���
							  int nHeightSrc,     // Դ�߶�
							  UINT crTransparent  // ͸��ɫ,COLORREF����
							  );

#ifdef __cplusplus
}
#endif

#endif
