#include "../Core.h"

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

namespace Swarm {
    namespace Logging {

        /*
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
         */

        // ********************
        //  Static Log Methods
        // ********************

        const char * Log::defaultFilepath = "logs/Swarm_";
        vector<FILE*> Log::fileptrs;

        Log Log::log_global("Global", true);
        Log Log::log_config("Configuration", true);
        Log Log::log_model("Model", true);
        Log Log::log_render("Rendering", true);

        void Log::setDefaultFilepath(const char * path) {
            defaultFilepath = path;
        }

        void Log::cleanupAll() {
            for(int i = 0; i < fileptrs.size(); i++) fclose(fileptrs[i]);
        }



        // ********************
        //  Member Log Methods
        // ********************

        Log::Log() {
            file = NULL;
            name = "";
            filepath = NULL;
            console = true;
        }

        Log::Log(const char * name, bool console) : name(name), console(console) {
            filepath = (string(defaultFilepath) + string(name) + ".log").c_str();
            file = fopen(filepath, "w");
            if(file == NULL) cerr << "Failed to create log: " << filepath;
            else fileptrs.push_back(file);
        }

        Log::Log(const char * name, FILE * file, bool console) : name(name), file(file), console(console) {
            filepath = "";
        }

        Log::~Log() {
            //if(file != NULL) fclose(file);
        }

        string Log::time() {
            time_t now = std::time(NULL);
            char buff[80];
            strftime(buff, sizeof(buff), "[%m/%d/%Y-%X]", localtime(&now));
            return string(buff);
        }

        const char * Log::prefix() {
            switch(severity) {
                case INFO:
                    return "[INFO]";
                case WARNING:
                    return "[WARN]";
                case ERR:
                    return "[ERROR]";
                case FATAL:
                    return "[FATAL]";
                default:
                    return "[UNKWN]";
            }
        }

        void Log::newline() {
            *this << "\n" << time() << "[" << name << "]" << prefix() << " ";
        }

        void Log::setSeverity(LogSeverity severity) {
            this->severity = severity;
            newline();
        }

        Log &Log::operator()(LogSeverity severity) {
            setSeverity(severity);
            return *this;
        }

        Log &Log::operator<<(const char * input) {
            if(file != NULL) fprintf(file, "%s", input);
            if(console && (severity == ERR || severity == FATAL)) fprintf(stderr, "%s", input);
            else if(console) fprintf(stdout, "%s", input);
            return *this;
        }

        Log &Log::operator<<(string input) {
            *this << input.c_str();
            return *this;
        }

        Log &Log::operator<<(GLuint input) {
            if(file != NULL) fprintf(file, "%u", input);
            if(console && (severity == ERR || severity == FATAL)) fprintf(stderr, "%u", input);
            else if(console) fprintf(stdout, "%u", input);
            return *this;
        }

        Log &Log::operator<<(GLint input) {
            if(file != NULL) fprintf(file, "%i", input);
            if(console && (severity == ERR || severity == FATAL)) fprintf(stderr, "%i", input);
            else if(console) fprintf(stdout, "%i", input);
            return *this;
        }

        Log &Log::operator<<(double input) {
            if(file != NULL) fprintf(file, "%f", input);
            if(console && (severity == ERR || severity == FATAL)) fprintf(stderr, "%f", input);
            else if(console) fprintf(stdout, "%f", input);
            return *this;
        }

        Log &Log::operator<<(char input) {
            if(file != NULL) fprintf(file, "%c", input);
            if(console && (severity == ERR || severity == FATAL)) fprintf(stderr, "%c", input);
            else if(console) fprintf(stdout, "%c", input);
            return *this;
        }

        Log &Log::operator<<(glm::vec3 input) {
            *this << "(" << input.x << ", " << input.y << ", " << input.z << ")";
            return *this;
        }

        Log &Log::operator=(Log &other) {
            this->name = other.name;
            this->file = other.file;
            this->filepath = other.filepath;
            this->console = other.console;
            this->severity = other.severity;
            return *this;
        }

    }
}