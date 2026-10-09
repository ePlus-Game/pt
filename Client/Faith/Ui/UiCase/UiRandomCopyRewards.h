//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2008年3月3日
//      File_base        : UiRandomCopyRewards
//      File_ext         : h
//      Author           : CaoLei (曹磊)
//      Description      : 随机副本奖励界面
//
//      <Change_list>	 :
//		1.	2008年3月4日，主要功能完成
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIRANDOMCOPYREWARDS_H__4A4ADD91_D57D_4703_99DB_AD68AB4C08C0__INCLUDED_)
#define AFX_UIRANDOMCOPYREWARDS_H__4A4ADD91_D57D_4703_99DB_AD68AB4C08C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../uicommon.h"
#include "TLGameObject.h"

const int MAX_NUMBERRIC_COUNT = 20;
const int MAX_RANDOMCOPY_REWARD_ITEM = 100;

class KUiRandomCopyRewards : public KUiWndSingleton<KUiRandomCopyRewards>
{
public:
	KUiRandomCopyRewards( const CEGUI::String& id_name );
	virtual ~KUiRandomCopyRewards();

	static void	Show();
	static void	Hide();
	static void Breathe();
	void Init();

	void ShowRewards( KUiInstanceReward& reward );
	//void ShowRewards(  int time, const int monsterCount, const int expRewards, const int yunHunRewards, int id, bool succeed );
	void ShowScore( const CEGUI::String& ImageNamePrefix );

	void BeginPlay();

private:
	void OnImageStop(const int number, const CEGUI::String& ImageNamePrefix, bool& allStopped, bool* stateArray );
	bool handleOKButtonClick( const EventArgs& e );
	
	inline int GetNumberAtPosition( int num, int position);
	inline int NumberToFrame( int num );

private:
	bool m_NeedPlay;
	bool m_ExpStopped;
	bool m_YunHunStopped;
	bool m_ExpImageStopped[MAX_NUMBERRIC_COUNT];
	bool m_YunHunImageStopped[MAX_NUMBERRIC_COUNT];
	KUiInstanceReward m_InstanceReward;
// 	int m_ExpRewards;
// 	int m_YunHunRewards;
	TLGameObject*	m_itemImage[MAX_RANDOMCOPY_REWARD_ITEM];

	int m_numbericCount;
};

#endif // !defined(AFX_UIRANDOMCOPYREWARDS_H__4A4ADD91_D57D_4703_99DB_AD68AB4C08C0__INCLUDED_)
