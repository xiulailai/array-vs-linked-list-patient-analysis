// ============================================================
// Test_Array.cpp
// CT077-3-2-DSTR  |  Program 1: ARRAY Implementation
// Entry point / test driver only - all logic lives in
// PatientArray, AgeGroupAnalyzer, and BillingSummary.
//
// Sections covered:
//   4/5) Age-group + billing analysis, cross-dataset/age-group comparisons
//   6)   Sorting performance: Tier 1 (Array vs Array, across sort keys)
//   7)   Searching performance: Tier 1 (Array vs Array, unsorted vs sorted)
// ============================================================
#include "PatientArray.hpp"
#include "AgeGroupAnalyzer.hpp"
#include "BillingSummary.hpp"
#include "SortKey.hpp"
#include "SearchResult.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Small helper to preview a plain array of search-result matches
void printMatches(const Patient* matches, int count, int limit) {
    int shown = (limit == -1 || limit > count) ? count : limit;
    cout << left
        << setw(12) << "PatientID"
        << setw(6) << "Age"
        << setw(18) << "CareType"
        << setw(10) << "StayHrs"
        << setw(14) << "CostPerHr"
        << setw(12) << "VisitsYr"
        << endl;
    cout << string(72, '-') << endl;
    for (int i = 0; i < shown; i++) {
        cout << left
            << setw(12) << matches[i].patientID
            << setw(6) << matches[i].age
            << setw(18) << matches[i].careType
            << setw(10) << matches[i].lengthOfStay
            << setw(14) << matches[i].baseCostPerHour
            << setw(12) << matches[i].daysVisitsPerYear
            << endl;
    }
    cout << "Showing " << shown << " of " << count << " matches.\n";
}

int main() {
    const int NUM_DATASETS = 3;

    string filenames[NUM_DATASETS] = {
        "../../Data/dataset1_facility_a.csv",
        "../../Data/dataset2_facility_b.csv",
        "../../Data/dataset3_facility_c.csv"
    };

    string labels[NUM_DATASETS] = {
        "Facility A - General Hospital",
        "Facility B - University Medical Center",
        "Facility C - Community Health Clinic"
    };

    string shortLabels[NUM_DATASETS] = { "Facility A", "Facility B", "Facility C" };

    // Store results per dataset for the final comparison tables
    double datasetTotalCost[NUM_DATASETS];
    int datasetTotalPatients[NUM_DATASETS];
    double datasetAvgStay[NUM_DATASETS];

    AgeGroupAnalyzer ageAnalyzers[NUM_DATASETS];

    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "\n==================================================\n";
        cout << " PROCESSING: " << labels[d] << "\n";
        cout << "==================================================\n";

        PatientArray facility;

        if (!facility.loadFromCSV(filenames[d])) {
            cout << "Skipping " << labels[d] << " due to load error.\n";
            datasetTotalCost[d] = 0.0;
            datasetTotalPatients[d] = 0;
            datasetAvgStay[d] = 0.0;
            continue;
        }

        cout << "Loaded " << facility.getSize() << " patient records.\n";

        int patientCount;
        Patient* allPatients = facility.toArray(patientCount);

        ageAnalyzers[d].analyze(allPatients, patientCount);
        cout << "\n--- Age Group & Cost Analysis ---\n";
        ageAnalyzers[d].printReport();

        BillingSummary billing;
        billing.analyze(allPatients, patientCount);
        billing.printReport(labels[d]);

        datasetTotalCost[d] = billing.getTotalCost();
        datasetTotalPatients[d] = billing.getTotalPatients();
        datasetAvgStay[d] = billing.getAverageLengthOfStay();

        delete[] allPatients;
    }

    // ---------- Cross-Dataset Comparison Summary ----------
    cout << "\n\n==================================================\n";
    cout << " CROSS-DATASET COMPARISON SUMMARY\n";
    cout << "==================================================\n";
    cout << left
        << setw(40) << "Facility"
        << setw(12) << "Patients"
        << setw(16) << "Total Cost"
        << setw(16) << "Avg Stay(hrs)"
        << endl;
    cout << string(84, '-') << endl;

    cout << fixed << setprecision(2);
    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << left
            << setw(40) << labels[d]
            << setw(12) << datasetTotalPatients[d]
            << setw(16) << datasetTotalCost[d]
            << setw(16) << datasetAvgStay[d]
            << endl;
    }

    int numGroups = AgeGroupAnalyzer::getGroupCount();

    // ---------- Age Group Expenditure Comparison Across Facilities ----------
    cout << "\n\n==================================================\n";
    cout << " AGE GROUP EXPENDITURE COMPARISON ACROSS FACILITIES\n";
    cout << " (Total Cost $ per Age Group, per Facility)\n";
    cout << "==================================================\n";

    cout << left << setw(40) << "Age Group";
    for (int d = 0; d < NUM_DATASETS; d++)
        cout << setw(14) << shortLabels[d];
    cout << endl;
    cout << string(40 + 14 * NUM_DATASETS, '-') << endl;

    for (int g = 0; g < numGroups; g++) {
        string label = ageAnalyzers[0].getGroupLabel(g) + " ("
            + ageAnalyzers[0].getGroupDescription(g) + ")";
        cout << left << setw(40) << label;
        for (int d = 0; d < NUM_DATASETS; d++)
            cout << setw(14) << ageAnalyzers[d].getGroupTotalCost(g);
        cout << endl;
    }

    // ---------- Age Group Visit Duration Comparison Across Facilities ----------
    cout << "\n\n==================================================\n";
    cout << " AGE GROUP VISIT DURATION COMPARISON ACROSS FACILITIES\n";
    cout << " (Average Length of Stay in hours, per Age Group, per Facility)\n";
    cout << "==================================================\n";

    cout << left << setw(40) << "Age Group";
    for (int d = 0; d < NUM_DATASETS; d++)
        cout << setw(14) << shortLabels[d];
    cout << endl;
    cout << string(40 + 14 * NUM_DATASETS, '-') << endl;

    for (int g = 0; g < numGroups; g++) {
        string label = ageAnalyzers[0].getGroupLabel(g) + " ("
            + ageAnalyzers[0].getGroupDescription(g) + ")";
        cout << left << setw(40) << label;
        for (int d = 0; d < NUM_DATASETS; d++)
            cout << setw(14) << ageAnalyzers[d].getGroupAverageLengthOfStay(g);
        cout << endl;
    }

    // ==================================================================
    // SECTION 6: SORTING PERFORMANCE ANALYSIS
    // Tier 1 - Array vs Array: same structure, compared across sort keys
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " SORTING PERFORMANCE ANALYSIS - ARRAY\n";
    cout << " Tier 1: comparing sort keys WITHIN the array structure\n";
    cout << "==================================================\n";

    SortKey keys[3] = { SORT_BY_AGE, SORT_BY_LENGTH_OF_STAY, SORT_BY_TOTAL_COST };

    ofstream sortResultsFile("../../Data/Array_SortResults.csv");
    sortResultsFile << "Facility,Algorithm,SortKey,Comparisons,Swaps,TimeMs\n";

    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "\n--- " << labels[d] << " ---\n";
        cout << left
            << setw(18) << "Algorithm"
            << setw(20) << "Sort Key"
            << setw(16) << "Comparisons"
            << setw(12) << "Swaps"
            << setw(14) << "Time (ms)"
            << endl;
        cout << string(80, '-') << endl;

        for (int a = 0; a < 3; a++) for (int k = 0; k < 3; k++) {
            PatientArray facility;
            facility.loadFromCSV(filenames[d]);

            SortResult result;
            if (a == 0) result = facility.bubbleSort(keys[k]);
            else if (a == 1) result = facility.quickSort(keys[k]);
            else result = facility.radixSort(keys[k]);
            string algorithm = (a == 0) ? "Bubble Sort" : (a == 1) ? "Quick Sort" : "Radix Sort";

            cout << left
                << setw(18) << algorithm
                << setw(20) << sortKeyLabel(keys[k])
                << setw(16) << result.comparisons
                << setw(12) << result.swaps
                << setw(14) << fixed << setprecision(4) << result.timeMs
                << endl;

            if (sortResultsFile.is_open()) {
                sortResultsFile << shortLabels[d] << "," << algorithm << ","
                    << sortKeyLabel(keys[k]) << ","
                    << result.comparisons << ","
                    << result.swaps << ","
                    << fixed << setprecision(4) << result.timeMs << "\n";
            }
        }
    }
    sortResultsFile.close();

    cout << "\n--- Correctness Check: Facility A sorted by Age (first 5 rows) ---\n";
    PatientArray sortCheckFacility;
    sortCheckFacility.loadFromCSV(filenames[0]);
    sortCheckFacility.bubbleSort(SORT_BY_AGE);
    sortCheckFacility.printAll(5);

    // ==================================================================
    // SECTION 7: SEARCHING PERFORMANCE ANALYSIS
    // Tier 1 - Array vs Array: unsorted vs sorted search performance
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " SEARCHING PERFORMANCE ANALYSIS - ARRAY\n";
    cout << " Tier 1: unsorted vs sorted search, within the array structure\n";
    cout << "==================================================\n";

    const int AGE_MIN = 61, AGE_MAX = 100;
    const string SEARCH_CARE_TYPE = "Emergency";
    const int DURATION_THRESHOLD = 24;

    ofstream searchResultsFile("../../Data/Array_SearchResults.csv");
    searchResultsFile << "Facility,SearchType,Mode,Comparisons,Matches,TimeMs\n";

    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "\n--- " << labels[d] << " ---\n";
        cout << left
            << setw(24) << "Search Type"
            << setw(10) << "Mode"
            << setw(14) << "Comparisons"
            << setw(10) << "Matches"
            << setw(12) << "Time (ms)"
            << endl;
        cout << string(70, '-') << endl;

        // Local helper: prints one row and logs it to the CSV, reused
        // for every search test below so the code isn't repeated 5 times.
        auto logSearch = [&](const string& displayLabel, const string& csvLabel,
            const string& mode, const SearchResult& r) {
                cout << left << setw(24) << displayLabel << setw(10) << mode
                    << setw(14) << r.comparisons << setw(10) << r.matchCount
                    << setw(12) << fixed << setprecision(4) << r.timeMs
                    << endl;
                searchResultsFile << shortLabels[d] << "," << csvLabel << "," << mode << ","
                    << r.comparisons << "," << r.matchCount << ","
                    << fixed << setprecision(4) << r.timeMs << "\n";
            };

        // ---- Age Range Search: Unsorted ----
        {
            PatientArray facility;
            facility.loadFromCSV(filenames[d]);
            Patient* matches; int matchCount;
            SearchResult r = facility.searchByAgeRangeUnsorted(AGE_MIN, AGE_MAX, matches, matchCount);
            logSearch("Age Range (61-100)", "AgeRange", "Unsorted", r);
            delete[] matches;
        }

        // ---- Age Range Search: Sorted (binary search boundary) ----
        {
            PatientArray facility;
            facility.loadFromCSV(filenames[d]);
            facility.bubbleSort(SORT_BY_AGE); // must sort by Age first
            Patient* matches; int matchCount;
            SearchResult r = facility.searchByAgeRangeSorted(AGE_MIN, AGE_MAX, matches, matchCount);
            logSearch("Age Range (61-100)", "AgeRange", "Sorted", r);
            delete[] matches;
        }

        // ---- Care Type Search (always linear - categorical field) ----
        {
            PatientArray facility;
            facility.loadFromCSV(filenames[d]);
            Patient* matches; int matchCount;
            SearchResult r = facility.searchByCareType(SEARCH_CARE_TYPE, matches, matchCount);
            logSearch("CareType=Emergency", "CareType", "Linear", r);
            delete[] matches;
        }

        // ---- Duration Threshold Search: Unsorted ----
        {
            PatientArray facility;
            facility.loadFromCSV(filenames[d]);
            Patient* matches; int matchCount;
            SearchResult r = facility.searchByDurationThresholdUnsorted(DURATION_THRESHOLD, matches, matchCount);
            logSearch("Duration > 24h", "Duration", "Unsorted", r);
            delete[] matches;
        }

        // ---- Duration Threshold Search: Sorted (binary search boundary) ----
        {
            PatientArray facility;
            facility.loadFromCSV(filenames[d]);
            facility.bubbleSort(SORT_BY_LENGTH_OF_STAY);
            Patient* matches; int matchCount;
            SearchResult r = facility.searchByDurationThresholdSorted(DURATION_THRESHOLD, matches, matchCount);
            logSearch("Duration > 24h", "Duration", "Sorted", r);
            delete[] matches;
        }
    }
    searchResultsFile.close();

    // Correctness check: preview actual matches for one search
    cout << "\n--- Correctness Check: Facility A, Age Range 61-100 (Sorted search) ---\n";
    PatientArray searchCheckFacility;
    searchCheckFacility.loadFromCSV(filenames[0]);
    searchCheckFacility.bubbleSort(SORT_BY_AGE);
    Patient* checkMatches; int checkMatchCount;
    searchCheckFacility.searchByAgeRangeSorted(AGE_MIN, AGE_MAX, checkMatches, checkMatchCount);
    printMatches(checkMatches, checkMatchCount, 5);
    delete[] checkMatches;

    // ==================================================================
    // SECTION 8: PERFORMANCE ANALYSIS - MEMORY FOOTPRINT
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " MEMORY FOOTPRINT ANALYSIS - ARRAY\n";
    cout << "==================================================\n";

    cout << left
        << setw(40) << "Facility"
        << setw(12) << "Patients"
        << setw(18) << "Memory (bytes)"
        << setw(18) << "Bytes/Patient"
        << endl;
    cout << string(88, '-') << endl;

    ofstream memoryResultsFile("../../Data/Array_MemoryUsage.csv");
    memoryResultsFile << "Facility,Patients,MemoryBytes,BytesPerPatient\n";

    for (int d = 0; d < NUM_DATASETS; d++) {
        PatientArray facility;
        facility.loadFromCSV(filenames[d]);

        long long memBytes = facility.getMemoryUsageBytes();
        int patients = facility.getSize();
        double bytesPerPatient = (patients > 0) ? (double)memBytes / patients : 0.0;

        cout << left
            << setw(40) << labels[d]
            << setw(12) << patients
            << setw(18) << memBytes
            << setw(18) << fixed << setprecision(2) << bytesPerPatient
            << endl;

        memoryResultsFile << shortLabels[d] << "," << patients << "," << memBytes << ","
            << fixed << setprecision(2) << bytesPerPatient << "\n";
    }
    memoryResultsFile.close();

    cout << "\nNote: memory shown uses ALLOCATED CAPACITY, not just used size -\n";
    cout << "this includes any unused slots left over from the array's doubling\n";
    cout << "resize strategy (see PatientArray::getMemoryUsageBytes comments).\n";

    // ==================================================================
    // SECTION 9: CLINICAL INSIGHTS - METROHEALTH SYSTEM-WIDE SUMMARY
    // Combines all 3 facilities into one network-wide view to directly
    // answer: "which demographic group generates the highest billing"
    // and "which CareType has the highest patient traffic" (assignment
    // brief, Section 9). The rest of Section 9 (recommendations) is
    // written up in the Word documentation using these numbers.
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " CLINICAL INSIGHTS - METROHEALTH SYSTEM-WIDE SUMMARY\n";
    cout << "==================================================\n";

    // Reload and combine all 3 facilities into one array
    PatientArray combinedFacilities[NUM_DATASETS];
    int totalCombinedCount = 0;
    for (int d = 0; d < NUM_DATASETS; d++) {
        combinedFacilities[d].loadFromCSV(filenames[d]);
        totalCombinedCount += combinedFacilities[d].getSize();
    }

    Patient* combinedPatients = new Patient[totalCombinedCount > 0 ? totalCombinedCount : 1];
    int insertPos = 0;
    for (int d = 0; d < NUM_DATASETS; d++) {
        int countD;
        Patient* arrD = combinedFacilities[d].toArray(countD);
        for (int i = 0; i < countD; i++)
            combinedPatients[insertPos++] = arrD[i];
        delete[] arrD;
    }

    cout << "\nTotal patients across MetroHealth System: " << totalCombinedCount << "\n";

    // ---- Highest-billing age group, network-wide ----
    AgeGroupAnalyzer networkAgeAnalyzer;
    networkAgeAnalyzer.analyze(combinedPatients, totalCombinedCount);

    int highestBillingGroup = 0;
    for (int g = 1; g < numGroups; g++) {
        if (networkAgeAnalyzer.getGroupTotalCost(g) > networkAgeAnalyzer.getGroupTotalCost(highestBillingGroup))
            highestBillingGroup = g;
    }

    cout << fixed << setprecision(2);
    cout << "\nHighest-billing age group (network-wide): "
        << networkAgeAnalyzer.getGroupLabel(highestBillingGroup) << " ("
        << networkAgeAnalyzer.getGroupDescription(highestBillingGroup) << ")\n";
    cout << "  -> Total Cost: $" << networkAgeAnalyzer.getGroupTotalCost(highestBillingGroup) << "\n";
    cout << "  -> Patients: " << networkAgeAnalyzer.getGroupPatientCount(highestBillingGroup) << "\n";

    // ---- Highest-traffic CareType, network-wide ----
    BillingSummary networkBilling;
    networkBilling.analyze(combinedPatients, totalCombinedCount);

    int highestTrafficIndex = 0;
    for (int i = 1; i < networkBilling.getCareTypeCount(); i++) {
        if (networkBilling.getCareTypePatientCount(i) > networkBilling.getCareTypePatientCount(highestTrafficIndex))
            highestTrafficIndex = i;
    }

    cout << "\nHighest-traffic Care Type (network-wide): "
        << networkBilling.getCareTypeLabel(highestTrafficIndex)
        << " (" << networkBilling.getCareTypePatientCount(highestTrafficIndex) << " patients, $"
        << networkBilling.getCareTypeTotalCost(highestTrafficIndex) << " total cost)\n";

    cout << "\n--- Full Network-Wide Billing Summary (All Facilities Combined) ---\n";
    networkBilling.printReport("MetroHealth System (All Facilities)");

    delete[] combinedPatients;

    return 0;
}