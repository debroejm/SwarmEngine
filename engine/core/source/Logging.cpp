#include "../headers/Logging.h"

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#ifdef _WIN32
#if       _WIN32_WINNT < 0x0500
  #undef  _WIN32_WINNT
  #define _WIN32_WINNT   0x0500
#endif
#include <Windows.h>
#include <GLFW/glfw3.h>

#endif

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_LOG {

        const char * LOG_FILENAME = "SwarmLogging.log";
        FILE * logFile;

        int logLevel = LOGGING_LEVEL_MOST;
        int logType = LOGGING_TYPE_FILE; // LOGGING_TYPE_NONE;

        void ChangeLoggingLevel(int newLoggingLevel) { logLevel = newLoggingLevel; }
        void ChangeLoggingType(int newLoggingType) { logType = newLoggingType; }

        void initLogging()
        {
            logFile = fopen(LOG_FILENAME, "w");
            if( logFile == NULL )
            {
                printf("Something went wrong; could not create the log file !\n");
            }
#ifdef _WIN32
            ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif
        }

        void initLogging(int newLoggingLevel, int newLoggingType)
        {
            logFile = fopen(LOG_FILENAME, "w");
            if( logFile == NULL )
            {
                printf("Something went wrong; could not create the log file !\n");
            }
#ifdef _WIN32
            ShowWindow( GetConsoleWindow(), SW_HIDE );
#endif
            ChangeLoggingLevel(newLoggingLevel);
            ChangeLoggingType(newLoggingType);
        }

        void cleanupLogging()
        {
            fclose( logFile );
        }

        void Log(int severity, const char * category, const char * message)
        {
            // Since the more severe log info is lower numerical wise, this is > instead of <
            if( severity > logLevel ) return;
            if( logType == LOGGING_TYPE_NONE ) return;

            // Convert our #define to a C String :P
            char * tS;
            if( severity == LOGGING_ERROR ) tS = "ERROR";
            else if( severity == LOGGING_WARNING ) tS = "WARNING";
            else if( severity == LOGGING_INFO ) tS = "INFO";

            double time = glfwGetTime();

            //char * completeMessage;
            //sprintf(completeMessage, "%s%s %s\n", tS, category, message);

            if( (logType == LOGGING_TYPE_CONSOLE) || (logType == LOGGING_TYPE_FILE_AND_CONSOLE) )
            {
                fprintf( stdout, "[%f][%s][%s] %s\n", time, tS, category, message);
            }
            if( (logType == LOGGING_TYPE_FILE) || (logType == LOGGING_TYPE_FILE_AND_CONSOLE) )
            {
                fprintf( logFile, "[%f][%s][%s] %s\n", time, tS, category, message);
            }
        }

        string formatVec3(glm::vec3 pos) { return formatVec3(pos.x, pos.y, pos.z); }
        string formatVec3(float x, float y, float z) {
            return SSTR("(" << x << ", " << y << ", " << z << ")");
        }

    }
}