#include "Configuration.h"

using namespace ENGINE_NAMESPACE::ENGINE_NAMESPACE_LOG;

namespace ENGINE_NAMESPACE {
    namespace ENGINE_NAMESPACE_CONFIG {

        unordered_map<int, Keybinding*> keybindingRegistry_int;
        unordered_map<string, Keybinding*> keybindingRegistry_str;

        void setPressed(int key, bool pressed) {
            unordered_map<int, Keybinding*>::iterator iInt = keybindingRegistry_int.find(key);
            if(iInt != keybindingRegistry_int.end()) {
                iInt->second->setPressed(pressed);
                unordered_map<string, Keybinding*>::iterator iStr = keybindingRegistry_str.find(iInt->second->getName());
                if(iStr != keybindingRegistry_str.end()) {
                    iStr->second->setPressed(pressed);
                } else { // Should exist, so make it exist
                    keybindingRegistry_str[iInt->second->getName()] = iInt->second;
                }
            }
        }

        void setPressed(string name, bool pressed) {
            unordered_map<string, Keybinding*>::iterator iStr = keybindingRegistry_str.find(name);
            if(iStr != keybindingRegistry_str.end()) {
                iStr->second->setPressed(pressed);
                unordered_map<int, Keybinding*>::iterator iInt = keybindingRegistry_int.find(iStr->second->getKey());
                if(iInt != keybindingRegistry_int.end()) {
                    iInt->second->setPressed(pressed);
                } else { // Should exist, so make it exist
                    keybindingRegistry_int[iStr->second->getKey()] = iStr->second;
                }
            }
        }

        bool isPressed(int key) {
            unordered_map<int, Keybinding*>::iterator i = keybindingRegistry_int.find(key);
            if(i != keybindingRegistry_int.end()) return i->second->isPressed();
            else return false;
        }

        bool isPressed(string name) {
            unordered_map<string, Keybinding*>::iterator i = keybindingRegistry_str.find(name);
            if(i != keybindingRegistry_str.end()) return i->second->isPressed();
            else return false;
        }

        Keybinding* getKeybinding(int key) {
            unordered_map<int, Keybinding*>::iterator i = keybindingRegistry_int.find(key);
            if(i != keybindingRegistry_int.end()) return i->second;
            else return NULL;
        }

        Keybinding* getKeybinding(string name) {
            unordered_map<string, Keybinding*>::iterator i = keybindingRegistry_str.find(name);
            if(i != keybindingRegistry_str.end()) return i->second;
            else return NULL;
        }

        void addKeybinding(Keybinding &binding) {
            keybindingRegistry_int[binding.getKey()] = &binding;
            keybindingRegistry_str[binding.getName()] = &binding;
        }

        RawConfigData::RawConfigData(const char * filepath) {
            this->filepath = filepath;
            ifstream dataStream(filepath, ios::in);
            if(dataStream.is_open()) {
                string line = "";
                while(getline(dataStream, line)) {
                    size_t splitIndex = line.find_first_of(":=");
                    if(splitIndex == string::npos) continue;
                    string key = line.substr(0, splitIndex);
                    string value = line.substr(splitIndex+1, line.size()-splitIndex-1);
                    int testInt;
                    if(stringstream(value)>>testInt) {
                        dataMap_int[key] = testInt;
                    } else {
                        dataMap_str[key] = value;
                    }
                }
                dataStream.close();
            } else {
                char errorMsg[256];
                sprintf(errorMsg, "Failed to open '%s'", filepath);
                Log(LOGGING_WARNING, "Configuration", errorMsg);
                return;
            }
        }

        RawConfigData::~RawConfigData() {

        }

        void RawConfigData::writeConfigData() { writeConfigData(filepath); }
        void RawConfigData::writeConfigData(const char * filepath) {
            ofstream outputStream;
            outputStream.open(filepath, ios::out | ios::trunc);

            unordered_map<string, int>::iterator iInt = dataMap_int.begin();
            while(iInt != dataMap_int.end()) {
                outputStream << iInt->first << "=" << iInt->second << "\n";
                iInt++;
            }

            unordered_map<string, string>::iterator iStr = dataMap_str.begin();
            while(iStr != dataMap_str.end()) {
                outputStream << iStr->first << "=" << iStr->second << "\n";
                iStr++;
            }

            outputStream.close();
        }

        int RawConfigData::getInteger(string key) { return getInteger(key, 0); }
        int RawConfigData::getInteger(string key, int defaultValue) {
            unordered_map<string, int>::iterator i = dataMap_int.find(key);
            if(i == dataMap_int.end()) return defaultValue;
            else return i->second;
        }
        void RawConfigData::setInteger(string key, int value) { dataMap_int[key] = value; }

        string RawConfigData::getString(string key) { return getString(key, ""); }
        string RawConfigData::getString(string key, string defaultValue) {
            unordered_map<string, string>::iterator i = dataMap_str.find(key);
            if(i == dataMap_str.end()) return defaultValue;
            else return i->second;
        }
        void RawConfigData::setString(string key, string value) { dataMap_str[key] = value; }



        Keybinding::Keybinding(int key) {
            this->key = key;
            this->name = "Unnamned";
            pressed = false;
        }

        Keybinding::Keybinding(string name, int key) {
            this->key = key;
            this->name = name;
            pressed = false;
        }

        Keybinding::Keybinding(string name, int key, RawConfigData &configData) {
            this->key = configData.getInteger(name, key);
            configData.setInteger(name, this->key);
            this->name = name;
            pressed = false;
        }

        Keybinding::~Keybinding() {

        }

        bool Keybinding::isPressed() { return pressed; }
        void Keybinding::setPressed(bool pressed) { this->pressed = pressed; }


    }
}