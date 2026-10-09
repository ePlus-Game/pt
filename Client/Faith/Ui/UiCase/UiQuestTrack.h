 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 03/14/2007
//      File_base        : KQuestTrack
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 任务追踪
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiQuestTrack_H
#define KUiQuestTrack_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "TLStatic.h"

#define UI_QUEST_TRACK_MAX_QUEST_COUNT 5

using namespace CEGUI;

class KUiQuestTrack : public KUiWndSingleton<KUiQuestTrack>
{
	//当前任务跟踪列表
	int		d_trackIds[UI_QUEST_TRACK_MAX_QUEST_COUNT];

	char	d_lomsg[LAYOUT_TEXT_MAX_LEN];
protected:
public:
    KUiQuestTrack( const CEGUI::String& id_name	);
    ~KUiQuestTrack(								);
	void	Init();
	void	freshTrackList();
	void	addTrack(int questId);
	void	ClearTrackList();
};


#endif