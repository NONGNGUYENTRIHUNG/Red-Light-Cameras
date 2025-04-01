#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

/*
 * CameraRecord class stores all data fields from a single line in the input file
 */
class CameraRecord {
public:
    string intersection;
    string address;
    int cameraNumber;
    string date;
    int violations;
    string neighborhood;

    CameraRecord(string inter, string addr, int camNum, string dt, int viol, string neigh) 
        : intersection(inter), address(addr), cameraNumber(camNum), date(dt), 
          violations(viol), neighborhood(neigh) {}
};

/*
 * NeighborhoodInfo class aggregates data for each neighborhood
 */
class NeighborhoodInfo {
public:
    string name;
    int cameraCount;
    int violationCount;

    NeighborhoodInfo(string n) : name(n), cameraCount(0), violationCount(0) {}
};

/*
 * Custom string splitting without range-based for loops or substr
 */
vector<string> splitString(const string &s, char delimiter) {
    vector<string> tokens;
    string token;
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += s[i];
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

/*
 * Manual string to lowercase conversion without range-based for
 */
string toLower(const string &s) {
    string lower;
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        if (c >= 'A' && c <= 'Z') {
            lower += c + ('a' - 'A');
        } else {
            lower += c;
        }
    }
    return lower;
}

/*
 * Reads camera data without emplace_back
 */
vector<CameraRecord> readCameraData(const string &filename) {
    vector<CameraRecord> records;
    ifstream fileIn(filename);
    if (!fileIn.is_open()) {
        cout << "Failed to open file: " << filename << endl;
        return records;
    }

    string line;
    while (getline(fileIn, line)) {
        vector<string> tokens = splitString(line, ',');
        if (tokens.size() >= 6) {
            try {
                int cameraNumber = stoi(tokens[2]);
                int violations = stoi(tokens[4]);
                records.push_back(CameraRecord(tokens[0], tokens[1], cameraNumber, 
                                  tokens[3], violations, tokens[5]));
            } catch (...) {
                cout << "Error parsing line: " << line << endl;
            }
        }
    }

    fileIn.close();
    return records;
}

/*
 * Date parsing without substr
 */
void parseDate(const string &date, string &month, string &day, string &year) {
    size_t dash1 = date.find('-');
    size_t dash2 = date.find('-', dash1 + 1);
    
    year = "";
    for (size_t i = 0; i < dash1; i++) {
        year += date[i];
    }
    
    month = "";
    for (size_t i = dash1 + 1; i < dash2; i++) {
        month += date[i];
    }
    
    day = "";
    for (size_t i = dash2 + 1; i < date.length(); i++) {
        day += date[i];
    }
}

void dataOverview(const vector<CameraRecord> &records) {
    cout << "Read file with " << records.size() << " records." << endl;

    vector<int> uniqueCameras;
    for (size_t i = 0; i < records.size(); i++) {
        bool found = false;
        for (size_t j = 0; j < uniqueCameras.size(); j++) {
            if (uniqueCameras[j] == records[i].cameraNumber) {
                found = true;
                break;
            }
        }
        if (!found) {
            uniqueCameras.push_back(records[i].cameraNumber);
        }
    }
    cout << "There are " << uniqueCameras.size() << " cameras." << endl;

    int totalViolations = 0;
    for (size_t i = 0; i < records.size(); i++) {
        totalViolations += records[i].violations;
    }
    cout << "A total of " << totalViolations << " violations." << endl;

    if (!records.empty()) {
        const CameraRecord *maxViolRecord = &records[0];
        for (size_t i = 1; i < records.size(); i++) {
            if (records[i].violations > maxViolRecord->violations) {
                maxViolRecord = &records[i];
            }
        }
        
        string month, day, year;
        parseDate(maxViolRecord->date, month, day, year);
        string formattedDate = month + "-" + day + "-" + year;

        cout << "The most violations in one day were " << maxViolRecord->violations 
             << " on " << formattedDate << " at " << maxViolRecord->intersection << endl;
    }
}

void bubbleSortNeighborhoods(vector<NeighborhoodInfo> &neighborhoods) {
    int n = neighborhoods.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (neighborhoods[j].violationCount < neighborhoods[j + 1].violationCount) {
                swap(neighborhoods[j], neighborhoods[j + 1]);
            }
        }
    }
}

void neighborhoodResults(const vector<CameraRecord> &records) {
    vector<NeighborhoodInfo> neighborhoods;

    for (size_t i = 0; i < records.size(); i++) {
        bool found = false;
        for (size_t j = 0; j < neighborhoods.size(); j++) {
            if (neighborhoods[j].name == records[i].neighborhood) {
                found = true;
                break;
            }
        }
        if (!found) {
            neighborhoods.push_back(NeighborhoodInfo(records[i].neighborhood));
        }
    }

    for (size_t i = 0; i < neighborhoods.size(); i++) {
        vector<int> camerasInNeigh;
        for (size_t j = 0; j < records.size(); j++) {
            if (records[j].neighborhood == neighborhoods[i].name) {
                bool camFound = false;
                for (size_t k = 0; k < camerasInNeigh.size(); k++) {
                    if (camerasInNeigh[k] == records[j].cameraNumber) {
                        camFound = true;
                        break;
                    }
                }
                if (!camFound) {
                    camerasInNeigh.push_back(records[j].cameraNumber);
                }
                neighborhoods[i].violationCount += records[j].violations;
            }
        }
        neighborhoods[i].cameraCount = camerasInNeigh.size();
    }

    bubbleSortNeighborhoods(neighborhoods);

    for (size_t i = 0; i < neighborhoods.size(); i++) {
        string displayName = neighborhoods[i].name;
        if (displayName.length() > 25) {
            displayName = displayName.substr(0, 25);
        }
        cout << displayName;
        
        int nameSpaces = 25 - displayName.length();
        for (int j = 0; j < nameSpaces; j++) {
            cout << " ";
        }

        string camStr = to_string(neighborhoods[i].cameraCount);
        int camSpaces = 4 - camStr.length();
        for (int j = 0; j < camSpaces; j++) {
            cout << " ";
        }
        cout << camStr;

        string violStr = to_string(neighborhoods[i].violationCount);
        int violSpaces = 7 - violStr.length();
        for (int j = 0; j < violSpaces; j++) {
            cout << " ";
        }
        cout << violStr << endl;
    }
}

void monthlyChart(const vector<CameraRecord> &records) {
    const string months[] = {"January", "February", "March", "April", "May", "June",
                           "July", "August", "September", "October", "November", "December"};
    int monthlyViolations[12] = {0};

    for (size_t i = 0; i < records.size(); i++) {
        size_t dashPos = records[i].date.find('-');
        if (dashPos != string::npos) {
            string monthStr;
            for (size_t j = dashPos + 1; j < records[i].date.length(); j++) {
                if (records[i].date[j] == '-') break;
                monthStr += records[i].date[j];
            }
            try {
                int month = stoi(monthStr);
                if (month >= 1 && month <= 12) {
                    monthlyViolations[month - 1] += records[i].violations;
                }
            } catch (...) {
                continue;
            }
        }
    }

    for (int i = 0; i < 12; i++) {
        cout << months[i];
        for (size_t j = months[i].length(); j < 12; j++) {
            cout << " ";
        }
        cout << " ";
        int stars = monthlyViolations[i] / 1000;
        for (int j = 0; j < stars; j++) {
            cout << "*";
        }
        cout << endl;
    }
}

void searchCameras(const vector<CameraRecord> &records) {
    string searchTerm;  // First declaration here
    cout << "What should we search for? " <<endl;
    getline(cin,searchTerm);

    searchTerm = toLower(searchTerm);

    vector<int> foundCameras;
    bool anyFound = false;

    for (size_t i = 0; i < records.size(); i++) {
        string lowerIntersection = toLower(records[i].intersection);
        string lowerNeighborhood = toLower(records[i].neighborhood);

        if (lowerIntersection.find(searchTerm) != string::npos || 
            lowerNeighborhood.find(searchTerm) != string::npos) {
            bool camFound = false;
            for (size_t j = 0; j < foundCameras.size(); j++) {
                if (foundCameras[j] == records[i].cameraNumber) {
                    camFound = true;
                    break;
                }
            }
            if (!camFound) {
                foundCameras.push_back(records[i].cameraNumber);
                anyFound = true;
                cout << "Camera: " << records[i].cameraNumber << endl;
                cout << "Address: " << records[i].address << endl;
                cout << "Intersection: " << records[i].intersection << endl;
                cout << "Neighborhood: " << records[i].neighborhood << endl;
                cout << endl;
                cout<<endl;
            }
        }
    }

    if (!anyFound) {
        cout << "No cameras found." << endl;
        cout<<endl;
    }
}


int main() {
    string filename;
    cout << "Enter file to use: ";
    cin >> filename;

    vector<CameraRecord> records = readCameraData(filename);
    if (records.empty()) {
        cout << "No data read from file. Exiting." << endl;
        return 1;
    }

    int choice;
    do {
            cout << "Select a menu option: " << endl 
                 << "  1. Data overview" << endl
                 << "  2. Results by neighborhood" << endl
                 << "  3. Chart by month" << endl
                 << "  4. Search for cameras" << endl
                 << "  5. Exit" << endl
                 << "Your choice: ";

        cin >> choice;
        cin.ignore();
        // if (cin.fail()) {
        //     cin.clear();  // Clear error flag
        //     // Manually consume characters until newline
        //     char c;
        //     while (cin.get(c) && c != '\n');

        //     continue;
        // }
        switch (choice) {
            case 1:
                dataOverview(records);
                break;
            case 2:
                neighborhoodResults(records);
                break;
            case 3:
                monthlyChart(records);
                break;
            case 4:
                searchCameras(records);
                break;
            case 5:
                cout << "" << endl;
                return 0;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 5);
    return 0;
}