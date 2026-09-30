// AgeGroupAnalyzer.hpp
// CT077-3-2-DSTR  |  Age Group / Cost Analysis (Linked List version)
// Buckets patients into the 5 assignment-defined age groups and
// computes per-group cost, visit-duration, and care-type stats by
// walking the linked list directly (no array copy).

#ifndef AGE_GROUP_ANALYZER_HPP
#define AGE_GROUP_ANALYZER_HPP
#include "Patient.hpp"
#include "Node.hpp"
#include <string>
using namespace std;

class AgeGroupAnalyzer {
private:
    static const int num_groups = 5;
    static const int max_care_types = 10; // safe upper bound per age group
    struct CareTypeStat {
        string care_type;
        int count;
        double total_cost;
    };
    struct AgeGroupStat {
        string range_label;      // e.g. "18-25"
        string description;     // e.g. "Young Adults / University Students"
        int patient_count;
        double total_cost;
        int total_length_of_staying;  // running sum, used for average visit duration
        CareTypeStat care_types[max_care_types];
        int care_type_count;
    };
    AgeGroupStat groups[num_groups];
    int getGroupIndex(int age) const;
    void addToGroup(int group_index, const Patient& p);
    int findMostRequestedCareType(int group_index) const;
public:
    AgeGroupAnalyzer();
    // Walks the list from 'head' and buckets each patient by age group.
    void analyze(const Node* head);
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