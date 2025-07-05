





import yfinance as yf
import psycopg2
from datetime import datetime

# Connect to PostgreSQL
conn = psycopg2.connect(
    dbname="finances_Dev",
    user="postgres",
    password="p0w2i8",
    host="localhost",
    port="5432"
)
cursor = conn.cursor()

# Get all codes from the table
cursor.execute("SELECT code FROM stonks")
rows = cursor.fetchall()

# Loop through each code and update current value
for row in rows:
    code = row[0]
    ticker = yf.Ticker(code)
    info = ticker.info
    current_price = info.get("currentPrice") or info.get("regularMarketPrice")
    currency = info.get("currency", "USD")

    if current_price:
        cursor.execute("""
            UPDATE stonks
            SET current_price = %s,
                currency = %s,
                last_updated = %s
            WHERE code = %s
        """, (current_price, currency, datetime.now(), code))

conn.commit()
cursor.close()
conn.close()






