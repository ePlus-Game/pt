
#include "UiQuestTrack.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "UiQuestManage.h"
#include "../UiConfigManager.h"

extern iCoreShell* g_pCoreShell;

using namespace CEGUI;

template<> 
KUiQuestTrack* KUiWndSingleton<KUiQuestTrack>::ms_Singleton	= NULL;

KUiQuestTrack::KUiQuestTrack(const String& id_name):
KUiWndSingleton<KUiQuestTrack>( id_name )
{

}

KUiQuestTrack::~KUiQuestTrack()
{
	
}

void KUiQuestTrack::Init()
{
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		m_pThisWnd->setZLevel(Window::SuperBottom);
		m_pThisWnd->SetBottomWindow();
		m_pThisWnd->setRenderMode(true);
		m_pThisWnd->SetBottomWindow();
		TLStaticText* thisWnd = (TLStaticText*)m_pThisWnd;
		thisWnd->useLayout();
		thisWnd->setLayoutOffset(0, 0);

		for(int i = 0; i < UI_QUEST_TRACK_MAX_QUEST_COUNT; ++i)
		{
			d_trackIds[i] = QUEST_INVALID_ID;
		}
	}
}

void KUiQuestTrack::freshTrackList()
{
	TLStaticText* thisWnd = (TLStaticText*)m_pThisWnd;
	
	const KUiCfgLoader::QuestTrackCfgData& questTrackCfg = KUiCfgLoader::getSingleton().getQuestTrackData();

	sprintf(d_lomsg, "<Layout width=%d>", questTrackCfg.windowWidth);

	char loTempStr[COMMON_CLIENT_MSG_LEN_128];

	for(int i = 0; i < UI_QUEST_TRACK_MAX_QUEST_COUNT; ++i)
	{
		if(QUEST_INVALID_ID == d_trackIds[i])
		{
			continue;
		}
		KQuestInfo questInfo;
		questInfo.id = QUEST_INVALID_ID;
		g_pCoreShell->GetGameData( GDI_GET_QUEST_INFO, (UINT)&questInfo, d_trackIds[i]);
		if(questInfo.id == QUEST_INVALID_ID)
		{
			d_trackIds[i] = QUEST_INVALID_ID;
			continue;
		}
		
		sprintf(loTempStr, "<Seg float=wrap><Obj color=%s font-family=%s>", 
			questTrackCfg.questNameColor, questTrackCfg.questNameFont);
		//显示任务名称
		strcat(d_lomsg, loTempStr);
		strcat(d_lomsg, questInfo.name);
		strcat(d_lomsg, "</Obj></Seg>");
		//显示完成情况
		for(int j = 0; j < max_objective_type; ++j)
		{
			for(int k = 0; k < MAX_OBJECTIVE; ++k)
			{
				KQuestInfo::ObjectiveInfo& npcRequire = questInfo.requirement[j][k];
				
				if(npcRequire.uID == QUEST_INVALID_ID)
				{
					continue;
				}
				if(npcRequire.uCount > npcRequire.uProcess)
				{
					
					sprintf(loTempStr, "<Seg float=wrap><Obj color=%s font-family=%s>", 
						questTrackCfg.IncompleteColor, questTrackCfg.IncompleteFont);
					strcat(d_lomsg, loTempStr);
				}
				else
				{
					sprintf(loTempStr, "<Seg float=wrap><Obj color=%s font-family=%s>", 
						questTrackCfg.CompleteColor, questTrackCfg.CompleteFont);
					strcat(d_lomsg, loTempStr);
				}
				sprintf(loTempStr, "\t%s", npcRequire.name);
				strcat(d_lomsg, loTempStr);
				strcat(d_lomsg, "</Obj></Seg>");
			}
		}
	}
	strcat(d_lomsg, "</Layout>");
	
	thisWnd->getLayout()->formatText(d_lomsg);
	thisWnd->getLayout()->SetText(d_lomsg);
	thisWnd->getLayout()->flashLayout();
	thisWnd->fitLayoutSize();
	
	Rect clipArea = thisWnd->getUnclippedInnerRect();
	LORect clipper;
	cerectToLorect(&clipArea, &clipper);
	thisWnd->getLayout()->setClipper(clipper);
}

void KUiQuestTrack::addTrack(int questId)
{
	Show();
	bool inList = false;
	int _iPos = 0;
	int _iQuestNum = 0;

	//看看当前列表中是否有该任务
	for(int i = 0; i < UI_QUEST_TRACK_MAX_QUEST_COUNT; ++i)
	{
		if(questId == d_trackIds[i])
		{
			d_trackIds[i] = QUEST_INVALID_ID;
			inList = true;
			_iPos = i;
			break;
		}
		if (QUEST_INVALID_ID != d_trackIds[i])
		{
			_iQuestNum++;
		}
	}

	//如果当前列表中没有该任务，则在跟踪列表中加入
	if(false == inList)
	{
		if (UI_QUEST_TRACK_MAX_QUEST_COUNT == _iQuestNum)
		{
			d_trackIds[UI_QUEST_TRACK_MAX_QUEST_COUNT - 1] = QUEST_INVALID_ID;
			for (int i = UI_QUEST_TRACK_MAX_QUEST_COUNT - 1; i > 0; i--)
			{
				d_trackIds[i] = d_trackIds[i - 1];
			}
			d_trackIds[0] = questId;
		}
		else
		{
			for(int j = UI_QUEST_TRACK_MAX_QUEST_COUNT - 1; j > -1; j--)
			{
				if(QUEST_INVALID_ID == d_trackIds[j])
				{
					d_trackIds[j] = questId;
					break;
				}
			}
		}
	}
	else	//否则在当前列表中删除该任务
	{
		int show = false;
		for(int j = 0; j < UI_QUEST_TRACK_MAX_QUEST_COUNT; ++j)
		{
			if(QUEST_INVALID_ID != d_trackIds[j])
			{
				show = true;
			}
		}
		for (int i =  _iPos; i > 0; i--)
		{
			d_trackIds[i] = d_trackIds[i - 1];
		}
		d_trackIds[0] = QUEST_INVALID_ID;
		if(!show)
		{
			Hide();
			return;
		}
	}

	freshTrackList();
}

void KUiQuestTrack::ClearTrackList()
{
	for (int i = 0; i < UI_QUEST_TRACK_MAX_QUEST_COUNT; i++)
	{
		d_trackIds[i] = QUEST_INVALID_ID;
	}
}