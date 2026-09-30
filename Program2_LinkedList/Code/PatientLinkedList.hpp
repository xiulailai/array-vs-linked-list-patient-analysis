// PatientLinkedList.hpp
// CT077-3-2-DSTR  |  Program 2: SINGLY LINKED LIST Implementation
// Class declaration for the linked-list-based patient collection.
// Everything (storage, search results, analysis input) stays in
// linked lists - no arrays of Patient are used in this program.

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
    Node* tail;   // kept for O(1) insertAtEnd
    int size;

public:
    PatientLinkedList();
    ~PatientLinkedList();
    // Copying is disabled: a shallow copy would double-delete nodes.
    // Use append() to make a deep copy of another list instead.
    PatientLinkedList(const PatientLinkedList&) = delete;
    PatientLinkedList& operator=(const PatientLinkedList&) = delete;
    void clear();
    void insertAtEnd(const Patient& p);
    int getSize() const;
    const Node* getHead() const;                 // read-only access for analyzers
    void append(const PatientLinkedList& other); // deep-copies other's nodes onto the end
    bool loadFromCSV(const string& filename);
    void printAll(int limit = -1) const;
    // Sorting (Section 6) - all sort in place
    SortResult bubbleSort(SortKey key);
    SortResult quickSort(SortKey key);
    SortResult radixSort(SortKey key);
    SortResult mergeSort(SortKey key);   // relinks nodes - no data is copied
    // Searching (Section 7)
    // Matches are returned as a linked list (out_matches is cleared first).
    SearchResult searchByAgeRangeUnsorted(int min_age, int max_age,
        PatientLinkedList& out_matches) const;
    SearchResult searchByAgeRangeSorted(int min_age, int max_age,
        PatientLinkedList& out_matches) const;
    SearchResult searchByCareType(const string& care_type,
        PatientLinkedList& out_matches) const;
    SearchResult searchByDurationThresholdUnsorted(int min_hours,
        PatientLinkedList& out_matches) const;
    SearchResult searchByDurationThresholdSorted(int min_hours,
        PatientLinkedList& out_matches) const;
    // Performance Analysis (Section 8)
    // ESTIMATE: one Node (Patient + 'next' pointer) per element, plus the
    // class's own fixed overhead. No unused capacity, but every node pays
    // for an extra pointer the array does not need.
    long long getMemoryUsageBytes() const;
};
#endif