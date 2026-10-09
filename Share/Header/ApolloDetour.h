/********************************************************************
	created:	2003/10/14
	created:	14:10:2003   13:12
	filename: 	D:\ApolloDetour\ApolloDetour\ApolloDetour.h
	file path:	D:\ApolloDetour\ApolloDetour
	file base:	ApolloDetour
	file ext:	h
	author:		万立新
	
	purpose:	
*********************************************************************/

// ApolloDetour.h: interface for the CApolloDetour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined _APOLLODETOUR_H_
#define _APOLLODETOUR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define Apollomemcpy(dst, src, count)	{	size_t size = count; char* pdst = (char*)(dst); char* psrc = (char*)(src);	\
        while (size--) {	\
                *pdst = *psrc;	\
                pdst = pdst + 1;	\
                psrc = psrc + 1;	\
		}}	\

#define Apollomemset(dst, val, count)	{	size_t size = count; char* pdst = (char*)(dst);	\
        while (size--) {	\
                *pdst = (char)val;	\
                pdst = pdst + 1;	\
        }}	\
		
class CDetourAssigner
{
public:
	typedef struct
	{
		LONG lOnBeginDetourOffset;		//OnBeginDetour函数距挂接自定义数据首的偏移
		LONG lOnEndDetourOffset;		//OnEndDetour函数距挂接自定义数据首的偏移
		LONG lQueryDetourInfoOffset;	//QueryDetourInfo函数距挂接自定义数据首的偏移
	} DETOURINFO,* PDETOURINFO;

	typedef BOOL (WINAPI* defOnBeginDetour)(PBYTE pCustomDetourInfo);	//pDetourInfo指向自定义数据
	typedef BOOL (WINAPI* defOnEndDetour)(PBYTE pCustomDetourInfo);	//pDetourInfo指向自定义数据
	typedef BOOL (WINAPI* defQueryDetourInfo)(PBYTE pCustomDetourInfo, LONG lIndex, PBYTE* ppbTarget, PBYTE* ppbDetour);	//pDetourInfo指向自定义数据
																															//这个函数返回每个要挂接的函数的地址，因为不同进程可能会有重定位的问题
																															//所以需要每个进程都查询要挂接的函数的地址，如果当前进程没有这个要挂接的函数，可以让*ppbTarget返回NULL，但不能跳过

	virtual LONG GetDetourDataSize()=0;			//返回挂接自定义数据的内存大小
	virtual LONG GetDetourApiNums()=0;
	virtual BOOL InitDetour(PDETOURINFO pDetourInfo, PBYTE pCustomDetourInfo)=0;
	virtual BOOL BeforeDetourProcess(DWORD dwProcessId, PBYTE pCustomDetourInfo)=0;
	virtual BOOL AfterDetourProcess(DWORD dwProcessId, PBYTE pCustomDetourInfo)=0;
};


class CDetourBase;
class CHookApiAgent;

class CApolloDetour  
{
public:
	CApolloDetour();
	virtual ~CApolloDetour();

	BOOL UnHook();
	//在调用成员函数 AddProcess 之前要先调用Hook，除非
	//在构造函数里指定参数 bHookNow 为TRUE
	BOOL Hook(BOOL bHookAllProcess = FALSE);
	
	//挂接一个进程
	BOOL AddProcess(DWORD dwProcessId);
	//移除某个进程的挂接
	BOOL RemoveProcess(DWORD dwProcessId);
private:
	CDetourAssigner* m_pDetourAssigner;
	CDetourBase* m_pDetourBase;
	CHookApiAgent* m_pHookApiAgent;
	HANDLE m_hMutex; 
	BOOL m_bFirstDetour;
};

#endif // !defined _APOLLODETOUR_H_
