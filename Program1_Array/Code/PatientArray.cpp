// Implementation of the array-based patient collection.
#include "PatientArray.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

PatientArray::PatientArray(int initialCapacity) {
    capacity = initialCapacity;
    size = 0;
    list = new Patient[capacity];
}

PatientArray::~PatientArray() {
    delete[] list;
}

void PatientArray::resize(int newCapacity) {
    Patient* newList = new Patient[newCapacity];
    for (int i = 0; i < size; i++)
        newList[i] = list[i];
    delete[] list;
    list = newList;
    capacity = newCapacity;
}

void PatientArray::addPatient(const Patient& p) {
    if (size >= capacity)
        resize(capacity * 2);
    list[size] = p;
    size++;
}

int PatientArray::getSize() const {
    return size;
}

Patient PatientArray::getPatientAt(int index) const {
    return list[index];
}

bool PatientArray::loadFromCSV(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // skip header row

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Patient p;

        getline(ss, p.patientID, ',');

        getline(ss, token, ',');
        p.age = stoi(token);

        getline(ss, p.careType, ',');

        getline(ss, token, ',');
        p.lengthOfStay = stoi(token);

        getline(ss, token, ',');
        p.baseCostPerHour = stod(token);

        getline(ss, token, ',');
        p.daysVisitsPerYear = stoi(token);

        addPatient(p);
    }

    file.close();
    return true;
}

void PatientArray::printAll(int limit) const {
    cout << left
        << setw(12) << "PatientID"
        << setw(6) << "Age"
        << setw(18) << "CareType"
        << setw(10) << "StayHrs"
        << setw(14) << "CostPerHr"
        << setw(12) << "VisitsYr"
        << endl;
    cout << string(72, '-') << endl;

    int count = (limit == -1 || limit > size) ? size : limit;
    for (int i = 0; i < count; i++) {
        cout << left
            << setw(12) << list[i].patientID
            << setw(6) << list[i].age
            << setw(18) << list[i].careType
            << setw(10) << list[i].lengthOfStay
            << setw(14) << list[i].baseCostPerHour
            << setw(12) << list[i].daysVisitsPerYear
            << endl;
    }
    cout << endl << "Displayed " << count << " of " << size << " records.\n";
}

Patient* PatientArray::toArray(int& outCount) const {
    outCount = size;
    Patient* copy = new Patient[size];
    for (int i = 0; i < size; i++)
        copy[i] = list[i];
    return copy;
}