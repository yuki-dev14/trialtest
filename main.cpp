#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <iomanip>

using namespace std;

// Logger Singleton
class Logger {
private:
    static Logger* instance;
    ofstream logFile;
    Logger() { logFile.open("log.txt", ios::app); }
public:
    static Logger* getInstance() {
        if (!instance)
            instance = new Logger();
        return instance;
    }
    void log(const string& action) {
        time_t now = time(0);
        string dt = ctime(&now);
        if (!dt.empty() && dt.back() == '\n') dt.pop_back();
        logFile << "[" << dt << "] " << action << endl;
    }
    ~Logger() {
        if (logFile.is_open())
            logFile.close();
    }
};
Logger* Logger::instance = nullptr;

// Utility: trim whitespace
string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

// --- Display Strategy Pattern ---
class DisplayStrategy {
public:
    virtual void displayStudents() = 0;
    virtual void displayCourses() = 0;
    virtual ~DisplayStrategy() {}
};

class TableView : public DisplayStrategy {
public:
    void displayStudents() override {
        ifstream fin("students.txt");
        string line;
        cout << "\n"
             << left << setw(12) << "ID"
             << left << setw(22) << "Name"
             << left << setw(28) << "Email"
             << left << setw(6) << "Age"
             << left << setw(16) << "Program" << endl;
        cout << string(84, '-') << endl;
        while (getline(fin, line)) {
            istringstream iss(line);
            string id, name, email, age, program, pwd;
            getline(iss, id, ','); getline(iss, name, ','); getline(iss, email, ',');
            getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
            cout << left << setw(12) << id
                 << left << setw(22) << name
                 << left << setw(28) << email
                 << left << setw(6) << age
                 << left << setw(16) << program << endl;
        }
    }
    void displayCourses() override {
        ifstream fin("courses.txt");
        string line;
        cout << "\n"
             << left << setw(12) << "Code"
             << left << setw(32) << "Name"
             << left << setw(8) << "Units" << endl;
        cout << string(52, '-') << endl;
        while (getline(fin, line)) {
            istringstream iss(line);
            string code, name, units;
            getline(iss, code, ','); getline(iss, name, ','); getline(iss, units, ',');
            cout << left << setw(12) << code
                 << left << setw(32) << name
                 << left << setw(8) << units << endl;
        }
    }
};

class SummaryView : public DisplayStrategy {
public:
    void displayStudents() override {
        ifstream fin("students.txt");
        string line;
        cout << "\nStudent IDs and Names:\n";
        while (getline(fin, line)) {
            istringstream iss(line);
            string id, name;
            getline(iss, id, ','); getline(iss, name, ',');
            cout << id << " - " << name << endl;
        }
    }
    void displayCourses() override {
        ifstream fin("courses.txt");
        string line;
        cout << "\nCourse Codes and Names:\n";
        while (getline(fin, line)) {
            istringstream iss(line);
            string code, name;
            getline(iss, code, ','); getline(iss, name, ',');
            cout << code << " - " << name << endl;
        }
    }
};

// Global pointer for current strategy
DisplayStrategy* displayStrategy = nullptr;

// Let user choose display mode
void chooseDisplayStrategy() {
    int opt = 0;
    do {
        cout << "\nChoose display format:\n";
        cout << "1. Table View\n";
        cout << "2. Summary View\n";
        cout << "Select option: ";
        string input;
        getline(cin, input);

        // Validation: must be exactly "1" or "2"
        if (input == "1" || input == "2") {
            opt = stoi(input);
        } else {
            cout << "Invalid input. Please enter 1 or 2 only.\n";
            continue;
        }

        if (displayStrategy) delete displayStrategy;
        if (opt == 2)
            displayStrategy = new SummaryView();
        else
            displayStrategy = new TableView();
        break;
    } while (true);
}

// User base class
class User {
protected:
    string id, name, email, password;
public:
    User(const string& id, const string& name, const string& email, const string& password)
        : id(id), name(name), email(email), password(password) {}
    virtual ~User() {}
    string getId() const { return id; }
    string getName() const { return name; }
    virtual void menu() = 0;
    virtual bool handleOption(int opt) = 0;
};

// Admin class
class Admin : public User {
public:
    Admin(const string& id, const string& name, const string& email, const string& password)
        : User(id, name, email, password) {}
    void menu() override {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Student\n";
        cout << "2. Add Course\n";
        cout << "3. View All Students\n";
        cout << "4. View All Courses\n";
        cout << "5. View Students per Course\n";
        cout << "6. Edit Student\n";
        cout << "7. Edit Course\n";
        cout << "8. Delete Student\n";
        cout << "9. Delete Course\n";
        cout << "10. Change Display Mode\n";
        cout << "11. Logout\n";
    }
    bool handleOption(int opt) override;
};

// Student class
class Student : public User {
public:
    Student(const string& id, const string& name, const string& email, const string& password)
        : User(id, name, email, password) {}
    void menu() override {
        cout << "\n--- Student Menu ---\n";
        cout << "1. View Profile\n";
        cout << "2. Enroll in Course\n";
        cout << "3. View Enrolled Courses\n";
        cout << "4. Edit Profile\n";
        cout << "5. Drop Course\n";
        cout << "6. Change Display Mode\n";
        cout << "7. Logout\n";
    }
    bool handleOption(int opt) override;
};

// --- File helpers ---
bool studentExists(const string& id) {
    ifstream fin("students.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid;
        getline(iss, sid, ',');
        if (trim(sid) == id) return true;
    }
    return false;
}
bool courseExists(const string& code) {
    ifstream fin("courses.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode;
        getline(iss, ccode, ',');
        if (trim(ccode) == code) return true;
    }
    return false;
}
bool isEnrolled(const string& sid, const string& ccode) {
    ifstream fin("enrollments.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, code;
        getline(iss, id, ',');
        getline(iss, code, ',');
        if (trim(id) == sid && trim(code) == ccode) return true;
    }
    return false;
}

// --- Helpers for validation and case-insensitive checks ---
bool isAlphanumeric(const string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')))
            return false;
    }
    return true;
}
bool isLettersOnly(const string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == ' '))
            return false;
    }
    return true;
}
bool isWholeNumber(const string& s) {
    if (s.empty()) return false;
    for (size_t i = 0; i < s.size(); ++i) {
        if (!(s[i] >= '0' && s[i] <= '9')) return false;
    }
    return true;
}
bool equalsIgnoreCase(const string& a, const string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        char ca = a[i], cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}
bool studentExistsCI(const string& id) {
    ifstream fin("students.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid;
        getline(iss, sid, ',');
        if (equalsIgnoreCase(trim(sid), trim(id))) return true;
    }
    return false;
}
bool courseExistsCI(const string& code) {
    ifstream fin("courses.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode;
        getline(iss, ccode, ',');
        if (equalsIgnoreCase(trim(ccode), trim(code))) return true;
    }
    return false;
}

// --- Admin Features ---
void addStudent() {
    string id, name, email, age, program, password;

    // Student ID input and validation
    bool validId = false;
    do {
        cout << "Enter Student ID: ";
        getline(cin, id);

        // Check for spaces in the input
        if (id.find(' ') != string::npos) {
            cout << "Student ID must not contain spaces.\n";
            continue;
        }

        if (!isAlphanumeric(id)) {
            cout << "Student ID must be strictly alphanumeric.\n";
        } else if (studentExistsCI(id)) {
            cout << "Student ID already exists.\n";
        } else {
            validId = true;
        }
    } while (!validId);

    // Name input and validation
    bool validName = false;
    do {
        cout << "Enter Name: ";
        getline(cin, name);
        if (!isLettersOnly(name)) {
            cout << "Name should be letters only.\n";
        } else {
            validName = true;
        }
    } while (!validName);

    cout << "Enter Email: ";
    getline(cin, email);

    // Age input and validation
    bool validAge = false;
    do {
        cout << "Enter Age: ";
        getline(cin, age);
        if (!isWholeNumber(age)) {
            cout << "Age should be a whole number.\n";
        } else {
            validAge = true;
        }
    } while (!validAge);

    cout << "Enter Program: ";
    getline(cin, program);
    cout << "Enter Password: ";
    getline(cin, password);

    ofstream fout("students.txt", ios::app);
    fout << id << "," << name << "," << email << "," << age << "," << program << "," << password << endl;
    Logger::getInstance()->log("Admin added student " + id);
    cout << "Student added.\n";
}
void addCourse() {
    string code, name, units;
    bool validCode = false;
    do {
        cout << "Enter Course Code: ";
        getline(cin, code);
        if (code.find(' ') != string::npos) {
            cout << "Course code must not contain spaces.\n";
            continue;
        }
        if (!isAlphanumeric(code)) {
            cout << "Course code must be strictly alphanumeric.\n";
        } else if (courseExistsCI(code)) {
            cout << "Course code already exists (case-insensitive).\n";
        } else {
            validCode = true;
        }
    } while (!validCode);

    cout << "Enter Course Name: ";
    getline(cin, name);

    bool validUnits = false;
    do {
        cout << "Enter Units: ";
        getline(cin, units);
        if (!isWholeNumber(units)) {
            cout << "Units should be a whole number.\n";
        } else {
            validUnits = true;
        }
    } while (!validUnits);

    ofstream fout("courses.txt", ios::app);
    fout << code << "," << name << "," << units << endl;
    Logger::getInstance()->log("Admin added course " + code);
    cout << "Course added.\n";
}
void viewAllStudents() {
    if (!displayStrategy) chooseDisplayStrategy();
    displayStrategy->displayStudents();
}
void viewAllCourses() {
    if (!displayStrategy) chooseDisplayStrategy();
    displayStrategy->displayCourses();
}
void viewStudentsPerCourse() {
    string inputCode;
    bool valid = false;
    do {
        cout << "Enter Course Code: ";
        getline(cin, inputCode);

        if (!courseExistsCI(inputCode)) {
            cout << "Course not found. Please try again.\n";
        } else {
            valid = true;
        }
    } while (!valid);

    cout << "Students enrolled in " << inputCode << ":\n";
    ifstream fin("enrollments.txt");
    string line;
    bool found = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid, ccode;
        getline(iss, sid, ',');
        getline(iss, ccode, ',');
        if (equalsIgnoreCase(trim(ccode), trim(inputCode))) {
            // Get student name
            ifstream sfin("students.txt");
            string sline;
            while (getline(sfin, sline)) {
                istringstream siss(sline);
                string id, name;
                getline(siss, id, ',');
                getline(siss, name, ',');
                if (trim(id) == trim(sid)) {
                    cout << id << " - " << name << endl;
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) cout << "No students enrolled in this course.\n";
}
void editStudent() {
    string id;
    bool found = false;
    do {
        cout << "Enter Student ID to edit: ";
        getline(cin, id);
        if (!studentExistsCI(id)) {
            cout << "Student not found (not case sensitive). Please try again.\n";
        } else {
            found = true;
        }
    } while (!found);

    ifstream fin("students.txt");
    ofstream fout("students_tmp.txt");
    string line;
    bool edited = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid, name, email, age, program, pwd;
        getline(iss, sid, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (equalsIgnoreCase(trim(sid), trim(id))) {
            string n, e, a, p;
            // Name validation
            do {
                cout << "Edit Name (" << name << "): ";
                getline(cin, n);
                if (n.empty()) break;
                if (!isLettersOnly(n)) {
                    cout << "Name should be letters only.\n";
                } else {
                    name = n;
                    break;
                }
            } while (true);

            cout << "Edit Email (" << email << "): ";
            getline(cin, e);
            if (!e.empty()) email = e;

            // Age validation
            do {
                cout << "Edit Age (" << age << "): ";
                getline(cin, a);
                if (a.empty()) break;
                if (!isWholeNumber(a)) {
                    cout << "Age should be a whole number.\n";
                } else {
                    age = a;
                    break;
                }
            } while (true);

            cout << "Edit Program (" << program << "): ";
            getline(cin, p);
            if (!p.empty()) program = p;

            fout << sid << "," << name << "," << email << "," << age << "," << program << "," << pwd << endl;
            edited = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("students.txt"); rename("students_tmp.txt", "students.txt");
    if (edited) {
        Logger::getInstance()->log("Admin edited student " + id);
        cout << "Student updated.\n";
    }
}
void editCourse() {
    string code;
    bool found = false;
    do {
        cout << "Enter Course Code to edit: ";
        getline(cin, code);
        if (!courseExistsCI(code)) {
            cout << "Course not found (not case sensitive). Please try again.\n";
        } else {
            found = true;
        }
    } while (!found);

    ifstream fin("courses.txt");
    ofstream fout("courses_tmp.txt");
    string line;
    bool edited = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode, name, units;
        getline(iss, ccode, ','); getline(iss, name, ','); getline(iss, units, ',');
        if (equalsIgnoreCase(trim(ccode), trim(code))) {
            string n, u;
            cout << "Edit Name (" << name << "): ";
            getline(cin, n);
            if (!n.empty()) name = n;

            // Units validation
            do {
                cout << "Edit Units (" << units << "): ";
                getline(cin, u);
                if (u.empty()) break;
                if (!isWholeNumber(u)) {
                    cout << "Units should be a whole number.\n";
                } else {
                    units = u;
                    break;
                }
            } while (true);

            fout << ccode << "," << name << "," << units << endl;
            edited = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("courses.txt"); rename("courses_tmp.txt", "courses.txt");
    if (edited) {
        Logger::getInstance()->log("Admin edited course " + code);
        cout << "Course updated.\n";
    }
}
void deleteStudent() {
    string id;
    bool found = false;
    do {
        cout << "Enter Student ID to delete: ";
        getline(cin, id);
        if (!studentExistsCI(id)) {
            cout << "Student not found (not case sensitive). Please try again.\n";
        } else {
            found = true;
        }
    } while (!found);

    ifstream fin("students.txt");
    ofstream fout("students_tmp.txt");
    string line;
    bool deleted = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid;
        getline(iss, sid, ',');
        if (equalsIgnoreCase(trim(sid), trim(id))) {
            deleted = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("students.txt"); rename("students_tmp.txt", "students.txt");
    // Remove enrollments
    ifstream ein("enrollments.txt");
    ofstream eout("enrollments_tmp.txt");
    while (getline(ein, line)) {
        istringstream iss(line);
        string sid, ccode;
        getline(iss, sid, ','); getline(iss, ccode, ',');
        if (!equalsIgnoreCase(trim(sid), trim(id))) eout << sid << "," << ccode << endl;
    }
    ein.close(); eout.close();
    remove("enrollments.txt"); rename("enrollments_tmp.txt", "enrollments.txt");
    if (deleted) {
        Logger::getInstance()->log("Admin deleted student " + id);
        cout << "Student deleted.\n";
    }
}
void deleteCourse() {
    string code;
    bool found = false;
    do {
        cout << "Enter Course Code to delete: ";
        getline(cin, code);
        if (!courseExistsCI(code)) {
            cout << "Course not found (not case sensitive). Please try again.\n";
        } else {
            found = true;
        }
    } while (!found);

    ifstream fin("courses.txt");
    ofstream fout("courses_tmp.txt");
    string line;
    bool deleted = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string ccode;
        getline(iss, ccode, ',');
        if (equalsIgnoreCase(trim(ccode), trim(code))) {
            deleted = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("courses.txt"); rename("courses_tmp.txt", "courses.txt");
    // Remove enrollments
    ifstream ein("enrollments.txt");
    ofstream eout("enrollments_tmp.txt");
    while (getline(ein, line)) {
        istringstream iss(line);
        string sid, ccode;
        getline(iss, sid, ','); getline(iss, ccode, ',');
        if (!equalsIgnoreCase(trim(ccode), trim(code))) eout << sid << "," << ccode << endl;
    }
    ein.close(); eout.close();
    remove("enrollments.txt"); rename("enrollments_tmp.txt", "enrollments.txt");
    if (deleted) {
        Logger::getInstance()->log("Admin deleted course " + code);
        cout << "Course deleted.\n";
    }
}

// --- Student Features ---
void viewProfile(const string& id) {
    ifstream fin("students.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string sid, name, email, age, program, pwd;
        getline(iss, sid, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (trim(sid) == id) {
            cout << "\nID: " << sid << "\nName: " << name << "\nEmail: " << email
                 << "\nAge: " << age << "\nProgram: " << program << endl;
            return;
        }
    }
}
void enrollCourse(const string& sid) {
    cout << "Available courses:\n";
    ifstream fin("courses.txt");
    string line;
    while (getline(fin, line)) {
        istringstream iss(line);
        string code, name, units;
        getline(iss, code, ','); getline(iss, name, ','); getline(iss, units, ',');
        cout << code << " - " << name << " (" << units << " units)\n";
    }
    string code;
    bool valid = false;
    do {
        cout << "Enter Course Code to enroll: ";
        getline(cin, code);
        if (!courseExistsCI(code)) {
            cout << "Course not found (not case sensitive). Please try again.\n";
        } else {
            // Check if already enrolled (case-insensitive)
            bool alreadyEnrolled = false;
            ifstream ein("enrollments.txt");
            string eline;
            while (getline(ein, eline)) {
                istringstream eiss(eline);
                string eid, ecode;
                getline(eiss, eid, ','); getline(eiss, ecode, ',');
                if (equalsIgnoreCase(trim(eid), trim(sid)) && equalsIgnoreCase(trim(ecode), trim(code))) {
                    alreadyEnrolled = true;
                    break;
                }
            }
            if (alreadyEnrolled) {
                cout << "You are already enrolled in this course. Please choose another course.\n";
            } else {
                valid = true;
            }
        }
    } while (!valid);
    ofstream fout("enrollments.txt", ios::app);
    fout << sid << "," << code << endl;
    Logger::getInstance()->log("Student " + sid + " enrolled in " + code);
    cout << "Enrolled in course.\n";
}
void viewEnrolledCourses(const string& sid) {
    cout << "Enrolled courses:\n";
    ifstream fin("enrollments.txt");
    string line;
    bool found = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, code;
        getline(iss, id, ','); getline(iss, code, ',');
        if (trim(id) == sid) {
            // Get course name
            ifstream cfin("courses.txt");
            string cline;
            while (getline(cfin, cline)) {
                istringstream ciss(cline);
                string ccode, cname, units;
                getline(ciss, ccode, ','); getline(ciss, cname, ','); getline(ciss, units, ',');
                if (trim(ccode) == trim(code)) {
                    cout << ccode << " - " << cname << " (" << units << " units)\n";
                    found = true;
                    break;
                }
            }
        }
    }
    if (!found) cout << "None.\n";
}
void editProfile(const string& sid) {
    ifstream fin("students.txt");
    ofstream fout("students_tmp.txt");
    string line;
    bool edited = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, name, email, age, program, pwd;
        getline(iss, id, ','); getline(iss, name, ','); getline(iss, email, ',');
        getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
        if (trim(id) == sid) {
            string n, e, a;
            // Name validation
            do {
                cout << "Edit Name (" << name << "): ";
                getline(cin, n);
                if (n.empty()) break;
                if (!isLettersOnly(n)) {
                    cout << "Name should be letters only.\n";
                } else {
                    name = n;
                    break;
                }
            } while (true);

            cout << "Edit Email (" << email << "): ";
            getline(cin, e);
            if (!e.empty()) email = e;

            // Age validation
            do {
                cout << "Edit Age (" << age << "): ";
                getline(cin, a);
                if (a.empty()) break;
                if (!isWholeNumber(a)) {
                    cout << "Age should be a whole number.\n";
                } else {
                    age = a;
                    break;
                }
            } while (true);

            fout << id << "," << name << "," << email << "," << age << "," << program << "," << pwd << endl;
            edited = true;
        } else {
            fout << line << endl;
        }
    }
    fin.close(); fout.close();
    remove("students.txt"); rename("students_tmp.txt", "students.txt");
    if (edited) {
        Logger::getInstance()->log("Student " + sid + " edited profile");
        cout << "Profile updated.\n";
    }
}
void dropCourse(const string& sid) {
    string code;
    bool valid = false;
    do {
        cout << "Enter Course Code to drop: ";
        getline(cin, code);
        if (!courseExistsCI(code)) {
            cout << "Course not found (not case sensitive). Please try again.\n";
        } else {
            // Check if enrolled (case-insensitive)
            bool enrolled = false;
            ifstream ein("enrollments.txt");
            string eline;
            while (getline(ein, eline)) {
                istringstream eiss(eline);
                string eid, ecode;
                getline(eiss, eid, ','); getline(eiss, ecode, ',');
                if (equalsIgnoreCase(trim(eid), trim(sid)) && equalsIgnoreCase(trim(ecode), trim(code))) {
                    enrolled = true;
                    break;
                }
            }
            if (!enrolled) {
                cout << "Not enrolled in this course.\n";
            } else {
                valid = true;
            }
        }
    } while (!valid);

    ifstream fin("enrollments.txt");
    ofstream fout("enrollments_tmp.txt");
    string line;
    bool dropped = false;
    while (getline(fin, line)) {
        istringstream iss(line);
        string id, ccode;
        getline(iss, id, ','); getline(iss, ccode, ',');
        if (equalsIgnoreCase(trim(id), trim(sid)) && equalsIgnoreCase(trim(ccode), trim(code))) {
            dropped = true;
        } else {
            fout << id << "," << ccode << endl;
        }
    }
    fin.close(); fout.close();
    remove("enrollments.txt"); rename("enrollments_tmp.txt", "enrollments.txt");
    if (dropped) {
        Logger::getInstance()->log("Student " + sid + " dropped course " + code);
        cout << "Dropped course.\n";
    }
}

// --- Admin Option Handler ---
bool Admin::handleOption(int opt) {
    switch (opt) {
        case 1: addStudent(); break;
        case 2: addCourse(); break;
        case 3: viewAllStudents(); break;
        case 4: viewAllCourses(); break;
        case 5: viewStudentsPerCourse(); break;
        case 6: editStudent(); break;
        case 7: editCourse(); break;
        case 8: deleteStudent(); break;
        case 9: deleteCourse(); break;
        case 10: chooseDisplayStrategy(); break;
        case 11:
            Logger::getInstance()->log("Admin logged out");
            return false;
        default:
            cout << "Invalid option.\n";
    }
    return true;
}

// --- Student Option Handler ---
bool Student::handleOption(int opt) {
    switch (opt) {
        case 1: viewProfile(getId()); break;
        case 2: enrollCourse(getId()); break;
        case 3: viewEnrolledCourses(getId()); break;
        case 4: editProfile(getId()); break;
        case 5: dropCourse(getId()); break;
        case 6: chooseDisplayStrategy(); break;
        case 7: Logger::getInstance()->log(getId() + " logged out"); return false;
        default: cout << "Invalid option.\n";
    }
    return true;
}

// --- Login ---
unique_ptr<User> login() {
    bool loggedIn = false;
    unique_ptr<User> user;
    do {
        string username, password;
        cout << "Username (admin or student ID): ";
        getline(cin, username);
        cout << "Password: ";
        getline(cin, password);

        // Admin credentials
        if (username == "admin" && password == "admin123") {
            Logger::getInstance()->log("Admin logged in");
            user.reset(new Admin("admin", "Administrator", "admin@school.edu", "admin123"));
            loggedIn = true;
        } else {
            ifstream fin("students.txt");
            string line;
            while (getline(fin, line)) {
                istringstream iss(line);
                string id, name, email, age, program, pwd;
                getline(iss, id, ','); getline(iss, name, ','); getline(iss, email, ',');
                getline(iss, age, ','); getline(iss, program, ','); getline(iss, pwd, ',');
                if (equalsIgnoreCase(trim(id), trim(username)) && trim(pwd) == password) {
                    Logger::getInstance()->log("Student " + id + " logged in");
                    user.reset(new Student(id, name, email, pwd));
                    loggedIn = true;
                    break;
                }
            }
        }
        if (!loggedIn) cout << "Login failed: Invalid credentials. Try again.\n";
    } while (!loggedIn);
    return user;
}

int main() {
    try {
        cout << "=== Student Management System ===\n";
        auto user = login();
        bool running = true;
        while (running) {
            user->menu();
            cout << "Select option: ";
            string optstr;
            getline(cin, optstr);
            int opt = 0;
            bool valid = true;

            // Check if Admin or Student for menu range
            Admin* adminPtr = dynamic_cast<Admin*>(user.get());
            int minOpt = 1, maxOpt = adminPtr ? 11 : 7;

            // Only digits, no spaces, and within allowed range
            if (optstr.empty() || optstr.find_first_not_of("0123456789") != string::npos)
                valid = false;
            else {
                opt = stoi(optstr);
                if (opt < minOpt || opt > maxOpt) valid = false;
            }

            if (!valid) {
                cout << "Invalid input. Please enter a number from " << minOpt << " to " << maxOpt << " only.\n";
                continue;
            }

            running = user->handleOption(opt);
        }
    } catch (const exception& ex) {
        cerr << ex.what() << endl;
        Logger::getInstance()->log(string("Login failed: ") + ex.what());
    }
    return 0;
}
