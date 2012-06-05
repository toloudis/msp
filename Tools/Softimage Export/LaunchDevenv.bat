
REM Requirements:

REM --Your Max SDK should be installed to
REM %ProgramFiles(x86)%\Autodesk\3ds Max 2009 SDK

REM --Your max 2009 (32) bit should be installed to
REM %ProgramFiles(x86)%\Autodesk\3ds Max 2009

REM --Your max 2009 (64) bit should be installed to
REM %ProgramFiles%\Autodesk\3ds Max 2009

REM --You should have VSStudio 8


set SSC=SourceCode
set x64=1
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
REM Please provide your maya 2009 location here
set MAYA_09_LOCATION=D:\Program Files (x86)\Autodesk\Maya2009
REM Please provide your maya 2010 location here
set MAYA_10_LOCATION=


set RhinoSDKPath=D:\Program Files (x86)\Rhino 4.0 SDK

REM set XSISDK_ROOT=
set XSISDK_ROOT=C:\Softimage\Softimage_2010\XSISDK
set XSISDK_ROOT_X64=C:\Softimage\Softimage_2010_x64\XSISDK
rem set XSISDK_ROOT=D:\Program Files (x86)\Autodesk\Softimage 2011\XSISDK
rem set XSISDK_ROOT_X64=D:\Program Files\Autodesk\Softimage 2011\XSISDK
REM if defined x64 set XSISDK_ROOT=C:\Softimage\Softimage_2010_x64\XSISDK
set XSISDK_2010_ROOT=%XSISDK_ROOT%
set XSISDK_2010_ROOT_X64=%XSISDK_ROOT_X64%
set XSISDK_75_ROOT=D:\Softimage\Softimage_7.5\XSISDK
set XSISDK_75_ROOT_X64=D:\Softimage\Softimage_7.5_x64\XSISDK
set XSISDK_2011_ROOT=D:\Program Files (x86)\Autodesk\Softimage 2011\XSISDK
set XSISDK_2011_ROOT_X64=D:\Program Files\Autodesk\Softimage 2011\XSISDK
set MSIDE=devenv.exe

start "" "%VSInstallDir%\Common7\IDE\%MSIDE%"
exit
