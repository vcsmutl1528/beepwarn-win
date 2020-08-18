@echo off
nmake /nologo help
if ERRORLEVEL 1 goto msg
if NOT '%1'=='' goto prm
nmake main
goto end
:prm
nmake %1
goto end
:msg
echo.
echo VC environment is needed to be set up.
:end
