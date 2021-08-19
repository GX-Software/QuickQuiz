# Microsoft Developer Studio Project File - Name="libpng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libpng - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libpng - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libpng___Win32_Release"
# PROP BASE Intermediate_Dir "libpng___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "NDEBUG" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /O2 /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "NDEBUG" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /i "..\.." /d "PNG_NO_PEDANTIC_WARNINGS" /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\.." /d "PNG_NO_PEDANTIC_WARNINGS" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\libpng.lib"
# ADD LIB32 /nologo /out:"..\..\..\libpng.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libpng___Win32_Debug"
# PROP BASE Intermediate_Dir "libpng___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D PNG_DEBUG=1 /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "..\.." /I "..\..\..\zlib" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D PNG_DEBUG=1 /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "PNG_NO_PEDANTIC_WARNINGS" /d "_DEBUG"
# ADD RSC /l 0x409 /d "PNG_NO_PEDANTIC_WARNINGS" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\libpngd.lib"
# ADD LIB32 /nologo /out:"..\..\..\libpngd.lib"

!ENDIF 

# Begin Target

# Name "libpng - Win32 Release"
# Name "libpng - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\png.c
# End Source File
# Begin Source File

SOURCE=..\..\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\..\pngget.c
# End Source File
# Begin Source File

SOURCE=..\..\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\..\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\..\pngread.c
# End Source File
# Begin Source File

SOURCE=..\..\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\..\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\..\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\..\pngset.c
# End Source File
# Begin Source File

SOURCE=..\..\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\..\scripts\pngwin.def
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\..\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\..\pngwutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\png.h
# End Source File
# Begin Source File

SOURCE=..\..\pngconf.h
# End Source File
# Begin Source File

SOURCE=..\..\pngpriv.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\scripts\pngwin.rc
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Target
# End Project
