#pragma once

#include <fstream>
#include <string>

#include "lineentry.h"

using namespace std;

class OutputFile {
public:
    OutputFile(string filename);
    ~OutputFile();

    void addEntry(Entry* entry, int priority = 0);
    void addInclude(string include);

    void write();

protected:
    string filename;
    map<int, vector<Entry*>> entries;
    vector<string> includes;

};

OutputFile* getFile(string filename);
void writeAllFiles();
void setOutputDirectory(string direct);