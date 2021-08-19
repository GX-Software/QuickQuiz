#ifndef _QERROR_H_
#define _QERROR_H_

/* 当返回值为EBOOL时，意味着包含了错误信息 */
typedef int EBOOL;
/* 当函数被RETURN_E修饰时，意味着包含了错误信息 */
#define RETURN_E

/* 无错误 */
#define ERROR_NO_ERROR							0L

/* 申请内存失败 */
#define ERROR_ALLOC_FAIL						1L
/* 文件不存在 */
#define ERROR_FILE_NOT_EXIST					2L
/* 创建文件失败 */
#define ERROR_CREATE_FILE_FAIL					3L
/* 读取文件失败 */
#define ERROR_READ_FILE_FAIL					4L
/* 写入文件失败 */
#define ERROR_WRITE_FILE_FAIL					5L
/* 文字转码失败 */
#define ERROR_TEXT_TRANSCODING_FAIL				6L
/* 资源文件缺失 */
#define ERROR_RESOURCE_NOT_EXIST				7L
/* 无法获得显示设备 */
#define ERROR_CREATE_DC_FAIL					8L
/* 无法创建DDB */
#define ERROR_CREATE_DDB_FAIL					9L
/* 无法绘图 */
#define ERROR_DRAWING_FAIL						10L

/* 文件不完整 */
#define ERROR_FILE_INCOMPLETE					101L
/* 文件内容被破坏 */
#define ERROR_FILE_INCORRECT					102L
/* 不支持的版本号 */
#define ERROR_INVALID_VERSION					103L
/* 不支持的文字编码方式 */
#define ERROR_INVALID_TEXT_CHARSET				104L
/* 文件头缺失 */
#define ERROR_HEAD_NOT_FOUND					105L
/* 密码缺失 */
#define ERROR_PASSWORD_NOT_FOUND				106L
/* 解密失败 */
#define ERROR_DECRYPT_FAIL						107L
/* 加密失败 */
#define ERROR_ENCRYPT_FAIL						108L
/* 压缩信息缺失 */
#define ERROR_COMPRESS_INFO_LACKING				109L
/* 解压失败 */
#define ERROR_EXPANSION_FAIL					110L
/* 压缩失败 */
#define ERROR_COMPRESS_FAIL						111L
/* XML解析失败 */
#define ERROR_XML_PARSE_FAIL					112L
/* XML编码失败 */
#define ERROR_XML_ENCODE_FAIL					113L
/* JPEG封面图片创建失败 */
#define ERROR_COVER_CREATE_FAIL					114L
/* JPEG封面图片压缩失败 */
#define ERROR_JPEG_COMPRESS_FAIL				115L
/* JPEG图片解码失败 */
#define ERROR_JPEG_DECODE_FAIL					116L
/* BMP图片读取失败 */
#define ERROR_BMP_LOAD_FAIL						117L
/* PNG图片解码失败 */
#define ERROR_PNG_DECODE_FAIL					118L
/* 无法识别的图像base64编码 */
#define ERROR_IMAGE_BASE64_INCORRECT			119L
/* base64编码错误 */
#define ERROR_BASE64_ENCODE_FAIL				120L
/* base64解码错误 */
#define ERROR_BASE64_DECODE_FAIL				121L
/* 不支持的图像格式 */
#define ERROR_INVALID_IMAGE_FORMAT				122L
/* 自定义类型错误 */
#define ERROR_CUSTOM_TYPE_INCORRECT				123L

/* 默认的错误，为内部原因，用于调试使用 */
#define ERROR_INNER								65535L

/*
调试时，只显示最初发生错误的位置，多次调用不会多次显示
*/
#ifdef _DEBUG
#define Q_ERROR(e)		((ERROR_NO_ERROR == Q_ERRORCODE) ? \
						TRACE3("发生错误【%d】！文件：%s，行%d\n", e, __FILE__, __LINE__) : 1, \
						SetError(e, TRUE))
#define Q_WARNING(w)	((ERROR_NO_ERROR == Q_ERRORCODE) ? \
						TRACE3("发生警告【%d】！文件：%s，行%d\n", w, __FILE__, __LINE__) : 1, \
						SetError(w, FALSE))
#else
#define Q_ERROR(e) SetError(e, TRUE)
#define Q_WARNING(w) SetError(w, FALSE)
#endif

#define Q_ERROR_RET(e, ret)			(Q_ERROR(e), ret)
#define Q_WARNING_RET(e, ret)		(Q_WARNING(e), ret)
#define Q_ERROR_NOMSG				(ClearError(), FALSE)

#define Q_TRUE						(ClearError())
#define Q_TRUE_RET(ret)				(ClearError(), ret)
#define Q_FALSE						(ClearError(), FALSE)

#define Q_RET(ret)					(ret)

#define Q_ERRORCODE					(GetError())
#define Q_CLEARERROR				ClearError

#define Q_SHOWERRORMSG_CODE(hWnd, Msg) \
	if(Q_ERRORCODE) { \
		CTMessageBox qerror_box_store; \
		qerror_box_store.SuperMessageBox(hWnd, Msg, IDS_MSGCAPTION_ERROR, \
		MB_OK | MB_ICONERROR, 0, 0, Q_ERRORCODE); \
	}


/* 执行成功后可以直接返回ClearError()，即TRUE */
BOOL FUNCCALL ClearError();

/*
执行失败后可以直接返回SetError()
当错误类型是致命时，将返回FALSE，否则，仍然返回TRUE
当多次返回错误时，仅记录最初的错误
*/
BOOL FUNCCALL SetError(long lCode, BOOL bFatal);

/* 获取错误信息 */
long FUNCCALL GetError();

/* 是否为致命错误 */
BOOL FUNCCALL IsFatal();

#endif