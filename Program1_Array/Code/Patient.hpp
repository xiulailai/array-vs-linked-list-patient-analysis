// ============================================================
// Patient.hpp
// CT077-3-2-DSTR  |  Shared Patient data structure (Array version)
// ============================================================
#ifndef PATIENT_HPP
#define PATIENT_HPP

#include <string>
using namespace std;

// Mirrors the dataset columns exactly:
// PatientID, Age, CareType, LengthOfStay, BaseCostPerHour, DaysVisitsPerYear
struct Patient {
    string patientID;
    int age;
    string careType;
    int lengthOfStay;        // hours
    double baseCostPerHour;  // MYR/hr
    int daysVisitsPerYear;
};

#endif