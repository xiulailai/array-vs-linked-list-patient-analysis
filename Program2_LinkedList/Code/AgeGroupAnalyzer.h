// Buckets patients into the 5 assignment-defined age groups and
// computes per-group cost and care-type statistics. Works off a
// plain Patient array so it is reusable by BOTH the array and
// linked-list programs (each supplies data via toArray()).
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
        string rangeLabel;    // e.g. "18-25"
        string description;   // e.g. "Young Adults / University Students"
        int patientCount;
        double totalCost;
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
    // accumulating cost and care-type stats per group.
    void analyze(const Patient* data, int count);

    // Prints the full report as formatted console tables, matching
    // the sample format shown in the assignment brief.
    void printReport() const;
};

#endif