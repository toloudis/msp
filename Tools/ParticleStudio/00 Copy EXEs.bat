echo off
@del ParticleStudio.exe
@del ParticleStudio-D.exe
@copy "ParticleStudioRelease Managed.exe" ParticleStudio.exe
@copy "ParticleStudioDebug Managed.exe" ParticleStudio-D.exe
REM pause
