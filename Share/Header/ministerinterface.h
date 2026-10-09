/////////////////////////////////////////////////////////////////////////////
//  FileName    :   ministerinterface.h
//  Creator     :   zuolizhi
//  Date        :   2006-5-30 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////
#ifndef _MINISTER_INTERFACE_H_
#define _MINISTER_INTERFACE_H_

#define interface struct

#ifndef INVALID_VALUE
#define INVALID_VALUE	( -1 )
#define SUCCESS_VALUE	( 0 )
#endif

typedef int (*PREQCALLBACK)
	(
		int nType,
		char* pSaveBuf,
		unsigned int nSize,
		void* Param
	);

enum
{
	error_code_neterr,
	error_code_memerr,
};
/////////////////////////////////////////////////////////////////////////////
//
//              Interface Declare
//
/////////////////////////////////////////////////////////////////////////////
interface IProcParam;
interface IProcRet;

enum
{
	protocol_type_paysys,
	protocol_type_db,
	protocol_type_client,
	protocol_type_clientshutdown,
	protocol_type_guard,
};

interface IController
{
	/* Initial function */
	virtual int Startup( int Daemon )	= 0;
	virtual int Stop( )					= 0;
	virtual int Release( )				= 0;
	virtual int Ishalt( )				= 0;
	virtual int IsStop( )				= 0;
	virtual void yield( )				= 0;
	virtual int GetLastErr( )			= 0;
	
	virtual int PushData( 
		int nType, 
		unsigned long ulNetID,
		const void* pData, 
		unsigned int datasize )			= 0;

	virtual IProcParam* GetProcParam( )	= 0;
	virtual IProcRet* GetProcRet( 
		char* szData,
		int nSize,
		char* szObjBuf = 0,
		int nObjSize = 0)				= 0;
	virtual int	CallProc( 
		int nSlot, 
		IProcParam* pParam )			= 0;

	virtual unsigned long GetTickCounter( ) = 0;
	virtual int GenGUID( 
		char szGUID[], 
		unsigned long ulPadding )		= 0;
	
	virtual int GetServerID( )			= 0;
	virtual int ReLogin( 
		unsigned long ulNetID )			= 0;
	
	virtual int PaysysIsValid( )		= 0;
};

int CreateController( IController* & pController );

#endif