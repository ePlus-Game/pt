//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KTabFile.h
// Date:	2002.02.18
// Code:	Spe
// Desc:	Header File

// CodeReset:Daniel liu
//---------------------------------------------------------------------------
#ifndef KTabFile_H
#define KTabFile_H

#include "KList.h"
#include "KITabFile.h"

//[[2004.3.18 Add by Daniel liu
/*
class CContextNode : public KNode
{
public:
	CContextNode();
	~CContextNode();

	bool SetContent( const char *pStr );
	bool SetContent( const char *pStr, int nBeingPos, int nLength );

	int			GetINT() const { return m_nValue; };
	float		GetFloat() const { return m_fValue; };
	const char *GetStr() const { return m_pString; };
	int			GetStrLength() const { return m_nStrLength; };

private:
	
	bool ConvertType( const char *pStr, int &nValue, float &fValue ) const;

private:
	
	int		m_nValue;
	float	m_fValue;
	
	char *	m_pString;
	int		m_nStrLength;
};

class ENGINE_API KTabFile : public KITabFile
{
public:
	KTabFile();
	~KTabFile();

private:

	int		m_nRow, m_nCol;

	KList	m_theItemList;
	DWORD	*m_pdwAddrTable; // y*m_nCol + x <= Table[x][y]

	char	m_scFileName[MAX_PATH];

	BOOL	AnalyseContent( const char *pBuf, int nSize );
	int		Str2Col( LPSTR szColumn );

public:
	
	BOOL	Load( LPSTR FileName );
	BOOL	Save( LPSTR FileName );
	int		FindRow( LPSTR szRow );
	int		FindColumn(LPSTR szColumn);
	int		GetWidth() { return m_nCol; };
	int		GetHeight() { return m_nRow; };
	BOOL	GetString(int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab = TRUE);
	BOOL	GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL	GetString(LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL	GetInteger(int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab = TRUE);
	BOOL	GetInteger(int nRow, int nColumn, int nDefault, int *pnValue);
	BOOL	GetInteger(LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue);
	BOOL	GetFloat(int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab = TRUE);
	BOOL	GetFloat(int nRow, int nColumn, float fDefault, float *pfValue);
	BOOL	GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue);
	void	Clear();
};
*/
//]]

#include "KMemClass.h"
#include "KITabFile.h"
//---------------------------------------------------------------------------
typedef struct tagTabOffset
{
	DWORD		dwOffset;
	DWORD		dwLength;
} TABOFFSET;

class ENGINE_API KTabFile:public KITabFile
{
private:
	int			m_Width;
	int			m_Height;
	KMemClass	m_Memory;
	KMemClass	m_OffsetTable;
private:
	void		CreateTabOffset();
	BOOL		GetValue(int nRow, int nColumn, LPSTR lpRString, DWORD dwSize);
	int			Str2Col(LPSTR szColumn);
public:
	KTabFile();
	~KTabFile();
	BOOL		Load(LPCTSTR FileName);
	BOOL		Save(LPSTR FileName){return FALSE;}; //无法保存
	BOOL		LoadPack(LPSTR FileName);
	int			FindRow(LPSTR szRow);//返回以1为起点的值
	int			FindColumn(LPSTR szColumn);//返回以1为起点的值
	void		Col2Str(int nCol, LPSTR szColumn);
	int			GetWidth() { return m_Width;};
	int			GetHeight() { return m_Height;};
	BOOL		GetString(int nRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize, BOOL bColumnLab = TRUE);
	BOOL		GetString(int nRow, int nColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL		GetString(LPSTR szRow, LPSTR szColumn, LPSTR lpDefault, LPSTR lpRString, DWORD dwSize);
	BOOL		GetInteger(int nRow, LPSTR szColumn, int nDefault, int *pnValue, BOOL bColumnLab = TRUE);
	BOOL		GetInteger(int nRow, int nColumn, int nDefault, int *pnValue);
	BOOL		GetInteger(LPSTR szRow, LPSTR szColumn, int nDefault, int *pnValue);
	BOOL		GetFloat(int nRow, LPSTR szColumn, float fDefault, float *pfValue, BOOL bColumnLab = TRUE);
	BOOL		GetFloat(int nRow, int nColumn, float fDefault, float *pfValue);
	BOOL		GetFloat(LPSTR szRow, LPSTR szColumn, float fDefault, float *pfValue);
	void		Clear();
};
//---------------------------------------------------------------------------

#endif
