#ifndef RECIPE_H
#define RECIPE_H

#include <string>
#include <vector>

// Represents a cleaned recipe from the Food.com dataset.
// ingredientIds must be stored in sorted ascending order with no duplicates.
struct Recipe {
    int id;
    std::string name;
    std::vector<int> ingredientIds;
};

// Stores similarity evaluation output for ranking and UI display.
struct ScoredRecipe {
    int recipeId;
    std::string name;
    double score;               // Jaccard or Cosine similarity score in [0.0, 1.0]
    int overlapCount;           // Number of matched ingredients |Q n R|
    int recipeIngredientCount;  // Total ingredients in the recipe |R|
};

#endif // RECIPE_H
