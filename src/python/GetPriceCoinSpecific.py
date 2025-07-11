import psycopg2
from pycoingecko import CoinGeckoAPI
from datetime import datetime, timezone
import time
from decimal import Decimal, ROUND_HALF_UP
import sys

# --- Accept coin_id as a command-line argument ---
if len(sys.argv) != 2:
    print("Usage: python update_price.py <coin_id>")
    sys.exit(1)

input_coin_id = sys.argv[1]

# PostgreSQL connection
conn = psycopg2.connect(
    host="localhost",
    port=5432,
    dbname="finances",
    user="postgres",
    password="p0w2i8"
)
cur = conn.cursor()

# CoinGecko API
cg = CoinGeckoAPI()

# Step 1: Fetch the specified coin from your database
cur.execute("""
    SELECT coin_id, symbol, name 
    FROM investment.crypto_coins 
    WHERE coin_id = %s
""", (input_coin_id,))
coin = cur.fetchone()

if not coin:
    print(f"❌ Coin with ID {input_coin_id} not found in the database.")
    sys.exit(1)

coin_id, symbol, name = coin

# Step 2: Fetch CoinGecko coin list and build lookup
print("Fetching CoinGecko coin list...")
cg_coins = cg.get_coins_list()
symbol_to_id = {c['symbol'].upper(): c['id'] for c in cg_coins}
name_to_id = {c['name'].lower(): c['id'] for c in cg_coins}

# Step 3: Get CoinGecko ID
cg_id = symbol_to_id.get(symbol.upper()) or name_to_id.get(name.lower())
if not cg_id:
    print(f"❌ No CoinGecko ID found for {name} ({symbol})")
    sys.exit(1)

# Step 4: Fetch price and insert
try:
    price_data = cg.get_price(ids=cg_id, vs_currencies=['usd', 'brl'])

    if cg_id in price_data:
        usd_raw = price_data[cg_id].get('usd')
        brl_raw = price_data[cg_id].get('brl')

        usd = Decimal(usd_raw).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP) if usd_raw is not None else None
        brl = Decimal(brl_raw).quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP) if brl_raw is not None else None

        if usd is not None and brl is not None:
            cur.execute("""
                INSERT INTO investment.coin_prices (coin_id, usd_price, brl_price, price_timestamp)
                VALUES (%s, %s, %s, %s)
            """, (
                coin_id,
                usd,
                brl,
                datetime.now(timezone.utc)
            ))
            conn.commit()
            print(f"✅ Inserted price for {name:<25} USD={usd:.8f} BRL={brl:.8f}")
        else:
            print(f"⚠️ Missing price for {cg_id}")
    else:
        print(f"⚠️ No price data found for {cg_id}")

except Exception as e:
    print(f"❌ Error inserting price for {name} ({symbol}): {e}")
    conn.rollback()

# Close connection
cur.close()
conn.close()
