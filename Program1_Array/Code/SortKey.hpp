// SortKey.hpp
// CT077-3-2-DSTR  |  Shared sorting support
// Lets PatientLinkedList and PatientArray sort by Age, LengthOfStay, or Total
// Cost using the same comparison logic.

#ifndef SORT_KEY_HPP
#define SORT_KEY_HPP
#include "Patient.hpp"
#include <string>
using namespace std;
// Which field to sort patient records by
enum SortKey {
    sort_by_age,
    sort_by_length_of_staying,
    sort_by_total_cost
};
// Converts a SortKey into the actual numeric value used for comparisons
inline double getSortValue(const Patient& p, SortKey key) {
    switch (key) {
    case sort_by_age:
        return p.age;
    case sort_by_length_of_staying:
        return p.length_of_staying;
    case sort_by_total_cost:
        return p.length_of_staying * p.base_cost_per_hour * p.day_visit_per_year;
    default:
        return 0.0;
    }
}
// Human-readable label for performance report tables
inline string sortKeyLabel(SortKey key) {
    switch (key) {
    case sort_by_age: return "Age";
    case sort_by_length_of_staying: return "Length of Stay";
    case sort_by_total_cost: return "Total Cost";
    default: return "Unknown";
    }
}

// Results returned by a sort operation, used to build performance tables.
// aux_bytes = estimated EXTRA memory the algorithm needed while running
// (recursion stack, bucket pointers, ...), separate from the list itself.
struct SortResult {
    long long comparisons;
    long long swaps;
    double ms_time;
    long long aux_bytes;
};
#endif