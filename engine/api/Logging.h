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

#pragma once



// ***************
//  STD Libraries
// ***************

#include <fstream>
#include <set>
#include <string>
#include <vector>



// ***************
//  Common Header
// ***************

#define SWARM_REQUEST_BOOST_THREAD
#include "common_header.h"
#undef SWARM_REQUEST_BOOST_THREAD



// ***********
//  API Begin
// ***********

namespace Swarm {

    namespace Logging {

        //! Enum representation of logging severity.
        /*!
         * This enumeration is used to define the severity of a logged message.
         * Depending on settings, some severities may be excluded from being logged to certain outputs.
         */
        enum LogSeverity {
            //! Very fine logging detail, used for tracing flow of execution, among other things
            TRACE = -10,
            //! Fine logging detail, used for debugging issues
            DEBUG = -5,
            //! Standard information that is expected to appear
            INFO = 0,
            //! Something did not happen as expected, but flow of execution will continue as normal
            WARNING = 5,
            //! An error occurred, and flow of execution has exited what it was doing
            ERR = 10,        // 'ERROR' is macro'd somewhere in Windows headers
            //! A fatal problem occurred, and the program will most likely exit
            FATAL = 20
        };

        //! Get a text-based time stamp
        /*!
         * Gets a time stamp string in the format of '[month/day/year-hour:minute:second]'.
         *
         * /return std::string representing a time stamp
         */
        std::string timeString();

        //! Get a text prefix for a LogSeverity value
        /*!
         * Takes a given LogSeverity value and returns a prefix in the format of '[SEVERITY]'.
         *
         * \param severity LogSeverity value
         * \return std::string prefix
         */
        std::string severityPrefix(LogSeverity severity);

        //! Marker Structure for flushing a Log object
        /*!
         * Convenience marker to insert into a Log that flushes the Log's buffer. Equivalent to calling Log::flush().
         *
         * \sa Log::operator<<(const EndLine&), Log::flush(), Log::flushAll()
         */
        struct Flush {};

        //! Internal Engine object for synchronization buffering
        struct LogBuffer;

        //! An object used to provide logging features
        /*!
         * A Log object is used as an interface to log events into a medium that can be output and read at a later time.
         * All Log objects will output to the standard output stream (std::cout) by default, and Log objects created
         * with a name will also write their output to a file on disk once initialized. A Log object can be used with the
         * \link Log::operator<<() stream insertion operator \endlink, as well as the \link Log::operator()() function
         * operator \endlink. The former will pipe text to the Log object, while the latter will start a new line with
         * the indicated LogSeverity. Console output can be redirected to another output stream by redirecting the
         * standard output stream (std::cout).
         *
         * \sa Log::operator<<(), Log::setSeverity(), Log::operator()(), LogSeverity
         */
        class Log {
        public:

            //! Create a Console-Only Log
            /*!
             * Creates a Log object that when streamed to, will output its contents only to the console.
             */
            Log();

            //! Create a Log with the given name
            /*!
             * Creates a Log object with a name. By default, a Log object will output to the console, but this can be
             * disabled by passing \c false for \a console. A Log object will not output to a file until Log::initFile()
             * has been called.
             *
             * \param name name to associate with the Log object and to label the associated log file when created
             * \param console whether or not this Log object should output to the console, defaults to \c false
             * \sa Log::initFile()
             */
            Log(const std::string &name, bool console = true);

            //! Log Copy-Constructor
            /*!
             * Creates a new Log object and sets it equal to another.
             *
             * \param other Log object to set equal to
             */
            Log(const Log &other) { *this = other; }

            //! Log Destructor
            /*!
             * Cleans up the LogBuffer object and other heap memory stored internally.
             */
            virtual ~Log();

            //! Log Assignment Operator
            /*!
             * Sets this Log object equal to another.
             *
             * \param other Log object to set equal to
             * \return reference to the current Log object
             */
            Log &operator=(const Log &other);

            //! Initializes the Log and opens its file for writing
            /*!
             * Initializes the Log and attempts to create a file for output with a path based on the current default
             * Log filepath. Log files will only output to the console until this is called. Log FILE pointers created
             * through initialization are stored internally, and are closed during Log cleanup.
             *
             * \sa Log::init(FILE*), Log::setDefaultFilepath(), Log::cleanupAll()
             */
            void initFile();

            //! Initializes the Log and registers a file for writing
            /*!
             * Initializes the Log and uses a given file for writing. Log files will only output to the console until
             * this is called. Log FILE pointers given to this method still belong to the developer, and it is the
             * developer's job to close them. If a nullptr is given, this method does nothing.
             *
             * \param file FILE pointer to write to
             * \sa Log::init()
             */
            void initFile(FILE* file);

            //! Sets the currently used logging severity
            /*!
             * Sets the logging severity so that future log snippets will use the given severity.
             * In addition, automatically calls Log::newline() so that future logging happens as a new entry.
             *
             * \param severity LogSeverity value to use for further logging
             * \sa LogSeverity, Log::newline()
             */
            void setSeverity(LogSeverity severity);

            //! Alias for Log::setSeverity(), with single-line chain insertion available
            /*!
             * Calls Log::setSeverity() (and consequently Log::newline()).
             * In addition, returns a reference to itself to be used in conjunction with << operators.
             *
             * \param severity LogSeverity value to use for further logging
             * \return reference to the current Log object
             * \sa LogSeverity, Log::setSeverity(), Log::newline()
             */
            Log &operator()(LogSeverity severity);

            //! Starts a new line
            /*!
             * Adds a newline character to the stream, and sets up the header for the next line.
             */
            void newline();
            
            //! Flushes the line buffer for the current thread
            /*!
             * Ensures that all content in the calling thread's buffer for this Log is flushed to its appropriate
             * output (IE the console and/or a log file), and clears the buffer.
             *
             * \sa Log::flushAll()
             */
            void flush();

            #if defined(SWARM_BOOST_AVAILABLE)
            //! Flushes the line buffer for a given thread
            /*!
             * Ensures that all content in the given thread's buffer for this Log is flushed to its appropiate output
             * (IE the console and/or a log file), and clears the buffer.
             *
             * \param thread_id The ID of the thread to flush for
             * \sa Log::flush()
             */
            void flush(const boost::thread::id& thread_id);
            #endif
            
            //! Flushes the line buffer for all threads
            /*!
             * Ensures that all content in every buffer is flushed to its appropriate output (IE the console and/or a
             * log file), and clears all buffers.
             *
             * \sa Log::flush()
             */
            void flushAll();

            //! Gets the user-friendly name of this Log
            std::string name() const { return _name; }

            //! Gets the relative filepath of this Log (this will be empty for developer owned FILE pointers)
            std::string filepath() const { return _filepath; }

            //! Sets whether or not this Log outputs to the console
            void setUseConsole(bool use) { _use_console = use; }

            //! Determines whether or not this Log outputs to the console
            bool usesConsole() const { return _use_console; }

            //! Determines whether or not this Log outputs to a file
            bool usesFile() const { return _file != nullptr; }


            // ****************************
            //  Stream Insertion Overloads
            // ****************************

            //! Flushes the log stream.
            /*!
             * Performs a flush of the log stream. Equivalent to calling Log::flush().
             *
             * \sa Log::flush(), Log::flushAll()
             */
            Log& operator<<(const Flush&);

            //! Adds a cstring to the log stream.
            Log& operator<<(const char *input);

            //! Adds a string to the log stream.
            Log& operator<<(std::string input);

            //! Adds an unsigned int to the log stream.
            Log& operator<<(unsigned int input);

            //! Adds a signed int to the log stream.
            Log& operator<<(int input);

            //! Adds an unsigned long to the log stream.
            Log& operator<<(unsigned long input);

            //! Adds a signed long to the log stream.
            Log& operator<<(long input);

            //! Adds an unsigned long long to the log stream.
            Log& operator<<(unsigned long long input);

            //! Adds a signed long long to the log stream.
            Log& operator<<(long long input);

            //! Adds a double to the log stream.
            Log& operator<<(double input);

            //! Adds a char to the log stream.
            Log& operator<<(char input);

            #if defined(SWARM_INCLUDE_GLM)
            //! Adds a vec2 to the log stream.
            Log& operator<<(glm::vec2 input);
            
            //! Adds a vec3 to the log stream.
            Log& operator<<(glm::vec3 input);
            
            //! Adds a vec4 to the log stream.
            Log& operator<<(glm::vec4 input);

            //! Adds a mat4 to the log stream.
            Log& operator<<(glm::mat4 input);
            #endif


            // ****************
            //  Static Members
            // ****************

            //! Set the default filepath to use for Log objects
            /*!
             * Sets the filepath that Log names will be appended to in order to determine their location.
             */
            static void setDefaultFilepath(const std::string &path);

            //! Cleanup all created Log objects
            /*!
             * Performs a cleanup on all Log objects that have been created. Generally, closes any file pointers that
             * have been opened by a Log object (but not any that were passed in by the developer).
             */
            static void cleanupAll();

        protected:
            std::string _name;
            std::string _filepath;
            FILE* _file = nullptr;
            bool _use_console = false;
            LogSeverity _severity = INFO;

        private:
            LogBuffer* _buffer = nullptr;
        };
    }
}