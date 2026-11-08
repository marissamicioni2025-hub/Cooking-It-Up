#ifndef RANKING_H
#define RANKING_H

#include <vector>
#include "Recipe.h"

namespace Ranking {

    // Deterministic comparator: returns true if recipe 'a' should be ranked
    // strictly ahead of recipe 'b'.
    bool isHigherRank(const ScoredRecipe& a, const ScoredRecipe& b);

    // Sorts a vector of ScoredRecipe in-place from highest to lowest rank
    // using a custom Merge Sort implementation (O(M log M)).
    void rankAllRecipes(std::vector<ScoredRecipe>& scoredRecipes);

    // Returns the top N highest-ranking recipes using a custom Min-Heap of size N
    // in O(M log N) time, followed by custom Merge Sort on the N winners.
    std::vector<ScoredRecipe> getTopNRecipes(const std::vector<ScoredRecipe>& scoredRecipes,
                                             int topN);

} // namespace Ranking

#endif // RANKING_H
