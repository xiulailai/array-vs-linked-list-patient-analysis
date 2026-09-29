// ============================================================
// SearchResult.hpp
// CT077-3-2-DSTR  |  Shared searching support
// Results returned by any search operation, used to build the
// performance comparison tables in Section 7 of the assignment.
// ============================================================
#ifndef SEARCH_RESULT_HPP
#define SEARCH_RESULT_HPP

struct SearchResult {
    long long comparisons;  // value comparisons made against the search criterion
    long long steps;        // nodes actually visited (pointer hops) - shows the O(n) walk
    int matchCount;
    double timeMs;
    long long auxBytes;     // memory used to store the matching records
};

#endif