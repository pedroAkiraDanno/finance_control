from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time
import os

# Set download directory
download_dir = os.path.abspath("downloads")

# Chrome options to set download folder
chrome_options = Options()
chrome_options.add_experimental_option("prefs", {
    "download.default_directory": download_dir,
    "download.prompt_for_download": False,
    "directory_upgrade": True,
    "safebrowsing.enabled": True
})

# Initialize WebDriver
driver = webdriver.Chrome(options=chrome_options)

try:
    # Go to the page
    driver.get("https://www.dadosdemercado.com.br/acoes")

    # Wait until the button is visible
    wait = WebDriverWait(driver, 15)
    download_button = wait.until(EC.element_to_be_clickable((By.XPATH, "//span[text()='Baixar em .csv']")))

    # Scroll to the element and click
    driver.execute_script("arguments[0].scrollIntoView(true);", download_button)
    time.sleep(1)  # let scroll finish
    download_button.click()

    print("Download button clicked. Waiting for download...")

    # Wait a bit for download to complete
    time.sleep(5)

finally:
    driver.quit()
