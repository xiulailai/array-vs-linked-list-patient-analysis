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

const Node* PatientLinkedList::getHead() const {
    return head;
}

void PatientLinkedList::append(const PatientLinkedList& other) {
    for (const Node* current = other.head; current != nullptr; current = current->next)
        insertAtEnd(current->data);
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

// ==================== SORTING (Section 6) ====================

// ---------- Bubble Sort ----------
SortResult PatientLinkedList::bubbleSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 }; // needs no extra memory

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

// ---------- Quick Sort (last node as pivot) ----------
// 'depth'/'maxDepth' track recursion depth so we can estimate stack memory.
static void quickSortNodes(Node* first, Node* last, SortKey key, SortResult& result,
    int depth, int& maxDepth) {
    if (first == nullptr || last == nullptr || first == last) return;
    if (depth > maxDepth) maxDepth = depth;

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
        // A singly linked list cannot step backwards, so we must walk from
        // 'first' to find the node before the pivot. Every hop is counted
        // so this hidden O(n) cost shows up in the comparison total.
        Node* before = first;
        while (before->next != pivotNode) {
            before = before->next;
            result.comparisons++;
        }
        quickSortNodes(first, before, key, result, depth + 1, maxDepth);
    }
    if (pivotNode != last && pivotNode->next != nullptr)
        quickSortNodes(pivotNode->next, last, key, result, depth + 1, maxDepth);
}

SortResult PatientLinkedList::quickSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    int maxDepth = 0;
    auto start = high_resolution_clock::now();
    if (head != nullptr) quickSortNodes(head, tail, key, result, 1, maxDepth);
    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    // Estimate: ~64 bytes of stack per recursive call frame.
    result.auxBytes = (long long)maxDepth * 64;
    return result;
}

// ---------- Radix Sort (LSD, linked-list buckets) ----------
// Age / stay use whole units; total cost is converted to cents.
static unsigned long long linkedRadixValue(const Patient& p, SortKey key) {
    if (key == SORT_BY_AGE) return (unsigned long long)p.age;
    if (key == SORT_BY_LENGTH_OF_STAY) return (unsigned long long)p.lengthOfStay;
    return (unsigned long long)llround(getSortValue(p, key) * 100.0);
}

SortResult PatientLinkedList::radixSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    auto start = high_resolution_clock::now();

    unsigned long long maxValue = 0;
    for (Node* current = head; current != nullptr; current = current->next)
        if (linkedRadixValue(current->data, key) > maxValue)
            maxValue = linkedRadixValue(current->data, key);

    for (unsigned long long place = 1; maxValue / place > 0;) {
        Node* bucketHead[10] = { nullptr };
        Node* bucketTail[10] = { nullptr };
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            current->next = nullptr;
            int digit = (int)((linkedRadixValue(current->data, key) / place) % 10);
            if (bucketHead[digit] == nullptr) bucketHead[digit] = current;
            else bucketTail[digit]->next = current;
            bucketTail[digit] = current;
            result.swaps++; // counts node relinks ("moves"); radix makes no comparisons
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
    // Only 10 head + 10 tail pointers - nodes are relinked, never copied.
    result.auxBytes = 20 * (long long)sizeof(Node*);
    return result;
}

// ---------- Merge Sort (relinks nodes, never copies Patient data) ----------
// The natural sort for linked lists: splitting needs no random access and
// merging just rewires 'next' pointers, so it needs NO extra node storage.
static Node* mergeSortedNodes(Node* a, Node* b, SortKey key, SortResult& result) {
    Node* mergedHead = nullptr;
    Node* mergedTail = nullptr;
    while (a != nullptr && b != nullptr) {
        result.comparisons++;
        Node* chosen;
        if (getSortValue(b->data, key) < getSortValue(a->data, key)) { chosen = b; b = b->next; }
        else { chosen = a; a = a->next; } // ties take 'a' first, so the sort is stable
        chosen->next = nullptr;
        if (mergedHead == nullptr) mergedHead = chosen;
        else mergedTail->next = chosen;
        mergedTail = chosen;
        result.swaps++; // counts a node relink (a "move")
    }
    Node* rest = (a != nullptr) ? a : b;
    if (mergedHead == nullptr) return rest;
    mergedTail->next = rest;
    return mergedHead;
}

static Node* mergeSortNodes(Node* first, SortKey key, SortResult& result,
    int depth, int& maxDepth) {
    if (first == nullptr || first->next == nullptr) return first;
    if (depth > maxDepth) maxDepth = depth;

    // Slow/fast pointers find the middle without needing an index.
    Node* slow = first;
    Node* fast = first->next;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node* second = slow->next;
    slow->next = nullptr;

    Node* left = mergeSortNodes(first, key, result, depth + 1, maxDepth);
    Node* right = mergeSortNodes(second, key, result, depth + 1, maxDepth);
    return mergeSortedNodes(left, right, key, result);
}

SortResult PatientLinkedList::mergeSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    int maxDepth = 0;
    auto start = high_resolution_clock::now();

    head = mergeSortNodes(head, key, result, 1, maxDepth);
    tail = head; // re-find the tail after relinking
    while (tail != nullptr && tail->next != nullptr) tail = tail->next;

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.auxBytes = (long long)maxDepth * 64; // recursion stack only (~64 B/frame)
    return result;
}

// ==================== SEARCHING (Section 7) ====================
// Matches are appended to 'outMatches' (a linked list). 'comparisons'
// counts criterion checks; 'steps' counts every node visited.

SearchResult PatientLinkedList::searchByAgeRangeUnsorted(int minAge, int maxAge,
    PatientLinkedList& outMatches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatches.clear();

    auto start = high_resolution_clock::now();

    // No way to skip nodes - every node must be visited and checked.
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.age >= minAge && current->data.age <= maxAge)
            outMatches.insertAtEnd(current->data);
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatches.getSize();
    result.auxBytes = (long long)outMatches.getSize() * sizeof(Node);
    return result;
}

SearchResult PatientLinkedList::searchByAgeRangeSorted(int minAge, int maxAge,
    PatientLinkedList& outMatches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatches.clear();

    auto start = high_resolution_clock::now();

    // List must already be sorted ascending by Age. No random access, so we
    // still walk from head, but we can stop as soon as we pass maxAge.
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.age > maxAge)
            break; // sorted ascending - no more matches possible
        if (current->data.age >= minAge)
            outMatches.insertAtEnd(current->data);
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatches.getSize();
    result.auxBytes = (long long)outMatches.getSize() * sizeof(Node);
    return result;
}

SearchResult PatientLinkedList::searchByCareType(const string& careType,
    PatientLinkedList& outMatches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatches.clear();

    auto start = high_resolution_clock::now();

    // CareType has no numeric order, so this is always a full linear scan.
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.careType == careType)
            outMatches.insertAtEnd(current->data);
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatches.getSize();
    result.auxBytes = (long long)outMatches.getSize() * sizeof(Node);
    return result;
}

SearchResult PatientLinkedList::searchByDurationThresholdUnsorted(int minHours,
    PatientLinkedList& outMatches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatches.clear();

    auto start = high_resolution_clock::now();

    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.lengthOfStay > minHours)
            outMatches.insertAtEnd(current->data);
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatches.getSize();
    result.auxBytes = (long long)outMatches.getSize() * sizeof(Node);
    return result;
}

SearchResult PatientLinkedList::searchByDurationThresholdSorted(int minHours,
    PatientLinkedList& outMatches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatches.clear();

    auto start = high_resolution_clock::now();

    // List must be sorted ascending by LengthOfStay. Comparisons stop once
    // the boundary is found (everything after it qualifies), BUT every node
    // is still visited to copy it out - so 'steps' stays O(n) even though
    // 'comparisons' is small. An array would jump straight to the boundary.
    bool foundBoundary = false;
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        if (!foundBoundary) {
            result.comparisons++;
            if (current->data.lengthOfStay > minHours)
                foundBoundary = true;
        }
        if (foundBoundary)
            outMatches.insertAtEnd(current->data);
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatches.getSize();
    result.auxBytes = (long long)outMatches.getSize() * sizeof(Node);
    return result;
}

// ==================== PERFORMANCE ANALYSIS (Section 8) ====================
long long PatientLinkedList::getMemoryUsageBytes() const {
    return (long long)size * sizeof(Node) + sizeof(*this);
}