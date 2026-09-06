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

void AgeGroupAnalyzer::analyze(const Patient* data, int count) {
    for (int i = 0; i < count; i++) {
        int groupIndex = getGroupIndex(data[i].age);
        addToGroup(groupIndex, data[i]);
    }
}

int AgeGroupAnalyzer::findMostRequestedCareType(int groupIndex) const {
    const AgeGroupStat& group = groups[groupIndex];
    if (group.careTypeCount == 0) return -1;

    int maxIndex = 0;
    for (int i = 1; i < group.careTypeCount; i++) {
        if (group.careTypes[i].count > group.careTypes[maxIndex].count)
            maxIndex = i;
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
            << setw(16) << "Total Cost"
            << setw(20) << "Avg Cost/Patient"
            << endl;
        cout << string(70, '-') << endl;

        cout << fixed << setprecision(2);
        for (int i = 0; i < group.careTypeCount; i++) {
            const CareTypeStat& ct = group.careTypes[i];
            double avgCost = (ct.count > 0) ? (ct.totalCost / ct.count) : 0.0;

            cout << left
                << setw(18) << ct.careType
                << setw(16) << ct.count
                << setw(16) << ct.totalCost
                << setw(20) << avgCost
                << endl;
        }

        double groupAvgCost = (group.patientCount > 0)
            ? (group.totalCost / group.patientCount) : 0.0;

        cout << string(70, '-') << endl;
        cout << "Total Billing for Age Group: $" << group.totalCost << endl;
        cout << "Average Cost per Patient (Group): $" << groupAvgCost << endl;
        cout << "Total Patients in Group: " << group.patientCount << endl;
    }
}