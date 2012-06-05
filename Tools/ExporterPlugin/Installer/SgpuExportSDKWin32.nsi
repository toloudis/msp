
!define PRODUCT_NAME "Sgpu Exporter SDK Win32"
!define PRODUCT_VERSION "1.2.4.22"
!define PRODUCT_PUBLISHER "studio|gpu"
!define PRODUCT_WEB_SITE "http://www.StudioGPU.com"
!define OUTPUT_EXE "SgpuExporterSDKWin32.exe"
!define PRODUCT_INSTALL_DIR "$PROGRAMFILES\StudioGPU"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
BrandingText " "

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES
; Language files
!insertmacro MUI_LANGUAGE "English"

var Company
var Product
var ProductOld
var ProductUninstallEXE
var ProductEXE
var ProductDir
var UninstallDir
var UserDataFolder
var ArtDataFolder
var InstallFile
var Is32
  
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${OUTPUT_EXE}"
InstallDir "${PRODUCT_INSTALL_DIR}"
ShowInstDetails show
ShowUnInstDetails show

Function checkPrevious
  IfFileExists "$PROGRAMFILES\StudioGPU\Studio GPU Exporter SDK Win32" renameFolder1 end1
  renameFolder1:
    rename "$PROGRAMFILES\StudioGPU\Studio GPU Exporter SDK Win32" "$PROGRAMFILES\StudioGPU\Studio GPU Exporter SDK Win32.old"
  end1:
FunctionEnd

Function checkPrevious_uninst
  IfFileExists "$PROGRAMFILES\StudioGPU\SgpuExporterSDKWin32_uninst.exe" renameFolder2 end2
  renameFolder2:
    rename "$PROGRAMFILES\StudioGPU\SgpuExporterSDKWin32_uninst.exe" "$PROGRAMFILES\StudioGPU\SgpuExporterSDKWin32_uninst.old.exe"
  end2:
FunctionEnd


Function checkPrevious2
  IfFileExists "$PROGRAMFILES\Studio GPU" renameFolder3 foo3
  renameFolder3:
    rename "$PROGRAMFILES\Studio GPU" "$PROGRAMFILES\Studio GPU.old"
  foo3:
FunctionEnd


Function delPrevious
  IfFileExists "$PROGRAMFILES\StudioGPU\Studio GPU Exporter SDK Win32.old" removeFolder1 end1
  removeFolder1:
    RMDir /r  "$PROGRAMFILES\StudioGPU\Studio GPU Exporter SDK Win32.old"
  end1:
FunctionEnd

Function delPrevious_uninst
   IfFileExists "$PROGRAMFILES\StudioGPU\SgpuExporterSDKWin32_uninst.old.exe" removeFolder2 end2
  removeFolder2:
    Delete  "$PROGRAMFILES\StudioGPU\SgpuExporterSDKWin32_uninst.old.exe"
  end2:
FunctionEnd

Function delPrevious2
  IfFileExists "$PROGRAMFILES\Studio GPU.old" removeFolder1 end1
  removeFolder1:
    RMDir /r  "$PROGRAMFILES\Studio GPU.old"
  end1:
FunctionEnd



Function .onInstSuccess
  Call delPrevious
  Call delPrevious2
  Call delPrevious_uninst
FunctionEnd

Function .onInit
  InitPluginsDir
  File /oname=$PLUGINSDIR\welcome.bmp "${NSISDIR}\Contrib\Graphics\Wizard\orange-nsis.bmp"
  System::Call "kernel32::GetCurrentProcess() i .s"
  System::Call "kernel32::IsWow64Process(i s, *i .r0)"
  StrCmp $0 0 is32bit is64bit
  is32bit:
    StrCpy $Is32 "True"
    IntCmp 0 0 end
  is64bit:
    StrCpy $Is32 "False"
    IntCmp 0 0 end
  end:
    MessageBox MB_OK "Rmoving Previous Installations If Any"
    Call checkPrevious
    Call checkPrevious_uninst
    Call checkPrevious2
FunctionEnd


Function setupVars
  StrCpy $Company "StudioGPU"
  StrCpy $Product "Studio GPU Exporter SDK Win32"
  StrCpy $ProductOld "Studio GPU Exporter SDK Win32.Old"
  StrCpy $ProductUninstallEXE "SgpuExporterSDKWin32_uninst.exe"
  StrCpy $ProductEXE "SgpuExporterSDKWin32"
  StrCpy $ProductDir "$INSTDIR\Studio GPU Exporter SDK Win32"
  StrCpy $UninstallDir "$INSTDIR"
  StrCpy $UserDataFolder "$LOCALAPPDATA\StudioGPU"
  StrCpy $ArtDataFolder "$DOCUMENTS\StudioGPU"
FunctionEnd


Function un.setupVars
  StrCpy $Company "StudioGPU"
  StrCpy $Product "Studio GPU Exporter SDK Win32"
  StrCpy $ProductUninstallEXE "SgpuExporterSDKWin32_uninst.exe"
  StrCpy $ProductEXE "SgpuExporterSDKWin32"
  StrCpy $ProductDir "$INSTDIR\Studio GPU Exporter SDK Win32"
  StrCpy $UninstallDir "$INSTDIR"
FunctionEnd

Section -
  Call setupVars
SectionEnd

Section "Sgpu Exporter SDK Win32" SEC01
  SectionIn RO
  Createdirectory "$INSTDIR\$Product"
  SetOutPath "$INSTDIR\$Product"
  SetOverwrite ifnewer
  File /r /x "*.svn*" /x *x64*  "..\public\sgpuExportLib\*.*"
SectionEnd

Section -
  WriteUninstaller "$UninstallDir\$ProductUninstallEXE"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} ""
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd


Section "Visual C++ redist" SEC04
  ;StrCmp $Is32 "True" is32bit is64bit
  ;is32bit:
    StrCpy $InstallFile '"Support Files\vcredist\vcredist_x86_VS2005SP1.exe" /Q'
    ExecWait $InstallFile
    StrCpy $InstallFile '"Support Files\VCredist\vcredist_x86_VS2008SP1.exe" /silent'
    IntCmp 0 0 install
  ;is64bit:
    ;StrCpy $InstallFile '"Support Files\vcredist\vcredist_x64_VS2005SP1.exe" /Q'
    ;ExecWait $InstallFile
    ;StrCpy $InstallFile '"Support Files\VCredist\vcredist_x64_VS2008SP1.exe" /silent'
    ;IntCmp 0 0 install
  install:
    ExecWait $InstallFile
SectionEnd

Section un.-
  Call un.setupVars
SectionEnd

Section Uninstall
  Delete "$INSTDIR\$ProductUninstallEXE"
  RMDir /r "$INSTDIR\$Product"
  SetAutoClose true
SectionEnd