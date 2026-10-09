//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KCore.cpp
// Date:	2000.08.08
// Code:	Daphnis Wang
// Desc:	Core class
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KEngine.h"
#include "KFilePath.h"
#include "specialskill_tab.h"
#include "OnceIBItemMgr.h"
#include "exp_insruance.h"
#ifndef _SERVER
//#include <fstream>
#include "KNpcResList.h"
#include "KBmpFile.h"
#include "ImgRef.h"
#include "iRepresentshell.h"
#endif
#include "KItemChangeRes.h"
#include "KNpcSet.h"
#include "KTabFile.h"
#include "KPlayerSet.h"
#include "kiteminlayrule.h"
#include "kiteminlayaddontable.h"
#ifndef _SERVER
#include "KPlayerTeam_C.h"
#include "KMissleSet.h"
#else
#include "KPlayerTeam_S.h"
#endif

#include "insurance_common.h"

//Add by brianyao 2007
#ifdef _SERVER
#include "ITaisuiWheelSettingMgr.h"
#include "KTaisuiWheelServer.h"
#include "tong_war_manager.h"
#include "pool_combat_mgr.h"
#include "social_recruit_svr.h"
#include "keconomysys.h"

extern    TaisuiGlobal g_TaisuiGlobal;
#endif

#include "KMath.h"
#include "time.h"
#include "KPlayerTask.h"
#include "KSubWorldSet.h"
#include "KItem.h"
#include "KItemGenerator.h"
#include "KObjSet.h"
#include "KItemSet.h"
#include "KNpc.h"
#include "KNpcTemplate.h"
#include "CoreUseNameDef.h"
#include "KBuySell.h"
#include "KSmithShop.h"
#include "KSortScript.h"
#include "LuaFuns.h"
#include "KPlayer.h"
#include "FilterText.h"

#ifndef WIN32 /* LINUX */
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#pragma warning (disable: 4512)
#pragma warning (disable: 4786)

#include "KCompoundRule.h"
#include "KIniFile.h"

#ifdef _SERVER
#include "PlayerCreator.h"
#include "ChatCenter_S.h"
#include "npc_save.h"
#include "player_monitor.h"
#include "IBCenter_S.h"
#include "ServerSocialUnitMgr.h"
#include "recommender.h"
#else
#include "ChatCenter_C.h"
#include "ClientAuctionMgr.h"
#include "client_combat_info.h"
#include "screeneffect_tab.h"
#include "screeneffect_man.h"
#include "io.h"
#endif
#include "pluspoint.h"

#include "CoreRelated.h"

int g_nScreenWidth = 1024;
int g_nScreenHeight = 768;

#ifdef _SERVER
unsigned int g_uShareExpDistance[] = {1024, 1024};
#endif

int g_ChanceTable[MAX_CHANCE_TABLE_ELEMENT];

#ifdef _SERVER
extern int g_WayPointPriceUnit;	//WayPoint表格中价格的单位量，WayPoint价格 = 单位量 * 表格数值
extern int g_StationPriceUnit;	//Station表格中价格的单位量，Station价格 = 单位量 * 表格数值
extern int g_DockPriceUnit;
#endif

//extern size_t GetIntervalTime(int nHour, int nMin, int nFurtherHour, int nFurtherMin);
#ifndef _SERVER

#define ADJUSTCOLOR_TABFILE				"\\settings\\AdjustColor.txt"	
unsigned int		InitAdjustColorTab();

unsigned int	* g_pAdjustColorTab = NULL;
unsigned int g_ulAdjustColorCount = 0;

#endif

//BOOL	InitTaskSetting();

#ifndef _SERVER
#include "Scene/KScenePlaceC.h"
BOOL g_bUISelIntelActiveWithServer = TRUE;//当前选择框是否与服务器端交互
BOOL g_bUISpeakActiveWithServer = FALSE;
int	g_bUISelLastSelCount = 0;
extern KTabFile g_StringResourseTabFile;
#endif


#ifdef _SERVER
IServer* g_pServer;
ILogSystem* g_pLogSystem;
#else
IClient* g_pClient;
int		 g_ConnectID;
BOOL	 g_bPingReply;
#endif

#ifdef _SERVER
#include "KWarInfoManager.h"
#endif

unsigned long TIMEZONE_CORRECT;

//---------------------------------------------------------------------------
KTabFile		g_OrdinSkillsSetting, g_MisslesSetting;


KTabFile		g_SkillLevelSetting;
KTabFile		g_NpcSetting;
KTabFile		g_NpcKindFile;
int				g_nHandSkill;
#ifndef	_SERVER
KSoundCache		g_SoundCache;
KMusic			*g_pMusic = NULL;
#endif

#ifdef _SERVER
#include "ITaisuiWheelEventMgr.h"
#include "ITaisuiWheelTianXiangMgr.h"
#include "question.h"
#endif

KLuaScript	*	g_pNpcLevelScript = NULL;
KLuaScript g_WorldScript;
void g_InitProtocol();

//---------------------------------------------------------------------------
#ifdef __linux
#include <sys/times.h>
#endif

//add by zuolizhi for pic question
#ifdef _SERVER

extern QUESTION_VECTOR		g_PicQuestion;
extern QUESTION_VECTOR		g_NumQuestion;
extern PQ_CONFIG			g_PQConfig;
extern LEVELKILLNPCVECTOR	g_KillNpcConfig;

//======================================================

void LoadQuestionPic( 
		 PIC_VECTOR& PicQueue,
		 const char* szFileName)
{
#define MAXSCAN		(1000)
#define EXTEND		".bmp"
	
	int		nLoopCount;
	PIC		_Pic;
	FILE*	file = NULL;
	long	fsize = 0;
	char	szPathFile[MAX_PATH];
	
	for( nLoopCount = 0;nLoopCount < MAXSCAN;nLoopCount++ )
	{
		if( nLoopCount == 100 )
			int a = 10;
		sprintf(
			szPathFile, 
			"%s%02d%s", 
			szFileName,
			nLoopCount,
			EXTEND );
		
		file = fopen( szPathFile,"rb" );
		
		if( file != NULL )
		{
			fseek( file, 0, SEEK_END );
			fsize = ftell( file );
			fseek( file, 0, SEEK_SET );
			fread( _Pic.szBuffer, 1, fsize, file );
			_Pic.nLen = fsize;
			fclose(file);
			file = NULL;
			
			PicQueue.push_back( _Pic );
		}
	}
}
/*
int RandomMix16BitBMP(
		PIC_VECTOR& BackQueue, 
		PIC_VECTOR& FrontQueue,
		PIC_VECTOR& DisturbQueue,
		QUESTION_VECTOR& OutQuestionQueue,
		int nRandomSeed)
{
#define BITCOUNT	(16)
#define MASKCOLOR	(0x7C1F)
#define DISPICCOUNT (30)

	char szOutPutBuffer[QUESTIONSIZE];
	int	nBufLen = 0;

	int nLoopCountH		=	0;
	int nLoopCountW		=	0;
	int nDisCount		=	0;
	int	nLoopCount		=	0;
	
	int nBakBMPWidth	=	0;
	int nBakBMPHeight	=	0;

	int nDisBMPWidth	=	0;
	int nDisBMPHeight	=	0;

	int nFrtBMPWidth	=	0;
	int nFrtBMPHeight	=	0;

	int nRanX	=	0;
	int nRanY	=	0;
	int nAlpha	=	0;

	int nBakStep	=	0;
	int nDisStep	=	0;
	int nFrtStep	=	0;
	

	PBITMAPFILEHEADER BMPFileHeader = NULL;
	PBITMAPINFOHEADER BMPInfoHeader = NULL;
	
	PWORD	pBakBMPColorTable = NULL;
	PWORD	pDisBMPColorTable = NULL;
	PWORD	pFrtBMPColorTable = NULL;
	
	PBYTE	pBakData = NULL;
	PBYTE	pDisData = NULL;
	PBYTE	pFrtData = NULL;

	PIC_QUESTION _Question;

	if( BackQueue.size() == 0 ||
		FrontQueue.size() == 0 ||
		DisturbQueue.size() == 0 )
		return FALSE;
	//===========================================================
	//
	for( nLoopCount = 0; nLoopCount < FrontQueue.size(); nLoopCount++)
	{
		//
		srand( nRandomSeed );
		PIC& _PicBak = BackQueue[ rand() % BackQueue.size() ];

		memcpy(
			szOutPutBuffer,
			_PicBak.szBuffer,
			_PicBak.nLen);

		nBufLen = _PicBak.nLen;

		BMPFileHeader = (PBITMAPFILEHEADER)szOutPutBuffer;
		pBakData = (PBYTE)(szOutPutBuffer + BMPFileHeader->bfOffBits);
		
		BMPInfoHeader = (PBITMAPINFOHEADER)(szOutPutBuffer + sizeof(BITMAPFILEHEADER));

		nBakBMPWidth	=	BMPInfoHeader->biWidth;
		nBakBMPHeight	=	BMPInfoHeader->biHeight;
		
		if( BMPInfoHeader->biBitCount != BITCOUNT )
			return FALSE;

		//===================================================

		//
		PIC& _PicFrt = FrontQueue[nLoopCount];
		
		BMPFileHeader = (PBITMAPFILEHEADER)_PicFrt.szBuffer;
		pFrtData = (PBYTE)(_PicFrt.szBuffer + BMPFileHeader->bfOffBits);

		BMPInfoHeader = (PBITMAPINFOHEADER)(_PicFrt.szBuffer + sizeof(BITMAPFILEHEADER));
		
		nFrtBMPWidth	=	BMPInfoHeader->biWidth;
		nFrtBMPHeight	=	BMPInfoHeader->biHeight;
		
		if( BMPInfoHeader->biBitCount != BITCOUNT )
			return FALSE;

		if( nFrtBMPWidth > nBakBMPWidth )
			return FALSE;

		if( nFrtBMPHeight > nBakBMPHeight )
			return FALSE;
		//===================================================

		nBakStep	=	4 * ((BITCOUNT * nBakBMPWidth + 31) / 32);
		nFrtStep	=	4 * ((BITCOUNT * nFrtBMPWidth + 31) / 32);

		RGB555 _B;
		RGB555 _S;
		
		//====================================================	
		//
		for(nDisCount = 0;nDisCount < g_PQConfig.nDisPicCount;nDisCount++)
		{
			srand( nRandomSeed + (nLoopCount + nDisCount) * 3 );
			//
			PIC& _PicDis = DisturbQueue[ rand() % DisturbQueue.size() ];
			
			BMPFileHeader = (PBITMAPFILEHEADER)_PicDis.szBuffer;
			pDisData = (PBYTE)(_PicDis.szBuffer + BMPFileHeader->bfOffBits);
			
			BMPInfoHeader = (PBITMAPINFOHEADER)(_PicDis.szBuffer + sizeof(BITMAPFILEHEADER));
			
			nDisBMPWidth	=	BMPInfoHeader->biWidth;
			nDisBMPHeight	=	BMPInfoHeader->biHeight;
			
			if( BMPInfoHeader->biBitCount != BITCOUNT )
				return FALSE;
			
			nDisStep	=	4 * ((BITCOUNT * nDisBMPWidth + 31) / 32);			
			//----------------------------------------------------------
			
			srand( (nLoopCount + nDisCount) * 3 );
			nRanX	=	rand();
			srand( (nLoopCount + nDisCount) * 4 );
			nRanY	=	rand();
			srand( (nLoopCount + nDisCount) + 3 );
			nAlpha	=	rand();

			nRanX	%=	nBakBMPWidth;
			nRanY	%=	nBakBMPHeight;
			nAlpha	%=	g_PQConfig.nDisMaxAlpha;

			for( nLoopCountH = 0; nLoopCountH < nDisBMPHeight;nLoopCountH++ )
			{
				if( nLoopCountH + nRanY >= nBakBMPHeight )
					break;
				
				pBakBMPColorTable = (PWORD)( pBakData + (nLoopCountH + nRanY) * nBakStep );

				pDisBMPColorTable = (PWORD)( pDisData + nLoopCountH * nDisStep );

				for( nLoopCountW = 0; nLoopCountW < nDisBMPWidth;nLoopCountW++ )
				{
					if( nLoopCountW + nRanX >= nBakBMPWidth )
						break;
					
					if( pDisBMPColorTable[nLoopCountW] != MASKCOLOR )
					{
						_B._R = (pBakBMPColorTable[nLoopCountW + nRanX] & RMASK) >> 10;
						_B._G = (pBakBMPColorTable[nLoopCountW + nRanX] & GMASK) >> 5;
						_B._B = (pBakBMPColorTable[nLoopCountW + nRanX] & BMASK);

						_S._R = (pDisBMPColorTable[nLoopCountW] & RMASK) >> 10;
						_S._G = (pDisBMPColorTable[nLoopCountW] & GMASK) >> 5;
						_S._B = (pDisBMPColorTable[nLoopCountW] & BMASK);

						_B._R = ( nAlpha * (_S._R - _B._R)/0x1f + _B._R) ;
						_B._G = ( nAlpha * (_S._G - _B._G)/0x1f + _B._G) ;
						_B._B = ( nAlpha * (_S._B - _B._B)/0x1f + _B._B) ;

						pBakBMPColorTable[nLoopCountW + nRanX] = *((PWORD)(&_B));
					}
				}
			}
		}
		//====================================================
		
		srand( nRandomSeed + nLoopCount * 3 );
		nRanX	=	rand();
		srand( nRandomSeed + nLoopCount * 4 );
		nRanY	=	rand();
		srand( nRandomSeed + nLoopCount * 10 );
		nAlpha	=	rand();
		
		nRanX	%=	nBakBMPWidth - nFrtBMPWidth;
		nRanY	%=	nBakBMPHeight - nFrtBMPHeight;
		nAlpha	%=	(0x1F - g_PQConfig.nFrtMinAlpha);
		nAlpha	+=	g_PQConfig.nFrtMinAlpha;
		
		for( nLoopCountH = 0; nLoopCountH < nFrtBMPHeight;nLoopCountH++ )
		{
			if( nLoopCountH + nRanY >= nBakBMPHeight )
				break;
			
			pBakBMPColorTable = (PWORD)( pBakData + (nLoopCountH + nRanY) * nBakStep );
			
			pFrtBMPColorTable = (PWORD)( pFrtData + nLoopCountH * nFrtStep );
			
			for( nLoopCountW = 0; nLoopCountW < nFrtBMPWidth;nLoopCountW++ )
			{
				if( nLoopCountW + nRanX >= nBakBMPWidth )
					break;
				
				if( pFrtBMPColorTable[nLoopCountW] != MASKCOLOR )
				{	
					_B._R = (pBakBMPColorTable[nLoopCountW + nRanX] & RMASK) >> 10;
					_B._G = (pBakBMPColorTable[nLoopCountW + nRanX] & GMASK) >> 5;
					_B._B = (pBakBMPColorTable[nLoopCountW + nRanX] & BMASK);
					
					_S._R = (pFrtBMPColorTable[nLoopCountW] & RMASK) >> 10;
					_S._G = (pFrtBMPColorTable[nLoopCountW] & GMASK) >> 5;
					_S._B = (pFrtBMPColorTable[nLoopCountW] & BMASK);
					
					_B._R = ( nAlpha * (_S._R - _B._R)/0x1f + _B._R) ;
					_B._G = ( nAlpha * (_S._G - _B._G)/0x1f + _B._G) ;
					_B._B = ( nAlpha * (_S._B - _B._B)/0x1f + _B._B) ;
					
					pBakBMPColorTable[nLoopCountW + nRanX] = *((PWORD)(&_B));

				}
			}
		}
		//====================================================

		_Question.nAnswer = nLoopCount + 1;

		if( g_PQConfig.nCompress == FALSE )
		{
			_Question.nLen = nBufLen;
			
			memcpy(
				_Question.szQBuf,
				szOutPutBuffer,
				nBufLen);
		}
		else
		{
			unsigned int nCompressLen = 0;

			lzo1x_1_compress(
				(const unsigned char*)szOutPutBuffer,
				nBufLen,
				(unsigned char*)_Question.szQBuf,
				&nCompressLen,
				wrkmem);
			
			_Question.nLen = nCompressLen;
		}

		OutQuestionQueue.push_back( _Question );
	}//	for( nLoopCount = 0; nLoopCount < FrontQueue.size(); nLoopCount++)

	return TRUE;
}
*/

int RandomMixNumPic(
	PIC_VECTOR& BackQueue,
	PIC_VECTOR& DisturbQueue,
	PIC_VECTOR& NumQueue,
	unsigned int nNum,
	QUESTION_VECTOR& OutQuestionQueue,
	int nRandomSeed)
{
#define BITCOUNT	(16)
#define MASKCOLOR	(0x7C1F)
#define DISPICCOUNT (30)

	char szOutPutBuffer[QUESTIONSIZE];
	int	nBufLen = 0;

	char szNum[50];
	int	nNumCount = 0;

	int nLoopCountH		=	0;
	int nLoopCountW		=	0;
	int nDisCount		=	0;
	int	nLoopCount		=	0;
	
	int nBakBMPWidth	=	0;
	int nBakBMPHeight	=	0;

	int nDisBMPWidth	=	0;
	int nDisBMPHeight	=	0;

	int nFrtBMPWidth	=	0;
	int nFrtBMPHeight	=	0;

	int nRanX	=	0;
	int nRanY	=	0;
	int nAlpha	=	0;

	int nBakStep	=	0;
	int nDisStep	=	0;
	int nFrtStep	=	0;
	
	int nNumGridWidth	=	0;

	PBITMAPFILEHEADER BMPFileHeader = NULL;
	PBITMAPINFOHEADER BMPInfoHeader = NULL;
	
	PWORD	pBakBMPColorTable = NULL;
	PWORD	pDisBMPColorTable = NULL;
	PWORD	pFrtBMPColorTable = NULL;
	
	PBYTE	pBakData = NULL;
	PBYTE	pDisData = NULL;
	PBYTE	pFrtData = NULL;

	RGB555	_B	=	{0};
	RGB555	_S	=	{0};

	PIC_QUESTION _Question;


	if( BackQueue.size() == 0 ||
		NumQueue.size() == 0 ||
		DisturbQueue.size() == 0 )
		return FALSE;

	//===========================================================

	//随机选择一个背景
	srand( nRandomSeed );
	PIC& _PicBak = BackQueue[ rand() % BackQueue.size() ];

	memcpy(
		szOutPutBuffer,
		_PicBak.szBuffer,
		_PicBak.nLen);

	nBufLen = _PicBak.nLen;

	BMPFileHeader = (PBITMAPFILEHEADER)szOutPutBuffer;
	pBakData = (PBYTE)(szOutPutBuffer + BMPFileHeader->bfOffBits);
	
	BMPInfoHeader = (PBITMAPINFOHEADER)(szOutPutBuffer + sizeof(BITMAPFILEHEADER));

	nBakBMPWidth	=	BMPInfoHeader->biWidth;
	nBakBMPHeight	=	BMPInfoHeader->biHeight;
	
	if( BMPInfoHeader->biBitCount != BITCOUNT )
		return FALSE;

	//===================================================

	nBakStep	=	4 * ((BITCOUNT * nBakBMPWidth + 31) / 32);
	
	//====================================================	
	//随机选择DISPICCOUNT个干扰图
	for(nDisCount = 0;nDisCount < g_PQConfig.nDisPicCount;nDisCount++)
	{
		srand( nRandomSeed + (nLoopCount + nDisCount) * 3 );
		//随机选择一个干扰图
		PIC& _PicDis = DisturbQueue[ rand() % DisturbQueue.size() ];
		
		BMPFileHeader = (PBITMAPFILEHEADER)_PicDis.szBuffer;
		pDisData = (PBYTE)(_PicDis.szBuffer + BMPFileHeader->bfOffBits);
		
		BMPInfoHeader = (PBITMAPINFOHEADER)(_PicDis.szBuffer + sizeof(BITMAPFILEHEADER));
		
		nDisBMPWidth	=	BMPInfoHeader->biWidth;
		nDisBMPHeight	=	BMPInfoHeader->biHeight;
		
		if( BMPInfoHeader->biBitCount != BITCOUNT )
			return FALSE;
		
		nDisStep	=	4 * ((BITCOUNT * nDisBMPWidth + 31) / 32);			
		//----------------------------------------------------------
		
		srand( (nLoopCount + nDisCount) * 3 );
		nRanX	=	rand();
		srand( (nLoopCount + nDisCount) * 4 );
		nRanY	=	rand();
		srand( (nLoopCount + nDisCount) + 3 );
		nAlpha	=	rand();

		nRanX	%=	nBakBMPWidth;
		nRanY	%=	nBakBMPHeight;
		nAlpha	%=	g_PQConfig.nDisMaxAlpha;

		for( nLoopCountH = 0; nLoopCountH < nDisBMPHeight;nLoopCountH++ )
		{
			if( nLoopCountH + nRanY >= nBakBMPHeight )
				break;
			
			pBakBMPColorTable = (PWORD)( pBakData + (nLoopCountH + nRanY) * nBakStep );

			pDisBMPColorTable = (PWORD)( pDisData + nLoopCountH * nDisStep );

			for( nLoopCountW = 0; nLoopCountW < nDisBMPWidth;nLoopCountW++ )
			{
				if( nLoopCountW + nRanX >= nBakBMPWidth )
					break;
				
				if( pDisBMPColorTable[nLoopCountW] != MASKCOLOR )
				{
					_B._R = (pBakBMPColorTable[nLoopCountW + nRanX] & RMASK) >> 10;
					_B._G = (pBakBMPColorTable[nLoopCountW + nRanX] & GMASK) >> 5;
					_B._B = (pBakBMPColorTable[nLoopCountW + nRanX] & BMASK);

					_S._R = (pDisBMPColorTable[nLoopCountW] & RMASK) >> 10;
					_S._G = (pDisBMPColorTable[nLoopCountW] & GMASK) >> 5;
					_S._B = (pDisBMPColorTable[nLoopCountW] & BMASK);

					_B._R = ( nAlpha * (_S._R - _B._R)/0x1f + _B._R) ;
					_B._G = ( nAlpha * (_S._G - _B._G)/0x1f + _B._G) ;
					_B._B = ( nAlpha * (_S._B - _B._B)/0x1f + _B._B) ;

					pBakBMPColorTable[nLoopCountW + nRanX] = *((PWORD)(&_B));
				}
			}
		}
	}
	//====================================================

	nNumCount = sprintf(szNum,"%d",nNum);
	nNumGridWidth	=	nBakBMPWidth / nNumCount;

	srand( nRandomSeed + nLoopCount * 7 );
	int	nLine	=	rand() % nBakBMPHeight - 6 ;
	if( nLine < 15 )
		nLine += 15 - nLine;

	if( nLine > nBakBMPHeight - 15 )
		nLine -= ( nLine - (nBakBMPHeight - 15) );

	int nWidthCount = 1;
	//遍历每一个字符
	for( nLoopCount = 0; nLoopCount < nNumCount;nLoopCount++ )
	{
		int nIndex = szNum[nLoopCount] - 0x30;

		int nGroupCount = (NumQueue.size() / 10);

		srand( nRandomSeed + nLoopCount * 7 );
		int nTemp = (rand() % nGroupCount) * 10;
		
		PIC& _PicFrt = NumQueue[nIndex + nTemp ];
		
		BMPFileHeader = (PBITMAPFILEHEADER)_PicFrt.szBuffer;
		pFrtData = (PBYTE)(_PicFrt.szBuffer + BMPFileHeader->bfOffBits);
		
		BMPInfoHeader = (PBITMAPINFOHEADER)(_PicFrt.szBuffer + sizeof(BITMAPFILEHEADER));
		
		nFrtBMPWidth	=	BMPInfoHeader->biWidth;
		nFrtBMPHeight	=	BMPInfoHeader->biHeight;
		
		if( BMPInfoHeader->biBitCount != BITCOUNT )
			return FALSE;
		
		if( nFrtBMPWidth > nBakBMPWidth )
			return FALSE;
		
		if( nFrtBMPHeight > nBakBMPHeight )
			return FALSE;
		//===================================================
		nFrtStep	=	4 * ((BITCOUNT * nFrtBMPWidth + 31) / 32);
		//===================================================
		
		int temp;
		srand( nRandomSeed + nLoopCount * 5 );
		nWidthCount +=
		( (temp = ( nNumGridWidth - nFrtBMPWidth > 0 ? nNumGridWidth - nFrtBMPWidth : 0 )) == 0 ? 0 :
		rand() % temp );

		nRanX	=	nWidthCount;

		srand( nRandomSeed + nLoopCount * 4 );
		nRanY	=	rand();
		srand( nRandomSeed + nLoopCount * 10 );
		nAlpha	=	rand();

		nRanX	%=	nBakBMPWidth;
		//nRanY	%=	( (nBakBMPHeight / 2) - nFrtBMPHeight );
		//nRanY	+=	(nBakBMPHeight / 2);
		
		nRanY	%=	5;
		nRanY	+=	(nBakBMPHeight - (nFrtBMPHeight + (nLine - 5)));

		nAlpha	%=	(0x1F - g_PQConfig.nFrtMinAlpha);
		nAlpha	+=	g_PQConfig.nFrtMinAlpha;
		
		for( nLoopCountH = 0; nLoopCountH < nFrtBMPHeight;nLoopCountH++ )
		{
			if( nLoopCountH + nRanY >= nBakBMPHeight )
				break;
			
			pBakBMPColorTable = (PWORD)( pBakData + (nLoopCountH + nRanY) * nBakStep );
			
			pFrtBMPColorTable = (PWORD)( pFrtData + nLoopCountH * nFrtStep );
			
			for( nLoopCountW = 0; nLoopCountW < nFrtBMPWidth;nLoopCountW++ )
			{
				if( nLoopCountW + nRanX >= nBakBMPWidth )
					break;
				
				if( pFrtBMPColorTable[nLoopCountW] != MASKCOLOR )
				{				
					_B._R = (pBakBMPColorTable[nLoopCountW + nRanX] & RMASK) >> 10;
					_B._G = (pBakBMPColorTable[nLoopCountW + nRanX] & GMASK) >> 5;
					_B._B = (pBakBMPColorTable[nLoopCountW + nRanX] & BMASK);
					
					_S._R = (pFrtBMPColorTable[nLoopCountW] & RMASK) >> 10;
					_S._G = (pFrtBMPColorTable[nLoopCountW] & GMASK) >> 5;
					_S._B = (pFrtBMPColorTable[nLoopCountW] & BMASK);
					
					_B._R = ( nAlpha * (_S._R - _B._R)/0x1f + _B._R) ;
					_B._G = ( nAlpha * (_S._G - _B._G)/0x1f + _B._G) ;
					_B._B = ( nAlpha * (_S._B - _B._B)/0x1f + _B._B) ;
					
					pBakBMPColorTable[nLoopCountW + nRanX] = *((PWORD)(&_B));

				}//if( pFrtBMPColorTable[nLoopCountW] != MASKCOLOR )
			}//for( nLoopCountW = 0; nLoopCountW < nFrtBMPWidth;nLoopCountW++ )
		}//for( nLoopCountH = 0; nLoopCountH < nFrtBMPHeight;nLoopCountH++ )
		nWidthCount += nFrtBMPWidth;
	}
	//====================================================
	
		_Question.nAnswer = nNum;

/*
		BMPFileHeader = (PBITMAPFILEHEADER)szOutPutBuffer;
		pBakData = (PBYTE)(szOutPutBuffer + BMPFileHeader->bfOffBits);
		BMPInfoHeader = (PBITMAPINFOHEADER)(szOutPutBuffer + sizeof(BITMAPFILEHEADER));

		BMPInfoHeader->biHeight /= 2;

		//先拷贝BMP头部
		memcpy(
			_Question.szQBuf,
			szOutPutBuffer,
			BMPFileHeader->bfOffBits);

		//再拷贝一半数据
		memcpy(
			_Question.szQBuf + BMPFileHeader->bfOffBits,
			pBakData + (nBakStep * BMPInfoHeader->biHeight),
			(nBakStep * BMPInfoHeader->biHeight));

		_Question.nLen = nBufLen - (nBakStep * BMPInfoHeader->biHeight);
*/
		if( g_PQConfig.nCompress == FALSE )
		{
			_Question.nLen = nBufLen;
			
			memcpy(
				_Question.szQBuf,
				szOutPutBuffer,
				nBufLen);
		}
		else
		{
			unsigned int nCompressLen = 0;

			lzo1x_1_compress(
				(const unsigned char*)szOutPutBuffer,
				nBufLen,
				(unsigned char*)_Question.szQBuf,
				&nCompressLen,
				wrkmem);
			
			_Question.nLen = nCompressLen;
		}
		
		OutQuestionQueue.push_back( _Question );

	return TRUE;
}


int RandomMixCharPic(
	PIC_VECTOR& BackQueue,
	PIC_VECTOR& DisturbQueue,
	PIC_VECTOR& CharQueue,
	QUESTION_VECTOR& OutQuestionQueue,
	int nRandomSeed)
{
#define BITCOUNT	(16)
#define MASKCOLOR	(0x7C1F)
#define DISPICCOUNT (30)

	char szOutPutBuffer[QUESTIONSIZE];
	int	nBufLen = 0;

	char szChar[50];
	int	nCharCount = 0;

	int nLoopCountH		=	0;
	int nLoopCountW		=	0;
	int nDisCount		=	0;
	int	nLoopCount		=	0;
	
	int nBakBMPWidth	=	0;
	int nBakBMPHeight	=	0;

	int nDisBMPWidth	=	0;
	int nDisBMPHeight	=	0;

	int nFrtBMPWidth	=	0;
	int nFrtBMPHeight	=	0;

	int nRanX	=	0;
	int nRanY	=	0;
	int nAlpha	=	0;

	int nBakStep	=	0;
	int nDisStep	=	0;
	int nFrtStep	=	0;
	
	int nNumGridWidth	=	0;

	PBITMAPFILEHEADER BMPFileHeader = NULL;
	PBITMAPINFOHEADER BMPInfoHeader = NULL;
	
	PWORD	pBakBMPColorTable = NULL;
	PWORD	pDisBMPColorTable = NULL;
	PWORD	pFrtBMPColorTable = NULL;
	
	PBYTE	pBakData = NULL;
	PBYTE	pDisData = NULL;
	PBYTE	pFrtData = NULL;

	RGB555	_B	=	{0};
	RGB555	_S	=	{0};

	PIC_QUESTION _Question;


	if( BackQueue.size() == 0 ||
		CharQueue.size() == 0 ||
		DisturbQueue.size() == 0 )
		return FALSE;

	//===========================================================

	//随机选择一个背景
	srand( nRandomSeed );
	PIC& _PicBak = BackQueue[ rand() % BackQueue.size() ];

	memcpy(
		szOutPutBuffer,
		_PicBak.szBuffer,
		_PicBak.nLen);

	nBufLen = _PicBak.nLen;

	BMPFileHeader = (PBITMAPFILEHEADER)szOutPutBuffer;
	pBakData = (PBYTE)(szOutPutBuffer + BMPFileHeader->bfOffBits);
	
	BMPInfoHeader = (PBITMAPINFOHEADER)(szOutPutBuffer + sizeof(BITMAPFILEHEADER));

	nBakBMPWidth	=	BMPInfoHeader->biWidth;
	nBakBMPHeight	=	BMPInfoHeader->biHeight;
	
	if( BMPInfoHeader->biBitCount != BITCOUNT )
		return FALSE;

	//===================================================

	nBakStep	=	4 * ((BITCOUNT * nBakBMPWidth + 31) / 32);
	
	//====================================================	
	//随机选择DISPICCOUNT个干扰图
	for(nDisCount = 0;nDisCount < g_PQConfig.nDisPicCount;nDisCount++)
	{
		srand( nRandomSeed + (nLoopCount + nDisCount) * 3 );
		//随机选择一个干扰图
		PIC& _PicDis = DisturbQueue[ rand() % DisturbQueue.size() ];
		
		BMPFileHeader = (PBITMAPFILEHEADER)_PicDis.szBuffer;
		pDisData = (PBYTE)(_PicDis.szBuffer + BMPFileHeader->bfOffBits);
		
		BMPInfoHeader = (PBITMAPINFOHEADER)(_PicDis.szBuffer + sizeof(BITMAPFILEHEADER));
		
		nDisBMPWidth	=	BMPInfoHeader->biWidth;
		nDisBMPHeight	=	BMPInfoHeader->biHeight;
		
		if( BMPInfoHeader->biBitCount != BITCOUNT )
			return FALSE;
		
		nDisStep	=	4 * ((BITCOUNT * nDisBMPWidth + 31) / 32);			
		//----------------------------------------------------------
		
		srand( (nLoopCount + nDisCount) * 3 );
		nRanX	=	rand();
		srand( (nLoopCount + nDisCount) * 4 );
		nRanY	=	rand();
		srand( (nLoopCount + nDisCount) + 3 );
		nAlpha	=	rand();

		nRanX	%=	nBakBMPWidth;
		nRanY	%=	nBakBMPHeight;
		nAlpha	%=	g_PQConfig.nDisMaxAlpha;

		for( nLoopCountH = 0; nLoopCountH < nDisBMPHeight;nLoopCountH++ )
		{
			if( nLoopCountH + nRanY >= nBakBMPHeight )
				break;
			
			pBakBMPColorTable = (PWORD)( pBakData + (nLoopCountH + nRanY) * nBakStep );

			pDisBMPColorTable = (PWORD)( pDisData + nLoopCountH * nDisStep );

			for( nLoopCountW = 0; nLoopCountW < nDisBMPWidth;nLoopCountW++ )
			{
				if( nLoopCountW + nRanX >= nBakBMPWidth )
					break;
				
				if( pDisBMPColorTable[nLoopCountW] != MASKCOLOR )
				{
					_B._R = (pBakBMPColorTable[nLoopCountW + nRanX] & RMASK) >> 10;
					_B._G = (pBakBMPColorTable[nLoopCountW + nRanX] & GMASK) >> 5;
					_B._B = (pBakBMPColorTable[nLoopCountW + nRanX] & BMASK);

					_S._R = (pDisBMPColorTable[nLoopCountW] & RMASK) >> 10;
					_S._G = (pDisBMPColorTable[nLoopCountW] & GMASK) >> 5;
					_S._B = (pDisBMPColorTable[nLoopCountW] & BMASK);

					_B._R = ( nAlpha * (_S._R - _B._R)/0x1f + _B._R) ;
					_B._G = ( nAlpha * (_S._G - _B._G)/0x1f + _B._G) ;
					_B._B = ( nAlpha * (_S._B - _B._B)/0x1f + _B._B) ;

					pBakBMPColorTable[nLoopCountW + nRanX] = *((PWORD)(&_B));
				}
			}
		}
	}
	//====================================================


	for( nLoopCount = 0; nLoopCount < 4; nLoopCount++ )
	{
		srand( nRandomSeed + nLoopCount * 9 );
		char	cChar	=	rand() % 25 ;
		szChar[nLoopCount] = 0x41 + cChar;
	}

	nCharCount = nLoopCount;

	nNumGridWidth	=	nBakBMPWidth / nCharCount;

	srand( nRandomSeed + nLoopCount * 7 );
	int	nLine	=	rand() % nBakBMPHeight - 6 ;
	if( nLine < 15 )
		nLine += 15 - nLine;

	if( nLine > nBakBMPHeight - 15 )
		nLine -= ( nLine - (nBakBMPHeight - 15) );


	int nWidthCount = 1;
	//遍历每一个字符
	for( nLoopCount = 0; nLoopCount < nCharCount;nLoopCount++ )
	{
		int nIndex = szChar[nLoopCount] - 0x41;

		int nGroupCount = (CharQueue.size() / 26);

		srand( nRandomSeed + nLoopCount * 7 );
		int nTemp = (rand() % nGroupCount) * 26;
		
		PIC& _PicFrt = CharQueue[ nIndex + nTemp ];
		
		BMPFileHeader = (PBITMAPFILEHEADER)_PicFrt.szBuffer;
		pFrtData = (PBYTE)(_PicFrt.szBuffer + BMPFileHeader->bfOffBits);
		
		BMPInfoHeader = (PBITMAPINFOHEADER)(_PicFrt.szBuffer + sizeof(BITMAPFILEHEADER));
		
		nFrtBMPWidth	=	BMPInfoHeader->biWidth;
		nFrtBMPHeight	=	BMPInfoHeader->biHeight;
		
		if( BMPInfoHeader->biBitCount != BITCOUNT )
			return FALSE;
		
		if( nFrtBMPWidth > nBakBMPWidth )
			return FALSE;
		
		if( nFrtBMPHeight > nBakBMPHeight )
			return FALSE;
		//===================================================
		nFrtStep	=	4 * ((BITCOUNT * nFrtBMPWidth + 31) / 32);
		//===================================================
		
		int temp;
		srand( nRandomSeed + nLoopCount * 5 );
		nWidthCount +=
		( (temp = ( nNumGridWidth - nFrtBMPWidth > 0 ? nNumGridWidth - nFrtBMPWidth : 0 )) == 0 ? 1 :
		rand() % temp );

		nRanX	=	nWidthCount;

		srand( nRandomSeed + nLoopCount * 4 );
		nRanY	=	rand();
		srand( nRandomSeed + nLoopCount * 10 );
		nAlpha	=	rand();

		nRanX	%=	nBakBMPWidth;
		//nRanY	%=	( (nBakBMPHeight / 2) - nFrtBMPHeight );
		//nRanY	+=	(nBakBMPHeight / 2);
		
		nRanY	%=	5;
		nRanY	+=	(nBakBMPHeight - (nFrtBMPHeight + (nLine - 5)));

		nAlpha	%=	(0x1F - g_PQConfig.nFrtMinAlpha);
		nAlpha	+=	g_PQConfig.nFrtMinAlpha;
		
		for( nLoopCountH = 0; nLoopCountH < nFrtBMPHeight;nLoopCountH++ )
		{
			if( nLoopCountH + nRanY >= nBakBMPHeight )
				break;
			
			pBakBMPColorTable = (PWORD)( pBakData + (nLoopCountH + nRanY) * nBakStep );
			
			pFrtBMPColorTable = (PWORD)( pFrtData + nLoopCountH * nFrtStep );
			
			for( nLoopCountW = 0; nLoopCountW < nFrtBMPWidth;nLoopCountW++ )
			{
				if( nLoopCountW + nRanX >= nBakBMPWidth )
					break;
				
				if( pFrtBMPColorTable[nLoopCountW] != MASKCOLOR )
				{				
					_B._R = (pBakBMPColorTable[nLoopCountW + nRanX] & RMASK) >> 10;
					_B._G = (pBakBMPColorTable[nLoopCountW + nRanX] & GMASK) >> 5;
					_B._B = (pBakBMPColorTable[nLoopCountW + nRanX] & BMASK);
					
					_S._R = (pFrtBMPColorTable[nLoopCountW] & RMASK) >> 10;
					_S._G = (pFrtBMPColorTable[nLoopCountW] & GMASK) >> 5;
					_S._B = (pFrtBMPColorTable[nLoopCountW] & BMASK);
					
					_B._R = ( nAlpha * (_S._R - _B._R)/0x1f + _B._R) ;
					_B._G = ( nAlpha * (_S._G - _B._G)/0x1f + _B._G) ;
					_B._B = ( nAlpha * (_S._B - _B._B)/0x1f + _B._B) ;
					
					pBakBMPColorTable[nLoopCountW + nRanX] = *((PWORD)(&_B));

				}//if( pFrtBMPColorTable[nLoopCountW] != MASKCOLOR )
			}//for( nLoopCountW = 0; nLoopCountW < nFrtBMPWidth;nLoopCountW++ )
		}//for( nLoopCountH = 0; nLoopCountH < nFrtBMPHeight;nLoopCountH++ )
		nWidthCount += nFrtBMPWidth;
	}
	//====================================================
	
		_Question.nAnswer = *((unsigned int*)szChar);

		if( g_PQConfig.nCompress == FALSE )
		{
			_Question.nLen = nBufLen;
			
			memcpy(
				_Question.szQBuf,
				szOutPutBuffer,
				nBufLen);
		}
		else
		{
			unsigned int nCompressLen = 0;

			lzo1x_1_compress(
				(const unsigned char*)szOutPutBuffer,
				nBufLen,
				(unsigned char*)_Question.szQBuf,
				&nCompressLen,
				wrkmem);
			
			_Question.nLen = nCompressLen;
		}
		
		OutQuestionQueue.push_back( _Question );

	return TRUE;
}
//======================================================
#define QUESTIONCONFIG	"\\settings\\picquestion\\config.ini"
#define BASESECT		"base"
#define KEYSENDBYTE		"sendbytesec"
#define KEYPICANSLIMIT	"picanswerlimit"
#define KEYPICMAXERR	"picmaxerror"
#define KEYNUMINTER		"numinterval"
#define KEYSWITCH		"switch"
#define KEYMAXKILLNPC	"maxkillnpc"
#define KEYDISPICCOUNT	"dispicncount"
#define KEYDISALPHA		"disalpha"
#define KEYFRTALPHA		"frtalpha"
#define KEYPICCOUNT		"picgroupcount"
#define KEYNUMCOUNT		"numgroupcount"
#define KEYNUMEND		"numend"
#define KEYCOMPRESS		"bcompress"
#define MAPSEC			"noquesmap"
#define KEYWORLD		"World"
#define KEYMAPCOUNT		"MapCount"

void g_InitPicQuestion( )
{
	KIniFile	_PQConfig;
	int			nValue;
	char		szNum[50];

	if( _PQConfig.Load( QUESTIONCONFIG ) )
	{
		//=============================================================
		_PQConfig.GetInteger(
			BASESECT,
			KEYSWITCH,
			0,
			&nValue);
		g_PQConfig.nSwitch = nValue;
		//==============================

		if( g_PQConfig.nSwitch == TRUE )
		{
			_PQConfig.GetInteger(
						BASESECT,
						KEYSENDBYTE,
						0,
						&nValue);
			g_PQConfig.nSendByteSec = nValue;
			//==============================

			_PQConfig.GetInteger(
						BASESECT,
						KEYPICANSLIMIT,
						5,
						&nValue);
			g_PQConfig.nPicQuesAnswerLimit = nValue;
			//==============================

			_PQConfig.GetInteger(
						BASESECT,
						KEYPICMAXERR,
						2,
						&nValue);
			g_PQConfig.nPicMaxError = nValue;
			//==============================

			_PQConfig.GetInteger(
						BASESECT,
						KEYNUMINTER,
						5,
						&nValue);
			g_PQConfig.nNumInterval = nValue;
			//==============================

			_PQConfig.GetInteger(
						BASESECT,
						KEYDISPICCOUNT,
						5,
						&nValue);
			g_PQConfig.nDisPicCount = nValue;
			//==============================

			_PQConfig.GetInteger(
						BASESECT,
						KEYDISALPHA,
						10,
						&nValue);
			g_PQConfig.nDisMaxAlpha = nValue;
			//==============================
			
			_PQConfig.GetInteger(
						BASESECT,
						KEYFRTALPHA,
						20,
						&nValue);
			g_PQConfig.nFrtMinAlpha = nValue;
			//==============================
			
			_PQConfig.GetInteger(
						BASESECT,
						KEYPICCOUNT,
						20,
						&nValue);
			g_PQConfig.nPicQuesCount = nValue;
			//==============================
			
			_PQConfig.GetInteger(
						BASESECT,
						KEYNUMCOUNT,
						200,
						&nValue);
			g_PQConfig.nNumQuesCount = nValue;
			//==============================
			
			_PQConfig.GetInteger(
						BASESECT,
						KEYNUMEND,
						999999,
						&nValue);
			g_PQConfig.nNumQuesEnd = nValue;

			//==============================
			
			_PQConfig.GetInteger(
				BASESECT,
				KEYCOMPRESS,
				1,
				&nValue);
			g_PQConfig.nCompress = nValue;
			//=============================================================
			g_KillNpcConfig.resize(MAX_LEVEL);
			int nLoopCount = 0;
			for( nLoopCount = 0;nLoopCount < MAX_LEVEL;nLoopCount++ )
			{
				sprintf(szNum,"%d",nLoopCount);
				_PQConfig.GetInteger(
						szNum,
						KEYMAXKILLNPC,
						0xFFFFFFFF,
						&nValue);

				g_KillNpcConfig[nLoopCount] = nValue;

				if( nValue > 0 )
				{
					for( int nLoopCountJ = nLoopCount - 1;nLoopCountJ > 0;nLoopCountJ-- )
					{
						if(g_KillNpcConfig[nLoopCountJ] == 0xFFFFFFFF)
							g_KillNpcConfig[nLoopCountJ] = nValue;
						else
							break;
					}
				}

			}//for

			int		nMapCount = 0;
			int		nMapID	=	0;
			char	szNumber [MAXMAPCOUNT];

			_PQConfig.GetInteger(MAPSEC,KEYMAPCOUNT,0,&nMapCount);

			for(nLoopCount = 0;nLoopCount < nMapCount;nLoopCount++)
			{
				sprintf(szNumber, KEYWORLD"%02d", nLoopCount);
				_PQConfig.GetInteger(MAPSEC,szNumber,0,&nMapID);
				g_PQConfig.nNoQMap[nLoopCount] = nMapID;
			}
			
		}//switch

	}
	//=============================

#ifdef WIN32 
	#define TEMPOUTPUT	"settings\\picquestion\\final\\"
	#define CHARNAME	"settings\\picquestion\\char"
	#define BACKNAME	"settings\\picquestion\\back"
	#define DISTURB		"settings\\picquestion\\disturb"
	#define NUMPIC		"settings\\picquestion\\num"
#else
	#define TEMPOUTPUT	"settings/picquestion/final/"
	#define CHARNAME	"settings/picquestion/char"
	#define BACKNAME	"settings/picquestion/back"
	#define DISTURB		"settings/picquestion/disturb"
	#define NUMPIC		"settings/picquestion/num"
#endif
	

	
	PIC_VECTOR	CharQueue;
	PIC_VECTOR	BackQueue;
	PIC_VECTOR	DisturbQueue;
	PIC_VECTOR	NumQueue;
	int			nLoopCount = 0;
	
	if( g_PQConfig.nSwitch == TRUE )
	{
		LoadQuestionPic( CharQueue,CHARNAME );
		LoadQuestionPic( BackQueue,BACKNAME );
		LoadQuestionPic( DisturbQueue,DISTURB );
		LoadQuestionPic( NumQueue, NUMPIC );

		//字符问题
		for(nLoopCount = 0;nLoopCount < g_PQConfig.nPicQuesCount;nLoopCount++)
		{
			RandomMixCharPic(
				BackQueue,
				DisturbQueue,
				CharQueue,
				g_PicQuestion,
				UNIX_TMIE_STAMP + nLoopCount + 2);
		}

		//数字问题
		srand(time(NULL) + 2);

		for( nLoopCount = 0;nLoopCount < g_PQConfig.nNumQuesCount;nLoopCount++ )
		{
			RandomMixNumPic(
				BackQueue,
				DisturbQueue,
				NumQueue,
				( ( rand() * 100 ) + NumQueue.size() ) % g_PQConfig.nNumQuesEnd ,
				g_NumQuestion,
				UNIX_TMIE_STAMP + nLoopCount + 2);
		}

		//debug
	
		char szPath[MAX_PATH];
		for(nLoopCount = 0;nLoopCount < g_PicQuestion.size();nLoopCount++)
		{
			sprintf(szPath,"%sFinal%02d.bmp",TEMPOUTPUT,nLoopCount);
			
			PIC_QUESTION& _Question = g_PicQuestion[nLoopCount];
			
			FILE* file = fopen( szPath, "wb" );
			if( file != 0 )
			{
				fwrite( _Question.szQBuf, 1, _Question.nLen, file );
				fclose(file);
				file = NULL;
			}
		}

		int nTemp = nLoopCount;
		for(nLoopCount = 0;nLoopCount < g_NumQuestion.size();nLoopCount++)
		{
			sprintf(szPath,"%sFinal%02d.bmp",TEMPOUTPUT,nTemp + nLoopCount);
			
			PIC_QUESTION& _Question = g_NumQuestion[nLoopCount];
			
			FILE* file = fopen( szPath, "wb" );
			if( file != 0 )
			{
				fwrite( _Question.szQBuf, 1, _Question.nLen, file );
				fclose(file);
				file = NULL;
			}
		}
		//

		CharQueue.clear();
		BackQueue.clear();
		DisturbQueue.clear();
		NumQueue.clear();

	}
}

#endif

//==================================================

#define PATHNAME_AUTOSCRIPT	"\\script\\startload.lua"
BOOL DoAutoexecScript(LPSTR pScriptCommand)
{
	if(NULL == pScriptCommand)
	{
		return FALSE;
	}

	KLuaScript *pLoadScript = new KLuaScript;
	if(NULL == pLoadScript)
	{
		return FALSE;
	}

	pLoadScript->Init();
	pLoadScript->RegisterFunctions(GameScriptFuns, g_GetGameScriptFunNum());
		
	BOOL bRet = FALSE;
	if(pLoadScript->Load(PATHNAME_AUTOSCRIPT) && 
		pLoadScript->LoadBuffer((PBYTE)pScriptCommand, strlen(pScriptCommand))) 
	{
		bRet = pLoadScript->ExecuteCode();
	}

	delete pLoadScript;

	return bRet;
}

extern UINT g_nNewRandomSeed;

#include "buff_tab.h"
#include "buff_man.h"
#include "ArmorSet_Table.h"
#include "Yao_Table.h"
#include "Yao_AddOnTable.h"
#include "Abrade_Table.h"
#include "ConfigManager.h"
#include "relation_template.h"
#include "talisman_manager.h"
#include "title.h"

#ifdef _SERVER
#include "exp_manager.h"
#endif

void g_FSEyeInitCore( )
{
	g_ItemGen.Init( );
}

const KBASICPROP_ITEM* g_GetItemTemplate(
	IN int nGenre,
	IN int nDetailType,
	IN int nParticularType,
	IN int nLevel )
{
	return g_ItemGen.GetItemTemplate( nGenre, nDetailType, nParticularType, nLevel );
}

//TaisuiWheel sys init & Release Add By Brianyao2007
void InitTaisuiWheel()
{
#ifdef _SERVER
	KTaisuiWheelServer::Singleton().Init();
#endif
}

void ReleaseTaisuiWheel()
{
#ifdef _SERVER
	KTaisuiWheelServer::Singleton().Release();
#endif
}

/*
#ifdef _SERVER
KPakList g_pakList;
#endif//*/

void g_InitCore()
{
	/*
#ifdef _SERVER
	g_SetRootPath( NULL );
	g_SetFilePath( DEFAULT_ROOT_DIR );
	if ( !g_pakList.Open( PACKAGE_INI ) )
	{
		_ASSERT(FALSE);
	}
#endif
	//*/

	//测试装备存盘最大ｂｕｆｆ容量
	//int nLen = (sizeof(TDBItemData_Version_1) + ((MAX_PLAYER_ITEM - 1) * sizeof(_TDBItemData_Version_1))) / 1024;

	/* 现改为从配置文件中读取时区修正
	//时区修正
	tm localtm, utctm;

#ifdef _SERVER
	time_t ucttime = UNIX_TMIE_STAMP;
#else
	time_t ucttime = time(NULL);
#endif

	memcpy(&utctm, gmtime( &ucttime ), sizeof(tm)); 
	memcpy(&localtm, localtime( &ucttime ), sizeof(tm)); 
	int nDay = localtm.tm_mday - utctm.tm_mday;
	int nHour = localtm.tm_hour - utctm.tm_hour;
	int nMin = localtm.tm_min - utctm.tm_min;
	nMin = ( nDay * 24 * 60 ) + ( nHour * 60 ) + nMin;
	TIMEZONE_CORRECT = nMin * 60;
	*/

	//载入配置管理器
	bool loadConfigManagerSuccess = ConfigManager::Singleton().Load();
	_ASSERT(loadConfigManagerSuccess);

	int timeZoneCorrectHour = ConfigManager::Singleton().GetGlobalVariable(global_var_timezone_correct_hour);
	if (timeZoneCorrectHour <= 0)
		timeZoneCorrectHour = 8;
	TIMEZONE_CORRECT = 3600 * timeZoneCorrectHour;

	//载入套装配置
	bool loadArmorSetTableSuccess = ArmorSetTable::Singleton().Load();
	_ASSERT(loadArmorSetTableSuccess);

	//载入爻装配置
 	bool loadYaoTableSuccess = YaoTable::Singleton().Load();
	_ASSERT(loadYaoTableSuccess);

	//载入爻装附加属性配置表
	bool loadYaoAddOnTableSuccess = YaoAddOnTable::Singleton().Load();
	_ASSERT(loadYaoAddOnTableSuccess);	

	//载入装备磨损配置表
	bool loadAbradeTableSuccess = AbradeTable::Singleton().Load();
	_ASSERT(loadAbradeTableSuccess);

	//载入升级信息
	bool loadLevelUpInfoSuccess = KLevelUpInfo::Singleton().Load();
	_ASSERT(loadLevelUpInfoSuccess);

	//载入关系模版
	bool loadRelationTemplateSuccess = RTM::Singleton().Load(RELATION_TEMPLATE_CFG_FILE);
	_ASSERT(loadRelationTemplateSuccess);

	//载入附加积分模板
	bool loadPlusPointTableSuccess = PlusPointTable::Singleton().Load(PLUS_POINT_SETTINGS);
	_ASSERT(loadPlusPointTableSuccess);

	//载入BUFF表
	BuffTable::Singleton().Load( );

	//载入法宝配置
	TalismanManager::Singleton().Load();

	KOnceIBItemMgr::GetSingleten().LoadConfig();

	//载入称号配置
	TitleManager::LoadSettings();

#ifdef _SERVER
	//初始化BUFF管理器
	BuffMgr& BMgr = BuffMgr::Singleton();	
	if( !BMgr.Init( ) )
		return;

	//载入经验管理器
	bool initExpManagerSuccess = ExpManager::Singleton().Init();
	_ASSERT(initExpManagerSuccess);
	
	//创建日志系统对象
	BOOL createLogSystemResult = CreateLogSystem(g_pLogSystem);
	_ASSERT(TRUE == createLogSystemResult);

	//玩家监视器
	bool loadPlayerMonitorSettingSuccess = g_PlayerMonitor.Load();
	_ASSERT(loadPlayerMonitorSettingSuccess);

	//载入雇用中心配置
	if ( !EmployCenter::Singleton().Load( ) )
		return;

	//推荐人系统配置载入
	RecommenderSystem::Singleton().Load();
	
	KWorldCombatSetting::Singleton().Init();

	//经济系统初始化
	KEconomySysManager::Singleton().InitEconomySys();
	KPlayer::ms_uCurSystemTime = time( NULL );
#else
	BOOL bBT = KNpcRes::InitBloodTemplate();
	_ASSERT( TRUE == bBT );

	//法宝NPC表
	bool loadTalismanNpcTableSuccess = ClientTalismanNpcTable::Singleton().Load();	
#endif
	//太岁之轮
	InitTaisuiWheel();

	KExpQuestInsuraceSetting::Singleton().Load();

	InsuranceSettingMgr::Singleton().Load(INSURANCE_SETTINGS_PATH);

#ifdef _SERVER
	KSocialRecruitMgr::Singlton().Init();
#endif	
	time_t ttt;
	srand(time(&ttt));
	char *szRandomMem1, *szRandomMem2, *szRandomMem3;	//这些变量没有应用价值，主要是防外挂找固定内存
	szRandomMem1 = new char[((rand() % 64) + 6) * 1024];
	Player = new KPlayer[MAX_PLAYER];
	szRandomMem2 = new char[((rand() % 64) + 6) * 1024];
	Item = new KItem[MAX_ITEM];
	szRandomMem3 = new char[((rand() % 64) + 6) * 1024];
	Npc = new KNpc[MAX_NPC];
	delete szRandomMem1;	szRandomMem1 = NULL;
	delete szRandomMem2;	szRandomMem2 = NULL;
	delete szRandomMem3;	szRandomMem3 = NULL;
    int i = 0;

	g_InitProtocol();

#ifndef __linux
	g_nNewRandomSeed = GetTickCount();
#else
	g_nNewRandomSeed = time(NULL);
#endif


#ifndef __linux
	g_RandomSeed(GetTickCount());
#else
	g_RandomSeed(times(NULL));
#endif

	srand( (unsigned)time( NULL ) );
	time_t ltime;
	time( &ltime );
	g_DebugLog("Starting Core......%s", ctime( &ltime ));
	
#ifndef _SERVER
	g_bPingReply = TRUE;
	g_SoundCache.Init(256);
	g_SubWorldSet.m_cMusic.Init();
#endif

    if (!g_InitMath())
    {
    	g_DebugLog("[Math]g_InitMath() error !");
    }
#ifdef _SERVER
	GetGlobalWarInfoManager().Initialize();
	GetPoolCombatInfoManager().Initialize();
#endif

	g_ItemChangeRes.Init();

#ifndef _SERVER
	g_ItemPalToHue.Init();	// 配色信息
#endif

	//
	ItemSet.Init();
	//
	g_ItemGen.Init();

	NpcSet.Init();

	ObjSet.Init();

#ifndef _SERVER
	// 通用动画初始化
	ScreenEffectTab::Singleton().LoadScreenEffect();
	ScreenEffectMgr::Singleton().Load();
#endif

	g_IniScriptEngine();
	g_OrdinSkillsSetting.Load(SKILL_SETTING_FILE);

	g_NpcSetting.Load(NPC_SETTING_FILE);
	g_DebugLog("[Script]script size %d", sizeof(g_ScriptSet));

#ifdef _SERVER
	PlayerCreator::Singleton( ).Init( );
#endif

#ifndef _SERVER
	bool bInitAuction = Player[CLIENT_PLAYER_INDEX].m_clientAucMgr.Init();
	_ASSERT(bInitAuction);
	
	bool bInitSocial = Player[CLIENT_PLAYER_INDEX].m_clientSUMgr.Init();
	_ASSERT(bInitSocial);
#endif

	
	InitSkillSetting();
#ifndef _SERVER
	MissleSet.Init();
	if (!g_StringResourseTabFile.Load(STRINGRESOURSE_TABFILE))
	{
		g_DebugLog("[TASK]CAN NOT LOAD %s", STRINGRESOURSE_TABFILE);
	}
#endif

	InitNpcSetting();

//	InitTaskSetting();

	// 这个涉及到与技能相关的东西，所以必须放在技能初始化之后

	if (!PlayerSet.Init())
	{
		CFS_FILELOGS::WriteDebugLog("Init PlayerSet Error!!!\n");
	}

	PolyMorphSettings.Init();

#ifdef _SERVER
	memset(g_TaskGlobalValue, 0, sizeof(g_TaskGlobalValue));
	g_TeamSet.Init();

	if (g_pController)
		g_GuidPadding = g_pController->GetServerID();

	StatueInfoMgr::Singleton().InitStatueUsedInfo();


	LoadInstanceGlobalData();
	g_SubWorldSet.Load("\\maps\\WorldSet.ini");

	GetGlobalTongWarMgr().InitWarMapSetting();
	GetGlobalPoolCombatMgr().InitWarMapSetting();


	// 必须放到subworld加载之后
	NpcSave::LoadGlobalNpcInfo();

	g_WorldScript.Init();
	g_WorldScript.RegisterFunctions(WorldScriptFuns, g_GetWorldScriptFunNum());
#endif
	
#ifndef _SERVER
	g_SubWorldSet.LoadMapList();
	g_ScenePlace.Initialize();
#endif
	time(&ltime);

	if (!BuySell.Init())
	{
		g_DebugLog("Buysell init failed!");
	}

	if( !KSmithShop::getSinglton().init())
	{
		g_DebugLog("smithshop init failed!");
	}

	if (!g_CompoundRule.Init())
	{
		g_DebugLog("CompoundRule init failed!");
	}

#ifdef _SERVER
	KIniFile aIniFile;
	if (aIniFile.Load("\\settings\\expsharemode.ini"))
	{
		aIniFile.GetInteger("classic", "distance", 1024, (int*)&g_uShareExpDistance[0]);
		aIniFile.GetInteger("average", "distance", 1024, (int*)&g_uShareExpDistance[1]);
	}
#endif
	//////////////////////////////////////////////////////////////////////////

#ifdef _SERVER
	if( !g_ChatCenterS.Init() )
	{
		CFS_FILELOGS::WriteDebugLog("Chat Center Init failed!\n");
	}
#else
	g_ChatCenterC.Init();
#endif
	
	//加载IB商店数据
#ifdef _SERVER
	IBCenter_S::Singleton().Init();
#endif

	//加载问答配置
#ifdef _SERVER
	QuestionManager::Singleton().LoadFonts();//载入字体
	QuestionManager::Singleton().ReloadAllSettings();//载入所有配置
#endif

	if( !LoadTextFilterExp(TEXT_FILTER_EXP_FILE) )
		_ASSERT(false);

	if (!LoadChatTxtFilterExp(CHAT_TEXT_FILTER_FILE))
		_ASSERT(false);

#ifndef _SERVER
	if (!LoadChatRecvFilterExp(CHAT_RECV_TEXT_FILTER_FILE))
	{
		_ASSERT(false);
	}
	KIniFile userChatFilterfile;
	if(!userChatFilterfile.Load(USER_CHAT_TEXT_FILTER_FILE))
	{
		KIniFile file;
		file.Load(CHAT_RECV_TEXT_FILTER_FILE);
		file.Save(USER_CHAT_TEXT_FILTER_FILE);
		file.Clear();
	}
	userChatFilterfile.Clear();
	if (!LoadUserChatFilterExp(USER_CHAT_TEXT_FILTER_FILE))
	{
		_ASSERT(false);
	}
#endif

#ifndef _SERVER
	InitCombatInfoResources();
#endif

	//SpecialSkillTab::Singleton().LoadSpecialSkill();

	KItemInlayRule& iir = KItemInlayRule::Singleton();
	iir.Load();

	KItemInlayAddOnTable& iiaddon = KItemInlayAddOnTable::Singleton();
	iiaddon.Load();

#ifndef _SERVER
	g_TeamViewer.LoadTeamIcon(TEAM_ICON_FILE);
#endif


	/*
	fstream ffffff;
	ffffff.open("c:\\height.txt");
	int nNpcHeight = g_NpcSetting.GetHeight();
	for ( int nNpcTemplateIdx = 0; nNpcTemplateIdx < nNpcHeight; ++nNpcTemplateIdx )
	{
		KNpc lucifer;
		lucifer.Load( nNpcTemplateIdx, 1 );
		std::string sprPath;
		lucifer.m_DataRes.GetSpr(sprPath);
		int nMaxHeight = 0;

		KSprite spr;
		
		if ( spr.Load((char*)sprPath.c_str()) )
		{
			int nSprFrames = spr.GetFrames();
			int nCy = spr.GetCenterY() > 0 ? spr.GetCenterY() : 312;
					
			for ( int nSprFramesIdx = 0; nSprFramesIdx < nSprFrames; ++nSprFramesIdx  )
			{
				SPRFRAME *pSprFrame = spr.GetFrameInfo( nSprFramesIdx );	
				if ( pSprFrame )
				{
					int newHeight = nCy - pSprFrame->OffsetY;
					if ( newHeight > nMaxHeight )
					{
						nMaxHeight = newHeight;
					}					
				}
			}		
		}
		char szbuff[1024];
		if ( nMaxHeight > 0 )
		{
			sprintf(szbuff,"%s	%d",lucifer.Name, nMaxHeight + 10 );
			ffffff<<szbuff<<endl;
		}
		else
		{
			sprintf(szbuff,"%s	%d",lucifer.Name, lucifer.m_nStature);
			ffffff<<szbuff<<endl;
		}
		
	}
	ffffff.close();//*/
}




//BOOL	InitTaskSetting()
//{
//#ifdef _SERVER
//	// lixuewu 使用统一的版本控制 2004.07.15
//     char    szTABFilePath[FILE_NAME_LENGTH];
//     sprintf(szTABFilePath, TABFILE_PATH"\\"QUESTITEM_TABFILE);
//	
// 	if (!g_TimerTask.Init())
// 	{
// 		g_DebugLog("Timer Task Init Erroorororo!");
// 	}
//
//
// 	if (!g_MissionTabFile.Load(TASK_MISSION_SETTING_TABFILE))
// 	{
// 		g_DebugLog("[error]Can Not Open %s", TASK_MISSION_SETTING_TABFILE);
// 	}
//
//	g_nCurrentTimeTaskIndex = 0;
//	g_ulNextSytemTimeTaskFrameTime = 0x0fffff;
// 	if (!g_SystemTimeTaskFile.Load(SYSTEMTIMETASK_TABFILE))
// 	{
// 		g_DebugLog("[error]Can Not Open %s", SYSTEMTIMETASK_TABFILE);
// 	}
//
// 	int nSysTaskCount = g_SystemTimeTaskFile.GetHeight() - 1 ;
// 	if (nSysTaskCount <= 0)
// 	{
// 		g_ulNextSytemTimeTaskFrameTime = 0x8fffffff;
// 	}
//	
// 	int nNextHour = 0;
// 	int nNextMin = 0;
//
// 	struct tm *curtime;
// 	time_t long_time;
// 	time( &long_time );                /* Get time as long integer. */
// 	curtime = localtime( &long_time ); /* Convert to local time. */
// 	
// 	int nCurHour = curtime->tm_hour;
// 	int nCurMin = curtime->tm_min;
// 	int nDMin = 0x0fffffff;
// 	int nTaskIndex = 0;
// 	
// 	for (int m = 0; m < nSysTaskCount; m ++)
// 	{
// 		g_SystemTimeTaskFile.GetInteger(m + 2 , "HOUR", 0, &nNextHour);
// 		g_SystemTimeTaskFile.GetInteger(m + 2 , "MIN", 0, &nNextMin);
// 		int nDMin1 = GetIntervalTime(nCurHour, nCurMin, nNextHour, nNextMin);
// 		
// 		if(nDMin1 >= 0 && nDMin1 < nDMin)
// 		{
// 			nTaskIndex = m + 2;
// 			nDMin = nDMin1;
// 			g_nCurrentTimeTaskIndex = nTaskIndex;
// 			g_ulNextSytemTimeTaskFrameTime = nDMin * 60 * GAME_FPS;
// 		}
// 	}
// 	CFS_FILELOGS::WriteDebugLog("Next TimeTask is %d, Interval is %d\n", g_nCurrentTimeTaskIndex, g_ulNextSytemTimeTaskFrameTime / (60 * GAME_FPS));
//
//#endif
//
//#ifndef _SERVER
///*
//	if (!g_RankTabSetting.Load(PLAYER_RANK_SETTING_TABFILE))
//	{
//		g_DebugLog("[TASK]CAN NOT LOAD %s", PLAYER_RANK_SETTING_TABFILE);
//	}//*/
//#endif
//
//	return TRUE;
//}

BOOL	InitNpcSetting()
{
	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	
	g_DebugLog("npc template count %d", nNpcTemplateNum);
	memset(g_pNpcTemplate, 0, sizeof(void*) * MAX_NPCSTYLE * MAX_NPC_LEVEL);

	if ( !g_NpcKindFile.Load(NPC_RES_KIND_FILE_NAME) )
	{
		g_DebugLog("open file %s error!!!", NPC_RES_KIND_FILE_NAME);
	}
	
	g_DebugLog("NpcTempleSize is %d * %d * %d = %d", sizeof(KNpcTemplate) , nNpcTemplateNum , MAX_NPC_LEVEL, sizeof(KNpcTemplate) * nNpcTemplateNum * MAX_NPC_LEVEL);
	
	//加载Npc等级设定的脚本文件，用于今后加载Npc时使用
#ifdef _SERVER
	g_pNpcLevelScript = (KLuaScript*)g_GetScript(NPC_LEVELSCRIPT_FILENAME);
#else
	g_pNpcLevelScript = new KLuaScript;
	g_pNpcLevelScript->Init();
	if (!g_pNpcLevelScript->Load(NPC_LEVELSCRIPT_FILENAME))
	{
		g_DebugLog ("[error]read file %s error", NPC_LEVELSCRIPT_FILENAME);
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}
#endif
	
	if (!g_pNpcLevelScript) 
	{
		g_DebugLog("%sNpc level script load error ", NPC_LEVELSCRIPT_FILENAME);
	}

#ifndef _SERVER
	g_NpcResList.Init();
#endif
	return TRUE;
}

BOOL	InitSkillSetting()
{
	if (!g_SkillManager.Init())
	{
		_ASSERT(0);
	}

	return TRUE;
}
#ifdef _SERVER
BOOL	LoadNpcSettingFromBinFile(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}

BOOL	SaveAsBinFileFromNpcSetting(LPSTR BinFile = NPC_TEMPLATE_BINFILE)
{
	return FALSE;
}
#endif

//---------------------------------------------------------------------------

void g_ReleaseCore()
{

	int nNpcTemplateNum = g_NpcSetting.GetHeight() - 1;
	unsigned long i = 0;
	unsigned long j = 0;
	
	for (i = 0; (int)i < nNpcTemplateNum; i++)
	{
		for (j = 0; j < MAX_NPC_LEVEL; j++)
		{
			if (g_pNpcTemplate[i][j])
			{
				delete ((KNpcTemplate *)g_pNpcTemplate[i][j]);
				g_pNpcTemplate[i][j] = NULL;
			}
		}
	}

#ifndef _SERVER
	g_SubWorldSet.Close();
	g_ScenePlace.ClosePlace();
	if (g_pNpcLevelScript)
	{
		delete g_pNpcLevelScript;
		g_pNpcLevelScript = NULL;
	}
	if (g_pAdjustColorTab)
	{
		delete []g_pAdjustColorTab;
		g_pAdjustColorTab = NULL;
		g_ulAdjustColorCount = 0;
	}
#endif

    g_UnInitMath();
	delete[] Item;
	delete[] Player;
	delete[] Npc;

	PolyMorphSettings.Release();

	ReleaseTaisuiWheel();
#ifdef _SERVER
	KSocialRecruitMgr::Singlton().Release();
#endif

#ifdef _SERVER
	ReleaseLogSystem(g_pLogSystem);
#endif

	/*
#ifdef _SERVER
	g_pakList.Close();
#endif
	//*/
}

#ifdef _SERVER

InstanceGlobalData g_InstanceGlobalData;

DWORD NewInstanceId()
{
	DWORD& instanceId = g_InstanceGlobalData.dwInstanceId;

	instanceId++;
	if (instanceId == 0)
		instanceId = 1;

	SaveInstanceGlobalData();

	return instanceId;
}

void g_SetServer(LPVOID pServer)
{
	g_pServer = reinterpret_cast< IServer * >(pServer);
}

IController* g_pController = NULL;
void SetController( LPVOID pController )
{
	g_pController = (IController*)pController;
}

_GlobalTask	g_GlobalTask = {0};

int GetTaskGlobal( int nIndex )
{
	if( nIndex < 0 || nIndex > MAX_TASK_VALUE_COUNT )
		return 0;

	return g_GlobalTask.nTaskValue[nIndex];
}

void SetTaskGlobal( int nIndex, int nValue )
{
	if( nIndex < 0 || nIndex > MAX_TASK_VALUE_COUNT )
		return;

	g_GlobalTask.nTaskValue[nIndex] = nValue;

	SaveTaskGlobal( );
}

void LoadTaskGlobal( )
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );

	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_task;

	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_GETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void SaveTaskGlobal( )
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );

	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_task;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	pParam->Push( BinPair( (void*)&g_GlobalTask, sizeof(g_GlobalTask) ) );
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

int GlobalDataProcess( 
	IProcRet* pRet )
{
	int nRetSize = 0;
	char* pPassBy = NULL;
	char* pRetBuf = NULL;

	pPassBy = pRet->GetPassBy( nRetSize );

	if( pPassBy != NULL && 
		nRetSize >= sizeof(_DBProcHeader) )
	{
		_DBProcHeader* DBHeader = (_DBProcHeader*)pPassBy;
		
		switch( DBHeader->ProcType )
		{
		case Proc_GetGlobal:
			{
				if( nRetSize != sizeof(_GlobalHeader) )
					return FALSE;

				_GlobalHeader* GHeader = (_GlobalHeader*)pPassBy;

				nRetSize = pRet->GetData( 0, 0, &pRetBuf );

				switch( GHeader->nGlobalID )
				{
				case global_data_task:
					{
						if( pRet->GetExeRet( ) && 
							pRet->GetRet( ) && 
							nRetSize == sizeof(g_GlobalTask) )
						{
							g_GlobalTask = *((_GlobalTask*)pRetBuf);
						}
					}
					break;

				case global_data_taisui:
					{
						if( pRet->GetExeRet( ) && 
							pRet->GetRet( ) )
						{
							KTaisuiWheelServer::ProcessGlobalDBTaisui(pRet->GetExeRet( ) && pRet->GetRet( ),nRetSize,(unsigned char *)pRetBuf);
						}//endif
					}
					break;

				case global_data_npcsave:
					{
						NpcSave::LoadGlobalNpcInfoRet(
							pRet->GetRet( ) && pRet->GetExeRet( ),
							nRetSize, 
							pRetBuf );

						GetGlobalTongWarMgr().OnNpcSaveLoadComplete();
						GetGlobalPoolCombatMgr().OnNpcSaveLoadComplete();
						StatueInfoMgr& sm = StatueInfoMgr::Singleton();
						sm.SetGlobalNpcLoaded();
					}
					break;

				case global_data_instance:
					{
						if( pRet->GetRet( ) && 
							pRet->GetExeRet( ) &&
							nRetSize == sizeof(g_InstanceGlobalData) )
						{
							memcpy(&g_InstanceGlobalData, pRetBuf, nRetSize);
						}
						if (g_InstanceGlobalData.dwInstanceId == 0)
							g_InstanceGlobalData.dwInstanceId = 1;
					}
					break;

				case global_data_warinfo :
					{
						GetGlobalWarInfoManager().LoadWarInfoGlobalDataRet(
							pRet->GetRet( ) && pRet->GetExeRet( ),
							nRetSize,
							(unsigned char *)pRetBuf);
					}
					break;

				case global_data_poolinfo:
					{
						GetPoolCombatInfoManager().LoadWarInfoGlobalDataRet(pRet->GetRet( ) && pRet->GetExeRet( ),
							nRetSize,
							(unsigned char *)pRetBuf);
					}
					break;

				case global_data_economy:
					{
						KEconomySysManager::Singleton().LoadComplete(pRet->GetRet() && pRet->GetExeRet(), nRetSize, (unsigned char* )pRetBuf);
					}
					break;

				default:
					break;
				}//end switch
			
			}
			break;
		case Proc_SetGlobal:
			{
			}
			break;

		case Proc_Npc:
			{
				NpcSave::DbOpComplete(
					pRet->GetExeRet( ) && pRet->GetRet( ),
					pRet );
			}
			break;
		case Proc_Mail:
			{
				if( pRet->GetExeRet( ) && pRet->GetRet( ) )
				{
					//SystemSendMail
					char szSenderName[MAXSIZE_ROLENAME];
					char szReciverName[MAXSIZE_ROLENAME];
					
					pRet->GetData( 0, 0, szSenderName, sizeof(szSenderName) );
					pRet->GetData( 0, 1, szReciverName, sizeof(szReciverName) );
					
					int nReceiverIdx = g_PlayerInfoToIndex.GetIndexByName(szReciverName);

					if(INVALID_PLAYER_INDEX != nReceiverIdx)
					{
						NewMailNotify(nReceiverIdx, 1, szSenderName);
					}
				}
			}
			break;
		case Proc_Auction:
			{
			}
			break;

		case Proc_Social:
			{
				ServerSocialUnitMgr::Singleton().ProcessGlobalDBRet(pRet->GetExeRet( ) && pRet->GetRet( ),pRet);   
			}
			break;

		case Proc_SocialRecruit:
			{
				KSocialRecruitMgr::Singlton().ProcessGlobalDBRet(pRet->GetExeRet( ) && pRet->GetRet( ),pRet);
			}
			break;

		case Proc_IBShop:
			{
				if( nRetSize != sizeof(_IBShopHeader) )
					return FALSE;

				_IBShopHeader* IBShopHeader = (_IBShopHeader*)pPassBy;
				//nRetSize = pRet->GetData( 0, 0, &pRetBuf );

				switch( IBShopHeader->nIBShopID )
				{
				case ibshop_data_shelf:
					{
						IBCenter_S::Singleton().LoadIBShopFromDBRet(
							(pRet->GetExeRet()&&pRet->GetRet()), IBShopHeader->nShopIdx, pRet);
					}
					break;
				case ibshop_data_panel:
					{
						IBCenter_S::Singleton().LoadPanelFromDBRet(
							(pRet->GetExeRet()&&pRet->GetRet()), pRet);
					}
					break;
				case ibshop_data_contentstyle:
					{
						IBCenter_S::Singleton().LoadContentStyleFromDBRet(
							(pRet->GetExeRet()&&pRet->GetRet()), pRet);
					}
					break;
				case ibshop_data_item:
					{
						IBCenter_S::Singleton().LoadIBShopItemFromDBRet(
							(pRet->GetExeRet()&&pRet->GetRet()), IBShopHeader->nShopIdx, 
							IBShopHeader->nIBShopShelfIdx, pRet);
					}
					break;
				}
			}
			break;
		case Proc_GetSystemVar:
			{
				ProcessGetSystemVar(pRet, (_SystemVarHeader*)pPassBy);
			}
			break;
// 		case Proc_LoadCharacterSet:
// 			{
// 				QuestionManager::Singleton().ProcessLoadCharacterSets(pRet);	
// 			}
// 			break;

		default:
			break;
		}
	}
	return TRUE;
}

//Instance

void LoadInstanceGlobalData()
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_GetGlobal;
	DBHeader.nGlobalID = global_data_instance;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_GETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

void SaveInstanceGlobalData()
{
	_GlobalHeader DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );

	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_SetGlobal;
	DBHeader.nGlobalID = global_data_instance;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SETGLOBAL );
	
	pParam->Push( DBHeader.nGlobalID );
	
	pParam->Push( BinPair( (void*)&g_InstanceGlobalData, sizeof(g_InstanceGlobalData) ) );
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
}

#endif

#ifndef _SERVER
void g_SetClient(LPVOID pClient, int nConnectID)
{
	g_pClient = reinterpret_cast< IClient * >(pClient);
	g_ConnectID = nConnectID;
}

unsigned int	InitAdjustColorTab()
{
	g_pAdjustColorTab = NULL;
	g_ulAdjustColorCount = 0;
	KTabFile TabFile;
	if (!TabFile.Load(ADJUSTCOLOR_TABFILE))
	{
		_ASSERT(0);
		g_DebugLog("can't open file %s", ADJUSTCOLOR_TABFILE);
		return 0;
	}

	int nHeight = TabFile.GetHeight() - 1;
	
	if (nHeight <= 0)
		return 0;

	g_pAdjustColorTab = (unsigned int *)new unsigned long [nHeight];
	g_ulAdjustColorCount = nHeight;

	for (int i = 0; i < nHeight; i ++)
	{
		BYTE bAlpha = 0;
		BYTE bRed	= 0;
		BYTE bGreen	= 0;
		BYTE bBlue	= 0;
		int nAlpha;
		int nRed;
		int nGreen;
		int nBlue;
		TabFile.GetInteger(i + 2, "ALPHA", 0x000000ff, &nAlpha);
		nAlpha	&= 0xff;
		TabFile.GetInteger(i + 2,"RED", 0, &nRed);
		nRed	&= 0xff;
		TabFile.GetInteger(i + 2,"GREEN",  0, &nGreen);
		nGreen	&= 0xff;
		TabFile.GetInteger(i + 2,"BLUE",  0, &nBlue);
		nBlue	&= 0xff;
		unsigned long ulAdjustColor = nAlpha << 24 | nRed << 16 | nGreen << 8 | nBlue;
		g_pAdjustColorTab[i] = ulAdjustColor;
	}
	return g_ulAdjustColorCount;
}
#endif
