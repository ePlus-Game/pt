# Microsoft Developer Studio Project File - Name="Engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Engine - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Engine.mak" CFG="Engine - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Engine - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Engine - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Program/Apotheosize/Client/Engine", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./Include" /I "../../Share/Header/Engine" /I "../../Share/Header/dbwrap" /D "_USENEWRANDOMFUNC" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /YX"KWin32.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib ddraw.lib dsound.lib dxguid.lib winmm.lib wsock32.lib dinput.lib JpgLib.lib LuaLib.lib mp3lib.lib DBWrap.lib netmod.lib common.lib /nologo /dll /map /debug /machine:I386 /out:"Release/Game.dll" /libpath:"..\..\Share\Lib\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\lib\release	md ..\..\bin\server\release	md ..\..\bin\client\release	copy release\Game.lib ..\..\Share\lib\release\Game.lib	copy release\Game.dll ..\..\bin\server\release\Game.dll	copy release\Game.dll ..\..\bin\client\release\Game.dll	copy release\Game.pdb ..\..\bin\client\release\Game.pdb	copy release\Game.map ..\..\bin\client\release\Game.map
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "./Include" /I "../../Share/Header/Engine" /I "../../Share/Header/dbwrap" /D "_USENEWRANDOMFUNC" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ENGINE_EXPORTS" /D WINVER=0x0500 /Fr /Yu"KWin32.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib ddraw.lib dsound.lib dxguid.lib winmm.lib wsock32.lib dinput.lib JpgLib.lib LuaLib.lib mp3lib.lib common.lib DBWrap.lib netmod.lib /nologo /dll /debug /machine:I386 /out:"Debug/Game.dll" /pdbtype:sept /libpath:"..\..\Share\Lib\Debug"
# SUBTRACT LINK32 /pdb:none /incremental:no /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\lib\debug	md ..\..\bin\server\debug	md ..\..\bin\client\debug	copy Debug\Game.lib ..\..\Share\lib\debug\Game.lib	copy Debug\Game.dll ..\..\bin\server\debug\Game.dll	copy Debug\Game.dll ..\..\bin\client\debug\Game.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Engine - Win32 Release"
# Name "Engine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\Src\DrawSpriteMP.inc
# End Source File
# Begin Source File

SOURCE=.\Src\KAutoMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KAviFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBitmap16.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBitmapConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBmp2Spr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBmpFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBmpFile24.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCanvas.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCodecLzo.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KColors.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDError.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDInput.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawBitmap16.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawFade.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDrawSpriteAlpha.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KDSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KEicScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KEicScriptSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KFileCopy.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KFilePath.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KFindBinTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KGifFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KGraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KHashList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KHashNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KHashTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Kime.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KIniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KJpgFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KLinkArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KLuaScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KLuaScriptSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KLubCmpl_Blocker.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMemBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMemClass.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMemClass1.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMemStack.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMouse.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMp3Music.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMp4Audio.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMp4Movie.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMp4Video.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMpgMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMsgNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMutex.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KOctree.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KOctreeNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPakData.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPakFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPakList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPakTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPcxFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPolyRelation.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSafeList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KScanDir.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KScriptCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KScriptList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KScriptSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSG_MD5_String.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSG_StringProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSortBinTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSortList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSoundCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSprite.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSpriteCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSpriteCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSpriteMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KStepLuaScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KStrBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KStrList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KStrNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KTabFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KTabFileCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KTgaFile32.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KThread.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KVideo.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWavCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWavFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWavMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWavSound.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWin32.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWin32App.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWin32Frame.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KWin32Wnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KZipCodec.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KZipData.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KZipFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KZipList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\stdafx.cpp
# ADD CPP /Yc"kwin32.h"
# End Source File
# Begin Source File

SOURCE=.\Src\XPackFile.cpp
# End Source File
# End Group
# Begin Group "Library Header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\Include\JpgLib.h
# End Source File
# Begin Source File

SOURCE=.\Include\LhaLib.h
# End Source File
# Begin Source File

SOURCE=.\Include\mp3lib.h
# End Source File
# End Group
# Begin Group "Encode&Decode"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\Cryptography\EDOneTimePad.cpp
# End Source File
# End Group
# Begin Group "Text Process"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\Text.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "Ucl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ucl\alloc.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\fake16.h
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\getbit.h
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\internal.h
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\io.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2_99.ch
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2b_99.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2b_d.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2b_ds.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2b_to.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2d_99.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2d_d.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2d_ds.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2d_to.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2e_99.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2e_d.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2e_ds.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\n2e_to.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Include\ucl\ucl.h
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_conf.h
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_crc.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_dll.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_init.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_mchw.ch
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_ptr.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_ptr.h
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_str.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_swd.ch
# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_util.c

!IF  "$(CFG)" == "Engine - Win32 Release"

# SUBTRACT CPP /YX

!ELSEIF  "$(CFG)" == "Engine - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ucl\ucl_util.h
# End Source File
# Begin Source File

SOURCE=.\Include\ucl\uclconf.h
# End Source File
# Begin Source File

SOURCE=.\Include\ucl\uclutil.h
# End Source File
# End Group
# Begin Group "Documents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\changelist.txt
# End Source File
# End Group
# Begin Group "Share Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KAutoMutex.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KAviFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBinsTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBinTreeNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBitmap16.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBitmapConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBmp2Spr.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBmpFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KBmpFile24.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KCache.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KCanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KCodecLzo.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KColors.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KCriticalSection.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDDraw.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDebug.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDError.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDInput.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawBase.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawBitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawBitmap16.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawFade.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawFont.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawSprite.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDrawSpriteAlpha.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KDSound.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KEicScript.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KEicScriptSet.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KEngine.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KFileCopy.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KFileDialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KFilePath.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KFindBinTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KFont.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KGifFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KGraphics.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KHashList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KHashNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KHashTable.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\Kime.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KIniFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KITabFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KJpgFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KKeyboard.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KLinkArray.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KLuaScript.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KLuaScriptSet.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KLubCmpl_Blocker.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMemBase.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMemClass.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMemClass1.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMemManager.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMemStack.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMessage.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMouse.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMp3Music.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMp4Audio.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMp4Movie.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMp4Video.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMpgMusic.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMsgNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMusic.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KMutex.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KOctree.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KOctreeNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPakData.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPakFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPakList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPakTool.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPalette.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPcxFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPolygon.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KPolyRelation.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KRandom.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSafeList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KScanDir.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KScript.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KScriptCache.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KScriptList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KScriptSet.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSG_MD5_String.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSG_StringProcess.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSortBinTree.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSortList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSoundCache.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSprite.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSpriteCache.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSpriteCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KSpriteMaker.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KStepLuaScript.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KStrBase.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KStrList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KStrNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KTabFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KTabFileCtrl.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KTgaFile32.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KThread.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KTimer.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KVideo.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWavCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWavFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWavMusic.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWavSound.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWin32.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWin32App.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KWin32Wnd.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KZipCodec.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KZipData.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KZipFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\KZipList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\LinkStruct.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\LuaLib.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\md5c.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\Text.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Engine\XPackFile.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Engine.rc
# End Source File
# End Group
# End Target
# End Project
