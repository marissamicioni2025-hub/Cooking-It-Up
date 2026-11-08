#include "menu.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <iomanip>

using namespace std;

void displayMenu() {
    cout << "\n========== COOKING IT UP ==========\n";
    cout << "Find recipes using ingredients you have!\n";
    cout << "-----------------------------------\n";
    cout << "1. Find recipes\n";
    cout << "0. Exit\n";
    cout << "-----------------------------------\n";
    cout << "Enter your choice: ";
}

string getIngredients() {
    string input;

    cout << "\nEnter ingredients separated by commas: ";
    getline(cin, input);

    return input;
}

int getNumberOfResults() {
    int n;

    while (true) {
        cout << "\nHow many recipes would you like to see? ";

        if (cin >> n && n > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return n;
        }

        if (cin.eof()) {
            return 0;
        }

        cout << "Invalid input. Enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int getSearchMethod() {
    int method;

    while (true) {
        cout << "\nChoose a similarity method:\n";
        cout << "1. Jaccard Similarity\n";
        cout << "2. Cosine Similarity\n";
        cout << "3. Both Methods\n";
        cout << "Enter your choice: ";

        if (cin >> method && method >= 1 && method <= 3) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return method;
        }

        if (cin.eof()) {
            return 0;
        }

        cout << "Invalid choice. Please enter 1, 2, or 3.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void displayMissingIngredients(
    const vector<int>& recipeIngredients,
    const vector<int>& queryIngredients,
    const vector<string>& vocabulary
) {
    cout << "Missing ingredients: ";

    bool missing = false;

    for (int id : recipeIngredients) {
        if (find(queryIngredients.begin(),
                 queryIngredients.end(), id)
            == queryIngredients.end()) {

            if (id >= 0 &&
                static_cast<size_t>(id) < vocabulary.size()) {

                if (missing) {
                    cout << ", ";
                }

                cout << vocabulary[id];
                missing = true;
            }
        }
    }

    if (!missing) {
        cout << "None!";
    }

    cout << "\n";
}

void displayResults(
    const vector<ScoredRecipe>& results,
    const vector<Recipe>& recipes,
    const vector<int>& queryIngredients,
    const vector<string>& vocabulary,
    double queryTime
) {
    cout << "\n========== RECIPE RESULTS ==========\n";

    if (results.empty()) {
        cout << "No matching recipes found.\n";
    } else {
        for (size_t i = 0; i < results.size(); i++) {
            const ScoredRecipe& result = results[i];

            cout << "\n" << i + 1 << ". "
                 << result.name << "\n";

            cout << "Similarity Score: "
                 << fixed << setprecision(4)
                 << result.score << "\n";

            cout << "Matched Ingredients: "
                 << result.overlapCount << "\n";

            for (const Recipe& recipe : recipes) {
                if (recipe.id == result.recipeId) {
                    displayMissingIngredients(
                        recipe.ingredientIds,
                        queryIngredients,
                        vocabulary
                    );
                    break;
                }
            }
        }
    }

    cout << "\nQuery Time: "
         << fixed << setprecision(3)
         << queryTime << " ms\n";

    cout << "====================================\n";
}
