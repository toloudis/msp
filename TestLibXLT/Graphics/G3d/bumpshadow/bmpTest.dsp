# Microsoft Developer Studio Project File - Name="bmpTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=bmpTest - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bmpTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bmpTest.mak" CFG="bmpTest - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bmpTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "bmpTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TestTerawatt/World/xtr", SSHBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bmpTest - Win32 Release"

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
# ADD CPP /nologo /G6 /W3 /GR /GX /O2 /I "." /I "Fragments" /I "\projects\Terawatt\graphics\may" /I "\projects\Terawatt\world\ent" /I "\projects\Terawatt\world\xtr" /I "\projects\Terawatt\world\til" /I "\projects\Terawatt\world\msc" /I "\projects\Terawatt\app\in" /I "\projects\Terawatt\app\ch" /I "\projects\Terawatt\world\frc" /I "\projects\Terawatt\graphics\mat" /I "\projects\Terawatt\base\dbg" /I "\projects\Terawatt\base\env" /I "\projects\Terawatt\app\fs" /I "\projects\Terawatt\app\it" /I "\projects\Terawatt\app\app" /I "\projects\Terawatt\app\app\private" /I "\projects\Terawatt\graphics\g2d" /I "\projects\Terawatt\graphics\an" /I "\projects\Terawatt\graphics\g3d" /I "\projects\Terawatt\math\ma" /I "\projects\Terawatt\math\geo" /I "\projects\Terawatt\graphics\sc" /I "\projects\Terawatt\app\gf" /D "NDEBUG" /D ENV_BUILD=51 /D "WIN32" /D "_WINDOWS" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmt.lib" /out:"bmpTestR.exe"

!ELSEIF  "$(CFG)" == "bmpTest - Win32 Debug"

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
# ADD CPP /nologo /G6 /W3 /Gm /GR /GX /ZI /Od /I "." /I "Fragments" /I "\projects\Terawatt\graphics\may" /I "\projects\Terawatt\world\ent" /I "\projects\Terawatt\world\xtr" /I "\projects\Terawatt\world\til" /I "\projects\Terawatt\world\msc" /I "\projects\Terawatt\app\in" /I "\projects\Terawatt\app\ch" /I "\projects\Terawatt\world\frc" /I "\projects\Terawatt\graphics\mat" /I "\projects\Terawatt\base\dbg" /I "\projects\Terawatt\base\env" /I "\projects\Terawatt\app\fs" /I "\projects\Terawatt\app\it" /I "\projects\Terawatt\app\app" /I "\projects\Terawatt\app\app\private" /I "\projects\Terawatt\graphics\g2d" /I "\projects\Terawatt\graphics\an" /I "\projects\Terawatt\graphics\g3d" /I "\projects\Terawatt\math\ma" /I "\projects\Terawatt\math\geo" /I "\projects\Terawatt\graphics\sc" /I "\projects\Terawatt\app\gf" /D "_DEBUG" /D ENV_BUILD=50 /D "WIN32" /D "_WINDOWS" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /FR /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt.lib" /out:"bmpTestD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "bmpTest - Win32 Release"
# Name "bmpTest - Win32 Debug"
# Begin Source File

SOURCE=.\bmpTest.cpp
# End Source File
# Begin Source File

SOURCE=.\camCameraManipOrbit.cpp
# End Source File
# Begin Source File

SOURCE=.\camCameraManipOrbit.hpp
# End Source File
# Begin Source File

SOURCE=.\camCameraMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\camCameraMgr.hpp
# End Source File
# Begin Source File

SOURCE=.\tstLightMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\tstLightMgr.hpp
# End Source File
# Begin Source File

SOURCE=.\tstMode.cpp
# End Source File
# Begin Source File

SOURCE=.\tstMode.hpp
# End Source File
# Begin Source File

SOURCE=.\tstModeBumpMaterialTest.cpp
# End Source File
# Begin Source File

SOURCE=.\tstModeBumpMaterialTest.hpp
# End Source File
# End Target
# End Project
