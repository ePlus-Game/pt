//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 01/08/2007 14:39
//      File_base        : UiTongManager
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef	UITONGMANAGER_H
#define UITONGMANAGER_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLGameObject.h"
#include "SocialComDef.h"
#include <list>
#include <map>


using namespace CEGUI;

class ListboxTextGUIDItem : public ListboxTextItem
{
public:
	ListboxTextGUIDItem( ) : CEGUI::ListboxTextItem( "", 0, NULL, false, false )
	{
	}
	FSGUID d_id;
};

class KUiTongManager : public KUiWndSingleton<KUiTongManager>
{
	struct	_UiTongPage
	{
		CEGUI::Window*	pPageWnd;
	};

	typedef std::map<int, _UiTongPage> LayerIndex;
	
public:
    KUiTongManager( const CEGUI::String& id_name	);
    ~KUiTongManager(								);

public:
	static void							Show							( void									);
	static void							Hide							( void									);
	static void							UpdateData						( void									);
	static void                         RefreshPage                     ( void                                  );
    void			                    ComInit			                ( void				                	);
public:
	virtual void						onCreate						( UIMDLEvent& rEvent					);
	virtual void						onRelease						( UIMDLEvent& rEvent					);
	virtual void						onChange						( UIMDLEvent& rEvent					);

private:
	void								hideAllPage						( void									);
	void								clearAllPage					( void									);
	SocietyLayerOperationController*	getOperReceive					( const SocietyInfoIndex& tagSocietyIdx );
	bool								handleExit						( const CEGUI::EventArgs& args			);
	bool								handleShowOnline				( const CEGUI::EventArgs& args			);
	bool								handleShowPage					( const CEGUI::EventArgs& args			);
	bool								handleOperation					( const CEGUI::EventArgs& args			);
	bool								handleRequestMemberList			( const CEGUI::EventArgs& args			);
	bool								handleListboxMouseDBClicked		( const CEGUI::EventArgs& args			);
	bool								handleListboxMouseClicked		( const CEGUI::EventArgs& args			);
	bool								handleStaticImageMouseClicked	( const CEGUI::EventArgs& args			);
    void                                RedrawMemberList                ( Window * pPage, TongPageData *  pMemberList );
	void                                RedrawLeagueMemberList          ( Window * pPage, TongPageData *  pMemberList );
	void                                SortMenberList                  ( TongPageData *  pMemberList           );
	bool								handleScrollBar 				( const CEGUI::EventArgs& args			);
	bool                                handlePopup                     ( const CEGUI::EventArgs& args          );
    bool                                handleEditOk                    ( const CEGUI::EventArgs& args          );
	bool                                handleCancel                    ( const CEGUI::EventArgs& args          );
	bool                                handleKeyDown                   ( const CEGUI::EventArgs& args          );

	bool
	staticImage_MouseDoubleClicked( const CEGUI::EventArgs& args );

private:  //Manulist buttong handler
    bool								handleAddFriend 				( const CEGUI::EventArgs& args			);
	bool								handleInvide					( const CEGUI::EventArgs& args			);
	bool								handleChat  					( const CEGUI::EventArgs& args			);
	bool								handleDetail        			( const CEGUI::EventArgs& args			);
	bool								handledChangeOwner	            ( const CEGUI::EventArgs& args			);
	bool								handledKick		                ( const CEGUI::EventArgs& args			);
	bool								handlePreventChat           	( const CEGUI::EventArgs& args			);
    bool                                hideListCheck                   ( const CEGUI::EventArgs& args          );

	Window*								tryGetChild( const Window* pWindow, const String& childName );
private: 
	FSGUID								d_id;                           //当前操作的对象id
	FSGUID                              d_SigleTarget;

	LayerIndex							d_LayerIdx;
	SocietyLayerInfo					d_LayerInfo[MAX_RELATION_LAYER_COUNT + 1];
	SocietyTemplateInfo					d_TemplateInfo;
	bool								d_bProcessDataing;
	//likun
	bool								d_bShowOnline;
	int                                 d_CurLayerId;                       //当前显示的页的层数

	TongPageData                        d_memberList[TONGMEMBER_MAX_NUM];
	int                                 d_CurSubPageNo;                     //当前成员列表的页数

	TongOperParam                       d_TempRedrawTongOper;
	bool                                d_IsTempOperValid;

	bool                                d_IsAllPageEventSubscribed;

	//StaticImageList Infos
	float                               d_elementHight;
	int                                 d_CurCanShowNo;

	//Info editors
	Window *                            d_EditorFrame;
	TongOperParam                       d_TempEditOper;
	int                                 d_CurSelect;

	//RightMouse button manu
	Window *                            d_Manu;
	Window *                            d_AddFriend;
	Window *                            d_Invite;
    Window *                            d_Chat;
	Window *                            d_Detail;
	Window *                            d_ChangeOwner;
	Window *                            d_Kick;
	Window *                            d_PreventChat;

	//Windows needs hide when blank click
	std::list<Window *>                 d_HideList;
};

class KUiTongOperMgr : public KUiWndSingleton<KUiTongOperMgr>
{
public:
   KUiTongOperMgr( const CEGUI::String& id_name	);
    ~KUiTongOperMgr(							);

public:
	static void		Show		( void								) {};
	static void		Show		( const TongOperParam& rOperType ,const bool bShowNotice = false);
	void			Init		( void								);

protected:
	bool			handleExit	( const CEGUI::EventArgs& args		);
	bool			handleOK	( const CEGUI::EventArgs& args		);

private:
	TongOperParam	      d_OperType;
};



//////////////////////////////////////////////////////////////////////////
///					KUiTongStatueMsg
//////////////////////////////////////////////////////////////////////////
class KUiTongStatueMsg : public KUiWndSingleton<KUiTongStatueMsg>
{
public:
	KUiTongStatueMsg( const CEGUI::String& id_name	);
    ~KUiTongStatueMsg();
	
public:
	void ShowStatueMsg( UIStatueInfo* uParam );
	void Init();
	
protected:
	bool btnClose_Clicked( const CEGUI::EventArgs& args );

private:
	void showTime( DWORD dwSeconds );

private:
	TLStaticText* m_pCity;
	TLStaticText* m_pLeague;
	TLStaticText* m_pLeader;
	TLStaticText* m_pTime;
	TLStaticText* m_pReward;
	TLStaticImage* m_pCityIcon;

};


#endif