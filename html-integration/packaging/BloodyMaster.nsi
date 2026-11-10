Unicode true
!if ${NSIS_PACKEDVERSION} >= 0x030B0000
Target amd64-unicode
!endif
!include "MUI2.nsh"
!include "x64.nsh"
!ifndef PLUGIN_DIR
  !error "PLUGIN_DIR requis : bundle VST3 Windows reel"
!endif
!ifndef WAV_EXE
  !error "WAV_EXE requis"
!endif
!ifndef OUT_DIR
  !define OUT_DIR "."
!endif
!define PRODUCT "BloodyMaster"
!define VERSION "0.3.3"
!define UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\BRIAudioBloodyMaster"
Name "BloodyMaster ${VERSION} - Test VST3"
OutFile "${OUT_DIR}\BloodyMaster-${VERSION}-Test-Setup.exe"
InstallDir "$PROGRAMFILES64\BRI Audio Systems\BloodyMaster"
InstallDirRegKey HKLM "${UNINSTALL_KEY}" "InstallLocation"
RequestExecutionLevel admin
SetCompressor /SOLID lzma
ShowInstDetails show
ShowUninstDetails show
VIProductVersion "0.3.3.0"
VIAddVersionKey /LANG=1036 "ProductName" "BloodyMaster - version de test"
VIAddVersionKey /LANG=1036 "FileDescription" "Installation VST3 Windows x64"
VIAddVersionKey /LANG=1036 "FileVersion" "0.3.3"
VIAddVersionKey /LANG=1036 "LegalCopyright" "B.R.I Audio Systems"
!define MUI_ABORTWARNING
!define MUI_WELCOMEPAGE_TITLE "BloodyMaster - test VST3 Windows x64"
!define MUI_WELCOMEPAGE_TEXT "Cet assistant installe Bloody Master dans le dossier VST3 standard de Windows.$\r$\n$\r$\nFermez FL Studio et les autres logiciels audio avant de continuer.$\r$\n$\r$\nVotre interface HTML est integree. Si le moteur Microsoft WebView2 manque, une connexion Internet sera necessaire pour l'installer."
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_TEXT "Installation terminee.$\r$\n$\r$\nDans FL Studio : Options > Manage plugins > Find installed plugins, puis chercher Bloody Master.$\r$\n$\r$\nLe guide de test est installe dans le dossier du programme."
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "French"

!macro CheckUnlocked
  IfFileExists "$COMMONFILES64\VST3\Bloody Master.vst3\Contents\x86_64-win\Bloody Master.vst3" 0 unlocked
  System::Call 'kernel32::CreateFileW(w "$COMMONFILES64\VST3\Bloody Master.vst3\Contents\x86_64-win\Bloody Master.vst3", i 0x40000000, i 0, p 0, i 3, i 0, p 0) p.r0'
  ${If} $0 == -1
    MessageBox MB_OK|MB_ICONEXCLAMATION "Le plugin est utilise ou protege en ecriture. Fermez les logiciels audio puis relancez cet assistant." /SD IDOK
    Abort
  ${EndIf}
  System::Call 'kernel32::CloseHandle(p r0)'
  unlocked:
!macroend
Function .onInit
  ${IfNot} ${RunningX64}
    MessageBox MB_OK|MB_ICONSTOP "Cette version demande Windows 64 bits." /SD IDOK
    Abort
  ${EndIf}
  SetRegView 64
  !insertmacro CheckUnlocked
FunctionEnd
Function un.onInit
  SetRegView 64
  !insertmacro CheckUnlocked
FunctionEnd

!if ${HTML_UI} == 1
Section "Moteur HTML Microsoft WebView2 (si necessaire)" SecRuntime
  SectionIn RO
  InitPluginsDir
  SetOutPath "$PLUGINSDIR"
  File /oname=WebViewProbe.exe "${WEBVIEW_PROBE}"
  ExecWait '"$PLUGINSDIR\WebViewProbe.exe"' $0
  ${If} $0 != 0
    File /oname=WebView2Setup.exe "${WEBVIEW_BOOTSTRAPPER}"
    DetailPrint "Installation du moteur HTML Microsoft Edge WebView2..."
    ExecWait '"$PLUGINSDIR\WebView2Setup.exe" /silent /install' $0
    ExecWait '"$PLUGINSDIR\WebViewProbe.exe"' $0
    ${If} $0 != 0
      MessageBox MB_OK|MB_ICONSTOP "Le moteur HTML WebView2 n'a pas pu etre installe. Connectez Windows a Internet puis relancez cet assistant." /SD IDOK
      SetErrorLevel 2
      Abort
    ${EndIf}
  ${EndIf}
SectionEnd
!endif

Section "Plugin VST3 x64 (obligatoire)" SecCore
  SectionIn RO
  SetShellVarContext all
  SetOutPath "$COMMONFILES64\VST3\Bloody Master.vst3"
  ClearErrors
  File /r "${PLUGIN_DIR}\*"
  ${If} ${Errors}
    MessageBox MB_OK|MB_ICONSTOP "Installation du VST3 impossible. Fermez les logiciels audio et reessayez." /SD IDOK
    Abort
  ${EndIf}
  SetOutPath "$INSTDIR"
  File /oname=TEST_WINDOWS_FR.txt "TEST_WINDOWS_FR.txt"
  File /oname=PRESETS_FR.md "..\docs\PRESETS_FR.md"
  File /oname=ANALOG_AUDIT_FR.md "..\docs\ANALOG_AUDIT_FR.md"
  File /oname=FactoryPresets.json "..\Resources\FactoryPresets.json"
  WriteUninstaller "$INSTDIR\Desinstaller.exe"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayName" "BloodyMaster - Test VST3 x64"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "Publisher" "B.R.I Audio Systems"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "${UNINSTALL_KEY}" "UninstallString" '$\"$INSTDIR\Desinstaller.exe$\"'
  WriteRegStr HKLM "${UNINSTALL_KEY}" "QuietUninstallString" '$\"$INSTDIR\Desinstaller.exe$\" /S'
  WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${UNINSTALL_KEY}" "NoRepair" 1
SectionEnd
Section "Outil de traitement WAV (optionnel)" SecWav
  SetOutPath "$INSTDIR"
  File /oname=BloodyMasterTest.exe "${WAV_EXE}"
SectionEnd

Section "Uninstall"
  SetShellVarContext all
  Delete "$COMMONFILES64\VST3\Bloody Master.vst3\Contents\x86_64-win\Bloody Master.vst3"
  Delete "$COMMONFILES64\VST3\Bloody Master.vst3\Contents\Resources\moduleinfo.json"
  RMDir "$COMMONFILES64\VST3\Bloody Master.vst3\Contents\x86_64-win"
  RMDir "$COMMONFILES64\VST3\Bloody Master.vst3\Contents\Resources"
  RMDir "$COMMONFILES64\VST3\Bloody Master.vst3\Contents"
  RMDir "$COMMONFILES64\VST3\Bloody Master.vst3"
  Delete "$INSTDIR\BloodyMasterTest.exe"
  Delete "$INSTDIR\TEST_WINDOWS_FR.txt"
  Delete "$INSTDIR\PRESETS_FR.md"
  Delete "$INSTDIR\ANALOG_AUDIT_FR.md"
  Delete "$INSTDIR\FactoryPresets.json"
  Delete "$INSTDIR\Desinstaller.exe"
  RMDir "$INSTDIR"
  DeleteRegKey HKLM "${UNINSTALL_KEY}"
SectionEnd
