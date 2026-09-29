// ============================================================
// PatientArray.cpp
// CT077-3-2-DSTR  |  Program 1: ARRAY Implementation
// Implementation of the array-based patient collection.
// ============================================================
#include "PatientArray.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
using namespace std;
using namespace std::chrono;

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

// ==================== SORTING (Section 6) ====================

// ---------- Bubble Sort ----------
SortResult PatientArray::bubbleSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 }; // needs no extra memory

    auto start = high_resolution_clock::now();

    for (int i = 0; i < size - 1; i++) {
        bool swappedThisPass = false;

        for (int j = 0; j < size - 1 - i; j++) {
            result.comparisons++;

            if (getSortValue(list[j], key) > getSortValue(list[j + 1], key)) {
                Patient temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
                result.swaps++;
                swappedThisPass = true;
            }
        }

        if (!swappedThisPass)
            break; // already sorted - stop early
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();

    return result;
}

// ---------- Quick Sort (middle-element pivot, Hoare-style partition) ----------
// 'depth'/'maxDepth' track recursion depth so stack memory can be estimated.
static void quickSortArray(Patient* values, int low, int high, SortKey key,
    SortResult& result, int depth, int& maxDepth) {
    if (depth > maxDepth) maxDepth = depth;
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
    if (low < j) quickSortArray(values, low, j, key, result, depth + 1, maxDepth);
    if (i < high) quickSortArray(values, i, high, key, result, depth + 1, maxDepth);
}

SortResult PatientArray::quickSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    int maxDepth = 0;
    auto start = high_resolution_clock::now();
    if (size > 1) quickSortArray(list, 0, size - 1, key, result, 1, maxDepth);
    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.auxBytes = (long long)maxDepth * 64; // ~64 B of stack per recursive call
    return result;
}

// ---------- Radix Sort (LSD, counting sort per digit) ----------
// Uses whole units for age/duration and cents for total cost.
static unsigned long long radixValue(const Patient& p, SortKey key) {
    if (key == SORT_BY_AGE) return (unsigned long long)p.age;
    if (key == SORT_BY_LENGTH_OF_STAY) return (unsigned long long)p.lengthOfStay;
    return (unsigned long long)llround(getSortValue(p, key) * 100.0);
}

SortResult PatientArray::radixSort(SortKey key) {
    SortResult result = { 0, 0, 0.0, 0 };
    auto start = high_resolution_clock::now();
    if (size > 1) {
        Patient* output = new Patient[size]; // full-size scratch copy of the data
        unsigned long long maxValue = 0;
        for (int i = 0; i < size; i++)
            if (radixValue(list[i], key) > maxValue) maxValue = radixValue(list[i], key);
        for (unsigned long long place = 1; maxValue / place > 0;) {
            int counts[10] = { 0 };
            for (int i = 0; i < size; i++) counts[(radixValue(list[i], key) / place) % 10]++;
            for (int i = 1; i < 10; i++) counts[i] += counts[i - 1];
            for (int i = size - 1; i >= 0; i--) {
                int digit = (int)((radixValue(list[i], key) / place) % 10);
                output[--counts[digit]] = list[i];
                result.swaps++; // counts element moves; radix makes no comparisons
            }
            for (int i = 0; i < size; i++) list[i] = output[i];
            if (place > maxValue / 10) break;
            place *= 10;
        }
        delete[] output;
        result.auxBytes = (long long)size * sizeof(Patient) + 10 * sizeof(int);
    }
    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    return result;
}

// ---------- Merge Sort (top-down, needs a temporary array) ----------
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
        result.auxBytes = (long long)size * sizeof(Patient);
    }
    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    return result;
}

// ==================== SEARCHING (Section 7) ====================
// 'comparisons' counts criterion checks; 'steps' counts array elements
// examined or copied. 'auxBytes' is the result buffer: the array must
// pre-allocate room for the WORST CASE (all records match), which is a
// real cost of using a fixed-size array for search results.

SearchResult PatientArray::searchByAgeRangeUnsorted(int minAge, int maxAge,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1]; // worst case: all match
    result.auxBytes = (long long)size * sizeof(Patient);

    auto start = high_resolution_clock::now();

    // Every element must be checked - matches could be anywhere.
    for (int i = 0; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].age >= minAge && list[i].age <= maxAge) {
            outMatches[outMatchCount] = list[i];
            outMatchCount++;
        }
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;
    return result;
}

SearchResult PatientArray::searchByAgeRangeSorted(int minAge, int maxAge,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];
    result.auxBytes = (long long)size * sizeof(Patient);

    auto start = high_resolution_clock::now();

    // Binary search for the FIRST index where age >= minAge.
    // Requires the array to already be sorted ascending by Age.
    int low = 0, high = size - 1, boundary = size; // default: no match
    while (low <= high) {
        int mid = (low + high) / 2;
        result.comparisons++;
        result.steps++;
        if (list[mid].age >= minAge) {
            boundary = mid;
            high = mid - 1; // keep looking left for an earlier match
        }
        else {
            low = mid + 1;
        }
    }

    // Matches are contiguous from 'boundary'; stop the instant we pass maxAge.
    for (int i = boundary; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].age > maxAge)
            break;
        outMatches[outMatchCount] = list[i];
        outMatchCount++;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;
    return result;
}

SearchResult PatientArray::searchByCareType(const string& careType,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];
    result.auxBytes = (long long)size * sizeof(Patient);

    auto start = high_resolution_clock::now();

    // CareType has no natural numeric order, so this is always linear.
    for (int i = 0; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].careType == careType) {
            outMatches[outMatchCount] = list[i];
            outMatchCount++;
        }
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;
    return result;
}

SearchResult PatientArray::searchByDurationThresholdUnsorted(int minHours,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];
    result.auxBytes = (long long)size * sizeof(Patient);

    auto start = high_resolution_clock::now();

    for (int i = 0; i < size; i++) {
        result.steps++;
        result.comparisons++;
        if (list[i].lengthOfStay > minHours) {
            outMatches[outMatchCount] = list[i];
            outMatchCount++;
        }
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;
    return result;
}

SearchResult PatientArray::searchByDurationThresholdSorted(int minHours,
    Patient*& outMatches, int& outMatchCount) const {
    SearchResult result = { 0, 0, 0, 0.0, 0 };
    outMatchCount = 0;

    outMatches = new Patient[size > 0 ? size : 1];
    result.auxBytes = (long long)size * sizeof(Patient);

    auto start = high_resolution_clock::now();

    // Binary search for the FIRST index where lengthOfStay > minHours.
    // Requires the array to already be sorted ascending by LengthOfStay.
    int low = 0, high = size - 1, boundary = size;
    while (low <= high) {
        int mid = (low + high) / 2;
        result.comparisons++;
        result.steps++;
        if (list[mid].lengthOfStay > minHours) {
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
        outMatches[outMatchCount] = list[i];
        outMatchCount++;
    }

    auto end = high_resolution_clock::now();
    result.timeMs = duration<double, std::milli>(end - start).count();
    result.matchCount = outMatchCount;
    return result;
}

// ==================== PERFORMANCE ANALYSIS (Section 8) ====================
long long PatientArray::getMemoryUsageBytes() const {
    // Uses 'capacity' (allocated), not 'size' (used): unused slots left over
    // from the doubling strategy are still real memory sitting idle.
    return (long long)capacity * sizeof(Patient) + sizeof(*this);
}