//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiElf
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 帮助小精灵
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _KUIELF_H_
#define _KUiELF_H_

#include "../uicommon.h"
#include "GameDataDef.h"

using namespace CEGUI;

#define MAX_FREE_ELF_NUM 3	// 最大空闲时小精灵数量
//#define ELF_INTERVAL 10		// 改变空闲时小精灵的时间间隔

#define ELF_IMAGESET_NAME "xiaojingling_elf"
#define ELF_IMAGE_NAME "xiaojingling_elf_%d"

enum ElfState
{
	ELF_INVALID = -1,
	ELF_FREE1,
	ELF_FREE2,
	ELF_FREE3,
	ELF_TIP_SHOW,
	ELF_SEARCH_SHOW,
	ELF_RESULT_SHOW,
	ELF_DLG_CLOSE,
	ELF_STATE_COUNT,
};

class KUiElfPopMenu : public KUiWndSingleton<KUiElfPopMenu>
{
public:
    KUiElfPopMenu( const CEGUI::String& id_name	);
    ~KUiElfPopMenu(								);
	
	static	void	Show();
	static	void	Hide();
	void			Init();

	void			SetPosition(Point pos);
	int			GetHeight();
	int			GetWidth();
private:
	bool			handleCloseQuery( const EventArgs& e );
	bool			handleCloseSearchHelp( const EventArgs& e );
	bool			handleCloseAll( const EventArgs& e );
	bool			handleShowTask( const EventArgs& e );
};

class KUiElf : public KUiWndSingleton<KUiElf>
{
public:
    KUiElf( const CEGUI::String& id_name	);
    ~KUiElf(								);
	
	static	void	Show();
	static	void	Hide();
	void			Init();

	void			Breathe();
	int				GetState() {return d_state;}
	void			SetState(int state);
	void			SetElfPos(const Point pos);
	void			MoveToRightEdge();

private:
	void			getChild(void);
	void			ChangeElf(int i);

	bool			handleElfPopMenu(const EventArgs& e);

private:
	int				d_state;					// 小精灵状态
	unsigned long	d_elfChangeTime;			// 随机播放空闲动画间隔时间
	
	String			d_elfImageName[ELF_STATE_COUNT];
	StaticImage*	d_pElf;	// 空闲时
	unsigned long	d_RandomHelpTime;

	int				d_elfChangeInterval;
	int				d_randomHelpInterval;
};

class KUiElfBtn : public KUiWndSingleton<KUiElfBtn>
{
public:
	KUiElfBtn(const CEGUI::String& id_name);
	~KUiElfBtn();
	static void Show();
	void	Init();
protected:
	bool    btnOpenElf_MouseClick( const CEGUI::EventArgs& args );
};

#endif