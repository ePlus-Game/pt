//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 10:42
//      File_base        : specialskill_tab
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _specialskill_tab_h

#define  _specialskill_tab_h

#include "specialskill_def.h"

/************************************************************************/
/*                      declare _BaseSpecialSkillTable	                */
/************************************************************************/
enum
{
	tabfile_specialskill_type_string,
	tabfile_specialskill_type_int,
	tabfile_specialskill_type_byte,
};//*/

struct _SpecialSkill_Record 
{
	char	szName[MAX_SPECIALSKILL_TITLENAME];
	int		nRecordType;
	void*	pStoreBuf;
	int		nSize;
};

class _BaseSpecialSkillTable
{
public:
	_BaseSpecialSkillTable( void );
	~_BaseSpecialSkillTable( void );

public:
	
	int OpenTable( char* szFileName );
	
	int GetRecordCount( void );
	
	void CloseTable( void );

	int LoadRecord(
		int nRow,
		_SpecialSkill_Record Rec[],
		int nMaxCount );

private:
	KTabFile	m_TabFile;
	BOOL		m_bOpen;
};


/************************************************************************/
/*                      declare SpecialSkillTab		                    */
/************************************************************************/

typedef struct _SpecialSkill_Template
{
	BYTE	byMetier;
	BYTE	bySeries;
	BYTE	byLevel;
	int		nSkillID;
	int		nFirstBuffID;
	int		nLevelupBuffID;
}SST,*PSST;

class SpecialSkillTab : public _BaseSpecialSkillTable
{
	typedef std::map<int,int> _SpecialSkillMap;
public:
	SpecialSkillTab( void );
	~SpecialSkillTab( void );

public:
	static SpecialSkillTab& Singleton( void );

	int	GetSpecialSkill( 
		BYTE byMetier,
		BYTE bySeries,
		BYTE byLevel );

	int	GetLevelupBuffID( 
		BYTE byMetier,
		BYTE bySeries,
		BYTE byLevel );
	
	int GetNearlySpecialSkill( 
		BYTE byMetier, 
		BYTE bySeries, 
		BYTE byLevel );

	int GetNearlyFirstupBuffID(
		BYTE byMetier, 
		BYTE bySeries, 
		BYTE byLevel );

	int LoadSpecialSkill( void );

private:
		_SpecialSkillMap	m_SSTIndex;
	SST		m_SST[MAX_SPECIALSKILL_COUNT];
	int		m_nSpecialSkillCount;
	int		m_nLoad;
};

#endif