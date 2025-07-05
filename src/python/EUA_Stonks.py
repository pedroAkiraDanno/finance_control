import pandas as pd
import psycopg2
from psycopg2.extras import execute_values

# PostgreSQL connection details
db_params = {
    'host': 'localhost',
    'port': 5432,
    'dbname': 'finances_Dev',
    'user': 'postgres',
    'password': 'p0w2i8'
}

# Download NASDAQ and NYSE stock data
nasdaq_url = "https://www.nasdaqtrader.com/dynamic/SymDir/nasdaqlisted.txt"
other_url = "https://www.nasdaqtrader.com/dynamic/SymDir/otherlisted.txt"

nasdaq = pd.read_csv(nasdaq_url, sep='|')[:-1]  # remove footer
other = pd.read_csv(other_url, sep='|')[:-1]

nasdaq_df = nasdaq[['Symbol', 'Security Name']].rename(columns={'Symbol': 'code', 'Security Name': 'company_name'})
other_df = other[['ACT Symbol', 'Security Name']].rename(columns={'ACT Symbol': 'code', 'Security Name': 'company_name'})

# Combine NASDAQ and other listed
combined_df = pd.concat([nasdaq_df, other_df], ignore_index=True)

# Fill additional columns with default values (you can later update them)
combined_df['country'] = 'USA'
combined_df['current_price'] = None
combined_df['currency'] = 'USD'

# Connect to PostgreSQL and insert data
try:
    conn = psycopg2.connect(**db_params)
    cur = conn.cursor()

    # Prepare the insert query
    insert_query = """
    INSERT INTO stonks (code, company_name, country, current_price, currency)
    VALUES %s
    ON CONFLICT (code) DO NOTHING;
    """

    # Convert DataFrame to list of tuples
    records = list(combined_df[['code', 'company_name', 'country', 'current_price', 'currency']].itertuples(index=False, name=None))

    # Bulk insert
    execute_values(cur, insert_query, records)

    conn.commit()
    print(f"{cur.rowcount} rows inserted.")
    cur.close()
    conn.close()
except Exception as e:
    print("Error:", e)
