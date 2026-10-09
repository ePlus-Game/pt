//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 11/03/2006 1:02
//      File_base        : KItemEnchaser
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 只在服务器上有效
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _ITEM_ENCHASER_
#define _ITEM_ENCHASER_

#ifdef _SERVER

#include "KItemCompounder.h"

/*!
\brief
	装备升级器类	
*/
class KItemEnchaser:public KItemCompounder
{
public:	
	KItemEnchaser( void )	{ ZeroMemory(&m_TargetItem, sizeof(TCompoundResult));m_bOk0 = false; m_bOk1 = false;}
	~KItemEnchaser( void )	{}
public:	
	int		Compund( TCompoundParams* pCompundParams	);
	int		smith	( TCompoundParams * pCompundParams, int ruleId						);
	void	doSmith	(TCompoundResultEx* smithResult);
	void	notifyClient(int playerIndex, int compoundType, int result);
private:
	void	LevelUp	( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	);
	void	LevelDown( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult );
	void	AddMagic( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	);
	void	Clear	( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	);
	void	AddYao	( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	);
	void	GetYao	( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	);
	void	Make	( TCompoundParams* pCompundParams, TCompoundResult* pCompundResult	);
private:
	TCompoundResult	m_TargetItem;
	bool			m_bOk0;
	bool			m_bOk1;
};

/*!
\brief
	装备合成器，只在服务器上有效。
*/
extern KItemEnchaser g_ItemEnchaser;

#endif

#endif