//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.cpp
// Date:	2002.02.20
// Code:	Huyi(Spe)
// Desc:	Tab File Operation Class

// CodeReset: Daniel liu
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KDebug.h"
#include "KStrBase.h"
#include "KFile.h"
#include "KFilePath.h"
#include "KPakFile.h"
#include "KTabFile.h"

//[[2004.3.18 Add by Daniel liu
/*
#define INIT_PTR(x)				do{ (x) = NULL; }while(0);
#define SAFE_DEL(x)				try{ if( (x) != NULL ) { delete (x); (x) = NULL; } } catch(...) { ; }
#define SAFE_DELETE_ARRAY(x)	try{ if( (x) != NULL ) { delete[] (x); (x) = NULL; } } catch(...) { ; }

CContextNode::CContextNode()
{
	m_nValue = 0;
	m_fValue = 0.0;

	m_nStrLength = 0;
	INIT_PTR( m_pString );

}

CContextNode::~CContextNode()
{
	m_nValue = 0;
	m_fValue = 0.0;

	m_nStrLength = 0;

	SAFE_DELETE_ARRAY( m_pString );
}

bool CContextNode::SetContent( const char *pStr )
{
	if ( NULL == pStr || pStr[0] == '\0' )
	{
		m_nStrLength = 0;

		SAFE_DELETE_ARRAY( m_pString );

		return false;
	}

	m_nStrLength = strlen( pStr );
	
	m_pString = new char[ m_nStrLength + 1 ];
	strcpy( m_pString, pStr );
	m_pString[ m_nStrLength ] = '\0';

	return ConvertType( m_pString, m_nValue, m_fValue );
}

bool CContextNode::SetContent( const char *pStr, int nBeingPos, int nLength )
{
	if ( NULL == pStr || pStr[0] == '\0' || nBeingPos < 0 || nLength <= 0 )
	{
		m_nStrLength = 0;

		SAFE_DELETE_ARRAY( m_pString );

		return false;
	}

	m_nStrLength = nLength;
	
	m_pString = new char[ m_nStrLength + 1 ];
	memcpy( m_pString, pStr + nBeingPos, nLength );
	m_pString[ m_nStrLength ] = '\0';

	return ConvertType( m_pString, m_nValue, m_fValue );
}

bool CContextNode::ConvertType( const char *pStr, int &nValue, float &fValue ) const
{
	if ( NULL == pStr || pStr[0] == '\0' )
	{
		fValue = 0.0;
		nValue = 0;

		return false;
	}

	nValue = atoi( pStr );
	fValue = atof( pStr );

	return true;
}


KTabFile::KTabFile()
{
	m_nRow = 0;
	m_nCol = 0;

	INIT_PTR( m_pdwAddrTable );
	m_scFileName[0] = '\0';
}

KTabFile::~KTabFile()
{
	Clear();
}

BOOL KTabFile::AnalyseContent( const char *pBuf, int nSize )
{
	if ( NULL == pBuf || nSize <= 0 )
	{
		return FALSE;
	}

	//Read first row to Get number of column
	int nOffset = 0;
	const char *pSearch = pBuf;

	m_nCol = m_nRow = 1;

	while ( 0x0d != *pSearch && 0x0a != *pSearch )
	{
		if ( 0x09 == *pSearch )
		{
			m_nCol ++;
		}

		pSearch ++;
		nOffset ++;
	}

	//Get begin address of second row
	if ( ( 0x0d == *pSearch && 0x0a == *( pSearch + 1 ) ) ||	// for window
		( 0x0a == *pSearch && 0x0d == *( pSearch + 1 ) ) )		// for unix
	{
		pSearch += 2;
		nOffset += 2;
	}
	else
	{
		pSearch ++;
		nOffset ++;
	}

	//Get number of column
	while ( nOffset < nSize )
	{
		while ( 0x0d != *pSearch && 0x0a != *pSearch )
		{
			pSearch ++;
			nOffset ++;

			if ( nOffset >= nSize )
			{
				//TO CHECK : jump twice loop ? 
				break;
			}
		}

		m_nRow ++;
		
		if ( ( 0x0d == *pSearch && 0x0a == *( pSearch + 1 ) ) ||	// for window
			( 0x0a == *pSearch && 0x0d == *( pSearch + 1 ) ) )		// for unix
		{
			pSearch += 2;
			nOffset += 2;
		}
		else
		{
			pSearch ++;
			nOffset ++;
		}
	}

	nOffset = 0;
	pSearch = pBuf;
	
	m_pdwAddrTable = new DWORD[ m_nRow * m_nCol ];
	memset( m_pdwAddrTable, 0, m_nRow * m_nCol * sizeof( DWORD ) );
	
	int nBeginPos = 0, nLength = 0;

	for ( int i = 0; i < m_nRow; i ++ )
	{
		for ( int j = 0; j< m_nCol; j ++ )
		{
			nBeginPos = nOffset;
			nLength = 0;

			while ( 0x09 != *pSearch && 0x0d != *pSearch && 0x0a != *pSearch && nOffset < nSize )
			{
				pSearch ++;
				nOffset ++;
				nLength ++;

			} // while ( ...

			pSearch ++;	//Jump flag ( etc. 0x09 & 0x0a & 0x0d )
			nOffset ++;
			
			CContextNode *pNode = new CContextNode;
			pNode->SetContent( pBuf, nBeginPos, nLength );
			
			m_theItemList.AddTail( pNode );

			m_pdwAddrTable[i*m_nCol+j] = ( DWORD )( ( DWORD * )pNode );

			if ( 0x0a == *( pSearch - 1 ) || 0x0d == *( pSearch - 1 ) )
			{
//				for ( int k = j+1; k < m_nCol; k ++ )
//				{
//					CContextNode *pNode = new CContextNode;
//					pNode->SetContent( "" );
//					
//					m_theItemList.AddTail( pNode );
//					
//					m_pdwAddrTable[i*m_nCol+k] = ( DWORD )( ( DWORD * )pNode );
//				}

				break;
			}
		}
		
		if ( *( pSearch - 1 ) == 0x0d && *pSearch == 0x0a )
		{
			pSearch ++;
			nOffset ++;
		}
	}

	return TRUE;
}

BOOL KTabFile::Load( LPSTR FileName )
{
	KPakFile	theFile;
	DWORD		dwSize = 0;
	char *		pBuffer = NULL;

	if ( '\0' == FileName[0] )
	{
		return FALSE;
	}

	if ( !theFile.Open( FileName ) )
	{
		g_DebugLog( "Can't open tab file : %s", FileName );

		return FALSE;
	}

	strcpy( m_scFileName, FileName );

	if ( 0 == ( dwSize = theFile.Size() ) )
	{
		return FALSE;
	}

	pBuffer = new char[dwSize];

	theFile.Read( pBuffer, dwSize );

	BOOL bResult = AnalyseContent( pBuffer, dwSize );

	SAFE_DELETE_ARRAY( pBuffer );

	return bResult;
}

BOOL KTabFile::Save( LPSTR FileName )
{
	//I'm not to support this function, please call me if you want to use it
	//Danile liu( liupeng@kingsoft.com )

	__asm int 3;

	return FALSE;
}

int KTabFile::Str2Col( LPSTR szColumn )
{
	int	nStrLen = strlen( szColumn );
	char	szTemp[4];

	strcpy( szTemp, szColumn );
	g_StrUpper( szTemp );
	
	if ( nStrLen == 1 )
	{
		return ( szTemp[0] - 'A' );
	}
	return ( ( szTemp[0] - 'A' + 1 ) * 26 + szTemp[1] - 'A' ) + 1;
}

int KTabFile::FindRow( LPSTR szRow )
{
	for ( int i = 0; i < m_nRow; i ++ )
	{
		CContextNode *pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ i * m_nCol + 0 ] ) );

		if ( NULL == pNode )
		{
			return -1;
		}

		const char *pStr = pNode->GetStr();
		if ( pStr )
		{
			if ( 0 == memcmp( pStr, szRow, strlen( szRow ) ) )
			{
				return i + 1;
			}
		}
	}

	return -1;
}

int KTabFile::FindColumn( LPSTR szColumn )
{
	for ( int i = 0; i < m_nCol; i ++ )
	{
		CContextNode *pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ 0 * m_nRow + i ] ) );

		if ( NULL == pNode )
		{
			return -1;
		}

		const char *pStr = pNode->GetStr();
		if ( pStr )
		{
			if ( 0 == memcmp( pStr, szColumn, strlen( szColumn ) ) )
			{
				return i + 1;
			}
		}
	}	

	return -1;
}

BOOL KTabFile::GetString( int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab )
{
	int nColumn = 0;
	CContextNode *pNode = NULL;
	const char *pStr = NULL;

	if ( bColumnLab )
	{
		nColumn = FindColumn( szColumn );
	}
	else
	{
		nColumn = Str2Col( szColumn );
	}

	if ( nRow <= 0 || nColumn <= 0 || nRow > m_nRow || nColumn > m_nCol )
	{
		goto _error_process;
	}

	pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		goto _error_process;
	}
	
	pStr = pNode->GetStr();
	if ( pStr )
	{
		g_StrCpyLen( lpRString, pStr, dwSize );

		return TRUE;
	}
	
_error_process:

	g_StrCpyLen( lpRString, lpDefault, dwSize );

	return FALSE;
}

BOOL KTabFile::GetString( int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize )
{
	CContextNode *pNode = NULL;
	const char *pStr = NULL;

	if ( nRow <= 0 || nColumn <= 0 || 
		// Add by cooler 2004-03-18
		// Begin -->
		nRow > m_nRow || nColumn > m_nCol)
	{	// --<
		goto _error_process;
	}

	pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		goto _error_process;
	}
	
	pStr = pNode->GetStr();
	if ( pStr )
	{
		g_StrCpyLen( lpRString, pStr, dwSize );

		return TRUE;
	}

_error_process:

	g_StrCpyLen( lpRString, lpDefault, dwSize );

	return TRUE;
}

BOOL KTabFile::GetString( LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize )
{
	int nRow = 0, nColumn = 0;
	CContextNode *pNode = NULL;
	const char *pStr = NULL;

	nRow = FindRow( szRow );
	nColumn = FindColumn( szColumn );

	// Add by cooler 2004-03-18
	// Begin -->
	if ( nRow <= 0 || nColumn <= 0 || nRow > m_nRow || nColumn > m_nCol )
	{
		goto _error_process;
	}
	// End <--
	
	pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		goto _error_process;
	}
	
	pStr = pNode->GetStr();
	if ( pStr )
	{
		g_StrCpyLen( lpRString, pStr, dwSize );

		return TRUE;
	}

_error_process:

	g_StrCpyLen( lpRString, lpDefault, dwSize );

	return FALSE;

}

BOOL KTabFile::GetInteger( int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab )
{
	int nColumn = 0;
	CContextNode *pNode = NULL;

	if ( bColumnLab )
	{
		nColumn = FindColumn( szColumn );
	}
	else
	{
		nColumn = Str2Col( szColumn );
	}

	if ( nRow <= 0 || nColumn <= 0 || nRow > m_nRow || nColumn > m_nCol )
	{
		goto _error_process;
	}

	pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( pNode )
	{
		*pnValue = pNode->GetINT();
		
		return TRUE;
	}

_error_process:

	*pnValue = nDefault;

	return FALSE;
}

BOOL KTabFile::GetInteger( int nRow, int nColumn, int nDefault, int *pnValue )
{
	CContextNode *pNode = NULL;

	if ( nRow <= 0 || nColumn <= 0 ||
		// Add by cooler 2004-03-18
		// Begin -->
		nRow > m_nRow || nColumn > m_nCol)
	{
		// End <--	
		goto _error_process;
	}

	pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( pNode )
	{
		*pnValue = pNode->GetINT();
		
		return TRUE;
	}

_error_process:

	*pnValue = nDefault;

	return TRUE;
}

BOOL KTabFile::GetInteger( LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue )
{
	int nRow = 0, nColumn = 0;
	CContextNode *pNode = NULL;

	nRow = FindRow( szRow );
	nColumn = FindColumn( szColumn );

	// Add by cooler 2004-03-18
	// Begin -->
	if ( nRow <= 0 || nColumn <= 0 || nRow > m_nRow || nColumn > m_nCol )
	{
		goto _error_process;
	}
	// End <--

	pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		*pnValue = pNode->GetINT();
		
		return TRUE;
	}

_error_process:
	
	*pnValue = nDefault;
	
	return FALSE;
}

BOOL KTabFile::GetFloat( int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab )
{
	int nColumn = 0;

	if ( bColumnLab )
	{
		nColumn = FindColumn( szColumn );
	}
	else
	{
		nColumn = Str2Col( szColumn );
	}

	if ( nRow <= 0 || nColumn <= 0 )
	{
		return FALSE;
	}

	CContextNode *pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		*pfValue = fDefault;

		return FALSE;
	}
	
	*pfValue = pNode->GetFloat();

	return TRUE;
}

BOOL KTabFile::GetFloat( int nRow, int nColumn, float fDefault, float *pfValue )
{
	if ( nRow <= 0 || nColumn <= 0 || 
		// Add by cooler 2004-03-18
		// Begin -->
		nRow > m_nRow || nColumn > m_nCol)
	{
		*pfValue = fDefault;
		// End <--

		return TRUE;
	}

	CContextNode *pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		*pfValue = fDefault;

		return FALSE;
	}

	*pfValue = pNode->GetFloat();

	return TRUE;
}

BOOL KTabFile::GetFloat( LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue )
{
	int nRow = 0, nColumn = 0;

	nRow = FindRow( szRow );
	nColumn = FindColumn( szColumn );

	// Add by cooler 2004-03-18
	// Begin -->
	if(nRow <= 0 || nColumn <= 0)
	{
		return FALSE;
	}
	// End <--

	CContextNode *pNode = ( CContextNode * )( ( DWORD * )( m_pdwAddrTable[ ( nRow - 1 ) * m_nCol + ( nColumn - 1 ) ] ) );

	if ( NULL == pNode )
	{
		*pfValue = fDefault;

		return FALSE;
	}
	
	*pfValue = pNode->GetFloat();

	return TRUE;
}

void KTabFile::Clear()
{
	m_nRow = 0;
	m_nCol = 0;

	SAFE_DELETE_ARRAY( m_pdwAddrTable );

	KNode * pNode = NULL;

	while( pNode = m_theItemList.GetTail() )
	{
		m_theItemList.RemoveTail();

		SAFE_DEL( pNode );
	}
}
*/
//]]

#ifndef _SERVER
#include "KCodec.h"
#endif

#include <string.h>
//---------------------------------------------------------------------------
// 函数:	KTabFile
// 功能:	购造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KTabFile::KTabFile()
{
	m_Width		= 0;
	m_Height	= 0;
}
//---------------------------------------------------------------------------
// 函数:	~KTabFile
// 功能:	析造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KTabFile::~KTabFile()
{
	Clear();
}
//---------------------------------------------------------------------------
// 函数:	Load
// 功能:	加载一个Tab文件
// 参数:	FileName	文件名
// 返回:	TRUE		成功
//			FALSE		失败
//---------------------------------------------------------------------------
BOOL KTabFile::Load(LPCTSTR FileName)
{
	KPakFile	File;
	DWORD		dwSize;
	PVOID		Buffer;

	// check file name
	if (FileName[0] == 0)
		return FALSE;

	if (!File.Open(FileName))
	{
		g_DebugLog("Can't open tab file : %s", FileName);
		return FALSE;
	}

	dwSize = File.Size();

	Buffer = m_Memory.Alloc(dwSize);

	File.Read(Buffer, dwSize);
	
	if (dwSize)
		CreateTabOffset();
	else
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// 函数:	CreateTabOffset
// 功能:	建立制作表符分隔文件的偏移表
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTabFile::CreateTabOffset()
{
	int		nWidth, nHeight, nOffset, nSize;
	BYTE	*Buffer;
	TABOFFSET *TabBuffer;

	nWidth	= 1;
	nHeight	= 1;
	nOffset = 0;

	Buffer	= (BYTE *)m_Memory.GetMemPtr();
	nSize	= m_Memory.GetMemLen();
	
	if (!Buffer || !nSize)
		return;

	// 读第一行决定有多少列
	while (*Buffer != 0x0d && *Buffer != 0x0a)
	{
		if (*Buffer == 0x09)
		{
			nWidth++;
		}
		Buffer++;
		nOffset++;
	}
	if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
	{
		Buffer += 2;	// 0x0a跳过		
		nOffset += 2;	// 0x0a跳过
	}
	else
	{
		Buffer += 1;	// 0x0a跳过		
		nOffset += 1;	// 0x0a跳过
	}
	while(nOffset < nSize)
	{
		while (*Buffer != 0x0d && *Buffer != 0x0a)
		{
			Buffer++;
			nOffset++;
			if (nOffset >= nSize)
				break;
		}
		nHeight++;
		if (*Buffer == 0x0d && *(Buffer + 1) == 0x0a)
		{
			Buffer += 2;	// 0x0a跳过		
			nOffset += 2;	// 0x0a跳过
		}
		else
		{
			Buffer += 1;	// 0x0a跳过		
			nOffset += 1;	// 0x0a跳过
		}
	}
	m_Width		= nWidth;
	m_Height	= nHeight;

	TabBuffer = (TABOFFSET *)m_OffsetTable.Alloc(m_Width * m_Height * sizeof (TABOFFSET));
	Buffer = (BYTE *)m_Memory.GetMemPtr();

	nOffset = 0;
	int nLength;
	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			TabBuffer->dwOffset = nOffset;	
			nLength = 0;
			while(*Buffer != 0x09 && *Buffer != 0x0d && *Buffer != 0x0a && nOffset < nSize)
			{
				Buffer++;
				nOffset++;
				nLength++;
			}
			Buffer++;	// 0x09或0x0d或0x0a(linux)跳过
			nOffset++;
			TabBuffer->dwLength = nLength;
			TabBuffer++;
			if (*(Buffer - 1) == 0x0a || *(Buffer - 1) == 0x0d)	//	本行已经结束了，虽然可能没到nWidth //for linux modified [wxb 2003-7-29]
			{
				for (int k = j+1; k < nWidth; k++)
				{
					TabBuffer->dwOffset = nOffset;
					TabBuffer->dwLength = 0;
					TabBuffer++;					
				}
				break;
			}
		}

		//modified for linux [wxb 2003-7-29]
		if (*(Buffer - 1) == 0x0d && *Buffer == 0x0a)
		{
			Buffer++;				// 0x0a跳过	
			nOffset++;				// 0x0a跳过	
		}
	}
}

//---------------------------------------------------------------------------
// 函数:	Str2Column
// 功能:	取得某行某列字符串的值
// 参数:	szColumn
// 返回:	第几列
//---------------------------------------------------------------------------
int KTabFile::Str2Col(LPSTR szColumn)
{
	int	nStrLen = g_StrLen(szColumn);
	char	szTemp[4];

	g_StrCpy(szTemp, szColumn);
	g_StrUpper(szTemp);
	if (nStrLen == 1)
	{
		return (szTemp[0] - 'A');
	}
	return ((szTemp[0] - 'A' + 1) * 26 + szTemp[1] - 'A') + 1;
}

//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab)
{
	int nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行	（关键字）
//			szColomn		列	（关键字）
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize)
{
	int nRow, nColumn;

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetString
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize)
{
	if (GetValue(nRow - 1, nColumn - 1,  lpRString, dwSize))
		return TRUE;
	g_StrCpyLen(lpRString, lpDefault, dwSize);
	return FALSE;
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab)
{
	char	Buffer[32];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue)
{
	int		nRow, nColumn;
	char	Buffer[32];

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetInteger
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)
{
	char	Buffer[32];

	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pnValue = atoi(Buffer);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return TRUE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab)
{
	char	Buffer[32];
	int		nColumn;
	if (bColumnLab)
		nColumn = FindColumn(szColumn);
	else
		nColumn = Str2Col(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	szRow			行
//			szColomn		列
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue)
{
	int		nRow, nColumn;
	char	Buffer[32];

	nRow = FindRow(szRow);
	nColumn = FindColumn(szColumn);
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetFloat
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行		从1开始
//			nColomn			列		从1开始
//			nDefault		缺省值
//			pnValue			返回值
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)
{
	char	Buffer[32];
	
	if (GetValue(nRow - 1, nColumn - 1, Buffer, sizeof(Buffer)))
	{
		*pfValue = (float)atof(Buffer);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetValue
// 功能:	取得某行某列字符串的值
// 参数:	nRow			行
//			nColomn			列
//			lpDefault		缺省值
//			lpRString		返回值
//			dwSize			返回字符串的最大长度
// 返回:	是否成功
//---------------------------------------------------------------------------
BOOL KTabFile::GetValue(int nRow, int nColumn, LPSTR lpRString, DWORD dwSize)
{
	if (nRow >= m_Height || nColumn >= m_Width || nRow < 0 || nColumn < 0)
		return FALSE;

	TABOFFSET	*TempOffset;
	LPSTR		Buffer;

	Buffer = (LPSTR)m_Memory.GetMemPtr();
	TempOffset = (TABOFFSET *)m_OffsetTable.GetMemPtr();
	TempOffset += nRow * m_Width + nColumn;

	ZeroMemory(lpRString, dwSize);
	Buffer += TempOffset->dwOffset;
	if (TempOffset->dwLength == 0)
	{
		return FALSE;
	}
	if (dwSize > TempOffset->dwLength)
	{
		memcpy(lpRString, Buffer, TempOffset->dwLength);
		lpRString[TempOffset->dwLength] = 0;
	}
	else
	{
		memcpy(lpRString, Buffer, dwSize);
		lpRString[dwSize] = 0;
	}

	return TRUE;
}
//---------------------------------------------------------------------------
// 函数:	Clear
// 功能:	清除TAB文件的内容
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KTabFile::Clear()
{
	m_Memory.Free();
	m_OffsetTable.Free();
}
//---------------------------------------------------------------------------
// 函数:	FindRow
// 功能:	查找行关键字
// 参数:	szRow（行关键字）
// 返回:	int
//---------------------------------------------------------------------------
int KTabFile::FindRow(LPSTR szRow)
{
	char	szTemp[128];
	for (int i = 0; i < m_Height; i++)	// 从1开始，跳过第一行的字段行
	{
		GetValue(i, 0, szTemp, sizeof(szTemp));
		if (g_StrCmp(szTemp, szRow))
			return i + 1; //改动此处为加一 by Romandou,即返回以1为起点的标号
	}
	return -1;
}
//---------------------------------------------------------------------------
// 函数:	FindColumn
// 功能:	查找列关键字
// 参数:	szColumn（行关键字）
// 返回:	int
//---------------------------------------------------------------------------
int KTabFile::FindColumn(LPSTR szColumn)
{
	char	szTemp[128];
	for (int i = 0; i < m_Width; i++)	// 从1开始，跳过第一列的字段行
	{
		GetValue(0, i, szTemp, sizeof(szTemp));
		if (g_StrCmp(szTemp, szColumn))
			return i + 1;//改动此处为加一 by Romandou,即返回以1为起点的标号
	}
	return -1;
}

//---------------------------------------------------------------------------
// 函数:	Col2Str
// 功能:	把整数转成字符串
// 参数:	szColumn
// 返回:	第几列
//---------------------------------------------------------------------------
void KTabFile::Col2Str(int nCol, LPSTR szColumn)
{

	if (nCol < 26)
	{
		szColumn[0] = 'A' + nCol;
		szColumn[1]	= 0;
	}
	else
	{
		szColumn[0] = 'A' + (nCol / 26 - 1);
		szColumn[1] = 'A' + nCol % 26;
		szColumn[2] = 0;
	}
}

