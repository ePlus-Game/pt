//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/17/2006 17:15
//      File_base        : UiBufferWnd
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef KUIDEBUFFERCENTRE_H 
#define KUIDEBUFFERCENTRE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "TLGameObject.h"
#include <map>

#include "GameDataDef.h"

struct KDebufferPtr 
{
	KDebufferPtr()
	{
		pBufferObect = NULL;
	}
	CEGUI::TLGameObject* pBufferObect;
};

typedef std::map<unsigned int,KDebufferPtr> KDebufferSet;

class KUiDebufferCentre : public KUiWndSingleton<KUiDebufferCentre>
{
public:
	enum CoverType
	{
		cover,		//替换
		wrap,		//累加
		parallel,	//并行
	};
public:
	KUiDebufferCentre		 ( const CEGUI::String& id_name );
	virtual ~KUiDebufferCentre();

public:
	static void			Show( void );
	static void			Hide( void );
	static bool         AddRoleBuffer		( unsigned int uParam, CoverType eCoverType	);
	static bool			DelRoleBuffer		( unsigned int uParam						);
	static bool			DelAllRoleBuffer	( void										);
	void				Init( void );
private:
	static bool			DelBufferByIndex( unsigned int uParam );
	void				InitBufferWnd( void );
	void				InitBufferPosition( void );
	int					FindBufferActive( unsigned int uBufferID );
	void				CoverBuffer(  const KBufferSyncInfo* buffInfo  );
	void				WrapBuffer(  const KBufferSyncInfo* buffInfo  );
	void				ParallelBuffer( const KBufferSyncInfo* buffInfo );
	bool				AddBufferReq( int nBufferID );
	bool				DelBufferReq(int nBufferID );
	bool				onMouseHover( const CEGUI::EventArgs& args );
	bool				onMouseIn( const CEGUI::EventArgs& args );
	bool				onMouseLeave( const CEGUI::EventArgs& args );
private:
	int					m_nActiveBufCount;
	int					m_nMaxBufCount;
	int					m_nMaxBufCountPerRow;
	KDebufferSet		m_bufferSet;
};

#endif KUiDebufferCentre_H