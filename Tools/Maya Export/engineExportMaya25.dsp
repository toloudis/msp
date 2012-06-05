# Microsoft Developer Studio Project File - Name="engineExportMaya25" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=engineExportMaya25 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "engineExportMaya25.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "engineExportMaya25.mak" CFG="engineExportMaya25 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engineExportMaya25 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "engineExportMaya25 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ToolsGeneric/Engine Maya Export", MPWBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "engineExportMaya25 - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /I "." /I "\AW\maya2.5\include" /I "\Projects\Terawatt\App\gf" /I "\Projects\Terawatt\App\it" /I "\Projects\Terawatt\App\fs" /I "\Projects\Terawatt\App\ch" /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /D "NDEBUG" /D ENV_BUILD=51 /D "WIN32" /D "_WINDOWS" /D "NT_PLUGIN" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /D ENV_TEXT=42 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib /nologo /subsystem:windows /dll /pdb:".\Release\engineExport.pdb" /machine:I386 /out:"\AW\maya2.5\bin\plug-ins\engineExport.mll" /libpath:"\AW\maya2.5\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "engineExportMaya25 - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "." /I "\AW\maya2.5\include" /I "\Projects\Terawatt\App\gf" /I "\Projects\Terawatt\App\it" /I "\Projects\Terawatt\App\fs" /I "\Projects\Terawatt\App\ch" /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /D "_DEBUG" /D ENV_BUILD=50 /D "WIN32" /D "_WINDOWS" /D "NT_PLUGIN" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /D ENV_TEXT=42 /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 comctl32.lib vfw32.lib quartz.lib amstrmid.lib strmbase.lib ddraw.lib d3drm.lib dinput.lib dxguid.lib winmm.lib OpenMayaAnim.lib OpenMayaUI.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib /nologo /subsystem:windows /dll /pdb:".\Debug\engineExport.pdb" /debug /machine:I386 /out:"\AW\maya2.5\bin\plug-ins\engineExport.mll" /libpath:"\AW\maya2.5\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "engineExportMaya25 - Win32 Release"
# Name "engineExportMaya25 - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter ".h,.hpp"
# Begin Source File

SOURCE=.\AnimFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\BakeAnim.hpp
# End Source File
# Begin Source File

SOURCE=.\GetFilename.hpp
# End Source File
# Begin Source File

SOURCE=.\JointFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\MayaUtil.hpp
# End Source File
# Begin Source File

SOURCE=.\PivotFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\RemovePivots.hpp
# End Source File
# Begin Source File

SOURCE=.\SceneFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\SetEngineFlag.hpp
# End Source File
# Begin Source File

SOURCE=.\WriteBRep.hpp
# End Source File
# Begin Source File

SOURCE=.\WriteJoints.hpp
# End Source File
# Begin Source File

SOURCE=.\WriteXForms.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\AnimFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\BakeAnim.cpp
# End Source File
# Begin Source File

SOURCE=.\engineExport.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFilename.cpp
# End Source File
# Begin Source File

SOURCE=.\JointFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\MayaUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\PivotFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\RemovePivots.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\SetEngineFlag.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteBRep.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteJoints.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteXForms.cpp
# End Source File
# End Target
# End Project
