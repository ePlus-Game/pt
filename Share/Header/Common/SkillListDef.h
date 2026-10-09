// SkillListDef.h: Some skill define
// Skill macro define
// by Cooler liuyujun@263.net 2004.03.04

#ifndef _SKILLLISTDEF_H_
#define _SKILLLISTDEF_H_

#define	MAX_NPCSKILL					80

#define PLAYERSKILLSINI_KNIGHT			"Knight"
#define PLAYERSKILLSINI_ENCHANTER		"Enchanter"
#define PLAYERSKILLSINI_MONSTROUS		"Monstrous"

#define PLAYERSKILLSINI_INITSKILLSKEY	"InitSkills"

// Notes: Can't change the following enum member's sequence
#define		NUM_ROLETYPE	3

// Skill disable reason define
#define		SKILLDISABLEREASON_NONE			0x0000
#define		SKILLDISABLEREASON_PLAYERLEVEL	0x0001
#define		SKILLDISABLEREASON_NOPARENT		0x0002
#define		SKILLDISABLEREASON_PARENTLEVEL	0x0004
#define		SKILLDISABLEREASON_PLAYEROTHER	0x0008
// add by chenshanglin on 2006-1-4 for new skill system
#define		SKILLDISABLEREASON_EXCLUSION	0x0010
// add end

// Role type enum
//typedef enum enumROLETYPE
//{
//	enRoleType_Knight = 0, 
//	enRoleType_Enchanter, 
//	enRoleType_Monstrous, 
//	enRoleType_Number,		// Enum type number
//}enROLETYPE, *enPROLETYPE;

// Skill status enum
typedef enum enumSKILLSTATUS
{
	enSkillStatus_disabled = 1, 
	enSkillStatus_enabled, 
	enSkillStatus_active,
}enSKILLSTATUS, *enPSKILLSTATUS;

// commented by chenshanglin on 2006-1-16 for new skill system
// use skillrelation.ini substitute
// Role init skills struct
// typedef struct tagROLESKILLINFOITEM
// {
// 	int nSkillID;
// 	int nInitLevel;
// 
// 	// commented by chenshanglin on 2006-1-4 for new skill system
//  	CSkillRelationInfo clsRelationInfo;
//  	CLiveupExpInfo clsLiveupExpInfo;
// 	// commented
// 
// }ROLESKILLINFOITEM, *PROLESKILLINFOITEM;

// typedef struct tagROLESKILLS
// {
// 	int nSkillCount;
// 
// 	// commented by chenshanglin on 2006-1-4 for new skill system
// 	CSkillOption clsSkillOption;
// 	// commented end
// 
// 	ROLESKILLINFOITEM szListSkillInfo[MAX_NPCSKILL];
// 	int nBaseIndex;
// }ROLESKILLS, *PROLESKILLS;

// commented end

#endif //_SKILLLISTDEF_H_