# Microsoft Developer Studio Project File - Name="QuickQuiz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=QuickQuiz - Win32 Debug_Unicode
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "QuickQuiz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "QuickQuiz.mak" CFG="QuickQuiz - Win32 Debug_Unicode"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "QuickQuiz - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "QuickQuiz - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "QuickQuiz - Win32 Debug_Unicode" (based on "Win32 (x86) Application")
!MESSAGE "QuickQuiz - Win32 Release_Unicode" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "QuickQuiz - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release/Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./lib/libjpeg" /I "./lib/libpng" /I "./lib/zlib" /I "./lib/tinyxml" /I "./src" /I "./res" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "./res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /libpath:"./lib/"

!ELSEIF  "$(CFG)" == "QuickQuiz - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug/Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./lib/libjpeg" /I "./lib/libpng" /I "./lib/zlib" /I "./lib/tinyxml" /I "./src" /I "./res" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "./res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"./lib/"

!ELSEIF  "$(CFG)" == "QuickQuiz - Win32 Debug_Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "QuickQuiz___Win32_Debug_Unicode"
# PROP BASE Intermediate_Dir "QuickQuiz___Win32_Debug_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode/Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./lib/libjpeg" /I "./lib/libpng" /I "./lib/zlib" /I "./lib/tinyxml" /I "./src" /I "./res" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /D "_AFXDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "./res" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"./lib/"

!ELSEIF  "$(CFG)" == "QuickQuiz - Win32 Release_Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "QuickQuiz___Win32_Release_Unicode"
# PROP BASE Intermediate_Dir "QuickQuiz___Win32_Release_Unicode"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_Unicode"
# PROP Intermediate_Dir "Release_Unicode/Intermediate"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./lib/libjpeg" /I "./lib/libpng" /I "./lib/zlib" /I "./lib/tinyxml" /I "./src" /I "./res" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_UNICODE" /D "_AFXDLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /i "./res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /libpath:"./lib/"

!ENDIF 

# Begin Target

# Name "QuickQuiz - Win32 Release"
# Name "QuickQuiz - Win32 Debug"
# Name "QuickQuiz - Win32 Debug_Unicode"
# Name "QuickQuiz - Win32 Release_Unicode"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\AddTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AES.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AllBuildDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\AnsiCEx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Base64.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BatchSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Blank.cpp
# End Source File
# Begin Source File

SOURCE=.\src\BlkTxtCheckDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChildBlankDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChildJudgeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChildMChoiseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChildSChoiseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChildTextDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Choise.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ChooseTypeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ClrCommon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ClrDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CodeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CopyExDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DataList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Doc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FindText.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FixedRecentFileList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ImageBar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ImageFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ImageManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ItemGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ItemGroupDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ItemPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ItemShowWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\src\JpegCoverDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Judge.cpp
# End Source File
# Begin Source File

SOURCE=.\src\LeftView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\List.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ListFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MD5.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MemBmp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Paper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaperMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaperPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaperSave.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PaperUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PasswordDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QButton.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QColorSetter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QError.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QGroupBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\QuickQuiz.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ShowManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StopWatch.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SummaryLenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TabPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TempList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Test.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestBar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestCompetitiveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestMaker.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestPara.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestSetBlkTxtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestSetChsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestSetCommDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TestSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Text.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TextManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TextSimilarity.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TxtOutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TypePropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\View.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ViewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\WordByWord.cpp
# End Source File
# Begin Source File

SOURCE=.\src\XmlTags.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\AddTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AES.h
# End Source File
# Begin Source File

SOURCE=.\src\AllBuildDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\AnsiCEx.h
# End Source File
# Begin Source File

SOURCE=.\src\Base64.h
# End Source File
# Begin Source File

SOURCE=.\src\BatchSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\Blank.h
# End Source File
# Begin Source File

SOURCE=.\src\BlkTxtCheckDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\Choise.h
# End Source File
# Begin Source File

SOURCE=.\src\ChooseTypeDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ClrCommon.h
# End Source File
# Begin Source File

SOURCE=.\src\ClrDialog.h
# End Source File
# Begin Source File

SOURCE=.\src\CodeDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CopyExDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\DataList.h
# End Source File
# Begin Source File

SOURCE=.\src\define.h
# End Source File
# Begin Source File

SOURCE=.\src\Doc.h
# End Source File
# Begin Source File

SOURCE=.\src\EditDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\EditFrame.h
# End Source File
# Begin Source File

SOURCE=.\src\FindText.h
# End Source File
# Begin Source File

SOURCE=.\src\FixedRecentFileList.h
# End Source File
# Begin Source File

SOURCE=.\src\FixedSharedFile.h
# End Source File
# Begin Source File

SOURCE=.\src\ImageBar.h
# End Source File
# Begin Source File

SOURCE=.\src\ImageFrame.h
# End Source File
# Begin Source File

SOURCE=.\src\ImageManager.h
# End Source File
# Begin Source File

SOURCE=.\src\Item.h
# End Source File
# Begin Source File

SOURCE=.\src\ItemGroup.h
# End Source File
# Begin Source File

SOURCE=.\src\ItemGroupDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ItemPtr.h
# End Source File
# Begin Source File

SOURCE=.\src\ItemShowWnd.h
# End Source File
# Begin Source File

SOURCE=.\src\JpegCoverDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\Judge.h
# End Source File
# Begin Source File

SOURCE=.\src\LeftView.h
# End Source File
# Begin Source File

SOURCE=.\src\List.h
# End Source File
# Begin Source File

SOURCE=.\src\ListFrame.h
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\src\MD5.h
# End Source File
# Begin Source File

SOURCE=.\src\MemBmp.h
# End Source File
# Begin Source File

SOURCE=.\src\Paper.h
# End Source File
# Begin Source File

SOURCE=.\src\PaperMaker.h
# End Source File
# Begin Source File

SOURCE=.\src\PasswordDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\PropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\QButton.h
# End Source File
# Begin Source File

SOURCE=.\src\QColorSetter.h
# End Source File
# Begin Source File

SOURCE=.\src\QComboBox.h
# End Source File
# Begin Source File

SOURCE=.\src\QEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\QError.h
# End Source File
# Begin Source File

SOURCE=.\src\QGroupBox.h
# End Source File
# Begin Source File

SOURCE=.\src\QList.h
# End Source File
# Begin Source File

SOURCE=.\src\QProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\QTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\QuickQuiz.h
# End Source File
# Begin Source File

SOURCE=.\src\ShowManager.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\StopWatch.h
# End Source File
# Begin Source File

SOURCE=.\src\SummaryLenDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\TabPage.h
# End Source File
# Begin Source File

SOURCE=.\src\TempList.h
# End Source File
# Begin Source File

SOURCE=.\src\TestBar.h
# End Source File
# Begin Source File

SOURCE=.\src\TestCompetitiveDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\TestFrame.h
# End Source File
# Begin Source File

SOURCE=.\src\TestMaker.h
# End Source File
# Begin Source File

SOURCE=.\src\TestPara.h
# End Source File
# Begin Source File

SOURCE=.\src\TestSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\Text.h
# End Source File
# Begin Source File

SOURCE=.\src\TextManager.h
# End Source File
# Begin Source File

SOURCE=.\src\TextSimilarity.h
# End Source File
# Begin Source File

SOURCE=.\src\TMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\src\TxtOutDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\View.h
# End Source File
# Begin Source File

SOURCE=.\src\ViewDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\WordByWord.h
# End Source File
# Begin Source File

SOURCE=.\src\XmlTags.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\res\lview.bmp
# End Source File
# Begin Source File

SOURCE=.\res\QuickQuiz.ico
# End Source File
# Begin Source File

SOURCE=.\res\QuickQuiz.rc
# End Source File
# Begin Source File

SOURCE=.\res\qview.bmp
# End Source File
# Begin Source File

SOURCE=.\res\resource.h
# End Source File
# Begin Source File

SOURCE=.\res\store.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Group "XML"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lib\tinyxml\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\tinyxml\tinystr.h
# End Source File
# Begin Source File

SOURCE=.\lib\tinyxml\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\tinyxml\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\lib\tinyxml\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=.\lib\tinyxml\tinyxmlparser.cpp
# End Source File
# End Group
# End Target
# End Project
