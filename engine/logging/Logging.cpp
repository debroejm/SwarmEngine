/*
 * Copyright 2017 James De Broeck
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define SWARM_INCLUDE_GLM
#define SWARM_BOOST_AVAILABLE
#include "api/Logging.h"

#include "api/Synchronization.h"

#include <unordered_set>

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
                case TRACE:
                    return "[TRACE]";
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



        // **********************
        //  Log Buffer and Locks
        // **********************

        struct LogBuffer {
            Synchro::SyncMap<boost::thread::id, std::stringstream> _data;
            Synchro::SyncMap<boost::thread::id, boost::mutex> _mutices;
            boost::mutex _file_mutex;
        };
        boost::mutex _static_console_mutex;



        // ********************
        //  Static Log Methods
        // ********************

        std::string _static_default_log_filepath = "Swarm_";
        std::set<FILE*> _static_log_fileptrs;

        void Log::setDefaultFilepath(const std::string &path) {
            _static_default_log_filepath = path;
        }

        void Log::cleanupAll() {
            for(FILE* file : _static_log_fileptrs) fclose(file);
            _static_log_fileptrs.clear();
        }



        // ********************
        //  Member Log Methods
        // ********************

        Log::Log() : _name(""), _filepath(""), _use_console(true) {
            _buffer = new LogBuffer();
        }

        Log::Log(const std::string &name, bool console) : _name(name), _filepath(""), _use_console(console) {
            _buffer = new LogBuffer();
        }

        Log::~Log() {
            delete _buffer;
        }

        Log &Log::operator=(const Log &other) {
            _name = other._name;
            _filepath = other._filepath;
            _file = other._file;
            _use_console = other._use_console;
            _severity = other._severity;
            return *this;
        }

        void Log::initFile() {
            _filepath = _static_default_log_filepath + _name + ".log";
            _file = fopen(_filepath.c_str(), "w");
            if(_file == nullptr) { std::cerr << "Failed to create log: '" << _filepath << "'; Error '" << std::strerror(errno) << "'\n"; }
            else _static_log_fileptrs.insert(_file);
        }

        void Log::initFile(FILE* file) {
            _filepath = "";
            _file = file;
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

        void Log::flush() {
            flush(boost::this_thread::get_id());
        }

        void Log::flush(const boost::thread::id& thread_id) {
            _buffer->_mutices[thread_id].lock();
            std::string buf = _buffer->_data[thread_id].str();
            _buffer->_data[thread_id].str("");
            _buffer->_mutices[thread_id].unlock();
            if(buf.empty()) return; // Skip outputting
            if(_file != nullptr) {
                boost::lock_guard<boost::mutex> file_lock(_buffer->_file_mutex);
                fprintf(_file, "%s", buf.c_str());
            }
            if(_use_console) {
                // TODO: Possibly move console log output to a separate thread with queue-set
                boost::lock_guard<boost::mutex> console_lock(_static_console_mutex);
                if(_severity == ERR || _severity == FATAL) fprintf(stderr, "%s", buf.c_str());
                else fprintf(stdout, "%s", buf.c_str());
            }
        }

        void Log::flushAll() {
            for(auto && iter : _buffer->_data)
                flush(iter.first);
        }

        Log& Log::operator<<(const Flush&) {
            flush();
            return *this;
        }

        Log &Log::operator<<(const char * input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(std::string input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(unsigned int input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(int input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(unsigned long input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(long input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(unsigned long long input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(long long input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(double input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(char input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << input;
            return *this;
        }

        Log &Log::operator<<(glm::vec2 input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << "(" << input.x << ", " << input.y << ")";
            return *this;
        }
        
        Log &Log::operator<<(glm::vec3 input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << "(" << input.x << ", " << input.y << ", " << input.z << ")";
            return *this;
        }

        Log &Log::operator<<(glm::vec4 input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()] << "(" << input.x << ", " << input.y << ", " << input.z << ", " << input.w << ")";
            return *this;
        }

        Log &Log::operator<<(glm::mat4 input) {
            boost::lock_guard<boost::mutex> lock(_buffer->_mutices[boost::this_thread::get_id()]);
            _buffer->_data[boost::this_thread::get_id()]
                          << "[(" << input[0][0] << ", " << input[0][1] << ", " << input[0][2] << ", " << input[0][3] << "), ("
                          << input[1][0] << ", " << input[1][1] << ", " << input[1][2] << ", " << input[1][3] << "), ("
                          << input[2][0] << ", " << input[2][1] << ", " << input[2][2] << ", " << input[2][3] << "), ("
                          << input[3][0] << ", " << input[3][1] << ", " << input[3][2] << ", " << input[3][3] << ")]";
            return *this;
        }
    }
}