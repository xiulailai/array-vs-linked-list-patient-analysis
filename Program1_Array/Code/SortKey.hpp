// ============================================================
// SortKey.hpp
// CT077-3-2-DSTR  |  Shared sorting support
// Lets PatientLinkedList and PatientArray sort by Age, LengthOfStay, or Total
// Cost using the same comparison logic.
// ============================================================
#ifndef SORT_KEY_HPP
#define SORT_KEY_HPP

#include "Patient.hpp"
#include <string>
using namespace std;

// Which field to sort patient records by
enum SortKey {
    SORT_BY_AGE,
    SORT_BY_LENGTH_OF_STAY,
    SORT_BY_TOTAL_COST
};

// Converts a SortKey into the actual numeric value used for comparisons
inline double getSortValue(const Patient& p, SortKey key) {
    switch (key) {
    case SORT_BY_AGE:
        return p.age;
    case SORT_BY_LENGTH_OF_STAY:
        return p.lengthOfStay;
    case SORT_BY_TOTAL_COST:
        return p.lengthOfStay * p.baseCostPerHour * p.daysVisitsPerYear;
    default:
        return 0.0;
    }
}

// Human-readable label for performance report tables
inline string sortKeyLabel(SortKey key) {
    switch (key) {
    case SORT_BY_AGE: return "Age";
    case SORT_BY_LENGTH_OF_STAY: return "Length of Stay";
    case SORT_BY_TOTAL_COST: return "Total Cost";
    default: return "Unknown";
    }
}

// Results returned by a sort operation, used to build performance tables.
// auxBytes = estimated EXTRA memory the algorithm needed while running
// (recursion stack, bucket pointers, ...), separate from the list itself.
struct SortResult {
    long long comparisons;
    long long swaps;
    double timeMs;
    long long auxBytes;
};

#endif