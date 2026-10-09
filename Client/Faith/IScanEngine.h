/////////////////////////////////////////////////////////////////////////////
//  FileName    :   ScanEngine.h
//  Creater     :   zuolizhi(zolazuo)
//  Date        :   2004-11-25 15:48:00
//  Comment     :   ScanEngine
/////////////////////////////////////////////////////////////////////////////
#ifndef _ISCANENGINE_H_
#define _ISCANENGINE_H_

#define interface struct

struct _PlugSign4 {
	int sign[4];
};

struct _PlugSign8 {
	int sign[8];
};

interface IScanEngine
{
	/*!
	* ... Load Sign ...
	*/
	virtual int LoadSign( _PlugSign4 Sign[], int nCount ) = 0;
	/*!
	* ... Clear Sign ...
	*/
	virtual void UnloadSign( ) = 0;


	/*!
	* ... Load Scan Virtual Address ...
	*/
	virtual int LoadTarget( int Target[], int nCount ) = 0;
	/*!
	* ... Clear Virtual Address ...
	*/
	virtual void UnloadTarget( ) = 0;


	/*!
	* ... Scan All Process ...
	* ... The return Value is TRUE represent that a bot is in memory,
	* ... reverse have not bot ...
	*/
	virtual int ScanMemory( ) = 0;
	/*!
	* ... Scan Single Sign ...
	* ... The return value is same as up function ...
	*/
	virtual int ScanSign( _PlugSign4 Sign ) = 0;
};

int CreateScanEngine( IScanEngine*& pScanEngine );

#endif