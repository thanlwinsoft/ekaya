;NSIS Modern User Interface
;Ekaya NSIS Installer script
;Written by Keith Stribley

; Some useful definitions that may need changing for different versions
!ifndef VERSION
  !define VERSION '0.1.8'
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
  !include "FileFunc.nsh"

;--------------------------------
;General

  ;Name and file
  Name "${APP_NAME} (${VERSION} ${ARCH})"
  Caption "Ekaya Input Method"

  OutFile "${APP_NAME}-${VERSION}+MyanmarFonts_${ARCH}.exe"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKLM "Software\${INSTALL_SUFFIX}\${APP_NAME}_${ARCH}" ""
  
  SetCompressor lzma

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "licenses.txt"
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
  File /oname=licenses.txt "licenses.txt"

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
	"$INSTDIR\${APP_NAME}\ekayaUninstall.exe" "" \
	"$INSTDIR\${APP_NAME}\ekayaUninstall.exe" 0 SW_SHOWNORMAL \
	"" "Uninstall ${APP_NAME}"
	
  ;Store installation folder
  WriteRegStr HKLM "Software\${INSTALL_SUFFIX}\${APP_NAME}_${ARCH}" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\${APP_NAME}\ekayaUninstall.exe"

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

!macro removeFont font fontname
    Push $0
    Push $1
    Push $2
    StrCpy $0 "$WINDIR\Fonts\${font}"
    IfFileExists $0 0 RemoveCompleteA${font}
        ClearErrors
        System::Call 'Gdi32::RemoveFontResource(t) i (r0.).r1'
        IfErrors 0 CheckRemoveFontA${font}
            MessageBox MB_OK|MB_ICONEXCLAMATION "RemoveFontResource Failed ${font} ($0 $1)"
CheckRemoveFontA${font}:
        IntCmpU $1 0 0 0 DeleteFontA${font}
            MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to remove font ${font} please close any applications using the font. ($0 $1)"
DeleteFontA${font}:
            Delete $0
            IfErrors 0 RemoveCompleteA${font}
                MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to delete font ${font} please close any applications using the font. ($0 $1)"

RemoveCompleteA${font}:

    ReadRegStr $2 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts" "${fontname} (TrueType)"
    IfErrors RemoveFontComplete${font} 0
    DetailPrint "Old fontname found: $2"
    StrCmp $2 ${font} RemoveCompleteB${font} 0
    # stored under another name
    StrCpy $0 "$WINDIR\Fonts\$2"
    IfFileExists $0 0 RemoveCompleteB${font}
        ClearErrors
        System::Call 'Gdi32::RemoveFontResource(t) i (r0.).r1'
        IfErrors 0 CheckRemoveFontB${font}
            MessageBox MB_OK|MB_ICONEXCLAMATION "RemoveFontResource Failed $2 ($0 $1)"
CheckRemoveFontB${font}:
        IntCmpU $1 0 0 0 DeleteFontB${font}
            MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to remove font $2 please close any applications using the font. ($0 $1)"
DeleteFontB${font}:
            Delete $0
            IfErrors 0 RemoveCompleteB${font}
                MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to delete font $2 please close any applications using the font. ($0 $1)"

RemoveCompleteB${font}:
    DeleteRegValue HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts" "${fontname} (TrueType)"
RemoveFontComplete${font}:
    Pop $2
    Pop $1
    Pop $0
!macroend

!macro installFont font
    # Use push "fontpath" to specify font
    SetOutpath $WINDIR\Fonts
    File "..\fonts\${font}"
    Push $0
    Push $1
    StrCpy $0 "$WINDIR\Fonts\${font}"
    DetailPrint "Copied font $0"
    System::Call 'Gdi32::AddFontResource(t) i (r0.).r1'
    IntCmpU $1 0 0 0 +2
        MessageBox MB_OK|MB_ICONEXCLAMATION "Failed to install font $0 ($1)"
    DetailPrint "AddFontResource $0 $1"

    Push $R0
    Push $R1
    Push $R2
    # strlen in R1, buffer in R2
    System::Call *(i${NSIS_MAX_STRLEN})i.R1
    System::Alloc ${NSIS_MAX_STRLEN}
    Pop $R2
    System::Call 'gdi32::GetFontResourceInfoW(wr0,iR1,iR2,i1)i.R0'
    IntCmp $R0 0 GFN_error${font}
    System::Call *$R2(&w${NSIS_MAX_STRLEN}.R0)
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion\Fonts" "$R0 (TrueType)" "${font}"
    Goto GFN_errordone${font}
GFN_error${font}:
    DetailPrint "GetFontResourceInfo error for $0"
GFN_errordone${font}:
        System::Free $R1
        System::Free $R2

    Pop $R2
    Pop $R1
    Pop $R0
    
    Pop $1
    Pop $0
!macroend

Section "Myanmar 3 Font" SecMM3Font
    !insertmacro removeFont "mm3_05-Jan-2011.ttf" "Myanmar3"
    !insertmacro installFont "mm3_05-Jan-2011.ttf"
    SendMessage HWND_BROADCAST WM_FONTCHANGE 0 0 /TIMEOUT=1000
SectionEnd

Section "Padauk Font" SecPadaukFont
    !insertmacro removeFont "Padauk.ttf" "Padauk"
    !insertmacro installFont "Padauk.ttf"
    !insertmacro removeFont "Padauk-bold.ttf" "Padauk Bold"
    !insertmacro installFont "Padauk-bold.ttf"
    !insertmacro removeFont "Padauk-book.ttf" "Padauk Book"
    !insertmacro installFont "Padauk-book.ttf"
    !insertmacro removeFont "Padauk-boldbold.ttf" "Padauk Book Bold"
    !insertmacro installFont "Padauk-bookbold.ttf"
SectionEnd

Section "ThanLwin Fonts" SecThanLwinFont
    !insertmacro removeFont "thanlwinMedium.ttf" "ThanLwin"
    !insertmacro installFont "thanlwinMedium.ttf"
    !insertmacro removeFont "thanlwinLight.ttf" "ThanLwin Light"
    !insertmacro installFont "thanlwinLight.ttf"
    !insertmacro removeFont "thanlwinBold.ttf" "ThanLwin Bold"
    !insertmacro installFont "thanlwinBold.ttf"
    !insertmacro removeFont "thanlwinFixed.ttf" "ThanLwin Fixed"
    !insertmacro installFont "thanlwinFixed.ttf"
    !insertmacro removeFont "thanlwinFixedBold.ttf" "ThanLwin Fixed Bold"
    !insertmacro installFont "thanlwinFixedBold.ttf"
    SendMessage HWND_BROADCAST WM_FONTCHANGE 0 0 /TIMEOUT=1000
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
  LangString DESC_SecMM3Font ${LANG_ENGLISH} "Myanmar3 Burmese Unicode Font from Myanmar NLP Lab"
  LangString DESC_SecPadaukFont ${LANG_ENGLISH} "SIL Padauk font family covering the whole of the Myanmar Unicode block including Burmese, Mon, Karen and Shan"
  LangString DESC_SecThanLwinFont ${LANG_ENGLISH} "ThanLwin font family for Burmese from ThanLwinSoft"

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecApp} $(DESC_SecApp)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSource} $(DESC_SecSource)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecGtkImModule} $(DESC_SecGtkImModule)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMyWin} $(DESC_SecMyWin)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMyanmar3} $(DESC_SecMyanmar3)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecSgawKaren} $(DESC_SecSgawKaren)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPaO} $(DESC_SecPaO)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMM3Font} $(DESC_SecMM3Font)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecPadaukFont} $(DESC_SecPadaukFont)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecThanLwinFont} $(DESC_SecThanLwinFont)
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

  MessageBox MB_YESNO "Do you want to remove the fonts as well?" IDYES 0 IDNO RemoveInstallDir
    !insertmacro removeFont "thanlwinMedium.ttf" "ThanLwin"
    !insertmacro removeFont "thanlwinLight.ttf" "ThanLwin Light"
    !insertmacro removeFont "thanlwinBold.ttf" "ThanLwin Bold"
    !insertmacro removeFont "thanlwinFixed.ttf" "ThanLwin Fixed"
    !insertmacro removeFont "thanlwinFixedBold.ttf" "ThanLwin Fixed Bold"
    !insertmacro removeFont "mm3_05-Jan-2011.ttf" "Myanmar3"
    !insertmacro removeFont "Padauk.ttf" "Padauk"
    !insertmacro removeFont "Padauk-bold.ttf" "Padauk Bold"
    !insertmacro removeFont "Padauk-book.ttf" "Padauk Book"
    !insertmacro removeFont "Padauk-boldbold.ttf" "Padauk Book Bold"

RemoveInstallDir:
  RMDir /REBOOTOK /r "$INSTDIR"

SectionEnd

