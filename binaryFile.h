#ifndef _binaryFile_H_
#define _binaryFile_H_

#include <iostream>
#include <string.h>

using namespace std;

struct Info {
    int ID;
    int department;
    char name[30];
};


class C_binaryFile {
    public:
    C_binaryFile();
    ~C_binaryFile();
    void P_readBinary(string filename);
    void P_writeBinary(Info information);

    private:
};
#endif
