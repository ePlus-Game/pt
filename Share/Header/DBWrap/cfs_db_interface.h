//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-9-24
//      File_base        : cfs_db_interface
//      File_ext         : h
//      Author           : zuolizhi
//      Description      : FSOnline DB Interface
//
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS_DB_INTERFACE_H_
#define _CFS_DB_INTERFACE_H_

#define interface struct 

struct BinPair 
{
	BinPair( void* pData, unsigned long ulSize ) : 
	m_pData( pData ), m_ulSize( ulSize )
	{}
	BinPair( const void* pData, unsigned long ulSize ) : 
	m_pData( pData ), m_ulSize( ulSize )
	{}
	const void* m_pData;
	unsigned long m_ulSize;
};

struct NullPair 
{
	NullPair(  ){}
};

//////////////////////////////////////////////////////////////////////////

interface IProcParam 
{
	virtual void BeginPush( 
		const char* szProcName )			=	0;

	virtual void EndPush(
		const char* pPassBy = 0, 
		int nSize = 0 )						=	0;

	virtual	char* To( 
		unsigned int& nLen )				=	0;
	
	virtual int	Push( NullPair )			=	0;
	virtual int	Push( BinPair )				=	0;

	virtual int	Push( int )					=	0;
	virtual int	Push( unsigned int )		=	0;

	virtual int	Push( long )				=	0;
	virtual int	Push( unsigned long )		=	0;

	virtual int	Push( const char* )			=	0;
	virtual int	Push( const unsigned char* )=	0;
	virtual int Push( unsigned char )		=	0;

	virtual void Release( )					=	0;

	//ÇëÎðÊ¹ÓÃ
	virtual char* GetPassByPtr( )			=	0;
};

//////////////////////////////////////////////////////////////////////////

interface IProcRet 
{
	virtual int GetRowCount( )				=	0;
	virtual int	GetColCount( )				=	0;
	virtual int GetSlot( )					=	0;
	virtual int GetRet( )					=	0;
	virtual int GetExeRet( )				=	0;

	virtual	int From( 
		char* szBuffer,
		int nLen )							=	0;
	virtual char* GetPassBy( 
		int& nSize )						=	0;

	//data block
	virtual int GetData(
		int nRow, 
		int nCol, 
		char* szData, 
		int nSize )							=	0;
	virtual int GetData(
		int nRow, 
		int nCol, 
		char** szData )						=	0;

	virtual int GetData(
		int nRow, 
		int nCol, 
		unsigned char* szData, 
		int nSize )							=	0;
	virtual int GetData(
		int nRow, 
		int nCol, 
		unsigned char** szData )			=	0;

	virtual int GetData(
		int nRow, 
		int nCol, 
		void* pData, 
		int nSize )							=	0;
	virtual int GetData(
		int nRow, 
		int nCol, 
		void** pData)						=	0;

	//numeric
	virtual void GetData(
		int nRow, 
		int nCol, 
		int& nValue)						=	0;

	virtual void GetData(
		int nRow, 
		int nCol, 
		unsigned int& nValue)				=	0;

	virtual void GetData(
		int nRow, 
		int nCol, 
		long& nValue)						=	0;
	
	virtual void GetData(
		int nRow, 
		int nCol, 
		unsigned long& nValue)				=	0;

	virtual void GetData(
		int nRow, 
		int nCol, 
		unsigned char& nValue)				=	0;
};

//////////////////////////////////////////////////////////////////////////

typedef int (*DBCALLBACK)(
	void *pRetData, 
	int nRetDataSize );

interface ICFSDBService
{
	virtual int Start( 
		DBCALLBACK CallBack )				=	0;
	virtual void Stop( )					=	0;
	virtual void Release( )					=	0;

	virtual IProcParam* GetProcParam( )		=	0;

	virtual IProcRet* GetProcRet( 
		char* szData,
		int nSize,
		char* szObjBuf = 0,
		int nObjSize =	0)					=	0;

	virtual int	CallProc( 
		int nSlot, 
		IProcParam* pParam )				=	0;
};

ICFSDBService* CreateDBService( );

//////////////////////////////////////////////////////////////////////////

enum
{
	cfs_db_cnn_role,
	cfs_db_cnn_log,
	cfs_db_cnn_mail_auction,
	cfs_db_cnn_global_npcsave,
	cfs_db_cnn_dblink,


	cfs_db_cnn_end
};

//////////////////////////////////////////////////////////////////////////

#endif // _CFS_DB_INTERFACE_H_
