#ifndef RECIPE_H
#define RECIPE_H

#include <string>
#include <vector>

struct Recipe {
    int id;
    std::string name;
    std::vector<int> ingredients;
};

struct SearchResult {
    Recipe recipe;
    double score;
};

#endif
