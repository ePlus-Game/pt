#ifndef __KSKILLSPECIAL_H__
#define __KSKILLSPECIAL_H__
/*******************************************************************************
// FileName			:	KSkillSpecial.h
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-9-23 20:49:13
// FileDescription	:	
// Revision Count	:	
*******************************************************************************/
#ifndef _SERVER
#include "KMissleRes.h"

// lixuewu 没必要包一层Node,直接从Node派生就好了

class KSkillSpecialNode :public KNode
{
	//Class Member
public:
	int			m_nRegionId;		
	int			m_nCurrentMapX;			//	当前的X坐标
	int			m_nCurrentMapY;			//	当前的Y坐标
	int			m_nCurrentMapZ;			//	当前的Z坐标
	int			m_nXOffset;				//	当前的X方向偏移
	int			m_nYOffset;				//	当前的Y方向偏移
	int			m_nPX;
	int			m_nPY;
	int			m_nPZ;
	int			m_nCurDir;
	int			m_nNpcIndex;
	DWORD		m_dwMatchID;
	TMissleRes  * m_pMissleRes;
	DWORD		m_nBeginTime;
	DWORD		m_nEndTime;
	BOOL		m_bFollowMissle;
	int			m_eStatus;
	
#ifdef TOOLVERSION	
	KCacheNode	* m_pSprNode ;
#endif
	KCacheNode  * m_pSndNode ;
	KRUImage	m_RUImage;
	//Class Function
public:
	KSkillSpecialNode();
	BOOL		Init();
	void		Draw(int nCurLifeFrame);
	void		Draw(int nX, int nY , int nZ, int nDir, int nAllFrame,  int nCurLifeFrame);
	void		SetAnimation(char * szFile, unsigned int uImage);
private:
	
};

//class KSkillSpecialNode : public KNode 
//{
//public:
//	KSkillSpecial * m_pSkillSpecial;
//	KSkillSpecialNode(){m_pSkillSpecial = NULL;};
//	~KSkillSpecialNode(){delete m_pSkillSpecial;};
//};
//extern KList SpecialMovieList;
#endif
#endif //__KSKILLSPECIAL_H__
