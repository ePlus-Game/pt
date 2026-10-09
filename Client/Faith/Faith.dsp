# Microsoft Developer Studio Project File - Name="Faith" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Faith - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Faith.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Faith.mak" CFG="Faith - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Faith - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Faith - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Faith - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O1 /I "./" /I "../../Share/Header" /I "../../Share/Header/Common" /I "../../Share/Header/Common/NewRelay" /I "../../Share/Header/Engine" /I "../../Share/Header/Represent" /I "../../Share/Header/Net" /I "../../share/header/DBWrap" /I "../../share/header/cegui" /I "./ui/UiElem/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "SWORDONLINE_USE_MD5_PASSWORD" /YX"Kwin23.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 Wininet.lib FSInterface.lib Ws2_32.lib netmod.lib Game.lib Winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LuaLib.lib CoreClient.lib Common.lib ComCtl32.lib DBWrap.lib layout.lib libboost_regex-vc6-mt-1_32.lib Shlwapi.lib comsupp.lib atl.lib /nologo /subsystem:windows /map /debug /machine:I386 /out:"Release/FSOnline2.exe" /libpath:"../../Share/lib/Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\FSOnline2.exe ..\..\bin\client\release\FSOnline2.exe	copy release\FSOnline2.pdb ..\..\bin\client\release\FSOnline2.pdb	copy release\FSOnline2.map ..\..\bin\client\release\FSOnline2.map
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Faith - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Z7 /Od /I "./" /I "../../Share/Header" /I "../../Share/Header/Common" /I "../../Share/Header/Common/NewRelay" /I "../../Share/Header/Engine" /I "../../Share/Header/Represent" /I "../../Share/Header/Net" /I "../../share/header/DBWrap" /I "../../share/header/cegui" /I "./ui/UiElem/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "SWORDONLINE_SHOW_DBUG_INFO" /D "SWORDONLINE_USE_MD5_PASSWORD" /D "_MBCS" /D "_OLDUI" /FR /YX"kwin32.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Wininet.lib FSInterface.lib Ws2_32.lib netmod.lib Game.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Winmm.lib shlwapi.lib LuaLib.lib CoreClient.lib Common.lib ComCtl32.lib DBWrap.lib layout.lib libboost_regex-vc6-mt-gd-1_32.lib Shlwapi.lib comctl32.lib comsupp.lib atl.lib /nologo /stack:0x4000000 /subsystem:windows /debug /machine:I386 /out:"Debug/FSOnline2.exe" /pdbtype:sept /libpath:"../../Share/lib/Debug"
# SUBTRACT LINK32 /incremental:no
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\FSOnline2.exe ..\..\bin\client\debug\FSOnline2.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Faith - Win32 Release"
# Name "Faith - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Faith.rc
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Group "UiCase"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ui\UiCase\ShopImpBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\ShopImpBase.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiAutoConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiAutoConnect.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBattleResult.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBattleResult.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBeginHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBeginHelp.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBubble.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBubble.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBufferWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiBufferWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCastBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCastBar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChangeMapWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChangeMapWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChatCentre.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChatCentre.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChatConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChatConfig.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChatWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiChatWindow.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCityManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCityManager.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCommonGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCommonGrid.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiComMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiComMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCompound.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCompound.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCreditShop.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiCreditShop.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDeathBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDeathBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDebufferWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDebufferWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDelayQuit.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDelayQuit.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDelComfirm.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDelComfirm.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDragItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDragItem.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDuraAlert.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiDuraAlert.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiElf.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiElf.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiEntrustComputer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiEntrustComputer.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiEntrustSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiEntrustSkill.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiEquipment.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiEquipment.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiErrorMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiErrorMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiESCDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiESCDlg.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFailTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFailTip.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible_QuestData.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible_QuestData.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible_SpecialQuestData.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible_SpecialQuestData.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible_SpecialQuestItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFSBible_SpecialQuestItem.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFuryBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiFuryBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGameSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGameSetting.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGameSpace.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGameSpace.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGenPersonalInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGenPersonalInfo.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGMCommunication.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiGMCommunication.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiHelpCentre.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiHelpCentre.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiHelpInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiHelpInfo.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiHire.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiHire.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiIBShop.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiIBShop.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiIEWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiIEWindow.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiInfoBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiInfoBar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiInsurance.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiInsurance.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemLockMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemLockMgr.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemOperPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemOperPanel.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemPassword.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemPassword.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiItemTip.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLevelUp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLevelUp.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLevelUpInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLevelUpInfo.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLinkedItemTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLinkedItemTip.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLogin.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLoginBg.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiLoginBg.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMailCentre.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMailCentre.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMapCentre.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMapCentre.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMovieFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiMovieFrame.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiNewPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiNewPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiNpcMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiNpcMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiNpcNavigation.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiNpcNavigation.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPathHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPathHelp.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPetFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPetFrame.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPKFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPKFilter.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPlayerMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPlayerMenu.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPlayerState.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPlayerState.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPointListCharts.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPointListCharts.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPopMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiPopMessage.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQueryWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQueryWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQuestionWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQuestionWindow.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQuestManage.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQuestManage.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQuestTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiQuestTrack.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRaid.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRaid.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRandomCopyRewards.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRandomCopyRewards.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRankButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRankButton.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRecommend.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRecommend.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleExp.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleExp.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleFace.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleFace.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleFacePopMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleFacePopMenu.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleHead.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiRoleHead.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiScrollPanelMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiScrollPanelMgr.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSearchHelpWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSearchHelpWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSelPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSelPlayer.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiServerList.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShizuBanner.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShizuBanner.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShop.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShop.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShortcutKeySetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShortcutKeySetting.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShortcutPlusWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShortcutPlusWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShortcutWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiShortcutWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSmith.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSmith.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSplitItemBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSplitItemBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiStoreBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiStoreBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiStudySkillManage.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiStudySkillManage.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSwichScene.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSwichScene.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSystemMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiSystemMessage.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTaisuiWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTaisuiWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTalisman.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTalisman.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetbufferWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetbufferWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetEquipment.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetEquipment.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetFace.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetFace.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTargetMenu.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTeamList.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTeamList.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTeamViewer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTeamViewer.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTimer.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTimer.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTipGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTipGenerator.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTongCreate.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTongCreate.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTongManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTongManager.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTongRecruitCentre.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTongRecruitCentre.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiToolsControlBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiToolsControlBar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTopMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTopMessage.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTradeBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTradeBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTradeConfirmBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTradeConfirmBox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTrafficLight.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiTrafficLight.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiUnitFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiUnitFrame.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiUnitMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiUnitMenu.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiUpdateTip.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiUpdateTip.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiVendueWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiVendueWnd.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiWaitingMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiWaitingMsg.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiWorldCombatInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCase\UiWorldCombatInfo.h
# End Source File
# End Group
# Begin Group "UiElem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ui\UiElem\TLAlternateProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLAlternateProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLButton.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLCheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLCheckbox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLCloseButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLCloseButton.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLComboEditbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLComboEditbox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLEditbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLEditbox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLGameObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLGameObject.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLIEWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLIEWindow.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLListbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLListbox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLListView.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLListView.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLListViewItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLListViewItem.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniHorzScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniHorzScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniHorzScrollbarThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniHorzScrollbarThumb.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniVertScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniVertScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniVertScrollbarThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMiniVertScrollbarThumb.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLModule.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLModule.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMultiLineEditbox.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLMultiLineEditbox.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLQuestionWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLQuestionWindow.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLRadioButton.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLSlider.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLSliderThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLSliderThumb.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLStatic.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLSystemWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLSystemWindow.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTooltip.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTree.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTree.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTreeEx.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTreeEx.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLTreeItem.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLVertScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLVertScrollbar.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLVertScrollbarThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLVertScrollbarThumb.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLVUMeter.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiElem\TLVUMeter.h
# End Source File
# End Group
# Begin Group "UIDML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ui\UiMDLDataset.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiMDLDataset.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiMDLManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiMDLManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Ui\ConvertUTF.c
# End Source File
# Begin Source File

SOURCE=.\Ui\ConvertUTF.h
# End Source File
# Begin Source File

SOURCE=.\Ui\GameSpaceChangedNotify.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\KMessageCentre.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\KMessageCentre.h
# End Source File
# Begin Source File

SOURCE=.\Ui\LayoutRender.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\LayoutRender.h
# End Source File
# Begin Source File

SOURCE=.\Ui\ShortcutKey.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\ShortcutKey.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiAdapter.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiAdapter.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiCommon.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiConfigManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiConfigManager.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiGlobalEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiGlobalEvent.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiSheetMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiSheetMgr.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiSoundSetting.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiSoundSetting.h
# End Source File
# Begin Source File

SOURCE=.\Ui\UiWindowMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Ui\UiWindowMgr.h
# End Source File
# End Group
# Begin Group "Login"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Login\Login.cpp
# End Source File
# Begin Source File

SOURCE=.\Login\Login.h
# End Source File
# End Group
# Begin Group "Net"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NetConnect\NetConnectAgent.cpp
# End Source File
# Begin Source File

SOURCE=.\NetConnect\NetConnectAgent.h
# End Source File
# Begin Source File

SOURCE=.\NetConnect\NetMsgTargetObject.h
# End Source File
# End Group
# Begin Group "CoreDump"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mdump.cpp
# End Source File
# Begin Source File

SOURCE=.\mdump.h
# End Source File
# End Group
# Begin Group "ChatWindowExtra"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\chatWindow\ChatCharContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatCharContainer.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanAnnouncementDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanAnnouncementDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanComboBox.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanListControl.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanListControl.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanManager.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanManager.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanPanel.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanPlayerData.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanPlayerData.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanPopMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanPopMenu.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanTitleControl.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatClanTitleControl.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatControlPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatControlPanel.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\chatDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\chatDialog.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatFriendPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatFriendPanel.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatFriendPanelManager.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatFriendPanelManager.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatMainDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatMainDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\chatManager.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\chatManager.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatMiniMap.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatMiniMap.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatPage.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatPage.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatPlayerBaseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatPlayerBaseInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatResource.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatResource.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatTipWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatTipWnd.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatTipWndItem.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatTipWndItem.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatTipWndItem.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\chatWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\chatWnd.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatWndProc.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\ChatWndProc.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\EntrustComputerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\EntrustComputerDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\FaceDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\faceDialog.h
# End Source File
# Begin Source File

SOURCE=.\loadSrcWnd\GDILoadBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\loadSrcWnd\GDILoadBitmap.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\GDIRender.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\GDIRender.h
# End Source File
# Begin Source File

SOURCE=.\loadSrcWnd\LoadSrcWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\loadSrcWnd\LoadSrcWnd.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\LookFriendInfoPopDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\LookFriendInfoPopDlg.h
# End Source File
# Begin Source File

SOURCE=.\MicrophoneIn.cpp
# End Source File
# Begin Source File

SOURCE=.\MicrophoneIn.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\OnwerPlayerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\OnwerPlayerInfo.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\PlayerInfoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\PlayerInfoDlg.h
# End Source File
# Begin Source File

SOURCE=.\chatWindow\PlayerShowInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\chatWindow\PlayerShowInfo.h
# End Source File
# Begin Source File

SOURCE=.\loadSrcWnd\ProcessBar.cpp
# End Source File
# Begin Source File

SOURCE=.\loadSrcWnd\ProcessBar.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ErrorCode.cpp
# End Source File
# Begin Source File

SOURCE=.\ErrorCode.h
# End Source File
# Begin Source File

SOURCE=.\Faith.cpp
# End Source File
# Begin Source File

SOURCE=.\Faith.h
# End Source File
# Begin Source File

SOURCE=.\FaithEncrypter.h
# End Source File
# Begin Source File

SOURCE=.\FaithXOREncrypter.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
