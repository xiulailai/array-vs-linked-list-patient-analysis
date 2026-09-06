// Implementation of the linked-list-based patient collection.
#include "PatientLinkedList.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

PatientLinkedList::PatientLinkedList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

PatientLinkedList::~PatientLinkedList() {
    clear();
}

void PatientLinkedList::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* toDelete = current;
        current = current->next;
        delete toDelete;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void PatientLinkedList::insertAtEnd(const Patient& p) {
    Node* newNode = new Node;
    newNode->data = p;
    newNode->next = nullptr;

    if (head == nullptr) {
        head = tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
    size++;
}

int PatientLinkedList::getSize() const {
    return size;
}

bool PatientLinkedList::loadFromCSV(const string& filename) {
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

        insertAtEnd(p);
    }

    file.close();
    return true;
}

void PatientLinkedList::printAll(int limit) const {
    cout << left
        << setw(12) << "PatientID"
        << setw(6) << "Age"
        << setw(18) << "CareType"
        << setw(10) << "StayHrs"
        << setw(14) << "CostPerHr"
        << setw(12) << "VisitsYr"
        << endl;
    cout << string(72, '-') << endl;

    Node* current = head;
    int count = 0;
    while (current != nullptr && (limit == -1 || count < limit)) {
        const Patient& p = current->data;
        cout << left
            << setw(12) << p.patientID
            << setw(6) << p.age
            << setw(18) << p.careType
            << setw(10) << p.lengthOfStay
            << setw(14) << p.baseCostPerHour
            << setw(12) << p.daysVisitsPerYear
            << endl;
        current = current->next;
        count++;
    }
    cout << endl << "Displayed " << count << " of " << size << " records.\n";
}

Patient* PatientLinkedList::toArray(int& outCount) const {
    outCount = size;
    Patient* copy = new Patient[size];
    Node* current = head;
    int i = 0;
    while (current != nullptr) {
        copy[i] = current->data;
        current = current->next;
        i++;
    }
    return copy;
}