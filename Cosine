#include "Cosine.h"
#include <cmath>

namespace Cosine {

    int countSortedOverlap(const std::vector<int>& queryIds,
                           const std::vector<int>& recipeIds) {
        int overlap = 0;
        size_t i = 0;
        size_t j = 0;

        while (i < queryIds.size() && j < recipeIds.size()) {
            if (queryIds[i] == recipeIds[j]) {
                ++overlap;
                ++i;
                ++j;
            } else if (queryIds[i] < recipeIds[j]) {
                ++i;
            } else {
                ++j;
            }
        }
        return overlap;
    }

    double computeCosineSimilarity(const std::vector<int>& queryIds,
                                   const std::vector<int>& recipeIds,
                                   int* outOverlapCount) {
        if (queryIds.empty() || recipeIds.empty()) {
            if (outOverlapCount) *outOverlapCount = 0;
            return 0.0;
        }

        int overlap = countSortedOverlap(queryIds, recipeIds);
        if (outOverlapCount) {
            *outOverlapCount = overlap;
        }

        if (overlap == 0) {
            return 0.0;
        }

        double denominator = std::sqrt(
            static_cast<double>(queryIds.size()) *
            static_cast<double>(recipeIds.size())
        );

        return (denominator > 0.0) ? (static_cast<double>(overlap) / denominator) : 0.0;
    }

    std::vector<ScoredRecipe> scoreAllRecipes(const std::vector<Recipe>& recipes,
                                              const std::vector<int>& queryIds,
                                              bool includeZeroScores) {
        std::vector<ScoredRecipe> results;
        if (queryIds.empty()) {
            return results;
        }

        results.reserve(recipes.size() / 4);

        for (const Recipe& recipe : recipes) {
            int overlap = 0;
            double score = computeCosineSimilarity(queryIds, recipe.ingredientIds, &overlap);

            if (overlap > 0 || includeZeroScores) {
                ScoredRecipe sr;
                sr.recipeId = recipe.id;
                sr.name = recipe.name;
                sr.score = score;
                sr.overlapCount = overlap;
                sr.recipeIngredientCount = static_cast<int>(recipe.ingredientIds.size());
                results.push_back(sr);
            }
        }
        return results;
    }

} // namespace Cosine
