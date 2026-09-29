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

    // Doubles the array size when full (keeps insertion amortized O(1))
    void resize(int newCapacity);

public:
    PatientArray(int initialCapacity = 250);
    ~PatientArray();

    // Copying is disabled (a shallow copy would double-delete the buffer).
    // Use append() to make a deep copy of another PatientArray instead.
    PatientArray(const PatientArray&) = delete;
    PatientArray& operator=(const PatientArray&) = delete;

    void addPatient(const Patient& p);
    int getSize() const;
    Patient getPatientAt(int index) const;
    const Patient* getData() const;             // read-only view for the analyzers
    void append(const PatientArray& other);     // deep-copies other's records onto the end

    // Loads records from CSV. Expects header row:
    // PatientID,Age,CareType,LengthOfStay,BaseCostPerHour,DaysVisitsPerYear
    bool loadFromCSV(const string& filename);

    // Prints records as a formatted console table. limit = -1 prints all.
    void printAll(int limit = -1) const;

    // ---------- Sorting (Section 6) - all sort in place ----------
    SortResult bubbleSort(SortKey key);
    SortResult quickSort(SortKey key);
    SortResult radixSort(SortKey key);
    SortResult mergeSort(SortKey key);

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
    // ESTIMATE: allocated CAPACITY (not just used size) times sizeof(Patient),
    // plus the class's own fixed overhead. Using capacity deliberately shows
    // the "wasted" space from the doubling strategy.
    long long getMemoryUsageBytes() const;
};

#endif