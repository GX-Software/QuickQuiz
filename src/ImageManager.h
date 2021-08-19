
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
按info中的信息生成DDB位图，并保存在pBmp中
*/
EBOOL FUNCCALL ShowDefaultCover(HDC hDC, CMemBmp *pBmp, LPPAPERCOVERINFO info);

/*
按info中的信息生成Jpeg格式数据，保存在info中
*/
EBOOL FUNCCALL MakeJpegCover(HDC hDC, LPPAPERCOVERINFO info);

/*
读取图像信息并生成DDB位图，保存在pBmp中
*/
BOOL FUNCCALL ShowImage(HDC hDC, CMemBmp *pBmp, int nImageType, PBYTE pImageData, size_t ulImageSize);

/*
读取图像的大小，保存在pImageSize中
*/
BOOL FUNCCALL ReadImageSize(int nImageType, PBYTE pImage, size_t ulImageSize, LPSIZE pImageSize);

/*
自己实现的透明绘图，适配Windows95
*/
void FUNCCALL TransparentBlt2(HDC hdcDest,      // 目标DC
							  int nXOriginDest,   // 目标X偏移
							  int nYOriginDest,   // 目标Y偏移
							  int nWidthDest,     // 目标宽度
							  int nHeightDest,    // 目标高度
							  HDC hdcSrc,         // 源DC
							  int nXOriginSrc,    // 源X起点
							  int nYOriginSrc,    // 源Y起点
							  int nWidthSrc,      // 源宽度
							  int nHeightSrc,     // 源高度
							  UINT crTransparent  // 透明色,COLORREF类型
							  );

#ifdef __cplusplus
}
#endif

#endif
