select * from information_schema.tables where table_schema = 'public' and table_type = 'BASE TABLE'


SELECT * FROM "credit_cards";
SELECT * FROM "categories";
SELECT * FROM "payment_methods";
SELECT * FROM "companies";
SELECT * FROM "income";
SELECT * FROM "banks_company";
SELECT * FROM "account";
SELECT * FROM "transactions";
SELECT * FROM "categories_income"



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








-- CREATE\UPDATE HOMOLOG DATABASE
pg_dump -U postgres -h localhost -p 5432 -F c -b -v -f "C:\Users\Akira\Documents\finances.backup" finances



createdb -U postgres -h localhost -p 5432 finances_Homolog

pg_restore -U postgres -h localhost -p 5432 -d finances_Homolog -v "C:\Users\Akira\Documents\finances.backup"






