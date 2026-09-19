// Computes total billing and CareType breakdown across an
// ENTIRE dataset (not scoped to an age group). Works off a
// plain Patient array, same pattern as AgeGroupAnalyzer, so it
// is reusable unchanged by both the array and linked-list programs.
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

    // Scans a plain array of patients and accumulates dataset-wide totals.
    void analyze(const Patient* data, int count);

    // Prints the dataset billing report as a formatted console table.
    void printReport(const string& datasetLabel) const;

    // Accessors used to build the cross-dataset comparison table
    double getTotalCost() const;
    int getTotalPatients() const;
    double getAverageLengthOfStay() const;
};

#endif