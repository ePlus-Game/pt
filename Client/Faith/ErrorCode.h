//-------------------------------------------------------------
//	Purpose   :	 
//	Filename  :	 ErrorCode.h
//	Author    :	 Lucifer~yu (Zhang jian yu)
//	CreateTime:	 05/30/2006
//-------------------------------------------------------------

#ifndef ERRORCODE_H
#define ERRORCODE_H
#include <string>
#include <vector>
#include "CoreUseNameDef.h"

#if defined _UNICODE || defined UNICODE
	#define kstring	std::wstring
#else
	#define kstring	std::string
#endif

enum ERROR_CODE
{
	ERR_T_FILE_NO_FOUND ,
	ERR_T_LOAD_MODULE_FAILED,
	ERR_T_MODULE_UNCORRECT,
	ERR_T_MODULE_INIT_FAILED,
	ERR_T_REPRESENT2_INIT_FAILED,
	ERR_T_REPRESENT3_INIT_FAILED,
	ERR_T_CODE_COUNT,
};

typedef std::vector<kstring> KStrintList;

class KClientError
{
	
private:
	KClientError();
	~KClientError();
public:
	static KClientError*	Error_Init		  (										);
	static void				Error_Release	  (										);
	static void				Error_SetErrorCode( unsigned int uCode, TCHAR* szPamam	);
private:
	//错误代码提示部分定义的全局变量
	static KClientError*	m_pSelf;
	static KStrintList		m_szErrorString;
};

#endif 
