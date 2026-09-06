#include "PatientArray.hpp"
#include <iostream>
using namespace std;

int main() {
    PatientArray facilityA;

    // NOTE: adjust the relative path if needed depending on where the
    // .exe runs from. In Visual Studio, the default working directory
    // is the project folder (Program1_Array), so if Data/ sits one
    // level up, use "../Data/dataset1_facility_a.csv".
    string filePath = "../Data/dataset1_facility_a.csv";

    cout << "Loading Facility A dataset (Array)..." << endl;
    if (facilityA.loadFromCSV(filePath)) {
        cout << "Loaded " << facilityA.getSize() << " patient records.\n\n";
        facilityA.printAll(10); // preview first 10 rows only
    }

    return 0;
}