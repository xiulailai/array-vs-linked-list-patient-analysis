// Patient.hpp
// CT077-3-2-DSTR  |  Shared Patient data structure (Linked List version)

#ifndef PATIENT_HPP
#define PATIENT_HPP
#include <string>
using namespace std;

// Mirrors the dataset columns exactly:
// PatientID, Age, CareType, LengthOfStay, BaseCostPerHour, DaysVisitsPerYear
struct Patient {
    string patient_id;
    int age;
    string care_type;
    int length_of_staying;        // hours
    double base_cost_per_hour;  // MYR/hr
    int day_visit_per_year;
};
#endif