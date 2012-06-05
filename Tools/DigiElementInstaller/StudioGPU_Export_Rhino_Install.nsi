
;--------------------------------
; HM NIS Edit Wizard helper defines

  !define PRODUCT_NAME "StudioGPU Export Plug-in for Rhinoceros"
	!define PRODUCT_INAME_BASE "StudioGPU_Export_Rhino"
	!define PRODUCT_INAME "${PRODUCT_INAME_BASE}"
  !define PRODUCT_BUILD "1.2.3.1"
  !define PRODUCT_VERSION "v. ${PRODUCT_BUILD}"
	!define PRODUCT_VERSIONNS "${PRODUCT_BUILD}"
  !define PRODUCT_PUBLISHER "StudioGPU"
	!define PRODUCT_LNAME "Export Plug-in for Rhinoceros"
  !define PRODUCT_WEB_SITE "http://www.studiogpu.com"
	!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\StudioGPUExport.rhp"
  !define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  !define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_MUTEX "{482F52CF-E4B2-47AD-8DC1-B29344C1714E}"
	
;--------------------------------
;Main install code
	!include "install.nsh"
