CALL compileShaders_x64.bat

@echo #####################################
@echo COPYING .DLL and .MI - x64
@echo #####################################

copy *.dll "C:\Projects\SourceCode\MachStudio\MachStudio-x64\Shaders\mray"
copy *.mi "C:\Projects\SourceCode\MachStudio\MachStudio-x64\Shaders\mray"

CALL clean.bat