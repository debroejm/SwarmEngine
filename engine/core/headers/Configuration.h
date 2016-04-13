#ifndef SWARMENGINE_CONFIGURATION_H
#define SWARMENGINE_CONFIGURATION_H

// External Libraries
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw/glfw3.h>

// Internal Libraries
#include "Logging.h"

#include "../Definitions.h"


namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_CONFIG {

        #pragma lhgMultiOn(SwarmEngine, ENGINE_NAMESPACE::ENGINE_NAMESPACE_CONFIG)
        class RawConfigData {
        public:
            RawConfigData(const char * filepath);
            ~RawConfigData();

            void writeConfigData();
            void writeConfigData(const char * filepath);

            int getInteger(string key);
            int getInteger(string key, int defaultValue);
            void setInteger(string key, int value);

            string getString(string key);
            string getString(string key, string defaultValue);
            void setString(string key, string value);
        protected:
            const char * filepath;

            unordered_map<string, int> dataMap_int;
            unordered_map<string, string> dataMap_str;
        };

        class Keybinding {
        public:
            Keybinding(int key);
            Keybinding(string name, int key);
            Keybinding(string name, int key, RawConfigData &configData);
            ~Keybinding();

            bool isPressed();
            void setPressed(bool pressed);

            string getName() { return name; }
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
        #pragma lhgMultiOff()

    }
}


#endif //SWARMENGINE_CONFIGURATION_H
