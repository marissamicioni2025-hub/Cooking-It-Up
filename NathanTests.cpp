#include "Recipe.h"

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>

namespace {

int checks = 0;

void check(bool condition, const std::string& message) {
    ++checks;

    if (!condition) {
        throw std::runtime_error(message);
    }
}

void checkScore(
    double actual,
    double expected,
    const std::string& message
) {
    check(std::abs(actual - expected) < 1e-12, message);
}

template <typename Function>
void checkThrows(Function function, const std::string& message) {
    bool threw = false;

    try {
        function();
    } catch (const std::exception&) {
        threw = true;
    }

    check(threw, message);
}

}

int main() {
    const auto stamp = std::chrono::steady_clock::now()
                           .time_since_epoch().count();

    const auto path = std::filesystem::temp_directory_path() /
        ("nathan-tests-" + std::to_string(stamp) + ".csv");

    try {
        check(
            normalizeIngredient("  OLIVE\t OIL \r\n") == "olive oil",
            "Normalize ingredient case and whitespace"
        );

        check(
            parseIngredientList("[' RICE ', 'rice', 'salt', '']") ==
                std::vector<std::string>({"rice", "salt"}),
            "Normalize and deduplicate ingredient lists"
        );

        checkThrows(
            [] {
                parseIngredientList("['rice'");
            },
            "Reject malformed ingredient lists"
        );

        // Two valid recipes, one duplicate ID, and four invalid records.
        {
            std::ofstream file(path, std::ios::binary);

            file << "name,id,ingredients\r\n"
                 << "\"Chicken, rice\",1,"
                    "\"[' CHICKEN ', 'rice', 'salt', 'rice']\"\r\n"
                 << "\"Rice \"\"bowl\"\"\r\nsecond line\",2,"
                    "\"['rice', 'water']\"\r\n"
                 << "Duplicate,1,\"['butter']\"\r\n"
                 << "Empty ingredients,3,[]\r\n"
                 << "Bad list,4,notalist\r\n"
                 << ",5,\"['rice']\"\r\n"
                 << "Bad ID,notanid,\"['rice']\"\r\n";

            check(
                static_cast<bool>(file),
                "Create temporary CSV fixture"
            );
        }

        RecipeDatabase db;
        const auto stats = db.loadCsv(path.string());

        check(stats.recordsRead == 7, "Count CSV records");
        check(stats.accepted == 2, "Count accepted recipes");
        check(stats.rejected == 4, "Count rejected recipes");
        check(stats.duplicates == 1, "Reject duplicate recipe IDs");
        check(db.vocabulary().size() == 4, "Build shared vocabulary");

        check(
            db.recipes()[0].name == "Chicken, rice",
            "Parse quoted comma"
        );

        check(
            db.recipes()[1].name == "Rice \"bowl\"\r\nsecond line",
            "Parse escaped quotes and multiline fields"
        );

        const auto query = db.makeQuery(
            "CHICKEN, rice, salt, rice"
        );

        const auto unknown = db.makeQuery(
            "chicken, rice, salt, unobtainium"
        );

        check(
            query.size() == 3 && query.ingredientIds.size() == 3,
            "Normalize and deduplicate query"
        );

        check(
            unknown.size() == 4 && unknown.unknown.size() == 1,
            "Retain unknown query ingredients"
        );

        check(
            db.vocabulary().size() == 4,
            "Queries preserve vocabulary"
        );

        checkThrows(
            [&] {
                db.makeQuery(" , , ");
            },
            "Reject empty query"
        );

        const std::vector<int> a{1, 2, 3};
        const std::vector<int> b{2, 3, 4, 5};
        const std::vector<int> disjoint{10, 11};
        const std::vector<int> empty;

        int overlap = -1;

        check(
            Jaccard::countSortedOverlap(a, b) == 2,
            "Count sorted overlap"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(a, b, &overlap),
            0.4,
            "Partial match: 2 shared / 5 total"
        );

        check(
            overlap == 2,
            "Return overlap through output pointer"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(a, a),
            1.0,
            "Identical sets score one"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(a, disjoint),
            0.0,
            "Disjoint sets score zero"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(empty, empty),
            0.0,
            "Defined empty-set score"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(
                query,
                db.recipes()[0].ingredientIds
            ),
            1.0,
            "Exact database query match"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(
                unknown,
                db.recipes()[0].ingredientIds
            ),
            0.75,
            "Include unknown ingredient in denominator"
        );

        const auto scores = Jaccard::scoreAllRecipes(
            db.recipes(),
            query
        );

        check(
            scores.size() == 2,
            "Score positive-overlap recipes"
        );

        check(
            scores[0].recipeId == 1 &&
            scores[0].overlapCount == 3 &&
            scores[0].recipeIngredientCount == 3,
            "Populate scored fields"
        );

        checkScore(scores[0].score, 1.0, "First batch score");
        checkScore(scores[1].score, 0.25, "Second batch score");

        const auto noMatch = db.makeQuery("unobtainium");

        check(
            Jaccard::scoreAllRecipes(db.recipes(), noMatch).empty(),
            "Exclude zero-overlap results by default"
        );

        check(
            Jaccard::scoreAllRecipes(
                db.recipes(),
                noMatch,
                true
            ).size() == 2,
            "Include zero-score results when requested"
        );

        // Independent reference uses set membership, not two pointers.
        std::mt19937 random(3530);

        for (int trial = 0; trial < 1000; ++trial) {
            std::set<int> querySet;
            std::set<int> recipeSet;

            for (int id = 0; id < 40; ++id) {
                if (random() % 2) {
                    querySet.insert(id);
                }

                if (random() % 3 == 0) {
                    recipeSet.insert(id);
                }
            }

            int expectedOverlap = 0;

            for (int id : querySet) {
                expectedOverlap += static_cast<int>(
                    recipeSet.count(id)
                );
            }

            const auto unionSize =
                querySet.size() + recipeSet.size() - expectedOverlap;

            const double expected = unionSize == 0
                ? 0.0
                : static_cast<double>(expectedOverlap) / unionSize;

            const std::vector<int> queryIds(
                querySet.begin(),
                querySet.end()
            );

            const std::vector<int> recipeIds(
                recipeSet.begin(),
                recipeSet.end()
            );

            int actualOverlap = -1;

            const double actual = Jaccard::computeJaccardSimilarity(
                queryIds,
                recipeIds,
                &actualOverlap
            );

            check(
                actualOverlap == expectedOverlap,
                "Random overlap check"
            );

            checkScore(actual, expected, "Random Jaccard check");
        }

        // Failed reloads must preserve the previously loaded database.
        {
            std::ofstream file(path, std::ios::binary);
            file << "name,id,ingredients\n\"unterminated";

            check(
                static_cast<bool>(file),
                "Create malformed CSV fixture"
            );
        }

        checkThrows(
            [&] {
                db.loadCsv(path.string());
            },
            "Reject unterminated CSV quoting"
        );

        check(
            db.recipes().size() == 2 &&
            db.vocabulary().size() == 4,
            "Preserve database after failed reload"
        );

        checkScore(
            Jaccard::computeJaccardSimilarity(
                query,
                db.recipes()[0].ingredientIds
            ),
            1.0,
            "Preserve ingredient IDs after failed reload"
        );

        std::filesystem::remove(path);

        std::cout << "All " << checks
                  << " Nathan checks passed.\n";

        return 0;
    } catch (const std::exception& error) {
        std::error_code ignored;
        std::filesystem::remove(path, ignored);

        std::cerr << "TEST FAILED: " << error.what() << '\n';

        return 1;
    }
}
