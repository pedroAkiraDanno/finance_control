



SELECT id, title, description, amount, type, date_record, date, category_id, payment_method_id, company_id, credit_card_id, is_repeated, account_id, user_id, purchase_date, subcategory_id, is_installment, installment_number, total_installments, installment_value, original_transaction_id, debit_card_id
FROM public.transactions
WHERE company_id IS NULL;


SELECT * FROM companies
WHERE name like '%graal%'

INSERT INTO companies (name, location)
VALUES ('Chiquinho Taubate', 'Taubate'); --110


INSERT INTO companies (name, location)
VALUES ('Graal', 'Sao Carlos'); --111



update transactions
SET company_id = 110
WHERE id = 43 



update transactions
SET company_id = 111
WHERE id = 42 





