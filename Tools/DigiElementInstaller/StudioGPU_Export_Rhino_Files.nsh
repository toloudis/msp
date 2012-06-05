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

  ;SetShellVarContext all
  
  SetOutPath "$INSTDIR"
  SetOverwrite on

	;Plugin
	File "..\Rhino Export\Bin\release\Win32\StudioGPUExport.rhp"
	
	; StudioGPU runtime
	File "..\ExporterPlugin\public\sgpuExportLib\bin\win32\sgpuExportLib.dll"
    	
  ;File "${PRODUCT_INAME}.ico"

!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
!insertmacro MUI_STARTMENU_WRITE_END

Call RegisterPlugin
  
SectionEnd

!define RhinoPath "SOFTWARE\McNeel\Rhinoceros\4.0";
!define PluginID "482f52cf-e4b2-47ad-8dc1-b29344c1714e"

Function RegisterPlugin
	
	StrCpy $0 0
	StrCpy $4 0
	
loop:
  EnumRegKey $1 HKLM "${RhinoPath}" $0
  StrCmp $1 "" done
	
	ReadRegStr $2 HKLM "${RhinoPath}\$1\Install" "Edition"

	;StrCmp $2 "Release" 0 next
	StrCmp $2 "" next
	
	ReadRegStr $2 HKLM "${RhinoPath}\$1\Install" "Version"
	StrCmp $2 "" next
	
	${VersionCompare} $2 "4.0.40226" $3
	IntCmp $3 2 next
		
	StrCpy $4 1 ;//Rhino found
	;--------------------------------
	
	WriteRegDWORD HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "AddToHelpMenu" 0x0
	WriteRegDWORD HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "LoadMode" 0x2
	WriteRegDWORD HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "PreviousLoadMode" 0x2
	WriteRegDWORD HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Type" 0x2
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Name" "GXB Export plug-in"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Organization" "StudioGPU"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Address" "1680 Vine Street suite 1010$\r$\nLos Angeles, CA 90028"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Country" "United States"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Phone" "323.544.1003"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "EMail" "support@studiogpu.com"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "WebSite" "http://www.studiogpu.com"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "UpdateURL" "http://www.studiogpu.com/support"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "Fax" "323.544.1003"
	WriteRegDWORD HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "IsDotNETPlugIn" 0x0
	WriteRegDWORD HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "LanguageID" 0x409
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-ins\${PluginID}" "RegPath" "\\HKEY_LOCAL_MACHINE\${RhinoPath}\$1\Plug-ins\${PluginID}"
	
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-Ins\${PluginID}\CommandList" "StudioGPUExport" "2;StudioGPUExport"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-Ins\${PluginID}\FileTypes" "StudioGPU files (*.gxb)" "gxb"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-Ins\${PluginID}\PlugIn" "FileName" "$INSTDIR\StudioGPUExport.rhp"
	WriteRegStr HKEY_LOCAL_MACHINE "${RhinoPath}\$1\Plug-Ins\${PluginID}\PlugIn" "FolderName" "$INSTDIR\"

	
	;--------------------------------
	
	
	

next:
	IntOp $0 $0 + 1
  Goto loop
done:

	IntCmp $4 1 alldone
	
	MessageBox MB_OK "Installer has not found Rhinoceros 4.0 SR 5b or higher.$\r$\nAfter installing Rhinoceros, please register plug-in manually or rerun installer." 

alldone:
  ;WriteRegStr HKLM "Software\Bare Software\PSD" "Server" ${PRODUCT_SERVER}
	
FunctionEnd