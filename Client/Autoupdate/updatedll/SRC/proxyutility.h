//////////////////////////////////////////////////////////////////////////////////////
//  
//  FileName    :   ProxyUtility.cpp
//  Version     :   1.0
//  Creater     :   Linsuyi
//  Date        :   2002-02-07 10:04:39
//  Comment     :   Download proxy utility routines header file
//  
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _UPDATEUTILITY_H
#define _UPDATEUTILITY_H    1

#include "DownloadFile.h"
#include "UpdateExport.h"

int IsCurrentUseProxy();
int PromptProxySetting();

int ValidateProxySetting(KPROXY_SETTING& ProxySetting);


#endif  //#ifndef _UPDATEUTILITY_H

