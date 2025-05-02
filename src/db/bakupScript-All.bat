@echo off
setlocal EnableDelayedExpansion

rem Get date in YYYYMMDD format
for /f %%a in ('wmic os get LocalDateTime ^| find "."') do set datetime=%%a
set year=!datetime:~0,4!
set month=!datetime:~4,2!
set day=!datetime:~6,2!

rem Format date as day_month_year
set datestr=!day!_!month!_!year!
echo datestr is !datestr!

rem Define path and backup file name
set BACKUP_PATH=Y:\Meu Drive\bkp\bkp_postgresql
set BACKUP_FILE=!BACKUP_PATH!\all_databases_!datestr!.sql
echo backup file name is !BACKUP_FILE!

rem Optional: Create the folder if it doesn't exist
if not exist "!BACKUP_PATH!" mkdir "!BACKUP_PATH!"

rem Set database password and run pg_dumpall
SET PGPASSWORD=p0w2i8
echo on
"C:\Program Files\PostgreSQL\17\bin\pg_dumpall.exe" -h localhost -p 5432 -U postgres -v -f "!BACKUP_FILE!"

endlocal



rem RESTORE 
rem SET PGPASSWORD=p0w2i8
rem "C:\Program Files\PostgreSQL\17\bin\psql.exe" -h localhost -p 5432 -U postgres -f "Y:\Meu Drive\bkp\bkp_postgresql\all_databases_DD_MM_YYYY.sql"


