#include <iostream>
#include <string.h>
#include <map>
#include <fstream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
#include "binaryFile.h"
#include "binaryFile.cpp"
#include "customErrorClass.h"
#include "customErrorClass.cpp"

using namespace std;

/*
**    Author: Levi
**    Function Purpose:
**      Makes sure the binary file isn't empty, or violating our assumptions of what and where it is.
**
**    Function Output: Either boolean that we are good to go, or runtime errors.
**    Side Effects: None
*/
bool inputValidation(int argc, char *argv[]) {
    bool retValue = false;
    struct stat fileInfo;
    string errorString;
    int error;

    if (1 >= argc || 3 <= argc) {
        cout << "USAGE: " << argv[0] << " [FILENAME]"<<endl;
        cout << "exiting..." <<endl;
    } else {
        error = stat(argv[1], &fileInfo);

        if (0 != error) {
            errorString = strerror(errno);
            errorString += ": ";
            errorString += argv[1];
            throw runtime_error(errorString);
        }

        if (0 == fileInfo.st_size) {
            errorString = "file is empty: ";
            errorString += argv[1];
            throw runtime_error(errorString);
        }

        retValue = true;
    }
    return retValue;
}

/*
**    Author: Levi
**    Function Purpose:
**      Prevents the issues that come with users entering in non-numerical input when it should be integers. If the user enters in letters without this, the menu will loop on bad input
**      infinitely. Also bars users from negative integers.
**
**    Function Output: Either good input or bothering the users to enter until they follow the rules of input
**    Side Effects: None
*/
int validateInput(int input, string message) {
    while (!(cin >> input) || input < 0) {
        cout << "Incorrect Input, please ONLY enter integer values." << endl;
        cin.clear();
        cin.ignore(INT_MAX, '\n');
        cout << message;
    }
    return input;
}

int main(int argc, char *argv[]) {

    bool retValue = false;
    bool getInput = true;
    int option = 0;
    try{
        retValue = inputValidation(argc, argv);
    } catch (runtime_error &e) {
        cout << "RUNTIME ERROR: " << e.what() << endl;
    } catch (exception &e) {
        cout << "EXCEPTION: " << e.what() << endl;
    }


    // Menu only terminates if done by ^C at commandline and through the exit. Rebooting might also do it.
    if (true == retValue) {
        C_binaryFile binFile;
        string message;
        binFile .P_createBinaryFiles(argv[1]);
        while (getInput) {
            int start = 0;
            int end = 0;
            int dept = 0;
            int ID = 0;
            
            cout << endl;
            cout << "Choose one of the following options to continue: " << endl;
            cout << "1.) Find Employee" << endl;
            cout << "2.) Update Employee Record" << endl;
            cout << "3.) Delete Employee" << endl;
            cout << "4.) Print range of employees in an department" << endl;
            cout << "5.) Quit" << endl;
            cout << "Enter number of desired option: ";
            option = validateInput(option, "Enter number of desired option: ");
            switch(option) {
                case 1:
                    cout << endl << endl;
                    cout << "Enter Employee ID: ";
                    ID = validateInput(ID, "Enter Employee ID: ");
                    cout << "Enter Department Number: ";
                    dept = validateInput(dept, "Enter Department Number: ");
                    binFile.P_findEmployee(dept, ID);
                    break;
                case 2:
                    cout << endl << endl;
                    cout << "Enter Employee ID: ";
                    ID = validateInput(ID, "Enter Employee ID: ");
                    cout << "Enter Department Number: ";
                    dept = validateInput(dept, "Enter Department Number: ");
                    binFile.P_updateEmployee(dept, ID);
                    break;
                case 3:
                    cout << endl << endl;
                    cout << "Enter Employee ID: ";
                    ID = validateInput(ID, "Enter Employee ID: ");
                    cout << "Enter Department Number: ";
                    dept = validateInput(dept, "Enter Department Number: ");
                    binFile.P_deleteEmployee(dept, ID);
                    break;
                case 4:
                    cout << endl << endl;
                    cout << "Enter start of employee range: ";
                    start = validateInput(start, "Enter start of employee range: ");
                    cout << "Enter end of employee range: ";
                    end = validateInput(end, "Enter end of employee range: ");
                    cout << "Enter Department Number: ";
                    dept = validateInput(dept, "Enter Department Number: ");
                    binFile.P_printRange(dept, start, end);
                    break;
                case 5:
                    getInput = false;
                    break;
                defualt:
                    cout << "Looks like the option you chose was not one of the provided. Try again with the provided options." << endl;
                    break;
            }

        }
    }

    return 0;
}
