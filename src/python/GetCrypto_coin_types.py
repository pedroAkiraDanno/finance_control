import psycopg2
from pycoingecko import CoinGeckoAPI
import time
from typing import Dict, List, Optional

# Database configuration
DB_CONFIG = {
    'dbname': 'finances_Dev',
    'user': 'postgres',
    'password': 'p0w2i8',
    'host': 'localhost',
    'port': '5432'
}

# Initialize CoinGecko API
cg = CoinGeckoAPI()

# Mapping between CoinGecko categories and your type names
CATEGORY_MAPPING = {
    'stablecoin': 'stablecoin',
    'memes': 'memecoin',
    'smart-contract-platform': 'smart contract',
    'privacy-coins': 'privacy'
    # Add more mappings as needed
}

def get_db_connection():
    """Establish connection to PostgreSQL database"""
    try:
        return psycopg2.connect(**DB_CONFIG)
    except Exception as e:
        print(f"Database connection error: {e}")
        return None

def get_existing_coins() -> List[tuple]:
    """Fetch all coins from crypto_coins table"""
    conn = get_db_connection()
    if not conn:
        return []
    
    try:
        with conn.cursor() as cur:
            cur.execute("SELECT coin_id, name, symbol FROM investment.crypto_coins")
            return cur.fetchall()
    except Exception as e:
        print(f"Error fetching coins: {e}")
        return []
    finally:
        if conn:
            conn.close()

def get_existing_types() -> Dict[str, int]:
    """Fetch existing types from coin_types table and return as name->id mapping"""
    conn = get_db_connection()
    if not conn:
        return {}
    
    try:
        with conn.cursor() as cur:
            cur.execute("SELECT type_id, type_name FROM investment.coin_types")
            return {name: type_id for type_id, name in cur.fetchall()}
    except Exception as e:
        print(f"Error fetching coin types: {e}")
        return {}
    finally:
        if conn:
            conn.close()

def fetch_coin_categories(coin_name: str) -> List[str]:
    """Fetch categories for a coin from CoinGecko API"""
    try:
        time.sleep(1)  # Rate limiting
        coin_data = cg.get_coin_by_id(id=coin_name.lower().replace(' ', '-'))
        return coin_data.get('categories', [])
    except Exception as e:
        print(f"Error fetching categories for {coin_name}: {e}")
        return []

def map_categories_to_types(categories: List[str], type_mapping: Dict[str, int]) -> List[int]:
    """Map CoinGecko categories to your type IDs"""
    type_ids = []
    for category in categories:
        category_lower = category.lower()
        # Check direct matches
        if category_lower in type_mapping:
            type_ids.append(type_mapping[category_lower])
            continue
        # Check partial matches
        for cg_category, type_name in CATEGORY_MAPPING.items():
            if cg_category in category_lower:
                if type_name in type_mapping:
                    type_ids.append(type_mapping[type_name])
    return list(set(type_ids))  # Remove duplicates

def insert_coin_types(coin_id: int, type_ids: List[int]) -> bool:
    """Insert coin-type relationships into crypto_coin_types"""
    if not type_ids:
        return False
    
    conn = get_db_connection()
    if not conn:
        return False
    
    try:
        with conn.cursor() as cur:
            # Prepare values for batch insert
            values = [(coin_id, type_id) for type_id in type_ids]
            
            # Use execute_values for batch insert
            from psycopg2.extras import execute_values
            execute_values(
                cur,
                """INSERT INTO investment.crypto_coin_types (coin_id, type_id)
                   VALUES %s
                   ON CONFLICT (coin_id, type_id) DO NOTHING""",
                values
            )
            conn.commit()
            return True
    except Exception as e:
        conn.rollback()
        print(f"Error inserting types for coin_id {coin_id}: {e}")
        return False
    finally:
        if conn:
            conn.close()

def main():
    # Get existing data
    coins = get_existing_coins()
    if not coins:
        print("No coins found in database")
        return
    
    type_mapping = get_existing_types()
    if not type_mapping:
        print("No coin types found in database")
        return
    
    print(f"Found {len(coins)} coins and {len(type_mapping)} types to process")
    
    success_count = 0
    for coin_id, name, symbol in coins:
        print(f"\nProcessing {name} ({symbol}, ID: {coin_id})...")
        
        # Try with name first, then symbol as fallback
        categories = fetch_coin_categories(name) or fetch_coin_categories(symbol)
        if not categories:
            print(f"No categories found for {name}")
            continue
        
        # Map categories to our type IDs
        type_ids = map_categories_to_types(categories, type_mapping)
        if not type_ids:
            print(f"No matching types found for categories: {categories}")
            continue
        
        # Insert relationships
        if insert_coin_types(coin_id, type_ids):
            success_count += 1
            print(f"Added types {type_ids} to coin {name}")
        else:
            print(f"Failed to add types to {name}")
    
    print(f"\nCompleted: Processed types for {success_count} of {len(coins)} coins")

if __name__ == "__main__":
    main()