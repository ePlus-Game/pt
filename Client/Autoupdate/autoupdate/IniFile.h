/**********************************************************************
** Description : INI数据库类
** FileName    : IniFile.h
** Author      : wangbin
** Datetime    : 2004-05-11 15:40
** Comment     : 读取INI信息，INI文件可以是一个文件或者内存块
**********************************************************************/
// IniFile.h: interface for the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__EF56219E_F0AD_4002_8F91_FC0A8850A052__INCLUDED_)
#define AFX_INIFILE_H__EF56219E_F0AD_4002_8F91_FC0A8850A052__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IniSection.h"
#include <map>
#include <list>
#include <string>
using namespace std;

class CIniSection;
class CIniFile  
{
public:
	CIniFile();
	virtual ~CIniFile();
	//---------------------------------------------------------------------
	// function	: 解析内存块中的INI信息
	// parameter: pcszBuffer 内存块指针
	// parameter: cbBuffSize  内存块的字节数
	// return	: void
	//---------------------------------------------------------------------
	void Parse(LPCSTR pcszBuffer, DWORD cbBuffSize);
	//---------------------------------------------------------------------
	// function	: 解析文件的INI信息
	// parameter: pcszFileName INI文件全路径名
	// return	: void
	//---------------------------------------------------------------------
	void Parse(LPCSTR pcszFileName);
	//---------------------------------------------------------------------
	// function	: 获取区块的数量
	// return	: ULONG
	//---------------------------------------------------------------------
	ULONG GetSectionCount();
	//---------------------------------------------------------------------
	// function	: 获取当前指针指向的区块
	// return	: CIniSection*
	// comment	: 返回的区块指针不能释放，因为区块对象由所在的CIniFile管理。
	//			  如果当前指针指向结尾，则返回NULL。该函数不影响当前指针
	//---------------------------------------------------------------------
	CIniSection *GetSection();
	//---------------------------------------------------------------------
	// function	: 获取命名区块
	// parameter: pcszSectionName 区块名字
	// return	: CIniSection*
	// comment	: 返回的区块指针不能释放，因为区块对象由所在的CIniFile管理。
	//			  如果当前指针指向结尾，则返回NULL。该函数不影响当前指针
	//---------------------------------------------------------------------
	CIniSection *GetSection(LPCSTR pcszSectionName);
	//---------------------------------------------------------------------
	// function	: 查询命名区块是否存在
	// parameter: pcszSectionName 区块名字
	// return	: BOOL TRUE-命名区块存在；FALSE-命名区块不存在
	// comment	: 返回的区块指针不能释放，因为区块对象由所在的CIniFile管理。
	//			  如果当前指针指向结尾，则返回NULL。该函数不影响当前指针
	//---------------------------------------------------------------------
	BOOL HasSection(LPCSTR pcszSectionName);
	//---------------------------------------------------------------------
	// function	: 复位指针，移动到第一个区块位置
	// return	: void
	// comment	: 该函数影响GetSection(void)的返回结果，但是不影响GetSection(LPCSTR)
	//---------------------------------------------------------------------
	void Reset();
	//---------------------------------------------------------------------
	// function	: 清除管理的所有区块对象
	// return	: void
	// comment	: 无持久化性质
	//---------------------------------------------------------------------
	void Clear();
	//---------------------------------------------------------------------
	// function	: 把当前区块对象指针向后移动一位
	// return	: void
	// comment	: 该函数影响GetSection(void)的返回结果，但是不影响GetSection(LPCSTR)
	//---------------------------------------------------------------------
	void Step();
	//---------------------------------------------------------------------
	// function	: 判断区块指针是否已经到达尾部位置
	// return	: BOOL TRUE-指针已经到达尾部；FALSE-指针指向有效区块对象
	//---------------------------------------------------------------------
	BOOL Eof();
private:
	// 解析缓冲区中的字符串，如果是新的区块则*ppNewSection返回非空指针，如果是区块记录则加入pSection
	void Parse(LPSTR pszBuffer, DWORD cbBuffSize, CIniSection *pCurSection, CIniSection **ppNewSection);
	// 把消息缓冲区中的第一个字符串复制到指定最大长度的缓冲区中，并通过返回后继指针
	void Parse(LPSTR pszMessage, DWORD dwMsgSize, LPSTR pszBuffer, DWORD cbMaxSize, DWORD *pcbFetched);
	// 增加区块
	void AddSection(CIniSection *pSection);
	// 去除字符串首尾的空白字符，如果全是空白字符返回NULL，否则返回子字符串
	LPSTR Trim(LPSTR pszMessage);
	// 判断是否空白字符
	inline BOOL IsBlank(char ch)
	{
		// 这里不应该出现回车、换行、结束符号
		ASSERT(ch != '\r' && ch != '\n' && ch != '\0');
		return ch == ' ' || ch == '\t';
	}
private:
	enum {BUFFER_MAXSIZE = 2048};	// 临时缓冲区最大长度
	map<string, CIniSection*>			m_mapSections;
	list<CIniSection*>					m_listSections;
	list<CIniSection*>::const_iterator	m_iterSections;
};

#endif // !defined(AFX_INIFILE_H__EF56219E_F0AD_4002_8F91_FC0A8850A052__INCLUDED_)
