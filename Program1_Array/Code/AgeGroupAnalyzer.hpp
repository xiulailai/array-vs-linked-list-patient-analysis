// ============================================================
// AgeGroupAnalyzer.hpp
// CT077-3-2-DSTR  |  Age Group / Cost Analysis
// Buckets patients into the 5 assignment-defined age groups and
// computes per-group cost, visit-duration, and care-type stats.
// Works off a plain Patient array so it is reusable by BOTH the
// array and linked-list programs (each supplies data via toArray()).
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

    // Scans a plain array of patients and buckets them by age group,
    // accumulating cost, visit-duration, and care-type stats per group.
    void analyze(const Patient* data, int count);

    // Prints the full report as formatted console tables, matching
    // the sample format shown in the assignment brief.
    void printReport() const;

    // ---------- Accessors for cross-dataset / cross-age-group comparisons ----------
    static int getGroupCount();                    // returns NUM_GROUPS
    string getGroupLabel(int index) const;          // e.g. "18-25"
    string getGroupDescription(int index) const;    // e.g. "Young Adults..."
    int getGroupPatientCount(int index) const;
    double getGroupTotalCost(int index) const;
    double getGroupAverageCost(int index) const;
    double getGroupAverageLengthOfStay(int index) const;
};

#endif