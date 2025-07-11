










import psycopg2
import requests
from datetime import datetime

# Database configuration
DB_CONFIG = {
    'host': 'localhost',
    'database': 'finances',
    'user': 'postgres',
    'password': 'p0w2i8'
}

def get_exchange_rate():
    """Get current USD to BRL rate from a simple API"""
    try:
        # Using a free API (you might need to get an API key for production)
        response = requests.get('https://economia.awesomeapi.com.br/json/last/USD-BRL')
        data = response.json()
        usd_brl = float(data['USDBRL']['bid'])
        brl_usd = 1 / usd_brl
        return {
            'USD/BRL': round(usd_brl, 6),
            'BRL/USD': round(brl_usd, 6)
        }
    except Exception as e:
        print(f"Error getting exchange rate: {e}")
        return None

def update_rates():
    """Update the exchange rates in the database"""
    rates = get_exchange_rate()
    if not rates:
        print("Failed to get rates")
        return

    conn = None
    try:
        conn = psycopg2.connect(**DB_CONFIG)
        cur = conn.cursor()
        
        # Update both currency pairs
        for pair, rate in rates.items():
            cur.execute("""
                INSERT INTO exchange_rates (currency_pair, rate)
                VALUES (%s, %s)
            """, (pair, rate))
        
        conn.commit()
        print(f"Updated rates at {datetime.now()}:")
        print(f"1 USD = {rates['USD/BRL']} BRL")
        print(f"1 BRL = {rates['BRL/USD']} USD")
        
    except Exception as e:
        print(f"Database error: {e}")
    finally:
        if conn:
            conn.close()

if __name__ == "__main__":
    update_rates()








# SELECT * FROM exchange_rates  ORDER BY last_updated DESC LIMIT 2;












