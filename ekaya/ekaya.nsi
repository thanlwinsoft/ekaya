;NSIS Modern User Interface
;Ekaya NSIS Installer script
;Written by Keith Stribley

; Some useful definitions that may need changing for different versions
!ifndef VERSION
  !define VERSION '0.1.3'
!endif

!define APP_NAME 'Ekaya'
!define INSTALL_SUFFIX "ThanLwinSoft.org"

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"

;--------------------------------
;General

  ;Name and file
  Name "${APP_NAME} (${VERSION})"
  Caption "Ekaya Input Method"

  OutFile "${APP_NAME}-${VERSION}.exe"
  InstallDir $PROGRAMFILES\${INSTALL_SUFFIX}
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\${INSTALL_SUFFIX}\${APP_NAME}" ""
  
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
  File "ekaya.ico"
  File /oname=license.txt "COPYING"

  ;File "Uninstall.ico"
  
  File "Release\ekaya.dll"
  File "Release\ekaya.dll*.manifest*"
  File "..\winkmfl\Release\libkmfl.dll"
  File "..\winkmfl\Release\libkmfl.dll*.manifest*"
  File "..\..\iconv-1.9.2.win32\bin\iconv.dll"
  File /r "doc"
  
  ; Redist dlls
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcp90.dll"
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcr90.dll"
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\msvcm90.dll"
  File "C:\Program Files\Microsoft Visual Studio 9.0\VC\redist\x86\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"

  ClearErrors
  RegDLL "$INSTDIR\${APP_NAME}\ekaya.dll"
  ; ExecWait 'regsvr32 "$INSTDIR\${APP_NAME}\ekaya.dll"' $0
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
  WriteRegStr HKLM "Software\${INSTALL_SUFFIX}\${APP_NAME}" "" $INSTDIR

  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\${APP_NAME}\Uninstall.exe"
  
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

; Add more keyboard sections here as needed
Section "MyWin Burmese Unicode 5.2 keyboard" SecMyWin
	SetOutPath "$INSTDIR\${APP_NAME}\kmfl"
	File "kmfl\myWin.png"
	File "kmfl\myWin.bmp"
	File "kmfl\myWin.jpg"
	File "kmfl\myWin.kmn"
	File "kmfl\myWin.html"
	File "kmfl\myWin_my.html"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\myWin Keyboard.lnk" \
		"$INSTDIR\${APP_NAME}\kmfl\myWin.html" '' \
		"" 0 SW_SHOWNORMAL \
		"" "myWin Keyboard"
SectionEnd

Section /o "Sgaw Karen Unicode 5.2 keyboard" SecSgawKaren
	SetOutPath "$INSTDIR\${APP_NAME}\kmfl"
	File "kmfl\SgawKaren.bmp"
	File "kmfl\SgawKaren.png"
	File "kmfl\SgawKaren.kmn"
	File "kmfl\SgawKaren.html"
	CreateShortCut "$SMPROGRAMS\${APP_NAME}\Sgaw Karen Keyboard.lnk" \
		"$INSTDIR\${APP_NAME}\kmfl\SgawKaren.html" '' \
		"" 0 SW_SHOWNORMAL \
		"" "Sgaw Karen Keyboard"
SectionEnd
;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecApp ${LANG_ENGLISH} "Install the ${APP_NAME} (version ${VERSION})."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecApp} $(DESC_SecApp)
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
  Delete /REBOOTOK "$INSTDIR\libkmfl.dll"
  Delete /REBOOTOK "$INSTDIR\license.txt"
  Delete /REBOOTOK "$INSTDIR\ekaya.ico"
  Delete /REBOOTOK "$INSTDIR\*.manifest"
  Delete /REBOOTOK "$INSTDIR\msvc*90.dll"
  
  Delete "$INSTDIR\ekaya-${VERSION}.tar.bz2"

  Delete /REBOOTOK "$INSTDIR\Uninstall.exe"

  Delete  "$DESKTOP\${APP_NAME}.lnk"
  RMDir /r "$SMPROGRAMS\${APP_NAME}"

  DeleteRegKey /ifempty HKLM "Software\${INSTALL_SUFFIX}"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"
  
  RMDir /REBOOTOK /r "$INSTDIR"

SectionEnd

