#ifndef SWARMENGINE_LOGGING_H
#define SWARMENGINE_LOGGING_H

// External Libraries
#include <stdio.h>
#include <string>
#include <sstream>
#include <cstring>
#include <ctime>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include <stdlib.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Definitions.h"

// -----------
// Definitions
// -----------

#pragma lhgMultiOn(SwarmEngine, LHG_NONE, 100)
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
#pragma lhgMultiOff()


// -------------------
// | LOGGING SECTION |
// -------------------

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_LOG {

        //#pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG)
        /*
        void ChangeLoggingLevel(int newLoggingLevel);
        void ChangeLoggingType(int newLoggingType);

        void initLogging();
        void initLogging(int newLoggingLevel, int newLoggingType);
        void cleanupLogging();

        void Log(int level, const char * category, const char * message);
        //void Log(int level, const char * category, const char * message, std::vector< int > intInputs);
         */

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG)
        string formatVec3(glm::vec3 pos);
        string formatVec3(float x, float y, float z);

        enum LogSeverity {
            INFO,
            WARNING,
            ERR,        // 'ERROR' is macro'd somewhere
            FATAL
        };

        class Log {
        public:
            Log();      // Console-Only Log
            Log(const char * name, bool console = false);
            Log(const char * name, FILE * file, bool console = false);
            virtual ~Log();

            void newline();

            // Getters
            const char * getName() { return name; }
            const char * getFilepath() { return filepath; }

            // Setters
            void setSeverity(LogSeverity severity);

            // Operator Overloads
            Log &operator()(LogSeverity severity);
            Log &operator<<(const char * input);
            Log &operator<<(string input);
            Log &operator<<(GLuint input);
            Log &operator<<(GLint input);
            Log &operator<<(double input);
            Log &operator<<(char input);
            Log &operator=(Log &other);

            static void setDefaultFilepath(const char * path);
            static void cleanupAll();

            static Log log_global;
            static Log log_config;
            static Log log_model;
            static Log log_render;

        protected:
            string time();
            const char * prefix();

            const char * name;
            const char * filepath;

            FILE * file = NULL;

            bool console = false;

            LogSeverity severity = INFO;

            static const char * defaultFilepath;

        private:
            static vector<FILE*> fileptrs;
        };

        #pragma lhgMultiOff()

    }
}

#endif //SWARMENGINE_LOGGING_H
