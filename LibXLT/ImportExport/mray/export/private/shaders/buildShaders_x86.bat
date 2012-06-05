CALL compileShaders_x86.bat

@echo #####################################
@echo COPYING .DLL and .MI - x86
@echo #####################################

copy *.dll "C:\Projects\SourceCode\MachStudio\MachStudio-win32\Shaders\mray"
copy *.mi "C:\Projects\SourceCode\MachStudio\MachStudio-win32\Shaders\mray"

CALL clean.bat