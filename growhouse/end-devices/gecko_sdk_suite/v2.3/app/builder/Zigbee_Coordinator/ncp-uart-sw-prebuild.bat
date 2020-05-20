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

"%studioInstallationDir%/features\com.silabs.external.java.windows.x86_64.feature_1.8.0.92\jre\bin\java.exe" -jar "%studioInstallationDir%/..\..\..\Users\keval.sanghvi\AppData\Roaming\SimplicityStudio\v4\python\jython.jar"  "%projDir%/ncp-uart-sw-prebuild.py"