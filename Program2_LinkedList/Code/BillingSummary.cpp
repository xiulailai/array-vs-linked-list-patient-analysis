// ============================================================
// BillingSummary.cpp
// CT077-3-2-DSTR  |  Dataset-Level Billing Summary (Linked List version)
// ============================================================
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

void BillingSummary::addRecord(const Patient& p) {
    // Cost = LengthOfStay x BaseCostPerHour x DaysVisitsPerYear
    double cost = p.lengthOfStay * p.baseCostPerHour * p.daysVisitsPerYear;

    totalPatients++;
    totalCost += cost;
    totalLengthOfStay += p.lengthOfStay;

    int idx = findOrAddCareType(p.careType);
    if (idx != -1) {
        careTypes[idx].count++;
        careTypes[idx].totalCost += cost;
    }
}

void BillingSummary::analyze(const Node* head) {
    for (const Node* current = head; current != nullptr; current = current->next)
        addRecord(current->data);
}

void BillingSummary::printReport(const string& datasetLabel) const {
    cout << "\n----------------------------------------\n";
    cout << "Dataset Billing Summary: " << datasetLabel << "\n";
    cout << "----------------------------------------\n";

    cout << left
        << setw(18) << "Care Type"
        << setw(16) << "Patient Count"
        << setw(18) << "Total Cost (RM)"
        << setw(20) << "Avg Cost/Patient"
        << endl;
    cout << string(72, '-') << endl;

    cout << fixed << setprecision(2);
    for (int i = 0; i < careTypeCount; i++) {
        double avgCost = (careTypes[i].count > 0)
            ? (careTypes[i].totalCost / careTypes[i].count) : 0.0;

        cout << left
            << setw(18) << careTypes[i].careType
            << setw(16) << careTypes[i].count
            << setw(18) << careTypes[i].totalCost
            << setw(20) << avgCost
            << endl;
    }

    cout << string(72, '-') << endl;
    cout << "Total Patients: " << totalPatients << endl;
    cout << "Total Billing for Dataset: RM " << totalCost << endl;
    cout << "Average Cost per Patient: RM "
        << (totalPatients > 0 ? totalCost / totalPatients : 0.0) << endl;
    cout << "Average Length of Stay: "
        << (totalPatients > 0 ? (double)totalLengthOfStay / totalPatients : 0.0)
        << " hours" << endl;
}

double BillingSummary::getTotalCost() const { return totalCost; }
int BillingSummary::getTotalPatients() const { return totalPatients; }

double BillingSummary::getAverageLengthOfStay() const {
    return (totalPatients > 0) ? (double)totalLengthOfStay / totalPatients : 0.0;
}

// ---------- CareType accessors (Section 9 - Clinical Insights) ----------
int BillingSummary::getCareTypeCount() const { return careTypeCount; }
string BillingSummary::getCareTypeLabel(int index) const { return careTypes[index].careType; }
int BillingSummary::getCareTypePatientCount(int index) const { return careTypes[index].count; }
double BillingSummary::getCareTypeTotalCost(int index) const { return careTypes[index].totalCost; }