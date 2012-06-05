  ; Get plug-in directory
	ReadRegStr $PluginDir HKLM "SOFTWARE\StudioGPU\${PRODUCT_INAME}" "Plug-in Dir" 
	IfErrors 0 +2
	StrCpy $PluginDir $INSTDIR

	
	; delete files
  Delete "$INSTDIR\uninst.exe"

	;Plugin
	ExecWait "regsvr32 /s /u $PluginDir\SkpStudioGpuExport.dll"
	Delete "$PluginDir\SkpStudioGpuExport.dll"
	
	; StudioGPU runtime
	Delete "$PluginDir\sgpuExportLib.dll"
  
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"

  RMDir "$SMPROGRAMS\$StartMenuFolder"
  RMDir "$INSTDIR"
