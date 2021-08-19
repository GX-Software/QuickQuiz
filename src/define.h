#ifndef _DEFINE_H_
#define _DEFINE_H_

#define _local
#define _countof(array) (sizeof(array)/sizeof(array[0]))

#ifdef _UNICODE
#define _stdstring			std::wstring
#else
#define _stdstring			std::string
#endif

// 操作系统版本号
enum OSVersion {
	OS_WIN95 = 0,
	OS_WIN98,
	OS_WINNT4,
	OS_WINNTNEW
};

// 全局信息

// 版本更新信息：
// 2版新增自定义题目类型功能，题目类型由2字节修改为4字节
// 3版新增题目解析内容，保存在通用CItem对象中
// 4版新增封面功能，且题库格式发生重大改动
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

// 视图消息
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

// 题目类型
#define TYPE_LASTDEFAULT		TYPE_GROUP
#define TYPE_MAXCUSTOMCOUNT		16
#define TYPE_CUSTOMRIGHT		100
#define TYPE_DEFAULTCOUNT		6
#define TYPE_SINGLEDEFAULTCOUNT	5
#define TYPE_DEFAULT			0

// 最大的题号
#define LIST_MAXQNUMBER			32

// 题目添加
#define SAVEABLE_OK				0	// 正常
#define SAVEABLE_CONTINUE		1	// 丢失本题，继续
#define SAVEABLE_DENY			2	// 留在本题，继续

#define TIME_QUIZMAXHOUR		6	// 最长6小时
#define TIME_QUIZMAXMINUTE		360

#define MARKTYPE_AVERAGE		0
#define MARKTYPE_USERSET		1

// 练习参数
typedef struct _tagTypePara
{
	DWORD dwTypeMask;
	BOOL bRandom;
}TYPEPARA, *PTYPEPARA;

// 通用三参数结构
typedef struct _tagTParas
{
	DWORD dwPara1;
	DWORD dwPara2;
	DWORD dwPara3;
}TPARAS, *PTPARAS;

#endif