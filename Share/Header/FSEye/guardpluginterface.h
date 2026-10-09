//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-02 17:00
//      File_base        : guardpluginterface.h
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLUG_INTERFACE_H_
#define _PLUG_INTERFACE_H_

#define interface struct

#define CREATEFUNCTIONNAME "CreatePlugInterface"

interface ISender
{
	virtual int SendPackToServer( 
		unsigned char* pData, 
		unsigned int datasize,
		bool bResetTS = false )	= 0;
};

interface IGuardPlugin
{
	virtual int LoadPlug( ISender*	pSender )	= 0;
	
	virtual int Release( )					= 0;

	virtual int ProcessNetMessage(
			unsigned char* pData,
			unsigned int datasize )			= 0;

	virtual int Disconnect( )				= 0;

	virtual int GetDescribe( 
			char* szDes,
			int nSize )						= 0;

	virtual int GetGUID( 
			char* szGUID,
			int nSize )						= 0;

	virtual int GetVersion( 
			int& nMaxVer, 
			int& nMinVer )					= 0;

	virtual int GetAuthor(
			char* szAuthor,
			int nSize  )					= 0;

	virtual int Breathe( )					= 0;
};

typedef void (*PCREATEPLUGINTERFACE)(
			IGuardPlugin* & pPlug );
#endif