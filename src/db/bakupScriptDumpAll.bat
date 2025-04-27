

   set BACKUP_FILE=C:\PostgreSQL\Globals_%datestr%.backup.sql
   echo backup file name is %BACKUP_FILE%
   SET PGPASSWORD=p0w2i8
   echo on
   bin\pg_dumpall -g -h localhost -p 5432 -U postgres -v -f %BACKUP_FILE% finances