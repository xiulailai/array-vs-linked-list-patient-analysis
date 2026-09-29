// ============================================================
// BillingSummary.hpp
// CT077-3-2-DSTR  |  Dataset-Level Billing Summary (Linked List version)
// Computes total billing and CareType breakdown across an ENTIRE
// dataset by walking the linked list directly (no array copy).
// ============================================================
#ifndef BILLING_SUMMARY_HPP
#define BILLING_SUMMARY_HPP

#include "Patient.hpp"
#include "Node.hpp"
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
    void addRecord(const Patient& p);

public:
    BillingSummary();

    // Walks the linked list from 'head' and accumulates dataset-wide totals.
    void analyze(const Node* head);

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