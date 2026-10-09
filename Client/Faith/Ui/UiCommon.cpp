//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/09/2006 16:49
//      File_base        : UiLoginBg
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 界面应用模板
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiCommon.h"
#include "CoreUseNameDef.h"
#include "layoutinterface.h"
#include "AuctionComDef.h"
#include "Ui/UiSheetMgr.h"
#include "Ui/UiCase/UiStoreBox.h"
#include "Ui/UiCase/UiStudySkillManage.h"
#include "Ui/UiCase/UiNpcMsgBox.h"
#include "Ui/UiCase/UiShop.h"
#include "Ui/UiCase/UiSmith.h"
#include "Ui/UiCase/UiCompound.h"
#include "Ui/UiCase/UiMailCentre.h"
#include "Ui/UiCase/UiVendueWnd.h"
#include "Ui/UiCase/UiTargetEquipment.h"
#include "Ui/UiCase/UiBufferWnd.h"
#include "Ui/UiCase/UiDebufferWnd.h"
#include "Ui/UiCase/UiDragItem.h"
#include "Ui/UiCase/UiDuraAlert.h"
#include "Ui/UiCase/UiPlayerMenu.h"
#include "Ui/UiCase/UiTargetMenu.h"
#include "Ui/UiCase/UiItemTip.h"
#include "Ui/UiCase/UiPKFilter.h"
#include "Ui/UiCase/UiTeamList.h"
#include "Ui/UiCase/UiRoleFacePopMenu.h"
#include "Ui/UiCase/UiTradeBox.h"
#include "Ui/UiCase/UiTargetFace.h"
#include "Ui/UiCase/UiTeamList.h"
#include "Ui/UiCase/UiShortcutWnd.h"
#include "Ui/UiCase/UiShortcutPlusWnd.h"
#include "Ui/UiCase/UiTaisuiWnd.h"
#include "Ui/UiCase/UiDeathBox.h"
#include "Ui/UiCase/UiItemBox.h"
#include "Ui/UiCase/UiMapCentre.h"
#include "Ui/UiCase/UiFSBible.h"
#include "Ui/UiCase/UiNpcNavigation.h"
#include "Ui/UiCase/UiRandomCopyRewards.h"
#include "Ui/UiCase/UiGMCommunication.h"
#include "ui/UiCase/UiHire.h"
#include "ui/UiCase/UiItemOperPanel.h"
#include "ui/UiCase/UiElf.h"
#include "ui/UiCase/UiRecommend.h"
#include "ui/UiCase/UiPetFrame.h"
#include "ui/UiCase/UiChatConfig.h"
#include "ui/UiCase/UiEntrustComputer.h"
#include "ui/UiCase/UiInfoBar.h"
#include "ui/UiCase/UiQuestionWindow.h"
#include "ui/UiCase/UiItemPassword.h"
#include "ui/UiCase/UiRoleExp.h"
#include "Ui/UiCase/UiPointListCharts.h"
#include "ui/UiCase/UiBattleResult.h"
#include "ui/UiCase/UiTongManager.h"
#include "Ui/UiCase/UiGenPersonalInfo.h"

extern iCoreShell*		g_pCoreShell;

KUiWnd::KUiWnd()
{
	m_pCEGUISystem		= CEGUI::System::getSingletonPtr();
	m_pWindowManager	= CEGUI::WindowManager::getSingletonPtr();
	m_pRootSheet		= KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
	m_pThisWnd			= NULL;
	GetMDLPtr( &m_pUiMDLManager );
}

int uiMoneyToSysMoney(int jin, int yin, int tong)
{
	return jin * 10000 + yin * 100 + tong;
}

void sysMoneyToUiMoney(int money, int& jin, int& yin, int& tong)
{
	jin = money / 10000;
	yin = (money % 10000) / 100;
	tong = money % 100; 
}

CEGUI::String iToString(int num)
{
	char newString[20];
	_itoa(num, newString, 10);
	return CEGUI::String(newString);
}

void lorectToCerect(void* lorect, CEGUI::Rect* cerect)
{
	cerect->d_left = ((LORect*)lorect)->getPosition().x;
	cerect->d_top = ((LORect*)lorect)->getPosition().y;
	cerect->d_right = ((LORect*)lorect)->getRight();
	cerect->d_bottom = ((LORect*)lorect)->getBottom();
}

void cerectToLorect(CEGUI::Rect* cerect, void* lorect)
{
	LORect* rect = (LORect*)lorect;
	rect->setPos(cerect->d_left, cerect->d_top);
	rect->setWidth(cerect->getWidth());
	rect->setHeight(cerect->getHeight());
}

bool _vendueFindItemFromMDL( void* pParam0, void* pParam1 )
{
	SEARCH_DB_RETDATA* pRecord = (SEARCH_DB_RETDATA*)pParam0;
	int*				pID		= (int*)pParam1;
	if ( pRecord )
	{
		if ( pRecord->recordId == *pID )
		{
			return true;
		}
	}
	return false;
}

void ansiToUnicode(const char* ansiText, wchar_t*& unicodeText)
{
	//得到unicode编码方式的文字
	size_t lengthAnsi = strlen(ansiText);
	size_t lengthUnicode = MultiByteToWideChar(CP_ACP, 0, ansiText, lengthAnsi, NULL, 0);
	
	unicodeText = new wchar_t[lengthUnicode + 1];
	MultiByteToWideChar(CP_ACP, 0, ansiText, lengthAnsi, unicodeText, lengthUnicode);
	unicodeText[lengthUnicode] = 0;
}

int unicodeToAnsi(const wchar_t* unicodeText, char*& ansiText)
{
	size_t unilen = wcslen(unicodeText);
	size_t ansilen = WideCharToMultiByte(CP_ACP, 0, unicodeText, unilen, NULL, 0, NULL, NULL);
	
	ansiText = new char[ansilen + 1];
	WideCharToMultiByte(CP_ACP, 0, unicodeText, unilen, ansiText, ansilen, NULL, NULL); 
	ansiText[ansilen] = 0;

	return ansilen;
}

void printWnd(int layer, CEGUI::Window* wnd, char* fileName)
{
	static std::map<std::string, int> windowCount;
	std::fstream fs;
 	if(layer == 1)
 	{
		windowCount.clear();
		fs.open(fileName, std::ios::binary | std::ios::trunc );
 	}
 	else
 	{
 		fs.open(fileName, std::ios::binary | std::ios::app);
 	}

	std::string wndName = wnd->getType().c_str();
	if(windowCount.find(wndName) != windowCount.end())
	{
		windowCount[wndName]++;
	}
	else
	{
		windowCount[wndName] = 1;
	}
 	
	fs<<layer;
 	for(int j = 0; j < layer; ++j)
 	{
 		fs<<"--";
 	}
 	const char* name = wnd->getName().c_str();
 	const char* type = wnd->getType().c_str();
 	fs<<name<<'\t'<<type<<std::endl;
 	fs.close();
 
 	for(int i = 0; i < wnd->getChildCount(); ++i)
 	{
 		printWnd(layer + 1, wnd->getChildAtIdx(i), fileName);
 	}

 	if(layer == 1)
	{
 		fs.open(fileName, std::ios::binary | std::ios::app);
		for(std::map<std::string, int>::iterator it = windowCount.begin();
		it != windowCount.end();
		++it)
		{
			std::string type = it->first;
			int count = it->second;

			fs<<type.c_str()<<":\t"<<count<<std::endl;
		}
 		fs.close();
	}//*/
}

void PrintWindowProperty(const char* windowName)
{
	Window* window = WindowManager::getSingleton().getWindow(windowName);
	if(!window)
	{
		return;
	}

	std::fstream fs;
	fs.open("logs\\WindowProperty.log", ios::binary|ios::out);
	fs<<"Name: "<<window->getName().c_str()<<endl;
	fs<<"Type: "<<window->getType().c_str()<<endl;
	fs<<"X: "<<window->getAbsoluteXPosition()<<endl;
	fs<<"Y: "<<window->getAbsoluteYPosition()<<endl;
	fs<<"Width: "<<window->getAbsoluteWidth()<<endl;
	fs<<"Height: "<<window->getAbsoluteHeight()<<endl;
	fs<<"Alpha: "<<window->getAlpha()<<endl;
	fs<<"Text: "<<window->getText().c_str()<<endl;
	if(window->getFont())
	{
		fs<<"Font: "<<window->getFont()->getName()<<endl;
	}
	fs<<"Tooltip: "<<window->getTooltipText().c_str()<<endl;
	fs<<"ZLevel: "<<(int)window->getZLevel()<<endl;
	fs<<"Visible: "<<window->isVisible(false)<<endl;
	fs<<"Visible(local): "<<window->isVisible(true)<<endl;
	fs<<"Disable: "<<window->isDisabled(false)<<endl;
	fs<<"Disable(local): "<<window->isDisabled(true)<<endl;

	if(window->getType() == TLStaticText::WidgetTypeName)
	{
		TLStaticText* thisWindow = (TLStaticText*)window;
		fs<<"RollSpeedH: "<<thisWindow->getRollSpeedH()<<endl;
		fs<<"TextColor: "<<thisWindow->getTextColours()<<endl;
	}
	else if(window->getType() == TLStaticImage::WidgetTypeName)
	{

	}
	else if(window->getType() == GUISheet::WidgetTypeName)
	{
		GUISheet* sheet = (GUISheet*)window;
		for(int i = 0; i < sheet->d_updateList.size(); ++i)
		{
			fs<< "updatelist" << i + 1 << ": " <<sheet->d_updateList[i]->getName().c_str();
			fs<<", need update:" << sheet->d_updateList[i]->isNeddUpdate() <<endl;
		}
	}

	fs<<endl<<endl<<"parent:"<<endl;
	Window* parent = window->getParent();
	while(parent)
	{
		fs<<parent->getName().c_str()<<endl;
		parent = parent->getParent();
	}

	fs.close();
}

void closeUiWnd(CLOSE_EVENT_ID e)
{
	switch(e)
	{
	case PLAYER_DEATH:
		{
			// 死亡关闭所有NPC对话框
			if (KUiStudySkillManage::IsVisible())
				KUiStudySkillManage::Hide();

			if (KUiNpcMsgBox::IsVisible())
				KUiNpcMsgBox::Hide();
			if (KUiShop::IsVisible())
				KUiShop::Hide();
			if (KUiStoreBox::getSingleton().isVisible())
				KUiStoreBox::getSingleton().hide();
			if (KUiSmith::getSingleton().isVisible())
				KUiSmith::getSingleton().hide();
			if (KUiCompound::IsVisible())
				KUiCompound::Hide();
			if (KUiMailCentre::IsVisible())
				KUiMailCentre::Hide();
			if (KUiVendueWnd::IsVisible())
				KUiVendueWnd::Hide();
			if (KUiTargetEquipment::IsVisible())
				KUiTargetEquipment::Hide();
			if (KUiTradeBox::IsVisible())
				KUiTradeBox::Hide();
			if (KUiTargetFace::IsVisible())
				KUiTargetFace::Hide();
			if (KUiTeamHideShow::IsVisible())
				KUiTeamHideShow::Hide();
			if ( KUiLRSkillWnd::IsVisible()  )
			{
				KUiLRSkillWnd::Hide();
			}
			if (KUiRandomCopyRewards::GetSingleton().IsVisible())
			{
				KUiRandomCopyRewards::GetSingleton().Hide();
			}
			if ( KUiHire::IsVisible() )
			{
				KUiHire::Hide();
			}
			if ( KUiHireConfigExp::IsVisible() )
			{
				KUiHireConfigExp::Hide();
			}
			if ( KUiHireConfigSalary::IsVisible() )
			{
				KUiHireConfigSalary::Hide();
			}
			if ( KUiElfPopMenu::IsVisible() )
			{
				KUiElfPopMenu::Hide();
			}
			if(KUiRecommend::getSingleton().isVisible())
			{
				KUiRecommend::getSingleton().hide();
			}
			if ( KUiEntrustComputer::IsVisible() )
			{
				KUiEntrustComputer::GetSingleton().ClosePanel();
			}

			if ( KUiItemPassword::IsVisible() )
			{
				KUiItemPassword::Hide();
			}
			if ( KUiItemPassword_Create::IsVisible() )
			{
				KUiItemPassword_Create::Hide();
			}
			if ( KUiItemPassword_Modify::IsVisible() )
			{
				KUiItemPassword_Modify::Hide();
			}
			if ( KUiPointListCharts::IsVisible() )
			{
				KUiPointListCharts::Hide();
			}
			if ( KUiSmallBattleFieldResult::IsVisible() )
			{
				KUiSmallBattleFieldResult::Hide();
			}
			if (KUiChatRoomListMenu::IsVisible())
			{
				KUiChatRoomListMenu::Hide();
			}

			if ( KUiTongStatueMsg::IsVisible() )
			{
				KUiTongStatueMsg::Hide();
			}
			if ( KUiGenPersonalInfo::IsVisible())
			{
				KUiGenPersonalInfo::Hide();
			}

			g_pCoreShell->LockSomeoneAction(0);
		}
		break;
	case PLAYER_RUN:
		{
			bool needCloseItemBox = false;
			// 移动关闭所有NPC对话框
			if (KUiNpcMsgBox::IsVisible())
				KUiNpcMsgBox::Hide();
			if (KUiShop::IsVisible())
			{
				KUiShop::Hide();
				needCloseItemBox = true;
			}
			if (KUiStoreBox::getSingleton().isVisible())
			{
				KUiStoreBox::getSingleton().hide();
				needCloseItemBox = true;
			}
			if (KUiSmith::getSingleton().isVisible())
				KUiSmith::getSingleton().hide();
			if (KUiCompound::IsVisible())
				KUiCompound::Hide();
			if (KUiMailCentre::IsVisible())
				KUiMailCentre::Hide();
			if (KUiVendueWnd::IsVisible())
				KUiVendueWnd::Hide();
// 			if (KUiTargetEquipment::IsVisible())
// 				KUiTargetEquipment::Hide();
			if (KUiTradeBox::IsVisible())
			{
				KUiTradeBox::Hide();
				needCloseItemBox = true;
			}
			if ( KUiHire::IsVisible() )
			{
				KUiHire::Hide();
			}
			if ( KUiHireConfigExp::IsVisible() )
			{
				KUiHireConfigExp::Hide();
			}
			if ( KUiHireConfigSalary::IsVisible() )
			{
				KUiHireConfigSalary::Hide();
			}
			if ( KUiCompound::IsVisible() )
			{
				KUiCompound::Hide();
			}
			if ( KUiItemOperPanel::getSingleton().isVisible() )
			{
				KUiItemOperPanel::getSingleton().hide();
			}
			if ( KUiElfPopMenu::IsVisible() )
			{
				KUiElfPopMenu::Hide();
			}

			if ( KUiItemPassword::IsVisible() )
			{
				KUiItemPassword::Hide();
			}
			if ( KUiItemPassword_Create::IsVisible() )
			{
				KUiItemPassword_Create::Hide();
			}
			if ( KUiItemPassword_Modify::IsVisible() )
			{
				KUiItemPassword_Modify::Hide();
			}

			if(needCloseItemBox)
			{
				KUiItemBox::getSingleton().hide();
			}
			if (KUiChatRoomListMenu::IsVisible())
			{
				KUiChatRoomListMenu::Hide();
			}
		}
		break;
	case PLAYER_TRANSMISION:
		{
			// 传送关闭所有NPC对话框
			if (KUiStudySkillManage::IsVisible())
				KUiStudySkillManage::Hide();

			if (KUiNpcMsgBox::IsVisible())
				KUiNpcMsgBox::Hide();
			if (KUiShop::IsVisible())
				KUiShop::Hide();
			if (KUiStoreBox::getSingleton().isVisible())
				KUiStoreBox::getSingleton().hide();
			if (KUiSmith::getSingleton().isVisible())
				KUiSmith::getSingleton().hide();
			if (KUiCompound::IsVisible())
				KUiCompound::Hide();
			if (KUiMailCentre::IsVisible())
				KUiMailCentre::Hide();
			if (KUiVendueWnd::IsVisible())
				KUiVendueWnd::Hide();
			if (KUiTargetEquipment::IsVisible())
				KUiTargetEquipment::Hide();
			if (KUiTradeBox::IsVisible())
				KUiTradeBox::Hide();
			if (KUiTargetFace::IsVisible())
				KUiTargetFace::Hide();
			if ( KUiLRSkillWnd::IsVisible()  )
			{
				KUiLRSkillWnd::Hide();
			}
			if ( KUiHire::IsVisible() )
			{
				KUiHire::Hide();
			}
			if ( KUiHireConfigExp::IsVisible() )
			{
				KUiHireConfigExp::Hide();
			}
			if ( KUiHireConfigSalary::IsVisible() )
			{
				KUiHireConfigSalary::Hide();
			}
			if ( KUiElfPopMenu::IsVisible() )
			{
				KUiElfPopMenu::Hide();
			}
			if ( KUiChanConfig::getSingleton().isVisible() )
			{
				KUiChanConfig::getSingleton().hide();
			}
			if ( KUiEntrustComputer::IsVisible() )
			{
				KUiEntrustComputer::GetSingleton().ClosePanel();
			}
			if ( KUiQuestionWindow::IsVisible() )
			{
				KUiQuestionWindow::Hide();
			}
			if ( KUiItemPassword::IsVisible() )
			{
				KUiItemPassword::Hide();
			}
			if ( KUiItemPassword_Create::IsVisible() )
			{
				KUiItemPassword_Create::Hide();
			}
			if ( KUiItemPassword_Modify::IsVisible() )
			{
				KUiItemPassword_Modify::Hide();
			}
			if ( KUiRewardExpNotify::IsVisible() )
			{
				KUiRewardExpNotify::Hide();
			}
			if (KUiChatRoomListMenu::IsVisible())
			{
				KUiChatRoomListMenu::Hide();
			}
			if ( KUiTongStatueMsg::IsVisible() )
			{
				KUiTongStatueMsg::Hide();
			}
			if ( KUiGenPersonalInfo::IsVisible())
			{
				KUiGenPersonalInfo::Hide();
			}
			if ( KUiPointListCharts::IsVisible() )
			{
				KUiPointListCharts::Hide();
			}
// 			if ( KUiSmallBattleFieldResult::IsVisible() )
// 			{
// 				KUiSmallBattleFieldResult::Hide();
// 			}
			g_pCoreShell->LockSomeoneAction(0);
		}
		break;
	case SERVER_DISCONNECT:
		{
			// 服务器断开关闭对话框
			if (KUiStudySkillManage::IsVisible())
				KUiStudySkillManage::Hide();
			if (KUiNpcMsgBox::IsVisible())
				KUiNpcMsgBox::Hide();
			if (KUiShop::IsVisible())
				KUiShop::Hide();
			if (KUiStoreBox::getSingleton().isVisible())
				KUiStoreBox::getSingleton().hide();
			if (KUiSmith::getSingleton().isVisible())
				KUiSmith::getSingleton().hide();
			if (KUiCompound::IsVisible())
				KUiCompound::Hide();
			if (KUiMailCentre::IsVisible())
				KUiMailCentre::Hide();
			if (KUiVendueWnd::IsVisible())
				KUiVendueWnd::Hide();
			if (KUiTargetEquipment::IsVisible())
				KUiTargetEquipment::Hide();
			if (KUiBufferCentre::IsVisible())
				KUiBufferCentre::Hide();
			if (KUiDebufferCentre::IsVisible())
				KUiDebufferCentre::Hide();
			if (KUiDeathBox::IsVisible())
				KUiDeathBox::Hide();
			if (KUiTradeBox::IsVisible())
				KUiTradeBox::Hide();
			if (KUiTargetFace::IsVisible())
				KUiTargetFace::Hide();
			if (KUiTeamList::IsVisible())
			{
				KUiTeamList::SetDisapearFlag();
				KUiTeamList::Hide();
			}
			if (KUiShortcutWnd::IsVisible())
				KUiShortcutWnd::Hide();
			if (KUiShortcutPlusWnd::IsVisible())
				KUiShortcutPlusWnd::Hide();
			if (KUiTeamHideShow::IsVisible())
				KUiTeamHideShow::Hide();
			if (KUiLRSkillWnd::IsVisible())
				KUiLRSkillWnd::Hide();
			if (KUiSceneMap::getSinglton().isVisible())
				KUiSceneMap::getSinglton().hide();
			if (KUiBigMap::getSinglton().isVisible())
				KUiBigMap::getSinglton().hide();
			if (KUiFSBible::getSingleton().isVisible())
				KUiFSBible::getSingleton().hide();
			if (KUiItemOperPanel::getSingleton().isVisible())
				KUiItemOperPanel::getSingleton().hide();
			if (KUiGMCommunication::getSingleton().isVisible())
				KUiGMCommunication::getSingleton().hide();
			if (KUiNpcNavigation::getSingleton().isVisible())
				KUiNpcNavigation::getSingleton().hide();
			KUiSelfPetFrame::getSingleton().hide();
			KUiDuraAlert::getSingleton().hide();
			KUiChanConfig::getSingleton().hide();
			if (KUiRandomCopyRewards::GetSingleton().IsVisible())
			{
				KUiRandomCopyRewards::GetSingleton().Hide();
			}
			if ( KUiHire::IsVisible() )
			{
				KUiHire::Hide();
			}
			if ( KUiHireConfigExp::IsVisible() )
			{
				KUiHireConfigExp::Hide();
			}
			if ( KUiHireConfigSalary::IsVisible() )
			{
				KUiHireConfigSalary::Hide();
			}
			if ( KUiElfPopMenu::IsVisible() )
			{
				KUiElfPopMenu::Hide();
			}
			if(KUiRecommend::getSingleton().isVisible())
			{
				KUiRecommend::getSingleton().hide();
			}
			if( KUiEntrustComputer::IsVisible() )
			{
				KUiEntrustComputer::GetSingleton().ClosePanel();
			}
			if ( KUiInfoBarPing::IsVisible() )
			{
				KUiInfoBarPing::Hide();
			}
			if ( KUiInfoBarTime::IsVisible() )
			{
				KUiInfoBarTime::Hide();
			}
			if ( KUiQuestionWindow::IsVisible() )
			{
				KUiQuestionWindow::Hide();
			}

			if ( KUiItemPassword::IsVisible() )
			{
				KUiItemPassword::Hide();
			}
			if ( KUiItemPassword_Create::IsVisible() )
			{
				KUiItemPassword_Create::Hide();
			}
			if ( KUiItemPassword_Modify::IsVisible() )
			{
				KUiItemPassword_Modify::Hide();
			}
			if ( KUiSmallBattleFieldResult::IsVisible() )
			{
				KUiSmallBattleFieldResult::Hide();
			}
			if ( KUiTongStatueMsg::IsVisible() )
			{
				KUiTongStatueMsg::Hide();
			}
			if ( KUiGenPersonalInfo::IsVisible())
			{
				KUiGenPersonalInfo::Hide();
			}
			if ( KUiPointListCharts::IsVisible() )
			{
				KUiPointListCharts::Hide();
			}
			//TaisuiSys rule enable
			KTaisuiWnd::Disconnect();
			if ( KUiChanConfig::getSingleton().isVisible() )
			{
				KUiChanConfig::getSingleton().hide();
			}
			if (KUiChatRoomListMenu::IsVisible())
			{
				KUiChatRoomListMenu::Hide();
			}
		}
		break;
	case GAMESPACE_CLICKED:
		{
			if (KUiRoleFacePopMenu::IsVisible())
				KUiRoleFacePopMenu::Hide();
			if (KUiTargeMenu::IsVisible())
				KUiTargeMenu::Hide();
			if (KUiPlayerMenu::IsVisible())
				KUiPlayerMenu::Hide();
			if (KUiPKFilter::IsVisible())
				KUiPKFilter::Hide();
			if (KUiItemTip::IsVisible())
				KUiItemTip::Hide();
			if ( KUiElfPopMenu::IsVisible() )
			{
				KUiElfPopMenu::Hide();
			}
			KUiTeamList::GetSingleton().HideTeamMenu();
			if ( KUiLRSkillWnd::IsVisible()  )
			{
				KUiLRSkillWnd::Hide();
			}
			if (KUiEntrustSkill::IsVisible())
			{
				KUiEntrustSkill::Hide();
			}
			if (KUiChatRoomListMenu::IsVisible())
			{
				KUiChatRoomListMenu::Hide();
			}
		}
		break;
	default:
		break;
	}
}

const Image* getImage(const char* imagePath)
{
	char imageSet[128] = {0};
	char imageName[128] = {0};

	sscanf(imagePath, "set:%127s image:%127s", imageSet, imageName);

	const Image* image;

	try
	{
		if(0 != imageSet[0])
		{
			image = &ImagesetManager::getSingleton().getImageset(imageSet)->getImage(imageName);
		}
		else
		{
			if(ImagesetManager::getSingleton().isImagesetPresent(imagePath) == true)
			{
				image = &ImagesetManager::getSingleton().getImageset(imagePath)->getImage("full_image");
			}
			else
			{
				image = &ImagesetManager::getSingleton().createImagesetFromImageFile(imagePath, imagePath)->getImage("full_image");
			}			
		}
	}
	catch (UnknownObjectException)
	{
		image = NULL;
	}
	
	return image;
}

void deleteImageSet(const char* setName)
{
	if(ImagesetManager::getSingleton().isImagesetPresent(setName))
	{
		Imageset* imgset = ImagesetManager::getSingleton().getImageset(setName);
		ImagesetManager::getSingleton().destroyImageset(imgset);
	}
}

void playSound(const char* soundPath)
{
	char soundSet[128] = {0};
	char soundName[128] = {0};

	sscanf(soundPath, "set:%127s sd:%127s", soundSet, soundName);

	if(0 == soundSet[0])
	{
		return;
	}
	
	CEGUI::SoundSet* st = SoundSetManager::getSingleton().GetSoundSet(soundSet);
	
	if(NULL == st)
	{
		return;
	}

	st->PlayASound(soundName);
}

