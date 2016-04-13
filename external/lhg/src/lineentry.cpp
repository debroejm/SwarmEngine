#include "lineentry.h"

#include <limits>

#include "misc.h"

LineEntry::LineEntry(string line, string nmspc) : line(trimLine(line)), Entry(nmspc) { }

LineEntry &LineEntry::operator=(const LineEntry rhs) {
    line = rhs.line;
    originalSpacing = rhs.originalSpacing;
    spacing = rhs.spacing;

    return *this;
}

string LineEntry::getData(int perLineSpace) {
    return space(perLineSpace + spacing) + line;
}

string LineEntry::trimLine(string line) {
    unsigned int firstIndex = 0;
    for(unsigned int i = 0; i < line.size(); i++) {
        if(line.at(i) != ' ') {
            firstIndex = i;
            break;
        }
    }
    originalSpacing = firstIndex;
    string result = replaceCustomDefine(line.substr(firstIndex, line.npos));
    return result;
}

BlockEntry::BlockEntry(string nmspc) : Entry(nmspc) {

}

BlockEntry &BlockEntry::operator=(const BlockEntry rhs) {
    entries = rhs.entries;
    trimmed = rhs.trimmed;
    return *this;
}

BlockEntry &BlockEntry::operator=(const LineEntry rhs) {
    entries.clear();
    addEntry(rhs);
    return *this;
}

void BlockEntry::addEntry(LineEntry entry) {
    entries.push_back(entry);
    trimmed = false;
}

string BlockEntry::getData(int perLineSpace) {
    if(!trimmed) trimBlock();
    string result = "";
    for(LineEntry &entry : entries)
        result += entry.getData(perLineSpace);
    return result;
}

void BlockEntry::clear() {
    entries.clear();
    trimmed = true;
}

void BlockEntry::trimBlock() {
    int lowestSpacing = std::numeric_limits<int>::max();
    for(LineEntry &entry : entries) {
        if(entry.getOriginalSpacing() < lowestSpacing) lowestSpacing = entry.getOriginalSpacing();
    }
    for(LineEntry &entry : entries) {
        entry.spacing = entry.getOriginalSpacing() - lowestSpacing;
    }
    trimmed = true;
}