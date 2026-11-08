#include "Ranking.h"
#include <cmath>
#include <utility>

namespace Ranking {

    namespace {
        constexpr double EPSILON = 1e-9;

        void mergeRange(std::vector<ScoredRecipe>& arr, int left, int mid, int right) {
            int n1 = mid - left + 1;
            int n2 = right - mid;

            std::vector<ScoredRecipe> leftArr(n1);
            std::vector<ScoredRecipe> rightArr(n2);

            for (int i = 0; i < n1; ++i) leftArr[i] = arr[left + i];
            for (int j = 0; j < n2; ++j) rightArr[j] = arr[mid + 1 + j];

            int i = 0, j = 0, k = left;
            while (i < n1 && j < n2) {
                if (isHigherRank(leftArr[i], rightArr[j])) {
                    arr[k++] = leftArr[i++];
                } else {
                    arr[k++] = rightArr[j++];
                }
            }
            while (i < n1) arr[k++] = leftArr[i++];
            while (j < n2) arr[k++] = rightArr[j++];
        }

        void mergeSortRecursive(std::vector<ScoredRecipe>& arr, int left, int right) {
            if (left >= right) return;
            int mid = left + (right - left) / 2;
            mergeSortRecursive(arr, left, mid);
            mergeSortRecursive(arr, mid + 1, right);
            mergeRange(arr, left, mid, right);
        }

        // Custom Min-Heap that keeps the "worst" of the top-N elements at the root (index 0)
        // so any better candidate can replace it in O(log N) time.
        class TopNMinHeap {
        private:
            std::vector<ScoredRecipe> heap;

            // Returns true if 'a' is WORSE than 'b' (so the worst of top-N sits at root)
            bool isWorseRank(const ScoredRecipe& a, const ScoredRecipe& b) const {
                return isHigherRank(b, a);
            }

            void siftUp(size_t idx) {
                while (idx > 0) {
                    size_t parent = (idx - 1) / 2;
                    if (isWorseRank(heap[idx], heap[parent])) {
                        std::swap(heap[idx], heap[parent]);
                        idx = parent;
                    } else {
                        break;
                    }
                }
            }

            void siftDown(size_t idx) {
                size_t size = heap.size();
                while (true) {
                    size_t smallest = idx;
                    size_t left = 2 * idx + 1;
                    size_t right = 2 * idx + 2;

                    if (left < size && isWorseRank(heap[left], heap[smallest])) {
                        smallest = left;
                    }
                    if (right < size && isWorseRank(heap[right], heap[smallest])) {
                        smallest = right;
                    }
                    if (smallest != idx) {
                        std::swap(heap[idx], heap[smallest]);
                        idx = smallest;
                    } else {
                        break;
                    }
                }
            }

        public:
            explicit TopNMinHeap(size_t capacity) {
                heap.reserve(capacity);
            }

            size_t size() const { return heap.size(); }
            const ScoredRecipe& peekWorst() const { return heap.front(); }

            void push(const ScoredRecipe& item) {
                heap.push_back(item);
                siftUp(heap.size() - 1);
            }

            void replaceWorst(const ScoredRecipe& item) {
                heap[0] = item;
                siftDown(0);
            }

            std::vector<ScoredRecipe> extractAll() {
                return std::move(heap);
            }
        };
    } // anonymous namespace

    bool isHigherRank(const ScoredRecipe& a, const ScoredRecipe& b) {
        // 1. Primary: Higher similarity score
        if (std::abs(a.score - b.score) > EPSILON) {
            return a.score > b.score;
        }
        // 2. Secondary: More matched ingredients
        if (a.overlapCount != b.overlapCount) {
            return a.overlapCount > b.overlapCount;
        }
        // 3. Tertiary: Fewer total recipe ingredients (fewer missing ingredients)
        if (a.recipeIngredientCount != b.recipeIngredientCount) {
            return a.recipeIngredientCount < b.recipeIngredientCount;
        }
        // 4. Quaternary: Lower recipe ID for strict determinism
        return a.recipeId < b.recipeId;
    }

    void rankAllRecipes(std::vector<ScoredRecipe>& scoredRecipes) {
        if (scoredRecipes.size() <= 1) return;
        mergeSortRecursive(scoredRecipes, 0, static_cast<int>(scoredRecipes.size()) - 1);
    }

    std::vector<ScoredRecipe> getTopNRecipes(const std::vector<ScoredRecipe>& scoredRecipes,
                                             int topN) {
        if (topN <= 0 || scoredRecipes.empty()) {
            return {};
        }

        size_t targetN = static_cast<size_t>(topN);
        TopNMinHeap minHeap(targetN);

        for (const ScoredRecipe& candidate : scoredRecipes) {
            if (minHeap.size() < targetN) {
                minHeap.push(candidate);
            } else if (isHigherRank(candidate, minHeap.peekWorst())) {
                minHeap.replaceWorst(candidate);
            }
        }

        std::vector<ScoredRecipe> result = minHeap.extractAll();
        rankAllRecipes(result); // Sort the top-N winners from highest to lowest
        return result;
    }

} // namespace Ranking
