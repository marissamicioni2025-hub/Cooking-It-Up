# sets up the data because the file was so large to put into github
#!/bin/bash
set -e

echo "Setting up Cooking It Up dataset..."

python3 -m pip install kagglehub

python3 - <<'PY'
import kagglehub
import shutil
from pathlib import Path

if Path("RAW_recipes.csv").exists():
    print("Dataset already exists!")
else:
    folder = Path(kagglehub.dataset_download(
        "shuyangli94/food-com-recipes-and-user-interactions"
    ))

    files = list(folder.rglob("RAW_recipes.csv"))

    if not files:
        raise FileNotFoundError("RAW_recipes.csv was not found.")

    shutil.copy2(files[0], "RAW_recipes.csv")
    print("Full recipe dataset downloaded successfully!")
PY

echo "Setup complete!"
