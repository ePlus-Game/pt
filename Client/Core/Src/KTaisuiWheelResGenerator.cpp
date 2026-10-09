#include "KCore.h"
#include "KTaisuiWheelResGenerator.h"
#include "ITaisuiWheelSettingMgr.h"
#include "KRandom.h"
#include "JiaziCommonDef.h"

KTaisuiWheelResGenerator::KTaisuiWheelResGenerator()
{
	Init();
}

KTaisuiWheelResGenerator::~KTaisuiWheelResGenerator()
{
	Release();
}

void KTaisuiWheelResGenerator::Init()
{
	//Count the totoal probability area
	unsigned long dwCount=0;
	for (int iIndex=1;iIndex<=MAX_TIANGAN_NUM;iIndex++)
	{
       TIAN_XIANG_PROBABILITY probability=GetMainTaisuiWheelSettingMgr()->GetProbability(iIndex,1);
	   dwCount+=probability.m_TianGan;
	}//end for iIndex
    _ASSERT(dwCount <= MAX_PROBABILITY_SPACE);

	if (dwCount<=MAX_PROBABILITY_SPACE)
		m_TianGanSeeds.m_TotalNum=dwCount;

	unsigned long dwInsertIndex=0;
	//Set the seeds
	for (int iTGIndex=1;iTGIndex<=MAX_TIANGAN_NUM;iTGIndex++)
	{
		TIAN_XIANG_PROBABILITY probability=GetMainTaisuiWheelSettingMgr()->GetProbability(iTGIndex,1);
	    for (int iCount=0;iCount<probability.m_TianGan;iCount++)
		{
			if (dwInsertIndex==m_TianGanSeeds.m_TotalNum)
				 break;

             m_TianGanSeeds.m_Seeds[dwInsertIndex]=iTGIndex;
			 dwInsertIndex++;

		}//end for iCount
	}//end for iTGIndex
}

void KTaisuiWheelResGenerator::GenRes(unsigned long * dwTianGanIndex,unsigned long * dwDiZhiIndex,const unsigned long dwNextJiaziToActivate)
{
     if (dwTianGanIndex && dwDiZhiIndex)
	 {
		   if (*dwTianGanIndex==0)
		   {
			     *dwTianGanIndex = GenTianGan();
		   }//endif
		 
		   *dwDiZhiIndex   = GenDizhi(*dwTianGanIndex,dwNextJiaziToActivate);
		   
	 }//endif
}

unsigned long KTaisuiWheelResGenerator::GenTianGan()const
{
   unsigned long dwRandom=(g_Random(m_TianGanSeeds.m_TotalNum)+UNIX_TMIE_STAMP)%m_TianGanSeeds.m_TotalNum;
   _ASSERT(dwRandom>=0 && dwRandom<m_TianGanSeeds.m_TotalNum);
   return m_TianGanSeeds.m_Seeds[dwRandom];
}

unsigned long KTaisuiWheelResGenerator::GenDizhi(const unsigned long dwTianGan,const unsigned long dwJiaziIndex)
{
	_ASSERT(dwJiaziIndex<=MAX_JIAZI_NUM);

	unsigned long dwNextTianGan=0;
	unsigned long dwNextDizhi=0;

	TianXiangToTianGanDiZhi(dwJiaziIndex,&dwNextTianGan,&dwNextDizhi);
	
	_ASSERT(dwNextDizhi<=MAX_DIZI_NUM && dwNextTianGan<=MAX_TIANGAN_NUM);
	//Degress the next tiangan to activate
	int iCount=0;
	for (int iDizhi=1;iDizhi<=MAX_DIZI_NUM;iDizhi++)
	{
		unsigned long dwDizhiCount = GetMainTaisuiWheelSettingMgr()->GetProbability(dwTianGan,iDizhi).m_DiZhi;
		//Degress
		if (dwNextTianGan == dwTianGan && dwNextDizhi==iDizhi)
		{
			dwDizhiCount =(unsigned long)(dwDizhiCount * GetMainTaisuiWheelSettingMgr()->GetDregress());
			
			//We Never let one dizhi ungetable
			if (dwDizhiCount==0)
				dwDizhiCount=1;
		}//endif

		if (dwTianGan % 2 ==0 && iDizhi % 2 == 1)
		{
			_ASSERT(dwDizhiCount == 0);
			dwDizhiCount = 0;
		}//endif

		if (dwTianGan % 2 ==1 && iDizhi % 2 == 0)
		{
			_ASSERT(dwDizhiCount == 0);
			dwDizhiCount = 0;
		}//endif

		iCount+=dwDizhiCount;
	}//end for iDizhi
	
	_ASSERT(iCount!=0);
	CheckDizhiBufferSize(iCount);

	int  iInsertIndex=0;
	for (int iDizhiIndex=1;iDizhiIndex<=MAX_DIZI_NUM;iDizhiIndex++)
	{
		unsigned long dwDizhiCount = (GetMainTaisuiWheelSettingMgr()->GetProbability(dwTianGan,iDizhiIndex).m_DiZhi);
		//Degress
		if (dwNextTianGan == dwTianGan && dwNextDizhi==iDizhiIndex)
		{
			dwDizhiCount =(unsigned long)(dwDizhiCount * GetMainTaisuiWheelSettingMgr()->GetDregress());
			
			//We Never let one dizhi ungetable
			if (dwDizhiCount==0)
				dwDizhiCount=1;
		}//endif

		if (dwTianGan % 2 ==0 && iDizhiIndex % 2 == 1)
		{
			_ASSERT(dwDizhiCount == 0);
			dwDizhiCount = 0;
		}//endif
		
		if (dwTianGan % 2 ==1 && iDizhiIndex % 2 == 0)
		{
			_ASSERT(dwDizhiCount == 0);
			dwDizhiCount = 0;
		}//endif

		_ASSERT(
			(dwTianGan % 2==0 && iDizhiIndex % 2==1 && dwDizhiCount==0) 
			|| (dwTianGan % 2==1 && iDizhiIndex % 2==0 && dwDizhiCount==0) 
			|| (dwTianGan % 2 ==0 && iDizhiIndex % 2==0) 
			|| (dwTianGan % 2 ==1 && iDizhiIndex % 2==1));

		for (int i=0;i<dwDizhiCount;i++)
		{
			if (iInsertIndex==m_DizhiSeedsBuffer.m_TotalNum)
				break;
			
			m_DizhiSeedsBuffer.m_Seeds[iInsertIndex]=iDizhiIndex;
            iInsertIndex+=1;
		}//end for i

	}//end for iDizhiIndex

	_ASSERT(iInsertIndex == m_DizhiSeedsBuffer.m_TotalNum);

	unsigned long dwRandom=(g_Random(m_DizhiSeedsBuffer.m_TotalNum)+UNIX_TMIE_STAMP)%m_DizhiSeedsBuffer.m_TotalNum;
	return m_DizhiSeedsBuffer.m_Seeds[dwRandom]; 
}

void KTaisuiWheelResGenerator::CheckDizhiBufferSize(const unsigned long dwSize)
{
    if (dwSize<=MAX_PROBABILITY_SPACE)
		m_DizhiSeedsBuffer.m_TotalNum=dwSize;
}

void KTaisuiWheelResGenerator::Release()
{}

ITaisuiWheelResGenerator * GetMainTaisuiWheelResGenerator(void)
{
    static ITaisuiWheelResGenerator * pGenerator=NULL;
	if (pGenerator==NULL)
		pGenerator=new KTaisuiWheelResGenerator;
	return pGenerator;
}

void                       ReleaseMainTaisuiWheelResGenerator(void)
{
    delete GetMainTaisuiWheelResGenerator();
}
