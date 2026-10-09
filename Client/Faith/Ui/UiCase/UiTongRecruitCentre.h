#ifndef K_UI_TONG_RECRUIT_CENTER_H
#define K_UI_TONG_RECRUIT_CENTER_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 12/03/2007 14:47
//      File_base        : UiTongRecruitCentre
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ÕÐÄ¼ÖÐÐÄ
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "SocialComDef.h"

class KUiSocialInfo: public KUiWndSingleton<KUiSocialInfo>
{
	TLStaticText   *    d_NameTitle;
	TLStaticText   *    d_OwnerTitle;
	TLStaticText   *    d_SubNumTitle;
	TLStaticText   *    d_AddtionTitle1;
	TLStaticText   *    d_AddtionTitle2;
	TLStaticText   *    d_AddtionTitle3;

	Window   *    d_Name;
	Window   *    d_Owner;
	Window   *    d_SubNum;
	Window   *    d_Addtion1;
	Window   *    d_Addtion2;
	Window   *    d_Addtion3;

	TLButton       *    d_CloseBtn;

public:
	KUiSocialInfo(const CEGUI::String & id_name);
    void	           Init();
	static void        Updata(const TongInfoData & data);
private:
	bool        OnClose(const CEGUI::EventArgs & args);
	void		updateLeagueInfo( const TongInfoData& data );
};


class KUiTongRecruitCentre : public KUiWndSingleton<KUiTongRecruitCentre>
{
	TLButton     *                      d_Close;
    TLButton     *                      d_ShizuBtn;
	TLButton     *                      d_ZhuhouBtn;
	TLButton     *                      d_PubBtn;
	TLButton     *                      d_DelBtn;
	TLButton     *                      d_JoinBtn;
	TLButton     *                      d_PrePage;
	TLButton     *                      d_NextPage;

	Window       *                      d_ZhuhouPage;
	Window       *                      d_ShizuPage;

	Window       *                      d_OperNotice;
	TLStaticText *                      d_OperTip;
	TLStaticText *                      d_CurPage;
	TLButton     *                      d_OperOK;
	TLButton     *                      d_OperCancel;

	Window       *                      d_Manu;
	TLButton     *                      d_ManuChat;
	TLButton     *                      d_ManuJoin;

	int                                 d_CurLayerID;
	int                                 d_CurPageNo;
	Window       *                      d_CurSelectItem;

	TongOperParam                       d_TongOper;
public:
	KUiTongRecruitCentre (  const CEGUI::String& id_name );
	~KUiTongRecruitCentre( );
public:
	static void							Show							( void									);
	static void							Hide							( void									);
	static void							UpdateData						( const TongRecruitData &    data       );
public:
    void                                Init();	
private:
	bool                                OnClose( const CEGUI::EventArgs& args );
	bool                                OnShizuClicked( const CEGUI::EventArgs & args);
	bool                                OnZhuhouClicked( const CEGUI::EventArgs & args);
	bool                                OnPubClicked ( const CEGUI::EventArgs & args ); 
	bool                                OnDelClicked ( const CEGUI::EventArgs & args );
	bool                                OnReqJoinClicked( const CEGUI::EventArgs & args );
	bool                                OnItemClicked (const CEGUI::EventArgs & args);
	bool                                OnPrePage(const CEGUI::EventArgs & args);
	bool                                OnNextPage(const CEGUI::EventArgs & args);

	bool                                OnOperaOk(const CEGUI::EventArgs & args);
	bool                                OnOperaCancel(const CEGUI::EventArgs & args);

	bool                                OnManuChat(const CEGUI::EventArgs & args );
	bool                                OnManuJoin(const CEGUI::EventArgs & args );

private:
	void                                ShizuClicked();
	void                                ShowShizuPage(const TongRecruitData &    data       );
	void                                ClearShizuPage();

	void                                ZhuhouClicked();
	void                                ShowZhuhouPage(const TongRecruitData & data     );
	void                                ClearZhuhouPage();

	void                                CheckButton(void);
	void                                ShowManu(void);
	
};

#endif