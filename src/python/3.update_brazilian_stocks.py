import csv
import psycopg2
from psycopg2 import sql
from datetime import datetime

# Database connection parameters
DB_PARAMS = {
    'host': 'localhost',
    'database': 'finances_Dev',
    'user': 'postgres',
    'password': 'p0w2i8'
}

# CSV file path
CSV_FILE = 'acoeslistadasb3.csv'

def load_stocks_to_db():
    try:
        # Connect to PostgreSQL
        conn = psycopg2.connect(**DB_PARAMS)
        cursor = conn.cursor()
        
        # Read CSV and prepare data
        stocks_data = []
        with open(CSV_FILE, mode='r', encoding='utf-8') as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                ticker = f"{row['Ticker'].strip()}.SA"  # Add .SA suffix
                name = row['Nome'].strip()
                # Convert Brazilian price format (9,85) to numeric (9.85)
                price = float(row['Última (R$)'].replace('.', '').replace(',', '.'))
                stocks_data.append((ticker, name, price))
        
        # Insert data into PostgreSQL
        insert_query = sql.SQL("""
            INSERT INTO stonks (code, company_name, country, current_price, currency, last_updated) 
            VALUES (%s, %s, 'Brazil', %s, 'BRL', %s)
            ON CONFLICT (code) DO UPDATE SET
                company_name = EXCLUDED.company_name,
                current_price = EXCLUDED.current_price,
                last_updated = EXCLUDED.last_updated
        """)
        
        current_time = datetime.now()
        for ticker, name, price in stocks_data:
            cursor.execute(insert_query, (ticker, name, price, current_time))
        
        conn.commit()
        print(f"Successfully processed {len(stocks_data)} stocks")
        
    except Exception as e:
        print(f"Error: {e}")
        if conn:
            conn.rollback()
    finally:
        if cursor:
            cursor.close()
        if conn:
            conn.close()

if __name__ == "__main__":
    load_stocks_to_db()










