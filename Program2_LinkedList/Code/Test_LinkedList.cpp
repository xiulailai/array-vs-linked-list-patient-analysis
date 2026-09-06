// Entry point / test driver only - all logic lives in
// PatientLinkedList, AgeGroupAnalyzer, and BillingSummary.
// Loops through all 3 facility datasets and produces:
//   1) A per-dataset age-group & cost breakdown
//   2) A per-dataset overall billing summary (by CareType)
//   3) A final cross-dataset comparison table
#include "PatientLinkedList.hpp"
#include "AgeGroupAnalyzer.hpp"
#include "BillingSummary.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

int main() {
    const int NUM_DATASETS = 3;

    string filenames[NUM_DATASETS] = {
        "../Data/dataset1_facility_a.csv",
        "../Data/dataset2_facility_b.csv",
        "../Data/dataset3_facility_c.csv"
    };

    string labels[NUM_DATASETS] = {
        "Facility A - General Hospital",
        "Facility B - University Medical Center",
        "Facility C - Community Health Clinic"
    };

    // Store results per dataset for the final comparison table
    double datasetTotalCost[NUM_DATASETS];
    int datasetTotalPatients[NUM_DATASETS];
    double datasetAvgStay[NUM_DATASETS];

    for (int d = 0; d < NUM_DATASETS; d++) {
        cout << "\n==================================================\n";
        cout << " PROCESSING: " << labels[d] << "\n";
        cout << "==================================================\n";

        PatientLinkedList facility; // fresh instance each loop iteration

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

        // ---------- Age Group & Cost Analysis ----------
        AgeGroupAnalyzer ageAnalyzer;
        ageAnalyzer.analyze(allPatients, patientCount);
        cout << "\n--- Age Group & Cost Analysis ---\n";
        ageAnalyzer.printReport();

        // ---------- Dataset-Level Billing Summary ----------
        BillingSummary billing;
        billing.analyze(allPatients, patientCount);
        billing.printReport(labels[d]);

        // Save totals for the cross-dataset comparison below
        datasetTotalCost[d] = billing.getTotalCost();
        datasetTotalPatients[d] = billing.getTotalPatients();
        datasetAvgStay[d] = billing.getAverageLengthOfStay();

        delete[] allPatients; // avoid memory leak from toArray()
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

    return 0;
}