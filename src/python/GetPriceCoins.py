import psycopg2
from pycoingecko import CoinGeckoAPI
from datetime import datetime, timezone
import time
from decimal import Decimal, ROUND_HALF_UP

# PostgreSQL connection
conn = psycopg2.connect(
    host="localhost",
    port=5432,
    dbname="finances_Dev",         # 🔁 Change this
    user="postgres",         # 🔁 Change this
    password="p0w2i8"  # 🔁 Change this
)
cur = conn.cursor()

# CoinGecko API
cg = CoinGeckoAPI()

# Step 1: Fetch all coins from your database
cur.execute("SELECT coin_id, symbol, name FROM investment.crypto_coins")
db_coins = cur.fetchall()  # List of (coin_id, symbol, name)

# Step 2: Fetch CoinGecko coin list and build lookup
print("Fetching CoinGecko coin list...")
cg_coins = cg.get_coins_list()
symbol_to_id = {}
name_to_id = {}

for coin in cg_coins:
    symbol_to_id[coin['symbol'].upper()] = coin['id']
    name_to_id[coin['name'].lower()] = coin['id']

# Step 3: For each coin in DB, get current price and insert into coin_prices
for coin_id, symbol, name in db_coins:
    cg_id = None

    # Match CoinGecko ID using symbol or name
    if symbol.upper() in symbol_to_id:
        cg_id = symbol_to_id[symbol.upper()]
    elif name.lower() in name_to_id:
        cg_id = name_to_id[name.lower()]
    else:
        print(f"❌ No CoinGecko ID found for {name} ({symbol})")
        continue

    try:
        # Get live price in USD and BRL
        price_data = cg.get_price(ids=cg_id, vs_currencies=['usd', 'brl'])

        if cg_id in price_data:
            usd_raw = price_data[cg_id].get('usd')
            brl_raw = price_data[cg_id].get('brl')

            # Convert to Decimal and round to 6 digits (Postgres schema: NUMERIC(18, 6))
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

    # Respect CoinGecko rate limits
    time.sleep(1.2)

# Close connection
cur.close()
conn.close()
