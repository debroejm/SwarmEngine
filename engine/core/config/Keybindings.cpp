#include "../../Core.h"

namespace Swarm {
    namespace Input {

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

        Keybinding::Keybinding(int key, string name) {
            this->key = key;
            this->name = name;
            pressed = false;
        }

        Keybinding::Keybinding(int key, Config::RawConfigData &configData, string name) {
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