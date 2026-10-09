/////////////////////////////////////////////////////////////////////////////
//  FileName    :   IScanEngine.h
//  Creater     :   zuolizhi(zolazuo)
//  Date        :   2004-10-25 15:48:00
//  Comment     :   ScanEngine
/////////////////////////////////////////////////////////////////////////////
#ifndef _ISCANENGINE_H_
#define _ISCANENGINE_H_

#define interface struct

#define SIGNLEN 4 //DWORD

struct _PlugSign4 {
	int sign[SIGNLEN];
	int plugtype;
};

enum
{
	scan_process,
	scan_dll
};

struct _ScanTarget {
	unsigned long	VirtualAddress;
	int				nScanType;
};

struct _PSHeader
{
	unsigned short	usSign;
	unsigned long	ulSignOffset;
	unsigned long	ulSignLen;
	unsigned long	ulMinVersion;
	unsigned long	ulMajVersion;
};

interface IPlugSign
{

	virtual int LoadSignFromFile( const char* szFileName ) = 0;

	virtual int LoadSignFromFileBuf( const char* szBuffer, int nBufLen ) = 0;
	/*!
	* ... Load Sign ...
	*/
	virtual int LoadSignBuffer( const char* szBuffer, int nBufLen ) = 0;
	
	/*!
	* ... Clear Sign ...
	*/
	virtual void UnloadSign( ) = 0;

	/*
	 *	... Scan Sign ...
	 */
	virtual int IsPlug( _PlugSign4& sign ) = 0;

	virtual int GetSignVersion(
		unsigned long& ulMajVersion,
		unsigned long& ulMinVersion ) = 0;

	virtual int SetSignVersion(
		unsigned long ulMajVersion,
		unsigned long ulMinVersion ) = 0;

	virtual void AddRef( )	= 0;	
	virtual void Release( ) = 0;
};

interface IScanEngine
{
	/*!
	* ... Load Scan Virtual Address ...
	*/
	virtual int LoadTarget( _ScanTarget Target[], int nCount ) = 0;

	/*!
	* ... Clear Virtual Address ...
	*/
	virtual void UnloadTarget( ) = 0;

	/*!
	* ... Set Sign Interface ...
	*/
	virtual int SetSign( IPlugSign* pSign ) = 0;

	/*!
	* ... Scan All Process ...
	* ... The return Value is TRUE represent that a bot is in memory,
	* ... reverse have not bot ...
	*/
	virtual int ScanMemory( ) = 0;

	virtual void Release( ) = 0;
};

interface IScanDll
{
	virtual int Init( ) = 0;

	virtual int LoadTarget( _ScanTarget Target[], int nCount ) = 0;
	
	virtual void UnloadTarget( ) = 0;

	virtual int SetSign( IPlugSign* pSign ) = 0;
	
	virtual int ScanDll( ) = 0;

	virtual void Release( ) = 0;
};

enum
{
	fun_scanprocess,
	fun_scanmemory
};

int CreateScanEngine( IScanEngine*& pScanEngine );
int CreateScanDll( IScanDll*& pScanDll );

int CreatePlugSign( IPlugSign*&	pPlugSign );

#endif