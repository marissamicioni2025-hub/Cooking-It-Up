#include <iostream>
#include <cassert>
#include <cmath>
#include "Recipe.h"
#include "Cosine.h"
#include "Ranking.h"

void testCosineKnownExamples() {
    // Query: 4 ingredients [1, 2, 5, 8]
    // Recipe: 9 ingredients [1, 3, 4, 5, 6, 8, 9, 10, 12] -> overlap = 3 (IDs 1, 5, 8)
    // Expected Cosine = 3 / sqrt(4 * 9) = 3 / 6 = 0.5
    std::vector<int> query = {1, 2, 5, 8};
    std::vector<int> recipe = {1, 3, 4, 5, 6, 8, 9, 10, 12};

    int overlap = 0;
    double score = Cosine::computeCosineSimilarity(query, recipe, &overlap);
    assert(overlap == 3);
    assert(std::abs(score - 0.5) < 1e-9);
    std::cout << "[PASS] testCosineKnownExamples\n";
}

void testZeroOverlapAndEmptyInputs() {
    std::vector<int> query = {1, 2, 3};
    std::vector<int> disjointRecipe = {4, 5, 6, 7};
    std::vector<int> emptyVec = {};

    int overlap = -1;
    assert(Cosine::computeCosineSimilarity(query, disjointRecipe, &overlap) == 0.0);
    assert(overlap == 0);

    assert(Cosine::computeCosineSimilarity(emptyVec, disjointRecipe, &overlap) == 0.0);
    assert(overlap == 0);

    assert(Cosine::computeCosineSimilarity(query, emptyVec, &overlap) == 0.0);
    assert(overlap == 0);
    std::cout << "[PASS] testZeroOverlapAndEmptyInputs\n";
}

void testTieBreakingAndTopN() {
    // Create candidates with identical cosine scores (0.5) to test all tie-breakers:
    // Candidate A: ID 30, score 0.5, overlap 2, recipe size 4
    // Candidate B: ID 20, score 0.5, overlap 3, recipe size 9 (Higher overlap -> beats A)
    // Candidate C: ID 10, score 0.5, overlap 2, recipe size 4 (Lower ID -> beats A)
    // Candidate D: ID 40, score 0.8, overlap 4, recipe size 5 (Highest score -> #1)
    std::vector<ScoredRecipe> candidates = {
        {30, "Recipe A", 0.5, 2, 4},
        {20, "Recipe B", 0.5, 3, 9},
        {10, "Recipe C", 0.5, 2, 4},
        {40, "Recipe D", 0.8, 4, 5}
    };

    std::vector<ScoredRecipe> top3 = Ranking::getTopNRecipes(candidates, 3);
    assert(top3.size() == 3);
    assert(top3[0].recipeId == 40); // Highest score (0.8)
    assert(top3[1].recipeId == 20); // Tied 0.5 score, but higher overlap (3 vs 2)
    assert(top3[2].recipeId == 10); // Tied 0.5 score & overlap 2, but lower ID (10 vs 30)

    // Edge cases: N = 0 and N > candidates.size()
    assert(Ranking::getTopNRecipes(candidates, 0).empty());
    assert(Ranking::getTopNRecipes(candidates, 10).size() == 4);
    std::cout << "[PASS] testTieBreakingAndTopN\n";
}

int main() {
    testCosineKnownExamples();
    testZeroOverlapAndEmptyInputs();
    testTieBreakingAndTopN();
    std::cout << "All Artem unit tests passed successfully!\n";
    return 0;
}
