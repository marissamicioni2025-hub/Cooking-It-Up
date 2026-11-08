#ifndef RECIPE_H
#define RECIPE_H

#include <cstddef>
#include <istream>
#include <string>
#include <unordered_map>
#include <vector>

// Ingredient IDs are sorted in ascending order without duplicates.
struct Recipe {
    int id;
    std::string name;
    std::vector<int> ingredientIds;
};

struct ScoredRecipe {
    int recipeId;
    std::string name;
    double score;
    int overlapCount;
    int recipeIngredientCount;
};

// Shared data-loading helpers.
std::string normalizeIngredient(const std::string& text);

bool readCsvRecord(
    std::istream& input,
    std::vector<std::string>& fields
);

std::vector<std::string> parseIngredientList(
    const std::string& literal
);

struct Query {
    std::vector<int> ingredientIds;
    std::vector<std::string> tokens;
    std::vector<std::string> unknown;

    // Includes unknown ingredients in the query cardinality.
    std::size_t size() const {
        return tokens.size();
    }
};

struct LoadStats {
    std::size_t recordsRead = 0;
    std::size_t accepted = 0;
    std::size_t rejected = 0;
    std::size_t duplicates = 0;
    double milliseconds = 0;
};

class RecipeDatabase {
public:
    // A limit of zero loads all usable recipes.
    LoadStats loadCsv(
        const std::string& path,
        std::size_t limit = 0
    );

    Query makeQuery(const std::string& commaSeparated) const;

    const std::vector<Recipe>& recipes() const {
        return recipes_;
    }

    const std::vector<std::string>& vocabulary() const {
        return vocabulary_;
    }

    // Logical payload estimate; excludes allocation/container overhead.
    std::size_t payloadBytes() const;

private:
    std::vector<Recipe> recipes_;
    std::vector<std::string> vocabulary_;
    std::unordered_map<std::string, int> ingredientToId_;
};

// All ingredient ID lists must be sorted, unique, and use the same vocabulary.
namespace Jaccard {

int countSortedOverlap(
    const std::vector<int>& queryIds,
    const std::vector<int>& recipeIds
);

double computeJaccardSimilarity(
    const std::vector<int>& queryIds,
    const std::vector<int>& recipeIds,
    int* outOverlapCount = nullptr
);

// This overload includes unknown query ingredients in the denominator.
double computeJaccardSimilarity(
    const Query& query,
    const std::vector<int>& recipeIds,
    int* outOverlapCount = nullptr
);

std::vector<ScoredRecipe> scoreAllRecipes(
    const std::vector<Recipe>& recipes,
    const std::vector<int>& queryIds,
    bool includeZeroScores = false
);

std::vector<ScoredRecipe> scoreAllRecipes(
    const std::vector<Recipe>& recipes,
    const Query& query,
    bool includeZeroScores = false
);

}

#endif
