#include <iostream>
#include <map>
#include <fstream>
#include <string.h>
#include "binaryFile.h"

using namespace std;

C_binaryFile::C_binaryFile() {
    map<int, Info> employeeInfo;
}

C_binaryFile::~C_binaryFile() {
}

void C_binaryFile::P_readBinary(string filename) {
    fstream binaryInputfile;
    map<int, Info> employeeInfo;
    map<int, Info>::iterator myIterator;
    // Allows us to handle information from the input file without directly inputting it into the map.
    Info handler;

    binaryInputfile.open(filename, ios::in|ios::binary);

    if (binaryInputfile.is_open()) {
        while (!binaryInputfile.eof()) {
            binaryInputfile.read((char*)&handler, sizeof(Info));
            employeeInfo.insert(pair<int, Info>(handler.ID, handler));
        }
    }
    for(myIterator = employeeInfo.begin(); myIterator != employeeInfo.end(); ++myIterator ) {
        cout << "Index " << myIterator->first << " = " << "Department: "<< myIterator->second.department
        << "   ID: " << myIterator->second.ID << "   Name: " << myIterator->second.name << endl; 
    }

}

