# Microsoft Developer Studio Project File - Name="shadowtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=shadowtest - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "shadowtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "shadowtest.mak" CFG="shadowtest - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "shadowtest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "shadowtest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TestTerawatt/g3dTest", CAQAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "shadowtest - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /I "\projects\terawatt\app\gf" /I "\projects\terawatt\app\fs" /I "\projects\terawatt\app\it" /I "\projects\terawatt\app\app" /I "\projects\terawatt\app\app\private" /I "\projects\terawatt\graphics\g2d" /I "\projects\terawatt\graphics\an" /I "\projects\terawatt\graphics\g3d" /I "\projects\terawatt\math\ma" /I "\projects\terawatt\graphics\may" /I "\projects\terawatt\graphics\sc" /I "\projects\terawatt\graphics\mat" /I "\projects\terawatt\graphics\cam" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /D ENV_TEXT=42 /D ENV_BUILD=51 /D "UNICODE" /D "_UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib d3d8.lib d3dx8.lib Advapi32.lib /nologo /subsystem:windows /machine:I386 /out:"g3dTestR.exe"

!ELSEIF  "$(CFG)" == "shadowtest - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /I "\projects\terawatt\app\gf" /I "\projects\terawatt\app\fs" /I "\projects\terawatt\app\it" /I "\projects\terawatt\app\app" /I "\projects\terawatt\app\app\private" /I "\projects\terawatt\graphics\g2d" /I "\projects\terawatt\graphics\an" /I "\projects\terawatt\graphics\g3d" /I "\projects\terawatt\math\ma" /I "\projects\terawatt\graphics\may" /I "\projects\terawatt\graphics\sc" /I "\projects\terawatt\graphics\mat" /I "\projects\terawatt\graphics\cam" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /D ENV_BUILD=50 /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib d3d8.lib d3dx8.lib Advapi32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt" /out:"g3dTestD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "shadowtest - Win32 Release"
# Name "shadowtest - Win32 Debug"
# Begin Source File

SOURCE=.\demG3dTestHierarch.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestHierarch.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMode.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMode.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestShadow.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestShadow.hpp
# End Source File
# Begin Source File

SOURCE=.\demMode.cpp
# End Source File
# Begin Source File

SOURCE=.\demMode.hpp
# End Source File
# Begin Source File

SOURCE=.\demModeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\demModeManager.hpp
# End Source File
# Begin Source File

SOURCE=.\ShadowTest.cpp
# End Source File
# End Target
# End Project
