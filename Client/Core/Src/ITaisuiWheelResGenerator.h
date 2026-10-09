#ifndef I_TAISUI_WHEEL_RES_GENERATOR_H
#define I_TAISUI_WHEEL_RES_GENERATOR_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/23/2007 21:01
//      File_base        : ITaisuiWheelResGenerator
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 产生旋转结果
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

struct ITaisuiWheelResGenerator
{
	virtual ~ITaisuiWheelResGenerator(){}
	virtual  void GenRes(unsigned long * dwTianGanIndex,unsigned long * dwDiZhiIndex,unsigned long dwNextJiaziToActivate=0)=0;
};

ITaisuiWheelResGenerator * GetMainTaisuiWheelResGenerator(void);
void                       ReleaseMainTaisuiWheelResGenerator(void);
#endif