// Test_LinkedList.cpp
// CT077-3-2-DSTR  |  Program 1: ARRAY Implementation
// Entry point / test driver only - all logic lives in
// PatientArray, AgeGroupAnalyzer, and BillingSummary.
//
// Sections covered:
//   4/5) Age-group + billing analysis, cross-dataset/age-group comparisons
//   6)   Sorting performance (Bubble / Quick / Radix, across sort keys)
//   7)   Searching performance (unsorted vs sorted)
//   8)   Memory footprint
//   9)   Clinical insights (network-wide + facility x age-group table)
// Timings are the AVERAGE of runs repetitions, because a single run on
// 200 records takes only microseconds and is dominated by noise.
 
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
static void printMatches(const Patient* matches, int count, int limit) {
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
            << setw(18) << matches[i].care_type
            << setw(10) << matches[i].length_of_staying
            << setw(14) << matches[i].base_cost_per_hour
            << setw(12) << matches[i].day_visit_per_year
            << endl;
    }
    cout << "Showing " << shown << " of " << count << " matches.\n";
}
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
static const int number_of_datasets = 3;
static const int runs = 100;
// Runs a search runs times; returns the last result with ms_time replaced
// by the average. (Comparisons / steps / matches are identical every run.)
template <typename F>
SearchResult averageSearch(F search_fn, int runs) {
    SearchResult last = { 0, 0, 0, 0.0, 0 };
    double total_ms = 0.0;
    for (int i = 0; i < runs; i++) {
        last = search_fn();
        total_ms += last.ms_time;
    }
    last.ms_time = total_ms / runs;
    return last;
}
int main() {
    const string data_dir = findDataDir();
    string filenames[number_of_datasets] = {
        data_dir + "dataset1_facility_a.csv",
        data_dir + "dataset2_facility_b.csv",
        data_dir + "dataset3_facility_c.csv"
    };
    string labels[number_of_datasets] = {
        "Facility A - General Hospital",
        "Facility B - University Medical Center",
        "Facility C - Community Health Clinic"
    };
    string short_labels[number_of_datasets] = { "Facility A", "Facility B", "Facility C" };
    PatientArray masters[number_of_datasets]; // one loaded list per facility
    AgeGroupAnalyzer age_analyzers[number_of_datasets];
    double dataset_total_cost[number_of_datasets];
    int dataset_total_patients[number_of_datasets];
    double dataset_avg_stay[number_of_datasets];
    // Section 4 & 5: AGE GROUP + BILLING ANALYSIS 
    for (int d = 0; d < number_of_datasets; d++) {
        cout << "\n----------------------------------------\n";
        cout << " PROCESSING: " << labels[d] << "\n";
        cout << "----------------------------------------\n";
        dataset_total_cost[d] = 0.0;
        dataset_total_patients[d] = 0;
        dataset_avg_stay[d] = 0.0;
        if (!masters[d].loadFromCSV(filenames[d])) {
            cout << "Skipping " << labels[d] << " due to load error.\n";
            continue;
        }
        cout << "Loaded " << masters[d].getSize() << " patient records.\n";
        age_analyzers[d].analyze(masters[d].getData(), masters[d].getSize());
        cout << "\n--- Age Group & Cost Analysis ---\n";
        age_analyzers[d].printReport();
        BillingSummary billing;
        billing.analyze(masters[d].getData(), masters[d].getSize());
        billing.printReport(labels[d]);
        dataset_total_cost[d] = billing.getTotalCost();
        dataset_total_patients[d] = billing.getTotalPatients();
        dataset_avg_stay[d] = billing.getAverageLengthOfStay();
    }
    if (masters[0].getSize() + masters[1].getSize() + masters[2].getSize() == 0) {
        cout << "\nERROR: no dataset could be loaded from \"" << data_dir << "\".\n";
        cout << "Put dataset1_facility_a.csv, dataset2_facility_b.csv and\n";
        cout << "dataset3_facility_c.csv in a folder named Data next to your\n";
        cout << "project folder (or set Project > Properties > Debugging >\n";
        cout << "Working Directory) and run again.\n";
        return 1;
    }
    // ---------- Cross-Dataset Comparison Summary ----------
    cout << "\n\n----------------------------------------\n";
    cout << " CROSS-DATASET COMPARISON SUMMARY\n";
    cout << "----------------------------------------\n";
    cout << left
        << setw(40) << "Facility"
        << setw(12) << "Patients"
        << setw(18) << "Total Cost (RM)"
        << setw(16) << "Avg Stay(hrs)"
        << endl;
    cout << string(86, '-') << endl;
    cout << fixed << setprecision(2);
    for (int d = 0; d < number_of_datasets; d++) {
        cout << left
            << setw(40) << labels[d]
            << setw(12) << dataset_total_patients[d]
            << setw(18) << dataset_total_cost[d]
            << setw(16) << dataset_avg_stay[d]
            << endl;
    }
    int number_of_groups = AgeGroupAnalyzer::getGroupCount();
    // ---------- Age Group Expenditure Comparison ----------
    cout << "\n\n----------------------------------------\n";
    cout << " AGE GROUP EXPENDITURE COMPARISON ACROSS FACILITIES\n";
    cout << " (Total Cost RM per Age Group, per Facility)\n";
    cout << "----------------------------------------\n";
    cout << left << setw(46) << "Age Group";
    for (int d = 0; d < number_of_datasets; d++)
        cout << setw(16) << short_labels[d];
    cout << endl;
    cout << string(46 + 16 * number_of_datasets, '-') << endl;
    cout << fixed << setprecision(2);
    for (int g = 0; g < number_of_groups; g++) {
        string label = age_analyzers[0].getGroupLabel(g) + " ("
            + age_analyzers[0].getGroupDescription(g) + ")";
        cout << left << setw(46) << label;
        for (int d = 0; d < number_of_datasets; d++)
            cout << setw(16) << age_analyzers[d].getGroupTotalCost(g);
        cout << endl;
    }
    // ---------- Age Group Visit Duration Comparison ----------
    cout << "\n\n----------------------------------------\n";
    cout << " AGE GROUP VISIT DURATION COMPARISON ACROSS FACILITIES\n";
    cout << " (Average Length of Stay in hours, per Age Group, per Facility)\n";
    cout << "----------------------------------------\n";
    cout << left << setw(46) << "Age Group";
    for (int d = 0; d < number_of_datasets; d++)
        cout << setw(16) << short_labels[d];
    cout << endl;
    cout << string(46 + 16 * number_of_datasets, '-') << endl;
    for (int g = 0; g < number_of_groups; g++) {
        string label = age_analyzers[0].getGroupLabel(g) + " ("
            + age_analyzers[0].getGroupDescription(g) + ")";
        cout << left << setw(46) << label;
        for (int d = 0; d < number_of_datasets; d++)
            cout << setw(16) << age_analyzers[d].getGroupAverageLengthOfStay(g);
        cout << endl;
    }
    // Section 6: SORTING PERFORMANCE ANALYSIS
    cout << "\n\n----------------------------------------\n";
    cout << " SORTING PERFORMANCE ANALYSIS - ARRAY\n";
    cout << " (time = average of " << runs << " runs; Swaps for Radix/Merge = element moves)\n";
    cout << "----------------------------------------\n";
    SortKey keys[3] = { sort_by_age, sort_by_length_of_staying, sort_by_total_cost };
    string algorithms[4] = { "Bubble Sort", "Quick Sort", "Radix Sort", "Merge Sort" };
    ofstream sort_results_file(data_dir + "Array_SortResults.csv");
    sort_results_file << "Facility,Algorithm,SortKey,Comparisons,Swaps,AvgTimeMs,AuxBytes\n";
    for (int d = 0; d < number_of_datasets; d++) {
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
                double total_ms = 0.0;
                for (int run = 0; run < runs; run++) {
                    PatientArray work;
                    work.append(masters[d]); // fresh unsorted copy (not timed)
                    SortResult r;
                    if (a == 0) r = work.bubbleSort(keys[k]);
                    else if (a == 1) r = work.quickSort(keys[k]);
                    else if (a == 2) r = work.radixSort(keys[k]);
                    else r = work.mergeSort(keys[k]);
                    total_ms += r.ms_time;
                    last = r;
                }
                double avg_ms = total_ms / runs;
                cout << left
                    << setw(16) << algorithms[a]
                    << setw(18) << sortKeyLabel(keys[k])
                    << setw(14) << last.comparisons
                    << setw(10) << last.swaps
                    << setw(16) << fixed << setprecision(4) << avg_ms
                    << setw(14) << last.aux_bytes
                    << endl;
                sort_results_file << short_labels[d] << "," << algorithms[a] << ","
                    << sortKeyLabel(keys[k]) << ","
                    << last.comparisons << "," << last.swaps << ","
                    << fixed << setprecision(4) << avg_ms << ","
                    << last.aux_bytes << "\n";
            }
        }
    }
    sort_results_file.close();
    cout << "\n--- Correctness Check: Facility A sorted by Age (first 5 rows) ---\n";
    {
        PatientArray check;
        check.append(masters[0]);
        check.bubbleSort(sort_by_age);
        check.printAll(5);
    }
    // Section 7: SEARCHING PERFORMANCE ANALYSIS

    cout << "\n\n----------------------------------------\n";
    cout << " SEARCHING PERFORMANCE ANALYSIS - ARRAY\n";
    cout << " (time = average of " << runs << " runs; Steps = elements examined/copied)\n";
    cout << "----------------------------------------\n";
    // Search criteria are chosen PER FACILITY so every search returns real
    // matches (Facility B has only young adults, short stays, no seniors).
    const int age_mins[number_of_datasets] = { 61, 18, 61 };
    const int age_maxs[number_of_datasets] = { 100, 25, 100 };
    const string care_types_to_find[number_of_datasets] = { "Emergency", "Rehabilitation", "Outpatient" };
    const int duration_limits[number_of_datasets] = { 24, 2, 24 };
    ofstream search_results_file(data_dir + "Array_SearchResults.csv");
    search_results_file << "Facility,SearchType,Mode,Comparisons,Steps,Matches,AvgTimeMs,AuxBytes\n";
    for (int d = 0; d < number_of_datasets; d++) {
        const PatientArray& master = masters[d];
        const int age_min = age_mins[d], age_max = age_maxs[d];
        const string search_care_type = care_types_to_find[d];
        const int duration_threshold = duration_limits[d];
        const string age_label = "Age Range (" + to_string(age_min) + "-" + to_string(age_max) + ")";
        const string care_label = "Care Type=" + search_care_type;
        const string duration_label = "Duration > " + to_string(duration_threshold) + "h";
        // Pre-sorted copies are built once, outside the timed searches.
        PatientArray sorted_by_age;
        sorted_by_age.append(master);
        sorted_by_age.radixSort(sort_by_age);
        PatientArray sorted_by_stay;
        sorted_by_stay.append(master);
        sorted_by_stay.radixSort(sort_by_length_of_staying);
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
        auto logSearch = [&](const string& display_label, const string& csv_label,
            const string& mode, const SearchResult& r) {
                cout << left << setw(26) << display_label << setw(10) << mode
                    << setw(13) << r.comparisons << setw(8) << r.steps
                    << setw(9) << r.match_count
                    << setw(16) << fixed << setprecision(4) << r.ms_time
                    << setw(12) << r.aux_bytes
                    << endl;
                search_results_file << short_labels[d] << "," << csv_label << "," << mode << ","
                    << r.comparisons << "," << r.steps << "," << r.match_count << ","
                    << fixed << setprecision(4) << r.ms_time << "," << r.aux_bytes << "\n";
            };
        SearchResult r;
        r = averageSearch([&]() -> SearchResult {
            Patient* m; int c;
            SearchResult x = master.searchByAgeRangeUnsorted(age_min, age_max, m, c);
            delete[] m;
            return x;
            }, runs);
        logSearch(age_label, "AgeRange", "Unsorted", r);
        r = averageSearch([&]() -> SearchResult {
            Patient* m; int c;
            SearchResult x = sorted_by_age.searchByAgeRangeSorted(age_min, age_max, m, c);
            delete[] m;
            return x;
            }, runs);
        logSearch(age_label, "AgeRange", "Sorted", r);
        r = averageSearch([&]() -> SearchResult {
            Patient* m; int c;
            SearchResult x = master.searchByCareType(search_care_type, m, c);
            delete[] m;
            return x;
            }, runs);
        logSearch(care_label, "CareType", "Linear", r);
        r = averageSearch([&]() -> SearchResult {
            Patient* m; int c;
            SearchResult x = master.searchByDurationThresholdUnsorted(duration_threshold, m, c);
            delete[] m;
            return x;
            }, runs);
        logSearch(duration_label, "Duration", "Unsorted", r);
        r = averageSearch([&]() -> SearchResult {
            Patient* m; int c;
            SearchResult x = sorted_by_stay.searchByDurationThresholdSorted(duration_threshold, m, c);
            delete[] m;
            return x;
            }, runs);
        logSearch(duration_label, "Duration", "Sorted", r);
    }
    search_results_file.close();
    cout << "\n(Criteria differ per facility so each search has matches: A = seniors/Emergency/>24h,\n";
    cout << " B = ages 18-25/Rehabilitation/>2h, C = seniors/Outpatient/>24h.)\n";
    cout << "\n--- Correctness Check: Facility A, Age Range 61-100 (Sorted search) ---\n";
    {
        PatientArray sorted_check;
        sorted_check.append(masters[0]);
        sorted_check.radixSort(sort_by_age);
        Patient* matches; int match_count;
        sorted_check.searchByAgeRangeSorted(age_mins[0], age_maxs[0], matches, match_count);
        printMatches(matches, match_count, 5);
        delete[] matches;
    }
    // Section 8: PERFORMANCE ANALYSIS - MEMORY FOOTPRINT
    cout << "\n\n----------------------------------------\n";
    cout << " MEMORY FOOTPRINT ANALYSIS - ARRAY\n";
    cout << "----------------------------------------\n";
    cout << left
        << setw(40) << "Facility"
        << setw(12) << "Patients"
        << setw(18) << "Memory (bytes)"
        << setw(18) << "Bytes/Patient"
        << endl;
    cout << string(88, '-') << endl;
    ofstream memory_results_file(data_dir + "Array_MemoryUsage.csv");
    memory_results_file << "Facility,Patients,MemoryBytes,BytesPerPatient\n";
    for (int d = 0; d < number_of_datasets; d++) {
        long long mem_bytes = masters[d].getMemoryUsageBytes();
        int patients = masters[d].getSize();
        double bytes_per_patient = (patients > 0) ? (double)mem_bytes / patients : 0.0;
        cout << left
            << setw(40) << labels[d]
            << setw(12) << patients
            << setw(18) << mem_bytes
            << setw(18) << fixed << setprecision(2) << bytes_per_patient
            << endl;
        memory_results_file << short_labels[d] << "," << patients << "," << mem_bytes << ","
            << fixed << setprecision(2) << bytes_per_patient << "\n";
    }
    memory_results_file.close();
    cout << "\nNote: memory shown uses ALLOCATED CAPACITY, not just used size -\n";
    cout << "this includes unused slots left over from the array's doubling\n";
    cout << "resize strategy (see PatientArray::getMemoryUsageBytes comments).\n";
    // Section 9: CLINICAL INSIGHTS
    cout << "\n\n----------------------------------------\n";
    cout << " CLINICAL INSIGHTS - SERVICE PREFERENCE & COST BY AGE GROUP\n";
    cout << " (Most requested care type and average cost per patient, RM)\n";
    cout << "----------------------------------------\n";
    cout << left << setw(30) << "Age Group";
    for (int d = 0; d < number_of_datasets; d++)
        cout << setw(22) << (short_labels[d] + " top care") << setw(12) << "Avg RM";
    cout << endl;
    cout << string(30 + 34 * number_of_datasets, '-') << endl;
    cout << fixed << setprecision(2);
    for (int g = 0; g < number_of_groups; g++) {
        cout << left << setw(30) << age_analyzers[0].getGroupLabel(g);
        for (int d = 0; d < number_of_datasets; d++)
            cout << setw(22) << age_analyzers[d].getMostRequestedCareTypeLabel(g)
            << setw(12) << age_analyzers[d].getGroupAverageCost(g);
        cout << endl;
    }
    cout << "\n\n----------------------------------------\n";
    cout << " CLINICAL INSIGHTS - METROHEALTH SYSTEM-WIDE SUMMARY\n";
    cout << "----------------------------------------\n";
    PatientArray combined; // all three facilities in ONE linked list
    for (int d = 0; d < number_of_datasets; d++)
        combined.append(masters[d]);
    cout << "\nTotal patients across MetroHealth System: " << combined.getSize() << "\n";
    AgeGroupAnalyzer network_age_analyzer;
    network_age_analyzer.analyze(combined.getData(), combined.getSize());
    int highest_billing_group = 0;
    for (int g = 1; g < number_of_groups; g++) {
        if (network_age_analyzer.getGroupTotalCost(g) > network_age_analyzer.getGroupTotalCost(highest_billing_group))
            highest_billing_group = g;
    }
    cout << fixed << setprecision(2);
    cout << "\nHighest-billing age group (network-wide): "
        << network_age_analyzer.getGroupLabel(highest_billing_group) << " ("
        << network_age_analyzer.getGroupDescription(highest_billing_group) << ")\n";
    cout << "  -> Total Cost: RM " << network_age_analyzer.getGroupTotalCost(highest_billing_group) << "\n";
    cout << "  -> Patients: " << network_age_analyzer.getGroupPatientCount(highest_billing_group) << "\n";
    BillingSummary network_billing;
    network_billing.analyze(combined.getData(), combined.getSize());
    int highest_traffic_index = 0;
    for (int i = 1; i < network_billing.getCareTypeCount(); i++) {
        if (network_billing.getCareTypePatientCount(i) > network_billing.getCareTypePatientCount(highest_traffic_index))
            highest_traffic_index = i;
    }
    cout << "\nHighest-traffic Care Type (network-wide): "
        << network_billing.getCareTypeLabel(highest_traffic_index)
        << " (" << network_billing.getCareTypePatientCount(highest_traffic_index) << " patients, RM "
        << network_billing.getCareTypeTotalCost(highest_traffic_index) << " total cost)\n";
    cout << "\n--- Full Network-Wide Billing Summary (All Facilities Combined) ---\n";
    network_billing.printReport("MetroHealth System (All Facilities)");

    return 0;
}