#include "outputfile.h"

#include "misc.h"

map<string, OutputFile*> fileMapping;
string outputDirectory = "";

OutputFile* getFile(string filename) {
    if(fileMapping[filename] == NULL)
        fileMapping[filename] = new OutputFile(filename);
    return fileMapping[filename];
}

void writeAllFiles() {
    for(auto &iter : fileMapping) {
        iter.second->write();
    }
}

OutputFile::OutputFile(string filename) : filename(filename) {
}

OutputFile::~OutputFile() { }

void OutputFile::addEntry(Entry* entry, int priority) {
    entries[priority].push_back(entry);
}

void OutputFile::addInclude(string include) {
    for(string &inc : includes) {
        if(inc == include) return;
    }
    includes.push_back(include);
}

void OutputFile::write() {
    FILE* file = fopen((outputDirectory+filename+".h").c_str(), "w");
    fprintf(file, "#pragma once\n\n");
    for(string inc : includes) fprintf(file, "#include %s", inc.c_str());
    fprintf(file, "\n");
    bool first = true;
    string lastNmspc = "";
    unsigned int tabCount = 0;
    for(auto &pri : entries) {
        vector<Entry*> &entries = pri.second;
        for(Entry* entry : entries) {

            // Namespace header check
            string thisNmspc = entry->getNamespace();
            if(thisNmspc != lastNmspc) {

                fprintf(file, "\n");

                // Shorten previous Namespace until you can find it in the beginning of the new one, or its empty
                while(!first) { // lol, used as a pseudo 'if' statement
                    int index = thisNmspc.find(lastNmspc);
                    if(index > 0 || index == thisNmspc.npos) {
                        if(tabCount > 0) tabCount--;
                        fprintf(file, "%s}\n", space(tabCount*4).c_str());
                        unsigned int brkIndex = lastNmspc.rfind("::");
                        if(brkIndex == lastNmspc.npos) {
                            lastNmspc = "";
                            break;
                        } else {
                            lastNmspc.erase(brkIndex, lastNmspc.size()-brkIndex);
                        }
                    } else break;
                }

                fprintf(file, "\n");

                // Lengthen previous Namespace until it matches the new one
                string rem = "";
                //if(thisNmspc.size() > lastNmspc.size()) rem = thisNmspc.substr(lastNmspc.size()+2);
                if(lastNmspc.size() < 1) rem = thisNmspc;
                else if(thisNmspc.size() > lastNmspc.size()) rem = thisNmspc.substr(lastNmspc.size()+2);
                while(lastNmspc != thisNmspc) {
                    string sec = rem.substr(0,rem.find("::"));
                    fprintf(file, "%snamespace %s {\n", space(tabCount*4).c_str(), replaceCustomDefine(sec).c_str());
                    lastNmspc += ("::" + sec);
                    tabCount++;
                    unsigned int brkIndex = rem.find("::");
                    if(brkIndex == rem.npos) rem = "";
                    else rem.erase(0,brkIndex+2);
                    if(rem == "") break; // Should happen at the same time as guard
                }
                lastNmspc = thisNmspc;

                fprintf(file, "\n");
            }
            first = false;

            // Print actual data
            fprintf(file, "%s", entry->getData(tabCount*4).c_str());
        }
    }

    // Apply ending brackets
    while(lastNmspc != "") { // lol, used as a pseudo 'if' statement
        if(tabCount > 0) tabCount--;
        fprintf(file, "%s}\n", space(tabCount*4).c_str());
        unsigned int brkIndex = lastNmspc.rfind("::");
        if(brkIndex == lastNmspc.npos) {
            lastNmspc = "";
            break;
        } else {
            lastNmspc.erase(brkIndex, lastNmspc.size()-brkIndex);
        }
    }
}

void setOutputDirectory(string direct) {
    outputDirectory = direct;
}