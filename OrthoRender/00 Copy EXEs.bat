echo off
@del OrthoRender.exe
@del OrthoRender-D.exe
@copy "OrthoRender-Release.exe" OrthoRender.exe
@copy "OrthoRender-Debug.exe" OrthoRender-D.exe
REM pause
