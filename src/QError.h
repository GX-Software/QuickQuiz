#ifndef _QERROR_H_
#define _QERROR_H_

/* ������ֵΪEBOOLʱ����ζ�Ű����˴�����Ϣ */
typedef int EBOOL;
/* ��������RETURN_E����ʱ����ζ�Ű����˴�����Ϣ */
#define RETURN_E

/* �޴��� */
#define ERROR_NO_ERROR							0L

/* �����ڴ�ʧ�� */
#define ERROR_ALLOC_FAIL						1L
/* �ļ������� */
#define ERROR_FILE_NOT_EXIST					2L
/* �����ļ�ʧ�� */
#define ERROR_CREATE_FILE_FAIL					3L
/* ��ȡ�ļ�ʧ�� */
#define ERROR_READ_FILE_FAIL					4L
/* д���ļ�ʧ�� */
#define ERROR_WRITE_FILE_FAIL					5L
/* ����ת��ʧ�� */
#define ERROR_TEXT_TRANSCODING_FAIL				6L
/* ��Դ�ļ�ȱʧ */
#define ERROR_RESOURCE_NOT_EXIST				7L
/* �޷������ʾ�豸 */
#define ERROR_CREATE_DC_FAIL					8L
/* �޷�����DDB */
#define ERROR_CREATE_DDB_FAIL					9L
/* �޷���ͼ */
#define ERROR_DRAWING_FAIL						10L

/* �ļ������� */
#define ERROR_FILE_INCOMPLETE					101L
/* �ļ����ݱ��ƻ� */
#define ERROR_FILE_INCORRECT					102L
/* ��֧�ֵİ汾�� */
#define ERROR_INVALID_VERSION					103L
/* ��֧�ֵ����ֱ��뷽ʽ */
#define ERROR_INVALID_TEXT_CHARSET				104L
/* �ļ�ͷȱʧ */
#define ERROR_HEAD_NOT_FOUND					105L
/* ����ȱʧ */
#define ERROR_PASSWORD_NOT_FOUND				106L
/* ����ʧ�� */
#define ERROR_DECRYPT_FAIL						107L
/* ����ʧ�� */
#define ERROR_ENCRYPT_FAIL						108L
/* ѹ����Ϣȱʧ */
#define ERROR_COMPRESS_INFO_LACKING				109L
/* ��ѹʧ�� */
#define ERROR_EXPANSION_FAIL					110L
/* ѹ��ʧ�� */
#define ERROR_COMPRESS_FAIL						111L
/* XML����ʧ�� */
#define ERROR_XML_PARSE_FAIL					112L
/* XML����ʧ�� */
#define ERROR_XML_ENCODE_FAIL					113L
/* JPEG����ͼƬ����ʧ�� */
#define ERROR_COVER_CREATE_FAIL					114L
/* JPEG����ͼƬѹ��ʧ�� */
#define ERROR_JPEG_COMPRESS_FAIL				115L
/* JPEGͼƬ����ʧ�� */
#define ERROR_JPEG_DECODE_FAIL					116L
/* BMPͼƬ��ȡʧ�� */
#define ERROR_BMP_LOAD_FAIL						117L
/* PNGͼƬ����ʧ�� */
#define ERROR_PNG_DECODE_FAIL					118L
/* �޷�ʶ���ͼ��base64���� */
#define ERROR_IMAGE_BASE64_INCORRECT			119L
/* base64������� */
#define ERROR_BASE64_ENCODE_FAIL				120L
/* base64������� */
#define ERROR_BASE64_DECODE_FAIL				121L
/* ��֧�ֵ�ͼ���ʽ */
#define ERROR_INVALID_IMAGE_FORMAT				122L
/* �Զ������ʹ��� */
#define ERROR_CUSTOM_TYPE_INCORRECT				123L

/* Ĭ�ϵĴ���Ϊ�ڲ�ԭ�����ڵ���ʹ�� */
#define ERROR_INNER								65535L

/*
����ʱ��ֻ��ʾ������������λ�ã���ε��ò�������ʾ
*/
#ifdef _DEBUG
#define Q_ERROR(e)		((ERROR_NO_ERROR == Q_ERRORCODE) ? \
						TRACE3("��������%d�����ļ���%s����%d\n", e, __FILE__, __LINE__) : 1, \
						SetError(e, TRUE))
#define Q_WARNING(w)	((ERROR_NO_ERROR == Q_ERRORCODE) ? \
						TRACE3("�������桾%d�����ļ���%s����%d\n", w, __FILE__, __LINE__) : 1, \
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


/* ִ�гɹ������ֱ�ӷ���ClearError()����TRUE */
BOOL FUNCCALL ClearError();

/*
ִ��ʧ�ܺ����ֱ�ӷ���SetError()
����������������ʱ��������FALSE��������Ȼ����TRUE
����η��ش���ʱ������¼����Ĵ���
*/
BOOL FUNCCALL SetError(long lCode, BOOL bFatal);

/* ��ȡ������Ϣ */
long FUNCCALL GetError();

/* �Ƿ�Ϊ�������� */
BOOL FUNCCALL IsFatal();

#endif