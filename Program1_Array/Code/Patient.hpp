// ============================================================
// PatientArray.hpp
// CT077-3-2-DSTR  |  Program 1: ARRAY Implementation
// Class declaration for the array-based patient collection.
// ============================================================
#ifndef PATIENT_ARRAY_HPP
#define PATIENT_ARRAY_HPP

#include "Patient.hpp"
#include "SortKey.hpp"
#include "SearchResult.hpp"
#include <string>
using namespace std;

class PatientArray {
private:
    Patient* list;   // dynamically allocated array (no STL containers)
    int size;        // number of patients currently stored
    int capacity;    // current max capacity of the array

    // Doubles the array size when full (keeps insertion "amortized" efficient)
    void resize(int newCapacity);

public:
    PatientArray(int initialCapacity = 250);
    ~PatientArray();

    void addPatient(const Patient& p);
    int getSize() const;
    Patient getPatientAt(int index) const;

    // Loads records from CSV. Expects header row:
    // PatientID,Age,CareType,LengthOfStay,BaseCostPerHour,DaysVisitsPerYear
    bool loadFromCSV(const string& filename);

    // Prints records as a formatted console table. limit = -1 prints all.
    void printAll(int limit = -1) const;

    // Returns a NEW dynamically allocated copy of all patients as a plain
    // array, so other classes (like AgeGroupAnalyzer) can process the data
    // without needing to know this class's internal storage details.
    // Caller is responsible for calling delete[] on the returned pointer.
    Patient* toArray(int& outCount) const;

    // Sorts the array in place by the given key using Bubble Sort.
    SortResult bubbleSort(SortKey key);

    // ---------- Searching (Section 7) ----------
    // Every search allocates outMatches with 'new' (caller must delete[]).

    // Age range search - full linear scan, works on ANY order.
    SearchResult searchByAgeRangeUnsorted(int minAge, int maxAge,
        Patient*& outMatches, int& outMatchCount) const;

    // Age range search - REQUIRES the array to already be sorted by Age
    // (call bubbleSort(SORT_BY_AGE) first). Uses binary search to find
    // the first matching index, then a contiguous scan for the rest.
    SearchResult searchByAgeRangeSorted(int minAge, int maxAge,
        Patient*& outMatches, int& outMatchCount) const;

    // CareType is categorical (no natural order), so this is always a
    // linear scan - sorting a category doesn't reduce comparisons
    // without a more advanced structure like a hash table.
    SearchResult searchByCareType(const string& careType,
        Patient*& outMatches, int& outMatchCount) const;

    // Duration threshold search (LengthOfStay > minHours) - linear scan.
    SearchResult searchByDurationThresholdUnsorted(int minHours,
        Patient*& outMatches, int& outMatchCount) const;

    // Duration threshold search - REQUIRES the array to already be sorted
    // by LengthOfStay (call bubbleSort(SORT_BY_LENGTH_OF_STAY) first).
    // Binary search finds the first qualifying index; everything after
    // it automatically qualifies too.
    SearchResult searchByDurationThresholdSorted(int minHours,
        Patient*& outMatches, int& outMatchCount) const;
};

#endif