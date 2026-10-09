# Microsoft Developer Studio Project File - Name="UpdateDLL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=UpdateDLL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UpdateDLL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UpdateDLL.mak" CFG="UpdateDLL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UpdateDLL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UpdateDLL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "UpdateDLL"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./src" /I "./" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 Ws2_32.lib Version.lib ApLib.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/Update.dll"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\bin\client\release	copy release\Update.dll ..\..\..\bin\client\release\UpdateDll.dll	copy release\Update.dll D:\FSII\fs2\UpdateDll.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./src" /I "./" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /YX"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Ws2_32.lib Version.lib ApLib.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/Update.dll" /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\..\bin\client\debug	copy debug\Update.dll ..\..\..\bin\client\debug\UpdateDll.dll	copy debug\Update.dll D:\FSII\fs2\UpdateDll.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "UpdateDLL - Win32 Release"
# Name "UpdateDLL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\bufsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BusyThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CRC32.C
# End Source File
# Begin Source File

SOURCE=.\src\downloadfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DownNotify.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ftpdownload.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GenKAVMoveProgram.cpp
# End Source File
# Begin Source File

SOURCE=.\src\getfilesversion.cpp
# End Source File
# Begin Source File

SOURCE=.\src\getproxysetting.cpp
# End Source File
# Begin Source File

SOURCE=.\GetVersion.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Global.cpp
# End Source File
# Begin Source File

SOURCE=.\src\httpdownload.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KCloseProgramMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KSChar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MsgWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ProcessIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\proxyutility.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PublicFun.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SaveLog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sockspacket.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UpdateData.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.cpp

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

# ADD CPP /YX"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.def
# End Source File
# Begin Source File

SOURCE=.\UpdateDLLImplement.cpp

!IF  "$(CFG)" == "UpdateDLL - Win32 Release"

!ELSEIF  "$(CFG)" == "UpdateDLL - Win32 Debug"

# ADD CPP /YX"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\UpdateExport.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UpdatePublic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\WndNotify.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\UpdateDLL.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\aplib.h
# End Source File
# Begin Source File

SOURCE=.\src\bufsocket.h
# End Source File
# Begin Source File

SOURCE=.\src\BusyThread.h
# End Source File
# Begin Source File

SOURCE=.\src\CRC32.h
# End Source File
# Begin Source File

SOURCE=.\src\DataDefine.h
# End Source File
# Begin Source File

SOURCE=.\src\downloadfile.h
# End Source File
# Begin Source File

SOURCE=.\src\DownNotify.h
# End Source File
# Begin Source File

SOURCE=.\src\ftpdownload.h
# End Source File
# Begin Source File

SOURCE=.\src\GenKAVMoveProgram.h
# End Source File
# Begin Source File

SOURCE=.\src\getfilesversion.h
# End Source File
# Begin Source File

SOURCE=.\src\getproxysetting.h
# End Source File
# Begin Source File

SOURCE=.\GetVersion.h
# End Source File
# Begin Source File

SOURCE=.\src\Global.h
# End Source File
# Begin Source File

SOURCE=.\src\httpdownload.h
# End Source File
# Begin Source File

SOURCE=.\KAESign.h
# End Source File
# Begin Source File

SOURCE=.\src\KAVPublic.h
# End Source File
# Begin Source File

SOURCE=.\src\KAVStrTranslate.h
# End Source File
# Begin Source File

SOURCE=.\src\KCloseProgramMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\KSChar.h
# End Source File
# Begin Source File

SOURCE=.\src\KString.h
# End Source File
# Begin Source File

SOURCE=.\src\KWString.h
# End Source File
# Begin Source File

SOURCE=.\src\MsgWnd.h
# End Source File
# Begin Source File

SOURCE=.\src\ProcessIndex.h
# End Source File
# Begin Source File

SOURCE=.\src\proxyutility.h
# End Source File
# Begin Source File

SOURCE=.\src\PublicFun.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\src\SaveLog.h
# End Source File
# Begin Source File

SOURCE=.\src\sockspacket.h
# End Source File
# Begin Source File

SOURCE=.\src\SourceDef.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdateData.h
# End Source File
# Begin Source File

SOURCE=.\UpdateDLL.h
# End Source File
# Begin Source File

SOURCE=.\src\updatedlllib.h
# End Source File
# Begin Source File

SOURCE=.\UpdateExport.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdatePublic.h
# End Source File
# Begin Source File

SOURCE=.\src\UpdateSelf.h
# End Source File
# Begin Source File

SOURCE=.\src\WndNotify.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section UpdateDLL : {C3F364B0-41D6-405D-9866-8720A30657D0}
# 	2:26:TYPEDEF: FTPDOWNLOADSTATUS:FTPDOWNLOADSTATUS
# 	2:15:ftpdownload.cpp:ftpdownload.cpp
# 	2:13:ftpdownload.h:ftpdownload.h
# 	2:19:CLASS: CFtpDownload:CFtpDownload
# 	2:27:TYPEDEF: PFTPDOWNLOADSTATUS:PFTPDOWNLOADSTATUS
# 	2:19:Application Include:UpdateDLL.h
# 	2:28:CLASS: _tagFtpDownloadStatus:_tagFtpDownloadStatus
# End Section
# Section UpdateDLL : {2D932168-8D25-4DB5-88DB-E1ED936EE730}
# 	2:18:TYPEDEF: SOCKS5REP:SOCKS5REP
# 	2:19:TYPEDEF: PSOCKS4REQ:PSOCKS4REQ
# 	2:18:TYPEDEF: SOCKS5REQ:SOCKS5REQ
# 	2:20:CLASS: _tagSocks5UDP:_tagSocks5UDP
# 	2:24:CLASS: _tagSocks5AuthRep:_tagSocks5AuthRep
# 	2:21:CLASS: _tagSocks4AReq:_tagSocks4AReq
# 	2:18:TYPEDEF: SOCKS4REQ:SOCKS4REQ
# 	2:19:TYPEDEF: PSOCKS5UDP:PSOCKS5UDP
# 	2:20:TYPEDEF: PSOCKS4AREQ:PSOCKS4AREQ
# 	2:24:CLASS: _tagSocks5AuthReq:_tagSocks5AuthReq
# 	2:28:TYPEDEF: SOCKS5AUTHPASSWDREP:SOCKS5AUTHPASSWDREP
# 	2:30:CLASS: _tagSocks5AuthPasswdRep:_tagSocks5AuthPasswdRep
# 	2:18:TYPEDEF: SOCKS5UDP:SOCKS5UDP
# 	2:28:TYPEDEF: SOCKS5AUTHPASSWDREQ:SOCKS5AUTHPASSWDREQ
# 	2:30:CLASS: _tagSocks5AuthPasswdReq:_tagSocks5AuthPasswdReq
# 	2:29:TYPEDEF: PSOCKS5AUTHPASSWDREP:PSOCKS5AUTHPASSWDREP
# 	2:29:TYPEDEF: PSOCKS5AUTHPASSWDREQ:PSOCKS5AUTHPASSWDREQ
# 	2:19:TYPEDEF: PSOCK4AREP:PSOCK4AREP
# 	2:17:TYPEDEF: SOCK4REP:SOCK4REP
# 	2:24:TYPEDEF: PSOCKSUDPPACKET:PSOCKSUDPPACKET
# 	2:24:TYPEDEF: PSOCKSREPPACKET:PSOCKSREPPACKET
# 	2:24:TYPEDEF: PSOCKSREQPACKET:PSOCKSREQPACKET
# 	2:15:sockspacket.cpp:sockspacket.cpp
# 	2:13:sockspacket.h:sockspacket.h
# 	2:19:TYPEDEF: SOCKS4AREP:SOCKS4AREP
# 	2:18:TYPEDEF: PSOCK4REP:PSOCK4REP
# 	2:22:TYPEDEF: SOCKS5AUTHREP:SOCKS5AUTHREP
# 	2:19:TYPEDEF: SOCKS4AREQ:SOCKS4AREQ
# 	2:25:CLASS: _tagSocksRepPacket:_tagSocksRepPacket
# 	2:22:TYPEDEF: SOCKS5AUTHREQ:SOCKS5AUTHREQ
# 	2:25:CLASS: _tagSocksReqPacket:_tagSocksReqPacket
# 	2:23:TYPEDEF: SOCKSREPPACKET:SOCKSREPPACKET
# 	2:20:CLASS: _tagSocks5Rep:_tagSocks5Rep
# 	2:19:Application Include:UpdateDLL.h
# 	2:23:TYPEDEF: SOCKSUDPPACKET:SOCKSUDPPACKET
# 	2:23:TYPEDEF: SOCKSREQPACKET:SOCKSREQPACKET
# 	2:19:TYPEDEF: PSOCKS5REP:PSOCKS5REP
# 	2:23:TYPEDEF: PSOCKS5AUTHREP:PSOCKS5AUTHREP
# 	2:20:CLASS: _tagSocks5Req:_tagSocks5Req
# 	2:19:CLASS: CSocksPacket:CSocksPacket
# 	2:19:TYPEDEF: PSOCKS5REQ:PSOCKS5REQ
# 	2:23:TYPEDEF: PSOCKS5AUTHREQ:PSOCKS5AUTHREQ
# 	2:25:CLASS: _tagSocksUDPPacket:_tagSocksUDPPacket
# 	2:20:CLASS: _tagSocks4Req:_tagSocks4Req
# 	2:19:CLASS: _tagSock4Rep:_tagSock4Rep
# End Section
# Section UpdateDLL : {BEADC802-445E-4802-84AC-3E97E83D3959}
# 	2:20:CLASS: CDownloadFile:CDownloadFile
# 	2:16:downloadfile.cpp:downloadfile.cpp
# 	2:14:downloadfile.h:downloadfile.h
# 	2:24:TYPEDEF: PDOWNLOADSTATUS:PDOWNLOADSTATUS
# 	2:19:Application Include:UpdateDLL.h
# End Section
# Section UpdateDLL : {6DEC0750-96DE-4E98-93EB-6A673DBB6AA8}
# 	2:28:TYPEDEF: PHTTPDOWNLOADSTATUS:PHTTPDOWNLOADSTATUS
# 	2:16:httpdownload.cpp:httpdownload.cpp
# 	2:29:CLASS: _tagHttpDownloadStatus:_tagHttpDownloadStatus
# 	2:14:httpdownload.h:httpdownload.h
# 	2:27:TYPEDEF: HTTPDOWNLOADSTATUS:HTTPDOWNLOADSTATUS
# 	2:19:Application Include:UpdateDLL.h
# 	2:20:CLASS: CHttpDownload:CHttpDownload
# End Section
# Section UpdateDLL : {369DE9E9-1EFF-4922-AD91-66365F52B004}
# 	2:13:TYPEDEF: PBSD:PBSD
# 	2:24:CLASS: _tagBufSocketData:_tagBufSocketData
# 	2:11:bufsocket.h:bufsocket.h
# 	2:17:CLASS: CBufSocket:CBufSocket
# 	2:13:bufsocket.cpp:bufsocket.cpp
# 	2:12:TYPEDEF: BSD:BSD
# 	2:19:Application Include:UpdateDLL.h
# End Section
# Section UpdateDLL : {5C2A0CA9-9EBA-4B92-80F0-A1766294C0E7}
# 	2:14:BusyThread.cpp:BusyThread.cpp
# 	2:18:CLASS: CBusyThread:CBusyThread
# 	2:19:Application Include:UpdateDLL.h
# 	2:12:BusyThread.h:BusyThread.h
# End Section
