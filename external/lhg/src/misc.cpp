#include "misc.h"

#include <map>

// Actual Flags
unsigned int flags = 0;

bool getFlag(unsigned int flag) {
    return (flag & flags) > 0;
}

void setFlag(unsigned int flag, bool val) {
    if(val) flags = flags | flag;
    else flags = flags & (~flag);
}



map<string, string> defines;

void addCustomDefine(string define, string replacement) {
    defines[define] = replacement;
}

string replaceCustomDefine(string line) {
    string result = line;
    for(auto iter : defines) {
        unsigned int index = result.find(iter.first);
        if(index != line.npos) {
            if(!textChar(line[index+iter.first.size()])) {
                result.replace(index, iter.first.size(), iter.second);
            }
        }
    }
    return result;
}



bool textChar(char c) {
    return ( (c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c == 95) || (c >= 97 && c <= 122) );
}

string space(unsigned int count) {
    return string(count, ' ');
}