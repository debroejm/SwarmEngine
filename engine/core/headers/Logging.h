#ifndef SWARMENGINE_LOGGING_H
#define SWARMENGINE_LOGGING_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

#include "../Definitions.h"

// -----------
// Definitions
// -----------

// Logging Level
#define LOGGING_LEVEL_MIN 1 // Just Errors
#define LOGGING_LEVEL_NORMAL 2 // Errors + Warnings
#define LOGGING_LEVEL_MOST 3 // All

// Logging Type
#define LOGGING_TYPE_NONE 0 // No log will be outputted
#define LOGGING_TYPE_FILE 1 // Output logging info to a file
#define LOGGING_TYPE_CONSOLE 2 // Only display logging info on the console
#define LOGGING_TYPE_FILE_AND_CONSOLE 3 // Output logging info to both the console and a file

// Logging Severety
#define LOGGING_ERROR 1
#define LOGGING_WARNING 2
#define LOGGING_INFO 3


// -------------------
// | LOGGING SECTION |
// -------------------

namespace ENGINE_NAMESPACE {
    namespace Logging {

        void ChangeLoggingLevel(int newLoggingLevel);
        void ChangeLoggingType(int newLoggingType);

        void initLogging();
        void initLogging(int newLoggingLevel, int newLoggingType);
        void cleanupLogging();

        void Log(int level, const char * category, const char * message);
        //void Log(int level, const char * category, const char * message, std::vector< int > intInputs);

    }
}

#endif //SWARMENGINE_LOGGING_H
