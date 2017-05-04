#define SWARM_INCLUDE_GLM
#include "api/Logging.h"

#include <cstring>
#include <iostream>
#include <time.h>

namespace Swarm {
    namespace Logging {

        std::string timeString() {
            time_t now = time(nullptr);
            char buff[80];
            strftime(buff, sizeof(buff), "[%m/%d/%Y-%X]", localtime(&now));
            return std::string(buff);
        }

        std::string severityPrefix(LogSeverity severity) {
            switch(severity) {
                case DEBUG:
                    return "[DEBUG]";
                case INFO:
                    return "[ INFO]";
                case WARNING:
                    return "[ WARN]";
                case ERR:
                    return "[ERROR]";
                case FATAL:
                    return "[FATAL]";
                default:
                    return "[UNKWN]";
            }
        }



        // ********************
        //  Static Log Methods
        // ********************

        std::string Log::_static_default_filepath = "Swarm_";
        std::vector<FILE*> Log::_static_fileptrs;

        Log Log::log_core("Core");
        Log Log::log_render("Render");
        Log Log::log_cl("CL");
        Log Log::log_physics("Physics");
        Log Log::log_vhe("VHE");

        void Log::setDefaultFilepath(const std::string &path) {
            _static_default_filepath = path;
        }

        void Log::cleanupAll() {
            for(FILE* file : _static_fileptrs) fclose(file);
        }



        // ********************
        //  Member Log Methods
        // ********************

        Log::Log() : _file(NULL), _name(""), _filepath(""), _use_console(true) {}

        Log::Log(const std::string &name, bool console) : _name(name), _use_console(console) {
            _filepath = std::string(_static_default_filepath) + name + ".log";
            _file = fopen(_filepath.c_str(), "w");
            if(_file == nullptr) std::cerr << "Failed to create log: '" << _filepath << "'; Error '" << std::strerror(errno) << "'\n";
            else _static_fileptrs.push_back(_file);
        }

        Log::Log(const std::string &name, FILE * file, bool console) : _file(file), _name(name), _filepath(""), _use_console(console) {}

        Log &Log::operator=(const Log &other) {
            _name = other._name;
            _filepath = other._filepath;
            _file = other._file;
            _use_console = other._use_console;
            _severity = other._severity;
            return *this;
        }

        void Log::newline() {
            *this << "\n" << timeString() << severityPrefix(_severity) << "[" << _name << "]" << " ";
        }

        void Log::setSeverity(LogSeverity severity) {
            _severity = severity;
            newline();
        }

        Log &Log::operator()(LogSeverity severity) {
            setSeverity(severity);
            return *this;
        }

        Log &Log::operator<<(const char * input) {
            if(_file != nullptr) fprintf(_file, "%s", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%s", input);
            else if(_use_console) fprintf(stdout, "%s", input);
            return *this;
        }

        Log &Log::operator<<(std::string input) {
            *this << input.c_str();
            return *this;
        }

        Log &Log::operator<<(unsigned int input) {
            if(_file != nullptr) fprintf(_file, "%u", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%u", input);
            else if(_use_console) fprintf(stdout, "%u", input);
            return *this;
        }

        Log &Log::operator<<(int input) {
            if(_file != nullptr) fprintf(_file, "%i", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%i", input);
            else if(_use_console) fprintf(stdout, "%i", input);
            return *this;
        }

        Log &Log::operator<<(unsigned long input) {
            if(_file != nullptr) fprintf(_file, "%lu", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%lu", input);
            else if(_use_console) fprintf(stdout, "%lu", input);
            return *this;
        }

        Log &Log::operator<<(long input) {
            if(_file != nullptr) fprintf(_file, "%li", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%li", input);
            else if(_use_console) fprintf(stdout, "%li", input);
            return *this;
        }

        Log &Log::operator<<(unsigned long long input) {
            if(_file != nullptr) fprintf(_file, "%llu", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%llu", input);
            else if(_use_console) fprintf(stdout, "%llu", input);
            return *this;
        }

        Log &Log::operator<<(long long input) {
            if(_file != nullptr) fprintf(_file, "%lli", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%lli", input);
            else if(_use_console) fprintf(stdout, "%lli", input);
            return *this;
        }

        Log &Log::operator<<(double input) {
            if(_file != nullptr) fprintf(_file, "%f", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%f", input);
            else if(_use_console) fprintf(stdout, "%f", input);
            return *this;
        }

        Log &Log::operator<<(char input) {
            if(_file != nullptr) fprintf(_file, "%c", input);
            if(_use_console && (_severity == ERR || _severity == FATAL)) fprintf(stderr, "%c", input);
            else if(_use_console) fprintf(stdout, "%c", input);
            return *this;
        }

        Log &Log::operator<<(glm::vec2 input) {
            *this << "(" << input.x << ", " << input.y << ")";
            return *this;
        }
        
        Log &Log::operator<<(glm::vec3 input) {
            *this << "(" << input.x << ", " << input.y << ", " << input.z << ")";
            return *this;
        }

        Log &Log::operator<<(glm::vec4 input) {
            *this << "(" << input.x << ", " << input.y << ", " << input.z << ", " << input.w << ")";
            return *this;
        }

        Log &Log::operator<<(glm::mat4 input) {
            *this << "[(" << input[0][0] << ", " << input[0][1] << ", " << input[0][2] << ", " << input[0][3] << "), ("
                          << input[1][0] << ", " << input[1][1] << ", " << input[1][2] << ", " << input[1][3] << "), ("
                          << input[2][0] << ", " << input[2][1] << ", " << input[2][2] << ", " << input[2][3] << "), ("
                          << input[3][0] << ", " << input[3][1] << ", " << input[3][2] << ", " << input[3][3] << ")]";
            return *this;
        }
    }
}