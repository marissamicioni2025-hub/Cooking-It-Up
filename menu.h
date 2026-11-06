#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include "Recipe.h"

void displayMenu();

std::vector<std::string> getIngredients();

int getNumberOfResults();

int getSearchMethod();

void displayResults(
    const std::vector<SearchResult>& results,
    double queryTime
);

void displayMissingIngredients(
    const std::vector<int>& recipeIngredients,
    const std::vector<int>& queryIngredients,
    const std::vector<std::string>& vocabulary
);

#endif

