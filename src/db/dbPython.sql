







-- Retrieve information about all tables in the 'public' schema
select * from information_schema.tables 
where table_schema = 'investment' and table_type = 'BASE TABLE' 
order by tables;


SELECT * FROM investment."coin_details";
SELECT * FROM investment."coin_prices";
SELECT * FROM investment."coin_types";
SELECT * FROM investment."crypto_coin_types";
SELECT * FROM investment."crypto_coins";
SELECT * FROM investment."crypto_transactions";
SELECT * FROM investment."crypto_wallets";
SELECT * FROM investment."wallet_snapshots";













-- 1. Create schema
CREATE SCHEMA investment;

-- 2. crypto_coins (basic info)
CREATE TABLE IF NOT EXISTS investment.crypto_coins (
    coin_id SERIAL PRIMARY KEY,
    symbol VARCHAR(100) UNIQUE NOT NULL, -- e.g., BTC, ETH
    name VARCHAR(100)  NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


-- 3. coin_details (extended metadata)
CREATE TABLE investment.coin_details (
    detail_id SERIAL PRIMARY KEY,
    coin_id INT NOT NULL UNIQUE,
    full_name VARCHAR(255),
    descriptionInfo TEXT,
    launched_year SMALLINT,
    market_cap_rank INT,
    is_active BOOLEAN DEFAULT TRUE,
    website_url TEXT,
    whitepaper_url TEXT,
    github_url TEXT,
    twitter_url TEXT,
    blockchain_explorer_url TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (coin_id) REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE
);

-- 4. coin_types (categories like memecoin, stablecoin, etc.)
CREATE TABLE investment.coin_types (
    type_id SERIAL PRIMARY KEY,
    type_name VARCHAR(50) UNIQUE NOT NULL
);


-- Insert types
INSERT INTO investment.coin_types (type_name) VALUES
('memecoin'),
('stablecoin'),
('smart contract'),
('privacy');


-- 5. crypto_coin_types (many-to-many relationship)
CREATE TABLE investment.crypto_coin_types (
    coin_id INT NOT NULL,
    type_id INT NOT NULL,
    PRIMARY KEY (coin_id, type_id),
    FOREIGN KEY (coin_id) REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE,
    FOREIGN KEY (type_id) REFERENCES investment.coin_types(type_id) ON DELETE CASCADE
);

-- 6. coin_prices (historical prices in USD and BRL)
CREATE TABLE investment.coin_prices (
    price_id SERIAL PRIMARY KEY,
    coin_id INT NOT NULL,
    usd_price NUMERIC(18, 6) NOT NULL,
    brl_price NUMERIC(18, 6) NOT NULL,
    price_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (coin_id) REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE
);











--------------------------









-- 4. Crypto Wallets: your current holdings per coin and account
CREATE TABLE IF NOT EXISTS investment.crypto_wallets (
    wallet_id SERIAL PRIMARY KEY,
    account_id INT REFERENCES account(id) ON DELETE CASCADE,
    coin_id INT REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE,
    quantity DECIMAL(30, 10) NOT NULL CHECK (quantity >= 0),
    average_buy_price DECIMAL(18, 8),
    total_value_usd NUMERIC(30, 4),  -- quantity * exchange_rates
    total_value_brl NUMERIC(30, 4),  -- quantity * exchange_rates
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
   UNIQUE(account_id, coin_id)
);


-- 5. Crypto Transactions: Buy, Sell, Transfer operations
CREATE TABLE IF NOT EXISTS investment.crypto_transactions (
    tx_id SERIAL PRIMARY KEY,
    account_id INT REFERENCES account(id) ON DELETE CASCADE,
    coin_id INT REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE,
    tx_type VARCHAR(10) CHECK (tx_type IN ('BUY', 'SELL', 'TRANSFER')),
    quantity DECIMAL(30, 10) NOT NULL CHECK (quantity > 0),
    amount DECIMAL(10, 2),    -- new coluam if needed put manual value
    amount_brl DECIMAL(10, 2),    -- new coluam if needed put manual value    
    brokerage_fee DECIMAL(10, 2),
    price_per_coin DECIMAL(18, 8),
    price_per_coin_brl DECIMAL(18, 8),    
    total_value DECIMAL(30, 2) GENERATED ALWAYS AS (quantity * price_per_coin) STORED,
    total_value_brl DECIMAL(30, 2) GENERATED ALWAYS AS (quantity * price_per_coin_brl) STORED,    
    tx_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    notes TEXT
);

-- 7. Alter existing transactions table (if you have it) to add account_id
-- ALTER TABLE transactions ADD COLUMN IF NOT EXISTS account_id INT REFERENCES account(id) ON DELETE SET NULL;

ALTER TABLE transactions
ADD COLUMN IF NOT EXISTS amount_brl DECIMAL(18, 2);








-----

-- 6. Optional: Wallet snapshots to track portfolio value over time
CREATE TABLE IF NOT EXISTS investment.wallet_snapshots (
    snapshot_id SERIAL PRIMARY KEY,
    wallet_id INT REFERENCES investment.crypto_wallets(wallet_id) ON DELETE CASCADE,
    quantity DECIMAL(30, 10),
    market_price_usd DECIMAL(18, 8),
    total_value_usd DECIMAL(30, 2),
    market_price_brl DECIMAL(18, 8),
    total_value_brl DECIMAL(30, 2),
    snapshot_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);












-- TRIGGERS 


CREATE OR REPLACE FUNCTION investment.snapshot_wallet()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO investment.wallet_snapshots (
        wallet_id,
        quantity,
        market_price_usd,
        total_value_usd,
        market_price_brl,
        total_value_brl,
        snapshot_at
    )
    VALUES (
        NEW.wallet_id,
        NEW.quantity,
        (CASE WHEN NEW.quantity > 0 THEN NEW.total_value_usd / NEW.quantity ELSE 0 END),
        NEW.total_value_usd,
        (CASE WHEN NEW.quantity > 0 THEN NEW.total_value_brl / NEW.quantity ELSE 0 END),
        NEW.total_value_brl,
        CURRENT_TIMESTAMP
    );

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;



CREATE TRIGGER trg_snapshot_wallet
AFTER UPDATE ON investment.crypto_wallets
FOR EACH ROW
WHEN (OLD.quantity IS DISTINCT FROM NEW.quantity OR 
      OLD.total_value_usd IS DISTINCT FROM NEW.total_value_usd)
EXECUTE FUNCTION investment.snapshot_wallet();

























CREATE TABLE exchange_rates (
    id SERIAL PRIMARY KEY,
    currency_pair VARCHAR(7) NOT NULL,  -- e.g., 'USD/BRL' or 'BRL/USD'
    rate DECIMAL(10, 6) NOT NULL,
    last_updated TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

-- Insert example data
INSERT INTO exchange_rates (currency_pair, rate) VALUES 
  ('USD/BRL', 5.4482),
  ('BRL/USD', 0.1835);


SELECT * FROM exchange_rates  ORDER BY last_updated DESC LIMIT 2;


















-- -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- OLD 










CREATE TABLE stonks (
    id SERIAL PRIMARY KEY,
    code VARCHAR(10) UNIQUE NOT NULL,
    company_name TEXT NOT NULL,
    country VARCHAR(20),
    current_price NUMERIC(15, 4),
    currency VARCHAR(10),
    last_updated TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);




-- 🇺🇸 US Stocks
INSERT INTO stonks (code, company_name, country) VALUES ('AAPL', 'Apple Inc.', 'USA');
INSERT INTO stonks (code, company_name, country) VALUES ('MSFT', 'Microsoft Corporation', 'USA');
INSERT INTO stonks (code, company_name, country) VALUES ('GOOGL', 'Alphabet Inc. (Google)', 'USA');
INSERT INTO stonks (code, company_name, country) VALUES ('AMZN', 'Amazon.com Inc.', 'USA');
INSERT INTO stonks (code, company_name, country) VALUES ('TSLA', 'Tesla Inc.', 'USA');

-- 🇧🇷 Brazilian Stocks (from B3 - note the .SA suffix)
INSERT INTO stonks (code, company_name, country) VALUES ('PETR4.SA', 'Petrobras PN', 'Brazil');
INSERT INTO stonks (code, company_name, country) VALUES ('VALE3.SA', 'Vale S.A.', 'Brazil');
INSERT INTO stonks (code, company_name, country) VALUES ('ITUB4.SA', 'Itaú Unibanco PN', 'Brazil');
INSERT INTO stonks (code, company_name, country) VALUES ('B3SA3.SA', 'B3 S.A. – Brasil Bolsa Balcão', 'Brazil');
INSERT INTO stonks (code, company_name, country) VALUES ('WEGE3.SA', 'WEG S.A.', 'Brazil');
INSERT INTO stonks (code, company_name, country) VALUES ('MGLU3.SA', 'Magazine Luiza S.A.', 'Brazil');



--pip install pandas yfinance psycopg2-binary requests beautifulsoup4
--pip install pandas yfinance psycopg2-binary
--pip install pandas yfinance psycopg2-binary requests beautifulsoup4
--pip install selenium
--pip install webdriver-manager






-- https://www.dadosdemercado.com.br/acoes




















CREATE TABLE IF NOT EXISTS crypto_coins (
    coin_id SERIAL PRIMARY KEY,
    symbol VARCHAR(100) UNIQUE NOT NULL, -- e.g., BTC, ETH
    name VARCHAR(100)  NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);



CREATE TABLE coin_prices (
    price_id SERIAL PRIMARY KEY,
    coin_id INT NOT NULL,
    usd_price NUMERIC(18, 6) NOT NULL,     -- Price in US Dollar
    brl_price NUMERIC(18, 6) NOT NULL,     -- Price in Brazilian Real
    price_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (coin_id) REFERENCES crypto_coins(coin_id) ON DELETE CASCADE
);




CREATE TABLE crypto_coin_types (
    coin_id INT NOT NULL,
    type_id INT NOT NULL,
    PRIMARY KEY (coin_id, type_id),
    FOREIGN KEY (coin_id) REFERENCES crypto_coins(coin_id) ON DELETE CASCADE,
    FOREIGN KEY (type_id) REFERENCES coin_types(coin_id) ON DELETE CASCADE
);



INSERT INTO coin_types (type_name) VALUES
('memecoin'),
('stablecoin'),
('smart contract'),
('privacy'),
('defi'),
('nft'),
('layer 1'),
('layer 2');





--pip install pycoingecko




































































































































-- -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-- OLD 



-- 1. Crypto Coins: basic info about coins
CREATE TABLE IF NOT EXISTS investment.crypto_coins (
    coin_id SERIAL PRIMARY KEY,
    symbol VARCHAR(100) UNIQUE NOT NULL, -- e.g., BTC, ETH
    name VARCHAR(100) NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


-- 2. Banks Company (optional)
CREATE TABLE IF NOT EXISTS banks_company (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    country VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


-- 3. Account: your fiat or exchange accounts
CREATE TABLE IF NOT EXISTS account (
    id SERIAL PRIMARY KEY,
    title_account VARCHAR(100) NOT NULL,
    balance DECIMAL(18, 2) NOT NULL,
    banks_company_id INT REFERENCES banks_company(id) ON DELETE SET NULL
);


-- 4. Crypto Wallets: your current holdings per coin and account
CREATE TABLE IF NOT EXISTS investment.crypto_wallets (
    wallet_id SERIAL PRIMARY KEY,
    account_id INT REFERENCES account(id) ON DELETE CASCADE,
    coin_id INT REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE,
    quantity DECIMAL(30, 10) NOT NULL CHECK (quantity >= 0),
    average_buy_price DECIMAL(18, 8),
    total_value_usd DECIMAL(30, 2),  -- quantity * market_price_usd
    total_value_brl DECIMAL(30, 2),  -- quantity * market_price_brl
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
   UNIQUE(account_id, coin_id)
);


-- 5. Crypto Transactions: Buy, Sell, Transfer operations
CREATE TABLE IF NOT EXISTS investment.crypto_transactions (
    tx_id SERIAL PRIMARY KEY,
    account_id INT REFERENCES account(id) ON DELETE CASCADE,
    coin_id INT REFERENCES investment.crypto_coins(coin_id) ON DELETE CASCADE,
    tx_type VARCHAR(10) CHECK (tx_type IN ('BUY', 'SELL', 'TRANSFER')),
    quantity DECIMAL(30, 10) NOT NULL CHECK (quantity > 0),
    price_per_coin DECIMAL(18, 8) NOT NULL,
    total_value DECIMAL(30, 2) GENERATED ALWAYS AS (quantity * price_per_coin) STORED,
    tx_date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    notes TEXT
);


-- 6. Optional: Wallet snapshots to track portfolio value over time
CREATE TABLE IF NOT EXISTS investment.wallet_snapshots (
    snapshot_id SERIAL PRIMARY KEY,
    wallet_id INT REFERENCES investment.crypto_wallets(wallet_id) ON DELETE CASCADE,
    quantity DECIMAL(30, 10),
    market_price_usd DECIMAL(18, 8),
    total_value_usd DECIMAL(30, 2),
    market_price_brl DECIMAL(18, 8),
    total_value_brl DECIMAL(30, 2),
    snapshot_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);


-- 7. Alter existing transactions table (if you have it) to add account_id
ALTER TABLE transactions
ADD COLUMN IF NOT EXISTS account_id INT REFERENCES account(id) ON DELETE SET NULL;







