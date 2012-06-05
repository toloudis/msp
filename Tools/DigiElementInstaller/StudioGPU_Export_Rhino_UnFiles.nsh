  ; delete files
  Delete "$INSTDIR\uninst.exe"

	;Plugin
	Delete "$INSTDIR\StudioGPUExport.rhp"
	
	; StudioGPU runtime
	Delete "$INSTDIR\sgpuExportLib.dll"
  
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"

  RMDir "$SMPROGRAMS\$StartMenuFolder"
  RMDir "$INSTDIR"
