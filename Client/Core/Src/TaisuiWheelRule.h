#ifndef TAISUI_WHEEL_RULE_H
#define TAISUI_WHEEL_RULE_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/20/2007 10:23
//      File_base        : TaisuiWheelRule
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 太岁之轮奖励数据结构
//                         太岁之轮奖励表示形式:
//                         Gift::=GiftSelection+Gift "|" times
//                                | ^
//                         GiftSelection::=(GiftType,GiftNum,GiftInfo)
//                         GiftInfo::=T+
//                         GiftType::=0|1|2|3  (0=invalid)
//                         GiftNum::=T+
//                         times 是选择的数目
//                         ex: (1,1,10234)+(2,1,314)
//      <Change_list>
//////////////////////////////////////////////////////////////////////

//TaisuiWheel Gift declaration..
#define MAX_GIFT_SELECTION 

typedef struct tagTS_GIFT_PARAM
{
	unsigned long dwTianGanBuff;
	unsigned long dwDizhiBuff;
	unsigned long dwAdditionBuff;

}TS_GIFT;

//TaisuiWheel grand
typedef unsigned long TS_GIFT_GRAD;
#define TSG_INVALID    0
#define TSG_NORMAL     1
#define TSG_DAY_SAME   2
#define TSG_MONTH_SAME 3
#define TSG_D_M_SAME   4

#define MAX_GRAD       4
 
#endif

