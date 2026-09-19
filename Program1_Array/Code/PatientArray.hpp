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
    SearchResult searchByAgeRangeUnsorted(int minAge, int maxAge,
        Patient*& outMatches, int& outMatchCount) const;
    SearchResult searchByAgeRangeSorted(int minAge, int maxAge,
        Patient*& outMatches, int& outMatchCount) const;
    SearchResult searchByCareType(const string& careType,
        Patient*& outMatches, int& outMatchCount) const;
    SearchResult searchByDurationThresholdUnsorted(int minHours,
        Patient*& outMatches, int& outMatchCount) const;
    SearchResult searchByDurationThresholdSorted(int minHours,
        Patient*& outMatches, int& outMatchCount) const;

    // ---------- Performance Analysis (Section 8) ----------
    // Returns an ESTIMATE of the structure's memory footprint in bytes:
    // the currently allocated CAPACITY (not just used size) times
    // sizeof(Patient), plus the small fixed overhead of the class itself.
    // Using capacity (not size) deliberately shows the "wasted" space
    // from the doubling resize strategy - a real cost of this design.
    // NOTE: does not add extra heap bytes used internally by std::string
    // members inside Patient if a string exceeds Small String
    // Optimization (SSO); acceptable simplification given short field values.
    long long getMemoryUsageBytes() const;
};

#endif