# Patient Data Analysis: Array vs Linked List

Two C++ console programs that solve the same problem with two different data structures, so their performance can be compared directly. Both load patient records from three hospital facilities (the "MetroHealth System"), analyse them by age group and billing, sort and search them, and measure how each structure behaves.

- **Program 1** stores patients in a **dynamic array**.
- **Program 2** stores patients in a **singly linked list**.

Everything is hand-written: **no STL containers** are used (only `std::string`). Built for CT077-3-2-DSTR Data Structures at Asia Pacific University (APU).

## What the programs do

| Step | What happens |
|---|---|
| Load | Reads three CSV datasets (one per facility, 200 patients each) |
| Age-group analysis | Groups patients into 0-17, 18-25, 26-45, 46-60 and 61-100, then reports patient count, total and average cost, average length of stay and the most requested care type |
| Billing summary | Totals and averages per care type, for each facility and for the whole network |
| Sorting | Sorts by **age**, **length of stay** or **total cost** with four algorithms: bubble, quick, radix and merge sort |
| Searching | Finds patients by age range, care type or length-of-stay threshold, on unsorted and sorted data |
| Performance | Counts comparisons, swaps and steps, measures time, and estimates memory use |
| Clinical insights | Compares facilities and age groups, and finds the highest-billing age group and busiest care type |

Each run writes its measurements to CSV files in `Data/`, so the two programs can be compared side by side.

## The two implementations

| | Array (Program 1) | Linked list (Program 2) |
|---|---|---|
| Storage | `Patient*` array that doubles when full | Chain of `Node { Patient data; Node* next; }` with head and tail pointers |
| Insert at end | Amortised O(1) | O(1), thanks to the tail pointer |
| Access by position | O(1) | O(n) |
| Merge sort | Copies records into a temporary buffer | Relinks nodes, so no record is copied |
| Search results | Returned in an array | Returned as a linked list |
| Extra memory | Unused capacity | One pointer per node |

Copying is disabled on both collection classes. A shallow copy would free the same memory twice, so `append()` makes a deep copy instead.

## What the results show

These figures are for sorting Facility A (200 patients) by age. The operation counts are exact and repeatable. Times depend on the machine and build, so they are recorded in the CSV files and not quoted here.

| Algorithm | Array comparisons | Linked list comparisons |
|---|---|---|
| Bubble sort | 19,710 | 19,710 |
| Quick sort | 1,720 | 2,336 |
| Merge sort | 1,311 | 1,311 |
| Radix sort | 0 (400 moves) | 0 (400 moves) |

- **Bubble sort** needs roughly fifteen times as many comparisons as merge sort on just 200 records, and the gap grows quickly with size.
- **Radix sort** does no comparisons at all. The array version needs a full scratch copy of the data (about 14 KB for 200 patients in the recorded results), while the list version only relinks nodes and needs 20 pointers of extra space.
- **Sorted search favours the array.** On sorted data the array finds a length-of-stay threshold with a binary search in 8 comparisons. The linked list has no random access, so it still visits all 200 nodes and needs 127 comparisons to reach the same boundary.
- **Memory depends on the setup.** In the recorded results an array record takes 72 bytes and a list node 80 (the extra pointer). Even so, the array used more in total (18,012 against 16,012 bytes for 200 patients), because it reserves 250 slots. Exact sizes change with the compiler and platform.

Network-wide, across all 600 patients, both programs produce identical analysis results:

- Total billing: **RM 3,231,610**
- Highest-billing age group: **61-100** (181 patients, RM 2,322,210)
- Busiest care type: **Outpatient** (180 patients)

## Dataset

Three CSV files in `Data/`, one per facility, with the columns:

```
PatientID,Age,CareType,LengthOfStay,BaseCostPerHour,DaysVisitsPerYear
```

| File | Facility |
|---|---|
| `dataset1_facility_a.csv` | Facility A, General Hospital |
| `dataset2_facility_b.csv` | Facility B, University Medical Center |
| `dataset3_facility_c.csv` | Facility C, Community Health Clinic |

Care types are Emergency, Inpatient, Outpatient, Vaccination, Rehabilitation and Routine Checkup. Length of stay is in hours and costs are in Malaysian ringgit (RM). All patient records are made up.

## Project structure

```
.
├── Data/                        Datasets and the results the programs write
├── Program1_Array/
│   ├── Code/
│   │   ├── Test_Array.cpp       main(): runs every section
│   │   ├── PatientArray.*       Dynamic array, sorting, searching
│   │   ├── AgeGroupAnalyzer.*   Age-group analysis
│   │   ├── BillingSummary.*     Billing totals per care type
│   │   └── Patient.hpp, SortKey.hpp, SearchResult.hpp
│   └── Program1_Array.slnx      Visual Studio solution
└── Program2_LinkedList/
    ├── Code/
    │   ├── Test_LinkedList.cpp  main(): runs every section
    │   ├── PatientLinkedList.*  Linked list, sorting, searching
    │   ├── Node.hpp
    │   └── (same analyzers and shared headers)
    └── Program2_LinkedList.slnx
```

## Getting started

### Visual Studio (Windows)
Open `Program1_Array/Program1_Array.slnx` or `Program2_LinkedList/Program2_LinkedList.slnx`, then build and run. The projects use the v145 toolset and C++20.

### Command line (g++ or clang++)
Run these from the repository root:

```bash
g++ -std=c++20 Program1_Array/Code/*.cpp -o program1
g++ -std=c++20 Program2_LinkedList/Code/*.cpp -o program2
./program1
./program2
```

The programs find the `Data` folder on their own, wherever they are started from. Each run prints its report to the console and **overwrites the result CSVs** (`Array_*.csv` and `LinkedList_*.csv`) in `Data/`. Both programs finish in well under a second.

## License

Released under the [MIT License](LICENSE). Copyright (c) 2026 automaticicemaker Contributors and APU.
