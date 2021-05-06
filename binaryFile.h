#ifndef _binaryFile_H_
#define _binaryFile_H_

#include <iostream>
#include <string.h>
#include <map>

using namespace std;

// Holds all the info, hence the name
struct Info {
    int department;
    int ID;
    char name[30];
};

class C_binaryFile {
    public:
    C_binaryFile();
    ~C_binaryFile();
    
    void P_createBinaryFiles(string);
    void P_printTest(string);
    void P_findEmployee(int, int);
    void P_updateEmployee(int, int);
    void P_deleteEmployee(int, int);
    void P_printRange(int, int, int);

    private:
    // Only 5 deparments, but an extra value allows us to store the index of the final value, which is useful in know the original length.
    int index[6] = {0};
    // Seperate array for total entries needed, as we aren't ever actually removing information from the binary output.
    int totalInDept[5] = {0};
    string outputFilename;


    map<int, Info> p_readBinary(string);
    void p_writeBinary(map<int, Info>, string);
    Info p_findEmployee(int, int);
    void p_updateEmployee(Info, int);
    int p_findIndex(int, int);
    void p_formattedPrint(Info);
    Info p_makeDelEntry();
    void p_printRange(int, int, int);
};
#endif
