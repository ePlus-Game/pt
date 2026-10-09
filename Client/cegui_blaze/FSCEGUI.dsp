# Microsoft Developer Studio Project File - Name="FSCEGUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=FSCEGUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FSCEGUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FSCEGUI.mak" CFG="FSCEGUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSCEGUI - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "FSCEGUI - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "FSCEGUI"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FSCEGUI - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\..\..\Share\Header\FreeType" /I ".\..\..\Share\Header\cegui" /I ".\dependencies\include" /I ".\..\..\Share\Header\engine" /I ".\..\..\Share\Header\Represent" /I ".\..\..\freetype234\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "CEGUIBASE_EXPORTS" /D "DIRECTX7_GUIRENDERER_EXPORTS" /D "TAHAREZLOOK_EXPORTS" /D "_UNICODE" /D "UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 Game.lib freetype234.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /nodefaultlib:"LIBCMT" /out:"Release/FSInterface.dll" /libpath:".\dependencies\lib" /libpath:".\..\..\share\lib\release"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\bin\client\release	md ..\..\Share\lib\release	copy release\FSInterface.dll ..\..\bin\client\release\FSInterface.dll	copy release\FSInterface.lib ..\..\Share\lib\release\FSInterface.lib	copy release\FSInterface.pdb ..\..\bin\client\release\FSInterface.pdb	copy release\FSInterface.map ..\..\bin\client\release\FSInterface.map
# End Special Build Tool

!ELSEIF  "$(CFG)" == "FSCEGUI - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\..\..\Share\Header\FreeType" /I ".\..\..\Share\Header\cegui" /I ".\dependencies\include" /I ".\..\..\Share\Header\engine" /I ".\..\..\Share\Header\Represent" /D "_UNICODE" /D "UNICODE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "CEGUIBASE_EXPORTS" /D "DIRECTX7_GUIRENDERER_EXPORTS" /D "TAHAREZLOOK_EXPORTS" /D "_STLP_DEBUG" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 game.lib freetype234_D.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /nodefaultlib:"LIBCMTD" /out:"Debug/FSInterface.dll" /pdbtype:sept /libpath:".\dependencies\lib" /libpath:".\..\..\share\lib\debug"
# SUBTRACT LINK32 /pdb:none /nodefaultlib
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=md ..\..\bin\client\debug	md ..\..\Share\lib\debug	copy Debug\FSInterface.dll ..\..\bin\client\debug\FSInterface.dll	copy Debug\FSInterface.lib ..\..\Share\lib\debug\FSInterface.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "FSCEGUI - Win32 Release"
# Name "FSCEGUI - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\FSCEGUI.rc
# End Source File
# End Group
# Begin Group "CEGUIBase"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "tinyxml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=.\src\tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\src\tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# Begin Group "pcre (source)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\pcre\chartables.c
# End Source File
# Begin Source File

SOURCE=.\src\pcre\get.c
# End Source File
# Begin Source File

SOURCE=.\src\pcre\maketables.c
# End Source File
# Begin Source File

SOURCE=.\src\pcre\pcre.c
# End Source File
# Begin Source File

SOURCE=.\src\pcre\pcreposix.c
# End Source File
# Begin Source File

SOURCE=.\src\pcre\study.c
# End Source File
# End Group
# Begin Group "elements (source)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\elements\CEGUIButtonBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIButtonBaseProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUICheckbox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUICheckboxProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIEditbox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIEditboxProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIGUISheet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIListbox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIListboxItem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIListboxProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIListboxTextItem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIMultiLineEditbox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIMultiLineEditboxProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIProgressBarProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIPushButton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIPushButtonProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIRadioButton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIRadioButtonProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIScrollbar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIScrollbarProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUISlider.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUISliderProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIStaticImage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIStaticImageProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIStaticProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIStaticText.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIStaticTextProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIThumb.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUIThumbProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUITooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUITooltipProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUITree.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUITreeItem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\elements\CEGUITreeProperties.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIBase.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIcolour.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIColourRect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIConfig_xmlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUICoordConverter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIDefaultResourceProvider.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIEvent.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIEventArgs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIEventSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIEventSignal.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIEventSignalSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIExceptions.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIFactoryModule.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIFont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIFont_xmlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIFontManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIFontProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIFreeTypeFont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIGdiFont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIGlobalEventSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIGUILayout_xmlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIImage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIImageset.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIImageset_xmlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIImagesetManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIMouseCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIPropertyHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIPropertySet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRefPtr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRenderableElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRenderableFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRenderableImage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRenderCache.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIScheme.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIScheme_xmlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISchemeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIScriptModule.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISize.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISoundObj.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISoundSet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISoundset_xmlHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISoundSetManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIString.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUISystem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUITexture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUITextUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\include\CEGUIUnicodeMap.h
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIVector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIWin32XMLSelectHack.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIWindowFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIWindowFactoryManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIWindowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIWindowProperties.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIXMLAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIXMLHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CEGUIXMLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KMRU.cpp
# End Source File
# Begin Source File

SOURCE=.\src\KRenderCache.cpp
# End Source File
# End Group
# End Group
# Begin Group "DX7Render"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\renderers\directx7GUIRenderer\dxdraw7renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\include\renderers\directx7GUIRenderer\dxdraw7renderer.h
# End Source File
# Begin Source File

SOURCE=.\src\renderers\directx7GUIRenderer\dxdraw7texture.cpp
# End Source File
# Begin Source File

SOURCE=.\include\renderers\directx7GUIRenderer\dxdraw7Texture.h
# End Source File
# End Group
# Begin Group "DSound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Sounder\UIDXSound.cpp
# End Source File
# Begin Source File

SOURCE=.\include\Sounder\UIDXSound.h
# End Source File
# End Group
# End Target
# End Project
