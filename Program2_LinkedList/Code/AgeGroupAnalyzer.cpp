// ============================================================
// AgeGroupAnalyzer.cpp
// CT077-3-2-DSTR  |  Age Group / Cost Analysis (Linked List version)
// ============================================================
#include "AgeGroupAnalyzer.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

AgeGroupAnalyzer::AgeGroupAnalyzer() {
    // Set up the 5 fixed age brackets from the assignment brief
    groups[0].rangeLabel = "0-17";
    groups[0].description = "Pediatrics & Adolescents";

    groups[1].rangeLabel = "18-25";
    groups[1].description = "Young Adults / University Students";

    groups[2].rangeLabel = "26-45";
    groups[2].description = "Working Adults (Early Career)";

    groups[3].rangeLabel = "46-60";
    groups[3].description = "Working Adults (Late Career)";

    groups[4].rangeLabel = "61-100";
    groups[4].description = "Senior Citizens / Geriatric Care";

    for (int i = 0; i < NUM_GROUPS; i++) {
        groups[i].patientCount = 0;
        groups[i].totalCost = 0.0;
        groups[i].totalLengthOfStay = 0;
        groups[i].careTypeCount = 0;
    }
}

int AgeGroupAnalyzer::getGroupIndex(int age) const {
    if (age <= 17) return 0;
    else if (age <= 25) return 1;
    else if (age <= 45) return 2;
    else if (age <= 60) return 3;
    else return 4; // 61 and above
}

void AgeGroupAnalyzer::addToGroup(int groupIndex, const Patient& p) {
    AgeGroupStat& group = groups[groupIndex];

    // Cost = LengthOfStay x BaseCostPerHour x DaysVisitsPerYear
    double cost = p.lengthOfStay * p.baseCostPerHour * p.daysVisitsPerYear;
    group.patientCount++;
    group.totalCost += cost;
    group.totalLengthOfStay += p.lengthOfStay;

    // Find an existing CareType entry for this group, or add a new one
    int foundIndex = -1;
    for (int i = 0; i < group.careTypeCount; i++) {
        if (group.careTypes[i].careType == p.careType) {
            foundIndex = i;
            break;
        }
    }

    if (foundIndex == -1 && group.careTypeCount < MAX_CARE_TYPES) {
        foundIndex = group.careTypeCount;
        group.careTypes[foundIndex].careType = p.careType;
        group.careTypes[foundIndex].count = 0;
        group.careTypes[foundIndex].totalCost = 0.0;
        group.careTypeCount++;
    }

    if (foundIndex != -1) {
        group.careTypes[foundIndex].count++;
        group.careTypes[foundIndex].totalCost += cost;
    }
}

void AgeGroupAnalyzer::analyze(const Node* head) {
    for (const Node* current = head; current != nullptr; current = current->next)
        addToGroup(getGroupIndex(current->data.age), current->data);
}

int AgeGroupAnalyzer::findMostRequestedCareType(int groupIndex) const {
    const AgeGroupStat& group = groups[groupIndex];
    if (group.careTypeCount == 0) return -1;

    int maxIndex = 0;
    for (int i = 1; i < group.careTypeCount; i++) {
        if (group.careTypes[i].count > group.careTypes[maxIndex].count)
            maxIndex = i; // ties keep the first care type found
    }
    return maxIndex;
}

void AgeGroupAnalyzer::printReport() const {
    for (int g = 0; g < NUM_GROUPS; g++) {
        const AgeGroupStat& group = groups[g];

        if (group.patientCount == 0)
            continue; // skip empty age groups for a cleaner report

        cout << "\nAge Group: " << group.rangeLabel
            << " (" << group.description << ")\n";

        int mostRequestedIndex = findMostRequestedCareType(g);
        if (mostRequestedIndex != -1) {
            cout << "Most Requested Care Type: "
                << group.careTypes[mostRequestedIndex].careType
                << " (" << group.careTypes[mostRequestedIndex].count
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
        for (int i = 0; i < group.careTypeCount; i++) {
            const CareTypeStat& ct = group.careTypes[i];
            double avgCost = (ct.count > 0) ? (ct.totalCost / ct.count) : 0.0;

            cout << left
                << setw(18) << ct.careType
                << setw(16) << ct.count
                << setw(18) << ct.totalCost
                << setw(20) << avgCost
                << endl;
        }

        double groupAvgCost = (group.patientCount > 0)
            ? (group.totalCost / group.patientCount) : 0.0;

        cout << string(72, '-') << endl;
        cout << "Total Billing for Age Group: RM " << group.totalCost << endl;
        cout << "Average Cost per Patient (Group): RM " << groupAvgCost << endl;
        cout << "Total Patients in Group: " << group.patientCount << endl;
    }
}

// ---------- Accessors for cross-dataset / cross-age-group comparisons ----------
int AgeGroupAnalyzer::getGroupCount() { return NUM_GROUPS; }
string AgeGroupAnalyzer::getGroupLabel(int index) const { return groups[index].rangeLabel; }
string AgeGroupAnalyzer::getGroupDescription(int index) const { return groups[index].description; }
int AgeGroupAnalyzer::getGroupPatientCount(int index) const { return groups[index].patientCount; }
double AgeGroupAnalyzer::getGroupTotalCost(int index) const { return groups[index].totalCost; }

double AgeGroupAnalyzer::getGroupAverageCost(int index) const {
    return (groups[index].patientCount > 0)
        ? (groups[index].totalCost / groups[index].patientCount) : 0.0;
}

double AgeGroupAnalyzer::getGroupAverageLengthOfStay(int index) const {
    return (groups[index].patientCount > 0)
        ? (double)groups[index].totalLengthOfStay / groups[index].patientCount : 0.0;
}

string AgeGroupAnalyzer::getMostRequestedCareTypeLabel(int index) const {
    int i = findMostRequestedCareType(index);
    return (i == -1) ? "-" : groups[index].careTypes[i].careType;
}