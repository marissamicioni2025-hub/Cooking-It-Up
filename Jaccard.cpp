#include "Recipe.h"

#include <stdexcept>

// Both ID lists must be sorted and contain no duplicates.
std::size_t ingredientOverlap(
    const std::vector<int>& query,
    const std::vector<int>& recipe
) {
    std::size_t i = 0;
    std::size_t j = 0;
    std::size_t overlap = 0;

    // Each iteration advances at least one pointer: O(q + r).
    while (i < query.size() && j < recipe.size()) {
        if (query[i] == recipe[j]) {
            ++overlap;
            ++i;
            ++j;
        } else if (query[i] < recipe[j]) {
            ++i;
        } else {
            ++j;
        }
    }

    return overlap;
}

double jaccardSimilarity(
    std::size_t overlap,
    std::size_t querySize,
    std::size_t recipeSize
) {
    if (overlap > querySize || overlap > recipeSize) {
        throw std::invalid_argument("Impossible overlap");
    }

    const double unionSize =
        static_cast<double>(querySize) +
        static_cast<double>(recipeSize) -
        static_cast<double>(overlap);

    if (unionSize == 0) {
        return 0.0;
    }

    return static_cast<double>(overlap) / unionSize;
}

namespace Jaccard {

int countSortedOverlap(
    const std::vector<int>& queryIds,
    const std::vector<int>& recipeIds
) {
    return static_cast<int>(
        ingredientOverlap(queryIds, recipeIds)
    );
}

double computeJaccardSimilarity(
    const std::vector<int>& queryIds,
    const std::vector<int>& recipeIds,
    int* outOverlapCount
) {
    const auto overlap = ingredientOverlap(queryIds, recipeIds);

    if (outOverlapCount) {
        *outOverlapCount = static_cast<int>(overlap);
    }

    return jaccardSimilarity(
        overlap,
        queryIds.size(),
        recipeIds.size()
    );
}

double computeJaccardSimilarity(
    const Query& query,
    const std::vector<int>& recipeIds,
    int* outOverlapCount
) {
    const auto overlap = ingredientOverlap(
        query.ingredientIds,
        recipeIds
    );

    if (outOverlapCount) {
        *outOverlapCount = static_cast<int>(overlap);
    }

    // query.size() includes unknown ingredients.
    return jaccardSimilarity(
        overlap,
        query.size(),
        recipeIds.size()
    );
}

std::vector<ScoredRecipe> scoreAllRecipes(
    const std::vector<Recipe>& recipes,
    const std::vector<int>& queryIds,
    bool includeZeroScores
) {
    std::vector<ScoredRecipe> results;

    for (const auto& recipe : recipes) {
        int overlap = 0;

        double score = computeJaccardSimilarity(
            queryIds,
            recipe.ingredientIds,
            &overlap
        );

        if (overlap > 0 || includeZeroScores) {
            results.push_back({
                recipe.id,
                recipe.name,
                score,
                overlap,
                static_cast<int>(recipe.ingredientIds.size())
            });
        }
    }

    return results;
}

std::vector<ScoredRecipe> scoreAllRecipes(
    const std::vector<Recipe>& recipes,
    const Query& query,
    bool includeZeroScores
) {
    std::vector<ScoredRecipe> results;

    for (const auto& recipe : recipes) {
        int overlap = 0;

        double score = computeJaccardSimilarity(
            query,
            recipe.ingredientIds,
            &overlap
        );

        if (overlap > 0 || includeZeroScores) {
            results.push_back({
                recipe.id,
                recipe.name,
                score,
                overlap,
                static_cast<int>(recipe.ingredientIds.size())
            });
        }
    }

    return results;
}

}
