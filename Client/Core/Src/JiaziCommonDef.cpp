#include "KCore.h"
#include "JiaziCommonDef.h"

typedef struct tagJIA_ZI
{
  unsigned long   m_TianGan;
  unsigned long   m_DiZhi;
}JIA_ZI;

static const JIA_ZI pTXToTD[MAX_JIAZI_NUM]=
{
	{1,1},{2,2},{3,3},{4,4},{5,5},{6,6},{7,7},{8,8},{9,9},{10,10},{1,11},{2,12},
	{3,1},{4,2},{5,3},{6,4},{7,5},{8,6},{9,7},{10,8},{1,9},{2,10},{3,11},{4,12},
	{5,1},{6,2},{7,3},{8,4},{9,5},{10,6},{1,7},{2,8},{3,9},{4,10},{5,11},{6,12},
	{7,1},{8,2},{9,3},{10,4},{1,5},{2,6},{3,7},{4,8},{5,9},{6,10},{7,11},{8,12},
	{9,1},{10,2},{1,3},{2,4},{3,5},{4,6},{5,7},{6,8},{7,9},{8,10},{9,11},{10,12}
};

unsigned long     TianGanDiZhiToTianXiang(const unsigned long dwTianGan,const unsigned long dwDizhi)
{
    unsigned long dwIndex=dwDizhi;
	while (pTXToTD[dwIndex-1].m_TianGan!=dwTianGan && dwIndex<=MAX_JIAZI_NUM)
	{
		dwIndex+=MAX_DIZI_NUM;
	}//endif
	
	if (dwIndex<=MAX_JIAZI_NUM)
		return dwIndex;

	_ASSERT(false);
	return ((unsigned long )-1);
}

void              TianXiangToTianGanDiZhi(const unsigned long dwTianXiang,unsigned long * dwTianGan,unsigned long * dwDiZhi)
{
   if (dwTianGan && dwDiZhi)
   {
       *dwTianGan = pTXToTD[dwTianXiang-1].m_TianGan;
	   *dwDiZhi   = pTXToTD[dwTianXiang-1].m_DiZhi;
   }//endif
} 