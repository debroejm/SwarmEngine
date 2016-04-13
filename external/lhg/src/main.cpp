#include <dirent.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "lineentry.h"
#include "outputfile.h"

using namespace std;

// Prototypes
bool scanDirectory(string dirName, bool recursive, int depth = 1);
bool isHeaderFile(const char* fileName, int nameLen);
void scanFile(string fileName);
void parsePragmaLine(string line);
void parsePragma(string command, string args[], int argc);

// Debug
string activeFilename = "";
int activeLineNum = 0;

// Reading Data
int activePriority = 0;
string activeNamespace = "";
OutputFile* activeFile;
BlockEntry* activeBlock;

void printErrHeader() {
    cerr << "[" << activeFilename << ":" << activeLineNum << "] ";
}

int main(int argc, char* argv[]) {

    char* directoryName = NULL;
    bool recursive = false;
    char* outputDirName = NULL;

    for(int i = 0; i < argc; i++) {
        if(strcmp(argv[i], "-p") == 0) {
            if(i+1 >= argc) {
                cerr << "ERR: Missing argument for -p" << endl;
                return EXIT_FAILURE;
            } else {
                directoryName = argv[i+1];
                i++;
                continue;
            }
        } else if(strcmp(argv[i], "-r") == 0) {
            recursive = true;
            continue;
        } else if(strcmp(argv[i], "-o") == 0) {
            if(i+1 >= argc) {
                cerr << "ERR: Missing argument for -o" << endl;
                return EXIT_FAILURE;
            } else {
                outputDirName = argv[i+1];
                i++;
                continue;
            }
        }
    }

    if(outputDirName != NULL) {
        DIR* dir = opendir(outputDirName);
        if(dir == NULL) {
            cerr << "ERR: No such output directory '" << outputDirName << "'" << endl;
            return EXIT_FAILURE;
        } else {
            setOutputDirectory(string(outputDirName) + "\\");
        }
    }

    if(directoryName != NULL) {
        if(!scanDirectory(directoryName, recursive)) {
            cerr << "ERR: No such directory '" << directoryName << "'" << endl;
            return EXIT_FAILURE;
        } else {
            writeAllFiles();
        }
    } else {
        // Print info
        cout << "Possible Arguments:" << endl;
        cout << "  -p path     | Specify the directory path to search in" << endl;
        cout << "  -o path     | Specify the directory to output to" << endl;
        cout << "  -r          | Parse recursively" << endl;
        return 0;
    }
}

bool scanDirectory(string dirName, bool recursive, int depth) {
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(dirName.c_str())) == NULL) return false;
    while((ent = readdir(dir)) != NULL) {
        if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        string fullPath = dirName+"\\"+string(ent->d_name);
        for (int i = 0; i < depth; i++) cout << "  ";
        cout << ent->d_name << endl;
        if(isHeaderFile(ent->d_name, ent->d_namlen)) {
            activeFilename = string(ent->d_name);
            scanFile(fullPath);
        }
        if (ent->d_type == DT_DIR && recursive)
            scanDirectory(fullPath, recursive, depth+1);
    }
    closedir(dir);
    return true;
}

bool isHeaderFile(const char* fileName, int nameLen) {
    int lastDot = -1;
    for(int i = 0; i < nameLen; i++) {
        if(fileName[i] == '.') lastDot = i;
    }
    if(lastDot < 0) return false;

    int extLen = nameLen-lastDot;
    char extension[extLen];
    for(int i = 0; i < extLen; i++) {
        extension[i] = fileName[lastDot+1+i];
    }

    return ( strcmp(extension, "h") == 0 || strcmp(extension, "hpp") == 0 );
}

void scanFile(string filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if(file == NULL) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    activeLineNum = 0;
    while( 1 ) {
        char line[1000];
        fgets(line, 1000, file);
        if(feof(file)) break;

        char header[100];
        if(sscanf(line, "%99s", header));
        if( strcmp(header, "#pragma") == 0 ) {
            parsePragmaLine(line);
        } else if(getFlag(FLAG_GRAB_SINGLE) || getFlag(FLAG_GRAB_MULTI)) {
            if( strcmp(header, "#define") == 0 && getFlag(FLAG_PROCESS_DEFINES)) {
                // TODO: Make more robust
                string defStr(line);
                unsigned int indexSpace = defStr.find(" ", defStr.find("#")+1);
                if(indexSpace != defStr.npos) {
                    unsigned int indexSpace2 = defStr.find(" ", indexSpace+1);
                    if(indexSpace2 != defStr.npos) {
                        addCustomDefine(defStr.substr(indexSpace+1, indexSpace2-indexSpace-1),
                                        defStr.substr(indexSpace2+1,defStr.size()-indexSpace2-2));
                    }
                }
            } else {
                if(getFlag(FLAG_GRAB_MULTI)) {
                    if(activeBlock == NULL) {
                        printErrHeader();
                        cerr << "Multi Grab Flag set without instantiating a Block (should not happen, report to creator)" << endl;
                    } else activeBlock->addEntry(LineEntry(line, activeNamespace));
                } else activeFile->addEntry(new LineEntry(line, activeNamespace), activePriority);
            }
            if(getFlag(FLAG_GRAB_SINGLE)) setFlag(FLAG_GRAB_SINGLE, false);
        }

        activeLineNum++;
    }

    setFlag(FLAG_ALL, false);
}

void parsePragmaLine(string line) {
    unsigned int indexPound = line.find("#");
    if(indexPound == line.npos) return; // Should never happen
    unsigned int indexSpace = line.find(" ", indexPound+1);
    unsigned int indexLeftParen = line.find("(");
    unsigned int indexRightParen = line.find(")");
    if(indexSpace == line.npos || indexLeftParen == line.npos || indexRightParen == line.npos ||
       indexSpace >= indexLeftParen || indexLeftParen >= indexRightParen) return;

    // Get Command Name
    char cmd_c_str[50];
    sscanf(line.substr(indexSpace+1, indexLeftParen-indexSpace-1).c_str(), " %49s ", cmd_c_str);
    string command(cmd_c_str);

    // Get Arguments
    vector<string> argVec;
    unsigned int lastIndex = indexLeftParen+1;
    while(true) {
        unsigned int indexComma = line.find(",", lastIndex);
        string argSec = "";
        if (indexComma == line.npos || indexComma >= indexRightParen)
            argSec = line.substr(lastIndex, indexRightParen - lastIndex);
        else
            argSec = line.substr(lastIndex, indexComma - lastIndex);
        lastIndex = indexComma + 1;

        // Check for blank Arg
        bool valid = false;
        for (int i = 0; i < argSec.size(); i++) {
            if (argSec[i] != ' ') valid = true;
        }
        if (!valid && (argVec.size() > 0 || !(indexComma == line.npos || indexComma >= indexRightParen))) {
            printErrHeader();
            cerr << "Empty Parameter! (" << argVec.size() << ")" << endl;
            return;
        }

        // Retrieve Arg without Gaps
        if (valid) {
            char arg_c_str[50];
            sscanf(argSec.c_str(), " %49s ", arg_c_str);
            argVec.push_back(string(arg_c_str));
        }

        // Check for End
        if(indexComma == line.npos || indexComma >= indexRightParen) break;
    }
    int argc = argVec.size();
    string args[argc];
    for(int i = 0; i < argc; i++)
        args[i] = string(argVec[i]);

    parsePragma(command, args, argc);
}

void parsePragma(string command, string args[], int argc) {
    if(command == "lhgSingle" || command == "lhgMultiOn") {

        // Check Arg Count
        if(!(argc == 2 || argc == 3)) {
            printErrHeader();
            if(command == "lhgMultiOn")
                cerr << "Invalid Number of Parameters for 'lhgMultiOn'; found " << argc << " but requires 2 or 3" << endl;
            else
                cerr << "Invalid Number of Parameters for 'lhgSingle'; found " << argc << " but requires 2 or 3" << endl;
            return;
        }

        // Retrieve Arguments
        string filename = args[0];
        string nmspc = args[1];
        if(nmspc == "LHG_NONE") nmspc = "";
        int priority = 0;
        if(argc == 3) {
            sscanf(args[2].c_str(), "%i", &priority);
            priority *= -1;
        }

        activeNamespace = nmspc;
        activeFile = getFile(filename);
        activePriority = priority;
        if(command == "lhgSingle") setFlag(FLAG_GRAB_SINGLE, true);
        if(command == "lhgMultiOn") {
            setFlag(FLAG_GRAB_MULTI, true);
            activeBlock = new BlockEntry(nmspc);
        }

    } else if(command == "lhgMultiOff") {

        // Check Arg Count
        if (argc != 0) {
            printErrHeader();
            cerr << "Invalid Number of Parameters for 'lhgMultiOff'; found " << argc << " but requires 0" << endl;
            return;
        }

        activeFile->addEntry(new LineEntry("\n", activeNamespace));
        setFlag(FLAG_GRAB_MULTI, false);
        activeFile->addEntry(activeBlock, activePriority);

    } else if(command == "lhgFlagOn" || command == "lhgFlagOff") {

        // Check Arg Count
        if (argc != 1) {
            printErrHeader();
            cerr << "Invalid Number of Parameters for 'lhgFlag'; found " << argc << " but requires 1" << endl;
            return;
        }

        bool set = false;
        if(command == "lhgFlagOn") set = true;

        // Process Flag
        if(args[0] == "PROCESS_DEFINES") setFlag(FLAG_PROCESS_DEFINES, set);
        else {
            printErrHeader();
            cerr << "Unknown Flag '" << args[0] << "'" << endl;
            return;
        }

    } else {
        cerr << "[" << activeFilename << ":" << activeLineNum << "] ";
        cerr << "Unknown Custom Pragma Command '" << command << "'" << endl;
        return;
    }
}