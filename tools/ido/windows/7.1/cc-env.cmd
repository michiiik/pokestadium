@echo off
set "IDO_TMPDIR=%~dp0..\..\..\..\.ido-tmp"
if not exist "%IDO_TMPDIR%" mkdir "%IDO_TMPDIR%"
set "TMPDIR=%IDO_TMPDIR%"
set "TEMP=%IDO_TMPDIR%"
set "TMP=%IDO_TMPDIR%"
"%~dp0cc.exe" %*