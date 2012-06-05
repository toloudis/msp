  ; delete files
  Delete "$INSTDIR\uninst.exe"

	
	;Delete every plug-in file
	
	StrCpy $0 0
	
loop:
	EnumRegKey $1 HKLM "${PRODUCT_REGKEY}\Installs" $0
  StrCmp $1 "" done
			
	ReadRegStr $7 HKLM "${PRODUCT_REGKEY}\Installs\$1" "Path"
	StrCmp $7 "" next
	
	;--------------------------------
			
	
	Call Un.DeleteFiles
			
	;--------------------------------
	

next:
	IntOp $0 $0 + 1
  Goto loop
done:
	
	DeleteRegKey HKLM "${PRODUCT_REGKEY}\Installs"
  
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"

  RMDir "$SMPROGRAMS\$StartMenuFolder"
  RMDir "$INSTDIR"
