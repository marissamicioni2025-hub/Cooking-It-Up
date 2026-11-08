#include "menu.h"
#include <iostream>
#include <sstream>
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

vector<string> getIngredients() {
    vector<string> ingredients;
    string input;

    cout << "\nEnter ingredients separated by commas: ";
    getline(cin, input);

    stringstream ss(input);
    string ingredient;

    while (getline(ss, ingredient, ',')) {
        size_t start = ingredient.find_first_not_of(" \t");

        if (start == string::npos) {
            continue;
        }

        size_t end = ingredient.find_last_not_of(" \t");
        ingredient = ingredient.substr(start, end - start + 1);

        for (char& c : ingredient) {
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
        }

        if (find(ingredients.begin(), ingredients.end(),
                 ingredient) == ingredients.end()) {
            ingredients.push_back(ingredient);
        }
    }

    return ingredients;
}

int getNumberOfResults() {
    int n;

    while (true) {
        cout << "\nHow many recipes would you like to see? ";

        if (cin >> n && n > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return n;
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

        cout << "Invalid choice. Please enter 1, 2, or 3.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void displayResults(
    const vector<SearchResult>& results,
    double queryTime
) {
    cout << "\n========== RECIPE RESULTS ==========\n";

    if (results.empty()) {
        cout << "No matching recipes found.\n";
    } else {
        for (size_t i = 0; i < results.size(); i++) {
            cout << "\n" << i + 1 << ". "
                 << results[i].recipe.name << "\n";

            cout << "Similarity Score: "
                 << fixed << setprecision(4)
                 << results[i].score << "\n";
        }
    }

    cout << "\nQuery Time: "
         << fixed << setprecision(3)
         << queryTime << " ms\n";

    cout << "====================================\n";
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

