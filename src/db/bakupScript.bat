@echo off
:: Get current date in format DDMMYY using WMIC
for /f %%a in ('"wmic os get localdatetime | findstr /r /v \"^$\""') do set dt=%%a

:: Extract parts of the date
set today=%dt:~6,2%%dt:~4,2%%dt:~2,2%

:: Set backup file name with the correct date
set "backupFile=C:\PostgreSQL\finances_%today%.sql"

:: Set PostgreSQL password
set PGPASSWORD=p0w2i8

:: Run the pg_dump command
"C:\Program Files\PostgreSQL\17\bin\pg_dump.exe" -U postgres -h localhost -p 5432 -F p -b -v -f "%backupFile%" finances

:: Clean up
set PGPASSWORD=
