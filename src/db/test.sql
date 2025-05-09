









            -- Transactions table (for expenses)
            CREATE TABLE transactions (
                id SERIAL PRIMARY KEY,
                title VARCHAR(100), -- Run the following SQL command to add the title column to the transactions table:
                description VARCHAR(100) NOT NULL,
                amount DECIMAL(10, 2) NOT NULL,
                type VARCHAR(10) CHECK (type IN ('income', 'expense')) NOT NULL,
                date_record DATE NOT NULL, -- Manually input by the user
                date DATE DEFAULT CURRENT_DATE, -- Automatically set by the database
                category_id INT REFERENCES categories(id) ON DELETE SET NULL,
                payment_method_id INT REFERENCES payment_methods(id) ON DELETE SET NULL,
                company_id INT REFERENCES companies(id) ON DELETE SET NULL
            );







            -- Income table
            CREATE TABLE income (
                id SERIAL PRIMARY KEY,
                description VARCHAR(100) NOT NULL,
                amount DECIMAL(10, 2) NOT NULL,
                date_record DATE NOT NULL, -- Manually input by the user
                date DATE DEFAULT CURRENT_DATE, -- Automatically set by the database
                category_income_id INT REFERENCES categories_income(id) ON DELETE SET NULL,
                payment_method_id INT REFERENCES payment_methods(id) ON DELETE SET NULL
            );






            CREATE TABLE account (
                id SERIAL PRIMARY KEY,
                title_account VARCHAR(100) NOT NULL,
                balance DECIMAL(10, 2) NOT NULL,
                banks_company_id INT REFERENCES banks_company(id) ON DELETE SET NULL
            );




            -- Insert initial data for income categories
            INSERT INTO categories_income (name) VALUES
            ('Payments'),
            ('Commission'),
            ('Recurring'),
            ('Sales'),
            ('Savings');









            -- Payment methods
            CREATE TABLE payment_methods (
                id SERIAL PRIMARY KEY,
                method VARCHAR(50) NOT NULL UNIQUE
            );





            -- Insert initial data for payment methods
            INSERT INTO payment_methods (method) VALUES
            ('Cash'),   
            ('Credit Card'),
            ('Debit Card'),
            ('Pix');  

      











Enter your choice: 1
Enter description: Salary
Enter amount: 1500
ID | Income Category
-------------------
1 | Payments
2 | Commission
3 | Recurring
4 | Sales
5 | Savings
Enter income category ID: 1
ID | Payment Method
------------------
1 | Cash
2 | Credit Card
3 | Debit Card
4 | Pix
Enter payment method ID (1 for Cash, 2 for Credit Card, 3 for Debit Card): 1
Enter date record (YYYY-MM-DD): 2025-05-05
Income added successfully!













ALTER TABLE income
ADD COLUMN account_id INT REFERENCES account(id) ON DELETE SET NULL;













































-- Table for invoice statuses
CREATE TABLE invoice_status (
    id SERIAL PRIMARY KEY,
    name VARCHAR(20) UNIQUE NOT NULL CHECK (name IN ('pending', 'paid', 'overdue')),
    description TEXT
);

-- Insert predefined statuses
INSERT INTO invoice_status (name) VALUES
('pending'),
('paid'),
('overdue');

-- Enhanced invoice table
CREATE TABLE invoice (
    id SERIAL PRIMARY KEY,
    invoice_number VARCHAR(50) NOT NULL UNIQUE,       -- Unique invoice identifier (e.g., INV-2024-001)
    issue_date DATE NOT NULL,                         -- Date the invoice was issued
    due_date DATE NOT NULL,                           -- Payment due date
    amount DECIMAL(10, 2) NOT NULL,                   -- Total invoice amount
    description TEXT,                                 -- Optional description
    user_id INT REFERENCES users(id) ON DELETE CASCADE, -- Owner of the invoice
    account_id INT REFERENCES account(id) ON DELETE SET NULL, -- Payment account (if applicable)
    credit_card_id INT REFERENCES credit_cards(id) ON DELETE SET NULL, -- Credit card used for payment
    invoice_status_id INT REFERENCES invoice_status(id) NOT NULL DEFAULT 1, -- Status (default: pending)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Optional: Add indexes for faster lookups
CREATE INDEX idx_invoice_user ON invoice(user_id);
CREATE INDEX idx_invoice_account ON invoice(account_id);
CREATE INDEX idx_invoice_credit_card ON invoice(credit_card_id);












Transferir contas - OK 
Definir limits 
Metas 
Avisos e notificações
Parcelas 
Salário | investimentos 
Borrow
faturas pagar por exemplo também 
alterando "income"


Investimos para mim é outro programa mas não sei































Invoice #3 Details:
ID | Title            | Description       | Amount  | Purchase Date | Category    | Subcategory
-------------------------------------------------------------------------------------------
47 | Smart Fit       | Smart Fit Gym     | 149.05  | 2025-04-02    | Health      | Gym and Fitness
50 | Sugar in Sant Marche | Pay half with mother sugar and my credit card Sant Marche | 1.39    | 2025-04-27    | Food        | Lunch


21 - OK 
23 - OK 
22 - OK 
26 - OK 
49 - OK 
39 - OK 
41 - OK 
42 - OK 


50 
47


























Invoice #14 Details:
ID | Title            | Description       | Amount  | Purchase Date | Category    | Subcategory
-------------------------------------------------------------------------------------------
15 | Uber Travel     | Uber to Recanto Paulista Because my ankle. | 21.93   | 2025-04-01    | Transport   |
16 | Uber Travel     | Uber to Recanto Paulista to work and help my father. | 24.94   | 2025-04-02    | Transport   |
46 | Google GSUITE   | Google GSUITE to my Google Drive 2TB | 70.00   | 2025-04-05    | Services    |
20 | Jet Scooter     | Use jet Scooter with Saty and Gio, to back gym to home. because the car was broke. | 17.78   | 2025-04-11    | Transport   |
28 | Uber Trip       | Uber Shopping Vila Olimpia to Home, because the car was broke. | 16.47   | 2025-04-12    | Transport   |
25 | Uber Trip       | Uber Church to Home, because the car was broke | 10.98   | 2025-04-13    | Transport   |
24 | Uber Trip       | Uber Home to Church with my father and gradmother because the car was broke | 13.94   | 2025-04-13    | Transport   |
51 | Uber to Church  | Uber Sp to Church because the car mechanic | 10.92   | 2025-04-27    | Transport   | Taxi and Ride Apps
52 | Uber Church to Home | Uber Church to Home because mechanic | 10.99   | 2025-04-27    | Transport   | Taxi and Ride Apps


15
16
46
20
28
25
24
52



11
12
20
28
31













-----------------------------------




-- FIX TO INVOICE 



-- gendai 
-- OK 

-- lunna 
UPDATE transactions
SET purchase_date = '2025-04-02'
WHERE id = 48 


-- sugar 
DELETE FROM transactions
WHERE id = 50;
-- need add again 


UPDATE Transactions 
SET amount = 26.5 
WHERE id = 41





-- c6 fix 







-----------------------------------


