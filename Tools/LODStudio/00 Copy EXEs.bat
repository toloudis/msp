echo off
@del LODStudio.exe
@del LODStudio-D.exe
@copy "LODStudioRelease Managed.exe" LODStudio.exe
@copy "LODStudioDebug Managed.exe" LODStudio-D.exe
REM pause
