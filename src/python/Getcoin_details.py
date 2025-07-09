import psycopg2
from pycoingecko import CoinGeckoAPI
from datetime import datetime
import time

# Database connection
DB_CONFIG = {
    'dbname': 'finances_Dev',
    'user': 'postgres',
    'password': 'p0w2i8',
    'host': 'localhost',
    'port': '5432'
}

# Initialize CoinGecko API
cg = CoinGeckoAPI()

def get_db_connection():
    """Establish PostgreSQL connection"""
    try:
        return psycopg2.connect(**DB_CONFIG)
    except Exception as e:
        print(f"Database connection error: {e}")
        return None

def get_existing_coins():
    """Fetch coins from crypto_coins table with their names"""
    conn = get_db_connection()
    if not conn:
        return None
    
    try:
        with conn.cursor() as cur:
            cur.execute("""
                SELECT coin_id, symbol, name 
                FROM investment.crypto_coins
                ORDER BY coin_id
            """)
            return cur.fetchall()
    except Exception as e:
        print(f"Error fetching coins: {e}")
        return None
    finally:
        if conn:
            conn.close()

def fetch_coin_details(coin_name):
    """Fetch details from CoinGecko using coin name"""
    try:
        time.sleep(1)  # Rate limiting
        return cg.get_coin_by_id(id=coin_name.lower().replace(" ", "-"))
    except Exception as e:
        print(f"API error for {coin_name}: {e}")
        return None

def prepare_details_data(coin_id, cg_data):
    """Prepare data for insertion into coin_details"""
    if not cg_data:
        return None
    
    try:
        details = {
            'coin_id': coin_id,
            'full_name': cg_data.get('name', ''),
            'descriptionInfo': cg_data.get('description', {}).get('en', ''),
            'market_cap_rank': cg_data.get('market_cap_rank'),
            'is_active': not cg_data.get('developer_data', {}).get('abandoned', False),
            'website_url': next(iter(cg_data.get('links', {}).get('homepage', [])), ''),
            'whitepaper_url': cg_data.get('links', {}).get('whitepaper', ''),
            'github_url': next((url for url in cg_data.get('links', {}).get('repos_url', {}).get('github', []) if url), ''),
            'twitter_url': f"https://twitter.com/{cg_data.get('links', {}).get('twitter_screen_name', '')}" if cg_data.get('links', {}).get('twitter_screen_name') else '',
            'blockchain_explorer_url': next((url for url in cg_data.get('links', {}).get('blockchain_site', []) if url), '')
        }

        # Handle genesis date
        if cg_data.get('genesis_date'):
            try:
                details['launched_year'] = datetime.strptime(cg_data['genesis_date'], '%Y-%m-%d').year
            except:
                pass

        return details
    except Exception as e:
        print(f"Error preparing data: {e}")
        return None

def upsert_coin_details(details):
    """Insert or update coin_details table"""
    if not details:
        return False
    
    conn = get_db_connection()
    if not conn:
        return False
    
    try:
        with conn.cursor() as cur:
            cur.execute("""
                INSERT INTO investment.coin_details (
                    coin_id, full_name, descriptionInfo, launched_year,
                    market_cap_rank, is_active, website_url, whitepaper_url,
                    github_url, twitter_url, blockchain_explorer_url
                ) VALUES (
                    %(coin_id)s, %(full_name)s, %(descriptionInfo)s, %(launched_year)s,
                    %(market_cap_rank)s, %(is_active)s, %(website_url)s, %(whitepaper_url)s,
                    %(github_url)s, %(twitter_url)s, %(blockchain_explorer_url)s
                )
                ON CONFLICT (coin_id) DO UPDATE SET
                    full_name = EXCLUDED.full_name,
                    descriptionInfo = EXCLUDED.descriptionInfo,
                    launched_year = EXCLUDED.launched_year,
                    market_cap_rank = EXCLUDED.market_cap_rank,
                    is_active = EXCLUDED.is_active,
                    website_url = EXCLUDED.website_url,
                    whitepaper_url = EXCLUDED.whitepaper_url,
                    github_url = EXCLUDED.github_url,
                    twitter_url = EXCLUDED.twitter_url,
                    blockchain_explorer_url = EXCLUDED.blockchain_explorer_url
            """, details)
            conn.commit()
            return True
    except Exception as e:
        conn.rollback()
        print(f"Database error for coin_id {details.get('coin_id')}: {e}")
        return False
    finally:
        if conn:
            conn.close()

def main():
    coins = get_existing_coins()
    if not coins:
        print("No coins found in database")
        return
    
    print(f"Found {len(coins)} coins to process")
    
    success_count = 0
    for coin_id, symbol, name in coins:
        print(f"\nProcessing {name} ({symbol}, ID: {coin_id})...")
        
        # Try with name first, then with symbol as fallback
        cg_data = fetch_coin_details(name) or fetch_coin_details(symbol)
        
        if not cg_data:
            print(f"Could not fetch data for {name} ({symbol})")
            continue
        
        details = prepare_details_data(coin_id, cg_data)
        if not details:
            print(f"Could not prepare data for {name}")
            continue
        
        if upsert_coin_details(details):
            success_count += 1
            print(f"Successfully updated details for {name}")
        else:
            print(f"Failed to update details for {name}")
    
    print(f"\nCompleted: Updated {success_count} of {len(coins)} coins")

if __name__ == "__main__":
    main()




# pip install psycopg2-binary pycoingecko

