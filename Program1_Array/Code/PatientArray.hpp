// PatientArray.hpp
// CT077-3-2-DSTR  |  Program 1: ARRAY Implementation
// Class declaration for the array-based patient collection.

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
    void resize(int new_capacity);
public:
    PatientArray(int initial_capacity = 250);
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
    // Sorting (Section 6) - all sort in place
    SortResult bubbleSort(SortKey key);
    SortResult quickSort(SortKey key);
    SortResult radixSort(SortKey key);
    SortResult mergeSort(SortKey key);
    // Searching (Section 7)
    SearchResult searchByAgeRangeUnsorted(int min_age, int max_age,
        Patient*& out_matches, int& out_match_count) const;
    SearchResult searchByAgeRangeSorted(int min_age, int max_age,
        Patient*& out_matches, int& out_match_count) const;
    SearchResult searchByCareType(const string& care_type,
        Patient*& out_matches, int& out_match_count) const;
    SearchResult searchByDurationThresholdUnsorted(int min_hours,
        Patient*& out_matches, int& out_match_count) const;
    SearchResult searchByDurationThresholdSorted(int min_hours,
        Patient*& out_matches, int& out_match_count) const;
    // Performance Analysis (Section 8)
    // ESTIMATE: allocated CAPACITY (not just used size) times sizeof(Patient),
    // plus the class's own fixed overhead. Using capacity deliberately shows
    // the "wasted" space from the doubling strategy.
    long long getMemoryUsageBytes() const;
};
#endif