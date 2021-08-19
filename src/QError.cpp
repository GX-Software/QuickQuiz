
#include "StdAfx.h"
#include "QError.h"

typedef struct _tagError
{
	long lErrorCode;
	BOOL bFatal;
}QERROR;

static QERROR error = {ERROR_NO_ERROR, FALSE};

BOOL FUNCCALL ClearError()
{
	error.lErrorCode = ERROR_NO_ERROR;
	error.bFatal = FALSE;
	return TRUE;
}

BOOL FUNCCALL SetError(long lCode, BOOL bFatal)
{
	if (error.lErrorCode != ERROR_NO_ERROR)
	{
		if (!bFatal || error.bFatal)
		{
			return FALSE;
		}
	}

	error.lErrorCode = lCode;
	error.bFatal = bFatal;
	return FALSE;
}

long FUNCCALL GetError()
{
	return error.lErrorCode;
}

BOOL FUNCCALL IsFatal()
{
	return error.bFatal;
}