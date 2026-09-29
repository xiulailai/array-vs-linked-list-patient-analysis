// ============================================================
// PatientLinkedList.cpp
// CT077-3-2-DSTR  |  Program 2: SINGLY LINKED LIST Implementation
// Implementation of the linked-list-based patient collection.
// ============================================================
#include "PatientLinkedList.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
using namespace std;
using namespace std::chrono;

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

// ---------- Bubble Sort ----------
SortResult PatientLinkedList::bubbleSort(SortKey key) {
    SortResult result;
    result.comparisons = 0;
    result.swaps = 0;

    auto start = high_resolution_clock::now();

    if (head != nullptr) {
        bool swappedThisPass;
        Node* lastSorted = nullptr;

        do {
            swappedThisPass = false;
            Node* current = head;

            while (current->next != lastSorted) {
                result.comparisons++;

                if (getSortValue(current->data, key) > getSortValue(current->next->data, key)) {
                    Patient temp = current->data;
                    current->data = current->next->data;
                    current->next->data = temp;
                    result.swaps++;
                    swappedThisPass = true;
                }

                current = current->next;
            }

            lastSorted = current;
        } while (swappedThisPass);
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();

    return result;
}

// ---------- Searching (Section 7) ----------

SearchResult PatientLinkedList::searchByAgeRangeUnsorted(int minAge, int maxAge,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result;
    result.comparisons = 0;
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];

    auto start = high_resolution_clock::now();

    // No way to skip nodes - every node must be visited and checked.
    Node* current = head;
    while (current != nullptr) {
        result.comparisons++;
        if (current->data.age >= minAge && current->data.age <= maxAge) {
            outMatches[outMatchCount] = current->data;
            outMatchCount++;
        }
        current = current->next;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;

    return result;
}

SearchResult PatientLinkedList::searchByAgeRangeSorted(int minAge, int maxAge,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result;
    result.comparisons = 0;
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];

    auto start = high_resolution_clock::now();

    // Requires the list to already be sorted ascending by Age. There is
    // still no way to "jump" to the boundary (no random access), so we
    // must walk from head - but because it's sorted, we CAN stop the
    // instant we pass maxAge, instead of checking every remaining node.
    Node* current = head;
    while (current != nullptr) {
        result.comparisons++;
        if (current->data.age > maxAge)
            break; // sorted ascending - no more matches possible
        if (current->data.age >= minAge) {
            outMatches[outMatchCount] = current->data;
            outMatchCount++;
        }
        current = current->next;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;

    return result;
}

SearchResult PatientLinkedList::searchByCareType(const string& careType,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result;
    result.comparisons = 0;
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];

    auto start = high_resolution_clock::now();

    Node* current = head;
    while (current != nullptr) {
        result.comparisons++;
        if (current->data.careType == careType) {
            outMatches[outMatchCount] = current->data;
            outMatchCount++;
        }
        current = current->next;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;

    return result;
}

SearchResult PatientLinkedList::searchByDurationThresholdUnsorted(int minHours,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result;
    result.comparisons = 0;
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];

    auto start = high_resolution_clock::now();

    Node* current = head;
    while (current != nullptr) {
        result.comparisons++;
        if (current->data.lengthOfStay > minHours) {
            outMatches[outMatchCount] = current->data;
            outMatchCount++;
        }
        current = current->next;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;

    return result;
}

SearchResult PatientLinkedList::searchByDurationThresholdSorted(int minHours,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result;
    result.comparisons = 0;
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];

    auto start = high_resolution_clock::now();

    // Requires the list to already be sorted ascending by LengthOfStay.
    // Once the boundary is found, every remaining node automatically
    // qualifies - so we stop COUNTING comparisons there, even though we
    // must still walk the pointers to collect the remaining matches.
    Node* current = head;
    bool foundBoundary = false;

    while (current != nullptr) {
        if (!foundBoundary) {
            result.comparisons++;
            if (current->data.lengthOfStay > minHours)
                foundBoundary = true;
        }

        if (foundBoundary) {
            outMatches[outMatchCount] = current->data;
            outMatchCount++;
        }

        current = current->next;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;

    return result;
}

// ---------- Performance Analysis (Section 8) ----------
long long PatientLinkedList::getMemoryUsageBytes() const {
    // Exactly 'size' nodes exist - no unused-capacity waste like the
    // array's doubling strategy - but each Node adds one pointer's
    // worth of overhead (the 'next' field) that the array doesn't pay.
    return (long long)size * sizeof(Node) + sizeof(*this);
}

static void quickSortNodes(Node* first, Node* last, SortKey key, SortResult& result) {
    if (first == nullptr || last == nullptr || first == last) return;
    double pivot = getSortValue(last->data, key);
    Node* boundary = first;
    for (Node* current = first; current != last; current = current->next) {
        result.comparisons++;
        if (getSortValue(current->data, key) <= pivot) {
            if (boundary != current) {
                Patient temp = boundary->data; boundary->data = current->data; current->data = temp;
                result.swaps++;
            }
            boundary = boundary->next;
        }
    }
    if (boundary != last) {
        Patient temp = boundary->data; boundary->data = last->data; last->data = temp;
        result.swaps++;
    }
    Node* pivotNode = boundary;
    if (pivotNode != first) {
        Node* before = first;
        while (before->next != pivotNode) before = before->next;
        quickSortNodes(first, before, key, result);
    }
    if (pivotNode != last && pivotNode->next != nullptr)
        quickSortNodes(pivotNode->next, last, key, result);
}

SortResult PatientLinkedList::quickSort(SortKey key) {
    SortResult result = { 0, 0, 0.0 };
    auto start = high_resolution_clock::now();
    if (head != nullptr) quickSortNodes(head, tail, key, result);
    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    return result;
}

static unsigned long long linkedRadixValue(const Patient& p, SortKey key) {
    if (key == SORT_BY_AGE) return (unsigned long long)p.age;
    if (key == SORT_BY_LENGTH_OF_STAY) return (unsigned long long)p.lengthOfStay;
    return (unsigned long long)llround(getSortValue(p, key) * 100.0);
}

SortResult PatientLinkedList::radixSort(SortKey key) {
    SortResult result = { 0, 0, 0.0 };
    auto start = high_resolution_clock::now();
    unsigned long long maxValue = 0;
    for (Node* current = head; current != nullptr; current = current->next)
        if (linkedRadixValue(current->data, key) > maxValue) maxValue = linkedRadixValue(current->data, key);

    for (unsigned long long place = 1; maxValue / place > 0;) {
        Node* bucketHead[10] = { nullptr }; Node* bucketTail[10] = { nullptr };
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            current->next = nullptr;
            int digit = (int)((linkedRadixValue(current->data, key) / place) % 10);
            if (bucketHead[digit] == nullptr) bucketHead[digit] = current;
            else bucketTail[digit]->next = current;
            bucketTail[digit] = current;
            result.swaps++;
            current = next;
        }
        head = nullptr; tail = nullptr;
        for (int i = 0; i < 10; i++) if (bucketHead[i] != nullptr) {
            if (head == nullptr) head = bucketHead[i];
            else tail->next = bucketHead[i];
            tail = bucketTail[i];
        }
        if (place > maxValue / 10) break;
        place *= 10;
    }
    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    return result;
}