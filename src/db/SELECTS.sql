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



credit_cards
1	"Inter Card"	4540.00	22	22
2	"C6 Card"	4940.11	29	5
3	"Mercado Pago Card"	4100.00	29	4



account
1	"Mercado Pago Account"	0.00	17
2	"Itau Account"	0.00	4
3	"C6 Bank Account"	0.00	25
4	"Inter"	0.00	15
5	"My Wallet"	0.00	1








            ('Cash'),   
            ('Credit Card'),
            ('Debit Card'),
            ('Pix');  
		
UPDATE payment_methods
SET method = 'Credit Card'
WHERE id = '2';











pg_dump -U postgres -h localhost -p 5432 -F c -b -v -f "C:\Users\Akira\Documents\finances.backup" finances



createdb -U postgres -h localhost -p 5432 finances_Homolog

pg_restore -U postgres -h localhost -p 5432 -d finances_Homolog -v "C:\Users\Akira\Documents\finances.backup"






