# Microsoft Developer Studio Project File - Name="Common" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Common - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Common.mak" CFG="Common - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Common - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Common - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Program/Apotheosize/Share/Common", HSNAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Common - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../../Share/Header" /I "../../Share/Header/Engine" /I "../../Share/Header/Common" /I "../../Share/Header/Net" /I "../../Share/Header/Common/NewRelay" /I "../../Server/Linux" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\Lib\Release	copy release\Common.lib ..\..\Share\Lib\Release\Common.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "../../Share/Header" /I "../../Share/Header/Engine" /I "../../Share/Header/Common" /I "../../Share/Header/Net" /I "../../Share/Header/Common/NewRelay" /I "../../Server/Linux" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D _WIN32_WINNT=0x0400 /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\Lib\Debug	copy debug\Common.lib ..\..\Share\Lib\Debug\Common.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Common - Win32 Release"
# Name "Common - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Buffer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Console.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\CRC32.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Event.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\EventSelect.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Exception.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Int64.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\IOBuffer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\IOCompletionPort.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\KSG_EncodeDecode.cpp

!IF  "$(CFG)" == "Common - Win32 Release"

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\KSocketClient2.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Library.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Macro.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ManualResetEvent.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Mutex.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\NodeList.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Socket.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SocketAddress.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SocketClient.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SocketServer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\UsesWinsock.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Win32Exception.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Console.h
# End Source File
# Begin Source File

SOURCE=.\Socket.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\SystemInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Timer.h
# End Source File
# End Group
# Begin Group "FSLogicCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bzcomfun.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Conc.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Configger.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\ExceptionNew.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\md5c.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\minilzo.c

!IF  "$(CFG)" == "Common - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Common - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PinCrypt.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Share Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Share\Header\Common\Buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Cipher.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Conc.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Configger.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\CRC32.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\CriticalSection.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\DeviceStream.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Event.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\EventSelect.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Exception.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\ExceptionNew.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\IniFile.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\IOBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\IOCompletionPort.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\KSG_EncodeDecode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\KSocketClient2.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Library.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Macro.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\ManualResetEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\NodeList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\OpaqueUserData.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\PackagerEx.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Reporter.h
# End Source File
# Begin Source File

SOURCE=..\Header\Common\SkillInfomation.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\SkillListDef.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\SocketAddress.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\SocketClient.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\SocketServer.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\tstring.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\UsesWinsock.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Utils.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\Common\Win32Exception.h
# End Source File
# End Group
# End Target
# End Project
