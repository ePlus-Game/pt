//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/22/2007 19:55
//      File_base        : kiteminlayrule
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _KItemInlayRule_h_
#define _KItemInlayRule_h_

#include "KCompoundRule.h"
#include "iiteminlay.h"
#include "KTabFile.h"
#include <string>
#include <map>

struct ItemInlayRule 
{
	ItemInlayRule( void )		
	{
		memset( szInfo, 0, sizeof(szInfo) );
		memset( inlayGroupIDArray, 0, sizeof(inlayGroupIDArray) );
		memset( buffIDArray, 0, sizeof(buffIDArray) );
		memset( DstItem, 0, sizeof(DstItem) );
	}
	char szInfo[MAX_SPECIALEFFECT_NAME_LEN];
	int	inlayGroupIDArray[MAX_INLAY_COUNT];
	int	buffIDArray[MAX_SPECIALEFFECT_COUNT];
	KCompoundRules::KSrcItemDescriptor DstItem[MAX_SPECIALEFFECT_FILTER];
};

typedef std::map<std::string, ItemInlayRule> _ItemInlayRuleMap;

class KItemInlayRule
{
public:
	KItemInlayRule( void ) {};
	~KItemInlayRule( void ) {};
	static KItemInlayRule&	Singleton( void );
public:
	void					Load( 
								void );
	bool					GetInlayEffect( 
								const char* szKey, 
								ItemInlayRule& rRule );
	bool					IsRight(  
								const char* szKey,
								const KItem* item );
private:
	_ItemInlayRuleMap		m_inlayRuleMap;
};

#endif