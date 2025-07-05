

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



























































-- 1. Create schema
CREATE SCHEMA investment;

-- 2. crypto_coins (basic info)
CREATE TABLE IF NOT EXISTS crypto_coins (
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
INSERT INTO finances.coin_types (type_name) VALUES
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








