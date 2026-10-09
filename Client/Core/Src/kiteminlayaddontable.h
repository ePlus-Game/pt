//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 19:44
//      File_base        : kiteminlayaddontable
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _INLAY_ADD_ON_TABLE_H_
#define _INLAY_ADD_ON_TABLE_H_

#include <vector>
#include <map>
#include <string>

struct InlayAddOn
{
	InlayAddOn( void )
	{
		Probability	= 0;
		BuffID		= 0;
		GroupID		= 0;
		YangRate	= 0;
		YinRate		= 0;
	}
	int Probability;	//概率
	int BuffID;			//附加效果编号
	short GroupID;		//分组
	short YangRate;		//阳爻概率
	short YinRate;		//阴爻概率
};

typedef std::vector<InlayAddOn> InlayAddOnArray;

typedef std::map<short,InlayAddOnArray> InlayAddOnMap;

typedef std::map<std::string, InlayAddOnArray> InlayAddOnMapPlus;

class KItemInlayAddOnTable
{
public:
	KItemInlayAddOnTable( void );
	~KItemInlayAddOnTable( void );
	static KItemInlayAddOnTable& Singleton();
public:
	bool				Load( void );
	const InlayAddOn*	GetInlayAddOnByGroup( 
							int group,
							int index );
	const InlayAddOn*	GetInlayAddOnByGroupAndBuffID( 
							int group,
							short buffID,
							int index );
	inline int			GetInlayAddOnCount(
							int group );
private:
	InlayAddOnMap		m_InlayAddOns;
	InlayAddOnMapPlus	m_InlayAddOnsPlus;
};

inline int KItemInlayAddOnTable::GetInlayAddOnCount( int group )
{
	InlayAddOnMap::iterator it = m_InlayAddOns.find( group );
	if ( it != m_InlayAddOns.end() )
	{
		return it->second.size();
	}
	return 0;
};

#endif