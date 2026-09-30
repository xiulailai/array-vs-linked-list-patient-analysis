// AgeGroupAnalyzer.cpp
// CT077-3-2-DSTR  |  Age Group / Cost Analysis (Linked List version)

#include "AgeGroupAnalyzer.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

AgeGroupAnalyzer::AgeGroupAnalyzer() {
    // Set up the 5 fixed age brackets from the assignment brief
    groups[0].range_label = "0-17";
    groups[0].description = "Pediatrics & Adolescents";
    groups[1].range_label = "18-25";
    groups[1].description = "Young Adults / University Students";
    groups[2].range_label = "26-45";
    groups[2].description = "Working Adults (Early Career)";
    groups[3].range_label = "46-60";
    groups[3].description = "Working Adults (Late Career)";
    groups[4].range_label = "61-100";
    groups[4].description = "Senior Citizens / Geriatric Care";
    for (int i = 0; i < num_groups; i++) {
        groups[i].patient_count = 0;
        groups[i].total_cost = 0.0;
        groups[i].total_length_of_staying = 0;
        groups[i].care_type_count = 0;
    }
}

int AgeGroupAnalyzer::getGroupIndex(int age) const {
    if (age <= 17) return 0;
    else if (age <= 25) return 1;
    else if (age <= 45) return 2;
    else if (age <= 60) return 3;
    else return 4; // 61 and above
}
void AgeGroupAnalyzer::addToGroup(int group_index, const Patient& p) {
    AgeGroupStat& group = groups[group_index];
    // Cost = LengthOfStay x BaseCostPerHour x DaysVisitsPerYear
    double cost = p.length_of_staying * p.base_cost_per_hour * p.day_visit_per_year;
    group.patient_count++;
    group.total_cost += cost;
    group.total_length_of_staying += p.length_of_staying;
    // Find an existing CareType entry for this group, or add a new one
    int found_index = -1;
    for (int i = 0; i < group.care_type_count; i++) {
        if (group.care_types[i].care_type == p.care_type) {
            found_index = i;
            break;
        }
    }
    if (found_index == -1 && group.care_type_count < max_care_types) {
        found_index = group.care_type_count;
        group.care_types[found_index].care_type = p.care_type;
        group.care_types[found_index].count = 0;
        group.care_types[found_index].total_cost = 0.0;
        group.care_type_count++;
    }
    if (found_index != -1) {
        group.care_types[found_index].count++;
        group.care_types[found_index].total_cost += cost;
    }
}
void AgeGroupAnalyzer::analyze(const Node* head) {
    for (const Node* current = head; current != nullptr; current = current->next)
        addToGroup(getGroupIndex(current->data.age), current->data);
}
int AgeGroupAnalyzer::findMostRequestedCareType(int group_index) const {
    const AgeGroupStat& group = groups[group_index];
    if (group.care_type_count == 0) return -1;
    int max_index = 0;
    for (int i = 1; i < group.care_type_count; i++) {
        if (group.care_types[i].count > group.care_types[max_index].count)
            max_index = i; // ties keep the first care type found
    }
    return max_index;
}
void AgeGroupAnalyzer::printReport() const {
    for (int g = 0; g < num_groups; g++) {
        const AgeGroupStat& group = groups[g];
        if (group.patient_count == 0)
            continue; // skip empty age groups for a cleaner report
        cout << "\nAge Group: " << group.range_label
            << " (" << group.description << ")\n";
        int most_requested_index = findMostRequestedCareType(g);
        if (most_requested_index != -1) {
            cout << "Most Requested Care Type: "
                << group.care_types[most_requested_index].care_type
                << " (" << group.care_types[most_requested_index].count
                << " patients)\n\n";
        }
        cout << left
            << setw(18) << "Care Type"
            << setw(16) << "Patient Count"
            << setw(18) << "Total Cost (RM)"
            << setw(20) << "Avg Cost/Patient"
            << endl;
        cout << string(72, '-') << endl;
        cout << fixed << setprecision(2);
        for (int i = 0; i < group.care_type_count; i++) {
            const CareTypeStat& ct = group.care_types[i];
            double avg_cost = (ct.count > 0) ? (ct.total_cost / ct.count) : 0.0;
            cout << left
                << setw(18) << ct.care_type
                << setw(16) << ct.count
                << setw(18) << ct.total_cost
                << setw(20) << avg_cost
                << endl;
        }
        double group_avg_cost = (group.patient_count > 0)
            ? (group.total_cost / group.patient_count) : 0.0;
        cout << string(72, '-') << endl;
        cout << "Total Billing for Age Group: RM " << group.total_cost << endl;
        cout << "Average Cost per Patient (Group): RM " << group_avg_cost << endl;
        cout << "Total Patients in Group: " << group.patient_count << endl;
    }
}
// ---------- Accessors for cross-dataset / cross-age-group comparisons ----------
int AgeGroupAnalyzer::getGroupCount() { return num_groups; }
string AgeGroupAnalyzer::getGroupLabel(int index) const { return groups[index].range_label; }
string AgeGroupAnalyzer::getGroupDescription(int index) const { return groups[index].description; }
int AgeGroupAnalyzer::getGroupPatientCount(int index) const { return groups[index].patient_count; }
double AgeGroupAnalyzer::getGroupTotalCost(int index) const { return groups[index].total_cost; }
double AgeGroupAnalyzer::getGroupAverageCost(int index) const {
    return (groups[index].patient_count > 0)
        ? (groups[index].total_cost / groups[index].patient_count) : 0.0;
}
double AgeGroupAnalyzer::getGroupAverageLengthOfStay(int index) const {
    return (groups[index].patient_count > 0)
        ? (double)groups[index].total_length_of_staying / groups[index].patient_count : 0.0;
}
string AgeGroupAnalyzer::getMostRequestedCareTypeLabel(int index) const {
    int i = findMostRequestedCareType(index);
    return (i == -1) ? "-" : groups[index].care_types[i].care_type;
}