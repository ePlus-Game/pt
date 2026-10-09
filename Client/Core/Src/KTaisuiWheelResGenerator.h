#ifndef K_TAISUI_WHEEL_RES_GENERATOR_H
#define K_TAISUI_WHEEL_RES_GENERATOR_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/24/2007 10:43
//      File_base        : KTaisuiWheelResGenerator
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 旋转结果概率产生器
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "ITaisuiWheelResGenerator.h"

#define   MAX_PROBABILITY_SPACE     12000       //12 * 100 * 10(Degress) 

class KTaisuiWheelResGenerator:public ITaisuiWheelResGenerator
{
	typedef struct tagTianXiangSeeds
	{
		unsigned long     m_TotalNum;
		unsigned long     m_Seeds[MAX_PROBABILITY_SPACE];
		tagTianXiangSeeds(void):m_TotalNum(MAX_PROBABILITY_SPACE){}
	}TIAN_XIANG_SEEDS;

    TIAN_XIANG_SEEDS      m_TianGanSeeds;
    TIAN_XIANG_SEEDS      m_DizhiSeedsBuffer;
public:
	KTaisuiWheelResGenerator(void);
	~KTaisuiWheelResGenerator(void);
public:
	void GenRes(unsigned long * dwTianGanIndex,unsigned long * dwDiZhiIndex,unsigned long dwNextJiaziToActivate=0);
private:
	void Init();
	void Release();
private:
	unsigned long GenTianGan(void)const;
	unsigned long GenDizhi(const unsigned long dwTianGan,const unsigned long dwJiaziIndex);
	void          CheckDizhiBufferSize(const unsigned long dwSize);
};
#endif