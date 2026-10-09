//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_action
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _BUFF_ACTION_H_
#define _BUFF_ACTION_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Header Include
//
/////////////////////////////////////////////////////////////////////////////


#include "buff_def.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////

namespace buff_str
{
	void ClearChar( 
		char* szAction, 
		char cOne, 
		char cTwo = 0 );
	
	int	FindDelimter( 
		char* szAction, 
		char cOne, 
		char cTwo = 0 );
	
	void ConvLowerCase( 
		char* szAction );
	
	int	CheckAct( 
		char* szAction );
	
	PBUFFACTION	FindAct( 
		char* szAction );
	
	char* FindCloseComma(
		char* szAction );

	char* FindAddiOp( 
		char* szAction, 
		int& nAddiOp );

	int FindEffectEventType( char* szType );
	
};

typedef struct _Buff_Dync_Param 
{
	struct _DyncAct {
		PBUFFACTION	pAct;
		BUFF_PARAM	Param;
		int nAdditiveCount;
		int nAdditiveValue[BUFF_MAX_ADDIOP];
		BYTE nAdditiveOP[BUFF_MAX_ADDIOP];
		int nLogicNot;
	};

	int			nCount;
	_DyncAct	_DAct[BUFF_MAX_PARAM];
	_Buff_Dync_Param( ):nCount(0){ memset( _DAct, 0, sizeof(_DAct) ); }
	
	int operator( )( 
		int nIndex,
		BUFF_ENV_PARAM& Env )
	{
		int nRet = 0;

		if( _DAct[nIndex].pAct )
		{
			nRet = _DAct[nIndex].pAct( Env, _DAct[nIndex].Param );

			for( int nLoopCount = 0; nLoopCount < _DAct[nIndex].nAdditiveCount; nLoopCount++ )
			{
				switch( _DAct[nIndex].nAdditiveOP[nLoopCount] ) 
				{
				case buff_addi_op_add:
					nRet += _DAct[nIndex].nAdditiveValue[nLoopCount];
					break;
				case buff_addi_op_sub:
					nRet -= _DAct[nIndex].nAdditiveValue[nLoopCount];
					break;
				case buff_addi_op_div:
					_DAct[nIndex].nAdditiveValue[nLoopCount] ? ( nRet  /= _DAct[nIndex].nAdditiveValue[nLoopCount] ) : 0;
					break;
				case buff_addi_op_mul:
					nRet *= _DAct[nIndex].nAdditiveValue[nLoopCount];
					break;
				default:
					break;
				}
			}
		}

		return nRet;
	}

	int operator=( char* szAction )
	{
		int nOp = buff_addi_op_none;
		char szActName[MAX_BUFF_DESC] = {0};

		char* szFun = buff_str::FindAddiOp( szAction, nOp );

		if( szFun[0] == NOTDELIMITERC )
		{
			buff_str::ClearChar( szFun, NOTDELIMITERC );
			_DAct[nCount].nLogicNot = effect_p_op_not;
		}

		sscanf( 
			szFun,
			PATTERN,
			szActName );
		
		if( _DAct[nCount].pAct = buff_str::FindAct( szActName ) )
		{
			strcpy( szFun, strstr( szFun, FUNLDELIMITER ) + 1 );

			buff_str::ClearChar( szFun, FUNLDELIMITERC, FUNRDELIMITERC );	
			
			char* szParam = strtok( szFun, FUNPDELIMITER );	
			while( szParam &&
				_DAct[nCount].Param( ) < BUFF_MAX_PARAM )
			{
				_DAct[nCount].Param = ::atoi( szParam );
				
				szParam = strtok( NULL, FUNPDELIMITER );
			}

			//parse additive op
			_DAct[nCount].nAdditiveCount = 0;
			
			szFun = buff_str::FindAddiOp( NULL, nOp );
			while( szFun )
			{
				_DAct[nCount].nAdditiveValue[_DAct[nCount].nAdditiveCount] = atoi( szFun );
				_DAct[nCount].nAdditiveOP[_DAct[nCount].nAdditiveCount] = nOp;

				szFun = buff_str::FindAddiOp( NULL, nOp );
				_DAct[nCount].nAdditiveCount++;
			}

			nCount++;
			return TRUE;
		}

		nCount++;
		return FALSE;
	}

}BUFF_DYNC_PARAM;

class BuffAction
{
	struct _Act {
			PBUFFACTION	pAct;
			BUFF_PARAM	Param;
			BUFF_DYNC_PARAM	DParam;
			int nParamType;
			int	nLogic;
			int nLogicNot;
		};

public:

	BuffAction( )
	{
		Clear( );
	}

	int	ParseAction( 
		char* szAction );

	int operator( )( 
		BUFF_ENV_PARAM& );

private:

	void Clear( )
	{
		memset( m_Act, 0, sizeof(m_Act) );
		m_nCount = 0;
	}

	int ParseOne( 
		char* szAct, 
		int nLogic );

	void CalcDyncParam( 
		int nIndex,
		BUFF_ENV_PARAM& Env );

private:

	_Act	m_Act[MAX_BUFFFUN];
	int		m_nCount;
};

//=====================================================================================


/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////

extern char* g_PE_Type[];
extern BUFF_PARAM	g_GlobalVar;

#endif