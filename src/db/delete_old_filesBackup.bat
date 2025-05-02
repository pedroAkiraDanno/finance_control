

@echo off
setlocal

set "targetDir=Y:\Meu Drive\bkp\bkp_postgresql"
set "daysOld=7"

forfiles /p "%targetDir%" /s /m *.* /d -%daysOld% /c "cmd /c del @path"

endlocal
