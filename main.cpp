#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <limits>
#include <exception>

#include "Recipe.h"
#include "Cosine.h"
#include "Ranking.h"
#include "menu.h"
#include "benchmark.h"

using namespace std;

int main(int argc, char* argv[]) {
    RecipeDatabase database;

    string filename = "RAW_recipes.csv";

    if (argc > 1) {
        filename = argv[1];
    }

    try {
        cout << "Loading recipe dataset...\n";

        LoadStats stats = database.loadCsv(filename);

        cout << "Dataset loaded successfully!\n";
        cout << "Recipes loaded: " << stats.accepted << "\n";
        cout << "Ingredients in vocabulary: "
             << database.vocabulary().size() << "\n";
        cout << "Load time: " << stats.milliseconds
             << " ms\n";

    } catch (const exception& error) {
        cerr << "Error loading dataset: "
             << error.what() << "\n";
        return 1;
    }

    int choice;

    while (true) {
        displayMenu();

        if (!(cin >> choice)) {
            if (cin.eof()) {
                break;
            }

            cout << "Invalid input. Please try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 0) {
            cout << "\nThank you for using Cooking it up!\n";
            break;
        }

        if (choice != 1) {
            cout << "Invalid choice. Please enter 1 or 0.\n";
            continue;
        }

        string ingredientInput = getIngredients();

        Query query;

        try {
            query = database.makeQuery(ingredientInput);
        } catch (const exception& error) {
            cout << "Invalid ingredients: "
                 << error.what() << "\n";
            continue;
        }

        if (!query.unknown.empty()) {
            cout << "\nIgnoring ingredients not found in dataset: ";

            for (size_t i = 0; i < query.unknown.size(); i++) {
                if (i > 0) {
                    cout << ", ";
                }

                cout << query.unknown[i];
            }

            cout << "\n";
        }

        if (query.ingredientIds.empty()) {
            cout << "No known ingredients to search for.\n";
            continue;
        }

        int topN = getNumberOfResults();

        if (topN == 0) {
            break;
        }

        int method = getSearchMethod();

        if (method == 0) {
            break;
        }

        // Searches and benchmarks use the same recognized ingredient IDs.
        const vector<Recipe>& recipes = database.recipes();

        auto runJaccard = [&]() {
            cout << "\nSearching with Jaccard Similarity...\n";

            auto start = chrono::steady_clock::now();

            vector<ScoredRecipe> scored =
                Jaccard::scoreAllRecipes(
                    recipes,
                    query.ingredientIds
                );

            vector<ScoredRecipe> topResults =
                Ranking::getTopNRecipes(scored, topN);

            auto end = chrono::steady_clock::now();

            double queryTime =
                chrono::duration<double, milli>(
                    end - start
                ).count();

            displayResults(
                topResults,
                recipes,
                query.ingredientIds,
                database.vocabulary(),
                queryTime
            );
        };

        auto runCosine = [&]() {
            cout << "\nSearching with Cosine Similarity...\n";

            auto start = chrono::steady_clock::now();

            vector<ScoredRecipe> scored =
                Cosine::scoreAllRecipes(
                    recipes,
                    query.ingredientIds
                );

            vector<ScoredRecipe> topResults =
                Ranking::getTopNRecipes(scored, topN);

            auto end = chrono::steady_clock::now();

            double queryTime =
                chrono::duration<double, milli>(
                    end - start
                ).count();

            displayResults(
                topResults,
                recipes,
                query.ingredientIds,
                database.vocabulary(),
                queryTime
            );
        };

        if (method == 1) {
            runJaccard();
        } else if (method == 2) {
            runCosine();
        } else if (method == 3) {
            runJaccard();
            runCosine();
        }

        cout << "\nWould you like to run benchmarks? (y/n): ";

        char benchmarkChoice;

        if (!(cin >> benchmarkChoice)) {
            break;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (benchmarkChoice == 'y' ||
            benchmarkChoice == 'Y') {

            SimilarityFunction jaccardFunction =
                [](const vector<int>& a,
                   const vector<int>& b) {
                    return Jaccard::computeJaccardSimilarity(a, b);
                };

            SimilarityFunction cosineFunction =
                [](const vector<int>& a,
                   const vector<int>& b) {
                    return Cosine::computeCosineSimilarity(a, b);
                };

            vector<BenchmarkResult> results =
                runBenchmarks(
                    recipes,
                    query.ingredientIds,
                    jaccardFunction,
                    cosineFunction
                );

            displayBenchmarks(results);
        }
    }

    return 0;
}
