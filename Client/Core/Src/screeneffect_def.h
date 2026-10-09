//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 10:43
//      File_base        : screeneffect_def
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _screeneffect_def_h
#define  _screeneffect_def_h


#include "KOption.h"
#include "KWavSound.h"
#include "KSoundCache.h"
#include "KSprControl.h"
#include "iRepresentShell.h"

extern struct iRepresentShell*	g_pRepresent;
extern KSoundCache				g_SoundCache;

#define MAX_SCREENEFFECT_COUNT 100
#define SCREENEFFECT_INVALID	-1

#define MAX_SCREENEFFECT_IMAGE 256
#define MAX_SCREENEFFECT_SOUND 256
#define MAX_SCREENEFFECT_TITLENAME 32

#endif
