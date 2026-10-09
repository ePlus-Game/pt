//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/09/2007 12:25
//      File_base        : UiRoleHead
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIROLEHEAD_H
#define UIROLEHEAD_H

#include "layoutinterface.h"
#include <map>
#include <string>
#include "../UiCommon.h"

class KUiLayoutUnit
{
public:
	KUiLayoutUnit();
	KUiLayoutUnit(DWORD id);
	~KUiLayoutUnit();
public:
//	void		AddInfo( char* info );
	void        UpDateInfo(char* pText);
	void		SetPosition( int x, int y );
//	void		Render( void );
	void        Show(bool nShow = true);
	void        Redraw();
private:
//	void		Init( void );
	void        Init(DWORD id);
private:
	int			d_nX;
	int			d_nY;
	int			d_width;
	int			d_height;

	TLStaticImage* parentControl;
	TLStaticText*  infoConrtrol;

//	ILayout*	d_layout;
};

class KUiLayoutManager
{
	typedef std::map<DWORD ,KUiLayoutUnit*> _LayoutSet;
public:
	KUiLayoutManager();
	~KUiLayoutManager();

public:
	static KUiLayoutManager& GetSingleton( void );
	void	AddLayoutUnit( DWORD dwID, char* info ,int x = 0,int y = 0);
	void	SetLayoutUnitPosition( DWORD dwID, int x, int y );
	void	DelLayoutUnit( DWORD dwID );
	void	RenderLayoutUnit( DWORD dwID );
	void	Render( void );
	void    Hide(DWORD dwID);
	void    Hide();
protected:
private:
	_LayoutSet d_layoutSet;
};

#endif