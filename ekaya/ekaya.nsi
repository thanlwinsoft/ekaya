;NSIS Modern User Interface
;Ekaya NSIS Installer script
;Written by Keith Stribley

; Some useful definitions that may need changing for different versions
!ifndef VERSION
  !define VERSION '0.1.6'
!endif

!ifndef EKAYA_BINARY_DIR
	!define EKAYA_BINARY_DIR "Release"
!endif

!ifndef WINKMFL_BINARY_DIR
	!define WINKMFL_BINARY_DIR "..\winkmfl\Release"
!endif

!ifndef ICONV_BINARY_DIR
	!define ICONV_BINARY_DIR "..\..\win-iconv\bin"
!endif

!define APP_NAME 'Ekaya'
!define INSTALL_SUFFIX "ThanLwinSoft.org"

!if ${ARCH} == 'x86_64'
	!define LIBRARY_X64 1
	!define REDIST "vcredist_x64.exe"
	InstallDir $PROGRAMFILES64\${INSTALL_SUFFIX}
!else
	!define REDIST "vcredist_x86.exe"
	InstallDir $PROGRAMFILES\${INSTALL_SUFFIX}
!endif

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

; SendMessage
  !include WinMessages.nsh

;--------------------------------
;General

  ;Name and file
  Name "${APP_NAME} (${VERSION} ${ARCH})"
  Caption "Ekaya Input Method"

  OutFile "${APP_NAME}-${VERSION}_${ARCH}.exe"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\${INSTALL_SUFFIX}\${APP_NAME}_${ARCH}" ""
  
  SetCompressor lzma

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "COPYING"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
  Icon "ekaya.ico"
  UninstallIcon "ekayaUninstall.ico"
;Installer Sections



Section "-!${APP_NAME}" SecApp
  
  IfFileExists "$INSTDIR\${APP_NAME}" 0 BranchNoExist
    
    MessageBox MB_YESNO|MB_ICONQUESTION "Would you like to overwrite existing ${APP_NAME} directory?" IDNO  NoOverwrite ; skipped if file doesn't exist

    BranchNoExist:
  
    SetOverwrite on ; NOT AN INSTRUCTION, NOT COUNTED IN SKIPPINGS

NoOverwrite:

  SetOutPath "$INSTDIR"

  CreateDirectory "$INSTDIR\${APP_NAME}"
  SetOutPath "$INSTDIR\${APP_NAME}"

!if ${ARCH} == 'x86_64'
	SetRegView 64
	# FIXME debug hack
	# MessageBox MB_OK "Info using 64 bit programfiles=$PROGRAMFILES64 system=$SYSDIR install=$INSTDIR"
!else
	SetRegView 32
!endif

  File "ekaya.ico"
  File /oname=license.txt "COPYING"

  ;File "Uninstall.ico"
  
  File "${EKAYA_BINARY_DIR}\ekaya.dll"
  File /nonfatal "${EKAYA_BINARY_DIR}\ekaya.dll*.manifest*"
  File "${WINKMFL_BINARY_DIR}\winkmfl.dll"
  File /nonfatal "${WINKMFL_BINARY_DIR}\winkmfl.dll*.manifest*"
  File "${ICONV_BINARY_DIR}\iconv.dll"
  File "${KMFLTEST_BINARY_DIR}\kmfltest.exe"
  File /nonfatal "${KMFLTEST_BINARY_DIR}\kmfltest.exe*.manifest*"
  File /r "doc"
  
  ; Redist dlls
  File redist\${REDIST}
  #File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcp90.dll"
  #File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcr90.dll"
  #File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcm90.dll"
  #File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"

  ClearErrors
  ExecWait '$INSTDIR\${APP_NAME}\${REDIST} /q' $0
  IfErrors 0 +2
	MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: failed to install Microsoft Redistributable DLLs"

  ClearErrors
!if ${ARCH} == 'x86_64'  
  ExecWait '"$SYSDIR\regsvr32" "$INSTDIR\${APP_NAME}\ekaya.dll"' $0
!else
  RegDLL "$INSTDIR\${APP_NAME}\ekaya.dll"
!endif

  IfErrors 0 +2
	MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: Regsvr32 failed to register Ekaya Text Service DLL"
  
  CreateDirectory "$INSTDIR\${APP_NAME}\kmfl"
  
  SetShellVarContext all
  ; set up shortcuts
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" \
	"$INSTDIR\${APP_NAME}\doc\ekaya.html" '' \
	"$INSTDIR\${APP_NAME}\ekaya.ico" 0 SW_SHOWNORMAL \
	"" "${APP_NAME}"
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall ${APP_NAME}.lnk" \
	"$INSTDIR\${APP_NAME}\Uninstall.exe" "" \
	"$INSTDIR\${APP_NAME}\Uninstall.exe" 0 SW_SHOWNORMAL \
	"" "Uninstall ${APP_NAME}"
	
  ;Store installation folder
  WriteRegStr HKLM "Software\${INSTALL_SUFFIX}\${APP_NAME}_${ARCH}" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\${APP_NAME}\Uninstall.exe"

  ; Do we need to send a message that these have changed? Will they take affect on reboot?
  ; Make sure langbar is visible at least for user installing it try HKU?
  WriteRegDWORD HKCU "SOFTWARE\Microsoft\CTF\LangBar" "ExtraIconsOnMinimized" 0x1  
  # 3 is hidden 0 or 4 are ok
  ReadRegDWORD $0 HKCU "SOFTWARE\Microsoft\CTF\LangBar" "ShowStatus"
  IntCmp $0 3 0 NoChange NoChange
	WriteRegDWORD HKCU "SOFTWARE\Microsoft\CTF\LangBar" "ShowStatus" 0x0

NoChange:
  
  ; Enable Advanced Text Services for all programs on XP
  ClearErrors
  ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\CTF\SystemShared" "CUAS"
  IfErrors SkipCUAS 0
  IntCmp $0 1 SkipCUAS 0 0
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\CTF\SystemShared" "CUAS" 0x1
  SetRebootFlag true

SkipCUAS:
  ; add keys for Add/Remove Programs entry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
                 "DisplayName" "${APP_NAME} ${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" \
                 "UninstallString" "$INSTDIR\${APP_NAME}\ekayaUninstall.exe"

SectionEnd

;Optional source - as a compressed archive
; hg archive -t tbz2 ekaya-0.1.0.tar.bz2
Section /o "Source" SecSource
	SetOutPath "$INSTDIR\${APP_NAME}"
	; generate the archive using mercurial
	!system "hg archive -t tbz2 ..\ekaya-${VERSION}.tar.bz2"
	File ..\ekaya-${VERSION}.tar.bz2
SectionEnd

; Set an environment variable to allow Windows GTK apps to use Ekaya and other
; IMEs by default
Section "GTK IM Module" SecGtkImModule
    WriteRegStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" \
        "GTK_IM_MODULE" "ime"
    SendMessage HWND_BROADCAST WM_SETTINGCHANGE 0 "STR:Environment" /TIMEOUT=5000
SectionEnd

; Add more keyboard sections here as needed
Section "MyWin Burmese Unicode 5.2 keyboard" SecMyWin
	SetOutPath "$INSTDIR\${APP_NAME}\kmfl"
	File "..\keyboards\kmfl\myWin.png"
	File "..\keyboards\kmfl\myWin.bmp"
	File "..\keyboards\kmfl\myWin.jpg"
	File "..\keyboards\kmfl\myWin.kmn"
	File "..\keyboards\kmfl\myWin.html"
	File "..\keyboards\kmfl\myWin_my.html"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\myWin Keyboard.lnk" \
		"$INSTDIR\${APP_NAME}\kmfl\myWin.html" '' \
		"" 0 SW_SHOWNORMAL \
		"" "myWin Keyboard"
SectionEnd

Section /o "Myanmar3 Unicode 5.2 keyboard" SecMyanmar3
	SetOutPath "$INSTDIR\${APP_NAME}\kmfl"
	File "..\keyboards\kmfl\myan3.bmp"
	File "..\keyboards\kmfl\myanmar3std.kmn"
	File "..\keyboards\kmfl\myanmar3std.html"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\Myanmar3 Keyboard.lnk" \
		"$INSTDIR\${APP_NAME}\kmfl\myanmar3std.html" '' \
		"" 0 SW_SHOWNORMAL \
		"" "Myanmar3 Keyboard"
SectionEnd

Section /o "Sgaw Karen Unicode 5.2 keyboard" SecSgawKaren
	SetOutPath "$INSTDIR\${APP_NAME}\kmfl"
	File "..\keyboards\kmfl\SgawKaren.bmp"
	File "..\keyboards\kmfl\SgawKaren.png"
	File "..\keyboards\kmfl\SgawKaren.kmn"
	File "..\keyboards\kmfl\SgawKaren.html"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\Sgaw Karen Keyboard.lnk" \
		"$INSTDIR\${APP_NAME}\kmfl\SgawKaren.html" '' \
		"" 0 SW_SHOWNORMAL \
		"" "Sgaw Karen Keyboard"
SectionEnd

Section /o "Pa-O Unicode 5.2 keyboard" SecPaO
	SetOutPath "$INSTDIR\${APP_NAME}\kmfl"
	File "..\keyboards\kmfl\pa-oh.bmp"
	File "..\keyboards\kmfl\pa-oh.png"
	File "..\keyboards\kmfl\pa-oh.kmn"
	File "..\keyboards\kmfl\pa-oh.html"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\Pa-O Keyboard.lnk" \
		"$INSTDIR\${APP_NAME}\kmfl\pa-oh.html" '' \
		"" 0 SW_SHOWNORMAL \
		"" "Pa-O Keyboard"
SectionEnd
;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecApp ${LANG_ENGLISH} "Install the ${APP_NAME} (version ${VERSION})."
  LangString DESC_SecSource ${LANG_ENGLISH} "Install the source code for ${APP_NAME} (version ${VERSION}) - only useful if you are a software developer."
  LangString DESC_SecGtkImModule ${LANG_ENGLISH} "Set the GTK_IM_MODULE environment variable to 'ime' to allow Ekaya to work in Windows GTK applications like the GIMP, Pidgin etc."
  LangString DESC_SecMyWin ${LANG_ENGLISH} "Myanmar (Burmese) Unicode keyboard"
  LangString DESC_SecMyanmar3 ${LANG_ENGLISH} "Myanmar (Burmese) Unicode keyboard"
  LangString DESC_SecSgawKaren ${LANG_ENGLISH} "Sgaw Karen Unicode keyboard"
  LangString DESC_SecPaO ${LANG_ENGLISH} "Pa-O Unicode keyboard"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecApp} $(DESC_SecApp)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSource} $(DESC_SecSource)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecGtkImModule} $(DESC_SecGtkImModule)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMyWin} $(DESC_SecMyWin)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMyanmar3} $(DESC_SecMyanmar3)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSgawKaren} $(DESC_SecSgawKaren)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPaO} $(DESC_SecPaO)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInstFailed
	MessageBox MB_OK "You may want to rerun the installer as an Administrator or specify a different installation directory."

FunctionEnd

Function .onInstSuccess

	ExecShell "open" '"$INSTDIR\${APP_NAME}\doc\ekaya.html"'

FunctionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  SetShellVarContext all

  IfFileExists "$INSTDIR" AppFound 0
    MessageBox MB_OK|MB_ICONEXCLAMATION "$INSTDIR\${APP_NAME} was not found! You may need to uninstall manually." 

AppFound:
  ; ExecWait 'regsvr32 /u /s "$INSTDIR\${APP_NAME}\ekaya.dll"' $0
  SetOutPath "$INSTDIR"
  UnRegDLL "$INSTDIR\ekaya.dll"

  RMDir /r /REBOOTOK "$INSTDIR\doc"
  RMDir /r /REBOOTOK "$INSTDIR\kmfl"
  Delete /REBOOTOK "$INSTDIR\ekaya.dll"
  Delete /REBOOTOK "$INSTDIR\iconv.dll"
  Delete /REBOOTOK "$INSTDIR\winkmfl.dll"
  Delete /REBOOTOK "$INSTDIR\license.txt"
  Delete /REBOOTOK "$INSTDIR\ekaya.ico"
  Delete /REBOOTOK "$INSTDIR\*.manifest"
  Delete /REBOOTOK "$INSTDIR\vcredist_*.exe"

  Delete "$INSTDIR\ekaya-${VERSION}.tar.bz2"

  Delete /REBOOTOK "$INSTDIR\Uninstall.exe"

  Delete  "$DESKTOP\${APP_NAME}.lnk"
  RMDir /r "$SMPROGRAMS\${APP_NAME}"

  DeleteRegKey /ifempty HKLM "Software\${INSTALL_SUFFIX}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
  
  RMDir /REBOOTOK /r "$INSTDIR"

SectionEnd

