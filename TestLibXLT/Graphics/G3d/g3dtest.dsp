# Microsoft Developer Studio Project File - Name="g3dtest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=g3dtest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "g3dtest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "g3dtest.mak" CFG="g3dtest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "g3dtest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "g3dtest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/TestTerawatt/Graphics/G3d", IKHBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "g3dtest - Win32 Release"

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
# ADD CPP /nologo /W3 /GR /GX /O2 /I "\projects\terawatt\graphics\mat" /I "\projects\terawatt\graphics\may" /I "\projects\terawatt\graphics\sc" /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /I "\projects\terawatt\app\gf" /I "\projects\terawatt\app\fs" /I "\projects\terawatt\app\it" /I "\projects\terawatt\app\app" /I "\projects\terawatt\app\app\private" /I "\projects\terawatt\graphics\g2d" /I "\projects\terawatt\graphics\an" /I "\projects\terawatt\graphics\g3d" /I "\projects\terawatt\math\ma" /I "\projects\png\libpng" /I "\projects\png\zlib" /I "\projects\terawatt\graphics\g2d\private" /I "\projects\terawatt\graphics\g3d\private" /I "\projects\terawatt\app\fs\private" /I "\projects\terawatt\base\env\private" /I "\projects\terawatt\graphics\sc\private" /I "\projects\terawatt\app\ch" /I "\projects\terawatt\math\geo" /I "\projects\3rdParty\png\libpng" /I "\projects\3rdParty\zlib" /I "\projects\terawatt\graphics\mat\private" /I "\projects\terawatt\graphics\cam" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /D ENV_TEXT=42 /D ENV_BUILD=51 /D "UNICODE" /D "_UNICODE" /YX /FD /c
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

!ELSEIF  "$(CFG)" == "g3dtest - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "\projects\terawatt\graphics\mat" /I "\projects\terawatt\graphics\may" /I "\projects\terawatt\graphics\sc" /I "\projects\terawatt\base\dbg" /I "\projects\terawatt\base\env" /I "\projects\terawatt\app\gf" /I "\projects\terawatt\app\fs" /I "\projects\terawatt\app\it" /I "\projects\terawatt\app\app" /I "\projects\terawatt\app\app\private" /I "\projects\terawatt\graphics\g2d" /I "\projects\terawatt\graphics\an" /I "\projects\terawatt\graphics\g3d" /I "\projects\terawatt\math\ma" /I "\projects\png\libpng" /I "\projects\png\zlib" /I "\projects\terawatt\graphics\g2d\private" /I "\projects\terawatt\graphics\g3d\private" /I "\projects\terawatt\app\fs\private" /I "\projects\terawatt\base\env\private" /I "\projects\terawatt\graphics\sc\private" /I "\projects\terawatt\app\ch" /I "\projects\terawatt\math\geo" /I "\projects\3rdParty\png\libpng" /I "\projects\3rdParty\zlib" /I "\projects\terawatt\graphics\mat\private" /I "\projects\terawatt\graphics\cam" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D ENV_MACHINE=1 /D ENV_OS=15 /D ENV_COMPILER=30 /D ENV_BUILD=50 /FR /FD /GZ /c
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

# Name "g3dtest - Win32 Release"
# Name "g3dtest - Win32 Debug"
# Begin Source File

SOURCE=.\demG3dLayerSpec.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dLayerSpec.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestAnims.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestAnims.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestBumpMap.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestBumpMap.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestCubeMap.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestCubeMap.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestFog.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestFog.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestHierarch.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestHierarch.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestLights.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestLights.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMatOverride.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMatOverride.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMode.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMode.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestModelMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestModelMaterial.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestModelSpace.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestModelSpace.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMorph.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestMorph.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestPixelShader.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestPixelShader.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestPreLit.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestPreLit.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestProgressive.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestProgressive.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestRenderToTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestRenderToTexture.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestSpriteGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestSpriteGroup.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestTextureCompression.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestTextureCompression.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestTextureReduce.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestTextureReduce.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestTextures.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestVertexColoring.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestVertexColoring.hpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestVertexShader.cpp
# End Source File
# Begin Source File

SOURCE=.\demG3dTestVertexShader.hpp
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

SOURCE=.\g3dTest.cpp
# End Source File
# End Target
# End Project
