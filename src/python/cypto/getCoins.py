import psycopg2
from pycoingecko import CoinGeckoAPI

# 1. Connect to Postgres
conn = psycopg2.connect(
    host="localhost", port=5432, dbname="finances_Dev", user="postgres", password="p0w2i8"
)
cur = conn.cursor()
cur.execute("""
  CREATE TABLE IF NOT EXISTS crypto_coins (
    coin_id SERIAL PRIMARY KEY,
    symbol VARCHAR(10) UNIQUE NOT NULL,
    name VARCHAR(50) UNIQUE NOT NULL,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );
""")
conn.commit()

# 2. Fetch top coins
cg = CoinGeckoAPI()
data = cg.get_coins_markets(vs_currency='usd', order='market_cap_desc', per_page=15, page=1)

# 3. Insert coins
for coin in data:
    symbol = coin['symbol'].upper()
    name = coin['name']
    desc = f"{name} ({symbol}), market cap rank #{coin.get('market_cap_rank', '?')}"
    cur.execute("""
      INSERT INTO crypto_coins (symbol, name, description)
      VALUES (%s, %s, %s)
      ON CONFLICT (symbol) DO NOTHING;
    """, (symbol, name, desc))
conn.commit()

# 4. Confirm entries
cur.execute("SELECT coin_id, symbol, name, created_at FROM crypto_coins;")
for row in cur.fetchall():
    print(row)

cur.close()
conn.close()







