
dir
pause

set CONFIG=release
set PYTHON_ROOT=c:\python26
set PYTHON_DLL_BUILT=boost_python-vc90-mt-1_39.dll
set PYTHON_INSTALL_DIR=%PYTHON_ROOT%\lib\site-packages\sgpuExportLib
if %CONFIG% == debug (
  set PYTHON_DLL_BUILT=boost_python-vc90-mt-gd-1_39.dll
) 

REM You need to have boost jam in the path
REM Refer to the latest boost jam installation procedures.
bjam --build-dir="%USERPROFILE%\boost_build"  --build-type=complete  --toolset=msvc  -a %CONFIG%

pause

if not exist %PYTHON_INSTALL_DIR% (
 mkdir %PYTHON_INSTALL_DIR%
 copy __init__ToBeCopiedToPythonDir.py %PYTHON_INSTALL_DIR%\__init__.py
)

copy "%USERPROFILE%\boost_build\sgpuExportLib-python\msvc-9.0\%CONFIG%\threading-multi\sgpuExportLib.pyd" %PYTHON_INSTALL_DIR%
copy "%USERPROFILE%\boost_build\boost\bin.v2\libs\python\build\msvc-9.0\%CONFIG%\threading-multi\%PYTHON_DLL_BUILT%" %PYTHON_INSTALL_DIR%
copy .\..\..\public\sgpuExportLib\bin\Win32\sgpuExportLib.dll %PYTHON_INSTALL_DIR%
pause