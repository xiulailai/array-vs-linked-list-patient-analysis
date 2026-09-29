// ============================================================
// BillingSummary.hpp
// CT077-3-2-DSTR  |  Dataset-Level Billing Summary (Array version)
// Computes total billing and CareType breakdown across an ENTIRE
// dataset by scanning a plain Patient array.
// ============================================================
#ifndef BILLING_SUMMARY_HPP
#define BILLING_SUMMARY_HPP

#include "Patient.hpp"
#include <string>
using namespace std;

class BillingSummary {
private:
    static const int MAX_CARE_TYPES = 10; // safe upper bound

    struct CareTypeStat {
        string careType;
        int count;
        double totalCost;
    };

    CareTypeStat careTypes[MAX_CARE_TYPES];
    int careTypeCount;

    int totalPatients;
    double totalCost;
    int totalLengthOfStay; // running sum, used to compute average stay

    int findOrAddCareType(const string& careType);

public:
    BillingSummary();

    // Scans an array of patients and accumulates dataset-wide totals.
    void analyze(const Patient* data, int count);

    // Prints the dataset billing report as a formatted console table.
    void printReport(const string& datasetLabel) const;

    double getTotalCost() const;
    int getTotalPatients() const;
    double getAverageLengthOfStay() const;

    // ---------- CareType accessors (Section 9 - Clinical Insights) ----------
    int getCareTypeCount() const;
    string getCareTypeLabel(int index) const;
    int getCareTypePatientCount(int index) const;
    double getCareTypeTotalCost(int index) const;
};

#endif