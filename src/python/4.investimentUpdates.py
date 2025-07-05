import yfinance as yf
import psycopg2
from datetime import datetime
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('stock_updater.log'),
        logging.StreamHandler()
    ]
)

logger = logging.getLogger(__name__)

def update_stock_prices():
    try:
        # Database connection
        logger.info("Connecting to PostgreSQL database...")
        conn = psycopg2.connect(
            dbname="finances_Dev",
            user="postgres",
            password="p0w2i8",
            host="localhost",
            port="5432"
        )
        cursor = conn.cursor()

        # Get all codes from the table
        logger.info("Fetching stock codes from database...")
        cursor.execute("SELECT code FROM stonks")
        rows = cursor.fetchall()
        total_stocks = len(rows)
        logger.info(f"Found {total_stocks} stocks to update")

        updated_count = 0
        failed_count = 0
        skipped_count = 0

        # Loop through each code and update current value
        for i, row in enumerate(rows, 1):
            code = row[0]
            logger.info(f"Processing stock {i}/{total_stocks}: {code}")

            try:
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
                    logger.info(f"Updated {code}: Price={current_price} {currency}")
                    updated_count += 1
                else:
                    logger.warning(f"No price available for {code}, skipping...")
                    skipped_count += 1

            except Exception as e:
                logger.error(f"Error processing {code}: {str(e)}")
                failed_count += 1
                conn.rollback()  # Rollback in case of error

        # Summary log
        logger.info("\nUpdate Summary:")
        logger.info(f"Total stocks processed: {total_stocks}")
        logger.info(f"Successfully updated: {updated_count}")
        logger.info(f"Skipped (no price): {skipped_count}")
        logger.info(f"Failed updates: {failed_count}")

    except psycopg2.Error as e:
        logger.error(f"Database error: {e}")
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
    finally:
        if 'conn' in locals():
            cursor.close()
            conn.close()
            logger.info("Database connection closed")

if __name__ == "__main__":
    logger.info("Starting stock price update process")
    update_stock_prices()
    logger.info("Update process completed")