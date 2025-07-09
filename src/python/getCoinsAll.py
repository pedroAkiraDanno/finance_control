import psycopg2
from pycoingecko import CoinGeckoAPI
import time

# PostgreSQL database configuration
DB_CONFIG = {
    "host": "localhost",
    "port": 5432,
    "dbname": "finances_Dev",       # Change this
    "user": "postgres",       # Change this
    "password": "p0w2i8"  # Change this
}

# Connect to the PostgreSQL database
conn = psycopg2.connect(**DB_CONFIG)
cur = conn.cursor()

# Create the table if it doesn't exist
cur.execute("""
  CREATE TABLE IF NOT EXISTS investment.crypto_coins (
    coin_id SERIAL PRIMARY KEY,
    symbol VARCHAR(100) UNIQUE NOT NULL,
    name VARCHAR(100) UNIQUE NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
""")
conn.commit()

# Initialize CoinGecko API
cg = CoinGeckoAPI()
per_page = 250  # Max allowed by CoinGecko
page = 1
total_inserted = 0

while True:
    print(f"Fetching page {page}...")
    try:
        data = cg.get_coins_markets(
            vs_currency='usd',
            order='market_cap_desc',
            per_page=per_page,
            page=page
        )
    except Exception as e:
        print(f"Error fetching data on page {page}: {e}")
        break

    if not data:
        print("No more data.")
        break

    for coin in data:
        try:
            symbol = coin['symbol'].upper()
            name = coin['name'].encode('utf-8', 'ignore').decode('utf-8')[:50]  # Clean and trim
            desc = f"{name} ({symbol}), market cap rank #{coin.get('market_cap_rank', '?')}"
            cur.execute("""
              INSERT INTO investment.crypto_coins (symbol, name, description)
              VALUES (%s, %s, %s)
              ON CONFLICT (symbol) DO NOTHING;
            """, (symbol, name, desc))
            conn.commit()
            total_inserted += 1
        except Exception as e:
            print(f"Error inserting {name}: {e}")
            conn.rollback()

    page += 1
    time.sleep(1.5)  # Respect API rate limits

print(f"✅ Finished. Total coins inserted: {total_inserted}")

# Confirm total entries
cur.execute("SELECT COUNT(*) FROM investment.crypto_coins;")
print("Total coins in database:", cur.fetchone()[0])

# Clean up
cur.close()
conn.close()
