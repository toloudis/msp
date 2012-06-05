
REM Requirements:

REM --Your Max SDK should be installed to
REM %ProgramFiles(x86)%\Autodesk\3ds Max 2009 SDK

REM --Your max 2009 (32) bit should be installed to
REM %ProgramFiles(x86)%\Autodesk\3ds Max 2009

REM --Your max 2009 (64) bit should be installed to
REM %ProgramFiles%\Autodesk\3ds Max 2009

REM --You should have VSStudio 8


set SSC=SourceCode
REM set x64=1
set ProgramFiles_x86=%ProgramFiles%
if defined x64 set ProgramFiles_x86=%ProgramFiles(x86)%


call "%VS90COMNTOOLS%\..\..\VC\vcvarsall.bat"
REM call "%VS90COMNTOOLS%\vsvarsall.bat"
set "PATH=%DXSDK_DIR%Developer Runtime\x86;%PATH%"
set "PATH="c:\Projects\%SSC%\3rdParty\Dlls;%PATH%"

set MAX_PATH2009_SDK=%ProgramFiles_x86%\Autodesk\3ds Max 2009 SDK
set MAX_PATH2009=%ProgramFiles_x86%\Autodesk\3ds Max 2009
set MAX_PATH2009_X64=%ProgramFiles%\Autodesk\3ds Max 2009


set MAX_PATH2008_SDK=%ProgramFiles_x86%\Autodesk\3ds Max 2008 SDK
set MAX_PATH2008=%ProgramFiles_x86%\Autodesk\3ds Max 2008
set MAX_PATH2008_X64=%ProgramFiles%\Autodesk\3ds Max 2008



set MAX_PATH2010_SDK=%ProgramFiles_x86%\Autodesk\3ds Max 2010 SDK
set MAX_PATH2010=%ProgramFiles_x86%\Autodesk\3ds Max 2010
set MAX_PATH2010_X64=%ProgramFiles%\Autodesk\3ds Max 2010



set MAX_PATH9_SDK=%ProgramFiles_x86%\Autodesk\3ds Max 9 SDK
set MAX_PATH9=%ProgramFiles_x86%\Autodesk\3ds Max 9
set MAX_PATH9_X64=%ProgramFiles%\Autodesk\3ds Max 9


REM Please provide your maya 8.5 location here
set MAYA_8_5_LOCATION=%MAYA_LOCATION%
set MAYA_8_5_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Maya8.5
set SHAVE_8_5_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Shave\Maya8.5
REM Please provide your maya 2008 location here
set MAYA_08_LOCATION=
set MAYA_08_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Maya08
set SHAVE_08_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Shave\Maya08
REM Please provide your maya 2009 location here

set MAYA_09_LOCATION_X64=c:\Program Files\Autodesk\Maya2009
set MAYA_09_LOCATION=d:\Program Files (x86)\Autodesk\Maya2009
set MAYA_LOCATION=%MAYA_09_LOCATION%
set MAYA_09_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Maya09
set SHAVE_09_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Shave\Maya09
REM Please provide your maya 2010 location here
set MAYA_10_LOCATION_X64=D:\Program Files\Autodesk\Maya2010
set MAYA_10_LOCATION=d:\Program Files (x86)\Autodesk\Maya2010
set MAYA_10_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Maya2010
set SHAVE_10_SDK_LOCATION=d:\Projects\SourceCode-3rdParty\Shave\Maya2010
set MSIDE=devenv.exe


REM set BOOST_ROOT_CGAL36=c:\Program Files (x86)\boost\boost_1_41
start "" "%VSInstallDir%\Common7\IDE\%MSIDE%"
echo %MAYA_LOCATION%
exit