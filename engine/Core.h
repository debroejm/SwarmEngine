#pragma once

// Don't remember what this does... should probably look it up and document it.
#if defined(_WIN32) || defined(__WIN32__)
#if defined(SwarmEngine_EXPORTS)
#define DLLFUNC __declspec(dllexport)
#else
#define DLLFUNC __declspec(dllimport)
#endif
#else
#define DLLFUNC
#endif


// ********************
//  External Libraries
// ********************

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INCLUDE_GLU
//#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define SWM_INIT_MINIMAL            0x0
#define SWM_INIT_MULTITHREAD        0x1
#define SWM_INIT_CL                 0x2
#define SWM_INIT_MODEL              0x4
#define SWM_INIT_RENDER             0x8  | SWM_INIT_MODEL | SWM_INIT_MULTITHREAD
#define SWM_INIT_PHYSICS            0x10 | SWM_INIT_CL

#define SWM_INIT_ALL                SWM_INIT_RENDER | SWM_INIT_PHYSICS


// ************
//  Code Begin
// ************

namespace Swarm {

    namespace Util {

        // TODO: Util Documentation
        glm::vec3 movePoint(glm::vec3 point, glm::vec3 target, float distance);

    }


    namespace Logging {

        //! Enum representation of Logging Severity.
        /*!
         * This Enumeration is used to define the severity of a logged snippet.
         * Depending on settings, some severities will be excluded from being logged.
         */
        enum LogSeverity {
            DEBUG,
            INFO,
            WARNING,
            ERR,        // 'ERROR' is macro'd somewhere in Windows headers
            FATAL
        };

        class Log {
        public:
            Log();      // Console-Only Log
            Log(const char * name, bool console = false);
            Log(const char * name, FILE * file, bool console = false);
            virtual ~Log();

            //! Sets the currently used Logging Severity.
            /*!
             * Sets the Logging Severity so that future log snippets will use the given severity.
             * In addition, automatically calls \ref newline() so that future logging happens as a new entry.
             * \sa newline()
             */
            void setSeverity(LogSeverity severity);

            //! Alias for setSeverity(), with single-line construction enabled.
            /*!
             * Calls \ref setSeverity() (and consequently \ref newline()).
             * In addition, returns a reference to itself to be used in conjunction with << operators.
             * \sa setSeverity(), newline()
             */
            Log &operator()(LogSeverity severity);

            //! Starts a new line.
            /*!
             * Adds a newline character to the stream, and sets up the header for the next line.
             */
            void newline();

            //! Gets the user-friendly name of this Log object.
            const char * getName() { return name; }
            //! Gets the relative filepath of this Log object.
            const char * getFilepath() { return filepath; }

            // Operator Overloads

            //! Adds a cstring to the log stream.
            Log &operator<<(const char * input);
            //! Adds a string to the log stream.
            Log &operator<<(string input);
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
            //! Adds a vec3 to the log stream.
            Log &operator<<(glm::vec3 input);

            //! Sets this Log object equal to another.
            /*!
             * Sets all variables and pointers equal to the other.
             * No need to close and cleanup the file ptr, as it is cleaned up by the static function \ref cleanupAll().
             * \sa cleanupAll()
             */
            Log &operator=(Log &other);

            static void setDefaultFilepath(const char * path);
            static void cleanupAll();

            //! Core Log, for the core selection of methods.
            static Log log_core;
            //! Render Log, used to document the render cycle + model loading.
            static Log log_render;
            //! CL Log, used to document CL commands and lifecycle
            static Log log_cl;

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
    }


    namespace Init {

        bool init(unsigned int flags);
        bool init(string windowName = "Swarm Engine Instance", unsigned int flags = SWM_INIT_ALL);
        bool init(int windowX, int windowY, string windowName = "Swarm Engine Instance", unsigned int flags = SWM_INIT_ALL);

        void cleanup();

    }


    namespace Config {

        class RawConfigData {
        public:
            RawConfigData(const char * filepath);
            ~RawConfigData();

            //! Writes this Raw Configuration Data to a file
            /*!
             * Writes the entire of this Raw Configuration Data to a file.
             * The format is simple, and consists of each key/value pair on a seperate line seperated by a single '='.
             * Writes to the specified filepath.
             */
            void writeConfigData(const char * filepath);

            //! Writes this Raw Configuration Data to a file
            /*!
             * Writes the entire of this Raw Configuration Data to a file.
             * The format is simple, and consists of each key/value pair on a seperate line seperated by a single '='.
             * Writes to the default filepath.
             */
            void writeConfigData() { writeConfigData(filepath); }


            //! Retrieves an Integer from the stored Configuration Data.
            /*!
             * Retrieves an Integer associated with the String key from the internal Integer map. If there is no mapping
             * related to the specified key, it creates a new mapping with the given default value and returns that value.
             */
            int getInteger(string key, int defaultValue = 0);
            //! Sets an Integer to the stored Configuration Data.
            /*!
             * Maps the specified value to the specified key in the internal Integer map.
             */
            void setInteger(string key, int value);

            //~ Retrieves a String from the stored Configuration Data.
            /*!
             * Retrives a String associated with the String key from the internal String map. If there is no mapping
             * related to the specified key, it creates a new mapping with the given default value and returns that value.
             */
            string getString(string key, string defaultValue = "");
            //! Sets a String to the stored Configuration Data.
            /*!
             * Maps the specified value to the specified key in the internal String map.
             */
            void setString(string key, string value);
        protected:
            const char * filepath;

            unordered_map<string, int> dataMap_int;
            unordered_map<string, string> dataMap_str;
        };

    }

    namespace Input {

        void computeMatricesFromInputs();
        glm::mat4 getViewMatrix();
        glm::mat4 getProjectionMatrix();
        void setWindow(GLFWwindow* windowIN);
        GLFWwindow* getWindow();

        class Keybinding {
        public:
            Keybinding(int key, string name = "Unnamed");
            Keybinding(int key, Config::RawConfigData &configData, string name = "Unnamed");
            ~Keybinding();

            //! Get the pressed status of this Keybinding.
            bool isPressed();
            operator bool() { return isPressed(); }
            //! Set the pressed status of this Keybinding.
            void setPressed(bool pressed);

            //! Get this Keybinding's assinged name.
            string getName() { return name; }
            //! Get this Keybinding's key code.
            int getKey() { return key; }
        protected:
            int key;
            bool pressed;
            string name;
        };

        void setPressed(int key, bool pressed);
        void setPressed(string name, bool pressed);
        bool isPressed(int key);
        bool isPressed(string name);

        Keybinding* getKeybinding(int key);
        Keybinding* getKeybinding(string name);
        void addKeybinding(Keybinding &binding);

    }

}