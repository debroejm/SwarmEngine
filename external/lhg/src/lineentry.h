#pragma once

#include <map>
#include <string>
#include <vector>

using namespace std;

class Entry {
public:
    Entry(string nmspc) : nmspc(nmspc) { }
    virtual string getData(int perLineSpace = 0) = 0;
    string getNamespace() { return nmspc; }

protected:
    string nmspc;

};

class LineEntry : public Entry {
public:
    LineEntry(string line, string nmspc);
    LineEntry &operator=(const LineEntry rhs);
    virtual string getData(int perLineSpace = 0);
    unsigned int getOriginalSpacing() { return originalSpacing; }
    bool isEmpty() { return line == ""; }

    unsigned int spacing = 0;

protected:
    string trimLine(string line);

    string line;
    unsigned int originalSpacing = 0;

};

class BlockEntry : public Entry {
public:
    BlockEntry(string nmspc);
    BlockEntry &operator=(const BlockEntry rhs);
    BlockEntry &operator=(const LineEntry rhs);
    void addEntry(LineEntry entry);
    virtual string getData(int perLineSpace = 0);
    void clear();

protected:
    void trimBlock();

    vector<LineEntry> entries;
    bool trimmed = false;

};