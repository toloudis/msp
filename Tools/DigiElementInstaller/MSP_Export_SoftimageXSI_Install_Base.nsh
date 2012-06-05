
;--------------------------------
; HM NIS Edit Wizard helper defines

  !define PRODUCT_NAME "MSP Export Plug-in for Softimage XSI ${BASE_PRODUCT_VERSION} ${PRODUCT_PLATFORM}"
	!define PRODUCT_INAME_BASE "MSP_Export_SoftImageXSI"
	!define PRODUCT_INAME "${PRODUCT_INAME_BASE}_${BASE_PRODUCT_VERSION}_${PRODUCT_PLATFORM}"
  !define PRODUCT_BUILD "1.2.3.x"
  !define PRODUCT_DLL_NAME "MSPSoftimageExport.dll"
  !define PRODUCT_VERSION "v. ${PRODUCT_BUILD}"
	!define PRODUCT_VERSIONNS "${PRODUCT_BUILD}"
  !define PRODUCT_PUBLISHER "StudioGPU"
	!define PRODUCT_LNAME "Export Plug-in for Softimage XSI ${BASE_PRODUCT_VERSION} ${PRODUCT_PLATFORM}"
  !define PRODUCT_WEB_SITE "http://www.studiogpu.com"
	
	!define PRODUCT_REGKEY "Software\StudioGPU\${PRODUCT_INAME}"
	!define PRODUCT_DIR_REGKEY "${PRODUCT_REGKEY}\InstallPath"
  !define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  !define PRODUCT_UNINST_ROOT_KEY "HKLM"
	!define PRODUCT_MUTEX "{05C99F22-F6F8-4a6b-8AD3-59C6F8A88A5A}"

	
;--------------------------------
;Main install code
	!include "install.nsh"
