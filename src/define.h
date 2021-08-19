#ifndef _DEFINE_H_
#define _DEFINE_H_

#define _local
#define _countof(array) (sizeof(array)/sizeof(array[0]))

#ifdef _UNICODE
#define _stdstring			std::wstring
#else
#define _stdstring			std::string
#endif

// ����ϵͳ�汾��
enum OSVersion {
	OS_WIN95 = 0,
	OS_WIN98,
	OS_WINNT4,
	OS_WINNTNEW
};

// ȫ����Ϣ

// �汾������Ϣ��
// 2�������Զ�����Ŀ���͹��ܣ���Ŀ������2�ֽ��޸�Ϊ4�ֽ�
// 3��������Ŀ�������ݣ�������ͨ��CItem������
// 4���������湦�ܣ�������ʽ�����ش�Ķ�
#define FILEVERSION_MAJOR		2
#define FILEVERSION_MINOR		0
#define FILEVERSION_PATCH		0
#define FILEVERSION_ADDITION	0
#define FILEVERSION				4
#define FILEVERSION_1X			3

#define MAX_QUESTION_NUM		5000
#define MAX_OPTION_LEN			512

#define MAX_PAPERNAME_LEN		64
#define MAX_CUSTOMTYPENAME_LEN	16
#define TYPE_BUFFSIZE			((MAX_CUSTOMTYPENAME_LEN + 1) << 1)

#define MAX_PASSWORD_LEN		16

// ��ͼ��Ϣ
#define HINT_ADD_PAPER			1000
#define HINT_CLOSE_PAPER		1001
#define HINT_SELECT_PAPER		1002
#define HINT_SAVE_PAPER			1003
#define HINT_RESET_PAPER		1004
#define HINT_EDIT_ITEM			1005
#define HINT_READONLY_PAPER		1006
#define HINT_COVER_PAPER		1007

#define HINT_SELECT_TYPE		1010

#define HINT_ADD_QUESTION		1020
#define HINT_STORE_QUESTION		1021

#define HINT_UNDO				1030
#define HINT_REDO				1031

#define HINT_ADD_CUSTOMTYPE		1040
#define HINT_DEL_CUSTOMTYPE_L	1041
#define HINT_DEL_CUSTOMTYPE_R	1042
#define HINT_EDIT_CUSTOMTYPE	1043

// ��Ŀ����
#define TYPE_LASTDEFAULT		TYPE_GROUP
#define TYPE_MAXCUSTOMCOUNT		16
#define TYPE_CUSTOMRIGHT		100
#define TYPE_DEFAULTCOUNT		6
#define TYPE_SINGLEDEFAULTCOUNT	5
#define TYPE_DEFAULT			0

// �������
#define LIST_MAXQNUMBER			32

// ��Ŀ���
#define SAVEABLE_OK				0	// ����
#define SAVEABLE_CONTINUE		1	// ��ʧ���⣬����
#define SAVEABLE_DENY			2	// ���ڱ��⣬����

#define TIME_QUIZMAXHOUR		6	// �6Сʱ
#define TIME_QUIZMAXMINUTE		360

#define MARKTYPE_AVERAGE		0
#define MARKTYPE_USERSET		1

// ��ϰ����
typedef struct _tagTypePara
{
	DWORD dwTypeMask;
	BOOL bRandom;
}TYPEPARA, *PTYPEPARA;

// ͨ���������ṹ
typedef struct _tagTParas
{
	DWORD dwPara1;
	DWORD dwPara2;
	DWORD dwPara3;
}TPARAS, *PTPARAS;

#endif