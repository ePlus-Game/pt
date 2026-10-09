//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:18   22:05
//      File_base        : SocialUnitAttr
//      File_ext         : h
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

#ifndef _SocialUnitAttr_h
#define _SocialUnitAttr_h

#include "SocialComDef.h"
#include "SocialUtil.h"

#define  INVALID_SOCIAL_ATTR_VERION 0xffffffff

class SocialUnitAttr
{
public:
	SocialUnitAttr();
	~SocialUnitAttr();

	void	InitAttrs(int nTplId, int nLayer);

	bool	IsAttrExist(int nAttrId) const;
	bool	IsAttrHasData(int nAttrId) const;
	bool	IsAttrSaveToDb(int nAttrId) const;
	int		GetAttr(int nAttrId, char*& pData) const;
	bool	AddAttr(int nAttrId, const char *pData, int nDataSize);
	bool	ChangeAttr(int nAttrId, const char *pNewData, int nDataSize);
	bool	DelAttr(int nAttrId);
	int		SaveAttrToBuf(char *pBuf, int nBufSize);
	bool	LoadAttrFromBuf(const char *pBuf, int nDataSize);

	DWORD   GetAttrVersion(void)const;
	bool    RecheckAttrVersion(int nTplId, int nLayer);

private:
	// Forbid copy operation
	SocialUnitAttr(const SocialUnitAttr &rhs);
	SocialUnitAttr& operator= (const SocialUnitAttr &rhs);

	typedef	struct	_SocialUnit_Attr
	{
		unsigned short int nDataSize;			
		char			   data[1];
				  
	} SOCIALUNIT_ATTR;

	typedef struct _UnitAttr_Info
	{
		char	bExist : 1;
		char	bSaveToDb : 1;
		char	unUsed : 6;

	} UNITATTR_INFO;

private:
	DWORD                m_attrVersion;
	SOCIALUNIT_ATTR		*m_attrData[enSUAttr_Num];	
	UNITATTR_INFO	  	 m_attrInfo[enSUAttr_Num];	
};

inline SocialUnitAttr::SocialUnitAttr()
{
	m_attrVersion = INVALID_SOCIAL_ATTR_VERION;
	
	memset(m_attrData, 0, sizeof(m_attrData));
	memset(m_attrInfo, 0, sizeof(m_attrInfo));
}

inline bool SocialUnitAttr::IsAttrExist(int nAttrId) const
{
	_ASSERT(m_attrVersion == CURRENT_SOCIALDATA_ATTR_VERSIONNO);
	_ASSERT( IsAttrIdValid(nAttrId) );

	if( IsAttrIdValid(nAttrId) )
		return m_attrInfo[nAttrId].bExist ? true : false;

	return false;
}

inline bool	SocialUnitAttr::IsAttrHasData(int nAttrId) const
{
	_ASSERT(m_attrVersion == CURRENT_SOCIALDATA_ATTR_VERSIONNO);
	if( IsAttrExist(nAttrId) )
		return m_attrData[nAttrId] ? true : false;
	else
		return false;
}

inline int	SocialUnitAttr::GetAttr(int nAttrId, char*& pData) const
{
	if( IsAttrHasData(nAttrId) )
	{
		pData = m_attrData[nAttrId]->data;
		return m_attrData[nAttrId]->nDataSize;
	}

	return 0;
}

inline bool	SocialUnitAttr::IsAttrSaveToDb(int nAttrId) const
{
	if( IsAttrExist(nAttrId) )
		return m_attrInfo[nAttrId].bSaveToDb ? true : false;
	else
		return false;
}

inline DWORD SocialUnitAttr::GetAttrVersion()const
{
	return m_attrVersion;
}

#endif