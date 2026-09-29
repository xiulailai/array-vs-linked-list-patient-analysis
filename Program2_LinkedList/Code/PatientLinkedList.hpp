// ============================================================
// PatientLinkedList.hpp
// CT077-3-2-DSTR  |  Program 2: SINGLY LINKED LIST Implementation
// Class declaration for the linked-list-based patient collection.
// ============================================================
#ifndef PATIENT_LINKED_LIST_HPP
#define PATIENT_LINKED_LIST_HPP

#include "Patient.hpp"
#include "Node.hpp"
#include "SortKey.hpp"
#include "SearchResult.hpp"
#include <string>
using namespace std;

class PatientLinkedList {
private:
    Node* head;
    Node* tail;   // kept for O(1) insertAtEnd, per lecture notes
    int size;

public:
    PatientLinkedList();
    ~PatientLinkedList();

    void clear();
    void insertAtEnd(const Patient& p);
    int getSize() const;

    bool loadFromCSV(const string& filename);
    void printAll(int limit = -1) const;
    Patient* toArray(int& outCount) const;

    // Sorts the list in place by the given key using Bubble Sort.
    SortResult bubbleSort(SortKey key);
    SortResult quickSort(SortKey key);
    SortResult radixSort(SortKey key);

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
    // one Node (Patient data + a 'next' pointer) per element actually
    // stored, plus the small fixed overhead of the class itself.
    // Unlike the array, there is no unused-capacity waste - but every
    // single node carries an extra pointer field the array does not need.
    long long getMemoryUsageBytes() const;
};

#endif