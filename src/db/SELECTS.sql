






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




SELECT SUM(amount) AS all_Sum_byMouth_Repeated
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
  AND is_repeated='TRUE';









SELECT SUM(amount) AS all_Sum
FROM "transactions";






-- -------------------------------------------------------------------------------------------------------------------------------------






SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;



SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
INNER JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;







SELECT c.name AS category, SUM(t.amount) AS all_Sum_byMonth
FROM transactions t
LEFT JOIN categories c ON t.category_id = c.id 
WHERE EXTRACT(MONTH FROM t.date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
GROUP BY c.name
ORDER BY all_Sum_byMonth;


SELECT c.name as category , SUM(t.amount) AS all_Sum_byMonth
FROM transactions t
INNER JOIN categories c ON t.category_id = c.id 
GROUP BY category
ORDER BY all_Sum_byMonth;










-- -------------------------------------------------------------------------------------------------------------------------------------




SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, pm.method AS payment_method, co.name AS company, 
t.date_record, t.purchase_date, t.date, cc.card_name, t.is_repeated 
             FROM transactions t 
             LEFT JOIN categories c ON t.category_id = c.id 
             LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
             LEFT JOIN companies co ON t.company_id = co.id 
             LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
             WHERE t.user_id = 1  AND is_repeated='TRUE'
             ORDER BY t.date DESC, user_id







-------------------------------------------------------------------------------------------------------------------------------------






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



















