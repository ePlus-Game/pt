//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/10/2006 9:34
//      File_base        : KMessageCentre
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KMESSAGECENTRE_H
#define KMESSAGECENTRE_H

#include "CoreUseNameDef.h"
#include "KIniFile.h"

#if _MSC_VER > 1000
#pragma once
#endif 

class KMessageCentre  
{
public:
	KMessageCentre();
	virtual ~KMessageCentre();
public:
	static char* GetMessage( int nMsgType, int nMsgCode );
	static char* GetMessageSafe( int nMsgType, int nMsgCode );
private:
	static char m_szMsg[COMMON_CLIENT_MSG_LEN_1024 * 10];
	static 	KIniFile ini;
	static BOOL m_bLoad;
};



#endif 