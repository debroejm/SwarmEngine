#define SWARM_INCLUDE_GLFW
#include "api/Core.h"

#include <set>

namespace Swarm {
    namespace Input {

        KeyType KeyType::K_SPACE        (GLFW_KEY_SPACE);
        KeyType KeyType::K_APOSTROPHE   (GLFW_KEY_APOSTROPHE);
        KeyType KeyType::K_COMMA        (GLFW_KEY_COMMA);
        KeyType KeyType::K_MINUS        (GLFW_KEY_MINUS);
        KeyType KeyType::K_PERIOD       (GLFW_KEY_PERIOD);
        KeyType KeyType::K_SLASH        (GLFW_KEY_SLASH);
        KeyType KeyType::K_0            (GLFW_KEY_0);
        KeyType KeyType::K_1            (GLFW_KEY_1);
        KeyType KeyType::K_2            (GLFW_KEY_2);
        KeyType KeyType::K_3            (GLFW_KEY_3);
        KeyType KeyType::K_4            (GLFW_KEY_4);
        KeyType KeyType::K_5            (GLFW_KEY_5);
        KeyType KeyType::K_6            (GLFW_KEY_6);
        KeyType KeyType::K_7            (GLFW_KEY_7);
        KeyType KeyType::K_8            (GLFW_KEY_8);
        KeyType KeyType::K_9            (GLFW_KEY_9);
        KeyType KeyType::K_SEMICOLON    (GLFW_KEY_SEMICOLON);
        KeyType KeyType::K_EQUAL        (GLFW_KEY_EQUAL);
        KeyType KeyType::K_A            (GLFW_KEY_A);
        KeyType KeyType::K_B            (GLFW_KEY_B);
        KeyType KeyType::K_C            (GLFW_KEY_C);
        KeyType KeyType::K_D            (GLFW_KEY_D);
        KeyType KeyType::K_E            (GLFW_KEY_E);
        KeyType KeyType::K_F            (GLFW_KEY_F);
        KeyType KeyType::K_G            (GLFW_KEY_G);
        KeyType KeyType::K_H            (GLFW_KEY_H);
        KeyType KeyType::K_I            (GLFW_KEY_I);
        KeyType KeyType::K_J            (GLFW_KEY_J);
        KeyType KeyType::K_K            (GLFW_KEY_K);
        KeyType KeyType::K_L            (GLFW_KEY_L);
        KeyType KeyType::K_M            (GLFW_KEY_M);
        KeyType KeyType::K_N            (GLFW_KEY_N);
        KeyType KeyType::K_O            (GLFW_KEY_O);
        KeyType KeyType::K_P            (GLFW_KEY_P);
        KeyType KeyType::K_Q            (GLFW_KEY_Q);
        KeyType KeyType::K_R            (GLFW_KEY_R);
        KeyType KeyType::K_S            (GLFW_KEY_S);
        KeyType KeyType::K_T            (GLFW_KEY_T);
        KeyType KeyType::K_U            (GLFW_KEY_U);
        KeyType KeyType::K_V            (GLFW_KEY_V);
        KeyType KeyType::K_W            (GLFW_KEY_W);
        KeyType KeyType::K_X            (GLFW_KEY_X);
        KeyType KeyType::K_Y            (GLFW_KEY_Y);
        KeyType KeyType::K_Z            (GLFW_KEY_Z);
        KeyType KeyType::K_LEFT_BRACKET (GLFW_KEY_LEFT_BRACKET);
        KeyType KeyType::K_BACKSLASH    (GLFW_KEY_BACKSLASH);
        KeyType KeyType::K_RIGHT_BRACKET(GLFW_KEY_RIGHT_BRACKET);
        KeyType KeyType::K_GRAVE_ACCENT (GLFW_KEY_GRAVE_ACCENT);
        KeyType KeyType::K_WORLD_1      (GLFW_KEY_WORLD_1);
        KeyType KeyType::K_WORLD_2      (GLFW_KEY_WORLD_2);
        KeyType KeyType::K_ESCAPE       (GLFW_KEY_ESCAPE);
        KeyType KeyType::K_ENTER        (GLFW_KEY_ENTER);
        KeyType KeyType::K_TAB          (GLFW_KEY_TAB);
        KeyType KeyType::K_BACKSPACE    (GLFW_KEY_BACKSPACE);
        KeyType KeyType::K_INSERT       (GLFW_KEY_INSERT);
        KeyType KeyType::K_DELETE       (GLFW_KEY_DELETE);
        KeyType KeyType::K_RIGHT        (GLFW_KEY_RIGHT);
        KeyType KeyType::K_LEFT         (GLFW_KEY_LEFT);
        KeyType KeyType::K_DOWN         (GLFW_KEY_DOWN);
        KeyType KeyType::K_UP           (GLFW_KEY_UP);
        KeyType KeyType::K_PAGE_UP      (GLFW_KEY_PAGE_UP);
        KeyType KeyType::K_PAGE_DOWN    (GLFW_KEY_PAGE_DOWN);
        KeyType KeyType::K_HOME         (GLFW_KEY_HOME);
        KeyType KeyType::K_END          (GLFW_KEY_END);
        KeyType KeyType::K_CAPS_LOCK    (GLFW_KEY_CAPS_LOCK);
        KeyType KeyType::K_SCROLL_LOCK  (GLFW_KEY_SCROLL_LOCK);
        KeyType KeyType::K_NUM_LOCK     (GLFW_KEY_NUM_LOCK);
        KeyType KeyType::K_PRINT_SCREEN (GLFW_KEY_PRINT_SCREEN);
        KeyType KeyType::K_PAUSE        (GLFW_KEY_PAUSE);
        KeyType KeyType::K_F1           (GLFW_KEY_F1);
        KeyType KeyType::K_F2           (GLFW_KEY_F2);
        KeyType KeyType::K_F3           (GLFW_KEY_F3);
        KeyType KeyType::K_F4           (GLFW_KEY_F4);
        KeyType KeyType::K_F5           (GLFW_KEY_F5);
        KeyType KeyType::K_F6           (GLFW_KEY_F6);
        KeyType KeyType::K_F7           (GLFW_KEY_F7);
        KeyType KeyType::K_F8           (GLFW_KEY_F8);
        KeyType KeyType::K_F9           (GLFW_KEY_F9);
        KeyType KeyType::K_F10          (GLFW_KEY_F10);
        KeyType KeyType::K_F11          (GLFW_KEY_F11);
        KeyType KeyType::K_F12          (GLFW_KEY_F12);
        KeyType KeyType::K_F13          (GLFW_KEY_F13);
        KeyType KeyType::K_F14          (GLFW_KEY_F14);
        KeyType KeyType::K_F15          (GLFW_KEY_F15);
        KeyType KeyType::K_F16          (GLFW_KEY_F16);
        KeyType KeyType::K_F17          (GLFW_KEY_F17);
        KeyType KeyType::K_F18          (GLFW_KEY_F18);
        KeyType KeyType::K_F19          (GLFW_KEY_F19);
        KeyType KeyType::K_F20          (GLFW_KEY_F20);
        KeyType KeyType::K_F21          (GLFW_KEY_F21);
        KeyType KeyType::K_F22          (GLFW_KEY_F22);
        KeyType KeyType::K_F23          (GLFW_KEY_F23);
        KeyType KeyType::K_F24          (GLFW_KEY_F24);
        KeyType KeyType::K_F25          (GLFW_KEY_F25);
        KeyType KeyType::K_KP_0         (GLFW_KEY_KP_0);
        KeyType KeyType::K_KP_1         (GLFW_KEY_KP_1);
        KeyType KeyType::K_KP_2         (GLFW_KEY_KP_2);
        KeyType KeyType::K_KP_3         (GLFW_KEY_KP_3);
        KeyType KeyType::K_KP_4         (GLFW_KEY_KP_4);
        KeyType KeyType::K_KP_5         (GLFW_KEY_KP_5);
        KeyType KeyType::K_KP_6         (GLFW_KEY_KP_6);
        KeyType KeyType::K_KP_7         (GLFW_KEY_KP_7);
        KeyType KeyType::K_KP_8         (GLFW_KEY_KP_8);
        KeyType KeyType::K_KP_9         (GLFW_KEY_KP_9);
        KeyType KeyType::K_KP_DECIMAL   (GLFW_KEY_KP_DECIMAL);
        KeyType KeyType::K_KP_DIVIDE    (GLFW_KEY_KP_DIVIDE);
        KeyType KeyType::K_KP_MULTIPLY  (GLFW_KEY_KP_MULTIPLY);
        KeyType KeyType::K_KP_SUBTRACT  (GLFW_KEY_KP_SUBTRACT);
        KeyType KeyType::K_KP_ADD       (GLFW_KEY_KP_ADD);
        KeyType KeyType::K_KP_ENTER     (GLFW_KEY_KP_ENTER);
        KeyType KeyType::K_KP_EQUAL     (GLFW_KEY_KP_EQUAL);
        KeyType KeyType::K_LEFT_SHIFT   (GLFW_KEY_LEFT_SHIFT);
        KeyType KeyType::K_LEFT_CONTROL (GLFW_KEY_LEFT_CONTROL);
        KeyType KeyType::K_LEFT_ALT     (GLFW_KEY_LEFT_ALT);
        KeyType KeyType::K_LEFT_SUPER   (GLFW_KEY_LEFT_SUPER);
        KeyType KeyType::K_RIGHT_SHIFT  (GLFW_KEY_RIGHT_SHIFT);
        KeyType KeyType::K_RIGHT_CONTROL(GLFW_KEY_RIGHT_CONTROL);
        KeyType KeyType::K_RIGHT_ALT    (GLFW_KEY_RIGHT_ALT);
        KeyType KeyType::K_RIGHT_SUPER  (GLFW_KEY_RIGHT_SUPER);
        KeyType KeyType::K_MENU         (GLFW_KEY_MENU);

        std::unordered_map<int, bool> _static_key_press_map;

        void setKeyPressed(int key, bool pressed) {
            if(key == GLFW_KEY_UNKNOWN) return;
            _static_key_press_map[key] = pressed;
        }

        bool keyPressed(int key) {
            if(_static_key_press_map.count(key)) return _static_key_press_map.at(key);
            else return false;
        }

        Keybinding::Keybinding() : _key(GLFW_KEY_UNKNOWN), _name("Unknown") {}
        Keybinding::Keybinding(int key, const std::string &name) : _key(key), _name(name) {}

        Keybinding::Keybinding(int key, Config::RawConfigData &configData, const std::string &name)
                : _name(name) {
            _key = configData.getInteger(_name, key);
            configData.setInteger(_name, _key);
        }

        Keybinding::~Keybinding() {
        }

        bool Keybinding::pressed() const {
            return keyPressed(_key);
        }

        void Keybinding::setPressed(bool pressed) const {
            setKeyPressed(_key, pressed);
        }

        std::string Keybinding::keyName() const {
            const char* key_name = glfwGetKeyName(_key, 0);
            if(key_name != nullptr) return std::string(key_name);
            switch(_key) {
                case GLFW_KEY_LEFT_SHIFT:       return "Left Shift";
                case GLFW_KEY_RIGHT_SHIFT:      return "Right Shift";
                case GLFW_KEY_LEFT_ALT:         return "Left Alt";
                case GLFW_KEY_RIGHT_ALT:        return "Right Alt";
                case GLFW_KEY_LEFT_CONTROL:     return "Left Control";
                case GLFW_KEY_RIGHT_CONTROL:    return "Right Control";
                case GLFW_KEY_LEFT_SUPER:       return "Left Super";
                case GLFW_KEY_RIGHT_SUPER:      return "Right Super";
                case GLFW_KEY_SPACE:            return "Space";
                case GLFW_KEY_GRAVE_ACCENT:     return "~";
                case GLFW_KEY_ESCAPE:           return "Escape";
                case GLFW_KEY_ENTER:            return "Enter";
                case GLFW_KEY_TAB:              return "Tab";
                case GLFW_KEY_BACKSPACE:        return "Backspace";
                case GLFW_KEY_INSERT:           return "Insert";
                case GLFW_KEY_DELETE:           return "Delete";
                case GLFW_KEY_RIGHT:            return "Right";
                case GLFW_KEY_LEFT:             return "Left";
                case GLFW_KEY_UP:               return "Up";
                case GLFW_KEY_DOWN:             return "Down";
                case GLFW_KEY_PAGE_UP:          return "Page Up";
                case GLFW_KEY_PAGE_DOWN:        return "Page Down";
                case GLFW_KEY_HOME:             return "Home";
                case GLFW_KEY_END:              return "End";
                case GLFW_KEY_CAPS_LOCK:        return "Caps Lock";
                case GLFW_KEY_SCROLL_LOCK:      return "Scroll Lock";
                case GLFW_KEY_NUM_LOCK:         return "Num Lock";
                case GLFW_KEY_PRINT_SCREEN:     return "Print Screen";
                case GLFW_KEY_PAUSE:            return "Pause";
                case GLFW_KEY_F1:               return "F1";
                case GLFW_KEY_F2:               return "F2";
                case GLFW_KEY_F3:               return "F3";
                case GLFW_KEY_F4:               return "F4";
                case GLFW_KEY_F5:               return "F5";
                case GLFW_KEY_F6:               return "F6";
                case GLFW_KEY_F7:               return "F7";
                case GLFW_KEY_F8:               return "F8";
                case GLFW_KEY_F9:               return "F9";
                case GLFW_KEY_F10:              return "F10";
                case GLFW_KEY_F11:              return "F11";
                case GLFW_KEY_F12:              return "F12";
                case GLFW_KEY_F13:              return "F13";
                case GLFW_KEY_F14:              return "F14";
                case GLFW_KEY_F15:              return "F15";
                case GLFW_KEY_F16:              return "F16";
                case GLFW_KEY_F17:              return "F17";
                case GLFW_KEY_F18:              return "F18";
                case GLFW_KEY_F19:              return "F19";
                case GLFW_KEY_F20:              return "F20";
                case GLFW_KEY_F21:              return "F21";
                case GLFW_KEY_F22:              return "F22";
                case GLFW_KEY_F23:              return "F23";
                case GLFW_KEY_F24:              return "F24";
                case GLFW_KEY_F25:              return "F25";
                case GLFW_KEY_KP_ENTER:         return "KP Enter";
                case GLFW_KEY_MENU:             return "Menu";
                default:                        return "UNKNOWN KEY";
            }
        }

    }
}