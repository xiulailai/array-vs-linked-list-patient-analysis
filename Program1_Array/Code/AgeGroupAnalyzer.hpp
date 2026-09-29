// ============================================================
// AgeGroupAnalyzer.hpp
// CT077-3-2-DSTR  |  Age Group / Cost Analysis (Array version)
// Buckets patients into the 5 assignment-defined age groups and
// computes per-group cost, visit-duration, and care-type stats by
// scanning a plain Patient array.
// ============================================================
#ifndef AGE_GROUP_ANALYZER_HPP
#define AGE_GROUP_ANALYZER_HPP

#include "Patient.hpp"
#include <string>
using namespace std;

class AgeGroupAnalyzer {
private:
    static const int NUM_GROUPS = 5;
    static const int MAX_CARE_TYPES = 10; // safe upper bound per age group

    struct CareTypeStat {
        string careType;
        int count;
        double totalCost;
    };

    struct AgeGroupStat {
        string rangeLabel;      // e.g. "18-25"
        string description;     // e.g. "Young Adults / University Students"
        int patientCount;
        double totalCost;
        int totalLengthOfStay;  // running sum, used for average visit duration
        CareTypeStat careTypes[MAX_CARE_TYPES];
        int careTypeCount;
    };

    AgeGroupStat groups[NUM_GROUPS];

    int getGroupIndex(int age) const;
    void addToGroup(int groupIndex, const Patient& p);
    int findMostRequestedCareType(int groupIndex) const;

public:
    AgeGroupAnalyzer();

    // Scans an array of patients and buckets each by age group.
    void analyze(const Patient* data, int count);

    // Prints the full report as formatted console tables.
    void printReport() const;

    // ---------- Accessors for cross-dataset / cross-age-group comparisons ----------
    static int getGroupCount();
    string getGroupLabel(int index) const;
    string getGroupDescription(int index) const;
    int getGroupPatientCount(int index) const;
    double getGroupTotalCost(int index) const;
    double getGroupAverageCost(int index) const;
    double getGroupAverageLengthOfStay(int index) const;

    // Most-requested care type in a group, or "-" if the group is empty.
    string getMostRequestedCareTypeLabel(int index) const;
};

#endif