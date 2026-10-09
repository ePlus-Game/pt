/////////////////////////////////////////////////////////////
//
//		Kingsoft Blaze Game Studio. Copyright (C) 2008
//		Create time	:	07/17/2008
//		File base	:	UiEntrustController
//		File ext	:	h
//		Create by	:	DarkMagic(DuanMu)
//		Description	:	ÄÚÖÃÒ£¿ØÆ÷
//
/////////////////////////////////////////////////////////////

#ifndef _UIENTRUSTCONTROLLER_H_
#define _UIENTRUSTCONTROLLER_H_

#include "Ui/UiCommon.h"
#include "Ui/UiCase/UiStudySkillManage.h"
#include "TLGameObject.h"

enum SkillIndex
{
	AttackSkill = 0,
	Skill0,
	Skill1,
	Skill2,
	Skill3,
	Skill4,
	ShouSkill,
	SkillCount,
};

class KUiEntrustSkill : public KUiWndSingleton<KUiEntrustSkill>
{
public:
	typedef std::map<std::string, std::string> _shortcutkey;
/*	void	onCreate			(UIMDLEvent& rEvent);
	void	onChange			(UIMDLEvent& rEvent);
	void	onRelease			(UIMDLEvent& rEvent);*/
	void	Init				(void);

public:
	bool	ShowSkills			(int nIdx);
	bool	OnClickSkill		(const CEGUI::EventArgs& args);
	void	SelectSkill			(TLGameObject * SkillObj, int nIdx);
	void	ClearAllShortcutKey	(void);
	void	ClearAllLRSkill		(void);
	void	setFileName			(const char *iniFileName)
	{
		strcpy(m_filename, iniFileName);
	}
	void	setRoleName			(const char* szRoleName)
	{
		strcpy(m_rolename, szRoleName);
	}

private:
	int				m_SkillIdx;
	int				m_nSkillKindCount;
	int				m_nSkillCount;
	int				m_SkillKindArray[MAX_SKILL_COUNT];
	int				m_SkillArray[MAX_SKILL_COUNT];
	_shortcutkey	m_lKeyList;
	_shortcutkey	m_rKeyList;
	char			m_filename[COMMON_CLIENT_MSG_LEN_256];
	char			m_rolename[COMMON_CLIENT_MSG_LEN_64];

public:
	KUiEntrustSkill(const CEGUI::String & sttPath);
	~KUiEntrustSkill();
};
#endif