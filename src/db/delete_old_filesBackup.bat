

@echo off
setlocal

set "targetDir=C:\PostgreSQL"
set "daysOld=7"

forfiles /p "%targetDir%" /s /m *.* /d -%daysOld% /c "cmd /c del @path"

endlocal
