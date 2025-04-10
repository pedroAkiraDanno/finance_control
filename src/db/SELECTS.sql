



-- -------------------------------------------------------------------------------------------------------------------------------------
-- SELECT ABOUT ALL TABLES 




-- Retrieve information about all tables in the 'public' schema
select * from information_schema.tables 
where table_schema = 'public' and table_type = 'BASE TABLE' 
order by tables;





-- all tables
SELECT * FROM "account";
SELECT * FROM "banks_company";
SELECT * FROM "categories";
SELECT * FROM "categories_income";
SELECT * FROM "companies";
SELECT * FROM "credit_cards";
SELECT * FROM "income";
SELECT * FROM "payment_methods";
SELECT * FROM "transactions";
SELECT * FROM "user_activity";
SELECT * FROM "users";










-- -------------------------------------------------------------------------------------------------------------------------------------


-- Summing the total amount from the "transactions" table
SELECT SUM(amount) as all_Sum FROM "transactions";




-- Summing the total amount from the "transactions" table for the current month
SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = 4
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);



-- Summing the total amount from the "transactions" table for the current month and year
SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);




-- Summing the total amount from the "transactions" table for the current month and year where 'is_repeated' is 'TRUE'
SELECT SUM(amount) AS all_Sum_byMouth_Repeated
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
  AND is_repeated='TRUE';




-- Summing the total amount from the "transactions" table for all records
SELECT SUM(amount) AS all_Sum
FROM "transactions";






-- -------------------------------------------------------------------------------------------------------------------------------------






-- Summing the total amount by credit card name, grouped by month and repeated transactions
SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;




-- Summing the total amount by credit card name, using INNER JOIN to ensure matching transactions only
SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
INNER JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;




-- Summing the total amount by category for the current month
SELECT c.name AS category, SUM(t.amount) AS all_Sum_byMonth
FROM transactions t
LEFT JOIN categories c ON t.category_id = c.id 
WHERE EXTRACT(MONTH FROM t.date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
GROUP BY c.name
ORDER BY all_Sum_byMonth;



-- Summing the total amount by category using INNER JOIN to ensure matching categories only
SELECT c.name as category , SUM(t.amount) AS all_Sum_byMonth
FROM transactions t
INNER JOIN categories c ON t.category_id = c.id 
GROUP BY category
ORDER BY all_Sum_byMonth;




-- -------------------------------------------------------------------------------------------------------------------------------------





-- Retrieving transaction details for a specific user where 'is_repeated' is 'TRUE'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, pm.method AS payment_method, co.name AS company, 
t.date_record, t.purchase_date, t.date, cc.card_name, t.is_repeated 
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
WHERE t.user_id = 1  AND is_repeated='TRUE'
ORDER BY t.date DESC, user_id;



-- Retrieving transaction details for a specific user where the company is 'Uber'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, pm.method AS payment_method, co.name AS company, 
t.date_record, t.purchase_date, t.date, cc.card_name, t.is_repeated 
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
WHERE t.user_id = 1  AND co.name = 'Uber'
ORDER BY t.purchase_date DESC, user_id;





-- -------------------------------------------------------------------------------------------------------------------------------------



-- Database backup and restore commands

-- Backup the 'finances' database
pg_dump -U postgres -h localhost -p 5432 -F c -b -v -f "C:\Users\Akira\Documents\finances.backup" finances;

-- Create a new database for homologation (testing purposes)
createdb -U postgres -h localhost -p 5432 finances_Homolog;

-- Restore the backup into the 'finances_Homolog' database
pg_restore -U postgres -h localhost -p 5432 -d finances_Homolog -v "C:\Users\Akira\Documents\finances.backup";




-- -------------------------------------------------------------------------------------------------------------------------------------




-- Dropping tables in reverse order to avoid foreign key constraint issues
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

-- Optional: Drop other objects (like functions, types, etc.) if needed
-- You can add commands to drop sequences or indexes if created manually.

-- Optional: Drop constraints if there are any
-- DROP CONSTRAINT IF EXISTS <constraint_name>;












