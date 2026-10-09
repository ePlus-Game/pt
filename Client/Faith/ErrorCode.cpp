//-------------------------------------------------------------
//	Purpose   :	 
//	Filename  :	 ErrorCode.cpp
//	Author    :	 Lucifer~yu (Zhang jian yu)
//	CreateTime:	 05/30/2006
//-------------------------------------------------------------
#include "KWin32.h"
#include "ErrorCode.h"

KClientError*	KClientError::m_pSelf		= NULL;
KStrintList		KClientError::m_szErrorString;

KClientError::KClientError()	
{

}

KClientError::~KClientError()
{

}
KClientError* KClientError::Error_Init()
{
	if ( m_pSelf == NULL )
	{
		m_pSelf = new KClientError;
		if ( m_pSelf )
		{
			m_szErrorString.push_back( APPLICATION_ERROR_0 );
			m_szErrorString.push_back( APPLICATION_ERROR_1 );
			m_szErrorString.push_back( APPLICATION_ERROR_2 );
			m_szErrorString.push_back( APPLICATION_ERROR_3 );
			m_szErrorString.push_back( APPLICATION_ERROR_4 );
			m_szErrorString.push_back( APPLICATION_ERROR_5 );
		}
	}
	return m_pSelf;
}

void KClientError::Error_SetErrorCode( unsigned int uCode, TCHAR* szPamam )
{
	if ( m_pSelf )
	{
		if( uCode >= 0 && uCode < ERR_T_CODE_COUNT  )
		{
			TCHAR szErrorMsg[COMMON_CLIENT_MSG_LEN_80 + 1];
			if ( szPamam )
			{
				ksprintf( szErrorMsg, m_szErrorString[uCode].c_str(), szPamam );
			}
			if ( szPamam )
			{
				::MessageBox( NULL, szErrorMsg, ERROR_MSGBOX_TITLE, MB_OK | MB_ICONERROR );
			}
			else
			{
				::MessageBox( NULL, m_szErrorString[uCode].c_str(), ERROR_MSGBOX_TITLE, MB_OK | MB_ICONERROR );
			}
		}
		else
		{
			::MessageBox( NULL, APPLICATION_ERROR, ERROR_MSGBOX_TITLE, MB_OK | MB_ICONERROR );
		}
	}
}

void KClientError::Error_Release()
{
	if ( m_pSelf )
	{
		delete m_pSelf;
	}
}
