/**********************************************************************
** Description : INI区块类
** FileName    : IniSection.h
** Author      : wangbin
** Datetime    : 2004-05-11 15:40
** Comment     : 读取INI区块信息
**********************************************************************/
// IniSection.h: interface for the CIniSection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INISECTION_H__9F44A049_63E0_4A0A_B52C_597751E03C86__INCLUDED_)
#define AFX_INISECTION_H__9F44A049_63E0_4A0A_B52C_597751E03C86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <list>
#include <string>
using namespace std;

class CIniSection  
{
public:
	CIniSection();
	~CIniSection();
	//---------------------------------------------------------------------
	// function	: 获取记录的数量
	// return	: ULONG
	//---------------------------------------------------------------------
	ULONG GetEntryCount();
	//---------------------------------------------------------------------
	// function	: 查询命名记录是否存在
	// return	: BOOL
	//---------------------------------------------------------------------
	BOOL HasEntry(LPCSTR pcszKey);
	//---------------------------------------------------------------------
	// function	: 获取命名记录的值
	// parameter: pcszKey[in]   命名记录的键名
	// parameter: pszVal[out]	命名记录键值缓冲区
	// parameter: cbMaxSize[in] 缓冲区的最大值
	// return	: BOOL 成功或者失败
	//---------------------------------------------------------------------
	BOOL GetEntry(LPCSTR pcszKey, LPSTR pszVal, DWORD cbMaxSize);
	//---------------------------------------------------------------------
	// function	: 获取当前记录的名值
	// parameter: pszKey[out]		命名记录键名缓冲区
	// parameter: cbKeyMaxSize[in]	键值缓冲区的最大值
	// parameter: pszVal[out]		命名记录键值缓冲区
	// parameter: cbValMaxSize[in]	键值缓冲区的最大值
	// return	: BOOL 成功或者失败
	//---------------------------------------------------------------------
	BOOL GetEntry(LPSTR pszKey, DWORD cbKeyMaxSize, LPSTR pszVal, DWORD cbValMaxSize);
	//---------------------------------------------------------------------
	// function	: 复位指针到第一个记录
	// return	: void
	// comment	: 影响GetEntry(LPSTR,DWORD,LPSTR,DWORD)
	//---------------------------------------------------------------------
	void Reset();	// 复位指针到第一个记录
	//---------------------------------------------------------------------
	// function	: 移动指针到下一个记录
	// return	: void
	// comment	: 影响GetEntry(LPSTR,DWORD,LPSTR,DWORD)
	//---------------------------------------------------------------------
	void Step();
	//---------------------------------------------------------------------
	// function	: 指针是否到达尾部
	// return	: BOOL
	//---------------------------------------------------------------------
	BOOL Eof();
	//---------------------------------------------------------------------
	// function	: 清除内存中的记录对象
	// return	: void
	//---------------------------------------------------------------------
	void Clear();	
	//---------------------------------------------------------------------
	// function	: 复制区块，把当前区块的内容复制到指定区块中
	// return	: void
	//---------------------------------------------------------------------
	void Clone(CIniSection *pSection) const;
	//---------------------------------------------------------------------
	// function	: 增加记录
	// return	: BOOL 成功或者失败，如果键名重复返回失败
	//---------------------------------------------------------------------
	BOOL AddEntry(LPCSTR pcszKey, LPCSTR pcszVal);
	//---------------------------------------------------------------------
	// function	: 获取所在区块名字
	// return	: BOOL 成功或者失败
	//---------------------------------------------------------------------
	BOOL GetSectionName(LPSTR pszSectionName, DWORD cbMaxSize);
	//---------------------------------------------------------------------
	// function	: 设置所在区块名字
	// return	: void
	//---------------------------------------------------------------------
	void PutSectionName(LPCSTR pcszSectionName);
private:
	struct RECORD
	{
		string strKey;
		string strVal;
		RECORD(LPCSTR pcszKey, LPCSTR pcszVal) : strKey(pcszKey), strVal(pcszVal)
		{}
	};
	map<string, RECORD*>		  m_mapRecords;
	list<RECORD*>				  m_listRecords;
	list<RECORD*>::const_iterator m_iterRecords;
	string						  m_strSection;		// 区块名字
};

#endif // !defined(AFX_INISECTION_H__9F44A049_63E0_4A0A_B52C_597751E03C86__INCLUDED_)
