// ============================================================
// Test_LinkedList.cpp
// CT077-3-2-DSTR  |  Program 2: SINGLY LINKED LIST Implementation
// Entry point / test driver only - all logic lives in
// PatientLinkedList, AgeGroupAnalyzer, and BillingSummary.
//
// Sections covered:
//   4/5) Age-group + billing analysis, cross-dataset/age-group comparisons
//   6)   Sorting performance (Bubble / Quick / Radix, across sort keys)
//   7)   Searching performance (unsorted vs sorted)
//   8)   Memory footprint
//   9)   Clinical insights (network-wide + facility x age-group table)
// Timings are the AVERAGE of RUNS repetitions, because a single run on
// 200 records takes only microseconds and is dominated by noise.
// ============================================================
#include "PatientLinkedList.hpp"
#include "AgeGroupAnalyzer.hpp"
#include "BillingSummary.hpp"
#include "SortKey.hpp"
#include "SearchResult.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Finds the Data folder no matter where the program is launched from
// (Visual Studio uses the project folder as the working directory, while
// a terminal may run from the Code folder), so no path edits are needed.
static string findDataDir() {
    const string candidates[] = { "../../Data/", "../Data/", "Data/", "./" };
    for (const string& c : candidates) {
        ifstream probe(c + "dataset1_facility_a.csv");
        if (probe.is_open()) return c;
    }
    return "../../Data/"; // not found: main() reports the problem below
}

static const int NUM_DATASETS = 3;
static const int RUNS = 100;

// Runs a search RUNS times; returns the last result with timeMs replaced
// by the average. (Comparisons / steps / matches are identical every run.)
template <typename F>
SearchResult averageSearch(F searchFn, int runs) {
    SearchResult last = { 0, 0, 0, 0.0, 0 };
    double totalMs = 0.0;
    for (int i = 0; i < runs; i++) {
        last = searchFn();
        totalMs += last.timeMs;
    }
    last.timeMs = totalMs / runs;
    return last;
}

int main() {
    const string dataDir = findDataDir();
    string filenames[NUM_DATASETS] = {
        dataDir + "dataset1_facility_a.csv",
        dataDir + "dataset2_facility_b.csv",
        dataDir + "dataset3_facility_c.csv"
    };

    string labels[NUM_DATASETS] = {
        "Facility A - General Hospital",
        "Facility B - University Medical Center",
        "Facility C - Community Health Clinic"
    };

    string shortLabels[NUM_DATASETS] = { "Facility A", "Facility B", "Facility C" };

    PatientLinkedList masters[NUM_DATASETS]; // one loaded list per facility
    AgeGroupAnalyzer ageAnalyzers[NUM_DATASETS];

    double datasetTotalCost[NUM_DATASETS];
    int datasetTotalPatients[NUM_DATASETS];
    double datasetAvgStay[NUM_DATASETS];

    // ==================================================================
    // SECTIONS 4 & 5: AGE GROUP + BILLING ANALYSIS
    // ==================================================================
    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "\n==================================================\n";
        cout << " PROCESSING: " << labels[d] << "\n";
        cout << "==================================================\n";

        datasetTotalCost[d] = 0.0;
        datasetTotalPatients[d] = 0;
        datasetAvgStay[d] = 0.0;

        if (!masters[d].loadFromCSV(filenames[d])) {
            cout << "Skipping " << labels[d] << " due to load error.\n";
            continue;
        }

        cout << "Loaded " << masters[d].getSize() << " patient records.\n";

        ageAnalyzers[d].analyze(masters[d].getHead());
        cout << "\n--- Age Group & Cost Analysis ---\n";
        ageAnalyzers[d].printReport();

        BillingSummary billing;
        billing.analyze(masters[d].getHead());
        billing.printReport(labels[d]);

        datasetTotalCost[d] = billing.getTotalCost();
        datasetTotalPatients[d] = billing.getTotalPatients();
        datasetAvgStay[d] = billing.getAverageLengthOfStay();
    }

    if (masters[0].getSize() + masters[1].getSize() + masters[2].getSize() == 0) {
        cout << "\nERROR: no dataset could be loaded from \"" << dataDir << "\".\n";
        cout << "Put dataset1_facility_a.csv, dataset2_facility_b.csv and\n";
        cout << "dataset3_facility_c.csv in a folder named Data next to your\n";
        cout << "project folder (or set Project > Properties > Debugging >\n";
        cout << "Working Directory) and run again.\n";
        return 1;
    }

    // ---------- Cross-Dataset Comparison Summary ----------
    cout << "\n\n==================================================\n";
    cout << " CROSS-DATASET COMPARISON SUMMARY\n";
    cout << "==================================================\n";
    cout << left
        << setw(40) << "Facility"
        << setw(12) << "Patients"
        << setw(18) << "Total Cost (RM)"
        << setw(16) << "Avg Stay(hrs)"
        << endl;
    cout << string(86, '-') << endl;

    cout << fixed << setprecision(2);
    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << left
            << setw(40) << labels[d]
            << setw(12) << datasetTotalPatients[d]
            << setw(18) << datasetTotalCost[d]
            << setw(16) << datasetAvgStay[d]
            << endl;
    }

    int numGroups = AgeGroupAnalyzer::getGroupCount();

    // ---------- Age Group Expenditure Comparison ----------
    cout << "\n\n==================================================\n";
    cout << " AGE GROUP EXPENDITURE COMPARISON ACROSS FACILITIES\n";
    cout << " (Total Cost RM per Age Group, per Facility)\n";
    cout << "==================================================\n";

    cout << left << setw(46) << "Age Group";
    for (int d = 0; d < NUM_DATASETS; d++)
        cout << setw(16) << shortLabels[d];
    cout << endl;
    cout << string(46 + 16 * NUM_DATASETS, '-') << endl;

    cout << fixed << setprecision(2);
    for (int g = 0; g < numGroups; g++) {
        string label = ageAnalyzers[0].getGroupLabel(g) + " ("
            + ageAnalyzers[0].getGroupDescription(g) + ")";
        cout << left << setw(46) << label;
        for (int d = 0; d < NUM_DATASETS; d++)
            cout << setw(16) << ageAnalyzers[d].getGroupTotalCost(g);
        cout << endl;
    }

    // ---------- Age Group Visit Duration Comparison ----------
    cout << "\n\n==================================================\n";
    cout << " AGE GROUP VISIT DURATION COMPARISON ACROSS FACILITIES\n";
    cout << " (Average Length of Stay in hours, per Age Group, per Facility)\n";
    cout << "==================================================\n";

    cout << left << setw(46) << "Age Group";
    for (int d = 0; d < NUM_DATASETS; d++)
        cout << setw(16) << shortLabels[d];
    cout << endl;
    cout << string(46 + 16 * NUM_DATASETS, '-') << endl;

    for (int g = 0; g < numGroups; g++) {
        string label = ageAnalyzers[0].getGroupLabel(g) + " ("
            + ageAnalyzers[0].getGroupDescription(g) + ")";
        cout << left << setw(46) << label;
        for (int d = 0; d < NUM_DATASETS; d++)
            cout << setw(16) << ageAnalyzers[d].getGroupAverageLengthOfStay(g);
        cout << endl;
    }

    // ==================================================================
    // SECTION 6: SORTING PERFORMANCE ANALYSIS
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " SORTING PERFORMANCE ANALYSIS - LINKED LIST\n";
    cout << " (time = average of " << RUNS << " runs; Swaps for Radix/Merge = node relinks)\n";
    cout << "==================================================\n";

    SortKey keys[3] = { SORT_BY_AGE, SORT_BY_LENGTH_OF_STAY, SORT_BY_TOTAL_COST };
    string algorithms[4] = { "Bubble Sort", "Quick Sort", "Radix Sort", "Merge Sort" };

    ofstream sortResultsFile(dataDir + "LinkedList_SortResults.csv");
    sortResultsFile << "Facility,Algorithm,SortKey,Comparisons,Swaps,AvgTimeMs,AuxBytes\n";

    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "\n--- " << labels[d] << " ---\n";
        cout << left
            << setw(16) << "Algorithm"
            << setw(18) << "Sort Key"
            << setw(14) << "Comparisons"
            << setw(10) << "Swaps"
            << setw(16) << "Avg Time (ms)"
            << setw(14) << "Aux Mem (B)"
            << endl;
        cout << string(88, '-') << endl;

        for (int a = 0; a < 4; a++) {
            for (int k = 0; k < 3; k++) {
                SortResult last = { 0, 0, 0.0, 0 };
                double totalMs = 0.0;

                for (int run = 0; run < RUNS; run++) {
                    PatientLinkedList work;
                    work.append(masters[d]); // fresh unsorted copy (not timed)

                    SortResult r;
                    if (a == 0) r = work.bubbleSort(keys[k]);
                    else if (a == 1) r = work.quickSort(keys[k]);
                    else if (a == 2) r = work.radixSort(keys[k]);
                    else r = work.mergeSort(keys[k]);

                    totalMs += r.timeMs;
                    last = r;
                }
                double avgMs = totalMs / RUNS;

                cout << left
                    << setw(16) << algorithms[a]
                    << setw(18) << sortKeyLabel(keys[k])
                    << setw(14) << last.comparisons
                    << setw(10) << last.swaps
                    << setw(16) << fixed << setprecision(4) << avgMs
                    << setw(14) << last.auxBytes
                    << endl;

                sortResultsFile << shortLabels[d] << "," << algorithms[a] << ","
                    << sortKeyLabel(keys[k]) << ","
                    << last.comparisons << "," << last.swaps << ","
                    << fixed << setprecision(4) << avgMs << ","
                    << last.auxBytes << "\n";
            }
        }
    }
    sortResultsFile.close();

    cout << "\n--- Correctness Check: Facility A sorted by Age (first 5 rows) ---\n";
    {
        PatientLinkedList check;
        check.append(masters[0]);
        check.bubbleSort(SORT_BY_AGE);
        check.printAll(5);
    }

    // ==================================================================
    // SECTION 7: SEARCHING PERFORMANCE ANALYSIS
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " SEARCHING PERFORMANCE ANALYSIS - LINKED LIST\n";
    cout << " (time = average of " << RUNS << " runs; Steps = nodes visited)\n";
    cout << "==================================================\n";

    // Search criteria are chosen PER FACILITY so every search returns real
    // matches (Facility B has only young adults, short stays, no seniors).
    const int ageMins[NUM_DATASETS] = { 61, 18, 61 };
    const int ageMaxs[NUM_DATASETS] = { 100, 25, 100 };
    const string careTypesToFind[NUM_DATASETS] = { "Emergency", "Rehabilitation", "Outpatient" };
    const int durationLimits[NUM_DATASETS] = { 24, 2, 24 };

    ofstream searchResultsFile(dataDir + "LinkedList_SearchResults.csv");
    searchResultsFile << "Facility,SearchType,Mode,Comparisons,Steps,Matches,AvgTimeMs,AuxBytes\n";

    for (int d = 0; d < NUM_DATASETS; d++) {
        const PatientLinkedList& master = masters[d];
        const int AGE_MIN = ageMins[d], AGE_MAX = ageMaxs[d];
        const string SEARCH_CARE_TYPE = careTypesToFind[d];
        const int DURATION_THRESHOLD = durationLimits[d];
        const string ageLabel = "Age Range (" + to_string(AGE_MIN) + "-" + to_string(AGE_MAX) + ")";
        const string careLabel = "CareType=" + SEARCH_CARE_TYPE;
        const string durLabel = "Duration > " + to_string(DURATION_THRESHOLD) + "h";

        // Pre-sorted copies are built once, outside the timed searches.
        PatientLinkedList sortedByAge;
        sortedByAge.append(master);
        sortedByAge.radixSort(SORT_BY_AGE);

        PatientLinkedList sortedByStay;
        sortedByStay.append(master);
        sortedByStay.radixSort(SORT_BY_LENGTH_OF_STAY);

        cout << "\n--- " << labels[d] << " ---\n";
        cout << left
            << setw(26) << "Search Type"
            << setw(10) << "Mode"
            << setw(13) << "Comparisons"
            << setw(8) << "Steps"
            << setw(9) << "Matches"
            << setw(16) << "Avg Time (ms)"
            << setw(12) << "Aux Mem (B)"
            << endl;
        cout << string(94, '-') << endl;

        auto logSearch = [&](const string& displayLabel, const string& csvLabel,
            const string& mode, const SearchResult& r) {
                cout << left << setw(26) << displayLabel << setw(10) << mode
                    << setw(13) << r.comparisons << setw(8) << r.steps
                    << setw(9) << r.matchCount
                    << setw(16) << fixed << setprecision(4) << r.timeMs
                    << setw(12) << r.auxBytes
                    << endl;
                searchResultsFile << shortLabels[d] << "," << csvLabel << "," << mode << ","
                    << r.comparisons << "," << r.steps << "," << r.matchCount << ","
                    << fixed << setprecision(4) << r.timeMs << "," << r.auxBytes << "\n";
            };

        SearchResult r;

        r = averageSearch([&]() -> SearchResult {
            PatientLinkedList m;
            return master.searchByAgeRangeUnsorted(AGE_MIN, AGE_MAX, m);
            }, RUNS);
        logSearch(ageLabel, "AgeRange", "Unsorted", r);

        r = averageSearch([&]() -> SearchResult {
            PatientLinkedList m;
            return sortedByAge.searchByAgeRangeSorted(AGE_MIN, AGE_MAX, m);
            }, RUNS);
        logSearch(ageLabel, "AgeRange", "Sorted", r);

        r = averageSearch([&]() -> SearchResult {
            PatientLinkedList m;
            return master.searchByCareType(SEARCH_CARE_TYPE, m);
            }, RUNS);
        logSearch(careLabel, "CareType", "Linear", r);

        r = averageSearch([&]() -> SearchResult {
            PatientLinkedList m;
            return master.searchByDurationThresholdUnsorted(DURATION_THRESHOLD, m);
            }, RUNS);
        logSearch(durLabel, "Duration", "Unsorted", r);

        r = averageSearch([&]() -> SearchResult {
            PatientLinkedList m;
            return sortedByStay.searchByDurationThresholdSorted(DURATION_THRESHOLD, m);
            }, RUNS);
        logSearch(durLabel, "Duration", "Sorted", r);
    }
    searchResultsFile.close();

    cout << "\n(Criteria differ per facility so each search has matches: A = seniors/Emergency/>24h,\n";
    cout << " B = ages 18-25/Rehabilitation/>2h, C = seniors/Outpatient/>24h.)\n";

    cout << "\n--- Correctness Check: Facility A, Age Range 61-100 (Sorted search) ---\n";
    {
        PatientLinkedList sortedCheck;
        sortedCheck.append(masters[0]);
        sortedCheck.radixSort(SORT_BY_AGE);
        PatientLinkedList matches;
        SearchResult r = sortedCheck.searchByAgeRangeSorted(ageMins[0], ageMaxs[0], matches);
        matches.printAll(5);
        cout << "Total matches: " << r.matchCount << "\n";
    }

    // ==================================================================
    // SECTION 8: PERFORMANCE ANALYSIS - MEMORY FOOTPRINT
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " MEMORY FOOTPRINT ANALYSIS - LINKED LIST\n";
    cout << "==================================================\n";

    cout << left
        << setw(40) << "Facility"
        << setw(12) << "Patients"
        << setw(18) << "Memory (bytes)"
        << setw(18) << "Bytes/Patient"
        << endl;
    cout << string(88, '-') << endl;

    ofstream memoryResultsFile(dataDir + "LinkedList_MemoryUsage.csv");
    memoryResultsFile << "Facility,Patients,MemoryBytes,BytesPerPatient\n";

    for (int d = 0; d < NUM_DATASETS; d++) {
        long long memBytes = masters[d].getMemoryUsageBytes();
        int patients = masters[d].getSize();
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

    cout << "\nNote: memory shown uses exact node count (no unused capacity),\n";
    cout << "but each node carries one extra pointer ('next') per element -\n";
    cout << "see PatientLinkedList::getMemoryUsageBytes comments.\n";

    // ==================================================================
    // SECTION 9: CLINICAL INSIGHTS
    // ==================================================================
    cout << "\n\n==================================================\n";
    cout << " CLINICAL INSIGHTS - SERVICE PREFERENCE & COST BY AGE GROUP\n";
    cout << " (Most requested care type and average cost per patient, RM)\n";
    cout << "==================================================\n";

    cout << left << setw(30) << "Age Group";
    for (int d = 0; d < NUM_DATASETS; d++)
        cout << setw(22) << (shortLabels[d] + " top care") << setw(12) << "Avg RM";
    cout << endl;
    cout << string(30 + 34 * NUM_DATASETS, '-') << endl;

    cout << fixed << setprecision(2);
    for (int g = 0; g < numGroups; g++) {
        cout << left << setw(30) << ageAnalyzers[0].getGroupLabel(g);
        for (int d = 0; d < NUM_DATASETS; d++)
            cout << setw(22) << ageAnalyzers[d].getMostRequestedCareTypeLabel(g)
            << setw(12) << ageAnalyzers[d].getGroupAverageCost(g);
        cout << endl;
    }

    cout << "\n\n==================================================\n";
    cout << " CLINICAL INSIGHTS - METROHEALTH SYSTEM-WIDE SUMMARY\n";
    cout << "==================================================\n";

    PatientLinkedList combined; // all three facilities in ONE linked list
    for (int d = 0; d < NUM_DATASETS; d++)
        combined.append(masters[d]);

    cout << "\nTotal patients across MetroHealth System: " << combined.getSize() << "\n";

    AgeGroupAnalyzer networkAgeAnalyzer;
    networkAgeAnalyzer.analyze(combined.getHead());

    int highestBillingGroup = 0;
    for (int g = 1; g < numGroups; g++) {
        if (networkAgeAnalyzer.getGroupTotalCost(g) > networkAgeAnalyzer.getGroupTotalCost(highestBillingGroup))
            highestBillingGroup = g;
    }

    cout << fixed << setprecision(2);
    cout << "\nHighest-billing age group (network-wide): "
        << networkAgeAnalyzer.getGroupLabel(highestBillingGroup) << " ("
        << networkAgeAnalyzer.getGroupDescription(highestBillingGroup) << ")\n";
    cout << "  -> Total Cost: RM " << networkAgeAnalyzer.getGroupTotalCost(highestBillingGroup) << "\n";
    cout << "  -> Patients: " << networkAgeAnalyzer.getGroupPatientCount(highestBillingGroup) << "\n";

    BillingSummary networkBilling;
    networkBilling.analyze(combined.getHead());

    int highestTrafficIndex = 0;
    for (int i = 1; i < networkBilling.getCareTypeCount(); i++) {
        if (networkBilling.getCareTypePatientCount(i) > networkBilling.getCareTypePatientCount(highestTrafficIndex))
            highestTrafficIndex = i;
    }

    cout << "\nHighest-traffic Care Type (network-wide): "
        << networkBilling.getCareTypeLabel(highestTrafficIndex)
        << " (" << networkBilling.getCareTypePatientCount(highestTrafficIndex) << " patients, RM "
        << networkBilling.getCareTypeTotalCost(highestTrafficIndex) << " total cost)\n";

    cout << "\n--- Full Network-Wide Billing Summary (All Facilities Combined) ---\n";
    networkBilling.printReport("MetroHealth System (All Facilities)");

    return 0;
}