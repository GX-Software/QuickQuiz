# Microsoft Developer Studio Project File - Name="zlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=zlib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "zlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "zlib___Win32_Debug"
# PROP BASE Intermediate_Dir "zlib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "_DEBUG" /FR /FD /GZ /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /D "WIN32" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "_DEBUG" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\zlibd.lib"
# ADD LIB32 /nologo /out:"..\..\..\zlibd.lib"

!ELSEIF  "$(CFG)" == "zlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "zlib___Win32_Release"
# PROP BASE Intermediate_Dir "zlib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /D "WIN32" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "NDEBUG" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /O2 /D "WIN32" /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "NDEBUG" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\..\zlib.lib"
# ADD LIB32 /nologo /out:"..\..\..\zlib.lib"

!ENDIF 

# Begin Target

# Name "zlib - Win32 Debug"
# Name "zlib - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\adler32.c
# End Source File
# Begin Source File

SOURCE=..\..\compress.c
# End Source File
# Begin Source File

SOURCE=..\..\crc32.c
# End Source File
# Begin Source File

SOURCE=..\..\deflate.c
# End Source File
# Begin Source File

SOURCE=..\..\gzclose.c
# End Source File
# Begin Source File

SOURCE=..\..\gzlib.c
# End Source File
# Begin Source File

SOURCE=..\..\gzread.c
# End Source File
# Begin Source File

SOURCE=..\..\gzwrite.c
# End Source File
# Begin Source File

SOURCE=..\..\infback.c
# End Source File
# Begin Source File

SOURCE=..\..\inffast.c
# End Source File
# Begin Source File

SOURCE=..\..\inflate.c
# End Source File
# Begin Source File

SOURCE=..\..\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\..\trees.c
# End Source File
# Begin Source File

SOURCE=..\..\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\..\win32\zlib.def
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\zutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\crc32.h
# End Source File
# Begin Source File

SOURCE=..\..\deflate.h
# End Source File
# Begin Source File

SOURCE=..\..\inffast.h
# End Source File
# Begin Source File

SOURCE=..\..\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\..\inflate.h
# End Source File
# Begin Source File

SOURCE=..\..\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\..\trees.h
# End Source File
# Begin Source File

SOURCE=..\..\zconf.h
# End Source File
# Begin Source File

SOURCE=..\..\zlib.h
# End Source File
# Begin Source File

SOURCE=..\..\zutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\win32\zlib1.rc
# End Source File
# End Group
# End Target
# End Project
