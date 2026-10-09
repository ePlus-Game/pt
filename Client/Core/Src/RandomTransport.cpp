//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-27 11:51
//      File_base        : RandomTransport
//      File_ext         : cpp
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


//////////////////////////////////////////////////////////////////////////
// Include region
#include "KCore.h"
#include "RandomTransport.h"


//////////////////////////////////////////////////////////////////////////
// CRandomTransport class implement region

// CRandomTransport class construct&deconstruct functions

CRandomTransport::CRandomTransport()
{
	for (int randomAreaGroupIndex = 0; randomAreaGroupIndex < MAX_RANDOM_AREA_GROUP_COUNT; randomAreaGroupIndex++)
	{
		m_pTransportPos[randomAreaGroupIndex] = NULL;
		m_nTransportPosNum[randomAreaGroupIndex] = 0;
	}
}

CRandomTransport::~CRandomTransport()
{
	Release();
}

// CRandomTransport class member functions

BOOL CRandomTransport::Init(const char *pcMapName)
{
	Release();

	char szDataFilePath[MAX_PATH] = {0};

	sprintf(szDataFilePath, "%s%s%s%s%s", 
		PATHNAME_MAPTRANSPORTDATA, 
		PATHNAME_SLIP, 
		pcMapName, 
		PATHNAME_SLIP, 
		MAP_POST_FILE);

	FILE *fTransportData = fopen(szDataFilePath, "rb");
	if(fTransportData == NULL)
	{
		return FALSE;
	}

	//读取分组数
	int groupCount = 0;
	fread((void *)&groupCount, sizeof(int), 1, fTransportData);

	if (groupCount > MAX_RANDOM_AREA_GROUP_COUNT)
	{
		fclose(fTransportData);
		return FALSE;
	}
	
	//按分组读取数据
	for (int randomAreaGroupIndex = 0; randomAreaGroupIndex < groupCount; randomAreaGroupIndex++)
	{
		int& posNum = m_nTransportPosNum[randomAreaGroupIndex];

		fread((void *)&posNum, sizeof(int), 1, fTransportData);
		if(posNum <= 0)
		{
			continue;
		}

		if (posNum > MAXCOUNT_TRANSPORT)
		{
			fclose(fTransportData);
			return FALSE;
		}
		
		m_pTransportPos[randomAreaGroupIndex] = new KPostRecord[posNum];
		if(m_pTransportPos[randomAreaGroupIndex] == NULL)
		{
			fclose(fTransportData);
			return FALSE;
		}
		memset((void *)m_pTransportPos[randomAreaGroupIndex], 0, sizeof(KPostRecord) * posNum);
		
		fread((void *)m_pTransportPos[randomAreaGroupIndex], sizeof(KPostRecord), posNum, fTransportData);
	}

	fclose(fTransportData);

	return TRUE;
}

BOOL CRandomTransport::GetRandomTransPos(int &nMapX, int &nMapY, int groupIndex)
{
	if (groupIndex < 0 || groupIndex >= MAX_RANDOM_AREA_GROUP_COUNT)
		return FALSE;

	if(m_pTransportPos[groupIndex] == NULL)
		return FALSE;

	KPostRecord &tagSelPosData = m_pTransportPos[groupIndex][g_Random(m_nTransportPosNum[groupIndex])];

	int nSelWidth = g_Random(tagSelPosData.wRight - tagSelPosData.wLeft);
	int nSelHeigh = g_Random(tagSelPosData.wBottom - tagSelPosData.wTop);

	nMapX = (nSelWidth + tagSelPosData.wLeft) * 16;
	nMapY = (nSelHeigh + tagSelPosData.wTop) * 32;

	return TRUE;
}

BOOL CRandomTransport::IsCanRandomTrans(int groupIndex)
{
	if (groupIndex < 0 || groupIndex >= MAX_RANDOM_AREA_GROUP_COUNT)
		return FALSE;

	return (m_pTransportPos[groupIndex] != NULL && m_nTransportPosNum[groupIndex] > 0);
}

void CRandomTransport::Release()
{	
	for (int randomAreaGroupIndex = 0; randomAreaGroupIndex < MAX_RANDOM_AREA_GROUP_COUNT; randomAreaGroupIndex++)
	{
		if(m_pTransportPos)
		{
			delete[] m_pTransportPos[randomAreaGroupIndex];
			m_pTransportPos[randomAreaGroupIndex] = NULL;
			m_nTransportPosNum[randomAreaGroupIndex] = 0;
		}
	}
}

bool CRandomTransport::CopyInstance(CRandomTransport& instance) const
{
	for (int randomAreaGroupIndex = 0; randomAreaGroupIndex < MAX_RANDOM_AREA_GROUP_COUNT; randomAreaGroupIndex++)
	{
		instance.m_nTransportPosNum[randomAreaGroupIndex] = m_nTransportPosNum[randomAreaGroupIndex];
		if (m_nTransportPosNum[randomAreaGroupIndex] > 0)
		{
			instance.m_pTransportPos[randomAreaGroupIndex] = new KPostRecord[m_nTransportPosNum[randomAreaGroupIndex]];
			if (instance.m_pTransportPos[randomAreaGroupIndex] == NULL || m_pTransportPos[randomAreaGroupIndex] == NULL)
				return false;
			memcpy(instance.m_pTransportPos[randomAreaGroupIndex], m_pTransportPos[randomAreaGroupIndex], sizeof(KPostRecord) * m_nTransportPosNum[randomAreaGroupIndex]);
		}
	}	

	return true;
}