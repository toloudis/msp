# Microsoft Developer Studio Project File - Name="engineExportMaya45" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=engineExportMaya45 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "engineExportMaya45.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "engineExportMaya45.mak" CFG="engineExportMaya45 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engineExportMaya45 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "engineExportMaya45 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ToolsGeneric/Engine Maya Export", MPWBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "engineExportMaya45 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "bin/Maya4.5/Release"
# PROP Intermediate_Dir "bin/Maya4.5/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /I "\AW\Maya4.0\include" /I "." /I "\projects\Terawatt\Math\ma" /I "\projects\Terawatt\Math\geo" /I "\Projects\Terawatt\App\gf" /I "\Projects\Terawatt\App\app" /I "\Projects\Terawatt\App\it" /I "\Projects\Terawatt\App\fs" /I "\Projects\Terawatt\App\ch" /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /I "\projects\3rdParty\Maya4.5\include" /D "NDEBUG" /D "WIN32" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib vfw32.lib quartz.lib amstrmid.lib strmbase.lib ddraw.lib d3drm.lib dinput.lib dxguid.lib winmm.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /pdb:".\bin\Maya4.5\Release\engineExport.pdb" /machine:I386 /out:"\projects\3rdParty\maya4.5\bin\plug-ins\engineExport.mll" /libpath:"\AW\maya4.0\lib" /libpath:"\projects\3rdParty\Maya4.5\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "engineExportMaya45 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "bin/Maya4.5/Debug"
# PROP Intermediate_Dir "bin/Maya4.5/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "\projects\3rdParty\maya4.5\include" /I "." /I "\projects\Terawatt\Math\ma" /I "\projects\Terawatt\Math\geo" /I "\Projects\Terawatt\App\gf" /I "\Projects\Terawatt\App\app" /I "\Projects\Terawatt\App\it" /I "\Projects\Terawatt\App\fs" /I "\Projects\Terawatt\App\ch" /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /I "\projects\3rdParty\Maya4.5\include" /D "_DEBUG" /D "WIN32" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 comctl32.lib vfw32.lib quartz.lib amstrmid.lib strmbase.lib ddraw.lib d3drm.lib dinput.lib dxguid.lib winmm.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Foundation.lib OpenMaya.lib OpenMayaAnim.lib OpenMayaUI.lib /nologo /subsystem:windows /dll /pdb:".\bin\Maya4.5\Debug\engineExport.pdb" /debug /machine:I386 /out:"\projects\3rdParty\maya4.5\bin\plug-ins\engineExport.mll" /libpath:"\projects\3rdParty\maya4.5\lib" /libpath:"\projects\3rdParty\Maya4.5\lib" /export:initializePlugin /export:uninitializePlugin
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "engineExportMaya45 - Win32 Release"
# Name "engineExportMaya45 - Win32 Debug"
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

SOURCE=.\LayerFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\MayaUtil.hpp
# End Source File
# Begin Source File

SOURCE=.\MeshUtil.hpp
# End Source File
# Begin Source File

SOURCE=.\PivotFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\RemovePivots.hpp
# End Source File
# Begin Source File

SOURCE=.\RotateFigure.hpp
# End Source File
# Begin Source File

SOURCE=.\SceneFuncs.hpp
# End Source File
# Begin Source File

SOURCE=.\SetEngineFlag.hpp
# End Source File
# Begin Source File

SOURCE=.\ShadeAmbient.hpp
# End Source File
# Begin Source File

SOURCE=.\ShadeUtil.hpp
# End Source File
# Begin Source File

SOURCE=.\WriteBRep.hpp
# End Source File
# Begin Source File

SOURCE=.\WriteJoints.hpp
# End Source File
# Begin Source File

SOURCE=.\WriteLayers.hpp
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

SOURCE=.\LayerFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\MayaUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\MeshUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\PivotFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\RemovePivots.cpp
# End Source File
# Begin Source File

SOURCE=.\RotateFigure.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneFuncs.cpp
# End Source File
# Begin Source File

SOURCE=.\SetEngineFlag.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadeAmbient.cpp
# End Source File
# Begin Source File

SOURCE=.\ShadeUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteBRep.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteJoints.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteLayers.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteXForms.cpp
# End Source File
# End Target
# End Project
