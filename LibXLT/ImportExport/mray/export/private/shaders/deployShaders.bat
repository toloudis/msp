@echo #####################################
@echo COMPILING AND LINKING SHADERS - x86
@echo #####################################
CALL setupVars_x86.bat
CALL compileShaders_x86.bat

@echo #####################################
@echo COPYING .DLL and .MI - x86
@echo #####################################
copy *.dll "C:\Projects\MachStudioPro-Dev\Shaders\mray"
copy *.mi "C:\Projects\MachStudioPro-Dev\Shaders\mray"
call clean.bat

@echo #####################################
@echo COMPILING AND LINKING SHADERS - x64
@echo #####################################
CALL setupVars_x64.bat
CALL compileShaders_x64.bat

@echo #####################################
@echo COPYING .DLL and .MI - x64
@echo #####################################
copy *.dll "C:\Projects\MachStudioPro-Dev-x64\Shaders\mray"
copy *.mi "C:\Projects\MachStudioPro-Dev-x64\Shaders\mray"
CALL clean.bat