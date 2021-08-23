
#include "stdafx.h"
#include "ImageManager.h"

#include "jpeglib.h"
#include "png.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
功能：	绘制默认封面预览
返回值：成功返回TRUE，否则返回FALSE
备注：
*/
static EBOOL FUNCCALL DrawTitle(HDC dc, LPPAPERCOVERINFO info)
{
	LOGFONT font;
	COLORREF oldClr = ::SetTextColor(dc, info->clrText);
	int oldMode = ::SetBkMode(dc, TRANSPARENT);
	HFONT hFont, hOldFont;
	UINT uOldAlign;
	int nFit = 0;
	SIZE size;
	LPTSTR str = info->strTitle;
	int nDrawHeight = 0, nLineIndex = 0, nCharIndex = 0;
	int nCharCount[10] = {0};
	
	if (!GetObject(g_hNormalFont, sizeof(LOGFONT), &font))
	{
		return Q_ERROR(ERROR_RESOURCE_NOT_EXIST);
	}
	font.lfHeight = info->nHeight / 6 * (-1);
	font.lfWeight = FW_BOLD;
	
	hFont = CreateFontIndirect(&font);
	if (!hFont)
	{
		return Q_ERROR(ERROR_RESOURCE_NOT_EXIST);
	}
	hOldFont = (HFONT)::SelectObject(dc, hFont);
	uOldAlign = ::SetTextAlign(dc, TA_CENTER);

	// 获得每行的字符数，及总显示高度
	while(1)
	{
		if (!GetTextExtentExPoint(dc, info->strTitle + nCharIndex, lstrlen(info->strTitle + nCharIndex),
			info->nWidth / 10 * 9, &nFit, NULL, &size))
		{
			goto fail;
		}
		if (nDrawHeight + size.cy > info->nHeight)
		{
			break;
		}
		nDrawHeight += size.cy;
		nCharCount[nLineIndex++] = nFit;
		nCharIndex += nFit;
		if (nCharIndex >= lstrlen(info->strTitle))
		{
			break;
		}
	}

	nCharIndex = 0;
	nLineIndex = 0;
	nDrawHeight = (info->nHeight - nDrawHeight) / 2;
	while(1)
	{
		if (!nCharCount[nLineIndex])
		{
			break;
		}
		TextOut(dc, info->nWidth / 2, nDrawHeight, info->strTitle + nCharIndex, nCharCount[nLineIndex]);
		nCharIndex += nCharCount[nLineIndex];
		nDrawHeight += size.cy;
		++nLineIndex;
	}
	
	::SetTextAlign(dc, uOldAlign);
	::SetTextColor(dc, oldClr);
	::SetBkMode(dc, oldMode);
	::DeleteObject(::SelectObject(dc, hOldFont));
	return Q_TRUE;

fail:
	::SetTextAlign(dc, uOldAlign);
	::SetTextColor(dc, oldClr);
	::SetBkMode(dc, oldMode);
	::DeleteObject(::SelectObject(dc, hOldFont));
	return Q_ERROR(ERROR_DRAWING_FAIL);
}

EBOOL FUNCCALL ShowDefaultCover(HDC hDC, CMemBmp *pBmp, LPPAPERCOVERINFO info)
{
	ASSERT(hDC);
	RECT rc = {0, 0, info->nWidth, info->nHeight};
	
	HDC hBmpDC = pBmp->CreateBmp(hDC, &rc, TRUE, info->clrBk);
	ReleaseDC(NULL, hDC);
	if (!hBmpDC)
	{
		return Q_ERROR(ERROR_CREATE_DC_FAIL);
	}
	if (!DrawTitle(hBmpDC, info))
	{
		return Q_ERROR(ERROR_COVER_CREATE_FAIL);
	}
	
	return Q_TRUE;
}

/* JPEG部分 */

typedef struct _tagJPEGToDIB
{
	size_t ulWidth;
	size_t ulHeight;
	
	LPBYTE pJpeg;
	size_t ulJpegSize;
	UINT uChannels;
	
	LPBYTE pDIB;
	size_t ulDIBSize;
} JPEGTODIB, *PJPEGTODIB;
static void FUNCCALL ClearJpegToDIB(PJPEGTODIB pJtD)
{
	if (pJtD->pDIB)
	{
		free(pJtD->pDIB);
		pJtD->pDIB = NULL;
	}
	pJtD->ulDIBSize = 0;
}

typedef struct _tagRGB24
{
	BYTE rgbRed;
	BYTE rgbGreen;
	BYTE rgbBlue;
}RGB24, *PRGB24;
typedef struct _tagRGB32
{
	BYTE rgbRed;
	BYTE rgbGreen;
	BYTE rgbBlue;
	BYTE rgbAlpha;
}RGB32, *PRGB32;
void CalcAlphaRGB(PRGB32 rgbIn, PRGB24 rgbOut);

/*
功能：	将RGB真彩色转换为DIB位图的像素
返回值：若分配的内存大小不够，则返回FALSE，否则返回TRUE
备注：	当pOut为空时，pInfo->bmiHeader.biSizeImage返回需要的空间大小
		本函数线程安全
*/
static BOOL FUNCCALL RGBToDIB(LPBYTE pRGB, UINT uChannels, LPBITMAPINFO pInfo, LPBYTE pOut)
{
	LPBITMAPINFOHEADER bmph = &pInfo->bmiHeader;
	long realHeight = bmph->biHeight;
	int a = bmph->biWidth * uChannels, b = bmph->biWidth * 3;
	size_t out = 0;

	if (realHeight < 0)
	{
		realHeight = 0 - realHeight;
	}
	if (b % 4)
	{
		b = b + (4 - (b % 4));
	}
	out = b * realHeight;
	if (!pOut)
	{
		pInfo->bmiHeader.biSizeImage = out;
		return TRUE;
	}
	else if (pInfo->bmiHeader.biSizeImage < out)
	{
		return FALSE;
	}

	ASSERT(pRGB);
	if (3 == uChannels)
	{
		int y, xOld, xNew;

		for (y = 0; y < realHeight; ++y)
		{
			for (xOld = 0, xNew = 0; xOld + 3 <= a; xOld += 3, xNew += 3)
			{
				pOut[y * b + xNew] = pRGB[y * a + xOld + 2];
				pOut[y * b + xNew + 1] = pRGB[y * a + xOld + 1];
				pOut[y * b + xNew + 2] = pRGB[y * a + xOld];
			}

			for (; xNew < b; ++xNew)
			{
				pOut[y * b + xNew] = 0;
			}
		}
	}
	else if (4 == uChannels)
	{
		RGB32 rgbIn;
		RGB24 rgbOut;
		int y, xOld, xNew;
		for (y = 0; y < realHeight; ++y)
		{
			for (xOld = 0, xNew = 0; xOld + 4 <= a; xOld += 4, xNew += 3)
			{
				rgbIn.rgbRed = pRGB[y * a + xOld];
				rgbIn.rgbGreen = pRGB[y * a + xOld + 1];
				rgbIn.rgbBlue = pRGB[y * a + xOld + 2];
				rgbIn.rgbAlpha = pRGB[y * a + xOld + 3];

				CalcAlphaRGB(&rgbIn, &rgbOut);

				pOut[y * b + xNew] = rgbOut.rgbBlue;
				pOut[y * b + xNew + 1] = rgbOut.rgbGreen;
				pOut[y * b + xNew + 2] = rgbOut.rgbRed;
			}
		}
	}
	return TRUE;
}

/*
功能：	将DIB位图的像素转换为24位RGB真彩色
返回值：若分配的内存大小不够，则返回FALSE，否则返回TRUE
备注：	当pOut为空时，ulOutSize返回需要的空间大小
		DIB位图是以BGR顺序存储颜色的
		本函数线程安全
*/
static BOOL FUNCCALL DIBToRGB(LPBYTE pDIB, LPBITMAPINFO pInfo, LPBYTE pOut, size_t *ulOutSize)
{
	LPBITMAPINFOHEADER bmph = &pInfo->bmiHeader;
	long realHeight = bmph->biHeight;
	int a = bmph->biWidth * bmph->biBitCount / 8, b = bmph->biSizeImage / bmph->biHeight, i = 0;
	size_t out = 0, size = 0;
	BYTE bb = 0;

	if (realHeight < 0)
	{
		realHeight = 0 - realHeight;
		b = bmph->biSizeImage / realHeight;
	}

	out = a * realHeight;
	if (!pOut)
	{
		*ulOutSize = out;
		return TRUE;
	}
	else if (*ulOutSize < out)
	{
		*ulOutSize = 0;
		return FALSE;
	}

	*ulOutSize = out;
	while(i < realHeight)
	{
		memcpy(pOut + a * i, pDIB + b * i, a);
		++i;
	}
	
	for (i = 0; i + 3 <= out; i += 3)
	{
		bb = pOut[i];
		pOut[i] = pOut[i + 2];
		pOut[i + 2] = bb;
	}
	return TRUE;
}

/*
功能：	将pJtD中保存的jpeg图像信息解压为RGB信息
返回值：成功返回TRUE，否则返回FALSE
备注：	当bReadSize为TRUE时，不进行解压操作，仅读取图像大小
*/
static BOOL FUNCCALL ReadJpeg(PJPEGTODIB pJtD, LPBITMAPINFO pBI, BOOL bReadSize)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW buffer;
	int row_stride;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, pJtD->pJpeg, pJtD->ulJpegSize);

	jpeg_read_header(&cinfo, TRUE);

	pJtD->ulWidth = cinfo.image_width;
	pJtD->ulHeight = cinfo.image_height;
	if (bReadSize)
	{
		jpeg_destroy_decompress(&cinfo);
		return TRUE;
	}
	
	pBI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBI->bmiHeader.biWidth = cinfo.image_width;
	pBI->bmiHeader.biHeight = 0 - cinfo.image_height;
	pBI->bmiHeader.biPlanes = 1;
	pBI->bmiHeader.biBitCount = 24;
	pBI->bmiHeader.biCompression = BI_RGB;

	pJtD->uChannels = cinfo.num_components;
	RGBToDIB(NULL, pJtD->uChannels, pBI, NULL);

	ClearJpegToDIB(pJtD);
	pJtD->ulDIBSize = cinfo.image_width * cinfo.num_components * cinfo.image_height;
	buffer = (JSAMPROW)malloc(pJtD->ulDIBSize);
	if (!buffer)
	{
		jpeg_destroy_decompress(&cinfo);
		return FALSE;
	}
	ASSERT(!pJtD->pDIB);
	pJtD->pDIB = buffer;
	
	jpeg_start_decompress(&cinfo);
	
	row_stride = cinfo.output_width * cinfo.output_components;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		jpeg_read_scanlines(&cinfo, &buffer, 1);
		buffer += row_stride;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	return TRUE;
}

static BOOL FUNCCALL MakeJpeg(const LPPAPERCOVERINFO info, const PBYTE inbuffer)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char *outbuffer = NULL;
	size_t outsize;
	JSAMPROW row_pointer[1] = {0};
	int row_stride;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_mem_dest(&cinfo, &outbuffer, &outsize);
	
	cinfo.image_width = info->nWidth;
	cinfo.image_height = info->nHeight;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, info->nQuality, TRUE);
	
	jpeg_start_compress(&cinfo, TRUE);
	
	row_stride = info->nWidth * 3;
	while(cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = inbuffer + (row_stride * cinfo.next_scanline);
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	
	info->lpData = (LPBYTE)malloc(outsize);
	if (!info->lpData)
	{
		return FALSE;
	}
	memcpy(info->lpData, outbuffer, outsize);
	info->ulDataSize = outsize;
	
	if (outbuffer)
	{
		free(outbuffer);
		outbuffer = NULL;
	}
	jpeg_destroy_compress(&cinfo);
	return TRUE;
}

/*
功能：	按设定要求制作默认jpeg封面图片
返回值：成功返回TRUE，否则返回FALSE
备注：	本函数非线程安全
*/
EBOOL FUNCCALL MakeJpegCover(HDC hDC, LPPAPERCOVERINFO info)
{
	CMemBmp Bmp;

	HDC hMakeDC = hDC;
	if (!hDC)
	{
		hMakeDC = GetDC(NULL);
	}
	if (!ShowDefaultCover(hMakeDC, &Bmp, info))
	{
		return Q_ERROR(ERROR_INNER);
	}
	if (!hDC)
	{
		ReleaseDC(NULL, hMakeDC);
	}

	BITMAPINFO DIBInfo = {0};
	DIBInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	DIBInfo.bmiHeader.biWidth = info->nWidth;
    DIBInfo.bmiHeader.biHeight = info->nHeight * (-1);
    DIBInfo.bmiHeader.biPlanes = 1;
    DIBInfo.bmiHeader.biBitCount = 24;
	DIBInfo.bmiHeader.biCompression = BI_RGB;
	if (!GetDIBits(Bmp.GetDC(), Bmp.GetBmp(), 0, info->nHeight, NULL, &DIBInfo, DIB_RGB_COLORS))
	{
		return Q_ERROR(ERROR_COVER_CREATE_FAIL);
	}

	LPBYTE pDIB = (LPBYTE)malloc(DIBInfo.bmiHeader.biSizeImage);
	if (!pDIB)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	if (!GetDIBits(Bmp.GetDC(), Bmp.GetBmp(), 0, info->nHeight, pDIB, &DIBInfo, DIB_RGB_COLORS))
	{
		free(pDIB);
		return Q_ERROR(ERROR_COVER_CREATE_FAIL);
	}

	size_t ulJpeg = 0;
	DIBToRGB(pDIB, &DIBInfo, NULL, &ulJpeg);
	LPBYTE pJpeg = (LPBYTE)malloc(ulJpeg);
	if (!pJpeg)
	{
		free(pDIB);
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}

	DIBToRGB(pDIB, &DIBInfo, pJpeg, &ulJpeg);
	free(pDIB);

	EBOOL bRet = MakeJpeg(info, pJpeg);
	free(pJpeg);
	if (!bRet)
	{
		return Q_ERROR(ERROR_JPEG_COMPRESS_FAIL);
	}

	return Q_TRUE;
}

static BOOL FUNCCALL ReadJpegSize(PBYTE pJpeg, size_t ulJpegSize, LPSIZE pImageSize)
{
	JPEGTODIB JtoD = {0};

	JtoD.pJpeg = pJpeg;
	JtoD.ulJpegSize = ulJpegSize;
	if (!ReadJpeg(&JtoD, NULL, TRUE))
	{
		return FALSE;
	}

	pImageSize->cx = JtoD.ulWidth;
	pImageSize->cy = JtoD.ulHeight;
	return TRUE;
}

static BOOL FUNCCALL ShowJpegImage(HDC hDC, CMemBmp *pBmp, PBYTE pJpeg, size_t ulJpegSize)
{
	BITMAPINFO BI = {0};
	JPEGTODIB JtoD = {0};
	PBYTE pDIB = NULL;
	
	ASSERT(hDC);
	JtoD.pJpeg = pJpeg;
	JtoD.ulJpegSize = ulJpegSize;
	if (!ReadJpeg(&JtoD, &BI, FALSE))
	{
		return FALSE;
	}
	pDIB = (PBYTE)malloc(BI.bmiHeader.biSizeImage);
	if (!pDIB)
	{
		ClearJpegToDIB(&JtoD);
		return FALSE;
	}
	if (!RGBToDIB(JtoD.pDIB, JtoD.uChannels, &BI, pDIB))
	{
		free(pDIB);
		ClearJpegToDIB(&JtoD);
		return FALSE;
	}
	ClearJpegToDIB(&JtoD);

	pBmp->CreateBmp(hDC, JtoD.ulWidth, JtoD.ulHeight);

	BOOL bRet = SetDIBits(hDC, pBmp->GetBmp(), 0, JtoD.ulHeight, pDIB, &BI, DIB_RGB_COLORS);
	free(pDIB);
	return bRet;
}

/* JPGE部分结束 */
/* DIB部分 */

/*
功能：	读取pDIB中保存的DIB图像的大小
返回值：成功返回TRUE，否则返回FALSE
备注：
*/
static BOOL FUNCCALL ReadDIBSize(LPBYTE pDIB, size_t ulDIBSize, LPSIZE pImageSize)
{
	LPBITMAPINFO pBI = NULL;
	LPBITMAPINFOHEADER pBIH = NULL;

	pBI = (LPBITMAPINFO)(pDIB + sizeof(BITMAPFILEHEADER));
	pBIH = (LPBITMAPINFOHEADER)pBI;

	pImageSize->cx = pBIH->biWidth;
	pImageSize->cy = pBIH->biHeight;
	if (pImageSize->cy < 0)
	{
		pImageSize->cy = 0 - pImageSize->cy;
	}
	return TRUE;
}

static BOOL FUNCCALL ShowDIBImage(HDC hDC, CMemBmp *pBmp, PBYTE pDIB, size_t ulDIBSize)
{
	LPBITMAPFILEHEADER pBFI = NULL;
	LPBITMAPINFO pBI = NULL;
	LPBITMAPINFOHEADER pBIH = NULL;
	PBYTE pData = NULL;
	long lHeight, paletteLen = 0;

	pBFI = (LPBITMAPFILEHEADER)pDIB;
	if (pBFI->bfType != 'MB' ||
		pBFI->bfSize < ulDIBSize)
	{
		return FALSE;
	}

	pBI = (LPBITMAPINFO)(pDIB + sizeof(BITMAPFILEHEADER));
	pBIH = (LPBITMAPINFOHEADER)pBI;
	lHeight = pBIH->biHeight;
	if (lHeight < 0)
	{
		lHeight = 0 - lHeight;
	}

	// 计算调色板大小，用于容错
	if (!pBIH->biClrUsed)
	{
		if (pBIH->biBitCount < 16)
		{
			paletteLen = (1 << pBIH->biBitCount) * sizeof(RGBQUAD);
		}
		else if (pBIH->biBitCount == 16 ||
				 pBIH->biBitCount == 32)
		{
			paletteLen = 3 * sizeof(DWORD);
		}
	}
	else
	{
		paletteLen = pBIH->biClrUsed * sizeof(RGBQUAD);
	}
	if (pBFI->bfOffBits != sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paletteLen)
	{
		return FALSE;
	}
	pData = pDIB + pBFI->bfOffBits;

	pBmp->CreateBmp(hDC, pBIH->biWidth, lHeight);
	return (SetDIBits(pBmp->GetDC(), pBmp->GetBmp(), 0, lHeight, pData, (LPBITMAPINFO)pBI, DIB_RGB_COLORS) == lHeight);
}

/* DIB部分结束 */
/* PNG部分 */

typedef struct _tagPNGToDIB
{
	size_t ulWidth;
	size_t ulHeight;

	PBYTE pPng;
	size_t ulPngSize;
	UINT uChannels;
	long lReadPos;

	PBYTE pDIB;
	size_t ulDIBSize;
}PNGTODIB, *PPNGTODIB;

static void CalcAlphaRGB(PRGB32 rgbIn, PRGB24 rgbOut)
{
	PBYTE pIn = (PBYTE)rgbIn;
	PBYTE pOut = (PBYTE)rgbOut;
	RGB32 rgbBk = {0};
	int nOut[3] = {0};
	PBYTE pBk = (PBYTE)&rgbBk;
	BYTE uAlpha = rgbIn->rgbAlpha;
	int i;

	rgbBk.rgbRed = GetRValue(AfxGetQColor(QCOLOR_EDITFACE));
	rgbBk.rgbGreen = GetGValue(AfxGetQColor(QCOLOR_EDITFACE));
	rgbBk.rgbBlue = GetBValue(AfxGetQColor(QCOLOR_EDITFACE));

	for (i = 0; i < 3; ++i)
	{
		nOut[i] = pBk[i] * (255 - uAlpha) + pIn[i] * uAlpha;
		pOut[i] = nOut[i] / 255;
	}
}

static void PngExceptError(png_structp png_ptr, png_const_charp msg)
{
	throw msg;
}

static void ClearPngToDIB(PPNGTODIB pPtD)
{
	if (pPtD->pDIB)
	{
		free(pPtD->pDIB);
		pPtD->pDIB = NULL;
	}
	pPtD->ulDIBSize = 0;
}

static void ReadPngBuffer(png_structp png_ptr, png_bytep data, png_size_t length)
{
	PPNGTODIB pData = (PPNGTODIB)png_get_io_ptr(png_ptr);

	if (pData->lReadPos + length > pData->ulPngSize)
	{
		throw _T("read png fail!");
	}
	else
	{
		memcpy(data, pData->pPng + pData->lReadPos, length);
		pData->lReadPos += length;
	}
}

/*
功能：	将pPtD中保存的png图像信息解压为RGB信息
返回值：成功返回TRUE，否则返回FALSE
备注：	当bReadSize为TRUE时，不进行解压操作，仅读取图像大小
*/
static BOOL ReadPng(PPNGTODIB pPtD, LPBITMAPINFO pBI, BOOL bReadSize)
{
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	int iBitDepth;
    int iColorType;
	double dGamma;
	png_color_16 *pBackground;
    png_uint_32 ulRowBytes;
	static png_byte **ppbRowPointers = NULL;
	int i;

	if (png_sig_cmp(pPtD->pPng, 0, 8))
	{
		return FALSE;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
		(png_error_ptr)PngExceptError, (png_error_ptr)NULL);
    if (!png_ptr)
    {
        return FALSE;
    }

	info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return FALSE;
    }
	
	try
	{
		png_set_read_fn(png_ptr, (png_voidp)pPtD, ReadPngBuffer);
		
		png_set_sig_bytes(png_ptr, 8);
		pPtD->lReadPos = 8;

		// 图片基本信息
		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr,
			&(pPtD->ulWidth), &(pPtD->ulHeight), &iBitDepth, &iColorType,
			NULL, NULL, NULL);

		if (bReadSize)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			return TRUE;
		}

		// 转换为RGB888模式
		if (iBitDepth == 16)
            png_set_strip_16(png_ptr);
        if (iColorType == PNG_COLOR_TYPE_PALETTE)
            png_set_expand(png_ptr);
        if (iBitDepth < 8)
            png_set_expand(png_ptr);
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
            png_set_expand(png_ptr);
        if (iColorType == PNG_COLOR_TYPE_GRAY ||
            iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png_ptr);

		// 设置背景色
		if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
        {
            png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
		}

		// 色彩校正？
		if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
            png_set_gamma(png_ptr, (double) 2.2, dGamma);

		// 重新读取图片信息
		png_read_update_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr,
			&(pPtD->ulWidth), &(pPtD->ulHeight), &iBitDepth, &iColorType,
			NULL, NULL, NULL);

		pBI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pBI->bmiHeader.biWidth = pPtD->ulWidth;
		pBI->bmiHeader.biHeight = 0 - pPtD->ulHeight;
		pBI->bmiHeader.biPlanes = 1;
		pBI->bmiHeader.biBitCount = 24;
		pBI->bmiHeader.biCompression = BI_RGB;

		// 读取内容
		// 这里注意，由于PNG的通道可能为4，所以需要处理背景色的问题
		ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
        pPtD->uChannels = png_get_channels(png_ptr, info_ptr);
		RGBToDIB(NULL, pPtD->uChannels, pBI, NULL);

		ClearPngToDIB(pPtD);
		pPtD->ulDIBSize = ulRowBytes * pPtD->ulHeight * sizeof(png_byte);
		if ((pPtD->pDIB = (PBYTE)malloc(pPtD->ulDIBSize)) == NULL)
        {
            png_error(png_ptr, "Visual PNG: out of memory");
        }

		if ((ppbRowPointers = (png_bytepp)malloc(pPtD->ulHeight
			* sizeof(png_bytep))) == NULL)
        {
            png_error(png_ptr, "Visual PNG: out of memory");
        }
		
        for (i = 0; i < pPtD->ulHeight; i++)
            ppbRowPointers[i] = pPtD->pDIB+ i * ulRowBytes;

		png_read_image(png_ptr, ppbRowPointers);
        png_read_end(png_ptr, NULL);

        free(ppbRowPointers);
        ppbRowPointers = NULL;

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	}
	catch (...)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		ClearPngToDIB(pPtD);
		if(ppbRowPointers)
		{
			free(ppbRowPointers);
		}
		return FALSE;
	}
	
	return TRUE;
}

static BOOL FUNCCALL ReadPngSize(PBYTE pPng, size_t ulPngSize, LPSIZE pImageSize)
{
	PNGTODIB ptd = {0};
	
	ptd.pPng = pPng;
	ptd.ulPngSize = ulPngSize;
	if (!ReadPng(&ptd, NULL, TRUE))
	{
		return FALSE;
	}

	pImageSize->cx = ptd.ulWidth;
	pImageSize->cy = ptd.ulHeight;
	return TRUE;
}

static BOOL FUNCCALL ShowPngImage(HDC hDC, CMemBmp *pBmp, PBYTE pPng, size_t ulPngSize)
{
	PNGTODIB ptd = {0};
	BITMAPINFO BI = {0};
	PBYTE pDIB = NULL;

	ptd.pPng = pPng;
	ptd.ulPngSize = ulPngSize;
	if (!ReadPng(&ptd, &BI, FALSE))
	{
		return FALSE;
	}

	pDIB = (PBYTE)malloc(BI.bmiHeader.biSizeImage);
	if (!pDIB)
	{
		ClearPngToDIB(&ptd);
		return FALSE;
	}
	if (!RGBToDIB(ptd.pDIB, ptd.uChannels, &BI, pDIB))
	{
		free(pDIB);
		ClearPngToDIB(&ptd);
		return FALSE;
	}
	ClearPngToDIB(&ptd);

	pBmp->CreateBmp(hDC, ptd.ulWidth, ptd.ulHeight, TRUE, AfxGetQColor(QCOLOR_EDITFACE));
	
	BOOL bRet = SetDIBits(hDC, pBmp->GetBmp(), 0, ptd.ulHeight, pDIB, &BI, DIB_RGB_COLORS);
	free(pDIB);
	return bRet;
}

BOOL FUNCCALL ReadImageSize(int nImageType, PBYTE pImage, size_t ulImageSize, LPSIZE pImageSize)
{
	BOOL bRet = TRUE;
	switch(nImageType)
	{
	case ITEMIMAGETYPE_DIB:
		bRet = ReadDIBSize(pImage, ulImageSize, pImageSize);
		break;
		
	case ITEMIMAGETYPE_JPEG:
		bRet = ReadJpegSize(pImage, ulImageSize, pImageSize);
		break;
		
	case ITEMIMAGETYPE_PNG:
		bRet = ReadPngSize(pImage, ulImageSize, pImageSize);
		break;
	}

	return bRet;
}

BOOL FUNCCALL ShowImage(HDC hDC, CMemBmp *pBmp, int nImageType, PBYTE pImageData, size_t ulImageSize)
{
	BOOL bRet = TRUE;
	switch(nImageType)
	{
	case ITEMIMAGETYPE_DIB:
		bRet = ShowDIBImage(hDC, pBmp, pImageData, ulImageSize);
		break;
		
	case ITEMIMAGETYPE_JPEG:
		bRet = ShowJpegImage(hDC, pBmp, pImageData, ulImageSize);
		break;
		
	case ITEMIMAGETYPE_PNG:
		bRet = ShowPngImage(hDC, pBmp, pImageData, ulImageSize);
		break;

	default:
		bRet = FALSE;
	}

	return bRet;
}

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
							  )
{
	HBITMAP hOldImageBMP, hImageBMP = CreateCompatibleBitmap(hdcDest, nWidthDest, nHeightDest); // 创建兼容位图
	HBITMAP hOldMaskBMP, hMaskBMP = CreateBitmap(nWidthDest, nHeightDest, 1, 1, NULL);          // 创建单色掩码位图
	HDC     hImageDC = CreateCompatibleDC(hdcDest);
	HDC     hMaskDC = CreateCompatibleDC(hdcDest);
	hOldImageBMP = (HBITMAP)SelectObject(hImageDC, hImageBMP);
	hOldMaskBMP = (HBITMAP)SelectObject(hMaskDC, hMaskBMP);
	
	// 将源DC中的位图拷贝到临时DC中
	if (nWidthDest == nWidthSrc && nHeightDest == nHeightSrc)
		BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hdcSrc, nXOriginSrc, nYOriginSrc, SRCCOPY);
	else
		StretchBlt(hImageDC, 0, 0, nWidthDest, nHeightDest,
		hdcSrc, nXOriginSrc, nYOriginSrc, nWidthSrc, nHeightSrc, SRCCOPY);
	
	// 设置透明色
	// 多色向单色绘制时，把多色图的背景色转换为白色，其他的转换为黑色。
	// 设置crTransparent为背景色，转换时成单色图时，把蓝色都转为白色，其余为黑色。
	SetBkColor(hImageDC, crTransparent);
	
	// 生成透明区域为白色，其它区域为黑色的掩码位图
	BitBlt(hMaskDC, 0, 0, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCCOPY);
	
	// 生成透明区域为黑色，其它区域保持不变的位图
	// 此时，hMaskDC中，白色为底色，黑色为前景色。
	
	SetBkColor(hImageDC, RGB(0,0,0));   //彩色位图的背景色被指定为黑色；
	SetTextColor(hImageDC, RGB(255,255,255));   //彩色位图的前景色被指定为白色，
	//如果此彩色位图转换为单色，那么黑色变为单色位图的背景色，其他变为前景色。
	//而单色位图的背景色一定是白色的，前景是黑色，也就是多色的黑->白。其他变黑。
	BitBlt(hImageDC, 0, 0, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);//将单色图绘制到这个DC上，
	//背景色是黑色，前景色是白色，即单色位图向这个DC上绘制，前景色要变为白色，背景色变为黑色，
	//再求与，就得到背景色为黑色(0 & n = 0)，前景色不变(1 & n = n)的位图。
	
	// 透明部分保持屏幕不变，其它部分变成黑色
	SetBkColor(hdcDest,RGB(0xff,0xff,0xff));
	SetTextColor(hdcDest,RGB(0,0,0));
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hMaskDC, 0, 0, SRCAND);//同上，
	//单色图向这个DC绘制，前景色为黑色，背景色为白色，则产生前景色为黑色，背景色不变的位图。
	
	// "或"运算,生成最终效果。
	BitBlt(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, nHeightDest, hImageDC, 0, 0, SRCPAINT);
	
	SelectObject(hImageDC, hOldImageBMP);
	DeleteDC(hImageDC);
	SelectObject(hMaskDC, hOldMaskBMP);
	DeleteDC(hMaskDC);
	DeleteObject(hImageBMP);
	DeleteObject(hMaskBMP);
}
