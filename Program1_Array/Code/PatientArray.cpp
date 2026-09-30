// PatientArray.cpp
// CT077-3-2-DSTR  |  Program 1: ARRAY Implementation
// Implementation of the array-based patient collection.

#include "PatientArray.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
using namespace std;
using namespace std::chrono;

PatientArray::PatientArray(int initial_capacity) {
    capacity = initial_capacity;
    size = 0;
    list = new Patient[capacity];
}
PatientArray::~PatientArray() {
    delete[] list;
}

void PatientArray::resize(int new_capacity) {
    Patient* newList = new Patient[new_capacity];
    for (int i = 0; i < size; i++)
        newList[i] = list[i];
    delete[] list;
    list = newList;
    capacity = new_capacity;
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
const Patient* PatientArray::getData() const {
    return list;
}
void PatientArray::append(const PatientArray& other) {
    for (int i = 0; i < other.size; i++)
        addPatient(other.list[i]);
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
        getline(ss, p.care_type, ',');
        getline(ss, token, ',');
        p.length_of_staying = stoi(token);
        getline(ss, token, ',');
        p.base_cost_per_hour = stod(token);
        getline(ss, token, ',');
        p.day_visit_per_year = stoi(token);
        addPatient(p);
    }
    file.close();
    return true;
}
void PatientArray::printAll(int limit) const {
    cout << left
        << setw(12) << "PatientID"
        << setw(6) << "Age"
        << setw(18) << "Care Type"
        << setw(10) << "Stay Hrs"
        << setw(14) << "Cost Per Hr"
        << setw(12) << "Visits Yr"
        << endl;
    cout << string(72, '-') << endl;
    int count = (limit == -1 || limit > size) ? size : limit;
    for (int i = 0; i < count; i++) {
        cout << left
            << setw(12) << list[i].patientID
            << setw(6) << list[i].age
            << setw(18) << list[i].care_type
            << setw(10) << list[i].length_of_staying
            << setw(14) << list[i].base_cost_per_hour
            << setw(12) << list[i].day_visit_per_year
            << endl;
    }
    cout << endl << "Displayed " << count << " of " << size << " records.\n";
}
// SORTING (Section 6) 
// Bubble Sort
SortResult PatientArray::bubbleSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 }; // needs no extra memory
    auto start = high_resolution_clock::now();
    for (int i = 0; i < size - 1; i++) {
        bool swapped_this_pass = false;
        for (int j = 0; j < size - 1 - i; j++) {
            result.comparisons++;
            if (getSortValue(list[j], key) > getSortValue(list[j + 1], key)) {
                Patient temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
                result.swaps++;
                swapped_this_pass = true;
            }
        }
        if (!swapped_this_pass)
            break; // already sorted - stop early
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();

    return result;
}
// Quick Sort (middle-element pivot, Hoare-style partition)
// 'depth'/'max_depth' track recursion depth so stack memory can be estimated.
static void quickSortArray(Patient* values, int low, int high, SortKey key,
    SortResult& result, int depth, int& max_depth) {
    if (depth > max_depth) max_depth = depth;
    int i = low, j = high;
    double pivot = getSortValue(values[low + (high - low) / 2], key);
    while (i <= j) {
        while (true) {
            result.comparisons++;
            if (!(getSortValue(values[i], key) < pivot)) break;
            i++;
        }
        while (true) {
            result.comparisons++;
            if (!(getSortValue(values[j], key) > pivot)) break;
            j--;
        }
        if (i <= j) {
            if (i != j) {
                Patient temp = values[i]; values[i] = values[j]; values[j] = temp;
                result.swaps++;
            }
            i++; j--;
        }
    }
    if (low < j) quickSortArray(values, low, j, key, result, depth + 1, max_depth);
    if (i < high) quickSortArray(values, i, high, key, result, depth + 1, max_depth);
}
SortResult PatientArray::quickSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    int max_depth = 0;
    auto start = high_resolution_clock::now();
    if (size > 1) quickSortArray(list, 0, size - 1, key, result, 1, max_depth);
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.aux_bytes = (long long)max_depth * 64; // ~64 B of stack per recursive call
    return result;
}
// Radix Sort (counting sort per digit)
// Uses whole units for age/duration and cents for total cost.
static unsigned long long radixValue(const Patient& p, SortKey key) {
    if (key == sort_by_age) return (unsigned long long)p.age;
    if (key == sort_by_length_of_staying) return (unsigned long long)p.length_of_staying;
    return (unsigned long long)llround(getSortValue(p, key) * 100.0);
}
SortResult PatientArray::radixSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    auto start = high_resolution_clock::now();
    if (size > 1) {
        Patient* output = new Patient[size]; // full-size scratch copy of the data
        unsigned long long max_value = 0;
        for (int i = 0; i < size; i++)
            if (radixValue(list[i], key) > max_value) max_value = radixValue(list[i], key);
        for (unsigned long long place = 1; max_value / place > 0;) {
            int counts[10] = { 0 };
            for (int i = 0; i < size; i++) counts[(radixValue(list[i], key) / place) % 10]++;
            for (int i = 1; i < 10; i++) counts[i] += counts[i - 1];
            for (int i = size - 1; i >= 0; i--) {
                int digit = (int)((radixValue(list[i], key) / place) % 10);
                output[--counts[digit]] = list[i];
                result.swaps++; // counts element moves; radix makes no comparisons
            }
            for (int i = 0; i < size; i++) list[i] = output[i];
            if (place > max_value / 10) break;
            place *= 10;
        }
        delete[] output;
        result.aux_bytes = (long long)size * sizeof(Patient) + 10 * sizeof(int);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    return result;
}
// Merge Sort (top-down, needs a temporary array)
// Merging must copy elements into a scratch array (unlike the linked-list
// version, which just rewires pointers), so it costs O(n) extra memory.
static void mergeSortArray(Patient* values, Patient* temp, int low, int high,
    SortKey key, SortResult& result) {
    if (low >= high) return;
    int mid = low + (high - low) / 2;
    mergeSortArray(values, temp, low, mid, key, result);
    mergeSortArray(values, temp, mid + 1, high, key, result);
    int i = low, j = mid + 1, k = low;
    while (i <= mid && j <= high) {
        result.comparisons++;
        if (getSortValue(values[j], key) < getSortValue(values[i], key))
            temp[k++] = values[j++];
        else
            temp[k++] = values[i++]; // ties take the left element - stable
        result.swaps++; // counts an element move
    }
    while (i <= mid) { temp[k++] = values[i++]; result.swaps++; }
    while (j <= high) { temp[k++] = values[j++]; result.swaps++; }
    for (k = low; k <= high; k++) values[k] = temp[k];
}
SortResult PatientArray::mergeSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    auto start = high_resolution_clock::now();
    if (size > 1) {
        Patient* temp = new Patient[size];
        mergeSortArray(list, temp, 0, size - 1, key, result);
        delete[] temp;
        result.aux_bytes = (long long)size * sizeof(Patient);
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    return result;
}
//  SEARCHING (Section 7) 
// 'comparisons' counts criterion checks; 'steps' counts array elements
// examined or copied. 'aux_bytes' is the result buffer: the array must
// pre-allocate room for the WORST CASE (all records match), which is a
// real cost of using a fixed-size array for search results.
SearchResult PatientArray::searchByAgeRangeUnsorted(int min_age, int max_age,
    Patient*& out_matches, int& out_match_count) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_match_count = 0;
    out_matches = new Patient[size > 0 ? size : 1]; // worst case: all match
    result.aux_bytes = (long long)size * sizeof(Patient);
    auto start = high_resolution_clock::now();
    // Every element must be checked - matches could be anywhere.
    for (int i = 0; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].age >= min_age && list[i].age <= max_age) {
            out_matches[out_match_count] = list[i];
            out_match_count++;
        }
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_match_count;
    return result;
}
SearchResult PatientArray::searchByAgeRangeSorted(int min_age, int max_age,
    Patient*& out_matches, int& out_match_count) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_match_count = 0;
    out_matches = new Patient[size > 0 ? size : 1];
    result.aux_bytes = (long long)size * sizeof(Patient);
    auto start = high_resolution_clock::now();
    // Binary search for the FIRST index where age >= min_age.
    // Requires the array to already be sorted ascending by Age.
    int low = 0, high = size - 1, boundary = size; // default: no match
    while (low <= high) {
        int mid = (low + high) / 2;
        result.comparisons++;
        result.steps++;
        if (list[mid].age >= min_age) {
            boundary = mid;
            high = mid - 1; // keep looking left for an earlier match
        }
        else {
            low = mid + 1;
        }
    }
    // Matches are contiguous from 'boundary'; stop the instant we pass max_age.
    for (int i = boundary; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].age > max_age)
            break;
        out_matches[out_match_count] = list[i];
        out_match_count++;
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_match_count;
    return result;
}
SearchResult PatientArray::searchByCareType(const string& care_type,
    Patient*& out_matches, int& out_match_count) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_match_count = 0;
    out_matches = new Patient[size > 0 ? size : 1];
    result.aux_bytes = (long long)size * sizeof(Patient);
    auto start = high_resolution_clock::now();
    // Care Type has no natural numeric order, so this is always linear.
    for (int i = 0; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].care_type == care_type) {
            out_matches[out_match_count] = list[i];
            out_match_count++;
        }
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_match_count;
    return result;
}
SearchResult PatientArray::searchByDurationThresholdUnsorted(int min_hours,
    Patient*& out_matches, int& out_match_count) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_match_count = 0;
    out_matches = new Patient[size > 0 ? size : 1];
    result.aux_bytes = (long long)size * sizeof(Patient);
    auto start = high_resolution_clock::now();
    for (int i = 0; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].length_of_staying > min_hours) {
            out_matches[out_match_count] = list[i];
            out_match_count++;
        }
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_match_count;
    return result;
}

SearchResult PatientArray::searchByDurationThresholdSorted(int min_hours,
    Patient*& out_matches, int& out_match_count) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    out_match_count = 0;
    out_matches = new Patient[size > 0 ? size : 1];
    result.aux_bytes = (long long)size * sizeof(Patient);
    auto start = high_resolution_clock::now();
    // Binary search for the FIRST index where length_of_staying > min_hours.
    // Requires the array to already be sorted ascending by LengthOfStay.
    int low = 0, high = size - 1, boundary = size;
    while (low <= high) {
        int mid = (low + high) / 2;
        result.comparisons++;
        result.steps++;
        if (list[mid].length_of_staying > min_hours) {
            boundary = mid;
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }
    // Everything from 'boundary' to the end qualifies - no more comparisons,
    // only copying (each copied element is one step).
    for (int i = boundary; i < size; i++) {
        result.steps++;
        out_matches[out_match_count] = list[i];
        out_match_count++;
    }
    auto end = high_resolution_clock::now();
    result.ms_time = duration<double, std::milli>(end - start).count();
    result.match_count = out_match_count;
    return result;
}
//  PERFORMANCE ANALYSIS (Section 8) 
long long PatientArray::getMemoryUsageBytes() const {
    // Uses 'capacity' (allocated), not 'size' (used): unused slots left over
    // from the doubling strategy are still real memory sitting idle.
    return (long long)capacity * sizeof(Patient) + sizeof(*this);
}