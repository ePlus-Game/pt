//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-27 11:51
//      File_base        : RandomTransport
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : 
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _RANDOMTRANSPORT__H___
#define _RANDOMTRANSPORT__H___

//////////////////////////////////////////////////////////////////////////
// Include region
#include "Scene/SceneDataDef.h"

// Macro define region
#ifdef WIN32
#define PATHNAME_MAPTRANSPORTDATA		"maps"
#define PATHNAME_SLIP					"\\"
#else
#define PATHNAME_MAPTRANSPORTDATA		"maps"
#define PATHNAME_SLIP					"/"
#endif
#define MAXCOUNT_TRANSPORT				102400
#define MAX_RANDOM_AREA_GROUP_COUNT		10

//////////////////////////////////////////////////////////////////////////
// CRandomTransport class define region

class CRandomTransport
{
public:
	CRandomTransport();
	virtual ~CRandomTransport();

	BOOL Init(const char *pcMapName);

	BOOL GetRandomTransPos(int &nMapX, int &nMapY, int groupIndex = 0);
	BOOL IsCanRandomTrans(int groupIndex = 0);
	bool CopyInstance(CRandomTransport& instance) const;

	void Release();
private:
	KPostRecord* m_pTransportPos[MAX_RANDOM_AREA_GROUP_COUNT];
	int m_nTransportPosNum[MAX_RANDOM_AREA_GROUP_COUNT];	
};

// Public inline

// Private inline

#endif // _RANDOMTRANSPORT__H___
