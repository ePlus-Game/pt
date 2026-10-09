#ifndef JIAZI_COMMON_DEF_H
#define JIAZI_COMMON_DEF_H
//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 07/23/2007 10:05
//      File_base        : JiaziCommonDef
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ¼××Ó¶¨Òå
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#define           MAX_JIAZI_NUM                    60
#define           MAX_TIANGAN_NUM                  10
#define           MAX_DIZI_NUM                     12
#define           MAX_DIZHI_PER_TIANGAN            6

unsigned long     TianGanDiZhiToTianXiang(const unsigned long dwTianGan,const unsigned long dwDizhi);
void              TianXiangToTianGanDiZhi(const unsigned long dwTianXiang,unsigned long * dwTianGan,unsigned long * dwDiZhi); 
/*TianGan=(1~10) Dizhi=(1~12) Jiazi(TianXiang)=(1~60)*/
#endif