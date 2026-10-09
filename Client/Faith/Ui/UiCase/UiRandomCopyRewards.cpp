//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2008年3月3日
//      File_base        : UiRandomCopyRewards
//      File_ext         : cpp
//      Author           : CaoLei (曹磊)
//      Description      : 随机副本奖励界面
//
//      <Change_list>	 :
//		1.	2008年3月4日，主要功能完成
//
//////////////////////////////////////////////////////////////////////

#include "UiRandomCopyRewards.h"
#include <math.h>
#include "CoreShell.h"
#include "UiFSBible_QuestData.h"
#include "UiChangeMapWnd.h"
#include "CEGUIWindow.h"

extern iCoreShell* g_pCoreShell;

const int FrameCountBetweenNumber = 0;

void SecondsToMinute(const int sec, String& result)
{
	int minShow, secShow, hourShow;
	secShow = sec % 60;
	minShow = (int)(sec / 60);
	int oldMinShow = minShow;
	minShow = oldMinShow % 60;
	hourShow = (int)(oldMinShow / 60);

	if (hourShow >0)
	{
		result += iToString(hourShow) + ":";
	}
	if (minShow > 0)
	{
		result += iToString(minShow) + ":";
	}
	result += iToString(secShow);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template<> 
KUiRandomCopyRewards* KUiWndSingleton<KUiRandomCopyRewards>::ms_Singleton = NULL;

KUiRandomCopyRewards::KUiRandomCopyRewards( const CEGUI::String& id_name )
: KUiWndSingleton<KUiRandomCopyRewards>( id_name ),
m_ExpStopped(false),
m_YunHunStopped(false),
m_NeedPlay(false)
{
	for ( int i = 0; i < MAX_NUMBERRIC_COUNT; ++i )
	{
		m_ExpImageStopped[i] = false;
		m_YunHunImageStopped[i] = false;
	}

	for ( int j = 0; j < MAX_QUEST_REWARD_ITEM; ++j )
	{
		m_itemImage[j] = NULL;
	}

	m_numbericCount = 0;
}

KUiRandomCopyRewards::~KUiRandomCopyRewards()
{
	for(int i = 0; i < MAX_QUEST_REWARD_ITEM; ++i)
	{
		if(m_itemImage[i] && m_itemImage[i]->getUserData() != NULL)
		{
			KObjAtContRegion* region = (KObjAtContRegion*)m_itemImage[i]->getUserData();
			delete region;
			region = NULL;
		}
	}
}

void KUiRandomCopyRewards::Show()
{
	KUiWndSingleton<KUiRandomCopyRewards>::Show();
}

void KUiRandomCopyRewards::Hide()
{
	KUiWndSingleton<KUiRandomCopyRewards>::Hide();
}

void KUiRandomCopyRewards::Init()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		m_pThisWnd->getChild("TaharezLook/RandomCopyRewards/Send")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiRandomCopyRewards::handleOKButtonClick, ms_Singleton));
		String rewardCtrlPath = "TaharezLook/RandomCopyRewards";
		char ctrlName[COMMON_CLIENT_MSG_LEN_128];
		for(int k = 0; k < MAX_QUEST_REWARD_ITEM; ++k)
		{
			sprintf(ctrlName, "/Item%d", k + 1);
			m_itemImage[k] = (TLGameObject*)m_pThisWnd->getChild(rewardCtrlPath + ctrlName);
							
			KObjAtContRegion* newObjInfo = new KObjAtContRegion();
			newObjInfo->Obj.uGenre = CGOG_NOTHING;
			m_itemImage[k]->setUserData(newObjInfo);
			m_itemImage[k]->hide();
		}

		m_numbericCount = 0;
		for ( int i = 0; i < MAX_NUMBERRIC_COUNT; i++ )
		{
			String numbericChildName;
			numbericChildName += "TaharezLook/RandomCopyRewards/ExpRewardsImage_" + iToString( i );
			if ( ! ms_Singleton->m_pThisWnd->isChild( numbericChildName ) )
			{
				m_numbericCount = i;
				break;
			}
		}
	}
}

//主要功能实现，弹出窗体并显示奖励
//void KUiRandomCopyRewards::BeginPlay( int time, const int monsterCount, const int expRewards, const int yunHunRewards, int id, bool succeed)
void KUiRandomCopyRewards::BeginPlay()
{
	//弹出主窗体
	KUiWndSingleton<KUiRandomCopyRewards>::Show();

	//播放成功/失败动画
	if (m_InstanceReward.Succeed)
	{
		((StaticImage*)m_pThisWnd->getChild("TaharezLook/RandomCopyRewards/SuccessImage"))->play(true);
	}
 	else
 	{
 		((StaticImage*)m_pThisWnd->getChild("TaharezLook/RandomCopyRewards/FailImage"))->play(true);
 	}

	//显示事件
	String timeShow;
	SecondsToMinute(m_InstanceReward.UseTime, timeShow);
	m_pThisWnd->getChild("TaharezLook/RandomCopyRewards/Time")->setText(timeShow);
	
	//显示杀死怪物数量
	m_pThisWnd->getChild("TaharezLook/RandomCopyRewards/MonsterCount")->setText(iToString(m_InstanceReward.KillNum));
	
	//开始播放经验值和蕴魂的数字动画
// 	m_ExpRewards = expRewards;
// 	m_YunHunRewards = yunHunRewards;
	m_ExpStopped = false;
	m_YunHunStopped = false;
	ShowScore("TaharezLook/RandomCopyRewards/ExpRewardsImage_");
	ShowScore("TaharezLook/RandomCopyRewards/YunHunRewardsImage_");
	
	//给玩家物品奖励
	KReward* reward = KUiFSBibleQuestData::getSingleton().getQuestRewardById(m_InstanceReward.RewardId);
	int rewardImageIndex = 0;
	if(reward)
	{
		for(int k = 0; k < MAX_QUEST_REWARD_ITEM; ++k)
		{
			if(reward->item[k].genre == 0
				&& reward->item[k].detail == 0
				&& reward->item[k].particular == 0
				&& reward->item[k].level == 0)
			{
				continue;
			}
			
			FIND_ITEMINDEX_PARAM itemidx;
			itemidx.nGenre = reward->item[k].genre;
			itemidx.nDetail = reward->item[k].detail;
			itemidx.nParticular = reward->item[k].particular;
			itemidx.nLevel = reward->item[k].level;
			
			KItemInfo itemInfo;	
			g_pCoreShell->GetGameData(GDI_ITEM_INFO_PARTICULAR, (unsigned int)&itemidx, (int)&itemInfo);
			
			TLGameObject::GameObject goInfo;
			goInfo.d_type			= TLGameObject::item;
			goInfo.d_gameobjectSet	= AnsiToUtf8( itemInfo.szImageSet );
			goInfo.d_gameobject		= AnsiToUtf8( itemInfo.szImage );
			goInfo.d_count			= 1;
			m_itemImage[rewardImageIndex]->setObject( goInfo );
			m_itemImage[rewardImageIndex]->setZLevel(Window::SuperTop);
			m_itemImage[rewardImageIndex]->show();
			m_itemImage[rewardImageIndex]->setTooltipText(AnsiToUtf8(itemInfo.szToolTip));
			
			KObjAtContRegion* objInfo = (KObjAtContRegion*)m_itemImage[rewardImageIndex]->getUserData();
			objInfo->Obj.uGenre = CGOG_ICON;
			objInfo->Region.h = itemidx.nGenre;
			objInfo->Region.v = itemidx.nDetail;
			objInfo->Region.Width = itemidx.nParticular;
			objInfo->Region.Height = itemidx.nLevel;
			
			++rewardImageIndex;
		}
		
		//隐藏没有物品的奖励物品栏
 		for (int i = rewardImageIndex; i < MAX_QUEST_REWARD_ITEM; ++i)
 		{
 			m_itemImage[i]->hide();
 		}
	}
}

//与脚本的接口
void KUiRandomCopyRewards::ShowRewards( KUiInstanceReward& reward )
{
	Show();	
	m_InstanceReward = reward;
	m_NeedPlay = true;
	//ShowRewards(reward.UseTime, reward.KillNum, reward.RewardExp, reward.SkillExp, reward.RewardId, reward.Succeed );
}

//开始播放经验值和蕴魂的数字动画
void KUiRandomCopyRewards::ShowScore(const CEGUI::String& ImageNamePrefix )
{
	StaticImage* siTemp;

	for (int k = 0; k < MAX_NUMBERRIC_COUNT; ++k)
	{
		m_ExpImageStopped[k] = false;
		m_YunHunImageStopped[k] = false;
	}

	for (int i = 0; i < m_numbericCount; ++i)
	{
 		
 		siTemp = (StaticImage*)m_pThisWnd->getChild(ImageNamePrefix + iToString(i));
		if (NULL != siTemp)
		{
			siTemp->setCycCount(3);
			siTemp->play();
		}
	}
}

//取得指定数字的个位，十位，百位，千位上的数字
int KUiRandomCopyRewards::GetNumberAtPosition( int num, int position )
{
	if (position > 0 && num >=0)
	{
		int res =  (int)(num / pow(10, position - 1)) % 10;
		return res;
	}
	else
		return -1;
}

//计算指定数字在图片的哪一帧上
int KUiRandomCopyRewards::NumberToFrame( int num )
{
	return (num - 1) * (FrameCountBetweenNumber + 1) + 1;
}

//在这里捕捉经验值和蕴魂动画停止的事件
void KUiRandomCopyRewards::Breathe()
{
	if (!KUiChangeMapWnd::IsVisible() && ms_Singleton->m_NeedPlay)
	{
		ms_Singleton->BeginPlay();
		ms_Singleton->m_NeedPlay = false;
	}
	if (NULL != ms_Singleton)
	{
		ms_Singleton->OnImageStop(ms_Singleton->m_InstanceReward.RewardExp, "TaharezLook/RandomCopyRewards/ExpRewardsImage_", ms_Singleton->m_ExpStopped, ms_Singleton->m_ExpImageStopped);
		ms_Singleton->OnImageStop(ms_Singleton->m_InstanceReward.SkillExp, "TaharezLook/RandomCopyRewards/YunHunRewardsImage_", ms_Singleton->m_YunHunStopped, ms_Singleton->m_YunHunImageStopped);
	}
}

//当经验值或者蕴魂的数字动画结束的时候，停到指定的一个数字上
void KUiRandomCopyRewards::OnImageStop(const int number, const CEGUI::String& ImageNamePrefix, bool& allStopped, bool* stateArray )
{
	if ( !allStopped )
	{
		StaticImage* siTemp;
		int curNum;
		
		for (int i = 0; i < m_numbericCount; ++i)
		{
			curNum = GetNumberAtPosition(number, i + 1);
 			siTemp = (StaticImage*)ms_Singleton->m_pThisWnd->getChild( ImageNamePrefix + iToString( m_numbericCount - 1 - i ) );
			if (( NULL != siTemp ) && ( !stateArray[i] ) &&( !((TLStaticImage*)siTemp)->isPlaying() ) )
			{
				siTemp->play(0, NumberToFrame(curNum));
				stateArray[i] = true;
			}
			allStopped &= stateArray[i];
		}	
	}		
}

//关闭按钮
bool KUiRandomCopyRewards::handleOKButtonClick( const EventArgs& e )
{
	if (NULL != m_pThisWnd)
	{
		if (m_pThisWnd->isVisible())
		{
			m_pThisWnd->hide();
		}
		return true;
	}
	else
		return false;
}