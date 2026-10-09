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

#ifndef KUITARGETBUFFERCENTRE_H 
#define KUITARGETBUFFERCENTRE_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "TLGameObject.h"
#include <map>

#include "GameDataDef.h"

struct KTargetbufferPtr 
{
	KTargetbufferPtr()
	{
		pBufferObect = NULL;
	}
	CEGUI::TLGameObject* pBufferObect;
};

typedef std::map<unsigned int,KTargetbufferPtr> KTargetbufferSet;

class KUiTargetbufferCentre : public KUiWndSingleton<KUiTargetbufferCentre>
{
public:
	enum CoverType
	{
		cover,		//替换
		wrap,		//累加
		parallel,	//并行
	};
public:
	KUiTargetbufferCentre		 ( const CEGUI::String& id_name );
	virtual ~KUiTargetbufferCentre();

public:
	static bool         AddRoleBuffer		( unsigned int uParam, CoverType eCoverType	);
	static bool			DelRoleBuffer		( unsigned int uParam						);
	static bool			DelAllRoleBuffer	( void										);
	void				Init( void );
private:
	static bool			DelBufferByIndex( unsigned int uParam );
	void				InitBufferPosition( void );
	int					FindBufferActive( unsigned int uBufferID );
	void				ParallelBuffer( const KBufferSyncInfo* buffInfo  );
	void				CoverBuffer(  const KBufferSyncInfo* buffInfo  );
	void				WrapBuffer(  const KBufferSyncInfo* buffInfo  );
	bool				AddBufferReq( int nBufferID );
	bool				DelBufferReq(int nBufferID );
private:
	int					m_nActiveBufCount;
	int					m_nMaxBufCount;
	int					m_nMaxBufCountPerRow;
	KTargetbufferSet	m_bufferSet;
};

#endif KUiTargetbufferCentre_H