



-- -------------------------------------------------------------------------------------------------------------------------------------
-- SELECT ABOUT ALL TABLES 




-- Retrieve information about all tables in the 'public' schema
select * from information_schema.tables 
where table_schema = 'public' and table_type = 'BASE TABLE' 
order by tables;





-- all tables
SELECT * FROM "account";
SELECT * FROM "account_type";
SELECT * FROM "banks_company";
SELECT * FROM "categories";
SELECT * FROM "categories_income";
SELECT * FROM "companies";
SELECT * FROM "credit_cards";
SELECT * FROM "income";
SELECT * FROM "invoices";
SELECT * FROM "payment_methods";
SELECT * FROM "spending_limits";
SELECT * FROM "subcategories";
SELECT * FROM "transactions";
SELECT * FROM "transf_account";
SELECT * FROM "user_activity";
SELECT * FROM "users";






-- -------------------------------------------------------------------------------------------------------------------------------------


-- Summing the total amount from the "transactions" table
SELECT SUM(amount) as all_Sum FROM "transactions";




-- Summing the total amount from the "transactions" table for the current month Manual
SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = 5
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);





-- Summing the total amount from the "transactions" table for the current month Automatic
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



-- Summing the total amount by credit card name, and showing % of total
SELECT 
    cc.card_name, 
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    ROUND(
        100.0 * SUM(t.amount) / SUM(SUM(t.amount)) OVER (), 
        2
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;


SELECT 
    cc.card_name, 
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / SUM(SUM(t.amount)) OVER (), 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name
ORDER BY all_Sum_byMonth_Repeated desc



SELECT 
    cc.card_name, 
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / SUM(SUM(t.amount)) OVER (), 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
INNER JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name
ORDER BY all_Sum_byMonth_Repeated desc



-- Get total sum by card, and percentage without using OVER ()
SELECT 
    cc.card_name,
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / total.total_amount, 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
-- Join with a subquery that calculates the total amount
JOIN (
    SELECT SUM(amount) AS total_amount
    FROM transactions
) total ON 1=1
GROUP BY cc.card_name, total.total_amount;






-- Step 1: Get the total sum in a CTE
WITH total_amount_cte AS (
    SELECT SUM(amount) AS total_amount
    FROM transactions
)

-- Step 2: Use that total in your main query
SELECT 
    cc.card_name,
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / total_amount_cte.total_amount, 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id,
     total_amount_cte  -- Reference the total in the FROM clause
GROUP BY cc.card_name, total_amount_cte.total_amount;





-- Set a variable with the total amount
SET @total_amount = (SELECT SUM(amount) FROM transactions);

-- Then run the main query using that variable
SELECT 
    cc.card_name,
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / @total_amount, 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;







SELECT 
    cc.card_name,
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(
            100.0 * SUM(t.amount) / (
                SELECT SUM(amount)
                FROM transactions
            ), 
            2
        ),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;





-- Step 1: Create a total row
SELECT 
    NULL AS card_name,
    SUM(amount) AS total_amount,
    NULL AS percentage_of_total
FROM transactions

UNION ALL

-- Step 2: Get breakdown by card and calculate percentage using scalar subquery
SELECT 
    cc.card_name,
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / (
            SELECT SUM(amount) FROM transactions
        ), 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;




-- Part 1: Total row labeled as 'TOTAL'
SELECT 
    'TOTAL' AS card_name,
    SUM(amount) AS all_Sum_byMonth_Repeated,
    NULL AS percentage_of_total
FROM transactions

UNION ALL

-- Part 2: Per-card sums and percentage
SELECT 
    cc.card_name,
    SUM(t.amount) AS all_Sum_byMonth_Repeated,
    CONCAT(
        ROUND(100.0 * SUM(t.amount) / (
            SELECT SUM(amount) FROM transactions
        ), 2),
        '%'
    ) AS percentage_of_total
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
GROUP BY cc.card_name;







-- Summing the total amount by credit card name, grouped by month and repeated transactions by this mouth 
SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
WHERE EXTRACT(MONTH FROM t.date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
GROUP BY cc.card_name;


SELECT * FROM transactions t 

-- Summing the total amount by credit card name, grouped by month and repeated transactions by this mouth 
SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
WHERE EXTRACT(MONTH FROM t.date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
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




-- Summing the total amount by subcategory for the current month, including category name
SELECT 
    c.name AS category,
    sc.name AS subcategory,
    SUM(t.amount) AS all_Sum_byMonth
FROM transactions t
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id
LEFT JOIN categories c ON sc.category_id = c.id
WHERE EXTRACT(MONTH FROM t.date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
GROUP BY c.name, sc.name
ORDER BY all_Sum_byMonth DESC;





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




       -- INSERT INTO categories (name) VALUES
        ('Taxes and Fees');         






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






-- with subcategory
-- Retrieving transaction details for a specific user where the company is 'Uber'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;










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





-- UPDATE transactions
SET purchase_date = '2025-04-12'
WHERE id = 28;





--UPDATE transactions
SET purchase_date = '2024-12-05'
WHERE id = 7;







-- my travel
-- Summing the total amount from the "transactions" table for the date range 17-04-2025 to 22-04-2025
SELECT SUM(amount) AS all_sum
FROM transactions
WHERE purchase_date BETWEEN '2025-04-17' AND '2025-04-23';




-- with subcategory
-- Retrieving transaction details for a specific user where the company is 'Uber'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 AND purchase_date BETWEEN '2025-04-17' AND '2025-04-23';
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;




















-- with subcategory
-- Retrieving transaction details for a specific user where the company is 'Uber'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 AND EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;










-- Summing the total amount from the "transactions" table for the current month and year
SELECT SUM(amount) AS all_Sum
FROM "transactions"
WHERE EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE);











-- Retrieving transaction details for a specific user where the card is 'c6'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 
  AND EXTRACT(MONTH FROM date_record) = EXTRACT(MONTH FROM CURRENT_DATE)
  AND EXTRACT(YEAR FROM date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
  --AND cc.card_name = 'c6'
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;










-- -------------------------------------------------------------------------------------------------------------------------------------
-- my analizes and UPDATES




-- Retrieving transaction details for a specific user where the card is 'c6'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 
  AND EXTRACT(MONTH FROM purchase_date) = EXTRACT(MONTH FROM CURRENT_DATE) 
  AND EXTRACT(YEAR FROM purchase_date) = EXTRACT(YEAR FROM CURRENT_DATE)
  AND cc.card_name = 'C6 Card'
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;









-- Retrieving transaction details for a specific user where the card is 'c6'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 
  --AND cc.card_name = 'C6 Card'
  AND co.name = 'Google';
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;




-- Retrieving transaction details for a specific user where the card is 'c6'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 
  AND cc.card_name = 'C6 Card'
  --AND co.name = 'Google';
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;





-- Retrieving transaction details for a specific user where the card is 'c6'
SELECT t.id, t.title, t.description, t.amount, t.type, c.name AS category, sc.name AS subcategory, 
       pm.method AS payment_method, co.name AS company, t.date_record, t.purchase_date, t.date, 
       cc.card_name, t.is_repeated, a.title_account AS account_name
FROM transactions t 
LEFT JOIN categories c ON t.category_id = c.id 
LEFT JOIN subcategories sc ON t.subcategory_id = sc.id 
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id 
LEFT JOIN companies co ON t.company_id = co.id 
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
LEFT JOIN account a ON t.account_id = a.id 
WHERE t.user_id = 1 
  AND cc.card_name = 'Mercado Pago Card'
  --AND co.name = 'Google';
-- Optional: Filter for specific company
-- AND co.name = 'Uber'
--ORDER BY t.purchase_date DESC;









SELECT COUNT(*)
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id 
WHERE date_record = '2025-05-05' 
AND cc.card_name = 'C6 Card'





 -- UPDATE transactions
SET date_record = '2025-05-05'
WHERE id IN (11, 12, 15, 16);





 -- UPDATE transactions
SET purchase_date = '2025-04-02'
WHERE id = 47;








-- Summing the total amount by credit card name, grouped by month and repeated transactions by this mouth 
SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
WHERE EXTRACT(MONTH FROM t.date_record) = 3
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
GROUP BY cc.card_name;







-- Summing the total amount by credit card name, grouped by month and repeated transactions by this mouth 
SELECT cc.card_name, SUM(t.amount) AS all_Sum_byMonth_Repeated
FROM transactions t
LEFT JOIN credit_cards cc ON t.credit_card_id = cc.id
WHERE EXTRACT(MONTH FROM t.date_record) = 4
  AND EXTRACT(YEAR FROM t.date_record) = EXTRACT(YEAR FROM CURRENT_DATE)
GROUP BY cc.card_name;




 -- UPDATE transactions
SET date_record = '2025-03-28'
WHERE id = 17;





 -- UPDATE transactions
SET date_record = '2025-05-05'
WHERE id in (21, 22, 23, 26, 39, 41, 42  );











SELECT 
    pm.method AS payment_method, SUM(t.amount) AS total_amount
FROM transactions t
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id
WHERE 
    pm.method IN ('Pix', 'Cash')
    AND t.purchase_date BETWEEN DATE '2025-04-01' AND DATE '2025-04-24'
GROUP BY 
    pm.method;




--SELECT * FROM payment_methods
SELECT 
    pm.method AS payment_method, SUM(t.amount) AS total_amount
FROM transactions t
LEFT JOIN payment_methods pm ON t.payment_method_id = pm.id
WHERE 
    pm.method IN ('Credit Card', 'Debit Card')
    AND t.purchase_date BETWEEN DATE '2025-04-01' AND DATE '2025-04-30'
GROUP BY 
    pm.method;












-- all tables
SELECT * FROM "account";
SELECT * FROM "banks_company";
SELECT * FROM "categories";
SELECT * FROM "categories_income";
SELECT * FROM "companies";
SELECT * FROM "credit_cards";
SELECT * FROM "income";
SELECT * FROM "payment_methods";
SELECT * FROM "subcategories";
SELECT * FROM "transactions";
SELECT * FROM "user_activity";
SELECT * FROM "users";







SELECT * FROM "categories";


SELECT * FROM "subcategories";


SELECT DISTINCT(category_id) FROM  "subcategories";



SELECT DISTINCT c.name, sub.category_id
FROM subcategories sub
INNER JOIN categories c 
ON sub.category_id = c.id;


SELECT * FROM categories
WHERE id NOT IN (10, 11, 19, 21, 12, 15, 1, 4, 20, 2, 17);


SELECT * FROM companies

SELECT t.title, c.name
FROM transactions t
INNER JOIN companies c
ON t.company_id = c.id
WHERE c.name = 'Amazon Prime'

SELECT * FROM categories
SELECT * FROM  transactions t

SELECT t.id , t.title, co.name , ca.name
FROM transactions t
INNER JOIN companies co ON t.company_id = co.id
INNER JOIN categories ca ON t.category_id = ca.id
WHERE co.name = 'Amazon Prime'


-- UPDATE transactions
SET category_id = 18
where id = 11

 -- UPDATE transactions
SET date_record = '2025-05-05'
WHERE id in (21, 22, 23, 26, 39, 41, 42  );



SELECT t.title
FROM transactions t
INNER JOIN companies co ON t.company_id = co.id




        -- Grocery
        --INSERT INTO subcategories (name, category_id) VALUES
        ('Fruits and Vegetables', 19),
        ('Meat and Seafood', 19),
        ('Pantry Items', 19);     




        --INSERT INTO subcategories (name, category_id) VALUES
        -- Food (1)
        ('supermarket', 1);



-- Bills
-- INSERT INTO subcategories (name, category_id) VALUES
('Electricity', 2),
('Water', 2),
('Internet', 2);



-- Home
-- INSERT INTO subcategories (name, category_id) VALUES
('Furniture', 11),
('Appliances', 11),
('Maintenance', 11);




        -- Bills
       -- INSERT INTO subcategories (name, category_id) VALUES
        ('Rent', 2);







        -- Clothing
      --  INSERT INTO subcategories (name, category_id) VALUES
        ('Men', 4),
        ('Women', 4),
        ('Children', 4);


-- Hobbies and Leisure Activities
--INSERT INTO subcategories (name, category_id) VALUES
('Crafts', 17),
('Sports Equipment', 17),
('Workshops', 17);



SELECT * from transactions t 


SELECT t.id, t.title, t.description, co.name, ca.name, t.category_id
FROM transactions t 
INNER JOIN companies co ON t.company_id = co.id
INNER JOIN categories ca ON t.category_id = ca.id 
WHERE co.name = 'MEI Gov'

SELECT SUM(amount)
FROM transactions t 
INNER JOIN companies co ON t.company_id = co.id
WHERE co.name = 'MEI Gov'


-- UPDATE transactions
SET category_id = 22
WHERE id = 9








SELECT SUM(t.amount)
FROM transactions t
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30';





SELECT * from transactions



SELECT pa.method, SUM(t.amount)
FROM transactions t
INNER JOIN payment_methods pa ON t.payment_method_id = pa.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY pa.method;











SELECT * FROM account
SELECT * FROM credit_cards

SELECT ac.title_account, ca.card_name, t.purchase_date, *
FROM transactions t 
INNER JOIN account ac ON t.account_id = ac.id
INNER JOIN credit_cards ca ON t.credit_card_id = ca.id
WHERE ac.id  = 1
ORDER BY t.id


--UPDATE transactions
SET purchase_date = '2025-04-02'
WHERE id = 48













-- -------------------------------------------------------------------------------------------------------------------------------------
















WITH user_credit_cards AS (
  SELECT DISTINCT cc.id, cc.card_name, cc.closes_on_day, cc.due_day
  FROM credit_cards cc
  JOIN transactions t ON cc.id = t.credit_card_id
  WHERE t.user_id = %d
),
card_periods AS (
  SELECT
    ucc.id AS credit_card_id,
    CASE
      WHEN EXTRACT(DAY FROM CURRENT_DATE) >= ucc.closes_on_day THEN
        DATE_TRUNC('month', CURRENT_DATE)::date + (ucc.closes_on_day - 1) * INTERVAL '1 day'
      ELSE
        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '1 month')::date + (ucc.closes_on_day - 1) * INTERVAL '1 day'
    END AS invoice_date,
    CASE
      WHEN EXTRACT(DAY FROM CURRENT_DATE) >= ucc.closes_on_day THEN
        DATE_TRUNC('month', CURRENT_DATE)::date + (ucc.due_day - 1) * INTERVAL '1 day'
      ELSE
        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '1 month')::date + (ucc.due_day - 1) * INTERVAL '1 day'
    END AS due_date,
    CASE
      WHEN EXTRACT(DAY FROM CURRENT_DATE) >= ucc.closes_on_day THEN
        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '1 month')::date + (ucc.closes_on_day - 1) * INTERVAL '1 day'
      ELSE
        DATE_TRUNC('month', CURRENT_DATE - INTERVAL '2 month')::date + (ucc.closes_on_day - 1) * INTERVAL '1 day'
    END AS period_start
  FROM user_credit_cards ucc
)
SELECT cp.credit_card_id, %d AS user_id, cp.invoice_date, cp.due_date,
       COALESCE(SUM(t.amount), 0) AS total_amount
FROM card_periods cp
LEFT JOIN transactions t ON t.credit_card_id = cp.credit_card_id
                         AND t.purchase_date >= cp.period_start
                         AND t.purchase_date < cp.invoice_date
                         AND t.user_id = %d
WHERE NOT EXISTS (
  SELECT 1 FROM invoices i
  WHERE i.credit_card_id = cp.credit_card_id
  AND i.invoice_date = cp.invoice_date
)
GROUP BY cp.credit_card_id, cp.invoice_date, cp.due_date
HAVING COALESCE(SUM(t.amount), 0) > 0;


127	"test credit card (Installment 64/64)"	"test credit card"	156.25	"expense"	"2025-05-05"	"2025-05-13"	1	2	1	2	true		1	"2030-08-05"	1	true	64	64	156.25	64	


-- -------------------------------------------------------------------------------------------------------------------------------------

-- test analyze and updates 



SELECT * FROM  account;

-- mercado pago 
--UPDATE account
SET balance = 2049.55
WHERE ID = 1;

-- c6 
--UPDATE account
SET balance = 0.82
WHERE ID = 3;

-- my wallet 
--UPDATE account
SET balance = 1268
WHERE id = 5;

-- inter
--UPDATE account
SET balance = 0.84
WHERE id = 4;


-- santander
--UPDATE account
SET balance = 1.79
WHERE id = 7;


-- picpay
--UPDATE account
SET balance = 1.33
WHERE id = 6;



-- itau
--UPDATE account
SET balance = 0.50
WHERE id = 2;




SELECT * FROM  account;

SELECT SUM(balance) FROM account;

SELECT * FROM account_type




SELECT * FROM categories

SELECT ca.name, *
FROM transactions t
INNER JOIN categories ca ON t.category_id = ca.id



SELECT SUM(amount) as sum, ca.name
FROM transactions t
INNER JOIN categories ca ON t.category_id = ca.id
GROUP BY ca.name
ORDER BY sum


SELECT SUM(amount) as sum, ca.name
FROM transactions t
INNER JOIN categories ca ON t.category_id = ca.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name
ORDER BY sum



SELECT SUM(amount) as sum, ca.name
FROM transactions t
INNER JOIN categories ca ON t.category_id = ca.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name
ORDER BY sum


SELECT SUM(amount) as sum, ca.name
FROM transactions t
INNER JOIN categories ca ON t.category_id = ca.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name
ORDER BY sum



SELECT 
    ca.name AS category,
    SUM(t.amount) AS sum
FROM transactions t
JOIN categories ca ON t.category_id = ca.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name WITH ROLLUP;








-- Per-category sums
SELECT 
    ca.name AS category,
    SUM(t.amount) AS sum
FROM transactions t
JOIN categories ca ON t.category_id = ca.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name
UNION ALL
-- Total sum
SELECT 
    'Total' AS category,
    SUM(t.amount) AS sum
FROM transactions t
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30';




SELECT 
    COALESCE(ca.name, 'Total') AS category,
    SUM(t.amount) AS sum
FROM transactions t
JOIN categories ca ON t.category_id = ca.id
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY GROUPING SETS ((ca.name), ())
ORDER BY 
    CASE WHEN ca.name IS NULL THEN 1 ELSE 0 END,
    sum;










-- Per-category sums
SELECT 
    ca.name AS category,
    AVG(t.amount) AS sum
FROM transactions t
JOIN categories ca ON t.category_id = ca.id
--WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name
UNION ALL
-- Total sum
SELECT 
    'Total' AS category,
    SUM(t.amount) AS sum
FROM transactions t
WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30';






SELECT 
    TO_CHAR(t.purchase_date, 'YYYY-MM') AS month,
    ca.name AS category,
    SUM(t.amount) AS sum
FROM transactions t
JOIN categories ca ON t.category_id = ca.id
GROUP BY GROUPING SETS (
    (TO_CHAR(t.purchase_date, 'YYYY-MM'), ca.name),
    (TO_CHAR(t.purchase_date, 'YYYY-MM')),
    ()
)
ORDER BY
    month NULLS LAST,
    category NULLS LAST;












SELECT SUM(amount) as sum, ca.name as name, TO_CHAR(t.purchase_date, 'YYYY-MM') as MOUTH
FROM transactions t
INNER JOIN categories ca ON t.category_id = ca.id
-- WHERE t.purchase_date BETWEEN '2025-04-01' AND '2025-04-30'
GROUP BY ca.name, MOUTH, media
ORDER BY name, sum



SELECT 
    SUM(t.amount) AS sum,
    AVG(t.amount) AS media,
    ca.name AS name,
    TO_CHAR(t.purchase_date, 'YYYY-MM') AS mouth
FROM transactions t
JOIN categories ca ON t.category_id = ca.id
GROUP BY ca.name, TO_CHAR(t.purchase_date, 'YYYY-MM')
ORDER BY name, sum;


select * from companies

SELECT co.name, co.id,t.purchase_date, * 
FROM transactions t
INNER JOIN companies co ON t.company_id = co.id
WHERE co.name = 'Amazon Prime'


select * from categories
SELECT su.id, su.name, su.category_id, ca.name 
FROM subcategories su
LEFT JOIN categories ca ON su.category_id = ca.id


SELECT ca.id, ca.name, su.id AS subcategory_id, su.name AS subcategory_name
FROM categories ca
LEFT JOIN subcategories su ON su.category_id = ca.id



SELECT * FROM companies

SELECT co.name,t.purchase_date, * 
FROM transactions t
INNER JOIN companies co ON t.company_id = co.id
WHERE co.name = 'Luuna Bed'


7
48




SELECT * FROM categories

DELETE FROM transactions
WHERE id IN (7, 48);


SELECT * FROM transactions
WHERE id IN (7, 48);







DELETE FROM transactions
WHERE id IN (7, 48);


SELECT * FROM transactions
WHERE id IN (7, 48);

SELECT * FROM transactions
where company_id = 11




SELECT co.name,t.purchase_date, * 
FROM transactions t
INNER JOIN companies co ON t.company_id = co.id
WHERE co.name = 'Luuna Bed'






SELECT * FROM transactions













select * from account


-- UPDATE account 
SET balance = balance + 395.31
WHERE id = 1;
-- 2049.55 | 2444.86

-- UPDATE account
SET balance = balance + 239.84 
WHERE id = 3; 
-- 0.82 | 240.66

-- UPDATE account 
SET balance = balance + 433.20
WHERE id = 4;
-- 0.84 | 434.04




-------------------------------------------------------------------------------------------------------------------------------------





SELECT * FROM account
3


SELECT ac.title_account, purchase_date ,*	
FROM public.transactions tr
INNER JOIN account ac ON tr.account_id = ac.id
WHERE tr.account_id = 3
AND tr.title not ilike '%invoice%'
order by 2
AND purchase_date = 




SELECT ac.title_account, purchase_date ,*	
FROM public.transactions tr
INNER JOIN account ac ON tr.account_id = ac.id
WHERE tr.account_id = 1
AND tr.title not ilike '%invoice%'
AND tr.purchase_date between '2025-05-07' and '2025-05-23'
order by 2






-- -------------------------------------------------------------------------------------------------------------------------------------



-- Database backup and restore commands

-- start postgresql 
-- net start postgresql-x64-17



-- Backup the 'finances' database
--"C:\Program Files\PostgreSQL\17\bin\pg_dump.exe" -U postgres -h localhost -p 5432 -F c -b -v -f "C:\PostgreSQL\finances_22042025New.backup" finances;
"C:\Program Files\PostgreSQL\17\bin\pg_dump.exe" -U postgres -h localhost -p 5432 -F p -b -v -f "C:\PostgreSQL\finances_11072025_New.sql" finances



-- Create a new database for homologation (testing purposes)
-- "C:\Program Files\PostgreSQL\17\bin\dropdb.exe" -U postgres -h localhost -p 5432 --force finances_BI
"C:\Program Files\PostgreSQL\17\bin\createdb.exe" -U postgres -h localhost -p 5432 finances_Dev




-- Restore the backup into the 'finances_Homolog' database
--"C:\Program Files\PostgreSQL\17\bin\pg_restore.exe" -U postgres -h localhost -p 5432 -d finances_Homolog -v "C:\PostgreSQL\finances_22042025New.backup";
"C:\Program Files\PostgreSQL\17\bin\psql.exe" -U postgres -h localhost -p 5432 -d finances_Dev < "C:\PostgreSQL\finances_11072025_New.sql"




-- AZURE service POSTGRESQL
-- "C:\Program Files\PostgreSQL\17\bin\psql.exe" -U postgresadm -h srvpostgresql23062025.postgres.database.azure.com -p 5432 -d finances_Dev < "C:\PostgreSQL\finances_21062025_New.sql"


-- start postgres windows 
-- net start postgresql-x64-17
-- "C:\Program Files\PostgreSQL\17\bin\pg_ctl.exe" start -D "C:\Program Files\PostgreSQL\17\data"







-- -------------------------------------------------------------------------------------------------------------------------------------




-- Dropping tables in reverse order to avoid foreign key constraint issues
-- DROP TABLE IF EXISTS categories CASCADE;
-- DROP TABLE IF EXISTS categories_income CASCADE;
-- DROP TABLE IF EXISTS companies CASCADE;
-- DROP TABLE IF EXISTS credit_cards CASCADE;
-- DROP TABLE IF EXISTS income CASCADE;
-- DROP TABLE IF EXISTS payment_methods CASCADE;
-- DROP TABLE IF EXISTS transactions CASCADE;
-- DROP TABLE IF EXISTS users CASCADE;
-- DROP TABLE IF EXISTS account CASCADE;
-- DROP TABLE IF EXISTS banks_company CASCADE;

-- Optional: Drop other objects (like functions, types, etc.) if needed
-- You can add commands to drop sequences or indexes if created manually.

-- Optional: Drop constraints if there are any
-- DROP CONSTRAINT IF EXISTS <constraint_name>;












