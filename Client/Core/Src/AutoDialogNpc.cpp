#include "KCore.h"
#ifdef _AUTO_ROBOT
#include "AutoDialogNpc.h"

#include "KNpcTemplate.h"
#include "KPlayer.h"

AutoDialogNpc::AutoDialogNpc()
{

}

AutoDialogNpc::~AutoDialogNpc()
{

}

AutoDialogNpc& AutoDialogNpc::getSingleton()
{
	static AutoDialogNpc singleton;
	return singleton;
}

void AutoDialogNpc::openDialogNpc()
{
	if(AUTO_DIALOG_NPC_INVALID_DIALOG_NPC_ID == _dialogNpcTemplateId)
	{
		return;
	}

	for(int i = 1; i < MAX_NPC; ++i)
	{
		if(Npc[i].IsValid() && Npc[i].GetTemplate())
		{
			if(Npc[i].GetTemplate()->m_NpcSettingIdx == _dialogNpcTemplateId)
			{
				GetClientPlayer().DialogNpc(i);
				_dialogNpcTemplateId = AUTO_DIALOG_NPC_INVALID_DIALOG_NPC_ID;
				break;
			}
		}
	}
	return;
}

#endif	// #ifdef _AUTO_ROBOT