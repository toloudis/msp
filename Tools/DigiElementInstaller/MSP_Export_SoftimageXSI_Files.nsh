!include "WordFunc.nsh"

 
Section "InstallVCRuntime" SEC00
	
	SetOutPath "$TEMP"
	!ifdef WINDOWS_X64
		File "VC2008SP1_x64\vcredist_${PRODUCT_PLATFORM_2}.exe"
	!else
  		File "VC2008SP1\vcredist_${PRODUCT_PLATFORM_2}.exe"
	!endif
	
  SetOverwrite on
	StrCpy $1 "$TEMP\vcredist_${PRODUCT_PLATFORM_2}.exe /q"
  ;ShowWindow $HWNDPARENT 6
	ExecWait $1
	Delete $1
	;ShowWindow $HWNDPARENT 9
	
SectionEnd

Section "Main files" SEC01

  ;SetShellVarContext all
  
  SetOutPath "$INSTDIR"
  SetOverwrite on

	;File "${PRODUCT_INAME}.ico"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
!insertmacro MUI_STARTMENU_WRITE_END

Call WritePlugin

SectionEnd

Function WriteFiles
	
	;Plugin
	File "..\SoftImage Export\bin\${PRODUCT_PLATFORM}\${BASE_PRODUCT_VERSION_2}\${PRODUCT_DLL_NAME}"
	
	; StudioGPU runtime
	File "..\ExporterPlugin\public\sgpuExportLib\bin\${PRODUCT_PLATFORM}\sgpuExportLib.dll"
	
FunctionEnd

Function Un.DeleteFiles
	
	;Plugin
	Delete "$7\${PRODUCT_DLL_NAME}"
	
	; StudioGPU runtime
	Delete "$7\sgpuExportLib.dll"
	
FunctionEnd

!define XSIPath "SOFTWARE\Softimage\SOFTIMAGE Application";
!define XSIPathVer "SOFTWARE\Softimage\CoExistence";

Function WritePlugin
	
	StrCpy $0 0
	StrCpy $4 0
	
loop:
  EnumRegKey $1 HKLM "${XSIPath}" $0
  StrCmp $1 "" done
	
		
	ReadRegStr $7 HKLM "${XSIPath}\$1\Root Locations" "InstallRoot"

	StrCmp $7 "" next
	
	ReadRegStr $2 HKLM "${XSIPathVer}\$1" "AppVersion"
	StrCmp $2 "" next
	
	${VersionCompare} $2 "8.0.201.0" $3
	IntCmp $3 2 next
	
	IntOp $4 $4 + 1 ;//XSI found

;--------------------------------
	
		
	StrCpy $7 "$7\Application\Plugins"
	WriteRegStr HKEY_LOCAL_MACHINE "${PRODUCT_REGKEY}\Installs\$4" "Path" "$7"
	
	SetOutPath "$7"
	Call WriteFiles
	
	;--------------------------------
	

next:
	IntOp $0 $0 + 1
  Goto loop
done:

	IntCmp $4 0 0 0 alldone
	
	MessageBox MB_OK "Installer has not found Softimage 7.0.$\r$\nAfter installing Softimage, please copy all DLLs from installation folder into Softimage's plug-in folder$\r$\nor reinstall plug-in." 

	;Write plug-in into default location
	WriteRegStr HKEY_LOCAL_MACHINE "${PRODUCT_REGKEY}\Installs\0" "Path" "$INSTDIR"
	Call WriteFiles

alldone:
  ;WriteRegStr HKLM "Software\Bare Software\PSD" "Server" ${PRODUCT_SERVER}
	
FunctionEnd