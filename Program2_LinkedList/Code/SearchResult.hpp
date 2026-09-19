// ============================================================
// SearchResult.hpp
// CT077-3-2-DSTR  |  Shared searching support
// Results returned by any search operation, used to build the
// performance comparison tables in Section 7 of the assignment.
// ============================================================
#ifndef SEARCH_RESULT_HPP
#define SEARCH_RESULT_HPP

struct SearchResult {
    long long comparisons;
    int matchCount;
    double timeMs;
};

#endif