#include "BillingSummary.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

BillingSummary::BillingSummary() {
    careTypeCount = 0;
    totalPatients = 0;
    totalCost = 0.0;
    totalLengthOfStay = 0;
}

int BillingSummary::findOrAddCareType(const string& careType) {
    for (int i = 0; i < careTypeCount; i++) {
        if (careTypes[i].careType == careType)
            return i;
    }
    if (careTypeCount < MAX_CARE_TYPES) {
        careTypes[careTypeCount].careType = careType;
        careTypes[careTypeCount].count = 0;
        careTypes[careTypeCount].totalCost = 0.0;
        careTypeCount++;
        return careTypeCount - 1;
    }
    return -1; // no room left (shouldn't happen with realistic data)
}

void BillingSummary::analyze(const Patient* data, int count) {
    for (int i = 0; i < count; i++) {
        // Cost = LengthOfStay x BaseCostPerHour x DaysVisitsPerYear
        double cost = data[i].lengthOfStay * data[i].baseCostPerHour * data[i].daysVisitsPerYear;

        totalPatients++;
        totalCost += cost;
        totalLengthOfStay += data[i].lengthOfStay;

        int idx = findOrAddCareType(data[i].careType);
        if (idx != -1) {
            careTypes[idx].count++;
            careTypes[idx].totalCost += cost;
        }
    }
}

void BillingSummary::printReport(const string& datasetLabel) const {
    cout << "\n----------------------------------------\n";
    cout << "Dataset Billing Summary: " << datasetLabel << "\n";
    cout << "----------------------------------------\n";

    cout << left
        << setw(18) << "Care Type"
        << setw(16) << "Patient Count"
        << setw(16) << "Total Cost"
        << setw(20) << "Avg Cost/Patient"
        << endl;
    cout << string(70, '-') << endl;

    cout << fixed << setprecision(2);
    for (int i = 0; i < careTypeCount; i++) {
        double avgCost = (careTypes[i].count > 0)
            ? (careTypes[i].totalCost / careTypes[i].count) : 0.0;

        cout << left
            << setw(18) << careTypes[i].careType
            << setw(16) << careTypes[i].count
            << setw(16) << careTypes[i].totalCost
            << setw(20) << avgCost
            << endl;
    }

    cout << string(70, '-') << endl;
    cout << "Total Patients: " << totalPatients << endl;
    cout << "Total Billing for Dataset: $" << totalCost << endl;
    cout << "Average Cost per Patient: $"
        << (totalPatients > 0 ? totalCost / totalPatients : 0.0) << endl;
    cout << "Average Length of Stay: "
        << (totalPatients > 0 ? (double)totalLengthOfStay / totalPatients : 0.0)
        << " hours" << endl;
}

double BillingSummary::getTotalCost() const {
    return totalCost;
}

int BillingSummary::getTotalPatients() const {
    return totalPatients;
}

double BillingSummary::getAverageLengthOfStay() const {
    return (totalPatients > 0) ? (double)totalLengthOfStay / totalPatients : 0.0;
}