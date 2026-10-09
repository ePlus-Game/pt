# Microsoft Developer Studio Project File - Name="Core_Lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CORE_LIB - WIN32 CLIENT DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Core_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Core_lib.mak" CFG="CORE_LIB - WIN32 CLIENT DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Core_Lib - Win32 Client Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Core_Lib - Win32 Client Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Core_Lib - Win32 Server Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "Core_Lib - Win32 Server Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Program/Apotheosize/Client/Core", FPAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Core___Win32_Client_Debug"
# PROP BASE Intermediate_Dir "Core___Win32_Client_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ClientDebug"
# PROP Intermediate_Dir "ClientDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\engine\src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CORE_EXPORTS" /YX"KCore.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "./Src" /I "../../Share/Header" /I "../../Share/Header/Common" /I "../../Share/Header/Engine" /I "../../Share/Header/Represent" /I "../../Share/Header/Common/NewRelay" /I "../../Share/Header/Net" /I "../../Share/Header/DBWrap" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CLIENT_SCRIPT" /D "_AUTO_ROBOT" /D "CORE_EXPORTS" /Fr /Yu"KCore.h" /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ClientDebug\CoreClient.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\lib\debug	copy ClientDebug\CoreClient.lib ..\..\Share\lib\debug\CoreClient.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Core___Win32_Client_Release"
# PROP BASE Intermediate_Dir "Core___Win32_Client_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ClientRelease"
# PROP Intermediate_Dir "ClientRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CORE_EXPORTS" /D "_SERVER" /YX /FD /c
# ADD CPP /nologo /MD /W2 /GX /O2 /I "./Src" /I "../../Share/Header" /I "../../Share/Header/Common" /I "../../Share/Header/Engine" /I "../../Share/Header/Represent" /I "../../Share/Header/Common/NewRelay" /I "../../Share/Header/Net" /I "../../Share/Header/DBWrap" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CORE_EXPORTS" /D "CLIENT_SCRIPT" /D "_AUTO_ROBOT" /Yu"KCore.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ClientRelease\CoreClient.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\lib\release	copy ClientRelease\CoreClient.lib ..\..\Share\lib\release\CoreClient.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Core_Lib___Win32_Server_Debug"
# PROP BASE Intermediate_Dir "Core_Lib___Win32_Server_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "ServerDebug"
# PROP Intermediate_Dir "ServerDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I ".\\" /I "src" /I "..\engine\src" /I "..\engine\include" /I "..\MultiServer\Common" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "SWORDONLINE_SHOW_DBUG_INFO" /D "CORE_EXPORTS" /Yu"KCore.h" /FD /GZ /c
# SUBTRACT BASE CPP /X /Fr
# ADD CPP /nologo /MDd /W3 /Gm /vmg /GX /Zi /Od /I "./Src" /I "../../Share/Header" /I "../../Share/Header/Common" /I "../../Share/Header/Engine" /I "../../Share/Header/Represent" /I "../../Share/Header/Common/NewRelay" /I "../../Share/Header/Net" /I "../../Share/Header/DBWrap" /I "../../Share/Header/FSEye" /D "_DEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CORE_EXPORTS" /D "GM_CMD" /Fr /Yu"KCore.h" /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"ClientDebug\CoreClient_D.lib"
# ADD LIB32 /nologo /out:"ServerDebug\CoreServer.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\lib\debug	copy ServerDebug\CoreServer.lib ..\..\Share\lib\debug\CoreServer.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Core_Lib___Win32_Server_Release"
# PROP BASE Intermediate_Dir "Core_Lib___Win32_Server_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ServerRelease"
# PROP Intermediate_Dir "ServerRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /MD /W2 /GX /O2 /I ".\\" /I "src" /I "..\engine\src" /I "..\engine\include" /I "..\MultiServer\Common" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CORE_EXPORTS" /Yu"KCore.h" /FD /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MD /W2 /GX /O2 /I "./Src" /I "../../Share/Header" /I "../../Share/Header/Common" /I "../../Share/Header/Engine" /I "../../Share/Header/Represent" /I "../../Share/Header/Common/NewRelay" /I "../../Share/Header/Net" /I "../../Share/Header/DBWrap" /I "../../Share/Header/FSEye" /D "NDEBUG" /D "_SERVER" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_LIB" /D "CORE_EXPORTS" /Yu"KCore.h" /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"ClientRelease\CoreClient.lib"
# ADD LIB32 /nologo /out:"ServerRelease\CoreServer.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\Share\lib\release	copy ServerRelease\CoreServer.lib ..\..\Share\lib\release\CoreServer.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Core_Lib - Win32 Client Debug"
# Name "Core_Lib - Win32 Client Release"
# Name "Core_Lib - Win32 Server Debug"
# Name "Core_Lib - Win32 Server Release"
# Begin Group "GlobalFiles"

# PROP Default_Filter ""
# Begin Group "Interface"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\CoreDrawGameObj.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\CoreDrawGameObj.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\CoreObjGenreDef.h
# End Source File
# Begin Source File

SOURCE=.\src\CoreServerShell.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP BASE Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP BASE Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\CoreServerShell.h
# End Source File
# Begin Source File

SOURCE=.\Src\CoreShell.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\CoreShell.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\CoreUseNameDef.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\GameDataDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\KIndexNode.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\ScriptDataDef.h
# End Source File
# End Group
# Begin Group "ProtocolDef"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KProtocol.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\KProtocol.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\KProtocolDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\KProtocolProcess.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KProtocolProcess.h
# End Source File
# End Group
# Begin Group "GuardProtocol"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\guard_protocol_process.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\guard_protocol_process.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\CoreUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\CoreUtil.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\GlobalDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\KCore.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCore.h
# End Source File
# Begin Source File

SOURCE=.\Src\UiImage.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1
# ADD CPP /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "NpcSystem"

# PROP Default_Filter ""
# Begin Group "NpcRes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KNpcRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcRes.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcResList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcResList.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcResNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcResNode.h
# End Source File
# End Group
# Begin Group "AI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ai_controller.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ai_controller.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ai_player_controller.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ai_player_controller.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ai_threat.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ai_threat.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "DelayedAction"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\action_delayer.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\action_delayer.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\delayed_action.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\delayed_action.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\client_combat_info.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\client_combat_info.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\client_talisman_npc.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\client_talisman_npc.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\exp_manager.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\exp_manager.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KCreature.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCreature.h
# End Source File
# Begin Source File

SOURCE=.\Src\KDirtyNpcSet.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KDirtyNpcSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\KLevelUp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\KLevelUp.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpc.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpc.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcAI.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcAI.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcFindPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcFindPath.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KNpcTemplate.h
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerDBFuns.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\KPlayerDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTask.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTask.h
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTeam_C.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTeam_C.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTeam_S.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTeam_S.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTrade.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KPlayerTrade.h
# End Source File
# Begin Source File

SOURCE=.\Src\KSprControl.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSprControl.h
# End Source File
# Begin Source File

SOURCE=.\Src\npc_save.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\npc_save.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\npc_statistic.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\npc_statistic.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\player_statistic.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\player_statistic.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\PlayerCreator.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\PlayerCreator.h
# End Source File
# End Group
# Begin Group "ItemSystem"

# PROP Default_Filter ""
# Begin Group "ItemCompound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KCompoundRule.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KCompoundRule.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemCompounder.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemCompounder.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemEnchaser.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemEnchaser.h
# End Source File
# End Group
# Begin Group "ArmorSet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ArmorSet_Monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ArmorSet_Monitor.h
# End Source File
# Begin Source File

SOURCE=.\Src\ArmorSet_Table.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ArmorSet_Table.h
# End Source File
# End Group
# Begin Group "Yao"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\Yao_AddOnTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Yao_AddOnTable.h
# End Source File
# Begin Source File

SOURCE=.\Src\Yao_Monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Yao_Monitor.h
# End Source File
# Begin Source File

SOURCE=.\Src\Yao_Table.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Yao_Table.h
# End Source File
# End Group
# Begin Group "Abrade"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\Abrade_Monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Abrade_Monitor.h
# End Source File
# Begin Source File

SOURCE=.\Src\Abrade_Table.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Abrade_Table.h
# End Source File
# End Group
# Begin Group "BasicProperty"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\BasicProperty_Monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\BasicProperty_Monitor.h
# End Source File
# End Group
# Begin Group "Talisman"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\talisman_manager.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\talisman_manager.h
# End Source File
# Begin Source File

SOURCE=.\Src\talisman_monitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\talisman_monitor.h
# End Source File
# End Group
# Begin Group "ItemInlay"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\IItemInlay.h
# End Source File
# Begin Source File

SOURCE=.\Src\kiteminlayaddontable.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\kiteminlayaddontable.h
# End Source File
# Begin Source File

SOURCE=.\Src\kiteminlayrule.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\kiteminlayrule.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemSocket.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemSocketSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemSocketSet.h
# End Source File
# End Group
# Begin Group "Charm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\charm_monitor.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\charm_monitor.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\BaseMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\BaseMonitor.h
# End Source File
# Begin Source File

SOURCE=.\Src\KBasPropTbl.CPP
# End Source File
# Begin Source File

SOURCE=.\Src\KBasPropTbl.h
# End Source File
# Begin Source File

SOURCE=.\Src\KBuySell.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KBuySell.h
# End Source File
# Begin Source File

SOURCE=.\Src\KInventory.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KInventory.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItem.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemChangeRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemChangeRes.h
# End Source File
# Begin Source File

SOURCE=.\Src\kItemdateparser.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\kItemdateparser.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KItemGenerator.CPP
# End Source File
# Begin Source File

SOURCE=.\Src\KItemGenerator.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemList.h
# End Source File
# Begin Source File

SOURCE=.\Src\KItemSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KItemSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\KLinkItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KLinkItem.h
# End Source File
# Begin Source File

SOURCE=.\Src\KObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KObj.h
# End Source File
# Begin Source File

SOURCE=.\Src\KObjSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KObjSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\KSmithShop.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSmithShop.h
# End Source File
# Begin Source File

SOURCE=.\Src\KViewItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KViewItem.h
# End Source File
# Begin Source File

SOURCE=.\Src\MyAssert.H
# End Source File
# End Group
# Begin Group "SkillSystem"

# PROP Default_Filter ""
# Begin Group "Missle"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KMissle.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KMissle.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KMissleRes.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KMissleRes.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KMissleSet.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KMissleSet.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KSkillSpecial.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KSkillSpecial.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "SpecialSkill"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\specialskill_def.h
# End Source File
# Begin Source File

SOURCE=.\Src\specialskill_tab.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\specialskill_tab.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\KSkills.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSkills.h
# End Source File
# Begin Source File

SOURCE=.\Src\NpcSkillList.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\NpcSkillList.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\SkillDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\SkillManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\SkillManager.h
# End Source File
# Begin Source File

SOURCE=.\Src\SkillTargetFilter.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SkillTargetFilter.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "WorldSystem"

# PROP Default_Filter ""
# Begin Group "Scene"

# PROP Default_Filter ""
# Begin Group "SceneTree"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\Scene\KIpotBranch.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1
# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KIpotBranch.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KIpotLeaf.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KIpotLeaf.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KIpoTree.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# ADD CPP /FAs
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1
# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1
# ADD BASE CPP /FAs
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /FAs
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KIpoTree.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "ScrollEffect"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\CoverViewLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\CoverViewLayer.h
# End Source File
# Begin Source File

SOURCE=.\Src\DistanceViewLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\DistanceViewLayer.h
# End Source File
# Begin Source File

SOURCE=.\Src\LinkStructEx.h
# End Source File
# Begin Source File

SOURCE=.\Src\PosterIncise.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\PosterIncise.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\Scene\KScenePlaceC.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KScenePlaceC.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KScenePlaceRegionC.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KScenePlaceRegionC.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KWeather.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\KWeather.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\MapNpcMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Scene\MapNpcMgr.h
# End Source File
# Begin Source File

SOURCE=.\Src\Scene\ObstacleDef.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\SceneDataDef.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\SceneMath.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1
# ADD BASE CPP /Yu
# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\SceneMath.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\ScenePlaceMapC.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\Scene\ScenePlaceMapC.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\KRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KRegion.h
# End Source File
# Begin Source File

SOURCE=.\Src\KSubWorld.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSubWorld.h
# End Source File
# Begin Source File

SOURCE=.\Src\KSubWorldSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSubWorldSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\RandomTransport.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\RandomTransport.h
# End Source File
# End Group
# Begin Group "ScriptSystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KScriptValueSet.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KScriptValueSet.h
# End Source File
# Begin Source File

SOURCE=.\Src\KSortScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSortScript.h
# End Source File
# Begin Source File

SOURCE=.\Src\LuaFuns.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\LuaFuns.h
# End Source File
# Begin Source File

SOURCE=.\Src\ScriptFuns.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ScriptFuns.h
# End Source File
# End Group
# Begin Group "NumericSystem"

# PROP Default_Filter ""
# Begin Group "BaseDamage"

# PROP Default_Filter ""
# End Group
# Begin Group "MagicAttribute"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\MagicAttribute.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MagicAttribute.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MagicDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\ValueAttribute.h
# End Source File
# End Group
# Begin Group "Buff"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\buff_action.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_action.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_alloc.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_def.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_item.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_item.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_list.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_list.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_man.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_man.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\buff_tab.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\buff_tab.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\BaseValue.h
# End Source File
# End Group
# Begin Group "OtherSystem"

# PROP Default_Filter ""
# Begin Group "Friends&Mails"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ChatCenter_C.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatCenter_C.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatCenter_S.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatCenter_S.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ChatCommon.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\ChatDataDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\ChatObjectMgr_C.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatObjectMgr_C.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatObjectMgr_S.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatObjectMgr_S.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatRoomMgr_C.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatRoomMgr_C.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatRoomMgr_S.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ChatRoomMgr_S.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\CoreRelated.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\CoreRelated.h
# End Source File
# Begin Source File

SOURCE=.\Src\MailManager_C.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MailManager_C.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MailManager_S.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MailManager_S.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Simulation"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KSimulation.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KSimulation.h
# End Source File
# End Group
# Begin Group "AntiEnthrall"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\AntiEnthrall.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AntiEnthrall.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "TaisuiWheel"

# PROP Default_Filter ""
# Begin Group "Server"

# PROP Default_Filter ""
# Begin Group "TaisuiWheelSettingMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ITaisuiWheelSettingMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelSetting.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelSettingMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "TaisuiWheelServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KTaisuiWheelServer.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelServer.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "TaisuiWheelTianXiangMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ITaisuiWheelTianXiangMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelTianXiangMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelTianXiangMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "TaisuiWheelResGenerator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ITaisuiWheelResGenerator.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelResGenerator.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelResGenerator.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "TaisuiWheelEventMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ITaisuiWheelEventMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelEventMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheelEventMgr.h
# End Source File
# End Group
# End Group
# Begin Group "Client"

# PROP Default_Filter ""
# Begin Group "TaisuiWheelTianXiang"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\TaisuiWheelTianXiang.h
# End Source File
# End Group
# End Group
# Begin Group "Share"

# PROP Default_Filter ""
# Begin Group "TaisuiWheelSys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ITaisuiWheel.h
# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheel.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KTaisuiWheel.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\JiaziCommonDef.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\JiaziCommonDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\TaisuiSubProtocol.h
# End Source File
# Begin Source File

SOURCE=.\Src\TaisuiWheelRule.h
# End Source File
# End Group
# End Group
# Begin Group "ScreenEffect"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\screeneffect.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\screeneffect.h
# End Source File
# Begin Source File

SOURCE=.\Src\screeneffect_def.h
# End Source File
# Begin Source File

SOURCE=.\Src\screeneffect_man.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\screeneffect_man.h
# End Source File
# Begin Source File

SOURCE=.\Src\screeneffect_tab.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\screeneffect_tab.h
# End Source File
# End Group
# Begin Group "QueryInfo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Share\Header\IQueryInfo.h
# End Source File
# Begin Source File

SOURCE=.\Src\QueryInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\QueryInfo.h
# End Source File
# Begin Source File

SOURCE=.\Src\QueryManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\QueryManager.h
# End Source File
# End Group
# Begin Group "PlayerMonitor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\player_monitor.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\player_monitor.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "TongWarSys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KWarInfoManager.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KWarInfoManager.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\pool_combat_info_mgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\pool_combat_info_mgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\pool_combat_mgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\pool_combat_mgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\tong_war_manager.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\tong_war_manager.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "FuryManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Share\Header\common_fury_def.h
# End Source File
# Begin Source File

SOURCE=.\Src\KClientFuryMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KClientFuryMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KServerFuryMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KServerFuryMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "BannerManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\BannerMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\BannerMgr.h
# End Source File
# End Group
# Begin Group "SocialRecruitMgr"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\social_recruit_svr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\social_recruit_svr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Employ"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Share\Header\EmplomentDataDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\employ.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\employ.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "WorldCombatInstance"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\world_combat_instance.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\world_combat_instance.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Recommender"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\recommender.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\recommender.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "InsuranceSystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\insurance_common.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\insurance_common.h
# End Source File
# Begin Source File

SOURCE=.\Src\insurance_mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\insurance_mgr.h
# End Source File
# End Group
# Begin Group "Question"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\question.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\question.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "ExpInsurance"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\exp_insrance.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\exp_insruance.h
# End Source File
# End Group
# Begin Group "PlusPoint"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\pluspoint.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\pluspoint.h
# End Source File
# End Group
# Begin Group "Title"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\title.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\title.h
# End Source File
# End Group
# Begin Group "KeconomySys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\keconomysys.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\keconomysys.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "KPlayerRealInfoSys"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\client_playerrealinfo_mgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\client_playerrealinfo_mgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\playerrealinfocomdef.h
# End Source File
# Begin Source File

SOURCE=.\Src\server_playerrealinfo_mgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\server_playerrealinfo_mgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# End Group
# Begin Group "OtherFiles"

# PROP Default_Filter ""
# Begin Group "TextFilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\FilterText.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\FilterText.h
# End Source File
# Begin Source File

SOURCE=.\Src\Regexp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\Src\Regexp.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KMath.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMath.h
# End Source File
# End Group
# Begin Group "Option"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KOption.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KOption.h
# End Source File
# End Group
# Begin Group "Music"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\KMapMusic.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KMapMusic.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Src\ConfigManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\ConfigManager.h
# End Source File
# Begin Source File

SOURCE=.\Src\ImgRef.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ImgRef.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\KGMCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\KGMCommand.h
# End Source File
# Begin Source File

SOURCE=.\Src\OnceIBItemMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\stdafx.cpp
# ADD CPP /Yc"KCore.h"
# End Source File
# Begin Source File

SOURCE=.\Src\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\Timer.h
# End Source File
# End Group
# Begin Group "Documents"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\changelist.txt
# End Source File
# End Group
# Begin Group "LogSystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\DbLogDevice.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\DbLogDevice.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\DebugLogDevice.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\DebugLogDevice.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ILogDevice.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ILogSystem.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\LogSystem.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\LogSystem.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Auction"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Share\Header\AuctionComDef.h
# End Source File
# Begin Source File

SOURCE=.\Src\ClientAuctionMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ClientAuctionMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\DBAucDataCenter.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\DBAucDataCenter.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ServerAuctionMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ServerAuctionMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "SocialRelation"

# PROP Default_Filter ""
# Begin Group "Template&Privilege"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\privilege_set.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\privilege_set.h
# End Source File
# Begin Source File

SOURCE=.\Src\relation_template.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\relation_template.h
# End Source File
# End Group
# Begin Group "RelationManage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\client_social_relation.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\client_social_relation.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\relation_set.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\relation_set.h
# End Source File
# End Group
# Begin Group "UnitManage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\ClientSocialUnitMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ClientSocialUnitMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ServerSocialUnitMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\ServerSocialUnitMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialUnit.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialUnit.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialUnitAttr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialUnitAttr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialUtil.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialUtil.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Serializer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\SocialSerializer.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialSerializer.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# End Group
# Begin Group "Allocator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\SocialAllocator.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\SocialAllocator.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Share\Header\SocialComDef.h
# End Source File
# End Group
# Begin Group "AutoRobot"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\AStarPathFinder.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AStarPathFinder.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AutoDialogNpc.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\AutoDialogNpc.h
# End Source File
# Begin Source File

SOURCE=.\Src\AutoPathFinder.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AutoPathFinder.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AutoRobotComDef.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AutoRobotMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\AutoRobotMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MapObstacleMgr.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\MapObstacleMgr.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\RobotSkillPolicy.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\RobotSkillPolicy.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "IBShop"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Src\IBCenter_C.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\IBCenter_C.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\IBCenter_S.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\IBCenter_S.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\IBLog.cpp

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\IBLog.h

!IF  "$(CFG)" == "Core_Lib - Win32 Client Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Client Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Debug"

!ELSEIF  "$(CFG)" == "Core_Lib - Win32 Server Release"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Src\IBMoney.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\IBMoney.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\IBShopComDef.h
# End Source File
# Begin Source File

SOURCE=..\..\Share\Header\IBShopProtocol.h
# End Source File
# Begin Source File

SOURCE=.\Src\IBShopUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Src\IBShopUtil.h
# End Source File
# Begin Source File

SOURCE=.\Src\IMoney.h
# End Source File
# End Group
# End Target
# End Project
