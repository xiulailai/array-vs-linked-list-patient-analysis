// PatientLinkedList.cpp
// CT077-3-2-DSTR  |  Program 2: SINGLY LINKED LIST Implementation
// Implementation of the linked-list-based patient collection.
 
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
        Node* to_delete = current;
        current = current->next;
        delete to_delete;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}
void PatientLinkedList::insertAtEnd(const Patient& p) {
    Node* new_node = new Node;
    new_node->data = p;
    new_node->next = nullptr;
    if (head == nullptr) {
        head = tail = new_node;
    }
    else {
        tail->next = new_node;
        tail = new_node;
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
        getline(ss, p.patient_id, ',');
        getline(ss, token, ',');
        p.age = stoi(token);
        getline(ss, p.care_type, ',');
        getline(ss, token, ',');
        p.length_of_staying = stoi(token);
        getline(ss, token, ',');
        p.base_cost_per_hour = stod(token);
        getline(ss, token, ',');
        p.day_visit_per_year = stoi(token);
        insertAtEnd(p);
    }
    file.close();
    return true;
}
void PatientLinkedList::printAll(int limit) const {
    cout << left
        << setw(12) << "Patient ID"
        << setw(6) << "Age"
        << setw(18) << "Care Type"
        << setw(10) << "Stay Hrs"
        << setw(14) << "Cost Per Hr"
        << setw(12) << "Visits Yr"
        << endl;
    cout << string(72, '-') << endl;
    Node* current = head;
    int count = 0;
    while (current != nullptr && (limit == -1 || count < limit)) {
        const Patient& p = current->data;
        cout << left
            << setw(12) << p.patient_id
            << setw(6) << p.age
            << setw(18) << p.care_type
            << setw(10) << p.length_of_staying
            << setw(14) << p.base_cost_per_hour
            << setw(12) << p.day_visit_per_year
            << endl;
        current = current->next;
        count++;
    }
    cout << endl << "Displayed " << count << " of " << size << " records.\n";
}
// SORTING (Section 6)
// Bubble Sort
SortResult PatientLinkedList::bubbleSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 }; // needs no extra memory
    auto start = high_resolution_clock::now();
    if (head != nullptr) {
        bool swapped_this_pass;
        Node* last_sorted = nullptr;
        do {
            swapped_this_pass = false;
            Node* current = head;
            while (current->next != last_sorted) {
                result.comparisons++;
                if (getSortValue(current->data, key) > getSortValue(current->next->data, key)) {
                    Patient temp = current->data;
                    current->data = current->next->data;
                    current->next->data = temp;
                    result.swaps++;
                    swapped_this_pass = true;
                }
                current = current->next;
            }
            last_sorted = current;
        } while (swapped_this_pass);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    return result;
}
// Quick Sort (last node as pivot)
// 'depth'/'max_depth' track recursion depth so we can estimate stack memory.
static void quickSortNodes(Node* first, Node* last, SortKey key, SortResult& result,
    int depth, int& max_depth) {
    if (first == nullptr || last == nullptr || first == last) return;
    if (depth > max_depth) max_depth = depth;
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
    Node* pivot_node = boundary;
    if (pivot_node != first) {
        // A singly linked list cannot step backwards, so we must walk from
        // 'first' to find the node before the pivot. Every hop is counted
        // so this hidden O(n) cost shows up in the comparison total.
        Node* before = first;
        while (before->next != pivot_node) {
            before = before->next;
            result.comparisons++;
        }
        quickSortNodes(first, before, key, result, depth + 1, max_depth);
    }
    if (pivot_node != last && pivot_node->next != nullptr)
        quickSortNodes(pivot_node->next, last, key, result, depth + 1, max_depth);
}
SortResult PatientLinkedList::quickSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    int max_depth = 0;
    auto start = high_resolution_clock::now();
    if (head != nullptr) quickSortNodes(head, tail, key, result, 1, max_depth);
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    // Estimate: ~64 bytes of stack per recursive call frame.
    result.aux_bytes = (long long)max_depth * 64;
    return result;
}
// Radix Sort (linked-list buckets)
// Age / stay use whole units; total cost is converted to cents.
static unsigned long long linked_radix_value(const Patient& p, SortKey key) {
    if (key == sort_by_age) return (unsigned long long)p.age;
    if (key == sort_by_length_of_staying) return (unsigned long long)p.length_of_staying;
    return (unsigned long long)llround(getSortValue(p, key) * 100.0);
}
SortResult PatientLinkedList::radixSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    auto start = high_resolution_clock::now();
    unsigned long long max_value = 0;
    for (Node* current = head; current != nullptr; current = current->next)
        if (linked_radix_value(current->data, key) > max_value)
            max_value = linked_radix_value(current->data, key);
    for (unsigned long long place = 1; max_value / place > 0;) {
        Node* bucket_head[10] = { nullptr };
        Node* bucket_tail[10] = { nullptr };
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->next;
            current->next = nullptr;
            int digit = (int)((linked_radix_value(current->data, key) / place) % 10);
            if (bucket_head[digit] == nullptr) bucket_head[digit] = current;
            else bucket_tail[digit]->next = current;
            bucket_tail[digit] = current;
            result.swaps++; // counts node relinks ("moves"); radix makes no comparisons
            current = next;
        }
        head = nullptr; tail = nullptr;
        for (int i = 0; i < 10; i++) if (bucket_head[i] != nullptr) {
            if (head == nullptr) head = bucket_head[i];
            else tail->next = bucket_head[i];
            tail = bucket_tail[i];
        }
        if (place > max_value / 10) break;
        place *= 10;
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    // Only 10 head + 10 tail pointers - nodes are relinked, never copied.
    result.aux_bytes = 20 * (long long)sizeof(Node*);
    return result;
}
// Merge Sort (relinks nodes, never copies Patient data)
// The natural sort for linked lists: splitting needs no random access and
// merging just rewires 'next' pointers, so it needs NO extra node storage.
static Node* mergeSortedNodes(Node* a, Node* b, SortKey key, SortResult& result) {
    Node* merged_head = nullptr;
    Node* merged_tail = nullptr;
    while (a != nullptr && b != nullptr) {
        result.comparisons++;
        Node* chosen;
        if (getSortValue(b->data, key) < getSortValue(a->data, key)) { chosen = b; b = b->next; }
        else { chosen = a; a = a->next; } // ties take 'a' first, so the sort is stable
        chosen->next = nullptr;
        if (merged_head == nullptr) merged_head = chosen;
        else merged_tail->next = chosen;
        merged_tail = chosen;
        result.swaps++; // counts a node relink (a "move")
    }
    Node* rest = (a != nullptr) ? a : b;
    if (merged_head == nullptr) return rest;
    merged_tail->next = rest;
    return merged_head;
}
static Node* mergeSortNodes(Node* first, SortKey key, SortResult& result,
    int depth, int& max_depth) {
    if (first == nullptr || first->next == nullptr) return first;
    if (depth > max_depth) max_depth = depth;
    // Slow/fast pointers find the middle without needing an index.
    Node* slow = first;
    Node* fast = first->next;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node* second = slow->next;
    slow->next = nullptr;
    Node* left = mergeSortNodes(first, key, result, depth + 1, max_depth);
    Node* right = mergeSortNodes(second, key, result, depth + 1, max_depth);
    return mergeSortedNodes(left, right, key, result);
}
SortResult PatientLinkedList::mergeSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    int max_depth = 0;
    auto start = high_resolution_clock::now();
    head = mergeSortNodes(head, key, result, 1, max_depth);
    tail = head; // re-find the tail after relinking
    while (tail != nullptr && tail->next != nullptr) tail = tail->next;
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.aux_bytes = (long long)max_depth * 64; // recursion stack only (~64 B/frame)
    return result;
}
// SEARCHING (Section 7)
// Matches are appended to 'out_matches' (a linked list). 'comparisons'
// counts criterion checks; 'steps' counts every node visited.
SearchResult PatientLinkedList::searchByAgeRangeUnsorted(int min_age, int max_age,
    PatientLinkedList& out_matches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_matches.clear();
    auto start = high_resolution_clock::now();
    // No way to skip nodes - every node must be visited and checked.
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.age >= min_age && current->data.age <= max_age)
            out_matches.insertAtEnd(current->data);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_matches.getSize();
    result.aux_bytes = (long long)out_matches.getSize() * sizeof(Node);
    return result;
}
SearchResult PatientLinkedList::searchByAgeRangeSorted(int min_age, int max_age,
    PatientLinkedList& out_matches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_matches.clear();
    auto start = high_resolution_clock::now();
    // List must already be sorted ascending by Age. No random access, so we
    // still walk from head, but we can stop as soon as we pass max_age.
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.age > max_age)
            break; // sorted ascending - no more matches possible
        if (current->data.age >= min_age)
            out_matches.insertAtEnd(current->data);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_matches.getSize();
    result.aux_bytes = (long long)out_matches.getSize() * sizeof(Node);
    return result;
}
SearchResult PatientLinkedList::searchByCareType(const string& care_type,
    PatientLinkedList& out_matches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_matches.clear();
    auto start = high_resolution_clock::now();
    // CareType has no numeric order, so this is always a full linear scan.
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.care_type == care_type)
            out_matches.insertAtEnd(current->data);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_matches.getSize();
    result.aux_bytes = (long long)out_matches.getSize() * sizeof(Node);
    return result;
}
SearchResult PatientLinkedList::searchByDurationThresholdUnsorted(int min_hours,
    PatientLinkedList& out_matches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_matches.clear();
    auto start = high_resolution_clock::now();
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        result.comparisons++;
        if (current->data.length_of_staying > min_hours)
            out_matches.insertAtEnd(current->data);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_matches.getSize();
    result.aux_bytes = (long long)out_matches.getSize() * sizeof(Node);
    return result;
}
SearchResult PatientLinkedList::searchByDurationThresholdSorted(int min_hours,
    PatientLinkedList& out_matches) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_matches.clear();
    auto start = high_resolution_clock::now();
    // List must be sorted ascending by LengthOfStay. Comparisons stop once
    // the boundary is found (everything after it qualifies), BUT every node
    // is still visited to copy it out - so 'steps' stays O(n) even though
    // 'comparisons' is small. An array would jump straight to the boundary.
    bool found_boundary = false;
    for (Node* current = head; current != nullptr; current = current->next) {
        result.steps++;
        if (!found_boundary) {
            result.comparisons++;
            if (current->data.length_of_staying > min_hours)
                found_boundary = true;
        }
        if (found_boundary)
            out_matches.insertAtEnd(current->data);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_matches.getSize();
    result.aux_bytes = (long long)out_matches.getSize() * sizeof(Node);
    return result;
}
// PERFORMANCE ANALYSIS (Section 8)
long long PatientLinkedList::getMemoryUsageBytes() const {
    return (long long)size * sizeof(Node) + sizeof(*this);
}