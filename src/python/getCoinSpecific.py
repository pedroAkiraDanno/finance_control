import sys
import psycopg2
from pycoingecko import CoinGeckoAPI

# === Get coin symbol from command line ===
if len(sys.argv) < 2:
    print("❌ Please provide a coin symbol. Example:")
    print("   python GetCoinPrice.py BTC")
    sys.exit(1)

symbol_to_find = sys.argv[1].lower()

# === 1. Connect to Postgres ===
conn = psycopg2.connect(
    host="localhost", port=5432, dbname="finances", user="postgres", password="p0w2i8"
)
cur = conn.cursor()

# === 2. Create Table if it doesn't exist ===
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

# === 3. Search Coin in CoinGecko ===
cg = CoinGeckoAPI()
all_coins = cg.get_coins_list()

# Find matching coin ID by symbol
coin_info = next((coin for coin in all_coins if coin['symbol'].lower() == symbol_to_find), None)

if not coin_info:
    print(f"❌ Coin with symbol '{symbol_to_find.upper()}' not found on CoinGecko.")
    sys.exit(1)

coin_id = coin_info['id']

# === 4. Get coin details ===
coin_data = cg.get_coin_by_id(coin_id)
symbol = coin_data['symbol'].upper()
name = coin_data['name']
desc = coin_data['description']['en'][:300] or f"{name} ({symbol})"  # Shorten long descriptions

# === 5. Insert into DB ===
cur.execute("""
  INSERT INTO investment.crypto_coins (symbol, name, description)
  VALUES (%s, %s, %s)
  ON CONFLICT (symbol) DO NOTHING;
""", (symbol, name, desc))
conn.commit()

print(f"✅ Inserted coin: {symbol} - {name}")

# === 6. Show inserted coin ===
cur.execute("SELECT coin_id, symbol, name, created_at FROM investment.crypto_coins WHERE symbol = %s;", (symbol,))
for row in cur.fetchall():
    print(row)

cur.close()
conn.close()
