# Microsoft Developer Studio Project File - Name="AutoUpdate" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AutoUpdate - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AutoUpdate.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AutoUpdate.mak" CFG="AutoUpdate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AutoUpdate - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AutoUpdate - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TOOLS/AUTOUPDATE", DAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AutoUpdate - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\Share\Header\Engine" /I ".\customcontrols" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Ws2_32.lib game.lib /nologo /subsystem:windows /machine:I386 /out:"release\Update.exe" /libpath:"../../../Share/lib/Release" /libpath:"..\updatedll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\Update.exe ..\..\..\bin\client\release\FS2Run.exe	copy release\Update.exe D:\FSII\fs2\AutoRun.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "AutoUpdate - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\Share\Header\Engine" /I ".\customcontrols" /I ".\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib game.lib /nologo /subsystem:windows /debug /machine:I386 /out:"debug\Update.exe" /pdbtype:sept /libpath:"../../../Share/lib/Debug" /libpath:"..\updatedll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\Update.exe ..\..\..\bin\client\debug\FS2Run.exe	copy debug\Update.exe D:\FSII\fs2\AutoRun.exe	copy debug\Update.exe E:\FSI\AutoRun.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "AutoUpdate - Win32 Release"
# Name "AutoUpdate - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ServerListCpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ServerList.cpp
# End Source File
# End Group
# Begin Group "AdditionDlgCpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TransTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateTipDlg.cpp
# End Source File
# End Group
# Begin Group "EncrypterCpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XOREncrypter.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\AutoUpdate.rc
# End Source File
# Begin Source File

SOURCE=.\AutoUpdateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\bmpbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnST.cpp
# End Source File
# Begin Source File

SOURCE=.\Dib.cpp
# End Source File
# Begin Source File

SOURCE=.\DirSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCustomMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgGetPath.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgShowInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DownLoadFile.cpp
# End Source File
# Begin Source File

SOURCE=.\EditionSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\GameOptionPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\HyperlinkStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\IEComCtrlSink.cpp
# End Source File
# Begin Source File

SOURCE=.\IniFile.cpp
# End Source File
# Begin Source File

SOURCE=.\IniSection.cpp
# End Source File
# Begin Source File

SOURCE=.\ItermProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\Kernel.cpp
# End Source File
# Begin Source File

SOURCE=.\KExServerDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\LastUpdateSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\lientGameOptionProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\Picture.cpp
# End Source File
# Begin Source File

SOURCE=.\PictureEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressCtrlST.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TransparentStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\urlbmpbutton.cpp
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.cpp
# End Source File
# Begin Source File

SOURCE=.\WndCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\WndTool.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "SeverList"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ServerList.h
# End Source File
# End Group
# Begin Group "AddtionalDlg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TransTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\UpdateTipDlg.h
# End Source File
# End Group
# Begin Group "Encrypter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Encrypter.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=.\AutoUpdateDlg.h
# End Source File
# Begin Source File

SOURCE=.\AutoupdateStringTable.h
# End Source File
# Begin Source File

SOURCE=.\AutoupdateStringTable_T.h
# End Source File
# Begin Source File

SOURCE=.\BitmapDialog.h
# End Source File
# Begin Source File

SOURCE=.\BitmapSlider.h
# End Source File
# Begin Source File

SOURCE=.\bmpbutton.h
# End Source File
# Begin Source File

SOURCE=.\BtnST.h
# End Source File
# Begin Source File

SOURCE=.\Dib.h
# End Source File
# Begin Source File

SOURCE=.\DirSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\DlgCustomMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\DlgShowInfo.h
# End Source File
# Begin Source File

SOURCE=.\DownLoadFile.h
# End Source File
# Begin Source File

SOURCE=.\EditionSelector.h
# End Source File
# Begin Source File

SOURCE=.\GameGuid.h
# End Source File
# Begin Source File

SOURCE=.\GameOptionPanel.h
# End Source File
# Begin Source File

SOURCE=.\HyperlinkStatic.h
# End Source File
# Begin Source File

SOURCE=.\IEComCtrlSink.h
# End Source File
# Begin Source File

SOURCE=.\IniFile.h
# End Source File
# Begin Source File

SOURCE=.\IniSection.h
# End Source File
# Begin Source File

SOURCE=.\ItermProcess.h
# End Source File
# Begin Source File

SOURCE=.\Kernel.h
# End Source File
# Begin Source File

SOURCE=.\KExServerDlg.h
# End Source File
# Begin Source File

SOURCE=.\LastUpdateSelection.h
# End Source File
# Begin Source File

SOURCE=.\lientGameOptionProcess.h
# End Source File
# Begin Source File

SOURCE=.\Picture.h
# End Source File
# Begin Source File

SOURCE=.\PictureEx.h
# End Source File
# Begin Source File

SOURCE=.\ProgressCtrlST.h
# End Source File
# Begin Source File

SOURCE=.\ReadOnlyEdit.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ShareUIInfo.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TransparentStatic.h
# End Source File
# Begin Source File

SOURCE=.\UpdateProcess.h
# End Source File
# Begin Source File

SOURCE=.\urlbmpbutton.h
# End Source File
# Begin Source File

SOURCE=.\webbrowser2.h
# End Source File
# Begin Source File

SOURCE=.\WndCallBack.h
# End Source File
# Begin Source File

SOURCE=.\WndCommand.h
# End Source File
# Begin Source File

SOURCE=.\WndTool.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\AutoUpdate.ico
# End Source File
# Begin Source File

SOURCE=.\res\AutoUpdate.rc2
# End Source File
# Begin Source File

SOURCE=.\RES\background.bmp
# End Source File
# Begin Source File

SOURCE=.\res\background.jpg
# End Source File
# Begin Source File

SOURCE=.\RES\bitmap_u.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ColumnHeaderEnd.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ColumnHeaderSpan.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\ColumnHeaderStart.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\RES\custommsgbackgroud.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\DetailInfoBackGround.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\filetypes.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\LedOff.ico
# End Source File
# Begin Source File

SOURCE=.\RES\LedOn.ico
# End Source File
# Begin Source File

SOURCE=.\RES\left.gif
# End Source File
# Begin Source File

SOURCE=.\RES\pitchoff.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\pitchon.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Setting_bar.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Setting_Block.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\SettingDlgback.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\TipOK_DISABLE.bmp
# End Source File
# Begin Source File

SOURCE=.\TipOK_DISABLE.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\TipOK_DOWN.bmp
# End Source File
# Begin Source File

SOURCE=.\TipOK_DOWN.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\TipOK_OVER.bmp
# End Source File
# Begin Source File

SOURCE=.\TipOK_OVER.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\TipOK_UP.bmp
# End Source File
# Begin Source File

SOURCE=.\TipOK_UP.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\TreeBack.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\VersionSelect.bmp
# End Source File
# Begin Source File

SOURCE=".\RES\窗口.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\窗口亮.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\打开.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\复件 游戏设置4.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\更新_DISABLE.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\更新_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\更新_OVER.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\更新_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\更新提示.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\更新提示2.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\关闭_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\关闭_OVER.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\关闭_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\光球02.gif"
# End Source File
# Begin Source File

SOURCE=".\res\进度点.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\进度点2.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\进度条.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\开始_DISABLE.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\开始_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\开始_OVER.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\开始_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\默认.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\取消.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\取消.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\全屏.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\全屏亮.ico"
# End Source File
# Begin Source File

SOURCE=".\RES\确定.ico"
# End Source File
# Begin Source File

SOURCE=".\res\设置_DISABLE.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\设置_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\设置_OVER.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\设置_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\退出_DISABLE.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\退出_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\退出_OVER.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\退出_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\文件进度条后图.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\信息_DISABLE.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\信息_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\信息_OVER.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\信息_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\主进度背景.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\主进度条标示.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\主进度条前景.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\最小化_DOWN.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\最小化_OVERR.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\最小化_UP.bmp"
# End Source File
# Begin Source File

SOURCE=".\RES\浏览文件面板.bmp"
# End Source File
# End Group
# Begin Group "http/ftp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\downloadtmp\bufsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\bufsocket.h
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\ftpdownload.cpp
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\ftpdownload.h
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\httpdownload.cpp
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\httpdownload.h
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\sockspacket.cpp
# End Source File
# Begin Source File

SOURCE=.\downloadtmp\sockspacket.h
# End Source File
# End Group
# Begin Group "CustomListCtrl"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\customcontrols\BitmapListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\customcontrols\BitmapListBox.h
# End Source File
# Begin Source File

SOURCE=.\customcontrols\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\customcontrols\ListCtrlEx.h
# End Source File
# Begin Source File

SOURCE=.\customcontrols\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\customcontrols\SkinHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\customcontrols\SkinHeaderCtrl.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\RES\default.mht
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section AutoUpdate : {B30A5E0B-E641-4C64-96B7-23CB4169A2D9}
# 	2:5:Dib.h:Dib1.h
# 	2:7:Dib.cpp:Dib1.cpp
# 	2:11:CLASS: CDIB:CDIB
# 	2:19:Application Include:AutoUpdate.h
# 	2:13:CLASS: CDibDC:CDibDC
# End Section
# Section AutoUpdate : {8A08E199-339A-4C13-9096-ADC5C9CC912A}
# 	2:21:LastUpdateSelection.h:LastUpdateSelection.h
# 	2:27:CLASS: CLastUpdateSelection:CLastUpdateSelection
# 	2:19:Application Include:AutoUpdate.h
# 	2:23:LastUpdateSelection.cpp:LastUpdateSelection.cpp
# End Section
# Section AutoUpdate : {65BEBC48-9645-4DAD-B6BC-876EDD40782A}
# 	2:21:TransparentStatic.cpp:TransparentStatic.cpp
# 	2:25:CLASS: CTransparentStatic:CTransparentStatic
# 	2:19:TransparentStatic.h:TransparentStatic.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {9B6D81AF-E7E4-413B-9A95-2E3B987BC0C3}
# 	2:20:CLASS: RecordProcess:RecordProcess
# 	2:15:RecordProcess.h:RecordProcess.h
# 	2:17:RecordProcess.cpp:RecordProcess.cpp
# 	2:19:Application Include:AutoUpdate.h
# 	2:24:TYPEDEF: ItermProcessSet:ItermProcessSet
# End Section
# Section AutoUpdate : {F68DF7AF-95A3-4EF1-AB58-994F0EB1FB74}
# 	2:16:CLASS: BMPButton:BMPButton
# 	2:11:ENUM: COLOR:COLOR
# 	2:11:WndTool.cpp:WndTool1.cpp
# 	2:17:CLASS: WindowRect:WindowRect
# 	2:15:CLASS: CWndTool:CWndTool
# 	2:14:CLASS: UrlLink:UrlLink
# 	2:9:WndTool.h:WndTool1.h
# 	2:19:Application Include:AutoUpdate.h
# 	2:19:CLASS: URLBmpButton:URLBmpButton
# End Section
# Section AutoUpdate : {767A8650-6F92-48BC-A417-E6BA46A6A3DD}
# 	2:10:ENUM: enum:enum
# 	2:16:CLASS: CButtonST:CButtonST
# 	2:9:BtnST.cpp:BtnST.cpp
# 	2:19:Application Include:AutoUpdate.h
# 	2:7:BtnST.h:BtnST.h
# End Section
# Section AutoUpdate : {DD6FF645-E6DD-4E47-B5AB-E816F81CF2C9}
# 	2:14:WndCommand.cpp:WndCommand.cpp
# 	2:10:ENUM: enum:enum
# 	2:18:CLASS: CWndCommand:CWndCommand
# 	2:19:Application Include:AutoUpdate.h
# 	2:12:WndCommand.h:WndCommand.h
# End Section
# Section AutoUpdate : {0B90E948-E54B-4D7A-88FB-27F79CAD891A}
# 	2:11:IniFile.cpp:IniFile.cpp
# 	2:10:ENUM: enum:enum
# 	2:15:CLASS: CIniFile:CIniFile
# 	2:19:Application Include:AutoUpdate.h
# 	2:9:IniFile.h:IniFile.h
# End Section
# Section AutoUpdate : {21EDD37D-1AA6-452B-BA1E-C22494BC6BE5}
# 	2:20:CLASS: CURLBmpButton:CURLBmpButton
# 	2:14:urlbmpbutton.h:urlbmpbutton.h
# 	2:16:urlbmpbutton.cpp:urlbmpbutton.cpp
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {C41BC4F4-9DA6-4271-ADD0-77BD36C0B65D}
# 	2:10:Kernel.cpp:Kernel.cpp
# 	2:14:CLASS: CKernel:CKernel
# 	2:8:Kernel.h:Kernel.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {D5218B82-3CB2-40B1-B1A2-93D80D5E72E6}
# 	2:15:UpdateProcess.h:UpdateProcess.h
# 	2:21:CLASS: CUpdateProcess:CUpdateProcess
# 	2:21:TYPEDEF: UPDATEA_INIT:UPDATEA_INIT
# 	2:22:TYPEDEF: UPDATE_UNINIT:UPDATE_UNINIT
# 	2:22:TYPEDEF: UPDATE_CANCEL:UPDATE_CANCEL
# 	2:17:UpdateProcess.cpp:UpdateProcess.cpp
# 	2:19:Application Include:AutoUpdate.h
# 	2:21:TYPEDEF: UPDATE_START:UPDATE_START
# End Section
# Section AutoUpdate : {4172D745-D3F6-4639-9D1F-6AE4CF1359A9}
# 	1:13:IDD_DIALOGBAR:104
# 	2:16:Resource Include:resource.h
# 	2:13:IDD_DIALOGBAR:IDD_DIALOGBAR
# 	2:17:GameOptionPanel.h:GameOptionPanel.h
# 	2:10:ENUM: enum:enum
# 	2:19:GameOptionPanel.cpp:GameOptionPanel.cpp
# 	2:22:CLASS: GameOptionPanel:GameOptionPanel
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {D734AED4-4D57-44D1-B954-8B30B060EC5E}
# 	2:16:BitmapDialog.cpp:BitmapDialog.cpp
# 	2:20:CLASS: CBitmapDialog:CBitmapDialog
# 	2:17:ENUM: LayOutStyle:LayOutStyle
# 	2:14:BitmapDialog.h:BitmapDialog.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {A0A58A70-56A2-4C0B-977F-5FDA13CB86B0}
# 	2:23:CLASS: CEditionSelector:CEditionSelector
# 	2:10:ENUM: enum:enum
# 	2:17:EditionSelector.h:EditionSelector.h
# 	2:19:EditionSelector.cpp:EditionSelector.cpp
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {E48EF174-E318-4FF6-82A8-5731219983A2}
# 	2:16:DefaultProcess.h:DefaultProcess.h
# 	2:18:DefaultProcess.cpp:DefaultProcess.cpp
# 	2:21:CLASS: DefaultProcess:DefaultProcess
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {2EC69855-BF41-4053-9C0F-5DC598E9E288}
# 	2:28:CLASS: ItermProcessTypeCheck:ItermProcessTypeCheck
# 	2:23:ItermProcessTypeCheck.h:ItermProcessTypeCheck.h
# 	2:25:ItermProcessTypeCheck.cpp:ItermProcessTypeCheck.cpp
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {B0DEA32C-9879-4CBA-9527-7C15829F1AEE}
# 	2:16:CLASS: BMPButton:BMPButton
# 	2:11:ENUM: COLOR:COLOR
# 	2:11:WndTool.cpp:WndTool.cpp
# 	2:17:CLASS: WindowRect:WindowRect
# 	2:15:CLASS: CWndTool:CWndTool
# 	2:14:CLASS: UrlLink:UrlLink
# 	2:9:WndTool.h:WndTool.h
# 	2:19:Application Include:AutoUpdate.h
# 	2:19:CLASS: URLBmpButton:URLBmpButton
# End Section
# Section AutoUpdate : {2EC69855-BF41-4053-9C0F-5DC598E9E288}
# 	2:28:CLASS: ItermProcessTypeCheck:ItermProcessTypeCheck
# 	2:23:ItermProcessTypeCheck.h:ItermProcessTypeCheck.h
# 	2:25:ItermProcessTypeCheck.cpp:ItermProcessTypeCheck.cpp
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {CF1F5CC7-13AF-4962-BDB7-DEE46BC58295}
# 	2:16:ItermProcess.cpp:ItermProcess.cpp
# 	2:14:ItermProcess.h:ItermProcess.h
# 	2:19:CLASS: ItermProcess:ItermProcess
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {565D7C98-2253-4710-A09B-720228EB5410}
# 	2:9:Picture.h:Picture.h
# 	2:11:Picture.cpp:Picture.cpp
# 	2:15:CLASS: CPicture:CPicture
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {2DCB0FA9-E057-48DE-9976-30FD16ED76CF}
# 	2:18:ProgressCtrlST.cpp:ProgressCtrlST.cpp
# 	2:22:CLASS: CProgressCtrlST:CProgressCtrlST
# 	2:16:ProgressCtrlST.h:ProgressCtrlST.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {D5E7A199-8711-45C0-B3F0-C9F65172C9BF}
# 	2:19:HyperlinkStatic.cpp:HyperlinkStatic.cpp
# 	2:23:CLASS: CHyperlinkStatic:CHyperlinkStatic
# 	2:19:Application Include:AutoUpdate.h
# 	2:17:HyperlinkStatic.h:HyperlinkStatic.h
# End Section
# Section AutoUpdate : {35FFB533-E393-4A63-BE3E-4F17D2B73A6F}
# 	1:23:IDD_DIRSELECTDLG_DIALOG:103
# 	2:16:Resource Include:resource.h
# 	2:23:IDD_DIRSELECTDLG_DIALOG:IDD_DIRSELECTDLG_DIALOG
# 	2:20:CLASS: CDirSelectDlg:CDirSelectDlg
# 	2:16:DirSelectDlg.cpp:DirSelectDlg.cpp
# 	2:10:ENUM: enum:enum
# 	2:14:DirSelectDlg.h:DirSelectDlg.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {6F27BD1C-D3FA-45F7-A1FD-DF0B63D93CA2}
# 	2:5:Dib.h:Dib.h
# 	2:7:Dib.cpp:Dib.cpp
# 	2:11:CLASS: CDIB:CDIB
# 	2:19:Application Include:AutoUpdate.h
# 	2:13:CLASS: CDibDC:CDibDC
# End Section
# Section AutoUpdate : {6F3A010C-517F-4137-B07F-7D3698D1AC61}
# 	2:14:IniSection.cpp:IniSection.cpp
# 	2:18:CLASS: CIniSection:CIniSection
# 	2:12:IniSection.h:IniSection.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {924520B0-1001-408F-B278-0082AD56F970}
# 	2:17:CLASS: CPictureEx:CPictureEx
# 	2:13:PictureEx.cpp:PictureEx.cpp
# 	2:11:PictureEx.h:PictureEx.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {3BE8BA0E-C724-4308-83C9-70C46D0A8D05}
# 	2:10:ENUM: enum:enum
# 	2:13:bmpbutton.cpp:bmpbutton.cpp
# 	2:11:bmpbutton.h:bmpbutton.h
# 	2:19:Application Include:AutoUpdate.h
# 	2:17:CLASS: CBmpButton:CBmpButton
# End Section
# Section AutoUpdate : {8A08E199-339A-4C13-9096-ADC5C9CC912A}
# 	2:21:LastUpdateSelection.h:LastUpdateSelection.h
# 	2:27:CLASS: CLastUpdateSelection:CLastUpdateSelection
# 	2:19:Application Include:AutoUpdate.h
# 	2:23:LastUpdateSelection.cpp:LastUpdateSelection.cpp
# End Section
# Section AutoUpdate : {B30A5E0B-E641-4C64-96B7-23CB4169A2D9}
# 	2:5:Dib.h:Dib1.h
# 	2:7:Dib.cpp:Dib1.cpp
# 	2:11:CLASS: CDIB:CDIB
# 	2:19:Application Include:AutoUpdate.h
# 	2:13:CLASS: CDibDC:CDibDC
# End Section
# Section AutoUpdate : {9B6D81AF-E7E4-413B-9A95-2E3B987BC0C3}
# 	2:20:CLASS: RecordProcess:RecordProcess
# 	2:15:RecordProcess.h:RecordProcess.h
# 	2:17:RecordProcess.cpp:RecordProcess.cpp
# 	2:19:Application Include:AutoUpdate.h
# 	2:24:TYPEDEF: ItermProcessSet:ItermProcessSet
# End Section
# Section AutoUpdate : {65BEBC48-9645-4DAD-B6BC-876EDD40782A}
# 	2:21:TransparentStatic.cpp:TransparentStatic.cpp
# 	2:25:CLASS: CTransparentStatic:CTransparentStatic
# 	2:19:TransparentStatic.h:TransparentStatic.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {767A8650-6F92-48BC-A417-E6BA46A6A3DD}
# 	2:10:ENUM: enum:enum
# 	2:16:CLASS: CButtonST:CButtonST
# 	2:9:BtnST.cpp:BtnST.cpp
# 	2:19:Application Include:AutoUpdate.h
# 	2:7:BtnST.h:BtnST.h
# End Section
# Section AutoUpdate : {F68DF7AF-95A3-4EF1-AB58-994F0EB1FB74}
# 	2:16:CLASS: BMPButton:BMPButton
# 	2:11:ENUM: COLOR:COLOR
# 	2:11:WndTool.cpp:WndTool1.cpp
# 	2:17:CLASS: WindowRect:WindowRect
# 	2:15:CLASS: CWndTool:CWndTool
# 	2:14:CLASS: UrlLink:UrlLink
# 	2:9:WndTool.h:WndTool1.h
# 	2:19:Application Include:AutoUpdate.h
# 	2:19:CLASS: URLBmpButton:URLBmpButton
# End Section
# Section AutoUpdate : {0B90E948-E54B-4D7A-88FB-27F79CAD891A}
# 	2:11:IniFile.cpp:IniFile.cpp
# 	2:10:ENUM: enum:enum
# 	2:15:CLASS: CIniFile:CIniFile
# 	2:19:Application Include:AutoUpdate.h
# 	2:9:IniFile.h:IniFile.h
# End Section
# Section AutoUpdate : {DD6FF645-E6DD-4E47-B5AB-E816F81CF2C9}
# 	2:14:WndCommand.cpp:WndCommand.cpp
# 	2:10:ENUM: enum:enum
# 	2:18:CLASS: CWndCommand:CWndCommand
# 	2:19:Application Include:AutoUpdate.h
# 	2:12:WndCommand.h:WndCommand.h
# End Section
# Section AutoUpdate : {D5218B82-3CB2-40B1-B1A2-93D80D5E72E6}
# 	2:15:UpdateProcess.h:UpdateProcess.h
# 	2:21:CLASS: CUpdateProcess:CUpdateProcess
# 	2:21:TYPEDEF: UPDATEA_INIT:UPDATEA_INIT
# 	2:22:TYPEDEF: UPDATE_UNINIT:UPDATE_UNINIT
# 	2:22:TYPEDEF: UPDATE_CANCEL:UPDATE_CANCEL
# 	2:17:UpdateProcess.cpp:UpdateProcess.cpp
# 	2:19:Application Include:AutoUpdate.h
# 	2:21:TYPEDEF: UPDATE_START:UPDATE_START
# End Section
# Section AutoUpdate : {C41BC4F4-9DA6-4271-ADD0-77BD36C0B65D}
# 	2:10:Kernel.cpp:Kernel.cpp
# 	2:14:CLASS: CKernel:CKernel
# 	2:8:Kernel.h:Kernel.h
# 	2:19:Application Include:AutoUpdate.h
# End Section
# Section AutoUpdate : {21EDD37D-1AA6-452B-BA1E-C22494BC6BE5}
# 	2:20:CLASS: CURLBmpButton:CURLBmpButton
# 	2:14:urlbmpbutton.h:urlbmpbutton.h
# 	2:16:urlbmpbutton.cpp:urlbmpbutton.cpp
# 	2:19:Application Include:AutoUpdate.h
# End Section
