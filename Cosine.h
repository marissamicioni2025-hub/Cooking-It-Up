#ifndef COSINE_H
#define COSINE_H

#include <vector>
#include "Recipe.h"

namespace Cosine {

    // Counts exact ingredient overlap |Q n R| between two sorted, unique ingredient ID lists
    // using a two-pointer scan in O(|Q| + |R|) time.
    int countSortedOverlap(const std::vector<int>& queryIds,
                           const std::vector<int>& recipeIds);

    // Computes binary cosine similarity from scratch:
    // Cosine(Q, R) = |Q n R| / sqrt(|Q| * |R|)
    // Returns 0.0 if either list is empty or if there is zero overlap.
    double computeCosineSimilarity(const std::vector<int>& queryIds,
                                   const std::vector<int>& recipeIds,
                                   int* outOverlapCount = nullptr);

    // Evaluates binary cosine similarity across all recipes in the dataset.
    // By default, only recipes with overlap > 0 are returned for ranking.
    std::vector<ScoredRecipe> scoreAllRecipes(const std::vector<Recipe>& recipes,
                                              const std::vector<int>& queryIds,
                                              bool includeZeroScores = false);

} // namespace Cosine

#endif // COSINE_H
