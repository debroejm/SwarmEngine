#pragma once



// ***************
//  STD Libraries
// ***************

#include <fstream>
#include <string>
#include <vector>



// ***************
//  Common Header
// ***************

#include "common_header.h"



// ***********
//  API Begin
// ***********

namespace Swarm {

    namespace Logging {

        //! Enum representation of Logging Severity.
        /*!
         * This Enumeration is used to define the severity of a logged message.
         * Depending on settings, some severities will be excluded from being logged.
         */
        enum LogSeverity {
            DEBUG,
            INFO,
            WARNING,
            ERR,        // 'ERROR' is macro'd somewhere in Windows headers
            FATAL
        };

        //! Get a text-based time stamp
        /*!
         * Gets a time stamp string in the format of '[month/day/year-hour:minute:second]'.
         *
         * /return string object containing a time stamp
         */
        std::string timeString();

        //! Get a text prefix for a \ref LogSeverity value
        /*!
         * Takes a given \ref LogSeverity value and returns a prefix in the format of [SEVERITY].
         *
         * \param severity \ref LogSeverity value
         * \return string prefix
         */
        std::string severityPrefix(LogSeverity severity);

        //! An object used to provide logging features
        /*!
         * A Log object is used as an interface to log events into a medium that can be output and read at a later time.
         * All Log objects will output to the standard output stream (std::cout) by default, and Log objects created
         * with a name will also write their output to a file on disk. A Log object can be interfaced with by using the
         * \link Log::operator<<() stream insertion operator \endlink, as well as the \link Log::operator()() function
         * operator \endlink. The former will pipe text to the Log object, while the latter will start a new line with
         * the indicated \ref LogSeverity. Console output can be redirected to another output stream by redirecting the
         * standard output stream (std::cout).
         * \sa Log::operator<<(), Log::setSeverity(), Log::operator()(), LogSeverity
         */
        class Log {
        public:

            //! Create a Console-Only Log
            /*!
             * Creates a Log object that when streamed to, will output its contents only to the standard output stream
             * (std::cout).
             */
            Log();

            //! Create a Log with the given name
            /*!
             * Creates a Log object with a name and attempts to open or create a file to write contents to with said
             * name. File pointers created by constructing a Log object this way are stored internally, and can be
             * cleaned up with the static function Log::cleanupAll(). Optionally, Console logging can be disabled.
             *
             * \param name name to associate with the Log object and to label the associated log file
             * \param console whether or not this Log object should output to the console (std::cout)
             * \sa cleanupAll()
             */
            Log(const std::string &name, bool console = true);

            //! Create a Log with the given name
            /*!
             * Creates a Log object with a name, but associated an already existing file with the Log object instead of
             * opening a new one. File pointers passed in by constructing a Log object this way are still owned by the
             * developer and not the engine, meaning it is still the developers job to clean them up when done.
             * Optionally, Console logging can be disabled.
             *
             * \param name name to associate with the Log object
             * \param file existing file to output Log text to
             * \param console whether or not this Log object should output to the console (std::cout)
             */
            Log(const std::string &name, FILE *file, bool console = true);

            //! Log Copy-Constructor
            /*!
             * Creates a new Log object and sets it equal to another.
             *
             * \param other Log object to set equal to
             */
            Log(const Log &other) { *this = other; }

            //! Log Assignment Operator
            /*!
             * Sets this Log object equal to another.
             *
             * \param other Log object to set equal to
             * \return reference to the current Log object
             */
            Log &operator=(const Log &other);

            //! Sets the currently used Logging Severity
            /*!
             * Sets the Logging Severity so that future log snippets will use the given severity.
             * In addition, automatically calls \ref newline() so that future logging happens as a new entry.
             *
             * \param severity \ref LogSeverity value to use for further logging
             * \sa LogSeverity, newline()
             */
            void setSeverity(LogSeverity severity);

            //! Alias for setSeverity(), with single-line construction enabled
            /*!
             * Calls \ref setSeverity() (and consequently \ref newline()).
             * In addition, returns a reference to itself to be used in conjunction with << operators.
             *
             * \param severity \ref LogSeverity value to use for further logging
             * \sa LogSeverity, setSeverity(), newline()
             */
            Log &operator()(LogSeverity severity);

            //! Starts a new line
            /*!
             * Adds a newline character to the stream, and sets up the header for the next line.
             */
            void newline();

            //! Gets the user-friendly name of this Log object.
            const std::string &getName() { return _name; }

            //! Gets the relative filepath of this Log object.
            const std::string &getFilepath() { return _filepath; }


            // ****************************
            //  Stream Insertion Overloads
            // ****************************

            //! Adds a cstring to the log stream.
            Log &operator<<(const char *input);

            //! Adds a string to the log stream.
            Log &operator<<(std::string input);

            //! Adds an unsigned int to the log stream.
            Log &operator<<(unsigned int input);

            //! Adds a signed int to the log stream.
            Log &operator<<(int input);

            //! Adds an unsigned long to the log stream.
            Log &operator<<(unsigned long input);

            //! Adds a signed long to the log stream.
            Log &operator<<(long input);

            //! Adds an unsigned long long to the log stream.
            Log &operator<<(unsigned long long input);

            //! Adds a signed long long to the log stream.
            Log &operator<<(long long input);

            //! Adds a double to the log stream.
            Log &operator<<(double input);

            //! Adds a char to the log stream.
            Log &operator<<(char input);

            #if defined(SWARM_INCLUDE_GLM)
            //! Adds a vec2 to the log stream.
            Log &operator<<(glm::vec2 input);
            
            //! Adds a vec3 to the log stream.
            Log &operator<<(glm::vec3 input);
            
            //! Adds a vec4 to the log stream.
            Log &operator<<(glm::vec4 input);

            //! Adds a mat4 to the log stream.
            Log &operator<<(glm::mat4 input);
            #endif


            // ****************
            //  Static Members
            // ****************

            static void setDefaultFilepath(const std::string &path);

            //! Cleanup all created Log objects
            /*!
             * Performs a cleanup on all Log objects that have been created. Generally, closes any file pointers that
             * have been opened by a Log object (but not any that were passed in by the developer).
             */
            static void cleanupAll();

            //! Core Log, for the core selection of methods.
            static Log log_core;
            //! Render Log, used to document the render cycle + model loading.
            static Log log_render;
            //! CL Log, used to document CL commands and lifecycle
            static Log log_cl;
            //! Physics Log, used to document the physics cycle.
            static Log log_physics;

        protected:

            std::string _name;
            std::string _filepath;
            FILE *_file = NULL;
            bool _use_console = false;
            LogSeverity _severity = INFO;

            static std::string _static_default_filepath;

        private:
            static std::vector<FILE*> _static_fileptrs;
        };
    }
}