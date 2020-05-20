@echo off
setlocal enableextensions enabledelayedexpansion
SET allParams=%*
SET params=%allParams:' '=","%
SET params=%params:'="%
set /a count=0
for %%i in (%params%) do (
  set /a count+=1
  set param[!count!]=%%i
)

set "studioInstallationDir=%param[4]:"=%"
set "projDir=%~dp0"
set "projDir=%projDir:"=%"

set "projDir=%projDir:\=/%"
set "projDir=%projDir:Z:=%"
start /unix "%studioInstallationDir%/jre/bin/java" -jar "%studioInstallationDir%/../../../.config/SimplicityStudio/v4/python/jython.jar"  "%projDir%/LED_Node-prebuild.py"