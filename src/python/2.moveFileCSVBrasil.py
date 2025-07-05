import os
import shutil

# Define source and destination paths
source_dir = r"C:\Users\Akira\Documents\GitHub\finance_control\src\python\downloads"
destination_dir = r"C:\Users\Akira\Documents\GitHub\finance_control\src\python"

# File names
old_filename = "acoes-listadas-b3.csv"
new_filename = "acoeslistadasb3.csv"

# Full paths
source_path = os.path.join(source_dir, old_filename)
destination_path = os.path.join(destination_dir, new_filename)

# Make sure destination exists
os.makedirs(destination_dir, exist_ok=True)

# Move and rename the file
shutil.move(source_path, destination_path)

print(f"File successfully moved to: {destination_path}")
