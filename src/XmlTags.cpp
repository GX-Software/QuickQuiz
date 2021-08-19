#include "StdAfx.h"

LPCSTR xml_lpszDeclar_Encoding			= "UTF-8";
LPCSTR xml_lpszDeclar_Encoding95		= "GBK";

LPCSTR xml_lpszNode_Library				= "library";
LPCSTR xml_lpszNode_Version				= "version";
LPCSTR xml_lpszNode_Key					= "key";
LPCSTR xml_lpszNode_Title				= "title";
LPCSTR xml_lpszNode_Pswd				= "pswd";
LPCSTR xml_lpszNode_Custom				= "cust";
LPCSTR xml_lpszNode_Type				= "type";

LPCSTR xml_lpszNode_Cover				= "cover";

LPCSTR xml_lpszNode_Question			= "qstn";
LPCSTR xml_lpszNode_Description			= "desc";
LPCSTR xml_lpszNode_Resolve				= "res";
LPCSTR xml_lpszNode_Image				= "img";
LPCSTR xml_lpszNode_Option				= "opt";
LPCSTR xml_lpszNode_Answer				= "ans";
LPCSTR xml_lpszNode_Blank				= "blk";
LPCSTR xml_lpszNode_Group				= "grp";

LPCSTR xml_lpszAttribute_Type			= "type";
LPCSTR xml_lpszAttribute_Index			= "index";
LPCSTR xml_lpszAttribute_QNum			= "qnum";
LPCSTR xml_lpszAttribute_Custom			= "cust";
LPCSTR xml_lpszAttribute_Answer			= "ans";
LPCSTR xml_lpszAttribute_Begin			= "beg";
LPCSTR xml_lpszAttribute_End			= "end";
LPCSTR xml_lpszAttribute_Title			= "title";
LPCSTR xml_lpszAttribute_Loc			= "loc";

LPCSTR xml_lpszAttribute_Width			= "width";
LPCSTR xml_lpszAttribute_Height			= "height";
LPCSTR xml_lpszAttribute_ColorBk		= "clrbk";
LPCSTR xml_lpszAttribute_ColorText		= "clrtxt";
LPCSTR xml_lpszAttribute_Quality		= "quality";

LPCSTR xml_lpszText_ImageHead			= "data:image/";
LPCSTR xml_lpszText_ImageBmp			= "bmp;base64,";
LPCSTR xml_lpszText_ImageJpeg			= "jpeg;base64,";
LPCSTR xml_lpszText_ImageJpg			= "jpg;base64,";
LPCSTR xml_lpszText_ImagePng			= "png;base64,";

LPCSTR xml_lpszType[TYPE_DEFAULTCOUNT + 1] =
{
	"unknown",
	"s-choise",
	"m-choise",
	"judge",
	"blank",
	"text",
	"group"
};

LPCSTR xml_lpszCommon_True				= "t";
LPCSTR xml_lpszCommon_False				= "f";

LPCSTR XMLGetTypeAttribute(int nType)
{
	if (nType > TYPE_DEFAULTCOUNT)
	{
		return NULL;
	}

	return xml_lpszType[nType];
}

int XMLGetTypeIndex(LPCSTR lpszType)
{
	int i;
	for (i = TYPE_SCHOISE; i <= TYPE_DEFAULTCOUNT; i++)
	{
		if (!strcmp(lpszType, xml_lpszType[i]))
		{
			return i;
		}
	}

	return TYPE_DEFAULT;
}