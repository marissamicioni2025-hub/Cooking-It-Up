# Cooking It Up

**Team Name:** CodeMonks

**Team Members:**
- Artem Antonian
- Nathan Chin-Lue
- Marissa Micioni

## Purpose

Cooking It Up is a C++ recipe recommendation program designed to help users find meals based on ingredients they already have available. Instead of searching through recipes individually, users can enter their ingredients and receive recommendations ranked by how closely they match.

The purpose of this project is to compare two similarity algorithms, **Jaccard Similarity and Cosine Similarity**, which we implemented from scratch. Using the Food.com dataset, our program calculates similarity scores, ranks recipes, identifies missing ingredients, and compares the execution times of both algorithms.

This project allows us to apply concepts learned in COP3530, including data structures, algorithm implementation, and performance analysis, to a real-world problem.

## How to Run

**1. Clone the GitHub repository**

```bash
git clone https://github.com/marissamicioni2025-hub/Cooking-It-Up.git
cd Cooking-It-Up
```

**2. Download the recipe dataset**

```bash
bash setup_data.sh
```

This downloads the Food.com dataset (`RAW_recipes.csv`) into the project folder. Python 3, pip, and an internet connection are required. Kaggle authentication may also be needed.

**3. Compile the program**

```bash
g++ -std=c++17 main.cpp menu.cpp benchmark.cpp DataLoader.cpp Jaccard.cpp Cosine.cpp Ranking.cpp -o cooking_it_up
```

**4. Run the program**

```bash
./cooking_it_up RAW_recipes.csv
```

**5. Using the program**

Once the dataset loads, follow the menu prompts to:

1. Enter available ingredients separated by commas (example: `chicken, rice, salt`).
2. Choose the number of recipe recommendations.
3. Select Jaccard Similarity, Cosine Similarity, or both.
4. View the recommended recipes, similarity scores, and missing ingredients.
5. Run benchmarks to compare the execution times of the algorithms.

**Dataset Source:** https://www.kaggle.com/datasets/shuyangli94/food-com-recipes-and-user-interactions
