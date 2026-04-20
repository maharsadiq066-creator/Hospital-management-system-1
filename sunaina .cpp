{,}#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <iomanip>
using namespace std;

// Data Structures Used:
// Struct for records, Vector for lists, Priority Queue for patients,
// Map for quick lookup, File handling for permanent storage

struct Doctor {
    int id;
    string name;
    string specialization;
    string shift; // Morning, Evening, Night
    bool isAvailable;
};

struct Nurse {
    int id;
    string name;
    string shift;
    bool isAvailable;
};

struct Patient {
    int id;}
    string name;
    int age;
    string condition;
    bool isCritical; // true = critical, false = non-critical
    int assignedDoctorID;
};
,
// Custom comparator for priority queue: critical patients first
struct ComparePatient {
    bool operator()(Patient const& p1, Patient const& p2) {
        // Critical patients have higher priority
        if (p1.isCritical!= p2.isCritical)
            return p2.isCritical; // if p2 is critical, p1 has lower priority
        return p1.id > p2.id; // if same priority, older patient first
    }
};

class HospitalSystem {
private:
    vector<Doctor> doctors;
    vector<Nurse> nurses;
    vector<Patient> allPatients;
    priority_queue<Patient, vector<Patient>, ComparePatient> patientQueue;
    map<int, Doctor> doctorMap; // for O(1) lookup

    // File names
    const string DOCTOR_FILE = "doctors.txt";
    const string NURSE_FILE = "nurses.txt";
    const string PATIENT_FILE = "patients.txt";
    const string ROSTER_FILE = "duty_roster.txt";

public:
    HospitalSystem() {
        loadData();
    }

    // 1. EFFICIENTLY RECORD DOCTORS, NURSES AND PATIENTS DATA
    void addDoctor() {
        Doctor d;
        cout << "Enter Doctor ID: "; cin >> d.id;
        cin.ignore();
        cout << "Enter Name: "; getline(cin, d.name);
        cout << "Enter Specialization: "; getline(cin, d.specialization);
        cout << "Enter Shift (Morning/Evening/Night): "; getline(cin, d.shift);
        d.isAvailable = true;
        doctors.push_back(d);
        doctorMap[d.id] = d;
        saveDoctors();
        cout << "Doctor added successfully!\n";
    }

    void addNurse() {
        Nurse n;
        cout << "Enter Nurse ID: "; cin >> n.id;
        cin.ignore();
        cout << "Enter Name: "; getline(cin, n.name);
        cout << "Enter Shift (Morning/Evening/Night): "; getline(cin, n.shift);
        n.isAvailable = true;
        nurses.push_back(n);
        saveNurses();
        cout << "Nurse added successfully!\n";
    }

    void addPatient() {
        Patient p;
        cout << "Enter Patient ID: "; cin >> p.id;
        cin.ignore();
        cout << "Enter Name: "; getline(cin, p.name);
        cout << "Enter Age: "; cin >> p.age;
        cin.ignore();
        cout << "Enter Condition: "; getline(cin, p.condition);
        cout << "Is Critical? (1 for Yes, 0 for No): "; cin >> p.isCritical;
        p.assignedDoctorID = -1; // not assigned yet
        allPatients.push_back(p);
        patientQueue.push(p); // 5. PRIORITY: critical patients auto-prioritized
        savePatients();
        cout << "Patient added to waiting queue!\n";
    }

    // 2. CORRECTLY SCHEDULING DUTY ROSTER FOR DOCTORS AND NURSES
    void generateDutyRoster() {
        ofstream file(ROSTER_FILE);
        file << "====== HOSPITAL DUTY ROSTER ======\n\n";
        file << "--- DOCTORS ---\n";
        file << left << setw(5) << "ID" << setw(20) << "Name" << setw(20) << "Specialization" << setw(10) << "Shift\n";
        file << "------------------------------------------------------------\n";
        for (auto& d : doctors) {
            file << left << setw(5) << d.id << setw(20) << d.name
                 << setw(20) << d.specialization << setw(10) << d.shift << "\n";
        }

        file << "\n--- NURSES ---\n";
        file << left << setw(5) << "ID" << setw(20) << "Name" << setw(10) << "Shift\n";
        file << "--------------------------------------\n";
        for (auto& n : nurses) {
            file << left << setw(5) << n.id << setw(20) << n.name << setw(10) << n.shift << "\n";
        }
        file.close();
        cout << "Duty roster generated in " << ROSTER_FILE << "\n";
    }

    // 3 & 4. MANAGING APPOINTMENTS, WAITING QUEUE & ASSIGNING DOCTORS EFFICIENTLY
    void assignDoctorToPatient() {
        if (patientQueue.empty()) {
            cout << "No patients in waiting queue.\n";
            return;
        }

        Patient currentPatient = patientQueue.top(); // Highest priority patient
        patientQueue.pop();

        // Find available doctor with matching specialization if possible
        // For simplicity: assign first available doctor
        int assigned = -1;
        for (auto& d : doctors) {
            if (d.isAvailable) {
                assigned = d.id;
                d.isAvailable = false; // Mark busy
                doctorMap[d.id].isAvailable = false;
                break;
            }
        }

        if (assigned!= -1) {
            currentPatient.assignedDoctorID = assigned;
            cout << "Patient " << currentPatient.name;
            if (currentPatient.isCritical) cout << " [CRITICAL]";
            cout << " assigned to Dr. " << doctorMap[assigned].name << "\n";

            // Update patient record
            for (auto& p : allPatients) {
                if (p.id == currentPatient.id) {
                    p.assignedDoctorID = assigned;
                    break;
                }
            }
            savePatients();
            saveDoctors();
        } else {
            cout << "No doctors available. Patient sent back to queue.\n";
            patientQueue.push(currentPatient);
        }
    }

    // 3. MANAGING PATIENT APPOINTMENTS AND WAITING QUEUE
    void viewWaitingQueue() {
        if (patientQueue.empty()) {
            cout << "Waiting queue is empty.\n";
            return;
        }
        cout << "\n====== PATIENT WAITING QUEUE ======\n";
        cout << left << setw(5) << "ID" << setw(15) << "Name" << setw(5) << "Age"
             << setw(15) << "Condition" << "Priority\n";
        cout << "----------------------------------------------------\n";

        // Copy queue to display without destroying original
        priority_queue<Patient, vector<Patient>, ComparePatient> temp = patientQueue;
        while (!temp.empty()) {
            Patient p = temp.top();
            temp.pop();
            cout << left << setw(5) << p.id << setw(15) << p.name << setw(5) << p.age
                 << setw(15) << p.condition << (p.isCritical? "CRITICAL" : "Normal") << "\n";
        }
    }

    void displayAllData() {
        cout << "\n--- ALL DOCTORS ---\n";
        for (auto& d : doctors)
            cout << "ID: " << d.id << ", Name: " << d.name << ", " << d.specialization
                 << ", Shift: " << d.shift << ", Available: " << (d.isAvailable? "Yes" : "No") << "\n";

        cout << "\n--- ALL PATIENTS ---\n";
        for (auto& p : allPatients) {
            cout << "ID: " << p.id << ", Name: " << p.name << ", Age: " << p.age
                 << ", Critical: " << (p.isCritical? "Yes" : "No");
            if (p.assignedDoctorID!= -1)
                cout << ", Doctor: " << doctorMap[p.assignedDoctorID].name;
            cout << "\n";
        }
    }

    // FILE SYSTEM TO PERMANENTLY RECORD DATA
    void saveDoctors() {
        ofstream file(DOCTOR_FILE);
        for (auto& d : doctors)
            file << d.id << "," << d.name << "," << d.specialization << "," << d.shift << "," << d.isAvailable << "\n";
        file.close();
    }

    void saveNurses() {
        ofstream file(NURSE_FILE);
        for (auto& n : nurses)
            file << n.id << "," << n.name << "," << n.shift << "," << n.isAvailable << "\n";
        file.close();
    }

    void savePatients() {
        ofstream file(PATIENT_FILE);
        for (auto& p : allPatients)
            file << p.id << "," << p.name << "," << p.age << "," << p.condition << ","
                 << p.isCritical << "," << p.assignedDoctorID << "\n";
        file.close();
    }

    void loadData() {
        // Load Doctors
        ifstream dFile(DOCTOR_FILE);
        Doctor d;
        char comma;
        while (dFile >> d.id >> comma && getline(dFile, d.name, ',') &&
               getline(dFile, d.specialization, ',') && getline(dFile, d.shift, ',') && dFile >> d.isAvailable) {
            doctors.push_back(d);
            doctorMap[d.id] = d;
        }
        dFile.close();

        // Load Patients
        ifstream pFile(PATIENT_FILE);
        Patient p;
        while (pFile >> p.id >> comma && getline(pFile, p.name, ',') && pFile >> p.age >> comma &&
               getline(pFile, p.condition, ',') && pFile >> p.isCritical >> comma && pFile >> p.assignedDoctorID) {
            allPatients.push_back(p);
            if (p.assignedDoctorID == -1) // Only unassigned go to queue
                patientQueue.push(p);
        }
        pFile.close();

        // Similar loading for nurses...
    }
};

int main() {
    HospitalSystem hs;
    int choice;

    do {
        cout << "\n===== HOSPITAL MANAGEMENT SYSTEM =====\n";
        cout << "1. Add Doctor\n2. Add Nurse\n3. Add Patient\n";
        cout << "4. View Waiting Queue\n5. Assign Doctor to Patient\n";
        cout << "6. Generate Duty Roster\n7. Display All Data\n0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: hs.addDoctor(); break;
            case 2: hs.addNurse(); break;
            case 3: hs.addPatient(); break;
            case 4: hs.viewWaitingQueue(); break;
            case 5: hs.assignDoctorToPatient(); break;
            case 6: hs.generateDutyRoster(); break;
            case 7: hs.displayAllData(); break;
            case 0: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice!= 0);

    return 0;
}