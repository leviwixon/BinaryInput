#include <iostream>
#include <map>
#include <fstream>
#include <string.h>
#include <math.h>
#include <iomanip>
#include "binaryFile.h"
#include "customErrorClass.h"

using namespace std;

// Const that stores the size of each input chunk we take from the binary file. Should there be more parameters, extension of existing parameters,
// or modification of parameters that require change, this could help.
const size_t sizeOfInput = sizeof(char)*38;

// This list helps translate a given integer representing account (which is 1-5 in this case) with its corresponding name
map<int, string> deptList = {
    {0, "Accounting"},
    {1, "Business"},
    {2, "Human Resources"},
    {3, "Sales"},
    {4, "Production"}
};

/*
**    Author: Levi
**    Function Purpose:
**      Self Explanatory
**
**    Function Output: Constructor
**    Side Effects: None
*/
C_binaryFile::C_binaryFile() {
}

/*
**    Author: Levi
**    Function Purpose:
**      Self Explanatory
**
**    Function Output: Destructor
**    Side Effects: None
*/
C_binaryFile::~C_binaryFile() {
}

/*
**    Author: Levi
**    Function Purpose:
**      Public function that calls both the reading of the original binary file, and the writing of the new sorted file. It also acts as a holding area for the sorted input from the
**      original binary file.
**
**    Function Output: "Sorted_[filename].(ext)"
**    Side Effects: None
*/
void C_binaryFile::P_createBinaryFiles(string filename) {
    try {
        map<int, Info> inputInfo = p_readBinary(filename);
        p_writeBinary(inputInfo, filename);
    } catch (MyException& e) {
		cout << e.what() << endl;
	}
	catch (exception& e) {
		cout << e.what() << endl;
	} 
}

/*
**    Author: Levi
**    Function Purpose:
**      Reads the binary file in 38 byte chunks into a premade struct called Info (can be found in "binaryFile.h"). This is all connected to pairs of info and the ID of each 38 byte
**      chunk, and the resulting map is then automatically organized by ID.
**
**    Function Output: A map of the pairs of ID and Info, sorted by ID
**    Side Effects: None
*/
map<int, Info> C_binaryFile::p_readBinary(string filename) {
    // File opener
    fstream binaryInputfile;
    // This map will allow us to sort out our employee's into an array which associates their information (which may sometimes
    // duplicate e.g. department or name) to their unique employee ID.
    map<int, Info> employeeInfo;
    
    // Allows us to handle information from the input file without directly inputting it into the map.
    Info handler;

    binaryInputfile.open(filename, ios::in|ios::binary);

    if (binaryInputfile.is_open()) {
        while (!binaryInputfile.eof()) {
            // Use of "sizeof(char)*38 since we need 2 ints and a character array of max 30, hence 4 bytes + 4 bytes + 30 bytes = 38 bytes."
            binaryInputfile.read((char*)&handler, sizeOfInput);
            employeeInfo.insert(pair<int, Info>(handler.ID, handler));
        }
    }
    binaryInputfile.close();
    // Helpful bit of code to print out what is in the map directly after it is read. Make sure to uncomment map<int, Info>::iterator myiterator
    // for code snippet to function correctly! Finishes by printing the size of the map.
    // map<int, Info>::iterator infoIterator;
    //  for(infoIterator = employeeInfo.begin(); infoIterator != employeeInfo.end(); ++infoIterator ) {
    //      if (infoIterator->second.ID == 18738) {
    //      cout << "Index " << infoIterator->first << " = " << "Department: "<< infoIterator->second.department
    //      << "   ID: " << infoIterator->second.ID << "   Name: " << infoIterator->second.name << endl; 
    //      }
    //  }
    //  cout << employeeInfo.size() << endl;

    return employeeInfo;
}


/*
**    Author: Levi
**    Function Purpose:
**      Uses the previously obtained map of pairings between ID and Info to populate a new binary file. The required method of sorting is to be done by Department, but because we
**      already sorted by ID, this has the consequence of leaving each department sorted by their ID as well. The sorting function works by identifying the pairs that have the 
**      first department on the list, writes them to the file, then uses the iterator to erase the entry, shrinking our search size for subsequent queries. It is INCREDIBLY important
**      to note that a slightly different approach is required if a user is using versions of C++ that aren't C++11, as previous versions of C++ did not allocated a return value for
**      the map method of "erase". This requires previous versions to use "map.erase(iterator++)" as opposed to "iterator = map.erase(iterator)". This method also does a lot of the
**      clerical work when it comes to indexing our new file, and keeping track of how many things are in a given index range, as the values of index range and total "things" NEEDS
**      to be different since we do not actually erase any lines from the Sorted_[filename].(ext), rather make them zeroed out of sorts (use '\0' for name and -1 for other fields).
**
**    Function Output: Sorted binary file by both department AND sorted departments by ID
**    Side Effects: Possible issues should user be using version of C++ that isn't C++11 due to void vs return. See article here: https://www.cplusplus.com/reference/map/map/erase/
*/
void C_binaryFile::p_writeBinary(map<int, Info> collectedInfo, string filename) {
    fstream binaryOutputFile;
    // We append the filename to this which will allow the transfer of file extension as well as the exact file name.
    outputFilename = "Sorted_";
    // This is a counter variable for keeping track of which department we are on within an arbitrary, positive integer department ID.
    int currentDeptID = 0;
    int indexTracker = 1;
    // sets the max index possible
    index[5] = collectedInfo.size() - 1;
    // Open binary file for output
    outputFilename.append(filename);
    
    binaryOutputFile.open(outputFilename, ios::out|ios::binary);

 
    // If the file is open, and the collected info isn't empty
    if (binaryOutputFile.is_open()) {
        // Nested loop to iterate through and delete entries that match the current department ID. Works for C++11, since the erase method
        // returns the next valid iterator value, however older versions of C++ will require a swap out of infoIterator = collectedInfo.erase(infoIterator)
        // with collectedInfo.erase(infoIterator++).
        while (currentDeptID < 5) {
            map<int, Info>::iterator infoIterator = collectedInfo.begin(); // reset to beginning each time as long as there are more departments to do.
            while (infoIterator != collectedInfo.end()) {
                if (infoIterator->second.department == currentDeptID) { // If matching department, write, then remove from map to lower consecutive search sizes.
                    binaryOutputFile.write((char*)&infoIterator->second, sizeOfInput);
                    infoIterator = collectedInfo.erase(infoIterator);
                    indexTracker++;
                    totalInDept[currentDeptID]++;
                } else { // If not a match, move to next one.
                    infoIterator++;
                }
            }
            if (currentDeptID < 4) {    // index[0] will ALWAYS be 0, since it starts, so we only worry about what is next.
                index[currentDeptID+1] = indexTracker+1;
            }
            currentDeptID++; // begins next department if valid.
        }
        binaryOutputFile.close();
    } else {    // If the file could not be opened for any reason, throw error with message of filename. Ends program since this file is vital.
        string errorMessage = "ERROR: could not create output file by the name of ";
        errorMessage.append(outputFilename);
        throw MyException(errorMessage); 
    }
}

/*
**    Author: Levi
**    Function Purpose:
**      Public facing function that does some input validation before launching into the private functions to find and printing the results. No exceptions because we want the user
**      to be able to continue inputting, not end the program.
**
**    Function Output: Either the employee's info, or error message stating why the info couldn't be retrieved.
**    Side Effects: None
*/
void C_binaryFile::P_findEmployee(int departmentID, int employeeID) {
    Info foundEmployee;
    if ((departmentID < 5 && departmentID >= 0) && (employeeID >= 0)) {
        foundEmployee = p_findEmployee(departmentID, employeeID);
        if (foundEmployee.ID == -1) {
            cout << "Could not find employee by that ID and Department." << endl;
            return;
        }
        p_formattedPrint(foundEmployee);

    } else {
        cout << "WARNING: Cannot search for a department that does not exist. Try again with a valid department for results." << endl;
    }
}

/*
**    Author: Levi
**    Function Purpose:
**      Finds a given employee using the index retrieved from p_findIndex(), and enters the file at said index using a multiplication of the index by input size. Once read, the info
**      is returned. Should the filed not open, we DO throw an error this time, as the sorted file is vital for operation, and if it is gone, the program SHOULD end.
**
**    Function Output: Info about employee
**    Side Effects: None
*/
Info C_binaryFile::p_findEmployee(int departmentID, int employeeID) {
    int employeeIndex = 0;
    Info entry;
    fstream findEntry;

    employeeIndex = p_findIndex(departmentID, employeeID);
    if (employeeIndex == -1) {
        entry.ID = -1;
        return entry;
    }
    findEntry.open(outputFilename, ios::in|ios::binary);

    if (findEntry.is_open()) {
        findEntry.seekg(employeeIndex*sizeOfInput);
        findEntry.read((char*)&entry, sizeOfInput);
        findEntry.close();
        return entry;
        
    } else {
        throw MyException("ERROR: Failed to open file to search for employee.");
    }
    
}

/*
**    Author: Levi
**    Function Purpose:
**      Prints out formatted text using setw with 2 width types. Aesthetically fine for me, but changing numWidth and nameWidth can always be done to customizer to users preference.
**
**    Function Output: Better looking output text.
**    Side Effects: None
*/
void C_binaryFile::p_formattedPrint(Info employee) {
    int nameWidth = 32;
    int deptWidth = 15;
    int numWidth = 5;
    cout << "Employee Name: " << left 
    << setw(nameWidth) << employee.name  
    << setw(deptWidth) << "Department: " << employee.department << "(" << deptList.find(employee.department)->second << ")" 
    << setw(numWidth) << " ID:\t" << employee.ID << endl;
}

/*
**    Author: Levi
**    Function Purpose:
**      Public facing employee record updater. Deals with some edge cases like the user choosing a name larger than the 30 char bye limit, or not choosing a name at all, but otherwise
**      jumps into the private function to update the name (uses strncpy so we can maintain 30 byte char array, as I couldn't seem to find solid yes or no surrounding whether c_str
**      did the same.)
**
**    Function Output: Proper name that fits the requirements set.
**    Side Effects: None
*/
void C_binaryFile::P_updateEmployee(int departmentID, int employeeID) {
    Info update;
    string updateName = "";
    bool proceed = false;
    update = p_findEmployee(departmentID, employeeID);
    int location = p_findIndex(departmentID, employeeID);
    char test[30];
    if (update.ID == -1 || location == -1) {
        cout << "WARNING: Employee as described does not exist. Ensure that correct information is being used." << endl;
        return;
    }
    p_formattedPrint(update);
    while (!proceed) {
        cout << "What would you like the employee's name to be changed to?(Enter nothing to exit): ";
        cin >> updateName;
        if (updateName.length() > 30) {
            cout << "Employee name is" << updateName.length() << "characters long, and exceeds the 30 character max. Try abbreviating or only entering partial name." << endl;
        } else if (updateName.empty()){
            return;
        } else {
            proceed = true;
        }
    }
    strncpy(update.name, updateName.c_str(), 30);
    p_updateEmployee(update, location);
    cout << "New employee record: ";
    P_findEmployee(departmentID, employeeID);
}

/*
**    Author: Levi
**    Function Purpose:
**      Opens up the sorted file, uses the location and size of input to write it in, then closes it back up.
**
**    Function Output: Updated Sorted_ file
**    Side Effects: None
*/
void C_binaryFile::p_updateEmployee(Info updatedEntry, int location) {
    fstream updateFile;
    updateFile.open(outputFilename, ios::in|ios::out|ios::binary);

    if (updateFile.is_open()) {
        updateFile.seekg(location*sizeOfInput);
        updateFile.write((char*)&updatedEntry, sizeOfInput);
    }
    updateFile.close();
}

/*
**    Author: Levi
**    Function Purpose:
**      Using a rough divide and conquer approach, this method ensures the department is valid, then jumps into a sorting routine in which it divides up a given index by 2, searches
**      whether the id is < or >, and since each ID is sorted per department and already unique, tries to find the value. 
**
**      *****SPECIAL CASE*****
**      If it reaches a midpoint of <1, that means the binary sort
**      likely tried to split a singleton into two peices, and a singleton that isn't our number means our number isn't here. Should we run into deleted records (since we 'zero' them
**      out instead of actually removing them form the file), the method tries to search for the nearest midpoint that isn't -1, isn't beyond our lower bound, and has and index that 
**      IS less than our current midpoint, it moves there. If it is -1 again, it keeps looping until it finds a valid midpoint, or hits the lower bound. Once the lower bound is hit,
**      the direction flag trips, preventing us from alternating left and right or vice-versa. We can ONLY head right now, and this continues until we confirm the right has been deleted,
**      in which case we admit defeat, set our info values to -1 to flag down the public program that the value does not exist. This is likely a HUGE edge case, as there would have to
**      be an absurd amount of deletions, but it is hopefully taken care of nonetheless.
**
**    Function Output: The index of the desired info
**    Side Effects: None
*/
int C_binaryFile::p_findIndex(int departmentID, int employeeID) {
    fstream readBinary;
    Info entry;
    bool directionFlag = false;
    entry.ID = -1;  // sets our starting value to be invalid.
    if ((departmentID < 0) || (departmentID > 4)) {
        cout << "WARNING: Invalid department ID. Try again with a valid department." << endl;
        return -1;
    }
    int lowerBound = index[departmentID];
    int upperBound = index[departmentID+1] -1;
    float findMid = (upperBound - lowerBound) / 2;
    int midPoint = floor(findMid + lowerBound);
    // Safe to not validate, since we can't enter this function unless the deptID is between 0 and 4.

    readBinary.open(outputFilename, ios::in|ios::binary);
    
    if (readBinary.is_open()) {
        // In case we get lucky and the lower bound is the entry, save us some time and grab it.
        readBinary.seekg(ios::beg+(lowerBound*sizeOfInput));
        readBinary.read((char*)&entry, sizeOfInput);

        if (entry.ID == employeeID) {
            return lowerBound;
        }
        // In case we get lucky and the upper bound is the entry, save us some time and grab it.
        readBinary.seekg(ios::beg+(upperBound*sizeOfInput));
        readBinary.read((char*)&entry, sizeOfInput);
        if (entry.ID == employeeID) {
            return upperBound;
        }
        // Set our entry to be the midpoint and work from there to search.
        readBinary.seekg(ios::beg+(midPoint*sizeOfInput));
        readBinary.read((char*)&entry, sizeOfInput);
        // If not upper or lower bound, the get binary searching.
        while (entry.ID != employeeID) {    // Continue searching until either the invalid case nested inside fires, or we get a value that matches.
            if (findMid < 1) {  // should only happen if we attempt to binary search on a single entry, which means we couldn't find it.
                return -1;
            }
            while (entry.ID == -1) {
            // On the ever so slight chance that we run into a midpoint on an already "deleted" node, this will attempt to resolve the nearest viable value.
            // This has an inherent lowerBound leaning direction, so to avoid constantly heading up, then right back down once we have traversed all the way to lower
            // bound, a direction flag is set. Should we ever reach the case that we head right, we will never head left again. Should we reach maximum right, we know
            // all the records we are searching through have been deleted, so we can return a -1 to allow the public function to recognize failure.
                if ((midPoint - 1 >= lowerBound) && !directionFlag) {
                    midPoint = midPoint - 1;
                } else if(midPoint + 1 <= upperBound) {
                    midPoint = midPoint + 1;
                    directionFlag = true;
                } else {
                    return -1;
                }
            }
            if (employeeID > entry.ID) {
                lowerBound = midPoint;
                findMid = (upperBound - lowerBound)/2;
                midPoint = floor(findMid + lowerBound);
            } else {
                upperBound = midPoint;
                findMid = (upperBound - lowerBound)/2;
                midPoint = floor(findMid + lowerBound);
            }
            readBinary.seekg(ios::beg+(midPoint*sizeOfInput));
            readBinary.read((char*)&entry, sizeOfInput);
        }
        return midPoint;
    } else {    // IF file cannot open throw error, otherwise invalid searches should warn, but not error, as the user needs to be able to correct them.
        throw MyException("ERROR: Failed to open file to search for employee.");
    }
}

/*
**    Author: Levi
**    Function Purpose:
**      Since we don't delete the employee from the file, or make a new file to replace the old, we instead just use the update record to 'zero' out our target employee. This can
**      only be done since the public function does all the error catching and warnings, so we can violate the rules of the Public facing updateEmployee by ONLY using the private.
**
**    Function Output: A 'zeroed' out employee
**    Side Effects: None
*/
void C_binaryFile::P_deleteEmployee(int departmentID, int employeeID) {
    if ((departmentID < 5 && departmentID >= 0) && (employeeID >= 0)) {
        if (p_findIndex(departmentID, employeeID) == -1) {
            cout << "WARNING: Employee as described does not exist. Ensure that correct information is being used." << endl;
        } else {
            p_updateEmployee(p_makeDelEntry(), p_findIndex(departmentID, employeeID));
            totalInDept[departmentID] = totalInDept[departmentID] - 1;
        }
    } else {
        cout << "WARNING: Information entered is invalid. Please try again with correct department ID and positive employee ID." << endl;
    }
}

/*
**    Author: Levi
**    Function Purpose:
**      Just makes a given record zeroed out.
**
**    Function Output: Empty record
**    Side Effects: None
*/
Info C_binaryFile::p_makeDelEntry() {
    Info entry;
    entry.ID = -1;
    entry.department = -1;
    string delName= "\0";
    strncpy(entry.name, delName.c_str(), 30);
    return entry;
}

/*
**    Author: Levi
**    Function Purpose:
**      Becuase our requirements are so focused on the user being able to continue inputs, this function just throws warnings and returns to the user effectively empty if certain rules
**      are violated. The method also understand that the larger number SHOULD be the end, so either direction should work in how you type in the range.
**
**    Function Output: Many printed records
**    Side Effects: None
*/
void C_binaryFile::P_printRange(int departmentID, int start, int end) {
    
    if (start < 0 || end < 0) {
        cout << "ERROR: Entered negative value in range, please try again using values greater than or equal to zero.";
    } else if (start > totalInDept[departmentID] || end > totalInDept[departmentID]) {
        cout << "ERROR: Entered value outside range of department. Max value for " << deptList.find(departmentID)->second << " is " << totalInDept[departmentID] << endl;
    } else {
        if (start > end) {
            p_printRange(end, start, departmentID);
        } else {
            p_printRange(start, end, departmentID);
        }
    }
}

/*
**    Author: Levi
**    Function Purpose:
**      Iterates through the range of numbers given and multiplies them by the sizeOfInput each time, grabbing results along the way and printing them out via formatted print.
**
**    Function Output: Printed range
**    Side Effects: Potential error thrown if file doesn't open
*/
void C_binaryFile::p_printRange(int start, int end, int departmentID) {
    fstream readBinaryFile;
    Info printInfo;
    readBinaryFile.open(outputFilename, ios::in|ios::binary);
    int curIndex;
    cout << (end - start) << " employees in " << deptList.find(departmentID)->second << endl;
    if (readBinaryFile.is_open()) {
        for (start; start <= end; start++) {
            curIndex = index[departmentID] + start;
            readBinaryFile.seekg(ios::beg+(curIndex*sizeOfInput));
            readBinaryFile.read((char*)&printInfo, sizeOfInput);
            p_formattedPrint(printInfo);
        }
        readBinaryFile.close();
    } else {
        throw MyException("ERROR: Failed to open file to search for employee.");
    }
}
