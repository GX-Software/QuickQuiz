#ifndef _XMLTAGS_H_
#define _XMLTAGS_H_

/*
存储格式详见帮助文档
*/

extern LPCSTR xml_lpszDeclar_Encoding;
extern LPCSTR xml_lpszDeclar_Encoding95;

extern LPCSTR xml_lpszNode_Library;
extern LPCSTR xml_lpszNode_Version;
extern LPCSTR xml_lpszNode_Key;
extern LPCSTR xml_lpszNode_Title;
extern LPCSTR xml_lpszNode_Pswd;
extern LPCSTR xml_lpszNode_Custom;
extern LPCSTR xml_lpszNode_Type;

extern LPCSTR xml_lpszNode_Cover;

extern LPCSTR xml_lpszNode_Question;
extern LPCSTR xml_lpszNode_Description;
extern LPCSTR xml_lpszNode_Resolve;
extern LPCSTR xml_lpszNode_Image;
extern LPCSTR xml_lpszNode_Option;
extern LPCSTR xml_lpszNode_Answer;
extern LPCSTR xml_lpszNode_Blank;
extern LPCSTR xml_lpszNode_Group;

extern LPCSTR xml_lpszAttribute_Type;
extern LPCSTR xml_lpszAttribute_Index;
extern LPCSTR xml_lpszAttribute_QNum;
extern LPCSTR xml_lpszAttribute_Custom;
extern LPCSTR xml_lpszAttribute_Answer;
extern LPCSTR xml_lpszAttribute_Begin;
extern LPCSTR xml_lpszAttribute_End;
extern LPCSTR xml_lpszAttribute_Title;
extern LPCSTR xml_lpszAttribute_Loc;

extern LPCSTR xml_lpszAttribute_Width;
extern LPCSTR xml_lpszAttribute_Height;
extern LPCSTR xml_lpszAttribute_ColorBk;
extern LPCSTR xml_lpszAttribute_ColorText;
extern LPCSTR xml_lpszAttribute_Quality;

extern LPCSTR xml_lpszText_ImageHead;
extern LPCSTR xml_lpszText_ImageBmp;
extern LPCSTR xml_lpszText_ImageJpeg;
extern LPCSTR xml_lpszText_ImageJpg;
extern LPCSTR xml_lpszText_ImagePng;

extern LPCSTR xml_lpszType[TYPE_DEFAULTCOUNT + 1];

extern LPCSTR xml_lpszCommon_True;
extern LPCSTR xml_lpszCommon_False;

LPCSTR XMLGetTypeAttribute(int nType);
int XMLGetTypeIndex(LPCSTR lpszType);

#endif