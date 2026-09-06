// Class declaration for the array-based patient collection.
#ifndef PATIENT_ARRAY_HPP
#define PATIENT_ARRAY_HPP

#include "Patient.hpp"
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
};

#endif