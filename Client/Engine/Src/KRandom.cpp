//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KRandom.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Random Number Generate Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KRandom.h"
//---------------------------------------------------------------------------
#define IM 139968
#define IA 3877
#define IC 29573
//---------------------------------------------------------------------------
static UINT nRandomSeed = 42;
//---------------------------------------------------------------------------
// 函数:	RandomnSeed
// 功能:	设置随机数种子
// 参数:	nRandomSeed	:	随机数种子
// 返回:	void
//---------------------------------------------------------------------------
void g_RandomSeed(UINT nSeed)
{
	nRandomSeed = nSeed;
}
//---------------------------------------------------------------------------
// 函数:	Random
// 功能:	返回一个小于nMax的随机整数
// 参数:	nMax	:	最大值
// 返回:	一个小于nMax的随机数
//---------------------------------------------------------------------------
UINT g_Random(UINT nMax)
{
	if (nMax)
	{
#ifdef _USENEWRANDOMFUNC
		//--> Rocker 2004/08/23
		unsigned int f = nRandomSeed * 0x08088405 + 1;
		nRandomSeed = f;
		#ifndef _WIN32
		long long t = (long long)f * (long long)nMax;
		#else
		_int64 t = (_int64)f * (_int64)nMax;
		#endif
		t = t >> 32;
		return (unsigned int)t;
		//<-- End
#else
		// Modify by cooler
		// liuyujun@263.net 2004/3/25 -->
		//nRandomSeed = nRandomSeed * IA + IC;
		//return nRandomSeed % nMax;
		// Standard C rand function, max rand number is 32767
		nRandomSeed = nRandomSeed * 214013L + 2531011L;
		return ((nRandomSeed >> 16) & 0x7fff) % nMax;
		// <-- End cooler modify.
#endif		
	}
	else
	{
		return 0;
	}
}
//---------------------------------------------------------------------------
// 函数:	GetRandomSeed
// 功能:	取得当时的伪随机种子
// 返回:	返回当前的伪随机种子
//---------------------------------------------------------------------------
UINT g_GetRandomSeed()
{
	return nRandomSeed;
}
