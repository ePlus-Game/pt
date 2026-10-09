#ifndef  K_WAR_INFO_DEF_H
#define K_WAR_INFO_DEF_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 09/03/2007 10:56
//      File_base        : WarInfoDef
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 文件功能描述
//
//      <Change_list>    : Declare the war infomation struct 
//                         used by the WarInfoManage System
//////////////////////////////////////////////////////////////////////

#include "KCore.h"

//Basic infomation struct declaration
typedef struct tagWarInfo
{
	FSGUID invaderGUID;
	int    mapID;
		
	FSGUID defenderGUID;  //Might be 0
	DWORD  warTime;

    int    warState;      //the war state
}FSWarInfo;

//The value declaration for the war state
#define   FS_WAR_STATE_INVALID     0x00000000
#define   FS_WAR_STATE_NOTIFY      0x00000001
#define   FS_WAR_STATE_PROCESS     0x00000002


#endif