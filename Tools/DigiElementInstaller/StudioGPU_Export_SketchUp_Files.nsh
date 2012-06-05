!include "WordFunc.nsh"

Section "InstallVCRuntime" SEC00
	
	SetOutPath "$TEMP"
	File "VC2008SP1\vcredist_x86.exe"
  SetOverwrite on
	StrCpy $1 "$TEMP\vcredist_x86.exe /q"
  ;ShowWindow $HWNDPARENT 6
	ExecWait $1
	Delete $1
	;ShowWindow $HWNDPARENT 9
	
SectionEnd


Section "Main files" SEC01

	; Get plug-in directry
	StrCmp $PluginDir "" 0 +2
	StrCpy $PluginDir "$INSTDIR"
	
	WriteRegStr HKLM "SOFTWARE\StudioGPU\${PRODUCT_INAME}" "Plug-in Dir" $PluginDir
	
  ;SetShellVarContext all
  
	; Install
  SetOutPath "$PluginDir"
  SetOverwrite on
	
	; StudioGPU runtime
	File "..\ExporterPlugin\public\sgpuExportLib\bin\Win32\sgpuExportLib.dll"
	
	;Plugin
	File "..\SketchUp Export\Bin\Release\Win32\SkpStudioGpuExport.dll"
	ExecWait 'regsvr32 /s "$PluginDir\SkpStudioGpuExport.dll"'
		
	SetOutPath "$INSTDIR"
    	
  ;File "${PRODUCT_INAME}.ico"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
!insertmacro MUI_STARTMENU_WRITE_END
 
SectionEnd

