//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-06-26 17:24
//      File_base        : PlayerCreator.h
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : Create New Player Function
//
//////////////////////////////////////////////////////////////////////

#ifndef _PLAYERCREATOR_H_
#define _PLAYERCREATOR_H_

#include "KCore.h"
#include "cfs_fs2_savedef.h"

#define SECTIONSIZE 64
#define KEYNAME	"RevivalId"
#define COUNT	"Count"
#define SECTION_ITEMLIST "ITEMS"
#define NEWROLELEN	( 8 * 1024 )

#define MAX_REVIVAL_MAP_COUT (5)
#define MAX_REVIVAL_POS		(3)

#define MAX_TOTAL_COUNT_REVIVAL (MAX_REVIVAL_MAP_COUT * MAX_REVIVAL_POS)

#define MAX_ROLE_COUNT ( 2 * enRoleType_Number )

#define	PLAYERCREATOR_FILE		"settings\\player\\newplayerini%02d.ini"
#define REVIVALID_FILENAME		"settings\\player\\revivalid.ini"

typedef struct tagRoleGeneratorParam
{
	char	szAccName[_NAME_LEN];
	char	szName[_NAME_LEN];
	int		nSeries;
	int		nSex;
	int		nMapID;
	int		nImageHead;
	unsigned long ulNetID;
} ROLEPARAM, *PROLEPARAM;
/////////////////////////////////////////////////////////////////////////////
//
//              Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class PlayerCreator
{
	struct _re_pos 
	{
		int nMap;
		int nRevivalPos;
	};

public:
	
	int Init( );
	static PlayerCreator& Singleton( );

	int CreateRole(
		ROLEPARAM& RP );
	int GetInitialSkillCount(int nProf, int nSex) const;
	const void* GetInitialSkillData(int nProf, int nSex) const;
		
private:
	int GetRoleFromIni( 
		BYTE* pData, 
		const char* szFileName, 
		int nRoleType );

	int GetRevivalFromIni( const char* szFileName );
	int AddRevivalPos( int nMap, int nRevivalPos );
	int GetRevivalPos( int nMap );

private:

	static _re_pos	m_sRevivalPos[MAX_TOTAL_COUNT_REVIVAL];
	static char		m_szRoleTemplateBuf[MAX_ROLE_COUNT * NEWROLELEN];
	static char*	m_pBuf[MAX_ROLE_COUNT];
};

#endif