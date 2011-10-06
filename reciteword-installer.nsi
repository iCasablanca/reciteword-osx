; Installer script for win32 ReciteWord
; Hu Zheng <huzheng_001@163.com>

;--------------------------------
;Global Variables
Var GTK_FOLDER
Var ISSILENT

;Configuration

;General

!define RECITEWORD_VERSION		"0.8.4"

Name "ReciteWord ${RECITEWORD_VERSION}"

OutFile "reciteword-${RECITEWORD_VERSION}.exe"

XPStyle on

SetCompressor /SOLID lzma

ShowInstDetails show
ShowUninstDetails show
SetDateSave on

; $INSTDIR is set in .onInit function..

!include "MUI.nsh"
!include Sections.nsh

!include "FileFunc.nsh"
!insertmacro GetParent

!include "WordFunc.nsh"
!insertmacro VersionCompare

;--------------------------------
;Defines


!define MUI_ICON			.\reciteword.ico
!define MUI_UNICON			.\reciteword-uninst.ico
!define MUI_SPECIALBITMAP		.\src\win32\nsis\reciteword-intro.bmp
!define MUI_HEADERBITMAP		.\src\win32\nsis\reciteword-header.bmp

!define RECITEWORD_NSIS_INCLUDE_PATH		".\src\win32\nsis"

!define RECITEWORD_REG_KEY			"SOFTWARE\reciteword"
!define RECITEWORD_UNINSTALL_KEY		"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\ReciteWord"
!define HKLM_APP_PATHS_KEY		"SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\reciteword.exe"
!define RECITEWORD_UNINST_EXE			"reciteword-uninst.exe"

!define GTK_MIN_VERSION			"2.12.0"
!define GTK_INSTALL_VERSION		"2.12.1"
!define GTK_REG_KEY			"SOFTWARE\GTK\2.0"
!define GTK_INSTALL_VERIFIER		"bin\libgtk-win32-2.0-0.dll"
!define GTK_DEFAULT_INSTALL_PATH		"$COMMONFILES\GTK\2.0"
!define GTK_RUNTIME_INSTALLER			"gtk_installer\gtk-runtime*.exe"



;--------------------------------
;Modern UI Configuration

  !define MUI_CUSTOMPAGECOMMANDS

  !define MUI_WELCOMEPAGE
  !define MUI_LICENSEPAGE
  !define MUI_DIRECTORYPAGE
  !define MUI_FINISHPAGE
  
  !define MUI_ABORTWARNING

  !define MUI_UNINSTALLER
  !define MUI_UNCONFIRMPAGE

;--------------------------------
;Pages
  
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE			"./COPYING"

  ; GTK+ install dir page
  !define MUI_PAGE_CUSTOMFUNCTION_PRE		preGtkDirPage
  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE		postGtkDirPage
  !define MUI_DIRECTORYPAGE_VARIABLE		$GTK_FOLDER
  !insertmacro MUI_PAGE_DIRECTORY

  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Language Strings
  LangString GTK_INSTALLER_NEEDED		${LANG_ENGLISH} \
		"The GTK+ runtime environment is either missing or needs to be upgraded.$\r \
		Please install v${GTK_MIN_VERSION} or higher of the GTK+ runtime"
  LangString GTK_BAD_INSTALL_PATH		${LANG_ENGLISH} \
		"The path you entered can not be accessed or created."
  LangString GTK_SECTION_TITLE			${LANG_ENGLISH} \
		"GTK+ Runtime (required if not present)"
  LangString GTK_UPGRADE_PROMPT			${LANG_ENGLISH} \
		"An old version of the GTK+ runtime was found. Do you wish to upgrade?$\rNote: StarDict may not work unless you do."
  LangString GTK_SKIP_PROMPT			${LANG_ENGLISH} \
		"A same version of the GTK+ runtime was found. Do you wish to skip its installation?"
  LangString GTK_INSTALL_ERROR			${LANG_ENGLISH} \
		"Error installing GTK+ runtime."


  ; ReciteWord Section Prompts and Texts
  LangString RECITEWORD_UNINSTALL_DESC		${LANG_ENGLISH} \
		"ReciteWord (remove only)"
  LangString RECITEWORD_PROMPT_WIPEOUT		${LANG_ENGLISH} \
		"You're old ReciteWord directory is about to be deleted. Would you like to continue?$\r$\r \
		ReciteWord user settings will not be affected."
  LangString RECITEWORD_PROMPT_DIR_EXISTS		${LANG_ENGLISH} \
		"The installation directory you specified already exists. Any contents $\r \
		it may have will be deleted. Would you like to continue?"

  ; Uninstall Section Prompts
  LangString un.RECITEWORD_UNINSTALL_ERROR_1         	${LANG_ENGLISH} \
		"The uninstaller could not find registry entries for ReciteWord.$\r \
		It is likely that another user installed this application."
  LangString un.RECITEWORD_UNINSTALL_ERROR_2         	${LANG_ENGLISH} \
		"You do not have permission to uninstall this application."

;--------------------------------
;Data
  
  LicenseData "./COPYING"

;--------------------------------
;Uninstall any old version of ReciteWord

Section -SecUninstallOldReciteWord
  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  StrCmp $R0 "HKLM" reciteword_hklm
  StrCmp $R0 "HKCU" reciteword_hkcu done

  reciteword_hkcu:
      ReadRegStr $R1 HKCU ${RECITEWORD_REG_KEY} ""
      ReadRegStr $R2 HKCU ${RECITEWORD_REG_KEY} "Version"
      ReadRegStr $R3 HKCU "${RECITEWORD_UNINSTALL_KEY}" "UninstallString"
      Goto try_uninstall

  reciteword_hklm:
      ReadRegStr $R1 HKLM ${RECITEWORD_REG_KEY} ""
      ReadRegStr $R2 HKLM ${RECITEWORD_REG_KEY} "Version"
      ReadRegStr $R3 HKLM "${RECITEWORD_UNINSTALL_KEY}" "UninstallString"

  ; If previous version exists .. remove
  try_uninstall:
    StrCmp $R1 "" done
      ; Version key started with 0.8.2. Prior versions can't be 
      ; automaticlly uninstalled.
      StrCmp $R2 "" uninstall_problem
        ; Check if we have uninstall string..
        IfFileExists $R3 0 uninstall_problem
          ; Have uninstall string.. go ahead and uninstall.
          SetOverwrite on
          ; Need to copy uninstaller outside of the install dir
          ClearErrors
          CopyFiles /SILENT $R3 "$TEMP\${RECITEWORD_UNINST_EXE}"
          SetOverwrite off
          IfErrors uninstall_problem
            ; Ready to uninstall..
            ClearErrors
	    ExecWait '"$TEMP\${RECITEWORD_UNINST_EXE}" /S _?=$R1'
	    IfErrors exec_error
              Delete "$TEMP\${RECITEWORD_UNINST_EXE}"
	      Goto done

	    exec_error:
              Delete "$TEMP\${RECITEWORD_UNINST_EXE}"
              Goto uninstall_problem

        uninstall_problem:
	  ; In this case just wipe out previous ReciteWord install dir..
          MessageBox MB_YESNO $(RECITEWORD_PROMPT_WIPEOUT) IDYES do_wipeout IDNO cancel_install
          cancel_install:
            Quit

          do_wipeout:
            StrCmp $R0 "HKLM" reciteword_del_lm_reg reciteword_del_cu_reg
            reciteword_del_cu_reg:
              DeleteRegKey HKCU ${RECITEWORD_REG_KEY}
              Goto uninstall_prob_cont
            reciteword_del_lm_reg:
              DeleteRegKey HKLM ${RECITEWORD_REG_KEY}

            uninstall_prob_cont:
	      RMDir /r "$R1"

  done:
SectionEnd

;--------------------------------
;GTK+ Runtime Install Section

Section $(GTK_SECTION_TITLE) SecGtk

  Call CheckUserInstallRights
  Pop $R1

  SetOutPath $TEMP
  SetOverwrite on
  File /oname=gtk-runtime.exe ${GTK_RUNTIME_INSTALLER}
  SetOverwrite off

  Call DoWeNeedGtk
  Pop $R0
  Pop $R6

  StrCmp $R0 "0" have_gtk
  StrCmp $R0 "1" upgrade_gtk
  StrCmp $R0 "2" upgrade_gtk
  StrCmp $R0 "3" no_gtk no_gtk

  no_gtk:
    StrCmp $R1 "NONE" gtk_no_install_rights
    ClearErrors
    ExecWait '"$TEMP\gtk-runtime.exe" /L=$LANGUAGE $ISSILENT /D=$GTK_FOLDER'
    IfErrors gtk_install_error done

  upgrade_gtk:
    StrCpy $GTK_FOLDER $R6
    StrCmp $R0 "2" +2 ; Upgrade isn't optional
    MessageBox MB_YESNO $(GTK_UPGRADE_PROMPT) /SD IDYES IDNO done
    ClearErrors
    ExecWait '"$TEMP\gtk-runtime.exe" /L=$LANGUAGE /S /D=$GTK_FOLDER'
    IfErrors gtk_install_error done

    gtk_install_error:
      Delete "$TEMP\gtk-runtime.exe"
      MessageBox MB_OK $(GTK_INSTALL_ERROR) /SD IDOK
      Quit

  have_gtk:
    StrCpy $GTK_FOLDER $R6
    StrCmp $R1 "NONE" done ; If we have no rights, we can't re-install
    ; Even if we have a sufficient version of GTK+, we give user choice to re-install.
    MessageBox MB_YESNO $(GTK_SKIP_PROMPT) /SD IDYES IDYES done
    ClearErrors
    ExecWait '"$TEMP\gtk-runtime.exe" /L=$LANGUAGE $ISSILENT'
    IfErrors gtk_install_error
    Goto done

  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ; end got_install rights

  gtk_no_install_rights:
    ; Install GTK+ to Pidgin install dir
    StrCpy $GTK_FOLDER $INSTDIR
    ClearErrors
    ExecWait '"$TEMP\gtk-runtime.exe" /L=$LANGUAGE $ISSILENT /D=$GTK_FOLDER'
    IfErrors gtk_install_error
      SetOverwrite on
      ClearErrors
      CopyFiles /FILESONLY "$GTK_FOLDER\bin\*.dll" $GTK_FOLDER
      SetOverwrite off
      IfErrors gtk_install_error
        Delete "$GTK_FOLDER\bin\*.dll"
        Goto done
  ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  ; end gtk_no_install_rights

  done:
    Delete "$TEMP\gtk-runtime.exe"
SectionEnd ; end of GTK+ section

;--------------------------------
;ReciteWord Install Section

Section SecReciteWord
  SectionIn 1 RO

  ; Check install rights..
  Call CheckUserInstallRights
  Pop $R0

  ; Get GTK+ lib dir if we have it..

  StrCmp $R0 "NONE" reciteword_none
  StrCmp $R0 "HKLM" reciteword_hklm reciteword_hkcu

  reciteword_hklm:
    ReadRegStr $R1 HKLM ${GTK_REG_KEY} "Path"
    WriteRegStr HKLM "${HKLM_APP_PATHS_KEY}" "Path" "$R1\bin"
    WriteRegStr HKLM ${RECITEWORD_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKLM ${RECITEWORD_REG_KEY} "Version" "${RECITEWORD_VERSION}"
    WriteRegStr HKLM "${RECITEWORD_UNINSTALL_KEY}" "DisplayName" $(RECITEWORD_UNINSTALL_DESC)
    WriteRegStr HKLM "${RECITEWORD_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\${RECITEWORD_UNINST_EXE}"
    ; Sets scope of the desktop and Start Menu entries for all users.
    SetShellVarContext "all"
    Goto reciteword_install_files

  reciteword_hkcu:
    ReadRegStr $R1 HKCU ${GTK_REG_KEY} "Path"
    StrCmp $R1 "" 0 reciteword_hkcu1
      ReadRegStr $R1 HKLM ${GTK_REG_KEY} "Path"
    reciteword_hkcu1:
    WriteRegStr HKCU ${RECITEWORD_REG_KEY} "" "$INSTDIR"
    WriteRegStr HKCU ${RECITEWORD_REG_KEY} "Version" "${RECITEWORD_VERSION}"
    WriteRegStr HKCU "${RECITEWORD_UNINSTALL_KEY}" "DisplayName" $(RECITEWORD_UNINSTALL_DESC)
    WriteRegStr HKCU "${RECITEWORD_UNINSTALL_KEY}" "UninstallString" "$INSTDIR\${RECITEWORD_UNINST_EXE}"
    Goto reciteword_install_files

  reciteword_none:
    ReadRegStr $R1 HKLM ${GTK_REG_KEY} "Path"

  reciteword_install_files:
    SetOutPath "$INSTDIR"
    ; ReciteWord files
    SetOverwrite on
    File /r .\win32-install-dir\*.*

    ; If we don't have install rights and no hklm GTK install.. then Start in lnk property should
    ; remain reciteword dir.. otherwise it should be set to the GTK lib dir. (to avoid dll hell)
    StrCmp $R0 "NONE" 0 startin_gtk
      StrCmp $R1 "" startin_reciteword
    startin_gtk:
      SetOutPath "$R1\bin"
    startin_reciteword:
    CreateDirectory "$SMPROGRAMS\ReciteWord"
    CreateShortCut "$SMPROGRAMS\ReciteWord\ReciteWord.lnk" "$INSTDIR\reciteword.exe"
    CreateShortCut "$SMPROGRAMS\ReciteWord\ReciteWord-Dict.lnk" "$INSTDIR\reciteword.exe" -d
    CreateShortCut "$DESKTOP\ReciteWord.lnk" "$INSTDIR\reciteword.exe"
    SetOutPath "$INSTDIR"

    ; If we don't have install rights.. we're done
    StrCmp $R0 "NONE" done
    CreateShortCut "$SMPROGRAMS\ReciteWord\Uninstall.lnk" "$INSTDIR\${RECITEWORD_UNINST_EXE}"
    CreateShortCut "$SMPROGRAMS\ReciteWord\readme.lnk" "$INSTDIR\readme.txt"    
    SetOverwrite off

    ; write out uninstaller
    SetOverwrite on
    WriteUninstaller "$INSTDIR\${RECITEWORD_UNINST_EXE}"
    SetOverwrite off

  done:
SectionEnd ; end of default ReciteWord section


;--------------------------------
;Uninstaller Section


Section Uninstall
  Call un.CheckUserInstallRights
  Pop $R0
  StrCmp $R0 "NONE" no_rights
  StrCmp $R0 "HKCU" try_hkcu try_hklm

  try_hkcu:
    ReadRegStr $R0 HKCU ${RECITEWORD_REG_KEY} ""
    StrCmp $R0 $INSTDIR 0 cant_uninstall
      ; HKCU install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKCU ${RECITEWORD_REG_KEY}
      DeleteRegKey HKCU "${RECITEWORD_UNINSTALL_KEY}"
      Goto cont_uninstall

  try_hklm:
    ReadRegStr $R0 HKLM ${RECITEWORD_REG_KEY} ""
    StrCmp $R0 $INSTDIR 0 try_hkcu
      ; HKLM install path matches our INSTDIR.. so uninstall
      DeleteRegKey HKLM ${RECITEWORD_REG_KEY}
      DeleteRegKey HKLM "${RECITEWORD_UNINSTALL_KEY}"
      DeleteRegKey HKLM "${HKLM_APP_PATHS_KEY}"
      ; Sets start menu and desktop scope to all users..
      SetShellVarContext "all"

  cont_uninstall:
    RMDir /r "$INSTDIR\locale"
    RMDir /r "$INSTDIR\dicts"
    RMDir /r "$INSTDIR\books"
    RMDir /r "$INSTDIR\modules"
    RMDir /r "$INSTDIR\skins"
    Delete "$INSTDIR\reciteword.exe"
    Delete "$INSTDIR\readme.txt"
    Delete "$INSTDIR\${RECITEWORD_UNINST_EXE}"
    ;Try to remove ReciteWord install dir .. if empty
    RMDir "$INSTDIR"

    ; Shortcuts..
    RMDir /r "$SMPROGRAMS\ReciteWord"
    Delete "$DESKTOP\ReciteWord.lnk"

    Goto done

  cant_uninstall:
    MessageBox MB_OK $(un.RECITEWORD_UNINSTALL_ERROR_1) IDOK
    Quit

  no_rights:
    MessageBox MB_OK $(un.RECITEWORD_UNINSTALL_ERROR_2) IDOK
    Quit

  done:
SectionEnd ; end of uninstall section


;--------------------------------
;Functions

Function CheckUserInstallRights
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1

	StrCmp $1 "Admin" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "Power" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "User" 0 +3
		StrCpy $1 "HKCU"
		Goto done
	StrCmp $1 "Guest" 0 +3
		StrCpy $1 "NONE"
		Goto done
	; Unknown error
	StrCpy $1 "NONE"
        Goto done

	Win9x:
		StrCpy $1 "HKLM"

	done:
        Push $1
FunctionEnd

Function un.CheckUserInstallRights
	ClearErrors
	UserInfo::GetName
	IfErrors Win9x
	Pop $0
	UserInfo::GetAccountType
	Pop $1
	StrCmp $1 "Admin" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "Power" 0 +3
                StrCpy $1 "HKLM"
		Goto done
	StrCmp $1 "User" 0 +3
		StrCpy $1 "HKCU"
		Goto done
	StrCmp $1 "Guest" 0 +3
		StrCpy $1 "NONE"
		Goto done
	; Unknown error
	StrCpy $1 "NONE"
        Goto done

	Win9x:
		StrCpy $1 "HKLM"

	done:
        Push $1
FunctionEnd

Function .onVerifyInstDir
  Push $INSTDIR
  Call VerifyDir
  Pop $0
  StrCmp $0 "0" 0 dir_good
    Abort
  dir_good:
FunctionEnd

;
; Usage:
; Call DoWeNeedGtk
; First Pop:
;   0 - We have the correct version
;       Second Pop: Key where Version was found
;   1 - We have an old version that should work, prompt user for optional upgrade
;       Second Pop: HKLM or HKCU depending on where GTK was found.
;   2 - We have an old version that needs to be upgraded
;       Second Pop: HKLM or HKCU depending on where GTK was found.
;   3 - We don't have Gtk+ at all
;       Second Pop: "NONE, HKLM or HKCU" depending on our rights..
;
Function DoWeNeedGtk
  ; Logic should be:
  ; - Check what user rights we have (HKLM or HKCU)
  ;   - If HKLM rights..
  ;     - Only check HKLM key for GTK+
  ;       - If installed to HKLM, check it and return.
  ;   - If HKCU rights..
  ;     - First check HKCU key for GTK+
  ;       - if good or bad exists stop and ret.
  ;     - If no hkcu gtk+ install, check HKLM
  ;       - If HKLM ver exists but old, return as if no ver exits.
  ;   - If no rights
  ;     - Check HKLM
  Push $0
  Push $1
  Push $2
  Push $3

  Call CheckUserInstallRights
  Pop $1
  StrCmp $1 "HKLM" check_hklm
  StrCmp $1 "HKCU" check_hkcu check_hklm
    check_hkcu:
      ReadRegStr $0 HKCU ${GTK_REG_KEY} "Version"
      StrCpy $2 "HKCU"
      StrCmp $0 "" check_hklm have_gtk

    check_hklm:
      ReadRegStr $0 HKLM ${GTK_REG_KEY} "Version"
      StrCpy $2 "HKLM"
      StrCmp $0 "" no_gtk have_gtk

  have_gtk:
    ; GTK+ is already installed; check version.
	; Change this to not even run the GTK installer if this version is already installed.
    ${VersionCompare} ${GTK_INSTALL_VERSION} $0 $3
    IntCmp $3 1 +1 good_version good_version
    ${VersionCompare} ${GTK_MIN_VERSION} $0 $3

      ; Bad version. If hklm ver and we have hkcu or no rights.. return no gtk
      StrCmp $1 "NONE" no_gtk ; if no rights.. can't upgrade
      StrCmp $1 "HKCU" 0 +2   ; if HKLM can upgrade..
      StrCmp $2 "HKLM" no_gtk ; have hkcu rights.. if found hklm ver can't upgrade..
      Push $2
      IntCmp $3 1 +3
        Push "1" ; Optional Upgrade
        Goto done
        Push "2" ; Mandatory Upgrade
        Goto done

  good_version:
    StrCmp $2 "HKLM" have_hklm_gtk have_hkcu_gtk
      have_hkcu_gtk:
        ; Have HKCU version
        ReadRegStr $0 HKCU ${GTK_REG_KEY} "Path"
        Goto good_version_cont

      have_hklm_gtk:
        ReadRegStr $0 HKLM ${GTK_REG_KEY} "Path"
        Goto good_version_cont

    good_version_cont:
      Push $0  ; The path to existing GTK+
      Push "0"
      Goto done

  no_gtk:
    Push $1 ; our rights
    Push "3"
    Goto done

  done:
  ; The top two items on the stack are what we want to return
  Exch 4
  Pop $1
  Exch 4
  Pop $0
  Pop $3
  Pop $2
FunctionEnd


;
; Usage:
;   Push $0 ; Path string
;   Call VerifyDir
;   Pop $0 ; 0 - Bad path  1 - Good path
;
Function VerifyDir
  Exch $0
  Push $1
  Push $2
  Loop:
    IfFileExists $0 dir_exists
    StrCpy $1 $0 ; save last
    ${GetParent} $0 $0
    StrLen $2 $0
    ; IfFileExists "C:" on xp returns true and on win2k returns false
    ; So we're done in such a case..
    IntCmp $2 2 loop_done
    ; GetParent of "C:" returns ""
    IntCmp $2 0 loop_done
    Goto Loop

  loop_done:
    StrCpy $1 "$0\GaImFooB"
    ; Check if we can create dir on this drive..
    ClearErrors
    CreateDirectory $1
    IfErrors DirBad DirGood

  dir_exists:
    ClearErrors
    FileOpen $1 "$0\pidginfoo.bar" w
    IfErrors PathBad PathGood

    DirGood:
      RMDir $1
      Goto PathGood1

    DirBad:
      RMDir $1
      Goto PathBad1

    PathBad:
      FileClose $1
      Delete "$0\pidginfoo.bar"
      PathBad1:
      StrCpy $0 "0"
      Push $0
      Goto done

    PathGood:
      FileClose $1
      Delete "$0\pidginfoo.bar"
      PathGood1:
      StrCpy $0 "1"
      Push $0

  done:
  Exch 3 ; The top of the stack contains the output variable
  Pop $0
  Pop $2
  Pop $1
FunctionEnd

Function preGtkDirPage
  Push $R0
  Push $R1
  Call DoWeNeedGtk
  Pop $R0
  Pop $R1

  IntCmp $R0 2 +2 +2 no_gtk
  StrCmp $R0 "3" no_gtk no_gtk

  ; Don't show dir selector.. Upgrades are done to existing path..
  Pop $R1
  Pop $R0
  Abort

  no_gtk:
    StrCmp $R1 "NONE" 0 no_gtk_cont
      ; Got no install rights..
      Pop $R1
      Pop $R0
      Abort
    no_gtk_cont:
      ; Suggest path..
      StrCmp $R1 "HKCU" 0 hklm1
        ${GetParent} $SMPROGRAMS $R0
        ${GetParent} $R0 $R0
        StrCpy $R0 "$R0\GTK\2.0"
        Goto got_path
      hklm1:
        StrCpy $R0 "${GTK_DEFAULT_INSTALL_PATH}"

   got_path:
     StrCpy $GTK_FOLDER $R0
     Pop $R1
     Pop $R0
FunctionEnd

Function postGtkDirPage
  Push $R0
  Push $GTK_FOLDER
  Call VerifyDir
  Pop $R0
  StrCmp $R0 "0" 0 done
    MessageBox MB_OK $(GTK_BAD_INSTALL_PATH) /SD IDOK
    Pop $R0
    Abort
  done:
  Pop $R0
FunctionEnd


Function .onInit
  StrCpy $ISSILENT "/S"
  Call CheckUserInstallRights
  Pop $0

  StrCmp $0 "HKLM" 0 user_dir
    StrCpy $INSTDIR "$PROGRAMFILES\ReciteWord"
    Goto instdir_done
  user_dir:
    StrCpy $2 "$SMPROGRAMS"
    Push $2
    Call GetParent
    Call GetParent
    Pop $2
    StrCpy $INSTDIR "$2\ReciteWord"

  instdir_done:

FunctionEnd
