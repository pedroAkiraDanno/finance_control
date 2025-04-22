



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










-- Retrieving transaction details for a specific user where the company is 'Uber'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, pm.method AS payment_method, co.name AS company, 
t.date_record, t.purchase_date, t.date, cc.card_name, t.is_repeated 
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
WHERE t.user_id = 1 
--ORDER BY t.purchase_date DESC, user_id;



-- Retrieving transaction details for a specific user where the company is 'Uber'
SELECT 
    t.id, 
    t.title, 
    t.description, 
    t.amount, 
    t.type, 
    c.name AS category,
    sc.name AS subcategory,  -- Added subcategory name
    pm.method AS payment_method, 
    co.name AS company, 
    t.date_record, 
    t.purchase_date, 
    t.date, 
    cc.card_name, 
    t.is_repeated,
    a.title_account AS account_name  -- Added account name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id  -- Added LEFT JOIN for subcategories
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id  -- Added LEFT JOIN for account
WHERE t.user_id = 1 
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
ORDER BY t.purchase_date DESC;










SELECT 
    t.purchase_date,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1 -- change the user ID as needed
GROUP BY 
    t.purchase_date
ORDER BY 
    t.purchase_date;







SELECT 
    TO_CHAR(t.purchase_date, 'DD-MM-YYYY') AS purchase_date,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
GROUP BY 
    t.purchase_date
ORDER BY 
    t.purchase_date;







SELECT 
    TO_CHAR(t.purchase_date, 'DD-MM-YYYY') AS purchase_date,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
    AND t.purchase_date = CURRENT_DATE
GROUP BY 
    t.purchase_date
ORDER BY 
    t.purchase_date;







SELECT 
    TO_CHAR(t.purchase_date, 'DD-MM-YYYY') AS purchase_date,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
    AND t.purchase_date = CURRENT_DATE - INTERVAL '1 day'
GROUP BY 
    t.purchase_date
ORDER BY 
    t.purchase_date;








--To show how much you spent on weekends (Saturday and Sunday)
SELECT 
    TO_CHAR(t.purchase_date, 'DD-MM-YYYY') AS purchase_date,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
    AND EXTRACT(DOW FROM t.purchase_date) IN (0, 6)
GROUP BY 
    t.purchase_date
ORDER BY 
    t.purchase_date;






--To show how much you spent on weekends (Saturday , Sunday and friday)
SELECT 
    TO_CHAR(t.purchase_date, 'DD-MM-YYYY') AS purchase_date,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
    AND EXTRACT(DOW FROM t.purchase_date) IN (5, 6, 0) -- Friday, Saturday, Sunday
GROUP BY 
    t.purchase_date
ORDER BY 
    t.purchase_date;











SELECT 
    'This Weekend (Friday to Sunday)' AS weekend_label,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
    AND t.purchase_date >= CURRENT_DATE - ((EXTRACT(DOW FROM CURRENT_DATE)::INT + 2) % 7)  -- Last Friday
    AND t.purchase_date <= CURRENT_DATE + ((7 - EXTRACT(DOW FROM CURRENT_DATE)::INT) % 7)  -- This Sunday







SELECT 
    'This Weekend (Saturday and Sunday)' AS weekend_label,
    SUM(t.amount) AS total_expenses
FROM 
    transactions t
WHERE 
    t.type = 'expense'
    AND t.user_id = 1
    AND t.purchase_date >= CURRENT_DATE - ((EXTRACT(DOW FROM CURRENT_DATE)::INT + 1) % 7)  -- Saturday
    AND t.purchase_date <= CURRENT_DATE + ((7 - EXTRACT(DOW FROM CURRENT_DATE)::INT) % 7)  -- Sunday





UPDATE transactions
SET purchase_date = '2025-04-12'
WHERE id = 28;





UPDATE transactions
SET purchase_date = '2024-12-05'
WHERE id = 7;



-- -------------------------------------------------------------------------------------------------------------------------------------



-- Database backup and restore commands

-- Backup the 'finances' database
"C:\Program Files\PostgreSQL\17\bin\pg_dump.exe" -U postgres -h localhost -p 5432 -F c -b -v -f "C:\PostgreSQL\finances_22042025New.backup" finances;
"C:\Program Files\PostgreSQL\17\bin\pg_dump.exe" -U postgres -h localhost -p 5432 -F p -b -v -f "C:\PostgreSQL\finances_22042025.sql" finances


"C:\Program Files\PostgreSQL\17\bin\psql.exe" -U postgres -h localhost -p 5432 -d finances_Homolog < "C:\PostgreSQL\finances_22042025.sql"


-- Create a new database for homologation (testing purposes)
"C:\Program Files\PostgreSQL\17\bin\createdb.exe" -U postgres -h localhost -p 5432 finances_Homolog

-- Restore the backup into the 'finances_Homolog' database
"C:\Program Files\PostgreSQL\17\bin\pg_restore.exe" -U postgres -h localhost -p 5432 -d finances_Homolog -v "C:\PostgreSQL\finances_22042025New.backup";




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












