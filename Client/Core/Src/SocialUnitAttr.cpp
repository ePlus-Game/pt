//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:18   22:06
//      File_base        : SocialUnitAttr
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "SocialUnitAttr.h"
#include "SocialUtil.h"
#include "CoreRelated.h"
#include "SocialAllocator.h"

SocialUnitAttr::~SocialUnitAttr()
{
	for(int nAttr = 0; nAttr < enSUAttr_Num; ++nAttr)
	{
		if(m_attrInfo[nAttr].bExist && m_attrData[nAttr])
		{
			SocialAllocator::FreeBuf(m_attrData[nAttr], 
									 sizeof(SOCIALUNIT_ATTR) + m_attrData[nAttr]->nDataSize - 1
									);

			m_attrData[nAttr] = NULL;
		}			
	}	
}

void SocialUnitAttr::InitAttrs(int nTplId, int nLayer)
{
	const PRelationLayer pLayer = GetRelationLayer(nTplId, nLayer);

	_ASSERT(pLayer);
	if(NULL == pLayer)
		return;

	_ASSERT(pLayer->AttributeCount < enSUAttr_Num);

	int	nOpeCount = enSUAttr_Num > pLayer->AttributeCount ? pLayer->AttributeCount : enSUAttr_Num;

	for(int nLoop = 0; nLoop < nOpeCount; ++nLoop)
	{
		int		nAttrId = pLayer->Attributes[nLoop].Id;

		_ASSERT( IsAttrIdValid(nAttrId) );

		if( IsAttrIdValid(nAttrId) )
		{
			m_attrInfo[nAttrId].bExist = true;
			m_attrInfo[nAttrId].bSaveToDb = pLayer->Attributes[nLoop].SaveFlag > 0 ? true : false;
		}
	}

	m_attrVersion = CURRENT_SOCIALDATA_ATTR_VERSIONNO;
}

bool SocialUnitAttr::AddAttr(int nAttrId, const char *pData, int nDataSize)
{
	_ASSERT( IsAttrIdValid(nAttrId) );

	if( !IsAttrIdValid(nAttrId) )
		return false;

	if( !m_attrInfo[nAttrId].bExist)
		return false;

	if(m_attrData[nAttrId])
		return false;

	char	*pBuf = (char*)SocialAllocator::AllocBuf(sizeof(SOCIALUNIT_ATTR) + nDataSize - 1);

	if(pBuf)
	{
		SOCIALUNIT_ATTR	*pAttr = (SOCIALUNIT_ATTR*)pBuf;
		pAttr->nDataSize = nDataSize;
		memcpy(pAttr->data, pData, nDataSize);
		m_attrData[nAttrId] = pAttr;

		return true;
	}

	return false;
}

bool SocialUnitAttr::ChangeAttr(int nAttrId, const char *pNewData, int nDataSize)
{
	_ASSERT( IsAttrIdValid(nAttrId) );

	if( !IsAttrIdValid(nAttrId) )
		return false;

	if(!m_attrInfo[nAttrId].bExist)
		return false;

	if(NULL == m_attrData[nAttrId])
		return false;

	if(nDataSize == m_attrData[nAttrId]->nDataSize)
	{
		memcpy(m_attrData[nAttrId]->data, pNewData, nDataSize);
		return true;
	}
	else
	{
		char *pBuf = (char*)SocialAllocator::AllocBuf(sizeof(SOCIALUNIT_ATTR) + nDataSize - 1);

		if(NULL == pBuf)
			return false;

		SocialAllocator::FreeBuf(m_attrData[nAttrId], 
								 sizeof(SOCIALUNIT_ATTR) + m_attrData[nAttrId]->nDataSize - 1
								 );

		SOCIALUNIT_ATTR	*pAttr = (SOCIALUNIT_ATTR*)pBuf;
		pAttr->nDataSize = nDataSize;
		memcpy(pAttr->data, pNewData, nDataSize);
		m_attrData[nAttrId] = pAttr;

		return true;
	}
}

bool SocialUnitAttr::DelAttr(int nAttrId)
{
	if( IsAttrHasData(nAttrId) )
	{
		SocialAllocator::FreeBuf(m_attrData[nAttrId],
			sizeof(SOCIALUNIT_ATTR) + m_attrData[nAttrId]->nDataSize - 1
			);
		m_attrData[nAttrId] = NULL;

		return true;
	}
	else
	{
		return false;
	}
}

int SocialUnitAttr::SaveAttrToBuf(char *pBuf, int nBufSize)
{
	if(NULL == pBuf)
	{
		_ASSERT(false);
		return 0;
	}

	if( nBufSize < sizeof(DB_UNIT_ATTR) )
	{
		_ASSERT(false);
		return 0;
	}

	DB_UNIT_ATTR	*pAttr = (DB_UNIT_ATTR*)pBuf;
	int				nUsedSize = (pAttr->attrData - pBuf);

	DB_ATTRDATA		sizeCalc;
	pAttr->version   = CURRENT_SOCIALDATA_ATTR_VERSIONNO;
	pAttr->attrCount = 0;	

	for(int nLoop = 0; nLoop < enSUAttr_Num; ++nLoop)
	{
		if(m_attrData[nLoop] && m_attrInfo[nLoop].bSaveToDb)
		{
			DB_ATTRDATA	*pTmp = (DB_ATTRDATA*)(pBuf + nUsedSize);
			int			nTmpSize = sizeCalc.size(m_attrData[nLoop]->nDataSize);

			if(nBufSize - nUsedSize < nTmpSize)
			{
				_ASSERT(false);
				break;
			}

			pTmp->attrId = (BYTE)nLoop;
			pTmp->attrSize = m_attrData[nLoop]->nDataSize;
			memcpy(pTmp->attrData, m_attrData[nLoop]->data, pTmp->attrSize);
			++pAttr->attrCount;

			nUsedSize += nTmpSize;
		}
	}

	return nUsedSize;
}

bool SocialUnitAttr::LoadAttrFromBuf(const char *pBuf, int nDataSize)
{
	_ASSERT(pBuf);
	if(NULL == pBuf)
		return false;

	DB_UNIT_ATTR	*pAttr = (DB_UNIT_ATTR*)pBuf;

	if( pAttr->size() != nDataSize )
	{
		_ASSERT(false);
		return false;
	}

	_ASSERT(pAttr->version <= CURRENT_SOCIALDATA_ATTR_VERSIONNO);  // Invalid data version
	if (pAttr->version > CURRENT_SOCIALDATA_ATTR_VERSIONNO)
	{
		return false;  
	}//endif
	
	char			*pAttrData = pAttr->attrData;
	
	for(BYTE loop = 0; loop < pAttr->attrCount; ++loop)
	{
		DB_ATTRDATA	*pData = (DB_ATTRDATA*)pAttrData;
		bool	bRet = AddAttr(pData->attrId, pData->attrData, pData->attrSize);
		
		_ASSERT(bRet);

		pAttrData += pData->size();
	}//end for loop

	m_attrVersion = pAttr->version;

	return true;
}

bool SocialUnitAttr::RecheckAttrVersion(int nTplId, int nLayer)
{
	_ASSERT(m_attrVersion <= CURRENT_SOCIALDATA_ATTR_VERSIONNO);
	if (m_attrVersion < CURRENT_SOCIALDATA_ATTR_VERSIONNO)
	{
		//Do things for version edition......

		//This may be changed in the futher Debug used....
		m_attrVersion = CURRENT_SOCIALDATA_ATTR_VERSIONNO;
	}//endif

	return true;
}