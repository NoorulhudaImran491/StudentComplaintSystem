#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int MAX_USERS = 100;
const int MAX_COMPLAINTS = 100;
const string ADMIN_USERNAME = "Nadeem"; 
const string ADMIN_PASSWORD = "Nadeem112233"; 

// Base class for shared functionality
class User {
public:
    int userID; // Added user ID field
    string username;
    string password;
    string fullName;
    string email;
    string department;

    virtual void readFromFile(ifstream &file) = 0;
    virtual void writeToFile(ofstream &file) const = 0;
};

class Student : public User {
public:
    void readFromFile(ifstream &file) override {
        file >> userID >> username >> password >> fullName >> email >> department; // Added userID read
    }

    void writeToFile(ofstream &file) const override {
        file << userID << " " << username << " " << password << " " << fullName << " " << email << " " << department << endl; // Added userID write
    }
};

class Complaint {
private:
    static int totalComplaints;

public:
    int userID; 
    string studentName;
    string studentDepartment;
    string username;
    string category;
    string description;
    bool resolved;
    string feedback;
    int complaintNumber = 1;

    Complaint() : resolved(false), complaintNumber(complaintNumber++) {}

    void readFromFile(ifstream &file) {
        file >> userID >> studentName >> studentDepartment >> username >> category >> description >> resolved >> feedback >> complaintNumber;
        if (file.fail()) return;
        if (complaintNumber >= totalComplaints) {
            totalComplaints = complaintNumber + 1;
        }
    }

    void writeToFile(ofstream &file) const {
        file << userID << " " << studentName << " " << studentDepartment << " " << username << " " << category << " " << description << " "
             << (resolved ? 1 : 0) << " " << feedback << " " << complaintNumber << endl;
    }

    static int getTotalComplaints() {
        return totalComplaints;
    }
};

int Complaint::totalComplaints = 0;

template <typename T>
void readItems(T items[], int& itemCount, const string& filename, int maxSize) {
    ifstream file(filename);
    itemCount = 0;
    if (file.is_open()) {
        while (itemCount < maxSize && !file.eof()) {
            items[itemCount].readFromFile(file);
            if (file.fail()) break;  
            itemCount++;
        }
        file.close();
    } else {
        ofstream newFile(filename);
        if (!newFile.is_open()) {
            cout << "Unable to create file: " << filename << endl;
        }
    }
}

template <typename T>
void writeItems(const T items[], int itemCount, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (int i = 0; i < itemCount; ++i) {
            items[i].writeToFile(file);
        }
        file.close();
    } else {
        cout << "Unable to open file for writing: " << filename << endl;
    }
}

template <typename T>
void addItem(T items[], int& itemCount, const T& item, int maxSize) {
    if (itemCount < maxSize) {
        items[itemCount++] = item;
    } else {
        cout << "Item limit reached. Cannot add more items." << endl;
    }
}

void displayHeader(const string& header) {
    cout << "\n=============================================\n";
    cout << "| " << header << "\n";
}

void displayMenu() {
    cout << "=============================================\n";
    cout << "|           Student Complaint System        |\n";
    cout << "=============================================\n";
    cout << "| 1. Register                               |\n";
    cout << "| 2. Login and Submit a Complaint           |\n";
    cout << "| 3. View Your Complaints                   |\n";
    cout << "| 4. Admin Login                            |\n"; 
    cout << "| 5. Provide Feedback                       |\n";
    cout << "| 6. Exit                                   |\n";
    cout << "=============================================\n";
}

// Function to check if a string contains only alphabetic characters
bool isAlphaCharacter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isAlphaString(const string& str) {
    for (char c : str) {
        if (!isAlphaCharacter(c)) {
            return false;
        }
    }
    return true;
}

bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    return atPos != string::npos && dotPos != string::npos && atPos < dotPos;
}

bool isEmptyString(const string& str) {
    return str.empty();
}

bool login(const Student students[], int studentCount, const string& username, const string& password, int& loggedInUserID) { 
    for (int i = 0; i < studentCount; ++i) {
        if (students[i].username == username && students[i].password == password) {
            loggedInUserID = students[i].userID; 
            return true;
        }
    }
    return false;
}
bool adminLogin(const string& username, const string& password) {
    return username == ADMIN_USERNAME && password == ADMIN_PASSWORD;
}

void displayAdminMenu() {
    cout << "=============================================\n";
    cout << "|                Admin Menu                 |\n";
    cout << "=============================================\n";
    cout << "| 1. View All Complaints                    |\n";
    cout << "| 2. Resolve a Complaint                    |\n";
    cout << "| 3. Logout                                 |\n";
    cout << "=============================================\n";
}

void adminFunctions(Complaint complaints[], int complaintCount, Student students[], int studentCount) {
    int choice;
    while (true) {
        displayAdminMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); 

        switch (choice) {
            case 1: {
                for (int i = 0; i < complaintCount; ++i) {
                    cout << "Complaint Number: " << complaints[i].complaintNumber << endl;
                    
                    string studentName;
                    string studentDepartment;
                    for (int j = 0; j < studentCount; ++j) {
                        if (students[j].userID == complaints[i].userID) {
                            studentName = students[j].username;
                            studentDepartment = students[j].department;
                            break;
                        }
                    }
                    cout << "Student Name: " << studentName << endl;
                    cout << "Department: " << studentDepartment << endl;
                    cout << "Category: " << complaints[i].category << endl;
                    cout << "Description: " << complaints[i].description << endl;
                    cout << "Resolved: " << (complaints[i].resolved ? "Yes" : "No") << endl;
                    if (complaints[i].resolved) {
                        cout << "Feedback: " << complaints[i].feedback << endl;
                    }
                    cout << "---------------------------------------------" << endl;
                }
                break;
            }
            
case 2: {
    int userID;
    cout << "Enter the user ID to resolve complaints: ";
    cin >> userID;
    cin.ignore();

    bool found = false;
    for (int i = 0; i < complaintCount; ++i) {
        if (complaints[i].userID == userID) {
            // Display the complaint details
            cout << "Complaint Number: " << complaints[i].complaintNumber << endl;
            cout << "Category: " << complaints[i].category << endl;
            cout << "Description: " << complaints[i].description << endl;
            // Option for admin to solve the problem
            char solveOption;
            cout << "Do you want to solve this problem? (Y/N): ";
            cin >> solveOption;
            cin.ignore(); // Clear newline character from the input buffer
            if (solveOption == 'Y' || solveOption == 'y') {
                complaints[i].resolved = true;
                cout << "The problem has been solved." << endl;
            }

            found = true;
            break;
        }
    }

    if (!found) {
        cout << "No matching complaint found." << endl;
    }
    break;
}

            case 3: {
                cout << "Admin logged out." << endl;
                return; 
            }
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
}

int main() {
    Student students[MAX_USERS];
    int studentCount = 0;
    Complaint complaints[MAX_COMPLAINTS];
    int complaintCount = 0;
    int loggedInUserID; 

    readItems(students, studentCount, "students.txt", MAX_USERS);
    readItems(complaints, complaintCount, "complaints.txt", MAX_COMPLAINTS);

    int choice;
    while (true) {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); 

        if (choice == 6) {
            cout << "Exiting the system." << endl;
            break;
        }

        switch (choice) {
            case 1: {
                if (studentCount >= MAX_USERS) {
                    cout << "User limit reached. Cannot register more users." << endl;
                    break;
                }
                Student newStudent;
                cout << "Enter username: ";
                getline(cin, newStudent.username);
                
                // Check if the username is empty
                if (isEmptyString(newStudent.username)) {
                    cout << "Username cannot be empty." << endl;
                    break; 
                }

                // Check if the username contains only alphabetic characters
                if (!isAlphaString(newStudent.username)) {
                    cout << "Username should contain only alphabetic characters." << endl;
                    break;
                }

                // Check for duplicate username
                bool duplicateUser = false;
                for (int i = 0; i < studentCount; ++i) {
                    if (students[i].username == newStudent.username) {
                        duplicateUser = true;
                        cout << "Username already exists. Please choose another username." << endl;
                        break;
                    }
                }
                if (duplicateUser) {
                    break; // Exit registration process if duplicate username found
                }

                cout << "Enter password: ";
                getline(cin, newStudent.password);

                // Check if the password is empty
                if (isEmptyString(newStudent.password)) {
                    cout << "Password cannot be empty." << endl;
                    break;
                }

                cout << "Enter full name: ";
                getline(cin, newStudent.fullName);

                // Check if the full name is empty
                if (isEmptyString(newStudent.fullName)) {
                    cout << "Full name cannot be empty." << endl;
                    break; 
                }

                // Check if the full name contains only alphabetic characters
                if (!isAlphaString(newStudent.fullName)) {
                    cout << "Full name should contain only alphabetic characters." << endl;
                    break;
                }

                cout << "Enter email: ";
                getline(cin, newStudent.email);

                // Check if the email is empty
                if (isEmptyString(newStudent.email)) {
                    cout << "Email cannot be empty." << endl;
                    break;
                }

                // Check if the email address is valid
                if (!isValidEmail(newStudent.email)) {
                    cout << "Invalid email address." << endl;
                    break; 
                }

                cout << "Enter department: ";
                getline(cin, newStudent.department);

                // Check if the department is empty
                if (isEmptyString(newStudent.department)) {
                    cout << "Department cannot be empty." << endl;
                    break; 
                }

                newStudent.userID = studentCount + 1; // Assign unique userID
                addItem(students, studentCount, newStudent, MAX_USERS);
                writeItems(students, studentCount, "students.txt");
                displayHeader("Registration Successful");
                break;
            }

            case 2: {
            string username;
            cout << "Enter username: ";
            getline(cin, username);

           // Check if the username field is empty
           if (isEmptyString(username)) {
           cout << "Username cannot be empty." << endl;
           break;
           }

           string password;
           cout << "Enter password: ";
           getline(cin, password);

           // Check if the password field is empty
           if (isEmptyString(password)) {
           cout << "Password cannot be empty." << endl;
           break;
           }

           if (login(students, studentCount, username, password, loggedInUserID)) {
           cout << "Logged in successfully! Welcome." << endl; 

          Complaint newComplaint;
          newComplaint.userID = loggedInUserID; 

          int categoryChoice;
          cout << "Select category of complaint:" << endl;
          cout << "1. Staff" << endl;
          cout << "2. Hostel" << endl;
          cout << "3. Faculty" << endl;
          cout << "4. Other" << endl; 
          cout << "Enter your choice (1-4): ";
          cin >> categoryChoice;
          cin.ignore(); 

        // Check if the category choice is valid
         switch (categoryChoice) {
            case 1:
                newComplaint.category = "Staff";
                break;
            case 2:
                newComplaint.category = "Hostel";
                break;
            case 3:
                newComplaint.category = "Faculty";
                break;
            case 4:
            	newComplaint.category = "Other";
                break;
                // Check if the category is empty
                if (isEmptyString(newComplaint.category)) {
                    cout << "Category cannot be empty." << endl;
                    break; 
                }
                break;
            default:
                cout << "Invalid choice. Please select a valid category." << endl;
                break; 
        }

            // Check if the category was set
            if (newComplaint.category.empty()) {
            break;
            }

            cout << "Enter description of complaint: ";
            getline(cin, newComplaint.description);

           // Check if the description is empty
           if (isEmptyString(newComplaint.description)) {
            cout << "Description cannot be empty." << endl;
            break; 
            }

            //validation check on the description
            if (newComplaint.description.length() < 10) {
            cout << "Description is too short. Please provide more details." << endl;
            break; 
            }
            
           addItem(complaints, complaintCount, newComplaint, MAX_COMPLAINTS);
           writeItems(complaints, complaintCount, "complaints.txt");
           displayHeader("Complaint Submitted Successfully");
           } else {
           cout << "Invalid username or password. Please try again." << endl;
           }
           break;
           }


            case 3: {
                string username;
                cout << "Enter username: ";
                getline(cin, username);
                
                // Check if the username field is empty
                if (isEmptyString(username)) {
                    cout << "Username cannot be empty." << endl;
                    break;
                }

                string password;
                cout << "Enter password: ";
                getline(cin, password);
                
                // Check if the password field is empty
                if (isEmptyString(password)) {
                    cout << "Password cannot be empty." << endl;
                    break;
                }
                
                if (login(students, studentCount, username, password, loggedInUserID)) {
                    displayHeader("Your Complaints");
                    bool complaintsFound = false;
                    for (int i = 0; i < complaintCount; ++i) {
                        if (complaints[i].userID == loggedInUserID) { 
                            complaintsFound = true;
                            cout << "UserID: " <<  complaints[i].userID << endl;
                            cout << "Category: " << complaints[i].category << endl;
                            cout << "Description: " << complaints[i].description << endl;
                            cout << "Status: " << (complaints[i].resolved ? "Resolved" : "Pending") << endl;
                            cout << "---------------------------------------------\n";
                        }
                    }
                    if (!complaintsFound) {
                        cout << "No complaints found for this user." << endl;
                    }
                } else {
                    cout << "Invalid username or password. Please try again." << endl;
                }
                break;
            }
      
      case 4: {
    string adminUsername, adminPassword;
    cout << "Enter admin username: ";
    getline(cin, adminUsername);
    cout << "Enter admin password: ";
    getline(cin, adminPassword);
    if (adminLogin(adminUsername, adminPassword)) {
        cout << "Admin logged in successfully!" << endl;
        adminFunctions(complaints, complaintCount, students, studentCount);
    } else {
        cout << "Invalid admin username or password." << endl;
    }
    break;
}

case 5: {
    string username;
    cout << "Enter username: ";
    getline(cin, username);

    // Check if the username field is empty
    if (isEmptyString(username)) {
        cout << "Username cannot be empty." << endl;
        break;
    }

    string password;
    cout << "Enter password: ";
    getline(cin, password);

    // Check if the password field is empty
    if (isEmptyString(password)) {
        cout << "Password cannot be empty." << endl;
        break;
    }

    int userID;
    if (login(students, studentCount, username, password, userID)) {
        bool feedbackGiven = false;
        displayHeader("Provide Feedback");
        for (int i = 0; i < complaintCount; ++i) {
            if (complaints[i].userID == userID && !complaints[i].resolved) {
                feedbackGiven = true;
                cout << "Enter your feedback: ";
                getline(cin, complaints[i].feedback);

                // Mark the complaint as resolved
                complaints[i].resolved = true;

                // Check if the feedback is empty
                if (isEmptyString(complaints[i].feedback)) {
                    cout << "Feedback cannot be empty." << endl;
                } else {
                    string satisfactionInput;
                    cout << "| Are you satisfied with the way the problem was handled? (Yes/No): ";
                    cin >> satisfactionInput;
                    cin.ignore(); // Clear newline character from the input buffer
                    if (satisfactionInput == "Yes" || satisfactionInput == "yes") {
                        cout << "| Thank you for your feedback!\n";
                    } else if (satisfactionInput == "No" || satisfactionInput == "no") {
                        string reason;
                        cout << "| We're sorry to hear that. Please tell us why you are not satisfied: ";
                        getline(cin, reason);
                        cout << "| Thank you for your feedback. We apologize for any inconvenience caused.\n";
                    } else {
                        cout << "| Invalid input. Please enter 'Yes' or 'No'.\n";
                    }
                }
            }
        }
        if (!feedbackGiven) {
            cout << "No unresolved complaints found for this user." << endl;
        }
    } else {
        cout << "Invalid username or password. Please try again." << endl;
    }
    break;
    }
  }
 }
}
