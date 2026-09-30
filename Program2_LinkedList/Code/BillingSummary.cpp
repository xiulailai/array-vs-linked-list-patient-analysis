// BillingSummary.cpp
// CT077-3-2-DSTR  |  Dataset-Level Billing Summary (Linked List version)

#include "BillingSummary.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

BillingSummary::BillingSummary() {
    care_type_count = 0;
    total_patients = 0;
    total_cost = 0.0;
    total_length_of_staying = 0;
}

int BillingSummary::findOrAddCareType(const string& care_type) {
    for (int i = 0; i < care_type_count; i++) {
        if (care_types[i].care_type == care_type)
            return i;
    }
    if (care_type_count < max_care_types) {
        care_types[care_type_count].care_type = care_type;
        care_types[care_type_count].count = 0;
        care_types[care_type_count].total_cost = 0.0;
        care_type_count++;
        return care_type_count - 1;
    }
    return -1; // no room left (shouldn't happen with realistic data)
}
void BillingSummary::addRecord(const Patient& p) {
    // Cost = LengthOfStay x BaseCostPerHour x DaysVisitsPerYear
    double cost = p.length_of_staying * p.base_cost_per_hour * p.day_visit_per_year;
    total_patients++;
    total_cost += cost;
    total_length_of_staying += p.length_of_staying;
    int idx = findOrAddCareType(p.care_type);
    if (idx != -1) {
        care_types[idx].count++;
        care_types[idx].total_cost += cost;
    }
}
void BillingSummary::analyze(const Node* head) {
    for (const Node* current = head; current != nullptr; current = current->next)
        addRecord(current->data);
}
void BillingSummary::printReport(const string& dataset_label) const {
    cout << "\n----------------------------------------\n";
    cout << "Dataset Billing Summary: " << dataset_label << "\n";
    cout << "----------------------------------------\n";
    cout << left
        << setw(18) << "Care Type"
        << setw(16) << "Patient Count"
        << setw(18) << "Total Cost (RM)"
        << setw(20) << "Avg Cost/Patient"
        << endl;
    cout << string(72, '-') << endl;
    cout << fixed << setprecision(2);
    for (int i = 0; i < care_type_count; i++) {
        double avg_cost = (care_types[i].count > 0)
            ? (care_types[i].total_cost / care_types[i].count) : 0.0;
        cout << left
            << setw(18) << care_types[i].care_type
            << setw(16) << care_types[i].count
            << setw(18) << care_types[i].total_cost
            << setw(20) << avg_cost
            << endl;
    }
    cout << string(72, '-') << endl;
    cout << "Total Patients: " << total_patients << endl;
    cout << "Total Billing for Dataset: RM " << total_cost << endl;
    cout << "Average Cost per Patient: RM "
        << (total_patients > 0 ? total_cost / total_patients : 0.0) << endl;
    cout << "Average Length of Stay: "
        << (total_patients > 0 ? (double)total_length_of_staying / total_patients : 0.0)
        << " hours" << endl;
}
double BillingSummary::getTotalCost() const { return total_cost; }
int BillingSummary::getTotalPatients() const { return total_patients; }
double BillingSummary::getAverageLengthOfStay() const {
    return (total_patients > 0) ? (double)total_length_of_staying / total_patients : 0.0;
}
// ---------- CareType accessors (Section 9 - Clinical Insights) ----------
int BillingSummary::getCareTypeCount() const { return care_type_count; }
string BillingSummary::getCareTypeLabel(int index) const { return care_types[index].care_type; }
int BillingSummary::getCareTypePatientCount(int index) const { return care_types[index].count; }
double BillingSummary::getCareTypeTotalCost(int index) const { return care_types[index].total_cost; }