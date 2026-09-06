// Class declaration for the linked-list-based patient collection.
#ifndef PATIENT_LINKED_LIST_HPP
#define PATIENT_LINKED_LIST_HPP

#include "Patient.hpp"
#include "Node.hpp"
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

    // Frees every node (prevents memory leaks)
    void clear();

    void insertAtEnd(const Patient& p);
    int getSize() const;

    // Loads records from CSV. Expects header row:
    // PatientID,Age,CareType,LengthOfStay,BaseCostPerHour,DaysVisitsPerYear
    bool loadFromCSV(const string& filename);

    // Prints records as a formatted console table. limit = -1 prints all.
    void printAll(int limit = -1) const;

    // Traverses the list once and returns a NEW dynamically allocated
    // plain array copy of all patients, so other classes (like
    // AgeGroupAnalyzer) can process the data without needing to walk
    // node pointers themselves. Caller must call delete[] when done.
    Patient* toArray(int& outCount) const;
};

#endif