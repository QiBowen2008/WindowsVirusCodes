@echo off
REM -- First make map file from Microsoft Visual C++ generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by EBOOK.HPJ. >"hlp\EBook.hm"
echo. >>"hlp\EBook.hm"
echo // Commands (ID_* and IDM_*) >>"hlp\EBook.hm"
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\EBook.hm"
echo. >>"hlp\EBook.hm"
echo // Prompts (IDP_*) >>"hlp\EBook.hm"
makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\EBook.hm"
echo. >>"hlp\EBook.hm"
echo // Resources (IDR_*) >>"hlp\EBook.hm"
makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\EBook.hm"
echo. >>"hlp\EBook.hm"
echo // Dialogs (IDD_*) >>"hlp\EBook.hm"
makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\EBook.hm"
echo. >>"hlp\EBook.hm"
echo // Frame Controls (IDW_*) >>"hlp\EBook.hm"
makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\EBook.hm"
REM -- Make help for Project EBOOK


echo Building Win32 Help files
start /wait hcw /C /E /M "hlp\EBook.hpj"
if errorlevel 1 goto :Error
if not exist "hlp\EBook.hlp" goto :Error
if not exist "hlp\EBook.cnt" goto :Error
echo.
if exist Debug\nul copy "hlp\EBook.hlp" Debug
if exist Debug\nul copy "hlp\EBook.cnt" Debug
if exist Release\nul copy "hlp\EBook.hlp" Release
if exist Release\nul copy "hlp\EBook.cnt" Release
echo.
goto :done

:Error
echo hlp\EBook.hpj(1) : error: Problem encountered creating help file

:done
echo.
