#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "Recipe.h"

void displayMenu();

std::string getIngredients();

int getNumberOfResults();

int getSearchMethod();

void displayResults(
    const std::vector<ScoredRecipe>& results,
    const std::vector<Recipe>& recipes,
    const std::vector<int>& queryIngredients,
    const std::vector<std::string>& vocabulary,
    double queryTime
);

void displayMissingIngredients(
    const std::vector<int>& recipeIngredients,
    const std::vector<int>& queryIngredients,
    const std::vector<std::string>& vocabulary
);

#endif
