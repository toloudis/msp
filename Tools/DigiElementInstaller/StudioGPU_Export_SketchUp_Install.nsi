
;--------------------------------
; HM NIS Edit Wizard helper defines

  !define PRODUCT_NAME "StudioGPU Export Plug-in for Google SketchUp Pro"
	!define PRODUCT_INAME_BASE "StudioGPU_Export_SketchUp"
	!define PRODUCT_INAME "${PRODUCT_INAME_BASE}"
  !define PRODUCT_BUILD "1.2.3.3"
  !define PRODUCT_VERSION "v. ${PRODUCT_BUILD}"
	!define PRODUCT_VERSIONNS "${PRODUCT_BUILD}"
  !define PRODUCT_PUBLISHER "StudioGPU"
	!define PRODUCT_LNAME "Export Plug-in for Google SketchUp Pro"
  !define PRODUCT_WEB_SITE "http://www.studiogpu.com"
	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\SkpStudioGpuExport.dll"
  !define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  !define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_MUTEX "{0FED30FF-9F2A-4210-BF82-5EA4BEA45207}"
	!define CUSTOMINIT

;--------------------------------
;Custom inititalization
; Check for SketchUp 7

!include "FileFunc.nsh"
!insertmacro GetFileAttributes


Var PluginDir

Function CustomInit
	
	ReadRegStr $0 HKLM "SOFTWARE\Google\Google SketchUp 7\InstallLocation" ""
	IfErrors ErrorCI
	
	${GetFileAttributes} "$0" "ALL" $1 ;Check file existence
	IfErrors ErrorCI
	
	
	StrCpy $PluginDir "$0Exporters" ;Store right Google SketchUp Pro path
	Goto ExitCI
	
	ErrorCI:
	MessageBox MB_YESNO "Google SketchUp Pro 7 is not detected.$\nDo you want to install the plug-in into the main application folder anyway?" IDYES ExitCI
	
	Abort

	ExitCI:
FunctionEnd

;--------------------------------
;Main install code
	!include "install.nsh"

