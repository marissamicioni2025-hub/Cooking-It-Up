
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "Recipe.h"
#include <vector>
#include <string>
#include <functional>

struct BenchmarkResult {
    std::string algorithm;
    int recipeCount;
    double timeMs;
};

using SimilarityFunction = std::function<double(
    const std::vector<int>&,
    const std::vector<int>&
)>;

double measureSearchTime(
    const std::vector<Recipe>& recipes,
    const std::vector<int>& query,
    SimilarityFunction similarity
);

std::vector<BenchmarkResult> runBenchmarks(
    const std::vector<Recipe>& recipes,
    const std::vector<int>& query,
    SimilarityFunction jaccard,
    SimilarityFunction cosine
);

void displayBenchmarks(
    const std::vector<BenchmarkResult>& results
);

#endif
