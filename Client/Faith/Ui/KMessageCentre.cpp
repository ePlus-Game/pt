//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/10/2006 9:34
//      File_base        : KMessageCentre
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KMessageCentre.h"
#include "Text.h"
#include <string>

int __replaceStrByStr( char *pObj, const char *pKey, const char *pStr )
{
	std::string strObj;
	std::string strKey;
	std::string strStr;
	if ( pObj && pKey && pStr )
	{
		int nPos = 0;
		strObj = pObj;
		strKey = pKey;
		strStr = pStr;
		while ( nPos != std::string::npos )
		{
			nPos = strObj.find( pKey );
			if ( nPos != std::string::npos )
			{
				strObj = strObj.replace( nPos, strKey.size(), strStr );
			}
		}
		strcpy( pObj, strObj.c_str() );
	}
	return strObj.size();
}

KIniFile KMessageCentre::ini;
BOOL KMessageCentre::m_bLoad = false;

char KMessageCentre::m_szMsg[COMMON_CLIENT_MSG_LEN_1024 * 10];

KMessageCentre::KMessageCentre()
{
	
}

KMessageCentre::~KMessageCentre()
{

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
char* KMessageCentre::GetMessage( int nMsgType, int nMsgCode )
{
	if ( m_bLoad == FALSE )
	{
		m_bLoad = ini.Load( STRING_RES_SCEME );	
	}	
	if ( m_bLoad == TRUE )
	{
		char szSection[COMMON_CLIENT_MSG_LEN_8];
		char szKey[COMMON_CLIENT_MSG_LEN_8];
		itoa( nMsgType, szSection, 10 );
		itoa( nMsgCode, szKey, 10 );
		ini.GetString( szSection, szKey, "", m_szMsg, sizeof(m_szMsg) );
		__replaceStrByStr( m_szMsg, "\\n", "\n" );
		return m_szMsg;
	}
	return NULL;
}

char* KMessageCentre::GetMessageSafe( int nMsgType, int nMsgCode )
{
	char* szMsg = GetMessage( nMsgType, nMsgCode );
	if ( NULL == szMsg )
	{
		ZeroMemory( m_szMsg, sizeof( m_szMsg ) );
	}

	return m_szMsg;
}