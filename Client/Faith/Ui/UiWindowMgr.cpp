
#include "UiWindowMgr.h"
#include "./UiCase/UiEquipment.h"
#include "./UiCase/UiItemBox.h"
#include "./UiCase/UiStudySkillManage.h"
#include "./UiCase/UiTalisman.h"
#include "./UiCase/UiQuestManage.h"
#include "./UiCase/UiTongManager.h"
#include "./UiCase/UiHelpInfo.h"
#include "./UiCase/UiGameSetting.h"

KUiWndMgr::KUiWndMgr()
{

}

KUiWndMgr& KUiWndMgr::getSingleton()
{
	static KUiWndMgr singleton;
	return singleton;
}

void KUiWndMgr::openAWindow(WindowName wndName)
{
	switch(wndName)
	{
	case Equipment:
		{
			KUiEquipment::GetSingleton().Show();
		}
		break;	
	case ItemBox:
		{
			KUiItemBox::getSingleton().show();
		}
		break;
	case Skill:
		{
			KUiStudySkillManage::GetSingleton().Show();
		}
		break;
	case Talisman:
		{
			KUiTalisman::getSingleton().show();
		}
		break;
	case Quest:
		{
			KUiQuestManage::GetSingleton().show();
		}
		break;
	case SocialCtrl:
		{
			KUiTongManager::GetSingleton().Show();
		}
		break;
	case Help:
		{
			//KUiHelpInfo::GetSingleton().Show(); KUiHelpInfo窗口已经不在使用，功能合并到封神宝典中
		}
		break;
	case Setting:
		{
			KUiGameSetting::GetSingleton().Show();
		}
	case Title:
		{
			KUiEquipment::GetSingleton().ShowTitle();
		}
		break;
	}
}