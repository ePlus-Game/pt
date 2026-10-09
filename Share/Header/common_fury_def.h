#ifndef K_COMMON_FURY_DEF
#define K_COMMON_FURY_DEF

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 11/06/2007 11:15
//      File_base        : common_fury_def
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : ±¬»ê¹«ÓÃ
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#pragma	pack(push, 1)

enum s2c_fury_sub_protocol
{
    s2c_fury_exp_sync=0,
	s2c_fury_warning,	
};

typedef struct
{
	BYTE             ProtocolType;
	BYTE             SubProtocolType;
	int              nAdditionalParam;
}S2C_FURY_SYNC,*PS2C_FURY_SYNC;

typedef struct 
{
	BYTE             ProtocolType;
}C2S_FURY_EXPLODE,*PC2S_FURY_EXPLODE;

#pragma pack(pop)

#endif