#include "benchmark.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;

double measureSearchTime(
    const vector<Recipe>& recipes,
    const vector<int>& query,
    SimilarityFunction similarity
) {
    volatile double totalScore = 0.0;

    auto start = chrono::steady_clock::now();

    for (const Recipe& recipe : recipes) {
        totalScore += similarity(
            query,
            recipe.ingredientIds
        );
    }

    auto end = chrono::steady_clock::now();

    chrono::duration<double, milli> elapsed = end - start;

    return elapsed.count();
}

vector<BenchmarkResult> runBenchmarks(
    const vector<Recipe>& recipes,
    const vector<int>& query,
    SimilarityFunction jaccard,
    SimilarityFunction cosine
) {
    vector<BenchmarkResult> results;

    vector<int> sizes = {1000, 10000, 50000, 100000};

    for (int size : sizes) {
        int count = min(size, static_cast<int>(recipes.size()));

        vector<Recipe> subset(
            recipes.begin(),
            recipes.begin() + count
        );

        double jaccardTime = measureSearchTime(
            subset, query, jaccard
        );

        double cosineTime = measureSearchTime(
            subset, query, cosine
        );

        results.push_back({
            "Jaccard",
            count,
            jaccardTime
        });

        results.push_back({
            "Cosine",
            count,
            cosineTime
        });

        if (count == static_cast<int>(recipes.size())) {
            break;
        }
    }

    return results;
}

void displayBenchmarks(
    const vector<BenchmarkResult>& results
) {
    cout << "\n========== BENCHMARK RESULTS ==========\n";

    cout << left << setw(15) << "Algorithm"
         << setw(15) << "Recipes"
         << setw(15) << "Time (ms)" << "\n";

    cout << "---------------------------------------\n";

    for (const BenchmarkResult& result : results) {
        cout << left << setw(15) << result.algorithm
             << setw(15) << result.recipeCount
             << fixed << setprecision(3)
             << result.timeMs << "\n";
    }

    cout << "=======================================\n";
}
