






-- -------------------------------------------------------------------------------------------------------------------------------------


select * from information_schema.tables where table_schema = 'public' and table_type = 'BASE TABLE' order by tables


SELECT * FROM "account";
SELECT * FROM "banks_company";
SELECT * FROM "categories";
SELECT * FROM "categories_income";
SELECT * FROM "companies";
SELECT * FROM "credit_cards";
SELECT * FROM "income";
SELECT * FROM "payment_methods";
SELECT * FROM"transactions";
SELECT * FROM "users";





SELECT * FROM "transactions";






SELECT * FROM "transactions";


SELECT SUM(amount) as all_Sum FROM "transactions" ;



SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = 3
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);



SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);




SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
  AND is_repeated='TRUE';




-- -------------------------------------------------------------------------------------------------------------------------------------



-- CREATE\UPDATE HOMOLOG DATABASE
pg_dump -U postgres -h localhost -p 5432 -F c -b -v -f "C:\Users\Akira\Documents\finances.backup" finances



createdb -U postgres -h localhost -p 5432 finances_Homolog

pg_restore -U postgres -h localhost -p 5432 -d finances_Homolog -v "C:\Users\Akira\Documents\finances.backup"













-- -------------------------------------------------------------------------------------------------------------------------------------





-- Drop the tables in reverse order of creation to avoid foreign key constraint issues
DROP TABLE IF EXISTS categories CASCADE;
DROP TABLE IF EXISTS categories_income CASCADE;
DROP TABLE IF EXISTS companies CASCADE;
DROP TABLE IF EXISTS credit_cards CASCADE;
DROP TABLE IF EXISTS income CASCADE;
DROP TABLE IF EXISTS payment_methods CASCADE;
DROP TABLE IF EXISTS transactions CASCADE;
DROP TABLE IF EXISTS users CASCADE;
DROP TABLE IF EXISTS account CASCADE;
DROP TABLE IF EXISTS banks_company CASCADE;

-- Drop any other objects (like functions, types, etc.) if necessary
-- You can add commands to drop sequences or indexes if created manually.

-- Drop constraints if there are any
-- DROP CONSTRAINT IF EXISTS <constraint_name>;



















